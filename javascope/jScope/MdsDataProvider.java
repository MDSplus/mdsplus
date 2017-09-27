package jScope;

import jScope.ConnectionEvent;
import jScope.ConnectionListener;
import java.io.*;
import java.net.*;
import java.awt.*;
import java.awt.image.*;
import java.util.*;
import java.lang.OutOfMemoryError;
import java.lang.InterruptedException;
import javax.imageio.*;
import javax.swing.*;
import java.text.*;
import java.util.logging.Level;
import java.util.logging.Logger;

public class MdsDataProvider
    implements DataProvider
{
    protected String provider;
    protected String experiment;
    String default_node;
    String environment_vars;
    private boolean def_node_changed = false;
    protected long shot;
    boolean open, connected;
    MdsConnection mds;
    String error;
    private boolean use_compression = false;
    int var_idx = 0;

    boolean is_tunneling = false;
    String tunnel_provider = "127.0.0.1:8000";
    SshTunneling ssh_tunneling;
    static final long RESAMPLE_TRESHOLD = 1000000000;
    static final int MAX_PIXELS = 20000;
    
    
    class SegmentedFrameData
        implements FrameData
    {
        String inX, inY;
        float timeMax, timeMin;
        int framesPerSegment;
        int numSegments;
        int startSegment, endSegment, actSegments;
        int mode;
        Dimension dim;
        float times[];
        int bytesPerPixel;

        public SegmentedFrameData(String inY, String inX, float timeMin, float timeMax, int numSegments) throws IOException
        {
            //Find out frames per segment and frame min and max based on time min and time max
            this.inX = inX;
            this.inY = inY;
            this.timeMin = timeMin;
            this.timeMax = timeMax;
            this.numSegments = numSegments;
            startSegment = -1;
            float startTimes[] = new float[numSegments];
 //Get segment window corresponding to the passed time window
            for(int i = 0; i < numSegments; i++)
            {
                float limits[] = GetFloatArray("GetSegmentLimits("+inY+","+i+")");
                startTimes[i] = limits[0];
                if(limits[1] > timeMin)
                {
                    startSegment = i;
                    break;
                }
            }
            if(startSegment == -1)
                throw new IOException("Frames outside defined time window");
//Check first if endTime is greated than the end of the last segment, to avoid rolling over all segments
            float endLimits[] = GetFloatArray("GetSegmentLimits("+inY+","+(numSegments - 1)+")");
//Throw away spurious frames at the end
            while(endLimits == null || endLimits.length != 2)
            {
                numSegments--;
                if(numSegments == 0)
                    break;
                endLimits = GetFloatArray("GetSegmentLimits("+inY+","+(numSegments - 1)+")");
            }
            if(numSegments > 100 && endLimits[0] < timeMax)
            {
                endSegment = numSegments - 1;
                for(int i = startSegment; i < numSegments; i++)
                      startTimes[i] = startTimes[0] + i*(endLimits[0] - startTimes[0])/numSegments;
            }
            else
            {
                for(endSegment = startSegment; endSegment < numSegments; endSegment++)
                {
                    try {
                        float limits[] = GetFloatArray("GetSegmentLimits("+inY+","+endSegment+")");
                        startTimes[endSegment] = limits[0];
                        if(limits[0] > timeMax)
                            break;
                    }catch(Exception exc){break;}
                }
            }

            actSegments = endSegment - startSegment;
//Get Frame Dimension and frames per segment
            int dims[] = GetIntArray("shape(GetSegment("+inY+", 0))");
            if(dims.length != 3 && dims.length != 1)  //The former refers to usual raster image, the latter to AWT images (jpg, gif...)
                throw new IOException("Invalid number of segment dimensions: "+ dims.length);
            if(dims.length == 3)
            {
                dim = new Dimension(dims[0], dims[1]);
                framesPerSegment = dims[2];
//Get Frame element length in bytes
                int len[] = GetIntArray("len(GetSegment("+inY+", 0))");
                bytesPerPixel = len[0];
                switch (len[0])
                {
                    case 1:
                        mode = BITMAP_IMAGE_8;
                        break;
                    case 2:
                        mode = BITMAP_IMAGE_16;
                        break;
                    case 4:
                        mode = BITMAP_IMAGE_32;
                        break;
                    default:
                        throw new IOException("Unexpected length for frame data: "+ len[0]);
                }
            }
            else // The degment contains a 1D char buffer, i.e. the binary format of the image (jpg,gif, ...O)
            {
                framesPerSegment = 1;
                mode = AWT_IMAGE;
                bytesPerPixel = 1;
                byte[] firstSegment = GetByteArray("GetSegment("+ inY+",0)");
                BufferedImage img = ImageIO.read(new ByteArrayInputStream(firstSegment));
                dim = new Dimension(img.getWidth(), img.getHeight());
            }
//Get Frame times
             if(framesPerSegment == 1) //We assume in this case that start time is the same of the frame time
             {
                 times = new float[actSegments];
                 for(int i = 0; i < actSegments; i++)
                     times[i] = startTimes[startSegment + i];
             }
             else //Get segment times. We assume that the same number of frames is contained in every segment
             {
                times = new float[actSegments * framesPerSegment];
                for(int i = 0; i < actSegments; i++)
                {
                    float segTimes [] = GetFloatArray("dim_of(GetSegment("+inY+","+i+"))");
                    if(segTimes.length != framesPerSegment)
                        throw new IOException("Inconsistent definition of time in frame + "+i+": read "+ segTimes.length+
                                " times, expected "+ framesPerSegment );
                    for(int j = 0; j < framesPerSegment; j++)
                        times[i * framesPerSegment + j] = segTimes[j];
                 }
             }
        }
        public int GetFrameType() throws IOException
        {
            return mode;
        }

        public int GetNumFrames()
        {
            return actSegments * framesPerSegment;
        }

        public Dimension GetFrameDimension()
        {
            return dim;
        }

        public float[] GetFrameTimes()
        {
            return times;
        }
        public byte[] GetFrameAt(int idx) throws IOException
        {
            //System.out.println("GET FRAME AT " + idx);
            int segmentIdx = startSegment + idx / framesPerSegment;
            int segmentOffset = (idx % framesPerSegment) * dim.width * dim.height * bytesPerPixel;
            byte[] segment = GetByteArray("GetSegment("+ inY+","+segmentIdx+")");
            if(framesPerSegment == 1)
                return segment;
            byte []outFrame = new byte[dim.width * dim.height * bytesPerPixel];
            System.arraycopy(segment, segmentOffset, outFrame, 0, dim.width * dim.height * bytesPerPixel);
            return outFrame;
        }
    }


    class SimpleFrameData
        implements FrameData
    {
        String in_x, in_y;
        float time_max, time_min;
        int mode = -1;
        int pixel_size;
        int first_frame_idx = -1;
        byte buf[];
        String error;
        private int st_idx = -1, end_idx = -1;
        private int n_frames = 0;
        private float times[] = null;
        private long  long_times[] = null; 
        private Dimension dim = null;
        private int header_size = 0;

        public SimpleFrameData(String in_y, String in_x, float time_min,
                               float time_max) throws Exception
        {
            int i;
            float t;
            float all_times[] = null;
            int n_all_frames = 0;

            this.in_y = in_y;
            this.in_x = in_x;
            this.time_min = time_min;
            this.time_max = time_max;
            buf = GetAllFrames(in_y);
            if (buf != null)
            {
                ByteArrayInputStream b = new ByteArrayInputStream(buf);
                DataInputStream d = new DataInputStream(b);

                pixel_size = d.readInt();
                int width = d.readInt();
                int height = d.readInt();
                int n_frame = d.readInt();

                dim = new Dimension(width, height);
                if (in_x == null || in_x.length() == 0)
                {
                    all_times = new float[n_frame];                
                    for (i = 0; i < n_frame; i++)
                    {
                        all_times[i] = d.readFloat();
                    }
                }
                else
                {
                  //all_times = MdsDataProvider.this.GetWaveData(in_x).GetFloatData();
                    all_times = MdsDataProvider.this.GetWaveData(in_x).getData(MAX_PIXELS).y;
                }
                
                header_size = 16 + 4 * n_frame;

                switch (pixel_size)
                {
                    case 8:
                        mode = BITMAP_IMAGE_8;
                        break;
                    case 16:
                        mode = BITMAP_IMAGE_16;
                        break;
                    case 32:
                        mode = BITMAP_IMAGE_32;
                        break;
                }
            }
            else
            {
                String mframe_error = ErrorString();

                if (in_x == null || in_x.length() == 0)
                    all_times = MdsDataProvider.this.GetFrameTimes(in_y);
                else
                  //all_times = MdsDataProvider.this.GetWaveData(in_x).GetFloatData();
                    all_times = MdsDataProvider.this.GetWaveData(in_x).getData(MAX_PIXELS).y;

                if (all_times == null)
                {
                    if (mframe_error != null)
                        error =
                            " Pulse file or image file not found\nRead pulse file error\n" +
                            mframe_error + "\nFrame times read error";
                    else
                        error = " Image file not found ";

                    if (ErrorString() != null)
                        error = error + "\n" + ErrorString();

                    throw (new IOException(error));
                }
            }

            for (i = 0; i < all_times.length; i++)
            {
                t = all_times[i];
                if (t > time_max)
                    break;
                if (t >= time_min)
                {
                    if (st_idx == -1)
                        st_idx = i;
                }
            }
            end_idx = i;

            if (st_idx == -1)
                throw (new IOException("No frames found between " + time_min +
                                       " - " + time_max));

            n_frames = end_idx - st_idx;
            times = new float[n_frames];
            int j = 0;
            for (i = st_idx; i < end_idx; i++)
                times[j++] = all_times[i];
        }

        public int GetFrameType() throws IOException
        {
            if (mode != -1)
                return mode;
            int i;
            for (i = 0; i < n_frames; i++)
            {
                buf = GetFrameAt(i);
                if (buf != null)
                    break;
            }
            first_frame_idx = i;
            mode = Frames.DecodeImageType(buf);
            return mode;
        }

        public int GetNumFrames()
        {
            return n_frames;
        }

        public Dimension GetFrameDimension()
        {
            return dim;
        }

        public float[] GetFrameTimes()
        {
            return times;
        }

        public byte[] GetFrameAt(int idx) throws IOException
        {
            byte[] b_img = null;

            if (mode == BITMAP_IMAGE_8 || mode == BITMAP_IMAGE_16 ||
                mode == BITMAP_IMAGE_32)
            {
                if (buf == null)
                    throw (new IOException("Frames not loaded"));

                ByteArrayInputStream b = new ByteArrayInputStream(buf);
                DataInputStream d = new DataInputStream(b);

                
                if (buf == null)
                    throw (new IOException("Frames dimension not evaluated"));

                int img_size = dim.width * dim.height * pixel_size / 8;
                
                d.skip(header_size + (st_idx + idx) * img_size);

                if( d.available() < img_size )
                    return null;
                

                b_img = new byte[img_size];
                d.readFully(b_img);
                return b_img;
            }
            else
            {
                //we = new WaveformEvent(wave, "Loading frame "+idx+"/"+n_frames);
                //wave.dispatchWaveformEvent(we);
                if (idx == first_frame_idx && buf != null)
                    return buf;
                b_img = MdsDataProvider.this.GetFrameAt(in_y, st_idx + idx);
                return b_img;
            }
        }
    }

    
    ////////////////////////////////////////GAB JULY 2014
     
    class SimpleWaveData
        implements WaveData
    {
        String in_x, in_y;
        boolean _jscope_set = false;
        static final int SEGMENTED_YES = 1, SEGMENTED_NO = 2, SEGMENTED_UNKNOWN = 3;
        static final int UNKNOWN = -1;
        int numDimensions = UNKNOWN;
        int segmentMode = SEGMENTED_UNKNOWN;
        int v_idx;
        boolean isXLong = false;
        String title = null;
        String xLabel = null;
        String yLabel = null;
        boolean titleEvaluated = false;
        boolean xLabelEvaluated = false;
        boolean yLabelEvaluated = false;
        String wd_experiment;
        long wd_shot;
        AsynchDataSource asynchSource = null;
    

        public SimpleWaveData(String in_y, String experiment, long shot)
        {
            this.wd_experiment = experiment;
            this.wd_shot = shot;
            if(checkForAsynchRequest(in_y))
            {
               this.in_y = "[]";
               this.in_x = "[]";
            }
            else
            {
                this.in_y = in_y;
            }
            v_idx = var_idx;
            var_idx+=2;
            if(segmentMode == SEGMENTED_UNKNOWN)
            {
                Vector<Descriptor> args = new Vector<>();
                String fixedY = in_y.replaceAll("\\\\", "\\\\\\\\");
                args.addElement(new Descriptor(null, fixedY));
                try {
                    byte[] retData = GetByteArray("byte(MdsMisc->IsSegmented($))", args);                              
                    if(retData[0] > 0)
                        segmentMode = SEGMENTED_YES;
                    else
                        segmentMode = SEGMENTED_NO;

 /*                   
 //                   int[] numSegments = GetIntArray("GetNumSegments("+in_y+")");
 //                   if(numSegments[0] > 0)
                        segmentMode = SEGMENTED_YES;
                    else
                        segmentMode = SEGMENTED_NO;
 */               }
                catch(Exception exc)
                {
                    error = null;
                    segmentMode = SEGMENTED_UNKNOWN;
                }
            }
         }

        public SimpleWaveData(String in_y, String in_x, String experiment, long shot)
        {
            this.wd_experiment = experiment;
            this.wd_shot = shot;
            if(checkForAsynchRequest(in_y))
            {
                this.in_y = "[]";
                this.in_x = "[]";
             }
            else
            {
                this.in_y = in_y;
                this.in_x = in_x;
            }
           v_idx = var_idx;
            var_idx += 2;
            if(segmentMode == SEGMENTED_UNKNOWN)
            {
                Vector<Descriptor> args = new Vector<>();
                String fixedY = in_y.replaceAll("\\\\", "\\\\\\\\");
                args.addElement(new Descriptor(null, fixedY));
                try {
                    byte[] retData = GetByteArray("byte(MdsMisc->IsSegmented($))", args);                              
                    if(retData[0] > 0)
                        segmentMode = SEGMENTED_YES;
                    else
                        segmentMode = SEGMENTED_NO;
/*                    int[] numSegments = GetIntArray("GetNumSegments("+in_y+")");
                    if(numSegments[0] > 0)
                        segmentMode = SEGMENTED_YES;
                    else
                        segmentMode = SEGMENTED_NO;
*/                }catch(Exception exc)
                {
                    error = null;
                    segmentMode = SEGMENTED_UNKNOWN;
                }
            }
        }

        //Check if the passed Y expression specifies also an asynchronous part (separated by the patern &&&)
        //in case get an implemenation of AsynchDataSource
        boolean checkForAsynchRequest(String expression)
        {
            if(expression.startsWith("ASYNCH::"))
            {
                 asynchSource = getAsynchSource();
                if(asynchSource != null)
                {
                    asynchSource.startGeneration(expression.substring("ASYNCH::".length()));
                }
                return true;
             }
            return false;
        }
        
        
        
        

        public int getNumDimension() throws IOException
        {
            if(numDimensions != UNKNOWN)
                return numDimensions;
            String expr;
            if (_jscope_set)
                expr = "shape(_jscope_" + v_idx + ")";
            else
            {
                if(segmentMode == SEGMENTED_YES)
                    expr = "shape(GetSegment(" + in_y +",0))";
                else
                {
                    _jscope_set = true;
                    expr = "( _jscope_" + v_idx + " = (" + in_y +";), shape(_jscope_" + v_idx + "))";
                }
            }
            error = null;
            int shape[] = GetNumDimensions(expr);

            if (error != null || shape == null)
            {
                _jscope_set = false;
                error = null;
                return 1;
            }
            numDimensions = shape.length;
            return shape.length;
        }

        public String GetTitle() throws IOException
        {
            String expr;
            if(!titleEvaluated)
            {
                titleEvaluated = true;
                if (_jscope_set)
               {
                   expr = "help_of(_jscope_" + v_idx + ")";
                   title = GetStringValue(expr);
               }
               else
               {
                   if(segmentMode == SEGMENTED_YES)
                   {
                       expr = "help_of(" + in_y + ")";
                   }
                   else
                   {
                       _jscope_set = true;
                       expr = "( _jscope_" + v_idx + " = (" + in_y +
                           "), help_of(_jscope_" + v_idx + "))";
                   }
                    title = GetStringValue(expr);
              }
            }
            return title;
        }

        public String GetXLabel() throws IOException
        {
 
            if(!xLabelEvaluated)
            {
                xLabelEvaluated = true;
                if (in_x == null || in_x.length() == 0)
                {
                    String expr;
                    if (_jscope_set)
                    {
                        expr = "Units(dim_of(_jscope_" + v_idx + "))";
                        xLabel = GetStringValue(expr);
                    }
                    else
                    {
                        if(segmentMode == SEGMENTED_YES)
                        {
                            expr = "Units(dim_of(GetSegment(" + in_y + ", 0)))";
                            xLabel = GetStringValue(expr);
                       }
                        else
                        {
                            _jscope_set = true;
                            expr = "( _jscope_" + v_idx + " = (" + in_y + "), Units(dim_of(_jscope_" + v_idx + ")))";
                            xLabel = GetStringValue(expr);
                        }
                    }
                }
                else
                {
                    xLabel = GetStringValue("Units(" + in_x + ")");
                }
            }
            return xLabel;
        }

        
        public String GetYLabel() throws IOException
        {
                        
            String expr;
 
            if(!yLabelEvaluated)
            {
                yLabelEvaluated = true;
                if( getNumDimension() > 1)
                {
                    if(segmentMode == SEGMENTED_YES)
                    {
                        expr = "Units(dim_of(GetSegment(" + in_y + ", 1)))";
                        yLabel = GetStringValue(expr);
                     }
                    else
                    {
                        if (_jscope_set)
                        {
                            expr = "Units(dim_of(_jscope_" + v_idx + ", 1))";
                            yLabel = GetStringValue(expr);
                        }
                        else
                        {
                            _jscope_set = true;
                            expr = "( _jscope_" + v_idx + " = (" + in_y +
                            "), Units(dim_of(_jscope_" + v_idx + ", 1)))";
                            yLabel = GetStringValue(expr);
                        }
                    }
                }
                return yLabel;
            }
            if(segmentMode == SEGMENTED_YES)
            {
                expr = "Units(dim_of(GetSegment(" + in_y + ", 0)))";
                yLabel = GetStringValue(expr);
            }
            else
            {
                if (_jscope_set)
                {
                    expr = "Units(_jscope_" + v_idx + ")";
                    yLabel = GetStringValue(expr);
                }
                else
                {
                    _jscope_set = true;
                    expr = "( _jscope_" + v_idx + " = (" + in_y +
                        "), Units(_jscope_" + v_idx + "))";
                     yLabel = GetStringValue(expr);
                }
            }
            return yLabel;
        }

        public String GetZLabel() throws IOException
        {            
            String expr;
                        
            if (_jscope_set)
                expr = "Units(_jscope_" + v_idx + ")";
            else
            {
                _jscope_set = true;
                expr = "( _jscope_" + v_idx + " = (" + in_y +
                    "), Units(_jscope_" + v_idx + "))";
            }
            String out = GetStringValue(expr);
            if (out == null)
                _jscope_set = false;

            return out;
        }
        
        //GAB JULY 2014 NEW WAVEDATA INTERFACE RAFFAZZONATA
        public XYData getData(double xmin, double xmax, int numPoints) throws Exception
        {
            return getData(xmin, xmax, numPoints, false);
        }
        public XYData getData(long xmin, long xmax, int numPoints) throws Exception
        {
            return getData((double)xmin, (double)xmax, numPoints, true);
        }
        public XYData getData(double xmin, double xmax, int numPoints, boolean isLong) throws Exception
        {
             String xExpr, yExpr;
             XYData res = null;
             double maxX = 0;
             
             if (!CheckOpen(this.wd_experiment, this.wd_shot))
                return null;

             if(segmentMode == SEGMENTED_UNKNOWN)
             {
                Vector<Descriptor> args = new Vector<>();
                args.addElement(new Descriptor(null, in_y));
                try {                 
                    byte[] retData = GetByteArray("byte(MdsMisc->IsSegmented($))", args);                              
                    if(retData[0] > 0)
                        segmentMode = SEGMENTED_YES;
                    else
                        segmentMode = SEGMENTED_NO;
                }catch(Exception exc)
                {
                    segmentMode = SEGMENTED_NO;
                }
            }
             
            if(segmentMode == SEGMENTED_NO) //Store in TDI variable only non segmented data
            {
                yExpr =  in_y;
                _jscope_set = true;
                if(in_x == null)
                    xExpr = "__jScope_var = ("+in_y+") ; DIM_OF( __jScope_var );";
                else
                    xExpr = in_x;
            }
            else
            {                 
                if(in_x == null)
                {
                    yExpr =  in_y;
                    xExpr = "__jScope_var = (" +in_y+") ; DIM_OF(__jScope_var);";
                }
                else
                {
                    yExpr =  in_y;
                    xExpr = in_x;
                }
            }            
            
                Vector<Descriptor> args = new Vector<>();
                args.addElement(new Descriptor(null, yExpr));
                if(in_x == null)
                    args.addElement(new Descriptor(null, ""));
                else
                    args.addElement(new Descriptor(null, xExpr));
                
                if(isLong)
                {
                    args.addElement(new Descriptor(null, new long[]{(xmin == -Double.MAX_VALUE)?0:(long)xmin}));
                    args.addElement(new Descriptor(null, new long[]{(xmax == Double.MAX_VALUE)?0:(long)xmax}));
                }
                else
                {
                args.addElement(new Descriptor(null, new float[]{(float)xmin}));
                args.addElement(new Descriptor(null, new float[]{(float)xmax}));
                }
                args.addElement(new Descriptor(null, new int[]{numPoints}));
                byte[] retData;
                int nSamples;
                try {
           //If the requeated number of points is Integer.MAX_VALUE, force the old way of getting data
                    if(numPoints == Integer.MAX_VALUE)
                        throw new Exception("Use Old Method for getting data");
                    if(isLong)
//                      retData = GetByteArray(setTimeContext+" MdsMisc->GetXYSignalLongTimes:DSC", args);
                        retData = GetByteArray(" MdsMisc->GetXYSignalLongTimes:DSC", args);
                    else
//                      retData = GetByteArray(setTimeContext+" MdsMisc->GetXYSignal:DSC", args);
                        retData = GetByteArray(" MdsMisc->GetXYSignal:DSC", args);
                    
                /*Decode data: Format:
                       -retResolution(float)
                       -number of samples (minumum between X and Y)
                       -type of X xamples (byte: long(1), double(2) or float(3))
                       -y samples 
                       -x Samples 
               */
                    ByteArrayInputStream bis = new ByteArrayInputStream(retData);
                    DataInputStream dis = new DataInputStream(bis);
                    float fRes;
                    double dRes;
                    fRes = dis.readFloat();
                    if(fRes >= 1E10)
                        dRes = Double.MAX_VALUE;
                    else
                        dRes = fRes;
                    nSamples = dis.readInt();
                    if( nSamples <= 0 )
                    {
                        error = "No Samples returned";
                        return null;
                    }
                    
                    byte type = dis.readByte();
                    float y[] = new float[nSamples];
                    for(int i = 0; i < nSamples; i++)
                    {
                        y[i] = dis.readFloat();
                    }
                    if(type == 1) //Long X (i.e. absolute times
                    {
                        long []longX = new long[nSamples];
                        for(int i = 0; i < nSamples; i++)
                            longX[i] = dis.readLong();
                        isXLong = true;
                        res = new XYData(longX, y, dRes);
                        if(longX.length > 0)
                            maxX = longX[longX.length - 1];
                        else 
                            maxX = 0;
                   }
                    else if(type == 2) //double X
                    {
                        double []x = new double[nSamples];
                        for(int i = 0; i < nSamples; i++)
                            x[i] = dis.readDouble();
                        res = new XYData(x, y, dRes);
                        if(x.length > 0)
                            maxX = x[x.length - 1];
                        else 
                            maxX = 0;
                    }
                    else //float X
                    {
                        double []x = new double[nSamples];
                        for(int i = 0; i < nSamples; i++)
                            x[i] = dis.readFloat();
                        res = new XYData(x, y, dRes);
                        if(x.length > 0)
                            maxX = x[x.length - 1];
                        else 
                            maxX = 0;
                   }
                    //Get title, xLabel and yLabel
                   int titleLen = dis.readInt();
                    if(titleLen > 0)
                    {
                        byte []titleBuf = new byte[titleLen];
                        dis.readFully(titleBuf);
                        title = new String(titleBuf);
                    }
                    int xLabelLen = dis.readInt();
                    if(xLabelLen > 0)
                    {
                        byte []xLabelBuf = new byte[xLabelLen];
                        dis.readFully(xLabelBuf);
                        xLabel = new String(xLabelBuf);
                    }
                
                    int yLabelLen = dis.readInt();
                    if(yLabelLen > 0)
                    {
                        byte []yLabelBuf = new byte[yLabelLen];
                        dis.readFully(yLabelBuf);
                        yLabel = new String(yLabelBuf);
                    }
                    titleEvaluated = xLabelEvaluated = yLabelEvaluated = true;
                    if(type == 1)
                        isLong = true;
/*                }
                catch(Exception exc)
                {
                    System.out.println("Error Reading data: "+exc);
                    nSamples = 0;
                }
 */               //Got resampled signal, if it is segmented and jScope.refreshPeriod > 0, enqueue a new request
                return res;
             }catch(Exception exc)
             {
                 //System.out.println("MdsMisc->GetXYSignal Failed: "+exc);  It means that MdsMisc->GetXYSignal() is likely not available on the server
             }
 //If execution arrives here probably MdsMisc->GetXYSignal() is not available on the server, so use the traditional approach
//            float y[] = GetFloatArray("SetTimeContext(*,*,*); ("+yExpr+");");
                
            
            float y[] = GetFloatArray("("+yExpr+")");
            RealArray xReal = GetRealArray("("+xExpr+";)");
            if(y == null || xReal == null) return null;
            if(xReal.isLong())
            {
                isXLong = true;
                return new XYData(xReal.getLongArray(), y, 1E12);
            }
            else
            {
                isXLong = false;
                return new XYData(xReal.getDoubleArray(), y, 1E12);
            }
         
         }
         public XYData getData(int numPoints)throws Exception
         {
             return getData(-Double.MAX_VALUE, Double.MAX_VALUE, numPoints);
         }

        public float[] getZ()
        {
            try {
                return GetFloatArray(in_y);
            }catch(Exception exc){return null;}
        }
        
        private long x2DLong[];
        public double[] getX2D()
        {
            String in = "__jScope_var = ("+in_y+") ; DIM_OF( __jScope_var, 0);";
            try {
                RealArray realArray = GetRealArray(in);
                if( realArray.isLong() )
                {
                    this.isXLong = true;
                    x2DLong = realArray.getLongArray();
                    return null;
                }
                else
                {
                    x2DLong = null;
                    return realArray.getDoubleArray();
                }
                //return GetFloatArray(in);
            } catch(Exception exc){return null;}
        }
        public long[] getX2DLong()
        {
            return x2DLong;
        }
          
        public float[] getY2D()
        {
            String in = "__jScope_var = ("+in_y+") ; DIM_OF( __jScope_var, 1);";
            try {
                return GetFloatArray(in);
            }catch(Exception exc){return null;}
        }

        //Cesare Mar 2015
        
        public float[] getX_Z()
        {
            try {
                return GetFloatArray(in_x);
            }catch(Exception exc){return null;}
        }
        public float[] getX_X2D()
        {
            String in = "__jScope_var = ("+in_x+") ; DIM_OF( __jScope_var, 0);";
            try {
                return GetFloatArray(in);
            }catch(Exception exc){return null;}
        }
        public float[] getX_Y2D()
        {
            String in = "__jScope_var = ("+in_x+") ; DIM_OF( __jScope_var, 1);";
            try {
                return GetFloatArray(in);
            }catch(Exception exc){return null;}
        }
        //End
        
        
        //public double[] getXLimits(){System.out.println("BADABUM!!"); return null;}
        //public long []getXLong(){System.out.println("BADABUM!!"); return null;}
        public boolean isXLong(){return isXLong;}

        //Async update management
        Vector<WaveDataListener> waveDataListenersV = new Vector<>();


        public void addWaveDataListener(WaveDataListener listener)
        {
            waveDataListenersV.addElement(listener);
            if(asynchSource != null)
                asynchSource.addDataListener(listener);
        }
        public void getDataAsync(double lowerBound, double upperBound, int numPoints)
        {
 //           System.out.println("***GET DATA ASYNCH "+lowerBound+"  " + upperBound+"  " + numPoints);
            updateWorker.updateInfo(lowerBound, upperBound, numPoints, waveDataListenersV, this, isXLong);
        }
    } //END Inner Class SimpleWaveData

    
    //Inner class UpdateWorker handler asynchronous requests for getting (portions of) data
    class UpdateWorker extends Thread
    {
        class UpdateDescriptor
        {
            double updateLowerBound;
            double updateUpperBound;
            int updatePoints;
            Vector<WaveDataListener> waveDataListenersV;
            SimpleWaveData simpleWaveData;
            boolean isXLong;
            long updateTime;
             UpdateDescriptor(double updateLowerBound, double updateUpperBound, int updatePoints,
                    Vector<WaveDataListener> waveDataListenersV, SimpleWaveData simpleWaveData, boolean isXLong, long updateTime)
            {
                this.updateLowerBound = updateLowerBound;
                this.updateUpperBound = updateUpperBound;
                this.updatePoints = updatePoints;
                this.waveDataListenersV = waveDataListenersV;
                this.simpleWaveData = simpleWaveData;
                this.isXLong = isXLong;
                this.updateTime = updateTime;
            }
        }
        boolean enabled = true;
        Vector<UpdateDescriptor>requestsV = new Vector<>();
        void updateInfo(double updateLowerBound, double updateUpperBound, int updatePoints,
                Vector<WaveDataListener> waveDataListenersV, SimpleWaveData simpleWaveData, boolean isXLong, long delay)
        {
            intUpdateInfo(updateLowerBound, updateUpperBound, updatePoints, waveDataListenersV, simpleWaveData, isXLong,
                   Calendar.getInstance().getTimeInMillis()+delay);
        }
       void updateInfo(double updateLowerBound, double updateUpperBound, int updatePoints,
                Vector<WaveDataListener> waveDataListenersV, SimpleWaveData simpleWaveData, boolean isXLong)
        {
 //           intUpdateInfo(updateLowerBound, updateUpperBound, updatePoints, waveDataListenersV, simpleWaveData, isXLong,
 //                  Calendar.getInstance().getTimeInMillis());
           intUpdateInfo(updateLowerBound, updateUpperBound, updatePoints, waveDataListenersV, simpleWaveData, isXLong,
                   -1);
        }
        synchronized void intUpdateInfo(double updateLowerBound, double updateUpperBound, int updatePoints,
                Vector<WaveDataListener> waveDataListenersV, SimpleWaveData simpleWaveData, boolean isXLong, long updateTime)
        {
            if(updateTime > 0)  //If a delayed request for update NOTE NOT USED!!!!Use MdsStreamingDataProvider instead
            {
                for(int i = 0; i < requestsV.size(); i++)
                {
                    UpdateDescriptor currUpdateDescr = requestsV.elementAt(i);
                    if(currUpdateDescr.updateTime > 0 && currUpdateDescr.simpleWaveData == simpleWaveData)
                        return; //Another delayed update request for that signal is already in the pending list
                }
            }
            requestsV.add(new UpdateDescriptor(updateLowerBound, updateUpperBound, updatePoints, 
                    waveDataListenersV, simpleWaveData, isXLong, updateTime));
            
            notify();
        }
        synchronized void enableAsyncUpdate(boolean enabled)
        {
            this.enabled = enabled;
            if(enabled)
                notify();
        }
        
        boolean stopWorker = false;
        
        synchronized void stopUpdateWorker()
        {
            stopWorker = true;
            notify();
        }
        
        public void run()
        {
            
            this.setName("UpdateWorker");
            
            while(true)
            {
                synchronized(this)
                {
                    try {
                        wait();
                        if(stopWorker)
                           return;
                    }                           
                    catch(InterruptedException exc) {}
                }
                if(!enabled) continue;
                long currTime = Calendar.getInstance().getTimeInMillis();
                long nextTime = -1;
                int i = 0;
                while(i < requestsV.size())
                {
                    if(!enabled) break;
                    UpdateDescriptor currUpdate = requestsV.elementAt(i);
                    if(currUpdate.updateTime < currTime)
                    {
                         try 
                        {
                            
                            requestsV.removeElementAt(i);
                            XYData currData = currUpdate.simpleWaveData.getData(currUpdate.updateLowerBound, currUpdate.updateUpperBound, currUpdate.updatePoints, currUpdate.isXLong);
                            
                            if( currData == null || currData.nSamples == 0 )
                                continue;
                            for(int j = 0; j < currUpdate.waveDataListenersV.size(); j++)
                            {
                                if(currUpdate.isXLong)
                                    currUpdate.waveDataListenersV.elementAt(j).dataRegionUpdated(currData.xLong, currData.y, currData.resolution);
                                else
                                    currUpdate.waveDataListenersV.elementAt(j).dataRegionUpdated(currData.x, currData.y, currData.resolution);

                            }
                        }catch(Exception exc)
                        {
                            Date d = new Date();
                            System.out.println(d+" Error in asynchUpdate: "+exc);
                        }
                    }
                    else
                    {
                        if(nextTime == -1 || nextTime > currUpdate.updateTime) //It will alway be nextTime != -1
                            nextTime = currUpdate.updateTime;
                        i++;
                    }
                }
                if(nextTime != -1) //If a pending request for which time did not expire, schedure a new notification NOTE NOT USED
                {
                    currTime = Calendar.getInstance().getTimeInMillis();
                    java.util.Timer timer = new java.util.Timer();
                    timer.schedule(new TimerTask() {
                        public void run()
                        {
                            synchronized(UpdateWorker.this)
                            {
                                UpdateWorker.this.notify();
                            }
                        }
                    }, (nextTime < currTime + 50)?50:(nextTime - currTime + 1));
                }
            }
        }
    } //End Inner class UpdateWorker

    UpdateWorker updateWorker;

    
    public MdsDataProvider()
    {
        experiment = null;
        shot = 0;
        open = connected = false;
        mds = getConnection();
        error = null;
        //updateWorker = new UpdateWorker();
        //updateWorker.start();
    }

    protected MdsConnection getConnection() 
    {
        return new MdsConnection();
    }
    
    
    public MdsDataProvider(String provider)
    {
        setProvider(provider);
        experiment = null;
        shot = 0;
        open = connected = false;
        mds = new MdsConnection(this.provider);
        error = null;
        //updateWorker = new UpdateWorker();
        //updateWorker.start();
    }

    public MdsDataProvider(String exp, int s)
    {
        experiment = exp;
        shot = 0;
        open = connected = false;
        mds = new MdsConnection();
        error = null;
        //updateWorker = new UpdateWorker();
        //updateWorker.start();
    }

    protected void finalize()
    {
        if (open)
            mds.MdsValue("JavaClose(\"" + experiment + "\"," + shot + ")");
        if (connected)
            mds.DisconnectFromMds();
    }
    //To be overridden by any DataProvider implementation with added dynamic generation
    public jScope.AsynchDataSource getAsynchSource()
    {
        return null;
    }

    public void SetArgument(String arg) throws IOException
    {
        setProvider(arg);
        mds.setProvider(provider);
    }

    private void setProvider(String arg)
    {
        if (is_tunneling)
            provider = tunnel_provider;
        else
            provider = arg;
    }

    public boolean SupportsCompression()
    {
        return true;
    }

    public void SetCompression(boolean state)
    {
        if (connected)
            Dispose();
        use_compression = state;
    }

    protected String GetExperimentName(String in_frame)
    {
        String exp;

        if (experiment == null)
        {
            if (in_frame.indexOf(".") == -1)
                exp = in_frame;
            else
                exp = in_frame.substring(0, in_frame.indexOf("."));
        }
        else
            exp = experiment;

        return exp;
    }

    public FrameData GetFrameData(String in_y, String in_x, float time_min,
                                  float time_max) throws IOException
    {
        int[] numSegments = null; 
        try
        {
            numSegments = GetIntArray("GetNumSegments("+in_y+")");
        } catch(Exception exc){ error = null;}
        
        if(numSegments != null && numSegments[0] > 0)
            return new SegmentedFrameData(in_y, in_x, time_min, time_max, numSegments[0]);
        else
        {
            try {
                return (new SimpleFrameData(in_y, in_x, time_min, time_max));
            }catch(Exception exc){return null;}
        }
    }

    public synchronized byte[] GetAllFrames(String in_frame) throws IOException
    {
        float time[];
        int shape[];
        int pixel_size = 8;
        int num_time = 0;

        if (!CheckOpen())
            return null;

        String in;

        in = "DIM_OF( _jScope_img = (" + in_frame + "), 2)";
        time = GetFloatArray(in);
        if (time == null || ((time.length >= 2 ) && (time[1] == 1.0)))
        {
            //in = "DIM_OF(" + in_frame + ")";
            in = "DIM_OF( _jScope_img )";
            time = GetFloatArray(in);
            if (time == null)
                return null;
        }
//      in = "eshape(data(" + in_frame + "))";
        in = "eshape(data( _jScope_img ))";
        shape = GetIntArray(in);
        if (shape == null)
            return null;

        //in = in_frame;
        in = "_jScope_img";
        byte[] img_buf = GetByteArray(in);
        if (img_buf == null)
            return null;

        if (shape.length == 3)
        {
            num_time = shape[2];
            pixel_size = img_buf.length / (shape[0] * shape[1] * shape[2]) * 8;
        }
        else
        {
            if (shape.length == 2)
            {
                num_time = 1;
                pixel_size = img_buf.length / (shape[0] * shape[1]) * 8;
            }
            else
            if (shape.length == 1)
            {
                throw (new IOException("The evaluated signal is not an image"));
            }
        }

        ByteArrayOutputStream b = new ByteArrayOutputStream();
        try (DataOutputStream d = new DataOutputStream(b)) {
            d.writeInt(pixel_size);
            d.writeInt(shape[0]);
            d.writeInt(shape[1]);
            d.writeInt(num_time);
            for (int i = 0; i < num_time; i++)
                d.writeFloat(time[i]);
            d.write(img_buf);
            return b.toByteArray();
	}
    }

    public synchronized float[] GetFrameTimes(String in_frame)
    {
        String exp = GetExperimentName(in_frame);

        String in = "JavaGetFrameTimes(\"" + exp + "\",\"" + in_frame + "\"," +
            shot + " )";
        //    if(!CheckOpen())
        //	    return null;
        Descriptor desc = mds.MdsValue(in);
        switch (desc.dtype)
        {
            case Descriptor.DTYPE_FLOAT:
                return desc.float_data;
            case Descriptor.DTYPE_LONG:
                float[] out_data = new float[desc.int_data.length];
                for (int i = 0; i < desc.int_data.length; i++)
                    out_data[i] = (float) desc.int_data[i];
                return out_data;
            case Descriptor.DTYPE_BYTE:
                error = "Cannot convert byte array to float array";
                return null;
            case Descriptor.DTYPE_CSTRING:
                if ( (desc.status & 1) == 0)
                    error = desc.error;
                return null;
        }
        return null;
    }

    public byte[] GetFrameAt(String in_frame, int frame_idx) throws IOException
    {

        String exp = GetExperimentName(in_frame);

        String in = "JavaGetFrameAt(\"" + exp + "\",\" " + in_frame + "\"," +
            shot + ", " + frame_idx + " )";

        //    if(!CheckOpen())
        //	    return null;
        return GetByteArray(in);
    }
    public  synchronized  byte[] GetByteArray(String in) throws IOException
    {
        return GetByteArray(in, null);
    }

    public  synchronized  byte[] GetByteArray(String in, Vector args) throws IOException
    {
        byte out_byte[] = null;
        ByteArrayOutputStream dosb = new ByteArrayOutputStream();
        DataOutputStream dos = new DataOutputStream(dosb);

        if (!CheckOpen())
            return null;
        Descriptor desc;
        if(args == null)
            desc = mds.MdsValue(in);
        else
            desc = mds.MdsValue(in, args);
        switch (desc.dtype)
        {
            case Descriptor.DTYPE_FLOAT:
                for (int i = 0; i < desc.float_data.length; i++)
                    dos.writeFloat(desc.float_data[i]);
                out_byte = dosb.toByteArray();
                return out_byte;
            case Descriptor.DTYPE_USHORT:
            case Descriptor.DTYPE_SHORT: // bdb hacked this to try to make profile dialog read true data values, not normalised                
                for(int i = 0; i < desc.short_data.length; i++)
                    dos.writeShort(desc.short_data[i]);
                out_byte = dosb.toByteArray();
                return out_byte;
            case Descriptor.DTYPE_LONG:
                for (int i = 0; i < desc.int_data.length; i++)
                    dos.writeInt(desc.int_data[i]);
                out_byte = dosb.toByteArray();
                return out_byte;
            case Descriptor.DTYPE_UBYTE:
            case Descriptor.DTYPE_BYTE:
                return desc.byte_data;
            case Descriptor.DTYPE_CSTRING:
                if ( (desc.status & 1) == 0)
                    error = desc.error;
                 throw new IOException(error);
       }
       throw new IOException(error);
    }

    public synchronized String ErrorString()
    {
        return error;
    }

    public synchronized void Update(String experiment, long shot)
    {
        Update( experiment,  shot, false);
    }
    
    public synchronized void Update(String experiment, long shot, boolean resetExperiment)
    {
        this.error = null;
        this.var_idx = 0;

        if (resetExperiment) {
          this.experiment = null;
        }
        if ((shot != this.shot) || (shot == 0L) || (this.experiment == null) || (this.experiment.length() == 0) || (!this.experiment.equalsIgnoreCase(experiment)))
        {
          this.experiment = ((experiment != null) && (experiment.trim().length() > 0) ? experiment : null);
          this.shot = shot;
          this.open = false;
          resetPrevious();
        }
    }

    public void resetPrevious()  //Will be used by subclass MdsSreaminDataProvider to close previous  connections
    {}
    public synchronized String GetString(String in) throws IOException
    {
        if (in == null)
            return null;

        error = null;

        if (NotYetString(in))
        {
            if (!CheckOpen())
                return null;
            Descriptor desc = mds.MdsValue(in);
            switch (desc.dtype)
            {
                case Descriptor.DTYPE_BYTE:
                case Descriptor.DTYPE_UBYTE:
                    return new String(desc.byte_data);
                case Descriptor.DTYPE_FLOAT:
                    error = "Cannot convert a float to string";
                    throw new IOException(error);
                case Descriptor.DTYPE_CSTRING:
                    if ( (desc.status & 1) == 1)
                        return desc.strdata;
                    else
                        return (error = desc.error);
            }
            if(desc.error == null)
                return "Undefined error";
            return (error = desc.error);
        }
        else
            return new String(in.getBytes(), 1, in.length() - 2);
    }

    public synchronized void SetEnvironment(String in) throws IOException
    {
        if (in == null || in.length() == 0)
            return;

        Properties pr = new Properties();
        pr.load(new ByteArrayInputStream(in.getBytes()));
        String def_node = pr.getProperty("__default_node");
        if (def_node != null)
        {
            def_node = def_node.trim();
            if (! (default_node != null && def_node.equals(default_node))
                || (def_node.length() == 0 && default_node != null))
            {
                default_node = (def_node.length() == 0) ? null : def_node;
                def_node_changed = true;
            }
            return;
        }
        
        if( in.indexOf("pulseSetVer") >= 0 )
        {
            open = false;
        }
        
        if( environment_vars == null || !environment_vars.equalsIgnoreCase(in) )
        {
            open = false;
            environment_vars = in;
        }
    }

    void SetEnvironmentSpecific(String in)
    {
        Descriptor desc = mds.MdsValue(in);
        switch (desc.dtype)
        {
            case Descriptor.DTYPE_CSTRING:
                if ( (desc.status & 1) == 0)
                    error = desc.error;
        }
    }
    public void enableAsyncUpdate(boolean enable)
    {
        updateWorker.enableAsyncUpdate(enable);
                  
    }

    double GetNow(String in) throws Exception
    {
        boolean isPlus = true;
        int hours = 0, minutes = 0, seconds = 0;
        String currStr = in.trim().toUpperCase();
        if(!currStr.startsWith("NOW"))
            throw new Exception();
        currStr = currStr.substring(3).trim();
        if(currStr.length() > 0) //Not only NOW
        {
            if(currStr.startsWith("+"))
                isPlus = true;
            else if(currStr.startsWith("-"))
                isPlus = false;
            else throw new Exception();
            currStr = currStr.substring(1).trim();
            StringTokenizer st = new StringTokenizer(currStr, ":", true);
            String currTok = st.nextToken();
            if(currTok.equals(":"))
                hours = 0;
            else
            {
                hours = Integer.parseInt(currTok);
                currTok = st.nextToken();
            }
            if(!currTok.equals(":"))
                throw new Exception();
            currTok = st.nextToken();
            if(currTok.equals(":"))
                minutes = 0;
            else
            {
                minutes = Integer.parseInt(currTok);
                currTok = st.nextToken();
            }
            if(!currTok.equals(":"))
                throw new Exception();
            if(st.hasMoreTokens())
            {
                seconds = Integer.parseInt(st.nextToken());
            }
        }
        if(!isPlus)
        {
            hours = -hours;
            minutes = -minutes;
            seconds = -seconds;
        }
        Calendar cal = Calendar.getInstance();
        //cal.setTimeZone(TimeZone.getTimeZone("GMT+00"));
        cal.setTime(new Date());
        cal.add(Calendar.HOUR, hours);
        cal.add(Calendar.MINUTE, minutes);
        cal.add(Calendar.SECOND, seconds);
        long javaTime = cal.getTime().getTime();
        return javaTime;
    }
 
    public synchronized double GetFloat(String in) throws IOException
    {
        error = null;

        //First check Whether this is a date
        try {
            Calendar cal = Calendar.getInstance();
            //cal.setTimeZone(TimeZone.getTimeZone("GMT+00"));
            DateFormat df = new SimpleDateFormat("d-MMM-yyyy HH:mm Z");
//            DateFormat df = new SimpleDateFormat("d-MMM-yyyy HH:mm");
            Date date = df.parse(in + " GMT");
            //Date date = df.parse(in);
            cal.setTime(date);
            long javaTime = cal.getTime().getTime();
            return javaTime;
        }catch(Exception exc)
        { 
            try {
                return GetNow(in);
            }catch(Exception exc1){}
        } //If exception occurs this is not a date, try NOW condtruction

        if (NotYetNumber(in))
        {
            if (!CheckOpen())
                return 0;
            Descriptor desc = mds.MdsValue(in);
            if (desc.error != null)
                error = desc.error;
            switch (desc.dtype)
            {
                case Descriptor.DTYPE_DOUBLE:
                    return desc.double_data[0];
                case Descriptor.DTYPE_FLOAT:
                    return desc.float_data[0];
                case Descriptor.DTYPE_LONG:
                    return (float) desc.int_data[0];
                case Descriptor.DTYPE_BYTE:
                case Descriptor.DTYPE_UBYTE:
                    return (float) desc.byte_data[0];
                case Descriptor.DTYPE_CSTRING:
                    if ( (desc.status & 1) == 0)
                    {
                        error = desc.error;
                        throw (new IOException(error));
                    }
                    return 0;
            }
        }
        else
            return Float.parseFloat(in);
        return 0;
    }

    public WaveData GetWaveData(String in)
    {
        return new SimpleWaveData(in, experiment, shot);
    }

    public WaveData GetWaveData(String in_y, String in_x)
    {
        return new SimpleWaveData(in_y, in_x, experiment, shot);
    }

    public float[] GetFloatArray(String in) throws IOException
    {
        RealArray realArray = GetRealArray(in);
        if (realArray == null)
            return null;
        return realArray.getFloatArray();
    }

    public double[] GetDoubleArray(String in) throws IOException
    {
        RealArray realArray = GetRealArray(in);
        if (realArray == null)
            return null;
        return realArray.getDoubleArray();
    }

    public synchronized RealArray GetRealArray(String in) throws IOException
    {

        RealArray out;
        ConnectionEvent e = new ConnectionEvent(this, 1, 0);
        DispatchConnectionEvent(e);

        if (!CheckOpen())
            return null;

        Descriptor desc = mds.MdsValue(in);
        out = null;
        switch (desc.dtype)
        {
            case Descriptor.DTYPE_FLOAT:
                out = new RealArray(desc.float_data);
                break;
            case Descriptor.DTYPE_DOUBLE:
                out = new RealArray(desc.double_data);
                break;
            case Descriptor.DTYPE_LONG:
            case Descriptor.DTYPE_USHORT:
            {
                float[] outF = new float[desc.int_data.length];
                for (int i = 0; i < desc.int_data.length; i++)
                    outF[i] = (float) desc.int_data[i];
                out = new RealArray(outF);
            }
            break;
            case Descriptor.DTYPE_SHORT:
            {
                float[] outF = new float[desc.short_data.length];
                for (int i = 0; i < desc.short_data.length; i++)
                    outF[i] = (float) desc.short_data[i];
                out = new RealArray(outF);
            }
            break;
            case Descriptor.DTYPE_BYTE:
            case Descriptor.DTYPE_UBYTE:
            {
                float[] outF = new float[desc.byte_data.length];
                for (int i = 0; i < desc.byte_data.length; i++)
                    outF[i] = (float) desc.byte_data[i];
                out = new RealArray(outF);
            }
            break;
            case Descriptor.DTYPE_ULONGLONG:
            case Descriptor.DTYPE_LONGLONG:
            {
               out = new RealArray(desc.long_data);
            }
            break;
            case Descriptor.DTYPE_CSTRING:
                if ( (desc.status & 1) == 0)
                    error = desc.error;
                break;
            default:
                error = "Data type code : " + desc.dtype +
                    " not yet supported ";
        }

        return out;
    }

    public long[] GetShots(String in) throws IOException
    {
        //To shot evaluation don't execute check
        //if a pulse file is open
        CheckConnection();
        //try
        {

            return GetLongArray(in);
        }
    }

    public int[] GetIntArray(String in) throws IOException
    {
        if (!CheckOpen())
            throw new IOException("Tree not open");
        return GetIntegerArray(in);
    }

    private synchronized long[] GetLongArray(String in) throws IOException
    {
        long out_data[];

        Descriptor desc = mds.MdsValue(in);
        switch (desc.dtype)
        {
            case Descriptor.DTYPE_ULONGLONG:
            case Descriptor.DTYPE_LONGLONG:
                return desc.long_data;
            case Descriptor.DTYPE_LONG:
                out_data = new long[desc.int_data.length];
                for (int i = 0; i < desc.int_data.length; i++)
                {
                    out_data[i] = (long) (desc.int_data[i]);
                }
                return out_data;
            case Descriptor.DTYPE_FLOAT:
                out_data = new long[desc.float_data.length];
                for (int i = 0; i < desc.float_data.length; i++)
                    out_data[i] = (long) (desc.float_data[i] + 0.5);
                return out_data;
            case Descriptor.DTYPE_BYTE:
            case Descriptor.DTYPE_UBYTE:
                out_data = new long[desc.byte_data.length];
                for (int i = 0; i < desc.byte_data.length; i++)
                    out_data[i] = (long) (desc.byte_data[i] + 0.5);
                return out_data;
            case Descriptor.DTYPE_CSTRING:
                if ( (desc.status & 1) == 0)
                    error = desc.error;
                throw new IOException(error);
            default:
                error = "Data type code : " + desc.dtype +
                    " not yet supported ";
        }
        throw new IOException(error);
     }

    private synchronized int[] GetIntegerArray(String in) throws IOException
    {
        int out_data[];

        Descriptor desc = mds.MdsValue(in);
        switch (desc.dtype)
        {
            case Descriptor.DTYPE_LONG:
                return desc.int_data;
            case Descriptor.DTYPE_FLOAT:
                out_data = new int[desc.float_data.length];
                for (int i = 0; i < desc.float_data.length; i++)
                    out_data[i] = (int) (desc.float_data[i] + 0.5);
                return out_data;
            case Descriptor.DTYPE_BYTE:
            case Descriptor.DTYPE_UBYTE:
                out_data = new int[desc.byte_data.length];
               for (int i = 0; i < desc.byte_data.length; i++)
                   out_data[i] = (int) (desc.byte_data[i] + 0.5);
               return out_data;
            case Descriptor.DTYPE_CSTRING:
                if ( (desc.status & 1) == 0)
                    error = desc.error;
                throw new IOException(error);
            default:
                error = "Data type code : " + desc.dtype +
                    " not yet supported ";
        }
        throw new IOException(error);
    }

    public synchronized void Dispose()
    {

       if (is_tunneling && ssh_tunneling != null)
       {
           ssh_tunneling.Dispose();
       }

       if (connected)
        {
            connected = false;
            mds.DisconnectFromMds();

            ConnectionEvent ce = new ConnectionEvent(this,
                ConnectionEvent.
                LOST_CONNECTION,
                "Lost connection from : " +
                provider);
            mds.dispatchConnectionEvent(ce);
        }
       
        if( updateWorker != null && updateWorker.isAlive() )
        {
            updateWorker.stopUpdateWorker();
        }
    }

    protected synchronized void CheckConnection() throws IOException
    {
        if (!connected)
        {
            if (mds.ConnectToMds(use_compression) == 0)
            {
                if (mds.error != null)
                    throw new IOException(mds.error);
                else
                    throw new IOException("Could not get IO for " + provider);
            }
            else
            {
                connected = true;
                updateWorker = new UpdateWorker();
                updateWorker.start();
            }
        }
    }

    protected synchronized boolean CheckOpen() throws IOException
    {
        return CheckOpen( this.experiment,  this.shot);
    }
    
    protected synchronized boolean CheckOpen(String experiment, long shot) throws IOException
    {
        int status;
        if (!connected)
        {
            status = mds.ConnectToMds(use_compression);
            if (status == 0)
            {
                if (mds.error != null)
                    throw new IOException("Cannot connect to data server : " +
                                          mds.error);
                else
                    error = "Cannot connect to data server";
                return false;
            }
            connected = true;
            updateWorker = new UpdateWorker();
            updateWorker.start();

        }
        if (!open && experiment != null || this.shot != shot || experiment != null && !experiment.equalsIgnoreCase(this.experiment) )
        {
            //System.out.println("\n-->\nOpen tree "+experiment+ " shot "+ shot +"\n<--\n");
            Descriptor descr = mds.MdsValue("JavaOpen(\"" + experiment + "\"," +
                                            shot + ")");
            if (descr.dtype != Descriptor.DTYPE_CSTRING
                && descr.dtype == Descriptor.DTYPE_LONG && descr.int_data != null
                && descr.int_data.length > 0 && (descr.int_data[0] % 2 == 1))
            {
                open = true;
                def_node_changed = true;
                this.shot = shot;
                this.experiment = experiment;
                
                if( environment_vars != null && environment_vars.length() > 0 )
                {
                    this.SetEnvironmentSpecific(environment_vars);
                    if(error != null)
                    {
                        error = "Public variable evaluation error " + experiment + " shot " +
                        shot + " : " + error;
                      return false;
                    }
                }
            }
            else
            {
                if (mds.error != null)
                    error = "Cannot open experiment " + experiment + " shot " +
                        shot + " : " + mds.error;
                else
                    error = "Cannot open experiment " + experiment + " shot " +
                        shot;
                return false;
            }
        }
        if (open && def_node_changed)
        {
            if (default_node != null) {
                Descriptor descr = mds.MdsValue("TreeSetDefault(\"\\\\" + default_node + "\")");
                if ((descr.int_data[0] & 1 ) == 0)
                    mds.MdsValue("TreeSetDefault(\"\\\\" + experiment + "::TOP\")");
            } else
                mds.MdsValue("TreeSetDefault(\"\\\\" + experiment + "::TOP\")");

            def_node_changed = false;
        }
        return true;
    }

    protected boolean NotYetString(String in)
    {
        int i;
        if (in.charAt(0) == '\"')
        {
            for (i = 1; i < in.length() && (in.charAt(i) != '\"' ||
                                            (i > 0 && in.charAt(i) == '\"' &&
                                             in.charAt(i - 1) == '\\')); i++)
                ;
            if (i == (in.length() - 1))
                return false;
        }
        return true;
    }

    protected boolean NotYetNumber(String in)
    {
        try {
            Float.parseFloat(in);
	    return false;
        } catch (NumberFormatException e) {
            return true;
        }
    }

    public synchronized void AddUpdateEventListener(UpdateEventListener l,
        String event_name) throws IOException
    {
        if (event_name == null || event_name.trim().length() == 0)
            return;
        CheckConnection();
        mds.MdsSetEvent(l, event_name);
    }

    public synchronized void RemoveUpdateEventListener(UpdateEventListener l,
        String event_name) throws IOException
    {
        if (event_name == null || event_name.trim().length() == 0)
            return;
        CheckConnection();
        mds.MdsRemoveEvent(l, event_name);
    }

    public synchronized void AddConnectionListener(ConnectionListener l)
    {
        if (mds != null)
            mds.addConnectionListener(l);
    }

    public synchronized void RemoveConnectionListener(ConnectionListener l)
    {
        if (mds != null)
            mds.removeConnectionListener(l);
    }

    protected void DispatchConnectionEvent(ConnectionEvent e)
    {
        if (mds != null)
            mds.dispatchConnectionEvent(e);
    }

    public boolean SupportsTunneling()
    {
        return true;
    }

    public int InquireCredentials(JFrame f, DataServerItem server_item)
    {
        mds.setUser(server_item.user);
        is_tunneling = false;
        if (server_item.tunnel_port != null &&
            server_item.tunnel_port.trim().length() != 0)
        {
            StringTokenizer st = new StringTokenizer(server_item.argument, ":");
            String ip;
            String remote_port = "" + MdsConnection.DEFAULT_PORT;

            ip = st.nextToken();
            if (st.hasMoreTokens())
                remote_port = st.nextToken();

            is_tunneling = true;

            try
            {
                ssh_tunneling = new SshTunneling(f, this, ip, remote_port,
                                                 server_item.user,
                                                 server_item.tunnel_port);
                ssh_tunneling.start();
                tunnel_provider = "127.0.0.1:" + server_item.tunnel_port;
            }
            catch (Throwable exc)
            {
                if (exc instanceof NoClassDefFoundError)
                    JOptionPane.showMessageDialog(f, "The MindTerm.jar library is required for ssh tunneling.You can download it from \nhttp://www.appgate.com/mindterm/download.php\n"+exc,
                                                  "alert",
                                                  JOptionPane.ERROR_MESSAGE);
                return DataProvider.LOGIN_ERROR;
            }
        }
        return DataProvider.LOGIN_OK;
    }

    public boolean SupportsFastNetwork()
    {
        return true;
    }

    protected String GetStringValue(String expr) throws IOException
    {
        String out = GetString(expr);
        if (out == null || out.length() == 0 || error != null)
        {
            error = null;
            return null;
        }
        if(out.indexOf(0) > 0 )
          out = out.substring(0, out.indexOf(0));

        return out;
    }

    protected int[] GetNumDimensions(String in_y) throws IOException
    {
        //return GetIntArray(in_y);
        //Gabriele June 2013: reduce dimension if one component is 1
        int [] fullDims = GetIntArray(in_y);

		if( fullDims == null )
			return null;

        if(fullDims.length == 1) return fullDims;
        //count dimensions == 1
        int numDimensions = 0;
        for(int i = 0; i < fullDims.length; i++)
        {
            if(fullDims[i] != 1)
                numDimensions++;
        }
        int [] retDims = new int[numDimensions];
        int j = 0;
        for(int i = 0; i < fullDims.length; i++)
        {
            if(fullDims[i] != 1)
                retDims[j++] = fullDims[i];
         }
         return retDims;
    }

    static class RealArray
    {
        double doubleArray[] = null;
        float floatArray[] = null;
        long  longArray[] = null;
        boolean isDouble;
        boolean isLong;

        RealArray(float[] floatArray)
        {
            this.floatArray = floatArray;
            isDouble = false;
            isLong = false;
        }

        RealArray(double[] doubleArray)
        {
            this.doubleArray = doubleArray;
            isDouble = true;
            isLong = false;
       }

       RealArray(long[] longArray)
       {
           this.longArray = longArray;
           for(int i = 0; i < longArray.length; i++)
               longArray[i] = jScopeFacade.convertFromSpecificTime(longArray[i]);

           isDouble = false;
           isLong = true;
      }

        boolean isDouble()
        {
            return isDouble;
        }

        boolean isLong()
        {
            return isLong;
        }

        float[] getFloatArray()
        {
            if(isLong) return null;

            if (isDouble && floatArray == null && doubleArray != null)
            {
                floatArray = new float[doubleArray.length];
                for (int i = 0; i < doubleArray.length; i++)
                    floatArray[i] = (float) doubleArray[i];
            }
            return floatArray;
        }

        double[] getDoubleArray()
        {
            if(isLong) return null;

            if (!isDouble && floatArray != null && doubleArray == null)
            {
                doubleArray = new double[floatArray.length];
                for (int i = 0; i < floatArray.length; i++)
                    doubleArray[i] = floatArray[i];
            }
            return doubleArray;
        }

        long[] getLongArray()
        {
            if(isDouble) return null;
            return longArray;
        }

        
    }
    public String getProvider()
    {
        return provider;
    }

}

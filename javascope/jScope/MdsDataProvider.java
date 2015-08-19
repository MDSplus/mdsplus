package jScope;

import jScope.ConnectionEvent;
import jScope.ConnectionListener;
import java.io.*;
import java.net.*;
import java.awt.*;
import java.util.*;
import java.lang.OutOfMemoryError;
import java.lang.InterruptedException;
import javax.swing.*;
import java.text.*;
import java.util.logging.Level;
import java.util.logging.Logger;

public class MdsDataProvider implements DataProvider
{
    public final class DEGUB {
    //set to false to allow compiler to identify and eliminate
    //unreachable code
        public static final boolean ON = false;
    }
    String provider;
    String experiment;
    String default_node;
    String environment_vars;
    private boolean def_node_changed = false;
    long shot;
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

    public MdsDataProvider()
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider()");}
        experiment = null;
        shot = 0;
        open = connected = false;
        mds = getConnection();
        error = null;
        //updateWorker = new UpdateWorker();
        //updateWorker.start();
    }

    public MdsDataProvider(String provider)
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider(\""+provider+"\")");}
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
        if (DEGUB.ON){System.out.println("MdsDataProvider(\""+exp+"\", "+s+")");}
        experiment = exp;
        shot = 0; //what's about s
        open = connected = false;
        mds = new MdsConnection();
        error = null;
        //updateWorker = new UpdateWorker();
        //updateWorker.start();
    }

    class SegmentedFrameData implements FrameData
    {
        String in_x, in_y;
        float time_max, time_min;
        int framesPerSegment;
        int numSegments;
        int startSegment, endSegment, actSegments;
        int mode;
        Dimension dim;
        float times[];
        int bytesPerPixel;

        public SegmentedFrameData(String in_y, String in_x, float time_min, float time_max, int numSegments) throws IOException
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SegmentedFrameData("+in_y+", "+in_x+", "+time_min+", "+time_max+", "+numSegments+")");}
            //Find out frames per segment and frame min and max based on time min and time max
            this.in_x = in_x;
            this.in_y = in_y;
            this.time_min = time_min;
            this.time_max = time_max;
            this.numSegments = numSegments;
            startSegment = -1;
            float startTimes[] = new float[numSegments];
 //Get segment window corresponding to the passed time window
            for(int i = 0; i < numSegments; i++)
            {
                float limits[] = GetFloatArray("GetSegmentLimits("+in_y+","+i+")");
                startTimes[i] = limits[0];
                if(limits[1] > time_min)
                {
                    startSegment = i;
                    break;
                }
            }
            if(startSegment == -1)
                throw new IOException("Frames outside defined time window");
//Check first if endTime is greated than the end of the last segment, to avoid rolling over all segments
            float endLimits[] = GetFloatArray("GetSegmentLimits("+in_y+","+(numSegments - 1)+")");
//Throw away spurious frames at the end
            while(endLimits == null || endLimits.length != 2)
            {
                numSegments--;
                if(numSegments == 0)
                    break;
                endLimits = GetFloatArray("GetSegmentLimits("+in_y+","+(numSegments - 1)+")");
            }
            if(numSegments > 100 && endLimits[0] < time_max)
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
                        float limits[] = GetFloatArray("GetSegmentLimits("+in_y+","+endSegment+")");
                        startTimes[endSegment] = limits[0];
                        if(limits[0] > time_max)
                            break;
                    }catch(Exception exc){break;}
                }
            }

            actSegments = endSegment - startSegment;
//Get Frame Dimension and frames per segment
            int dims[] = GetIntArray("shape(GetSegment("+in_y+", 0))");
            if(dims.length != 3)
                throw new IOException("Invalid number of segment dimensions: "+ dims.length);
            dim = new Dimension(dims[0], dims[1]);
            framesPerSegment = dims[2];
//Get Frame element length in bytes
            int len[] = GetIntArray("len(GetSegment("+in_y+", 0))");
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
                    float segTimes [] = GetFloatArray("dim_of(GetSegment("+in_y+","+i+"))");
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
            byte[] segment = GetByteArray("GetSegment("+ in_y+","+segmentIdx+")");
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
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleFrameData("+in_y+", "+in_x+", time_min, time_max)");}
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
                int img_size = height * width;
                int n_frame = d.readInt();
                Vector f_time = new Vector();

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
                    if (DEGUB.ON){System.out.println("GetWaveData(in_x), "+in_x);}
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
    } //END Inner Class SimpleFrameData

    
    ////////////////////////////////////////GAB JULY 2014
     
    class SimpleWaveData implements WaveData
    {
        String in_x, in_y, v_x, v_y;
        static final int SEGMENTED_YES = 1, SEGMENTED_NO = 2, SEGMENTED_UNKNOWN = 3;
        static final int UNKNOWN = -1;
        int numDimensions = UNKNOWN;
        int segmentMode = SEGMENTED_UNKNOWN;
        boolean isXLong = false;
        String title = null;
        String xLabel = null;
        String yLabel = null;
        boolean titleEvaluated = false;
        boolean xLabelEvaluated = false;
        boolean yLabelEvaluated = false;
        boolean continuousUpdate = false;
        String wd_experiment;
        long wd_shot;
        AsynchDataSource asynchSource = null;

        public SimpleWaveData(String in_y, String experiment, long shot)
        {
            this(in_y, null , experiment, shot);
        }

        public SimpleWaveData(String in_y, String in_x, String experiment, long shot)
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData("+in_y+", "+in_x+", "+experiment+", "+shot+")");}
            this.wd_experiment = experiment;
            this.wd_shot = shot;
            v_y = "_jscope_"+(var_idx  );
            v_x = "_jscope_"+(var_idx+1);
            var_idx+=2;
            if(checkForAsynchRequest(in_y))
            {
                this.in_y = "[]";
                this.in_x = "[]";
            }
            else
            {
                try
                {
                    if (DEGUB.ON){System.out.println(">> wait y - "+in_y);}
                    GetString(v_y+"=("+in_y+");\"\"");//we just want mds to execute in_y; returning empty string \"\"
                    if (DEGUB.ON){System.out.println(">> y done: ("+v_y+"="+in_y+")");}

                    if(in_x == null)
                    {
                        if (DEGUB.ON){System.out.println(">> wait x - null");}
                        in_x = "DIM_OF("+in_y+");"; //to be save with other code, write the real expression to in_x
                        GetString(v_x+"=DIM_OF("+v_y+");\"\"");//we just want mds to execute DIM_OF("+v_y+"); returning empty string \"\"
                        if (DEGUB.ON){System.out.println(">> x done: ("+v_x+"=DIM_OF("+v_y+"))");}
                    }
                    else
                    {
                        if (DEGUB.ON){System.out.println(">> wait x - "+in_x);}
                        GetString(v_x+"=("+in_x+");\"\"");//we just want mds to execute in_x; returning empty string \"\"
                        if (DEGUB.ON){System.out.println(">> x done: ("+v_x+"="+in_x+")");}
                    }

                } catch(Exception exc){System.err.println(exc);}
                this.in_y = in_y;
                this.in_x = in_x;
            }
            SegmentMode();
        }


        private void SegmentMode()
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.SegmentMode()");}
            if(segmentMode == SEGMENTED_UNKNOWN)
            {
                try {//fast using in_y as NumSegments is a node property
                    int[] numSegments = GetIntArray("GetNumSegments("+in_y+")");
                    if (numSegments==null)
                        segmentMode = SEGMENTED_UNKNOWN;
                    else if(numSegments[0] > 0)
                        segmentMode = SEGMENTED_YES;
                    else
                        segmentMode = SEGMENTED_NO;
                }catch(Exception exc)
                {// numSegments==null should not get here anymore
                    if (DEGUB.ON){System.err.println("SegmentMode:\n"+exc);}
                    error = null;
                    segmentMode = SEGMENTED_UNKNOWN;
                }
            }
        }

        //Check if the passed Y expression specifies also an asynchronous part (separated by the patern &&&)
        //in case get an implemenation of AsynchDataSource
        boolean checkForAsynchRequest(String expression)
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.checkForAsynchRequest(\""+expression+"\")");}
            if(expression.startsWith("ASYNCH::"))
            {
                 asynchSource = getAsynchSource();
                if(asynchSource != null)
                     asynchSource.startGeneration(expression.substring("ASYNCH::".length()));
                return true;
             }
            return false;
        }

        public void setContinuousUpdate(boolean continuousUpdate)
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.setContinuousUpdate("+continuousUpdate+")");}
            this.continuousUpdate = continuousUpdate;
        }

        public int getNumDimension() throws IOException
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.getNumDimension()");}
            if(numDimensions != UNKNOWN)
                return numDimensions;

            String expr;
            if(segmentMode == SEGMENTED_YES)
                expr = "GetSegment("+v_y+",0)";
            else
                expr = v_y;

            error = null;
            int shape[] = GetNumDimensions(expr);

            if (error != null || shape == null)
            {
                error = null;
                return 1;
            }
            numDimensions = shape.length;
            return shape.length;
        }

        public String GetTitle() throws IOException
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.GetTitle()");}
            if(!titleEvaluated)
            {
                titleEvaluated = true;
                title = GetStringValue("help_of("+v_y+")");
            }
            return title;
        }

        public String GetXLabel() throws IOException
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.GetXLabel()");}
            if(!xLabelEvaluated)
            {
                xLabelEvaluated = true;
                xLabel = GetStringValue("Units("+v_x+")");
            }
            return xLabel;
        }


        public String GetYLabel() throws IOException
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.GetYLabel()");}
            if(!yLabelEvaluated)
            {
                yLabelEvaluated = true;
                if( getNumDimension() > 1)
                {
                    if(segmentMode == SEGMENTED_YES)
                        yLabel = GetStringValue("Units(dim_of(GetSegment("+v_y+",0),1))");
                    else
                        yLabel = GetStringValue("Units(dim_of("+v_y+",1))");
                }
                else
                {
                    if(segmentMode == SEGMENTED_YES)
                        yLabel = GetStringValue("Units(GetSegment("+v_y+",0))");
                    else
                        yLabel = GetStringValue("Units("+v_y+")");
                }
            }
            return yLabel;
        }

        public String GetZLabel() throws IOException
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.GetZLabel()");}
            return GetStringValue("Units("+v_y+")");
        }

        //GAB JULY 2014 NEW WAVEDATA INTERFACE RAFFAZZONATA
        public XYData getData(double xmin, double xmax, int numPoints) throws Exception
        {
            return getData(xmin, xmax, numPoints, false);
        }
        public XYData getData(double xmin, double xmax, int numPoints, boolean isLong) throws Exception
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.XYData("+xmin+", "+xmax+", "+numPoints+", "+isLong+")");}
            if (!CheckOpen(this.wd_experiment, this.wd_shot))
              return null;
            
            if(segmentMode == SEGMENTED_UNKNOWN)
            {
              Vector args = new Vector();
              args.addElement(new Descriptor(null, in_y));
              try {
                  byte[] retData = GetByteArray("byte(MdsMisc->IsSegmented($))", args);
            
                  if (retData[0] > 0)
                      segmentMode = SEGMENTED_YES;
                  else
                      segmentMode = SEGMENTED_NO;
              }catch(Exception exc)
              {// MdsMisc->IsSegmented failed
                segmentMode = SEGMENTED_NO;
              }
            }
//            String setTimeContext = getTimeContext(xmin,xmax,isLong);
            String setTimeContext = "";
            try{
                return getXYSignal(xmin, xmax, numPoints, isLong, setTimeContext);
            }catch(Exception exc)
            {
               if (DEGUB.ON){System.err.println("MdsMisc->GetXYSignal() is not available on the server\n:"+exc);}
            }
            if (DEGUB.ON){System.out.println("traditional method");}
            float y[] = GetFloatArray(setTimeContext+"("+v_y+")");
            RealArray xReal = GetRealArray("("+v_x+")");
            if(xReal.isLong)
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
        private String getTimeContext(double xmin, double xmax, boolean isLong)throws Exception
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.setTimeContext("+xmin+", "+xmax+", "+isLong+")");}
            String res;
            try{
                if(xmin == -Double.MAX_VALUE && xmax == Double.MAX_VALUE)
                    res = "SetTimeContext(*,*,*);";
                else if(xmin == -Double.MAX_VALUE)
                {
                    if(isLong)
                        res = "SetTimeContext(*, QUADWORD("+(long)xmax+"Q), *);";
                    else
                        res = "SetTimeContext(*, "+xmax+", *);";
                }
                else if(xmax == Double.MAX_VALUE)
                {
                    if(isLong)
                        res = "SetTimeContext(QUADWORD("+(long)xmin+"Q),*, *);";
                    else
                        res = "SetTimeContext("+xmin+",*, *);";
                }
                else
                {
                    if(isLong)
                        res = "SetTimeContext(QUADWORD("+(long)xmin+"Q),QUADWORD("+(long)xmax+"Q), *);";
                    else
                        res = "SetTimeContext("+xmin+","+xmax+", *);";
                }
            }
            catch(Exception exc)
            {
                if (DEGUB.ON){System.err.println("setTimeContext:\n:"+exc);}
                res = "";
            }
            return res;
        }
        private XYData getXYSignal(double xmin, double xmax, int numPoints, boolean isLong, String setTimeContext)throws Exception
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.getXYSignal("+xmin+", "+xmax+", "+numPoints+", "+isLong+", \""+setTimeContext+"\")");}
            XYData res = null;
            double maxX = 0;
            Vector args = new Vector();
            args.addElement(new Descriptor(null, in_y));
            args.addElement(new Descriptor(null, in_x));

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
            //all fine if setTimeContext is an empty string
            //if a space is required between ; and further code setTimeContext sould have it
            if (DEGUB.ON){System.out.println("MdsMisc->GetXYSignal*Long*Times:DSC");}
            if(isLong)
                retData = GetByteArray(setTimeContext+"MdsMisc->GetXYSignalLongTimes:DSC", args);
            else
                retData = GetByteArray(setTimeContext+"MdsMisc->GetXYSignal:DSC", args);
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
                y[i] = dis.readFloat();
            
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
/*          }
            catch(Exception exc)
            {
            if (DEGUB.ON){System.err.println("Error Reading data:\n"+exc);}
                nSamples = 0;
            }
*/          //Got resampled signal, if it is segmented and jScope.refreshPeriod > 0, enqueue a new request
            if(segmentMode == SEGMENTED_YES && continuousUpdate)
            {
                long refreshPeriod = jScopeFacade.getRefreshPeriod();
                if(refreshPeriod <= 0) refreshPeriod = 1000; //default 1 s refresh
                updateWorker.updateInfo(/*xmin*/maxX, Double.MAX_VALUE, 2000,
                    waveDataListenersV, this, isLong, refreshPeriod);
            }
            return res;
         }

         public XYData getData(int numPoints)throws Exception
         {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.getData("+numPoints+")");}
             return getData(-Double.MAX_VALUE, Double.MAX_VALUE, numPoints);
         }

        public float[] getZ()
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.getZ()");}
            try {
                return GetFloatArray(v_y);
            }catch(Exception exc){return null;}
        }
        
        private long x2DLong[];
        public double[] getX2D()
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.getX2D()");}
            try {
                RealArray realArray = GetRealArray("DIM_OF("+v_y+", 0)");
                if( realArray.isLong )
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
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.getX2DLong()");}
            return x2DLong;
        }
          
        public float[] getY2D()
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.getY2D()");}
            try {
                return GetFloatArray("DIM_OF("+v_y+", 1)");
            }catch(Exception exc){return null;}
        }

        //Cesare Mar 2015
        
        public float[] getX_Z()
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.getX_Z()");}
            try {
                return GetFloatArray("("+v_x+")");
            }catch(Exception exc){return null;}
        }
        public float[] getX_X2D()
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.getX_X2D()");}
            try {
                return GetFloatArray("DIM_OF("+v_x+", 0)");
            }catch(Exception exc){return null;}
        }
        public float[] getX_Y2D()
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.getX_Y2D()");}
            try {
                return GetFloatArray("DIM_OF("+v_x+", 1)");
            }catch(Exception exc){return null;}
        }
        //End
        
        
        //public double[] getXLimits(){System.out.println("BADABUM!!"); return null;}
        //public long []getXLong(){System.out.println("BADABUM!!"); return null;}
        public boolean isXLong(){return isXLong;}

        //Async update management
        Vector<WaveDataListener> waveDataListenersV = new Vector<WaveDataListener>();


        public void addWaveDataListener(WaveDataListener listener)
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.addWaveDataListener()");}
            waveDataListenersV.addElement(listener);
            if(asynchSource != null)
                asynchSource.addDataListener(listener);
        }
        public void getDataAsync(double lowerBound, double upperBound, int numPoints)
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.SimpleWaveData.getDataAsync("+lowerBound+", "+upperBound+", "+numPoints+")");}
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
        Vector<UpdateDescriptor>requestsV = new Vector<UpdateDescriptor>();
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
            if(updateTime > 0)  //If a delayed request for update
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
            if (DEGUB.ON){System.out.println("run()");}
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
                            if (DEGUB.ON){System.err.println(d+" Error in asynchUpdate:\n"+exc);}
                        }
                    }
                    else
                    {
                        if(nextTime == -1 || nextTime > currUpdate.updateTime) //It will alway be nextTime != -1
                            nextTime = currUpdate.updateTime;
                        i++;
                    }
                }
                if(nextTime != -1) //If a pending request for which time did not expire, schedure a new notification
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

    protected MdsConnection getConnection() 
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider(\""+provider+"\")");}
        return new MdsConnection();
    }

    protected void finalize()
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.finalize()");}
        int status;
        String err = new String("");
        if (open)
            mds.MdsValue("JavaClose(\"" + experiment + "\"," + shot + ")");
        if (connected)
            status = mds.DisconnectFromMds();
            if (DEGUB.ON){System.out.println(">> disconnected");}
    }
    //To be overridden by any DataProvider implementation with added dynamic generation
    AsynchDataSource getAsynchSource()
    {
        return null;
    }

    public void SetArgument(String arg) throws IOException
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.SetArgument("+arg+")");}
        setProvider(arg);
        mds.setProvider(provider);
    }

    private void setProvider(String arg)
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.setProvider("+arg+")");}
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
        if (DEGUB.ON){System.out.println("MdsDataProvider.SetCompression("+state+")");}
        if (connected)
            Dispose();
        use_compression = state;
    }

    protected String GetExperimentName(String in_frame)
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.GetExperimentName(\""+in_frame+"\")");}
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

    public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.GetFrameData(\""+in_y+"\", \""+in_x+"\", "+time_min+", "+time_max+")");}
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
        if (DEGUB.ON){System.out.println("MdsDataProvider.GetAllFrames("+in_frame+")");}
        byte img_buf[], out[] = null;
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
        img_buf = GetByteArray(in);
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
        DataOutputStream d = new DataOutputStream(b);

        d.writeInt(pixel_size);

        d.writeInt(shape[0]);
        d.writeInt(shape[1]);
        d.writeInt(num_time);

        for (int i = 0; i < num_time; i++)
            d.writeFloat(time[i]);

        d.write(img_buf);
        img_buf = null;
        out = b.toByteArray();
        d.close();

        return out;
    }

    public synchronized float[] GetFrameTimes(String in_frame)
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.GetFrameTimes(\""+in_frame+"\")");}
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
        if (DEGUB.ON){System.out.println("MdsDataProvider.GetFrameAt(\""+in_frame+"\", "+frame_idx+")");}
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
        if (DEGUB.ON){System.out.println("MdsDataProvider.GetByteArray(\""+in+"\", "+args+")");}
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
        if (DEGUB.ON){System.out.println("MdsDataProvider.Update(\""+experiment+"\", "+shot+", "+resetExperiment+")");}
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
        }
    }

    public synchronized String GetString(String in) throws IOException
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.GetString(\""+in+"\")");}
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
        if (DEGUB.ON){System.out.println("MdsDataProvider.SetEnvironment(\""+in+"\")");}
        if (in == null || in.length() == 0)
            return;

        Properties pr = new Properties();
        pr.load(new ByteArrayInputStream(in.getBytes()));
        String def_node = ( (String) pr.getProperty("__default_node"));
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
        if (DEGUB.ON){System.out.println("MdsDataProvider.SetEnvironmentSpecific(\""+in+"\")");}
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
        if (DEGUB.ON){System.out.println("MdsDataProvider.GetNow(\""+in+"\")");}
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
        if (DEGUB.ON){System.out.println("MdsDataProvider.GetFloat(\""+in+"\")");}
        error = null;
        //First check Whether this is a date
        try {
            Calendar cal = Calendar.getInstance();
            //cal.setTimeZone(TimeZone.getTimeZone("GMT+00"));
            DateFormat df = new SimpleDateFormat("d-MMM-yyyy HH:mm Z");
            //DateFormat df = new SimpleDateFormat("d-MMM-yyyy HH:mm");-
            Date date = df.parse(in + " GMT");
            //Date date = df.parse(in);
            cal.setTime(date);
            long javaTime = cal.getTime().getTime();
            return javaTime;
        }catch(Exception exc1) //If exception occurs this is not a date, try NOW condtruction
        {
            try {
                return GetNow(in);
            }catch(Exception exc2){if (DEGUB.ON){System.err.println("MdsDataProvider.GetFloat:\n"+exc2);}}
        }

        if (NotYetNumber(in))
        {
            if (!CheckOpen())
                return 0;
            Descriptor desc = mds.MdsValue(in);
            if (desc.error != null)
                error = desc.error;
            switch (desc.dtype)
            {
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
            return new Float(in).floatValue();
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
        if (DEGUB.ON){System.out.println("MdsDataProvider.GetRealArray(\""+in+"\")");}
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
                error = "Data type code : " + desc.dtype + " not yet supported ";
        }
        return out;
    }


    public long[] GetShots(String in) throws IOException
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.GetShots(\""+in+"\")");}
        //To shot evaluation don't execute check
        //if a pulse file is open
        CheckConnection();
        return GetLongArray(in);
    }

    public int[] GetIntArray(String in) throws IOException
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.GetIntArray(\""+in+"\")");}
        if (!CheckOpen())
            throw new IOException("Tree not open");
        return GetIntegerArray(in);
    }

    private synchronized long[] GetLongArray(String in) throws IOException
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.GetLongArray(\""+in+"\")");}
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
                error = "Data type code : " + desc.dtype + " not yet supported ";
        }
        throw new IOException(error);
    }

    private synchronized int[] GetIntegerArray(String in) throws IOException
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.GetIntegerArray(\""+in+"\")");}
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
                error = "Data type code : " + desc.dtype + " not yet supported ";
        }
        throw new IOException(error);
    }

    public synchronized void Dispose()
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.Dispose()");}

        if (is_tunneling && ssh_tunneling != null)
           ssh_tunneling.Dispose();
        
        if (connected)
        {
            connected = false;
            mds.DisconnectFromMds();
            if (DEGUB.ON){System.out.println(">> disconnected");}
            ConnectionEvent ce = new ConnectionEvent(this,
                ConnectionEvent.LOST_CONNECTION,
                "Lost connection from : " + provider);
            mds.dispatchConnectionEvent(ce);
        }

        if( updateWorker != null && updateWorker.isAlive() )
        {
            updateWorker.stopUpdateWorker();
        }
    }

    protected synchronized void CheckConnection() throws IOException
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.CheckConnection()");}
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
            if (DEGUB.ON){System.out.println("connected");}
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
        if (DEGUB.ON){System.out.println("MdsDataProvider.CheckOpen(\""+experiment+"\", "+shot+")");}
        int status;
        if (!connected)
        {
            status = mds.ConnectToMds(use_compression);
            if (status == 0)
            {
                if (mds.error != null)
                    throw new IOException("Cannot connect to data server : " + mds.error);
                else
                    error = "Cannot connect to data server";
                return false;
            }
            if (DEGUB.ON){System.out.println(">> connected");}
            connected = true;
            updateWorker = new UpdateWorker();
            updateWorker.start();

        }
        if (!open && experiment != null || this.shot != shot || experiment != null && !experiment.equalsIgnoreCase(this.experiment) )
        {
            //System.out.println("\n-->\nOpen tree "+experiment+ " shot "+ shot +"\n<--\n");
            Descriptor descr = mds.MdsValue("JavaOpen(\"" + experiment + "\"," + shot + ")");
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
                        error = "Public variable evaluation error " + experiment + " shot " + shot + " : " + error;
                        return false;
                    }
                }
            }
            else
            {
                if (mds.error != null)
                    error = "Cannot open experiment " + experiment + " shot " + shot + " : " + mds.error;
                else
                    error = "Cannot open experiment " + experiment + " shot " + shot;
                return false;
            }
        }
        if (open && def_node_changed)
        {
            Descriptor descr;
            if (default_node != null)
            {
                descr = mds.MdsValue("TreeSetDefault(\"\\\\" + default_node + "\")");
                if( (descr.int_data[0] & 1 ) == 0   )
                    mds.MdsValue("TreeSetDefault(\"\\\\" + experiment + "::TOP\")");
            }
            else
                descr = mds.MdsValue("TreeSetDefault(\"\\\\" + experiment + "::TOP\")");

            def_node_changed = false;
        }
        return true;
    }

    protected boolean NotYetString(String in)
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.NotYetString(\""+in+"\")");}
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
        if (DEGUB.ON){System.out.println("MdsDataProvider.NotYetNumber(\""+in+"\")");}
        boolean ris;
        ris = false;
        try
        {
            Float f = new Float(in);
        }
        catch (NumberFormatException e)
        {
            ris = true;
        }
        return ris;
    }

    public synchronized void AddUpdateEventListener(UpdateEventListener l, String event_name) throws IOException
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.AddUpdateEventListener(l,"+event_name+")");}
        int eventid;
        String error;

        if (event_name == null || event_name.trim().length() == 0)
            return;
        CheckConnection();
        mds.MdsSetEvent(l, event_name);
    }

    public synchronized void RemoveUpdateEventListener(UpdateEventListener l, String event_name) throws IOException
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.RemoveUpdateEventListener(l,"+event_name+")");}
        int eventid;
        String error;

        if (event_name == null || event_name.trim().length() == 0)
            return;
        CheckConnection();
        mds.MdsRemoveEvent(l, event_name);
    }

    public synchronized void AddConnectionListener(ConnectionListener l)
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.AddConnectionListener(l)");}
        if (mds == null)
        {
            return;
        }
        mds.addConnectionListener(l);
    }

    public synchronized void RemoveConnectionListener(ConnectionListener l)
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.RemoveConnectionListener(l)");}
        if (mds == null)
        {
            return;
        }
        mds.removeConnectionListener(l);
    }

    protected void DispatchConnectionEvent(ConnectionEvent e)
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.DispatchConnectionEvent(e)");}
        if (mds == null)
        {
            return;
        }
        mds.dispatchConnectionEvent(e);
    }

    public boolean SupportsTunneling()
    {
        return true;
    }


    public int InquireCredentials(JFrame f, DataServerItem server_item)
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.InquireCredentials(f,server_item)");}
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

    protected String GetStringValue(String expression) throws IOException
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.GetStringValue(\""+expression+"\")");}
        String out = GetString(expression);
        if (out == null || out.length() == 0 || error != null)
        {
            error = null;
            return null;
        }
        if(out.indexOf(0) > 0 )
          out = out.substring(0, out.indexOf(0));

        return out;
    }

    protected int[] GetNumDimensions(String expression) throws IOException
    {
        if (DEGUB.ON){System.out.println("MdsDataProvider.GetNumDimensions(\""+expression+"\")");}
        //return GetIntArray(in_y);
        //Gabriele June 2013: reduce dimension if one component is 1
        int [] fullDims = GetIntArray("shape("+expression+")");
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
            if (DEGUB.ON){System.out.println("MdsDataProvider.RealArray(floatArray)");}
            this.floatArray = floatArray;
            isDouble = false;
            isLong = false;
        }

        RealArray(double[] doubleArray)
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.RealArray(doubleArray)");}
            this.doubleArray = doubleArray;
            isDouble = true;
            isLong = false;
        }

        RealArray(long[] longArray)
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.RealArray(longArray)");}
            this.longArray = longArray;
            for(int i = 0; i < longArray.length; i++)
                longArray[i] = jScopeFacade.convertFromSpecificTime(longArray[i]);
            
            isDouble = false;
            isLong = true;
        }

        float[] getFloatArray()
        {
            if (DEGUB.ON){System.out.println("MdsDataProvider.RealArray.getFloatArray()");}
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
            if (DEGUB.ON){System.out.println("MdsDataProvider.RealArray.getDoubleArray()");}
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
            if (DEGUB.ON){System.out.println("MdsDataProvider.RealArray.getLongArray()");}
            if(isDouble) return null;
            return longArray;
        }
    }
}

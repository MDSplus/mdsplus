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

public class MdsDataProvider
    implements DataProvider
{
    String provider;
    String experiment;
    String default_node;
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
    static final int MAX_PIXELS = 2000;

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
            if(dims.length != 3)
                throw new IOException("Invalid number of segment dimensions: "+ dims.length);
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
        private Dimension dim = null;
        private int header_size = 0;

        public SimpleFrameData(String in_y, String in_x, float time_min,
                               float time_max) throws IOException
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
                    all_times = MdsDataProvider.this.GetWaveData(in_x).
                        GetFloatData();
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
                    all_times = MdsDataProvider.this.GetWaveData(in_x).
                        GetFloatData();

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

    class SimpleWaveData
        implements WaveData
    {
        String in_x, in_y;
        double xmax, xmin;
        int n_points;
        double waveMin, waveMax;
        boolean resample = false;
        boolean _jscope_set = false;
        static final int SEGMENTED_YES = 1, SEGMENTED_NO = 2, SEGMENTED_UNKNOWN = 3;
        static final int UNKNOWN = -1;
        int numDimensions = UNKNOWN;
        int segmentMode = SEGMENTED_UNKNOWN;
        int v_idx;

        public SimpleWaveData(String in_y)
        {
            this.in_y = in_y;
            v_idx = var_idx;
            if(segmentMode == SEGMENTED_UNKNOWN)
            {
                try {
                    int[] numSegments = GetIntArray("GetNumSegments("+in_y+")");
                    if(numSegments[0] > 0)
                        segmentMode = SEGMENTED_YES;
                    else
                        segmentMode = SEGMENTED_NO;
                }
                catch(Exception exc)
                {
                    error = null;
                    segmentMode = SEGMENTED_UNKNOWN;
                }
            }
         }

        public SimpleWaveData(String in_y, String in_x)
        {
            this.in_y = in_y;
            this.in_x = in_x;
            v_idx = var_idx;
            if(segmentMode == SEGMENTED_UNKNOWN)
            {
                try {
                    int[] numSegments = GetIntArray("GetNumSegments("+in_y+")");
                    if(numSegments[0] > 0)
                        segmentMode = SEGMENTED_YES;
                    else
                        segmentMode = SEGMENTED_NO;
                }catch(Exception exc)
                {
                    error = null;
                    segmentMode = SEGMENTED_UNKNOWN;
                }
            }
        }

        public SimpleWaveData(String in_y, double xmin, double xmax, int n_points)
        {
            resample = true;
            if(xmin == -1E8 && xmax == 1E8) //If no limits explicitly set
            {
                try {
                    int limits[];
                    if(in_y.startsWith("\\"))
                        limits = GetIntArray("JavaGetMinMax(\"\\"+in_y+"\")");
                    else
                        limits = GetIntArray("JavaGetMinMax(\""+in_y+"\")");
                    this.xmin = this.waveMin = limits[0];
                    this.xmax = this.waveMax = limits[1];
                    
                }catch(IOException exc){System.out.println("Cannot get Min and Max of for signel " + in_y);}
            }
            else
            {
                try {
                    int limits[];
                    if(in_y.startsWith("\\"))
                        limits = GetIntArray("JavaGetMinMax(\"\\"+in_y+"\")");
                    else
                        limits = GetIntArray("JavaGetMinMax(\""+in_y+"\")");
                    this.waveMin = limits[0];
                    this.waveMax = limits[1];
                }catch(IOException exc)
                {
                    System.out.println("Cannot get Min and Max of for signel " + in_y);
                    this.waveMin = xmin;
                    this.waveMax = xmax;
                }
                if(xmin >= waveMin)
                    this.xmin = xmin;
                else
                    this.xmin = waveMin;
                if(xmax <= waveMax)
                    this.xmax = xmax;
                else
                    this.xmax = waveMax;
            }
            this.in_y = in_y;
            this.n_points = n_points;
            v_idx = var_idx;
            if(segmentMode == SEGMENTED_UNKNOWN)
            {
                try {
                    int[] numSegments = GetIntArray("GetNumSegments("+in_y+")");
                    if(numSegments[0] > 0)
                        segmentMode = SEGMENTED_YES;
                    else
                        segmentMode = SEGMENTED_NO;
                }catch(Exception exc)
                {
                    error = null;
                    segmentMode = SEGMENTED_UNKNOWN;
                }
            }
 //            setResampleLimits(this.xmin, this.xmax);
        }

        public SimpleWaveData(String in_y, String in_x, double xmin, double xmax,
                              int n_points)
        {
           resample = true;
            this.in_y = in_y;
            this.in_x = in_x;
            this.xmin = xmin;
            this.xmax = xmax;
            this.n_points = n_points;
            v_idx = var_idx;
            if(segmentMode == SEGMENTED_UNKNOWN)
            {
                try {
                    int[] numSegments = GetIntArray("GetNumSegments("+in_y+")");
                    if(numSegments[0] > 0)
                        segmentMode = SEGMENTED_YES;
                    else
                        segmentMode = SEGMENTED_NO;
                }catch(Exception exc)
                {
                    error = null;
                    segmentMode = SEGMENTED_UNKNOWN;
                }
            }
//            setResampleLimits(xmin, xmax);
        }

        public int GetNumDimension() throws IOException
        {
            if(numDimensions != UNKNOWN)
                return numDimensions;
//            if(resample)
//                setResampleLimits(this.xmin, this.xmax, in_y);
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
                    expr = "( _jscope_" + v_idx + " = (" + in_y +"), shape(_jscope_" + v_idx + "))";
                    var_idx+=2;
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
//            if(resample)
//                resetResampleLimits();
            numDimensions = shape.length;
            return shape.length;
        }

        public float[] GetFloatData() throws IOException
        {
            boolean useResample = false;
            if(resample)
                useResample = setResampleLimits(this.xmin, this.xmax, in_y);

            String in_y_expr = "_jscope_" + v_idx;
            String set_tdivar = "";
            if (!_jscope_set)// || resample)
            {
                _jscope_set = true;
//Classic resampling is carried out by
//MDSplus itself (works also for expressions)
// JavaResample will provide min-max pairs. Still experimental, not used now.
                //if(resample && segmentMode == SEGMENTED_YES && useResample)
                //    set_tdivar = "_jscope_" + v_idx + " = JavaResample(" + in_y + "), ";
                //else
                    set_tdivar = "_jscope_" + v_idx + " = (" + in_y + "), ";
                var_idx+=2;
            }

            float[] res = GetFloatArray(set_tdivar + "fs_float(" + in_y_expr + ")");
            if(resample)
                resetResampleLimits();
            return res;
        }

        private double[] encodeTimeBase(String expr)
        {
            try
            {
                double t0 = GetFloat("dscptr(window_of(dim_of(" + expr + ")),2)");
                int startIdx[] = GetIntArray("begin_of(window_of(dim_of(" +
                                             expr + ")))");
                int endIdx[] = GetIntArray("end_of(window_of(dim_of(" + expr +
                                           ")))");

                if (startIdx.length != 1 || endIdx.length != 1)
                    return null;

                int numPoint = endIdx[0] - startIdx[0] + 1;
                double delta[] = GetDoubleArray("slope_of(axis_of(dim_of(" +
                                                expr + ")))");
                double begin[] = null;
                double end[] = null;
                double curr;
                double firstTime[] = GetDoubleArray("i_to_x(dim_of(" + expr +
                    ")," + startIdx[0] + ")");
                try
                {
                    begin = GetDoubleArray("begin_of(axis_of(dim_of(" + expr +
                                           ")))");
                    end = GetDoubleArray("end_of(axis_of(dim_of(" + expr +
                                         ")))");
                }
                catch (IOException e)
                {}

                if (delta.length == 1 && numPoint > 1)
                {
                    int i, j;
                    double out[] = new double[numPoint];

                    for (i = j = 0, curr = firstTime[0]; i < numPoint; i++, j++)
                        out[i] = curr + j * delta[0];

                    return out;
                }

                if (delta.length > 1 && numPoint > 1)
                {
                    int i, j, idx;
                    double out[] = new double[numPoint];

                    for (i = j = 0, idx = 0, curr = firstTime[0]; i < numPoint;
                         i++, j++)
                    {
                        out[i] = curr + j * delta[idx];
                        if (out[i] > end[idx])
                        {
                            out[i] = end[idx];
                            idx++;
                            curr = begin[idx];
                            j = 0;
                        }
                    }
                    return out;

                }
            }
            catch (Exception exc)
            {}
            ; //System.out.println(exc.getMessage());}
            return null;

        }

        RealArray currXData = null;

        public double[] GetXDoubleData()
        {
            try
            {
                if (currXData == null)
                    currXData = GetXRealData();
                if (!currXData.isDouble())
                    return null;
                return currXData.getDoubleArray();
            }
            catch (Exception exc)
            {
                return null;
            }
        }

        public long[] GetXLongData()
        {
            try
            {
                if (currXData == null)
                    currXData = GetXRealData();
                if (!currXData.isLong())
                    return null;
                return currXData.getLongArray();
            }
            catch (Exception exc)
            {
                return null;
            }
        }

        public float[] GetXData()
        {
            try
            {
                if (currXData == null)
                    currXData = GetXRealData();
                return currXData.getFloatArray();
            }
            catch (Exception exc)
            {
                return null;
            }
        }


        RealArray GetXRealData() throws IOException
        {
            String expr = null;
            boolean isCoded = false;
            double tBaseOut[] = null;




            if (in_x == null)
            {

                if (_jscope_set)
                {
                    expr = "dim_of(_jscope_" + v_idx + ")";
                 }
                else
                {
                    _jscope_set = true;
                    String in_y_expr = "_jscope_" + v_idx;
                    String set_tdivar = "( _jscope_" + v_idx + " = (" + in_y +
                        "), ";
                    var_idx+=2;
                    expr = set_tdivar + "dim_of(" + in_y_expr + ")";
                    tBaseOut = encodeTimeBase(in_y);
                }
                if (tBaseOut != null)
                    return new RealArray(tBaseOut);
                else
                    return GetRealArray(expr);
            }
            else
                return GetRealArray(in_x);
        }

        public float[] GetYData() throws IOException
        {
            String expr;
            if (_jscope_set)
                expr = "dim_of(_jscope_" + v_idx + ", 1)";
            else
            {
                _jscope_set = true;
                expr = "( _jscope_" + v_idx + " = (" + in_y +
                    "), dim_of(_jscope_" + v_idx + ", 1))";
                var_idx+=2;
            }
            return GetFloatArray(expr);
       }

        public String GetTitle() throws IOException
        {
            String expr;
            String out;
            if (_jscope_set)
            {
                expr = "help_of(_jscope_" + v_idx + ")";
                out = GetStringValue(expr);
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
                    var_idx+=2;
                }
 //               if(resample)
 //                   setResampleLimits(xmin, xmax, in_y);
                out = GetStringValue(expr);
//                if(resample)
//                    resetResampleLimits();

            }
           return out;
            //return GetDefaultTitle(in_y);
        }

        public String GetXLabel() throws IOException
        {
            String out = null;

            if (in_x == null || in_x.length() == 0)
            {
                String expr;
                if (_jscope_set)
                {
                    expr = "Units(dim_of(_jscope_" + v_idx + "))";
                    out = GetStringValue(expr);
                }
                else
                {
                    if(segmentMode == SEGMENTED_YES)
                    {
                        expr = "Units(dim_of(GetSegment(" + in_y + ", 0)))";
                        out = GetStringValue(expr);
                   }
                    else
                    {
                        _jscope_set = true;
//                        if(resample)
//                            setResampleLimits(xmin, xmax, in_y);
                        expr = "( _jscope_" + v_idx + " = (" + in_y + "), Units(dim_of(_jscope_" + v_idx + ")))";
                        var_idx+=2;
                        out = GetStringValue(expr);
//                        if(resample)
//                            resetResampleLimits();
                    }
                }
            }
            else
            {
                out = GetStringValue("Units(" + in_x + ")");
            }
            return out;
        }

        
        public String GetYLabel() throws IOException
        {
                        
            String expr;
            String out;
           
            if( GetNumDimension() > 1)
            {
                if(segmentMode == SEGMENTED_YES)
                {
                    expr = "Units(dim_of(GetSegment(" + in_y + ", 1)))";
                    out = GetStringValue(expr);
                 }
                else
                {
                    if (_jscope_set)
                    {
                        expr = "Units(dim_of(_jscope_" + v_idx + ", 1))";
                        out = GetStringValue(expr);
                    }
                    else
                    {
                        _jscope_set = true;
                        expr = "( _jscope_" + v_idx + " = (" + in_y +
                        "), Units(dim_of(_jscope_" + v_idx + ", 1)))";
                        var_idx+=2;
//                        if(resample)
//                            setResampleLimits(xmin, xmax, in_y);
                        out = GetStringValue(expr);
//                        if(resample)
//                            resetResampleLimits();
                    }
                }
               return out;
            }
            if(segmentMode == SEGMENTED_YES)
            {
                expr = "Units(dim_of(GetSegment(" + in_y + ", 0)))";
                out = GetStringValue(expr);
            }
            else
            {
                if (_jscope_set)
                {
                    expr = "Units(_jscope_" + v_idx + ")";
                    out = GetStringValue(expr);
                }
                else
                {
                    _jscope_set = true;
                    expr = "( _jscope_" + v_idx + " = (" + in_y +
                        "), Units(_jscope_" + v_idx + "))";
                    var_idx+=2;
//                    if(resample)
//                        setResampleLimits(xmin, xmax, in_y);
                    out = GetStringValue(expr);
//                    if(resample)
//                        resetResampleLimits();
                }
            }
            return out;
            //return GetDefaultYLabel(in_y);
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
                var_idx+=2;
            }
            
            //String out = GetDefaultZLabel(expr);
            String out = GetStringValue(expr);
            if (out == null)
                _jscope_set = false;

            return out;
            //           return GetDefaultZLabel(in_y);
        }
    }

    public MdsDataProvider()
    {
        experiment = null;
        shot = 0;
        open = connected = false;
        mds = new MdsConnection();
        error = null;
    }

    public MdsDataProvider(String provider)
    {
        setProvider(provider);
        experiment = null;
        shot = 0;
        open = connected = false;
        mds = new MdsConnection(this.provider);
        error = null;
    }

    public MdsDataProvider(String exp, int s)
    {
        experiment = exp;
        shot = 0;
        open = connected = false;
        mds = new MdsConnection();
        error = null;
    }

    protected void finalize()
    {
        int status;
        String err = new String("");
        if (open)
            mds.MdsValue("JavaClose(\"" + experiment + "\"," + shot + ")");
        if (connected)
            status = mds.DisconnectFromMds();
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
        int[] numSegments = GetIntArray("GetNumSegments("+in_y+")");
        if(numSegments != null && numSegments[0] > 0)
            return new SegmentedFrameData(in_y, in_x, time_min, time_max, numSegments[0]);
        else
            return (new SimpleFrameData(in_y, in_x, time_min, time_max));
    }

    public synchronized byte[] GetAllFrames(String in_frame) throws IOException
    {
        byte img_buf[], out[] = null;
        float time[];
        int shape[];
        int pixel_size = 8;
        int num_time = 0;

        if (!CheckOpen())
            return null;

        String in;

        in = "DIM_OF( _jScope_img = " + in_frame + ", 2)";
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

    public synchronized byte[] GetByteArray(String in) throws IOException
    {
        byte out_byte[] = null;
        ByteArrayOutputStream dosb = new ByteArrayOutputStream();
        DataOutputStream dos = new DataOutputStream(dosb);

        if (!CheckOpen())
            return null;
        Descriptor desc = mds.MdsValue(in);
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
                return null;
        }
        return null;
    }

    public synchronized String ErrorString()
    {
        return error;
    }

    public synchronized void Update(String exp, long s)
    {
        error = null;
        var_idx = 0;
        /*
                 if(exp == null || exp.length() == 0)
                 {
            experiment = null;
            open = true;
            shot = s;
            return;
                 }
         */
        if (s != shot || s == 0 || experiment == null ||
            experiment.length() == 0 || !experiment.equals(exp))
        {
            //  System.out.println("Close "+experiment+ " "+shot);
            experiment = ( (exp != null && exp.trim().length() > 0) ? exp : null);
            shot = s;
            open = false;
            //  System.out.println("Open "+experiment+ " "+s);
        }
    }

    public synchronized void Update(String experiment, long shot, boolean resetExperiment)
    {
        this.error = null;
        this.var_idx = 0;

        if (resetExperiment) {
          this.experiment = null;
        }
        if ((shot != this.shot) || (shot == 0L) || (this.experiment == null) || (this.experiment.length() == 0) || (!this.experiment.equals(experiment)))
        {
          this.experiment = ((experiment != null) && (experiment.trim().length() > 0) ? experiment : null);
          this.shot = shot;
          this.open = false;
        }
    }

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
                    return null;
                case Descriptor.DTYPE_CSTRING:
                    if ( (desc.status & 1) == 1)
                        return desc.strdata;
                    else
                        return (error = desc.error);
            }
            return null;
        }
        else
            return new String(in.getBytes(), 1, in.length() - 2);
    }

    public synchronized void SetEnvironment(String in) throws IOException
    {

        if (in == null || in.length() == 0)
            return;

        if(in.indexOf("pulseSetVer") != 0)
        {
            open = false;
        }

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
                return;
            }
        }
        error = null;
        //Update(null , 0);
        if (!CheckOpen())
        {
            error = "Cannot connetion to data server";
            return;
        }
        SetEnvironmentSpecific(in, default_node);
    }

    void SetEnvironmentSpecific(String in, String defaultNode)
    {

        Descriptor desc = mds.MdsValue(in);
        switch (desc.dtype)
        {
            case Descriptor.DTYPE_CSTRING:
                if ( (desc.status & 1) == 0)
                    error = desc.error;
        }
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
            //DateFormat df = new SimpleDateFormat("d-MMM-yyyy HH:mm");-
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

    private float[] ExpandTimes(float codedTime[])
    {
        float[] expandedTime = null;

        if (codedTime[0] > 0)
        { //JavaDim decided to apply coding
            int max_len = 0;
            int num_blocks = (codedTime.length - 1) / 3;
            //each block codes start, end, delta
            int out_idx, in_idx, curr_block = 0;
            float curr_time;

            for (curr_block = 0; curr_block < num_blocks; curr_block++)
                max_len +=
                    (int) ( (codedTime[curr_block * 3 + 2] -
                             codedTime[curr_block * 3 + 1]) /
                           codedTime[curr_block * 3 + 3] + 0.5);

            expandedTime = new float[max_len];

            for (curr_block = 0, out_idx = 0;
                 out_idx < max_len && curr_block < num_blocks;
                 curr_block++)
            {
                for (in_idx = 0; out_idx < max_len; in_idx++)
                {
                    curr_time = codedTime[curr_block * 3 + 1] +
                        in_idx * codedTime[curr_block * 3 + 3];
                    if (curr_time > codedTime[curr_block * 3 + 2])
                        break;
                    expandedTime[out_idx++] = curr_time;
                }
            }
        }
        else
        { //JavaDim did not apply coding
            expandedTime = new float[codedTime.length - 1];
            System.arraycopy(codedTime, 1, expandedTime, 0, expandedTime.length);
        }
        return expandedTime;
    }

    public WaveData GetWaveData(String in)
    {
        return new SimpleWaveData(in);
    }

    public WaveData GetWaveData(String in_y, String in_x)
    {
        return new SimpleWaveData(in_y, in_x);
    }

    public WaveData GetResampledWaveData(String in, double start, double end,
                                         int n_points)
    {
        return new SimpleWaveData(in, start, end, n_points);
    }

    public WaveData GetResampledWaveData(String in_y, String in_x, double start,
                                         double end, int n_points)
    {
        return new SimpleWaveData(in_y, in_x, start, end, n_points);
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
        RealArray out = null;

        ConnectionEvent e = new ConnectionEvent(this, 1, 0);
        DispatchConnectionEvent(e);

        if (!CheckOpen())
            return null;

        Descriptor desc = mds.MdsValue(in);
        switch (desc.dtype)
        {
            case Descriptor.DTYPE_FLOAT:
                out = new RealArray(desc.float_data);
                break;
            case Descriptor.DTYPE_DOUBLE:
                out = new RealArray(desc.double_data);
                break;
            case Descriptor.DTYPE_LONG:
            {
                float[] outF = new float[desc.int_data.length];
                for (int i = 0; i < desc.int_data.length; i++)
                    outF[i] = (float) desc.int_data[i];
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
        try
        {

            return GetLongArray(in);
        }
        catch (Exception exc)
        {
            return null;
        }
    }

    public int[] GetIntArray(String in) throws IOException
    {
        if (!CheckOpen())
            return null;
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
                return null;
            default:
                error = "Data type code : " + desc.dtype +
                    " not yet supported ";
        }

        return null;
    }


    boolean setResampleLimits(double min, double max, String inY)
    {
        String limitsExpr;
        boolean useResample;
//November 2011
        if(Math.abs(min) > RESAMPLE_TRESHOLD || Math.abs(max) > RESAMPLE_TRESHOLD)
        {
            long maxSpecific = jScopeFacade.convertToSpecificTime((long)max);
            long minSpecific = jScopeFacade.convertToSpecificTime((long)min);

            long dt = ((long)maxSpecific - (long)minSpecific)/MAX_PIXELS;
            limitsExpr = "JavaSetResampleLimits("+minSpecific+"UQ,"+maxSpecific+"UQ,"+dt+"UQ)";
            useResample = true;
        }
        else
        {
            double dt = (max - min) / MAX_PIXELS;
            String numPointsExpr;
            if(inY.startsWith("\\"))
                numPointsExpr = "JavaGetNumPoints(\"\\"+inY+"\","+min+","+max+","+MAX_PIXELS+")";
            else
                numPointsExpr = "JavaGetNumPoints(\""+inY+"\","+min+","+max+","+MAX_PIXELS+")";
            int numPoints[];
            try {
                numPoints = GetIntArray(numPointsExpr);
            }catch(Exception exc){numPoints = new int[0];}
            if(numPoints.length > 0 && numPoints[0] < MAX_PIXELS)
            {
               limitsExpr = "JavaSetResampleLimits("+min + "," +max+",0Q)";
               useResample = false;
            }
            else
            {
               limitsExpr = "JavaSetResampleLimits("+min + "," +max+","+dt+")";
               useResample = true;
            }
        }
        mds.MdsValue(limitsExpr);
        return useResample;
    }
    void resetResampleLimits()
    {
       //mds.MdsValue("TreeShr->TreeResetTimeContext()");
       mds.MdsValue("JavaSetResampleLimits(0,0,0)");
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
                return null;
            default:
                error = "Data type code : " + desc.dtype +
                    " not yet supported ";
        }
        return null;
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
                connected = true;
        }
    }

    protected synchronized boolean CheckOpen() throws IOException
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
        }
        if (!open && experiment != null)
        {
            //System.out.println("Open tree "+experiment+ " shot "+ shot);
            Descriptor descr = mds.MdsValue("JavaOpen(\"" + experiment + "\"," +
                                            shot + ")");
            if (descr.dtype != Descriptor.DTYPE_CSTRING
                && descr.dtype == Descriptor.DTYPE_LONG && descr.int_data != null
                && descr.int_data.length > 0 && (descr.int_data[0] % 2 == 1))
            {
                /*
                              if(default_node != null)
                     descr = mds.MdsValue("TreeSetDefault(\""+default_node+"\")");
                              else
                     descr = mds.MdsValue("TreeSetDefault(\"\\\\"+experiment+"::TOP\")");
                 */
                open = true;
                def_node_changed = true;
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
                //return true;
            }
        }
        if (open && def_node_changed)
        {
            Descriptor descr;
            String set_node;
            if (default_node != null)
            {
                descr = mds.MdsValue("TreeSetDefault(\"" + default_node + "\")");
            }
            else
                descr = mds.MdsValue("TreeSetDefault(\"\\\\" + experiment +
                                     "::TOP\")");

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

    public synchronized void AddUpdateEventListener(UpdateEventListener l,
        String event_name) throws IOException
    {

        int eventid;
        String error;

        if (event_name == null || event_name.trim().length() == 0)
            return;
        CheckConnection();
        mds.MdsSetEvent(l, event_name);
    }

    public synchronized void RemoveUpdateEventListener(UpdateEventListener l,
        String event_name) throws IOException
    {
        int eventid;
        String error;

        if (event_name == null || event_name.trim().length() == 0)
            return;
        CheckConnection();
        mds.MdsRemoveEvent(l, event_name);
    }

    public synchronized void AddConnectionListener(ConnectionListener l)
    {
        if (mds == null)
        {
            return;
        }
        mds.addConnectionListener(l);
    }

    public synchronized void RemoveConnectionListener(ConnectionListener l)
    {
        if (mds == null)
        {
            return;
        }
        mds.removeConnectionListener(l);
    }

    protected void DispatchConnectionEvent(ConnectionEvent e)
    {
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

    public boolean SupportsContinuous()
    {
        return false;
    }

    public boolean DataPending()
    {
        return false;
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

/*
    protected String GetDefaultTitle(String in_y) throws IOException
    {
        //String out = GetString("help_of("+in_y+")");
        String out = GetString(in_y);
        if (out == null || out.length() == 0 || error != null)
        {
            error = null;
            return null;
        }
        out = out.substring(0, out.indexOf(0));

        return out;
    }

    protected String GetDefaultXLabel(String in_y) throws IOException
    {
        //String out = GetString("Units(dim_of("+in_y+"))");
        String out = GetString(in_y);
        if (out == null || out.length() == 0 || error != null)
        {
            error = null;
            return null;
        }
        return out;
    }

    protected String GetDefaultYLabel(String in_y) throws IOException
    {
        //String out = GetString("Units("+in_y+")");

        String out = GetString(in_y);
        if (out == null || out.length() == 0 || error != null)
        {
            error = null;
            return null;
        }
        return out;
    }

    protected String GetDefaultZLabel(String in_y) throws IOException
    {
        //      String out = GetString("Units(dim_of("+in_y+", 1))");
        String out = GetString(in_y);
        if (out == null || out.length() == 0 || error != null)
        {
            error = null;
            return null;
        }
        return out;
    }
*/
    protected int[] GetNumDimensions(String in_y) throws IOException
    {
        return GetIntArray(in_y);
        //return GetIntArray("shape("+in_y+")");
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

}

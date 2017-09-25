/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
package jScope;

/* $Id$ */
import jScope.ConnectionListener;
import javax.swing.JFrame;
import java.io.IOException;
import java.util.*;
import java.awt.*;
import java.text.SimpleDateFormat;
import java.text.DateFormat;

public class LocalDataProvider extends MdsDataProvider /* implements DataProvider */
{
    Vector<EventDescriptor> listeners = new Vector<>();
    Vector<String> eventNames = new Vector<>();

    static class EventDescriptor
    {
        UpdateEventListener listener;
        String event;
        int evId;
        EventDescriptor(UpdateEventListener listener, String event, int evId)
        {
            this.listener = listener;
            this.event = event;
            this.evId = evId;

        }
        public boolean equals(Object obj)
        {
            if(!(obj instanceof EventDescriptor)) return false;
            EventDescriptor evDescr = (EventDescriptor)obj;
            return listener == evDescr.getListener() && event.equals(evDescr.getEvent());
        }
        UpdateEventListener getListener() {return listener;}
        String getEvent() {return event;}
        int getEvId() {return evId;}
    } //EventDescriptor


    static native boolean isSegmentedNode(String nodeName);
    static native byte[] getSegment(String nodeName, int segIdx, int segOffset);
    static native byte[] getAllFrames(String nodeName, int startIdx, int endIdx);
    static native LocalDataProviderInfo getInfo(String nodeName, boolean isSegmented); //returned: width, height,  bytesPerPixel
    static native float[] getSegmentTimes(String nodeName, String timeNames, float timeMin, float timeMax);
    static native float[] getAllTimes(String nodeName, String timeNames);
    static native int[] getSegmentIdxs(String nodeName, float timeMin, float timeMax);
    class LocalFrameData implements FrameData
    {
        //If the frames are stored in non segmented data, all the frames are read at the same time
        //otherwise they are read when needed
        boolean isSegmented;
        String nodeName;
        float [] times;
        int segIdxs[];
        int width, height;
        byte [][] frames;
        byte []allFrames;
        int pixelSize;
        int startIdx, endIdx;

        void configure(String nodeName, String timeName, float timeMin, float timeMax) throws IOException
        {
              this.nodeName = nodeName;
              isSegmented = isSegmentedNode(nodeName);
              if(isSegmented)
              {
                  times = getSegmentTimes(nodeName, timeName, timeMin, timeMax);
                  if(times == null) throw new IOException(LocalDataProvider.this.ErrorString());
                  frames = new byte[times.length][];
                  segIdxs = getSegmentIdxs(nodeName, timeMin, timeMax);
                  if(segIdxs == null) throw new IOException(LocalDataProvider.this.ErrorString());
             }
              else
              {
                 if(timeName == null || timeName.trim().equals(""))
                     timeName = "dim_of("+nodeName+")";
                 float[] allTimes = getAllTimes(nodeName, timeName);
                 if(allTimes == null) throw new IOException(LocalDataProvider.this.ErrorString());
                 for(startIdx = 0; startIdx < allTimes.length && allTimes[startIdx] < timeMin; startIdx++);
                 for(endIdx = startIdx; endIdx < allTimes.length && allTimes[endIdx] < timeMax; endIdx++);
                 times = new float[endIdx - startIdx];
                 for(int i = 0; i < endIdx - startIdx; i++)
                     times[i] = allTimes[startIdx + i];
                 allFrames = getAllFrames(nodeName, startIdx, endIdx);
                 if(allFrames == null) throw new IOException(LocalDataProvider.this.ErrorString());
              }
              LocalDataProviderInfo info = getInfo(nodeName, isSegmented);
              if(info == null) throw new IOException(LocalDataProvider.this.ErrorString());
              width = info.dims[0];
              height = info.dims[1];
              pixelSize = info.pixelSize;
        }
     /**
     * Returns the type of the corresponding frames. Returned frames can have either of the following types:
     * <br>
     * -FrameData.BITMAP_IMAGE meaning that method GetFrameAt will return a byte matrix.
     * <br>
     * -FrameData.AWT_IMAGE meaning that method GetFrameAt will return a byte vector representing the binary
     *  content of a gif or jpeg file.
     * <br>
     * -FramDeata.JAI_IMAGE meaning that method GetFrameAt will return a byte vector representing the binary
     *  content of every image file supported by the JAI (Java Advanced Imaging) package. The JAI package needs not
     *  to be installed unless file formats other than gif or jpeg are used.
     *
     * @return The type of the corresponding frame.
     * @exception java.io.IOException
     */
        public int GetFrameType() throws IOException
        {
            switch(pixelSize) {
                case 1:
                     return FrameData.BITMAP_IMAGE_8;
                case 2:
                     return FrameData.BITMAP_IMAGE_16;
                case 4:
                     return FrameData.BITMAP_IMAGE_32;
                default:
                     return FrameData.BITMAP_IMAGE_8;
            }
        }

    /**
     * Returns thenumber of frames in the sequence.
     *
     * @return The number of frames in the sequence.
     * @exception java.io.IOException
     */
        public int GetNumFrames() throws IOException
        {
            return times.length;
        }

    /**
     * Return the dimension of a frame. All the frames in the sequence must have the same dimension.
     *
     * @return The frame dimension.
     * @exception java.io.IOException
     */
        public Dimension GetFrameDimension()throws IOException
        {
            return new Dimension(width, height);
        }

    /**
     * Return the times associated with every frame of the sequence. This information is required to correlate
     * the frame sequence with the other signals displayed by jScope.
     *
     * @return The time array for the frame sequence.
     * @exception java.io.IOException
     */
        public float[] GetFrameTimes() throws IOException
        {
            return times;
        }

    /**
     * Return the frame at the given position.
     *
     * @param idx The index of the requested frame in the frame sequence.
     * @return The frame as a byte array. If the frame type is FrameData.BITMAP_IMAGE, the matrix uses row major
     * ordering.
     * @exception java.io.IOException
     */
        public byte[] GetFrameAt(int idx) throws IOException
        {
            if(isSegmented)
            {
                if(frames[idx] == null)
                {
                    int segIdx = segIdxs[idx];
                    int segOffset = 0;
                    for(int i = idx-1; i >= 0 && segIdxs[i] == segIdx; i--, segOffset++);
                    frames[idx] = getSegment(nodeName, segIdx, segOffset);
                }
                return frames[idx];
            }
            byte[] outFrame = new byte[pixelSize * width * height];
            System.arraycopy(allFrames, idx*pixelSize * width * height, outFrame, 0, pixelSize * width * height);
            //for(int i = 0; i < 10000; i++) System.out.println(allFrames[i]);
            return outFrame;
        }
    } //LocalFrameData

     static {
        try
        {
          System.loadLibrary("JavaMds");
        }
        catch(UnsatisfiedLinkError  e)
        {
          //System.out.println("Load library "+e);
          javax.swing.JOptionPane.showMessageDialog(null, "Can't load data provider class LocalDataProvider : " + e, "Alert LocalDataProvider",
                                          javax.swing.JOptionPane.ERROR_MESSAGE);

          //e.printStackTrace();
        }
    }

    public void Update(String exp, long s)
    {
        var_idx = 0;
        UpdateNative(exp, s);
    }

    native public void SetEnvironmentSpecific(String in, String defaultNode);
    native public void UpdateNative(String exp, long s);
    native public String GetString(String in);
    native public double GetFloatNative(String in);

    public synchronized double GetFloat(String in) throws IOException
    {
        error = null;

        try {
            Calendar cal = Calendar.getInstance();
//            cal.setTimeZone(TimeZone.getTimeZone("GMT+00"));
            DateFormat df = new SimpleDateFormat("d-MMM-yyyy HH:mm Z");
            //DateFormat df = new SimpleDateFormat("d-MMM-yyyy HH:mm");-
            Date date = df.parse(in + " GMT");
            //Date date = df.parse(in);
            cal.setTime(date);
            long javaTime = cal.getTime().getTime();
            return javaTime;
        }catch(Exception exc){} //If exception occurs this is not a date
         return GetFloatNative(in);
    }

    public synchronized float[] GetFloatArray(String in)  throws IOException
    {
        return GetFloatArrayNative(in);
    }
    public synchronized double[] GetDoubleArray(String in)  throws IOException
    {
        return GetDoubleArrayNative(in);
    }
    public synchronized RealArray GetRealArray(String in)  throws IOException
    {
        long longArray[] = GetLongArrayNative(in);
        if(longArray != null)
            return new RealArray(longArray);
        else
            return new RealArray(GetDoubleArray(in));
    }

    native public long []GetLongArrayNative(String in);
    native public float[] GetFloatArrayNative(String in);
    native public double[] GetDoubleArrayNative(String in);
    native public int[]   GetIntArray(String in);

    public long[] GetShots(String in)
    {
        try {
            int shots[] =  GetIntArray(in.trim());
            long lshots[] = new long[shots.length];
            for(int i = 0; i < shots.length; i++)
                lshots[i] = shots[i];
            return lshots;
        }
        catch(UnsatisfiedLinkError e)
        {
            System.err.println("Error in GetIntArray: " + e);
            return null;
        }
        catch(Exception exc)
        {
            System.err.println("Error in GetIntArray: " + exc);
            return null;
        }
    }

    native public byte [] GetByteArray(String in);
    native public String ErrorString();
    public void AddUpdateEventListener(UpdateEventListener l, String event)
    {
        int evId;
        int idx;
        try {
            evId = getEventId(event);
            idx = eventNames.indexOf(event);
        }catch(Exception exc)
        {
            idx = eventNames.size();
            eventNames.addElement(event);
            evId = registerEvent(event, idx);
        }
        listeners.addElement(new EventDescriptor(l, event, evId));
    }
    public void RemoveUpdateEventListener(UpdateEventListener l, String event)
    {
        int idx = listeners.indexOf(new EventDescriptor(l, event, 0));
        if(idx != -1)
        {
            int evId = listeners.elementAt(idx).getEvId();
            listeners.removeElementAt(idx);
            try {
                int id = getEventId(event);
            }catch(Exception exc)
            {
                unregisterEvent(evId);
            }
        }

    }
    public void AddConnectionListener(ConnectionListener l){}

    public void RemoveConnectionListener(ConnectionListener l){}
//    public float[]  GetFrameTimes(String in_frame) {return null; }
//    public byte[] GetFrameAt(String in_frame, int frame_idx) {return null; }

    public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
    {
        LocalFrameData frameData = new LocalFrameData();
        frameData.configure(in_y, in_x, time_min, time_max);
        return frameData;
    }

    protected synchronized boolean  CheckOpen(String experiment, long shot) {return true; }
    public boolean SupportsCompression(){return false;}
    public void SetCompression(boolean state){}
    public boolean SupportsContinuous() {return false; }
    public boolean DataPending() {return  false;}
    public int     InquireCredentials(JFrame f, DataServerItem server_item){return DataProvider.LOGIN_OK;}
    public boolean SupportsFastNetwork(){return true;}
    public void    SetArgument(String arg){};
    public boolean SupportsTunneling(){return false;}


    int getEventId(String event) throws Exception
    {
        for(EventDescriptor evDescr : listeners)
            if(event.equals(evDescr.getEvent()))
                return evDescr.getEvId();
        throw(new Exception());
    }

    public void fireEvent(int nameIdx)
    {
        String event = eventNames.elementAt(nameIdx);
        for(EventDescriptor evDescr : listeners)
            if(evDescr.getEvent().equals(event))
                evDescr.getListener().processUpdateEvent(new UpdateEvent(this, event));
    }

    native public int registerEvent(String event, int idx);
    native public void unregisterEvent(int evId);

    void setResampleLimits(double min, double max)
    {
        String limitsExpr;
        if (Math.abs(min) > RESAMPLE_TRESHOLD ||
            Math.abs(max) > RESAMPLE_TRESHOLD)
        {
            long maxSpecific = jScopeFacade.convertToSpecificTime( (long) max);
            long minSpecific = jScopeFacade.convertToSpecificTime( (long) min);

            long dt = (maxSpecific - minSpecific) / MAX_PIXELS;
            limitsExpr = "JavaSetResampleLimits(" + minSpecific + "UQ," +
                maxSpecific + "UQ," + dt + "UQ)";
        } else {
            double dt = (max - min) / MAX_PIXELS;
            limitsExpr = "JavaSetResampleLimits(" + min + "," + max + "," + dt +
                ")";
        }
        GetFloatNative(limitsExpr);
    }
    boolean supportsLargeSignals() {return false;} //Subclass LocalDataProvider will return false
}

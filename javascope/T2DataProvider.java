
import java.io.*;
import java.net.*;
import java.util.*;
import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.lang.InterruptedException;


class T2DataProvider
    implements DataProvider
{
    String vmsIp = "130.237.47.22";
    int shot;
    int port = 8000;
    Socket socket = null;
    DataInputStream dis;
    DataOutputStream dos;
    String errorString = null;
    transient Vector   connection_listener = new Vector();


    public T2DataProvider()
    {
        shot = 0;
    }


    //DataProvider implementation
    public boolean SupportsCompression(){return false;}
    public void    SetCompression(boolean state){}
    public void    SetEnvironment(String s) {}
    public void    Dispose(){}
    public String  GetString(String in) {return in; }
    public float   GetFloat(String in){ return new Float(in).floatValue(); }
    public String  ErrorString() { return errorString; }
    public void    AddUpdateEventListener   (UpdateEventListener l, String event){}
    public void    RemoveUpdateEventListener(UpdateEventListener l, String event){}
    public boolean SupportsContinuous() {return false; }
    public boolean DataPending()        {return false; }
    public int     InquireCredentials(JFrame f, DataServerItem server_item){return DataProvider.LOGIN_OK;}
    public boolean SupportsFastNetwork(){return false;}
    public void    SetArgument(String arg)
    {
      StringTokenizer st = new StringTokenizer(arg, ":");
        try {
          vmsIp = st.nextToken();
          port = Integer.parseInt(st.nextToken());
        }catch(Exception exc){port = 8000;}
    }
    public boolean SupportsTunneling() {return false;}

    //  --------------------------------------------------------------------------------------------
    //     interface methods for getting *Data objects
    //  ---------------------------------------------------

    public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max)
        throws IOException
    {
        return null;
    }

    public synchronized WaveData GetWaveData (String in)
    {
        return GetWaveData (in, in+"$");
    }

    public synchronized WaveData GetWaveData (String in_y, String in_x)
    {
        return new T2WaveData(in_y.toUpperCase(), (in_x != null)?in_x.toUpperCase():in_x);
    }

    public synchronized WaveData GetResampledWaveData(String in, float start, float end, int n_points)
    {
        return null;
    }

    public synchronized WaveData
    GetResampledWaveData(String in_y, String in_x, float start, float end, int n_points)
    {
        return null;
    }

    public synchronized void AddConnectionListener(ConnectionListener l)
    {
        if (l == null)
          return;

        connection_listener.addElement(l);
    }

    public synchronized void RemoveConnectionListener(ConnectionListener l)
    {
        if (l == null)
          return;

        connection_listener.removeElement(l);
    }

    protected void DispatchConnectionEvent(ConnectionEvent e)
    {
        if (connection_listener != null)
        {
            for(int i = 0; i < connection_listener.size(); i++)
              ((ConnectionListener)connection_listener.elementAt(i)).processConnectionEvent(e);
        }
    }

    //  -------------------------------------------
    //      constructor, small stuff ...
    //  -------------------------------------------

    public void Update(String experiment, long shot)
    {
        this.shot = (int)shot;
        errorString = null;
    }
    public long[] GetShots(String in)
    {
        errorString = null;
        long [] result;
        String curr_in = in.trim();
        if(curr_in.startsWith("[", 0))
        {
            if(curr_in.endsWith("]"))
            {
                curr_in = curr_in.substring(1, curr_in.length() - 1);
                StringTokenizer st = new StringTokenizer(curr_in, ",", false);
                result = new long[st.countTokens()];
                int i = 0;
                try
                {
                    while(st.hasMoreTokens())
                      result[i++] = Integer.parseInt(st.nextToken());
                    return result;
                }
                catch(Exception e) {}
            }
        }
        else
        {
            if(curr_in.indexOf(":") != -1)
            {
                StringTokenizer st = new StringTokenizer(curr_in, ":");
                int start, end;
                if(st.countTokens() == 2)
                {
                    try
                    {
                        start = Integer.parseInt(st.nextToken());
                        end = Integer.parseInt(st.nextToken());
                        if(end < start)
                          end = start;
                        result = new long[end-start+1];
                        for(int i = 0; i < end-start+1; i++)
                          result[i] = start+i;
                        return result;
                    }
                    catch(Exception e){}
                }
            }
            else
            {
                result = new long[1];
                try
                {
                    result[0] = Long.parseLong(curr_in);
                    return result;
                }
                catch(Exception e){}
            }
        }
        errorString = "Error parsing shot number(s)";
        return null;
    }


    class T2WaveData implements WaveData
    {
        float []y = new float[0];
        float []x = new float[0];
        T2WaveData(String inY, String inX)
        {
            if(socket == null)
            {
                try {
                    socket = new Socket(vmsIp, port);
                    dos = new DataOutputStream(socket.getOutputStream());
                    dis = new DataInputStream(socket.getInputStream());
                    dos.writeInt(1);
                }catch(Exception exc)
                {
                    errorString = "Error connecting to T2 data server: " + exc;
                    socket = null;
                }
            }
            if(shot == 0)
            {
                errorString = "Missing shot";
                return;
            }
            if(inY.length() == 0)
            {
                return;
            }
            try {
                dos.writeInt(shot);
                dos.writeInt(inY.length());
                dos.write(inY.getBytes());
                dos.flush();
                int retlen = dis.readInt();
                y = new float[retlen];
                for(int i = 0; i < retlen; i++)
                    y[i] = dis.readFloat();

                //TEMPORARY
                /*x = new float[retlen];
                for(int i = 0; i < retlen; i++)
                    x[i] = i;
                if(true)return;*/

                dos.writeInt(shot);
                dos.writeInt(inX.length());
                dos.write(inX.getBytes());
                dos.flush();
                retlen = dis.readInt();
                x = new float[retlen];
                for(int i = 0; i < retlen; i++)
                    x[i] = dis.readFloat();
            }catch(Exception exc)
            {
                errorString = "Error reading data from T2: " + exc;
                socket = null;
            }
        }

        public int GetNumDimension(){return 1;}

        /**
        * Get Y data (for unidimensional signals) or Z data (for bidimensional signals) as a float array.
        * If bidimensional sugnals are returned, values are ordered by rows.
        *
        * @return The signal Y or Z data coded as a float array.
        * @exception java.io.IOException
        */
        public float[] GetFloatData(){return y;}

        /**
        * Get X array, usually representing the time values for signals.
        *
        * @return The returned X values coded as a float array
        * @exception java.io.IOException
        */
       public double[] GetXDoubleData(){return null;}
       public long[] GetXLongData(){return null;}
        public float[] GetXData()   {return x;}

        /**
        * Get Y data description (only for bidimensional signals)
        *
        * @return The Y data specification coded as a float array.
        * @exception java.io.IOException
        */
        public float[] GetYData()  {return null;}

        /**
        * Get the associated title for the signal. It is displayed if no title is defined in the setup
        * data definition.
        *
        * @return The title string.
        * @exception java.io.IOException
        */
        public String GetTitle() {return "";}

        /**
        * Get the associated label for X axis. It is displayed if no X axis label is defined in the setup data
        * definition.
        *
        * @return The X label string.
        * @exception java.io.IOException
        */
        public String GetXLabel()  {return "";}

        /**
        * Get the associated label for Y axis. It is displayed if no Y axis label is defined in the setup data
        * definition.
        *
        * @return The Y label string.
        * @exception java.io.IOException
        */
        public String GetYLabel() {return "";}

        /**
        * Get the associated label for Z axis (for bidimensional signals only). It is displayed if no X axis label is defined in the setup data
        * definition.
        *
        * @return The Z label string.
        * @exception java.io.IOException
        */
        public String GetZLabel()   {return "";}
    }
}






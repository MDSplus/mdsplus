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

// -------------------------------------------------------------------------------------------------
// TwuDataProvider
// An implementation of "DataProvider" for signals from a TEC Web-Umbrella (TWU) server.
//
// The first versions of this class (cvs revisions 1.x) were designed and written
// by Gabriele Manduchi and with some minor hacks by Jon Krom.
// Marco van de Giessen <A.P.M.vandeGiessen@phys.uu.nl> did some major surgery on
// this class (starting revision line 2.x) mainly in order to ensure that zooming
// works in more situations. (See also the cvs log.)
//
// Most of this revision 2.x work has now, from 2.21 onwards, been moved into separate classes.
// No new major revision number was started; has little use in CVS.
//
// $Id$
//
// -------------------------------------------------------------------------------------------------

import jScope.DataProvider;
import jScope.FrameData;
import jScope.DataServerItem;
import java.io.*;
import java.net.*;
import java.util.*;
import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.lang.InterruptedException;


class TwuDataProvider
    implements DataProvider
{
    private   String experiment;
    protected long   shot;
    private String error_string;
    private transient Vector<ConnectionListener> connection_listener = new Vector<>();
    private String user_agent;
    private TwuWaveData lastWaveData = null ;


    //DataProvider implementation
    public boolean SupportsCompression(){return false;}
    public void    SetCompression(boolean state){}
    public void    SetEnvironment(String s) {}
    public void    Dispose(){}
    public String  GetString(String in) {return in; }
    public double  GetFloat(String in) { return Double.parseDouble(in); }
    public String  ErrorString() { return error_string; }
    public void    AddUpdateEventListener   (UpdateEventListener l, String event){}
    public void    RemoveUpdateEventListener(UpdateEventListener l, String event){}
    public boolean SupportsContinuous() {return false; }
    public boolean DataPending()        {return false; }
    public int     InquireCredentials(JFrame f, DataServerItem server_item){return DataProvider.LOGIN_OK;}
    public boolean SupportsFastNetwork(){return true;}
    public void    SetArgument(String arg){}
    public boolean SupportsTunneling() {return false;}

    //  --------------------------------------------------------------------------------------------
    //     interface methods for getting *Data objects
    //  ---------------------------------------------------

    public void enableAsyncUpdate(boolean enable){}
    public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max)
        throws IOException
    {
        return (new TwuSimpleFrameData(this, in_y, in_x, time_min, time_max));
    }

    //  ---------------------------------------------------
    public synchronized WaveData GetWaveData (String in)
    {
        return GetWaveData (in, null);
    }

    public synchronized WaveData GetWaveData (String in_y, String in_x)
    {
        TwuWaveData find = FindWaveData (in_y, in_x);
        find.setFullFetch() ;
        return find ;
    }

    public synchronized WaveData GetResampledWaveData(String in, double start, double end, int n_points)
    {
        return GetResampledWaveData(in, null, start, end, n_points);
    }
    public synchronized WaveData
    GetResampledWaveData(String in_y, String in_x, double start, double end, int n_points)
    {
        TwuWaveData find = FindWaveData (in_y, in_x);
        find.setZoom ((float)start, (float)end, n_points);
        return find ;
    }

    public synchronized TwuWaveData FindWaveData (String in_y, String in_x)
    {
        if ( lastWaveData == null  ||  lastWaveData.notEqualsInputSignal (in_y, in_x, shot) )
        {
            lastWaveData = new TwuWaveData (this, in_y, in_x) ;
            try
            {
                // Ensure that the properties are fetched right away.
                lastWaveData.getTWUProperties() ;
            }
            catch (IOException e)
            {
                setErrorstring("No Such Signal : " +
                               TwuNameServices.GetSignalPath(in_y, shot) );
                //throw new IOException ("No Such Signal");
            }
        }
        return lastWaveData ;
    }

    // ---------------------------------------------------------------------------------------------

    public synchronized float[] GetFloatArray(String in)
    {
        boolean is_time;
        resetErrorstring(null);

        if(in.startsWith("TIME:", 0))
        {
            is_time = true;
            in = in.substring(5);
        }
        else
          is_time = false;

        TwuWaveData wd   = (TwuWaveData)GetWaveData  (in) ;
        float [] data = null ;
        try
        {
//            data = wd.GetFloatData() ;
            data = wd.getData(4000).y ;
        }
        catch (  Exception e )
        {
            resetErrorstring(e.toString());
            data = null ;
        }
        return data ;
    }

    //  ----------------------------------------------------
    //  Methods for TwuAccess.
    //  ----------------------------------------------------

    public synchronized float[]
    GetFloatArray (String in, boolean is_time) throws IOException
    {
        TwuWaveData wd = (TwuWaveData)GetWaveData(in) ; // TwuAccess wants to get the full signal data .
        return is_time ? wd.GetXData() : wd.GetYData() ;
    }

    
    
    
    public synchronized String GetSignalProperty (String prop, String in) throws IOException
    {
        TwuWaveData wd = (TwuWaveData) GetWaveData(in) ;
        return wd.getTWUProperties().getProperty(prop);
    }

    //  -------------------------------------------------------
    //     parsing of / extraction from input signal string
    //  -------------------------------------------------------

    public long[] GetShots(String in)
    {
        resetErrorstring(null);
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
        resetErrorstring("Error parsing shot number(s)");

        return null;
    }

    //  -------------------------------------------
    //     connection handling methods ....
    //  -------------------------------------------

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
            for(int i = 0; i < connection_listener.size(); i++)
              connection_listener.elementAt(i).processConnectionEvent(e);
    }

    //  -------------------------------------------
    //  Constructor, other small stuff ...
    //  -------------------------------------------

    public synchronized void
    Update(String experiment, long shot)
    {
        this.experiment = experiment;
        this.shot = shot;
        resetErrorstring(null);
        lastWaveData = null;
    }

    protected synchronized String
    getExperiment()
    {
        return experiment;
    }

    protected synchronized void
    setErrorstring(String newErrStr)
    {
        if(error_string==null)
          error_string = newErrStr;
    }

    protected synchronized void
    resetErrorstring(String newErrStr)
    {
        error_string = newErrStr;
    }


    public TwuDataProvider()
    {
        super();
    }

    public static String
    revision()
    {
        return "$Id$";
    }

    public TwuDataProvider(String user_agent)
    {
        this.user_agent = user_agent;
        // Could be used in the constructor for TWUProperties and in similar get URL actions.
        // A site could used this as a possible (internal) software distribution management
        // tool.  In the log of a web-server you can, by checking the user_agent, see which
        // machines are still running old software.
    }
}

// -------------------------------------------------------------------------------------------------
// End of: $Id$
// -------------------------------------------------------------------------------------------------

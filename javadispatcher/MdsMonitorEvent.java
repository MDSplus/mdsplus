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
import java.util.*;
import java.text.*;

class MdsMonitorEvent extends MdsServerEvent
{
    static final int MonitorBeginSequence = 1;
    static final int MonitorBuildBegin    = 2;
    static final int MonitorBuild         = 3;
    static final int MonitorBuildEnd      = 4;
    static final int MonitorCheckin       = 5;
    static final int MonitorDispatched    = 6;
    static final int MonitorDoing         = 7;
    static final int MonitorDone          = 8;
    static final int MonitorEndPhase             = 9;
    static final int MonitorStartPhase           = 10;
    static final int MonitorEndSequence          = 11;

    static final int MonitorServerConnected      = 12;
    static final int MonitorServerDisconnected   = 13;
    
    /*
    static final int MonitorServerConnected      = 10;
    static final int MonitorServerDisconnected   = 11;
    static final int MonitorEndPhase             = 12;
    static final int MonitorStartPhase           = 13;
    static final int MonitorEndSequence          = 9;
    */

    String tree;
    long   shot;
    int    phase;
    int    nid;
    String name;
    int    on;
    int    mode;
    String server;
    String server_address;
    int    ret_status;

    String node_path;
    Date   date;
    String date_st;
    String error_message;
    long   execution_time; //msec

    
    public MdsMonitorEvent(Object obj, int mode,  String msg)
    {
        super(obj, 0, 0, 1);
        this.mode = mode;
        this.error_message = msg;
   }

    
    public MdsMonitorEvent(Object obj, int phase, int nid, String msg)
    {
        super(obj, 0, 0, 1);
        this.tree = null;
        this.shot = 0;
        this.phase = phase;
        this.nid = nid;
        this.error_message = msg;
   }

    public MdsMonitorEvent(Object obj, String tree, int shot, int phase, int nid, String name, int on, int mode,
        String server, String server_address, int ret_status)
    {
        super(obj, 0, 0, 1);
        this.tree = tree;
        this.shot = shot;
        this.phase = phase;
        this.nid = nid;
        this.name = name;
        this.on = on;
        this.mode = mode;
        this.server = server;
        this.server_address = server_address;
        this.ret_status = ret_status;
        this.error_message = MdsHelper.getErrorString(ret_status);
        date_st = (new Date()).toString();
    }

    public MdsMonitorEvent(Object source, int id, int flags, int status, String data) throws Exception
    {
        super(source, id, flags, status);
        date = new Date();
        try
        {            
            StringTokenizer buf = new StringTokenizer(data);

            tree   = new String(buf.nextToken());
            if(buf.hasMoreTokens())
                shot   = Long.decode(buf.nextToken()).intValue();
            if(buf.hasMoreTokens())
                phase    = Integer.decode(buf.nextToken()).intValue();
            if(buf.hasMoreTokens())
                nid    = Integer.decode(buf.nextToken()).intValue();
            if(buf.hasMoreTokens())
                on    = Integer.decode(buf.nextToken()).intValue();
            if(buf.hasMoreTokens())
                mode   = Integer.decode(buf.nextToken()).intValue();
            if(buf.hasMoreTokens())
                server = new String(buf.nextToken());
            if(buf.hasMoreTokens())
                server_address = new String(buf.nextToken());
            if(buf.hasMoreTokens())
                ret_status    = Integer.decode(buf.nextToken()).intValue();
            if(buf.hasMoreTokens())
                node_path    = new String(buf.nextToken());
            if(buf.hasMoreTokens())
                date_st    = new String(buf.nextToken(";"));
            if(buf.hasMoreTokens())
            {
                error_message    = new String(buf.nextToken(""));
                error_message = error_message.substring(1, error_message.length());
            }
        }
        catch (NumberFormatException e)
        {
            System.out.println(data);
            e.printStackTrace();
            throw(new Exception("Bad monitor event data" + e));
        }
    }

    public synchronized byte [] toBytes()
    {
        String out_st = tree + " " + shot + " " + phase + " " + nid + " " + on + " "
            + mode + " " + server + " " + server_address + " " + ret_status + " " + name + " " + date_st + " ; " + error_message;
        byte [] msg = out_st.getBytes();

        String head_st = "" + jobid + " " + flags + " " + status + " " + msg.length;
        byte [] headmsg = head_st.getBytes();
        byte [] outmsg = new byte[60 + msg.length];
        System.arraycopy(headmsg, 0, outmsg, 0, headmsg.length);
        System.arraycopy(msg, 0, outmsg, 60, msg.length);
        return outmsg;
    }

    private String getMode(int mode_id)
    {
        switch(mode_id)
        {
            case MonitorBeginSequence 	    : return "MonitorBeginSequence";
            case MonitorBuildBegin          : return "MonitorBuildBegin";
            case MonitorBuild               : return "MonitorBuild";
            case MonitorBuildEnd            : return "MonitorBuildEnd";
            case MonitorCheckin             : return "MonitorCheckin";
            case MonitorDispatched          : return "MonitorDispatched";
            case MonitorDoing               : return "MonitorDoing";
            case MonitorDone                : return "MonitorDone";
            case MonitorServerConnected     : return "MonitorServerConnected";
            case MonitorServerDisconnected  : return "MonitorServerDisconnected";
            case MonitorEndPhase            : return "MonitorEndPhase";
            case MonitorStartPhase          : return "MonitorStartPhase";
            case MonitorEndSequence 	    : return "MonitorEndSequence";
        }
        return "";
    }

    public synchronized String toString()
    {
        return new String("[exp="+tree+";shot="+shot+";phase="+phase+";nid="+nid+";on="+on+";mode= "+getMode(mode)+" ;server="+server+
               "; ServerAddress="+ server_address+" ;status="+ret_status+"]");
    }

 
    public synchronized String getMonitorString()
    {
        StringBuffer out = new StringBuffer();

        if(mode == MonitorDone)
          out.append("["+ msecToString(execution_time) +"] "+date_st+", ");
        else
          out.append(date_st+", ");

        switch(mode)
        {
            case MonitorBuildBegin :
            case MonitorBuild      :
            case MonitorBuildEnd   :
                String on_off = (on == 1) ? "ON" : "OFF";
                out.append(" Action " + on_off + " node "+ node_path );
                break;
            case MonitorDispatched :
                out.append(" Dispatching node "+ node_path + "("+nid+")"+" to " + server);
                break;
            case MonitorDoing      :
                out.append(" Doing "+ node_path + " in " + tree + " shot " + shot + " on "+server);
                break;
            case MonitorDone       :
                if((ret_status & 1) == 1)
                    out.append(" Done "+ node_path + " in " + tree + " shot " + shot + " status " + ret_status + " on "+server);
                else
                    out.append(" Failed "+ node_path + " in " + tree + " shot " + shot + " status " + ret_status + " on "+server + " "+error_message);
                break;
        }
        return out.toString().trim();
    }

    public static String msecToString(long msec)
    {
      int min = (int)(msec/60000.);
      msec -= min * 60000;
      int sec = (int)(msec / 1000.);
      msec -= sec * 1000;

      DecimalFormat df = new DecimalFormat("#00");
      DecimalFormat df1 = new DecimalFormat("#000");

      return df.format(min)+"."+df.format(sec)+"."+df1.format(msec);
    }

}

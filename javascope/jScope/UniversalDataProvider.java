package jScope;

/* $Id$ */
import jScope.TSDataProvider;
import jScope.DataProvider;
import jScope.JetMdsDataProvider;
import jScope.FrameData;
import jScope.DataServerItem;
import jScope.MdsDataProvider;
import jScope.FtuDataProvider;
import jScope.JetDataProvider;
import jScope.ConnectionListener;
import jScope.AsdexDataProvider;
import java.io.*;
import javax.swing.JFrame;
import java.util.*;

class UniversalDataProvider implements DataProvider
{
    String error = "Unknown experiment";
    MdsDataProvider rfx;
    FtuDataProvider ftu;
    TwuDataProvider twu;
    JetDataProvider jet;
    JetMdsDataProvider jetmds;
    TSDataProvider ts;
    AsdexDataProvider asd;
    public UniversalDataProvider() throws IOException
    {
        rfx = new MdsDataProvider();
        try {
            rfx.SetArgument("150.178.3.80");
        }catch(Exception exc){rfx = null;}
        ftu = new FtuDataProvider();
        try {
            ftu.SetArgument("192.107.51.84:8100");
        }catch(Exception exc){ftu = null;}
        twu = new TwuDataProvider();
        jet = new JetDataProvider();
        jetmds = new JetMdsDataProvider();
        ts = new TSDataProvider();
        try {
            ts.SetArgument("132.169.8.164:8000");
        }catch(Exception exc){ts = null;}
        asd = new AsdexDataProvider();
        try {
            asd.SetArgument("localhost:8000");
        }catch(Exception exc){asd = null;}
    }

    public void setContinuousUpdate(){}

    public void enableAsyncUpdate(boolean enable){}
    protected DataProvider SelectProvider(String spec)
    {
        if(spec.startsWith("rfx:"))
            return rfx;
        if(spec.startsWith("ftu:"))
            return ftu;
        if(spec.startsWith("twu:"))
            return twu;
        if(spec.startsWith("jet:"))
            return jet;
        if(spec.startsWith("jetmds:"))
            return jetmds;
        if(spec.startsWith("ts:"))
            return ts;
        if(spec.startsWith("asd:"))
            return asd;
        error = "Unknown experiment";

        return null;
    }

    protected String RemoveExp(String spec)
    {
        if(spec.startsWith("jetmds:"))
            return spec.substring(7);
        if(spec.startsWith("ts:"))
            return spec.substring(3);
        return spec.substring(4);
    }

    public WaveData GetWaveData(String in)
    {
        try {
            return SelectProvider(in).GetWaveData(RemoveExp(in));
        }catch(Exception exc) {return null; }
    }
    public WaveData GetWaveData(String in_y, String in_x)
    {
        try {
            return SelectProvider(in_y).GetWaveData(RemoveExp(in_y), in_x);
        }catch(Exception exc) {return null; }
    }
    public WaveData GetResampledWaveData(String in, double start, double end, int n_points)
    {
        return null;
    }
    public WaveData GetResampledWaveData(String in_y, String in_x, double start, double end, int n_points)
    {
        return null;
    }

    public void    Dispose()
    {
        if(rfx != null) rfx.Dispose();
        if(ftu != null) ftu.Dispose();
        if(twu != null) twu.Dispose();
        if(jet != null) jet.Dispose();
        if(jetmds != null) jetmds.Dispose();
        if(ts != null) ts.Dispose();
        if(asd != null) asd.Dispose();
    }
    public boolean SupportsCompression(){return false;}
    public void    SetCompression(boolean state){}
    public boolean SupportsContinuous() { return true; }
    public int     InquireCredentials(JFrame f, DataServerItem server_item)
    {
        if(rfx != null) rfx.InquireCredentials(f, new DataServerItem("java_user_ext"));
        return jet.InquireCredentials(f, server_item);
    }
    public boolean SupportsTunneling() {return false; }
    public boolean SupportsFastNetwork(){return false;}
    public void    SetArgument(String arg){}


    public void SetEnvironment(String exp)
    {
        error = null;
    }
    public void Update(String exp, long s)
    {
        if(exp == null) return;
        if(exp.equals("rfx") && rfx != null)
            rfx.Update(exp, s);
        else if(exp.equals("ftu") && ftu != null)
            ftu.Update(exp, s);
        else if(exp.equals("twu") && twu != null)
            twu.Update(exp, s);
        else if(exp.equals("jet") && jet != null)
            jet.Update(null, s);
        else if(exp.equals("jetmds") && jetmds != null)
            jetmds.Update(null, s);
        else if(exp.equals("ts") && ts != null)
            ts.Update(null, s);
        else if(exp.equals("asd") && asd != null)
            asd.Update(null, s);
        error = null;
    }

    public String GetString(String in)
    {
        error = null;
        return new String(in);
    }

    public double GetFloat(String in)
    {
        error = null;
        return Double.parseDouble(in);
    }

    public long[] GetShots(String in)
    {
        long d[] = new long[1];
        try {
            return rfx.GetShots(in);
        }catch (Exception exc)
        {
            try {
                StringTokenizer st = new StringTokenizer(in, ":");
                String shotStr = st.nextToken();
                d[0] = Long.parseLong(shotStr);
            }catch(Exception exc1) {d[0] = 0;}
        }
        return d;
    }

    public String ErrorString()
    {
        return error;
    }
    public void AddUpdateEventListener(UpdateEventListener l, String event) throws IOException
    {
        if(twu != null) twu.AddUpdateEventListener(l, event);
        if(rfx != null) rfx.AddUpdateEventListener(l, event);
        if(ftu != null) ftu.AddUpdateEventListener(l, event);
        if(jet != null) jet.AddUpdateEventListener(l, event);
        if(jetmds != null) jetmds.AddUpdateEventListener(l, event);
        if(ts != null) ts.AddUpdateEventListener(l, event);
        if(asd != null) asd.AddUpdateEventListener(l, event);
    }
    public void RemoveUpdateEventListener(UpdateEventListener l, String event)throws IOException
    {
        if(twu != null) twu.RemoveUpdateEventListener(l, event);
        if(rfx != null) rfx.RemoveUpdateEventListener(l, event);
        if(ftu != null) ftu.RemoveUpdateEventListener(l, event);
        if(jet != null) jet.RemoveUpdateEventListener(l, event);
        if(jetmds != null) jetmds.RemoveUpdateEventListener(l, event);
        if(ts != null) ts.RemoveUpdateEventListener(l, event);
        if(asd != null) asd.RemoveUpdateEventListener(l, event);

    }
    public void    AddConnectionListener(ConnectionListener l)
    {
        if(twu != null) twu.AddConnectionListener(l);
        if(rfx != null) rfx.AddConnectionListener(l);
        if(ftu != null) ftu.AddConnectionListener(l);
        if(jet != null) jet.AddConnectionListener(l);
        if(jetmds != null) jetmds.AddConnectionListener(l);
        if(ts != null) ts.AddConnectionListener(l);
        if(asd != null) asd.AddConnectionListener(l);
    }
    public void    RemoveConnectionListener(ConnectionListener l)
    {
        if(twu != null) twu.RemoveConnectionListener(l);
        if(rfx != null) rfx.RemoveConnectionListener(l);
        if(ftu != null) ftu.RemoveConnectionListener(l);
        if(jet != null) jet.RemoveConnectionListener(l);
        if(jetmds != null) jetmds.RemoveConnectionListener(l);
        if(ts != null) ts.RemoveConnectionListener(l);
        if(asd != null) asd.RemoveConnectionListener(l);

    }

    public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
    {
        return null;
    }


    public float[] GetFrameTimes(String in_expr)
    {
        return null;
     }

    public byte[]  GetAllFrames(String in_frame){return null;}

    public byte[] GetFrameAt(String in_expr, int frame_idx)
    {
        return null;
    }

    public boolean DataPending()
    {
        return false;
    }

 }

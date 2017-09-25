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
    String error;
    MdsDataProvider defaultProvider;
    Hashtable dataProviderH = new Hashtable();
    MdsDataProvider getProvider(String ip)
    {
        if(!ip.startsWith("//"))
            return defaultProvider;
        int pos = ip.substring(2).indexOf("/");
        String currIp = ip.substring(2,pos+2);
        MdsDataProvider provider = (MdsDataProvider)dataProviderH.get(currIp);
        if(provider == null)
        {
            provider = new MdsDataProvider();
            try {
                provider.SetArgument(currIp);
                dataProviderH.put(currIp, provider);
            }catch(Exception exc){return null;}
        }
        return provider;
    }
    boolean isDefault(String in)
    {
        return (!in.startsWith("//"));
    }
    String getExperiment(String in)
    {
        String subStr1 = in.trim().substring(2);
        int pos1 = subStr1.indexOf("/");
        String subStr2 = subStr1.substring(pos1+1);
        int pos2 = subStr2.indexOf("/");
        return subStr2.substring(0, pos2);
    }
    int getShot(String in) throws Exception
    {
        String subStr1 = in.trim().substring(2);
        int pos1 = subStr1.indexOf("/");
        String subStr2 = subStr1.substring(pos1+1);
        int pos2 = subStr2.indexOf("/");
        String subStr3 = subStr2.substring(pos2+1);
        int pos3 =  subStr3.indexOf("/");
        String subStr4 = subStr3.substring(0, pos3);
        return Integer.parseInt(subStr4);
    }
    
    public UniversalDataProvider() 
    {
        defaultProvider = new MdsDataProvider();
    }
    public void    SetArgument(String arg)
    {
        try {
            defaultProvider.SetArgument(arg);
        }catch(Exception exc){defaultProvider = null;}
    }

    public void enableAsyncUpdate(boolean enable)
    {
        Enumeration en = dataProviderH.elements();
        while(en.hasMoreElements())
            ((MdsDataProvider)en.nextElement()).enableAsyncUpdate(enable);
    }
 
    String getExpr(String spec)
    {
        if(!spec.startsWith("//"))
            return spec;
        String subStr1 = spec.trim().substring(2);
        int pos1 = subStr1.indexOf("/");
        String subStr2 = subStr1.substring(pos1+1);
        int pos2 = subStr2.indexOf("/");
        String subStr3 = subStr2.substring(pos2+1);
        int pos3 =  subStr3.indexOf("/");
        return subStr3.substring(pos3+1);
     }
    
    public WaveData GetWaveData(String in)
    {
        MdsDataProvider currProvider = getProvider(in);
        if(currProvider == null)
        {
            error = "Missing default provider";
            return null;
        }
        if(!isDefault(in))
        {
            try {
                currProvider.Update(getExperiment(in), getShot(in));
            }
            catch(Exception exc)
            {
                System.out.println(exc);
            }
        }
        try {
            return currProvider.GetWaveData(getExpr(in));
        }catch(Exception exc) {return null; }
    }
    public WaveData GetWaveData(String in_y, String in_x)
    {
        MdsDataProvider currProvider = getProvider(in_y);
        if(currProvider == null)
        {
            error = "Missing default provider";
            return null;
        }
        if(!isDefault(in_y))
        {
            try {
                currProvider.Update(getExperiment(in_y), getShot(in_y));
            }
            catch(Exception exc)
            {
                System.out.println(exc);
            }
        }
        try {
            return currProvider.GetWaveData(getExpr(in_y), getExpr(in_x));
        }catch(Exception exc) 
        {
            error = ""+exc; 
            return null; 
        }
    }

    public void    Dispose()
    {
        Enumeration en = dataProviderH.elements();
        while(en.hasMoreElements())
            ((MdsDataProvider)en.nextElement()).Dispose();
    }
    public int     InquireCredentials(JFrame f, DataServerItem server_item)
    {
        return 1;
    }
    public boolean SupportsTunneling() {return false; }

    public void SetEnvironment(String exp)
    {
        Enumeration en = dataProviderH.elements();
        while(en.hasMoreElements())
        {
            try {
                ((MdsDataProvider)en.nextElement()).SetEnvironment(exp);
            }catch(Exception exc){}
        }
    }
    public void Update(String exp, long s)
    {
        defaultProvider.Update(exp, s);
    }

    public String GetString(String in)
    {
        try {
            return defaultProvider.GetString(in);
        }catch(Exception exc){return null;}
    }
    public double GetFloat(String in)
    {
         try {
            return defaultProvider.GetFloat(in);
        }catch(Exception exc) 
        {
            error = ""+exc; 
            return 0;
        }
   }

    public long[] GetShots(String in)
    {
        try {
            return defaultProvider.GetShots(in);
        }catch(Exception exc){error = ""+exc;return null;}
     }

    public String ErrorString()
    {
        return error;
    }
    
    
    
    
    public void AddUpdateEventListener(UpdateEventListener l, String event) throws IOException
    {
        defaultProvider.AddUpdateEventListener(l, event);
    }
    public void RemoveUpdateEventListener(UpdateEventListener l, String event)throws IOException
    {
        defaultProvider.RemoveUpdateEventListener(l, event);
    }
    public void    AddConnectionListener(ConnectionListener l)
    {
        defaultProvider.AddConnectionListener(l);
    }
    public void    RemoveConnectionListener(ConnectionListener l)
    {
        defaultProvider.RemoveConnectionListener(l);
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

 }

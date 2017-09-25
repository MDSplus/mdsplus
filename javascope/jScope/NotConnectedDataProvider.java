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
import jScope.DataProvider;
import jScope.FrameData;
import jScope.DataServerItem;
import jScope.ConnectionListener;
import java.io.*;
import javax.swing.JFrame;


class NotConnectedDataProvider implements DataProvider
{
    final String error = "Not Connected";


    public WaveData GetWaveData(String in)
    {
        return null;
    }
    public WaveData GetWaveData(String in_y, String in_x)
    {
        return null;
    }
    public WaveData GetResampledWaveData(String in, double start, double end, int n_points)
    {
        return null;
    }
    public WaveData GetResampledWaveData(String in_y, String in_x, double start, double end, int n_points)
    {
        return null;
    }
    public void enableAsyncUpdate(boolean enable){}

    public void    Dispose(){}
    public boolean SupportsCompression(){return false;}
    public void    SetCompression(boolean state){}
    public boolean SupportsContinuous() { return false; }
    public int     InquireCredentials(JFrame f, DataServerItem server_item){return DataProvider.LOGIN_OK;}
    public boolean SupportsFastNetwork(){return false;}
    public void    SetArgument(String arg){}
    public boolean SupportsTunneling() {return false; }

    public void SetEnvironment(String exp)
    {
    }

    public void Update(String exp, long s)
    {
    }

    public String GetString(String in)
    {
        return "";
    }

    public double GetFloat(String in)
    {
        return Double.parseDouble(in);
    }

    public float[] GetFloatArray(String in_x, String in_y, float start, float end)
    {
        return null;
    }

    public float[] GetFloatArray(String in)
    {
        return null;
    }

    public long[] GetShots(String in)
    {
        long d[] = new long[1];
        d[0] = 0;
        return d;
    }

    public String ErrorString()
    {
        return error;
    }

    public void AddUpdateEventListener(UpdateEventListener l, String event)
    {
    }

    public void RemoveUpdateEventListener(UpdateEventListener l, String event)
    {
    }

    public void AddConnectionListener(ConnectionListener l)
    {
    }

    public void RemoveConnectionListener(ConnectionListener l)
    {
    }


    public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
    {
        return null;
    }

    public boolean DataPending()
    {
        return false;
    }
}

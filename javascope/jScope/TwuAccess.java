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
import jScope.Signal;
import jScope.DataProvider;
import jScope.FrameData;
import jScope.DataAccess;
import java.util.*;
import java.awt.*;
import java.io.IOException;

public class TwuAccess implements DataAccess
{
    String ip_addr = null;
    String shot_str = null;
    String signal = null;
    TwuDataProvider tw = null;
        
    public static void main(String args[])
    {
        TwuAccess access = new TwuAccess();
        String url = "twu://ipptwu.ipp.kfa-juelich.de/textor/all/86858/RT2/IVD/IBT2P-star";
        boolean supports = access.supports(url);
        try
        {
        float y [] = access.getY(url);
        float x [] = access.getX(url);
 
        for(int i = 0; i < x.length; i++)
            System.out.println(x[i] + "  " +y[i]);
           
        System.out.println("Num. points: "+y.length);
        } catch (IOException exc){}
    }
    
    public boolean supports(String url)
    {
        StringTokenizer st = new StringTokenizer(url, ":");
        if(st.countTokens() < 2) return false;
        return st.nextToken().equals("twu");
    }
    
    public void setProvider(String url) throws IOException
    {
        signal = "http" + url.substring(url.indexOf(":"));
        
        String dummy;
        StringTokenizer st = new StringTokenizer(url, "/");
        dummy = st.nextToken();
        dummy = st.nextToken();
        dummy = st.nextToken();
        dummy = st.nextToken();
        shot_str  = st.nextToken();
        
        if(tw == null)
        {
            tw = new TwuDataProvider("jScope applet (Version 7.2.2)");
        }        
    }
    
    public String getShot()
    {
        return shot_str;
    }

    public String getSignalName()
    {
        return signal;
    }

    public String getExperiment()
    {
        return null;
    }

    public DataProvider getDataProvider()
    {
        return tw;
    }

    public void close(){}
    
    public void setPassword(String encoded_credentials){}

    public float [] getX(String url) throws IOException
    {
        setProvider(url);
        if(signal == null) return null;
        return tw.GetFloatArray(signal, true);
    }
    
    public float [] getY(String url) throws IOException
    {
        setProvider(url);
        if(signal == null) return null;
        return tw.GetFloatArray(signal, false);
    }    
    
    public Signal getSignal(String url) throws IOException
    {
        setProvider(url);
        if(signal == null) return null;
        Signal s = null;
        
        float y[] = tw.GetFloatArray(signal, false);
        float x[] = tw.GetFloatArray(signal, true);
            
        if(x == null || y == null)
            return null;

        s = new Signal(x, y);
        
        s.setName(tw.GetSignalProperty("SignalName", signal));
        
        //System.out.println(tw.getSignalProperty("SignalName", signal));
                
        return s;
    }
    
    public String getError()
    {   
        if(tw == null)
            return("Cannot create TwuDataProvider");
        return tw.ErrorString();
    }

    public FrameData getFrameData(String url)
    {
        return null;
    }
}
        

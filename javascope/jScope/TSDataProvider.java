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
import jScope.MdsplusParser;
import jScope.DataServerItem;
import jScope.MdsDataProvider;
import java.util.*;
import java.io.IOException;
import javax.swing.JFrame;

class TSDataProvider extends MdsDataProvider
{

    public TSDataProvider()
    {
        super();
    }
    
    public TSDataProvider(String provider) throws IOException
    {
        super(provider);
    }
    
    public void SetArgument(String arg) throws IOException
    {
        mds.setProvider(arg);
        mds.setUser("mdsplus");
    }
    
    public synchronized void Update(String exp, long s)
	{
	    error = null;
		shot = (int) s;
	}
	
	protected String ParseExpression(String in)
	{
	    //if(in.startsWith("DIM_OF("))
	    //    return in;
	        
	    String res = MdsplusParser.parseFun(in, "GetTsBase(" + shot + ", \"", "\")");
	        
/*	    StringTokenizer st = new StringTokenizer(in, ":");
        String res = "GetTSData(\"";
	    try{
	        String name = st.nextToken();*/
/*	        String rang0 = st.nextToken();
	        String rang1 = st.nextToken();
	        res = "GetTSData(\"" + name + "\", " + shot + ", " +
	            rang0 + ", " + rang1 + ")"; */
	        //res = "GetTsBase(" + shot + ", \"" + name + "\")";
/*	    }catch(Exception e)
	    {
	        error = "Wrong signal format: must be <signal_name>:<rangs[0]>:<rangs[1]>";
	        return null;
	    }*/
	    //System.out.println(res);
	    return res;
	}

    public synchronized int[] GetIntArray(String in) throws IOException
    {
        String parsed = ParseExpression(in);
        if(parsed == null) return null;
        return super.GetIntArray(parsed);
    }
	
	
	
    public synchronized float[] GetFloatArray(String in) throws IOException
    {
        String parsed = ParseExpression(in);
        if(parsed == null) return null;
	    error= null;
	    float [] out_array = super.GetFloatArray(parsed);
	    if(out_array == null&& error == null)
	        error = "Cannot evaluate " + in + " for shot " + shot;
	    if(out_array != null && out_array.length <= 1)
	    {
	        error = "Cannot evaluate " + in + " for shot " + shot;
	        return null;
	    }
	    return out_array;
    }

        
protected String GetDefaultXLabel(String in_y)  throws IOException
{
	error= null;
	return GetString("GetTSUnit(0)");
}
        
protected String GetDefaultYLabel()  throws IOException
{
	error= null;
	return GetString("GetTSUnit(1)");
}

public boolean SupportsCompression(){return false;}
public void    SetCompression(boolean state){}
public boolean SupportsContinuous() {return false; }
public boolean DataPending() {return  false;}
public int     InquireCredentials(JFrame f, DataServerItem server_item){return DataProvider.LOGIN_OK;}
public boolean SupportsFastNetwork(){return false;}
public int []    GetNumDimensions(String spec) {return new int[] {1};}
}
								

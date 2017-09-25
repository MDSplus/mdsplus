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
import java.util.*;
import java.io.IOException;
import javax.swing.JFrame;

class JetMdsDataProvider extends MdsDataProvider
{
    public JetMdsDataProvider()
    {
        super("mdsplus.jet.efda.org");
    }

    public void SetArgument(String arg){};

    public synchronized void Update(String exp, long s)
	{
	    error = null;
		shot = s;
	}
    public synchronized int[] GetIntArray(String in) throws IOException
    {
        return super.GetIntArray(ParseExpression(in));
    }

	//Syntax: ppf/<signal> or jpf/<signal>
	//Ex: ppf/magn/ipla
	protected String ParseExpression(String in)
	{
	  error= null;
	  StringTokenizer st = new StringTokenizer(in, " /(){}[]*+,:;", true);
	  String parsed = "", signal = "";
	  int state = 0;
	  try{
	  while(st.hasMoreTokens())
	  {
		String curr_str = st.nextToken();
		//System.out.println("Token: "+curr_str);
		switch(state)  {
		    case 0:
			  if(curr_str.equals("/"))
			  {
			    if(parsed.endsWith("PPF") ||parsed.endsWith("ppf")  ||
			        parsed.endsWith("JPF") ||parsed.endsWith("jpf"))
			    {
			        signal = parsed.substring(parsed.length() - 3)+ "/";
			        parsed = parsed.substring(0, parsed.length() - 3);
				    state = 1;
				}
				else
				    parsed += curr_str;
			  }
			  else
		    	  	parsed+=curr_str;
			  break;
		    case 1:
		        signal += curr_str;
                state = 2;
		        break;
		    case 2:
		        signal += curr_str;
		        state = 3;
		        break;
		    case 3:
		        parsed += ("(jet(\"" + signal + curr_str + "\", " + shot + ")) ");
		        signal = "";
		        state = 0;
		        break;
		    }
		}
	}
	catch(Exception e)
	{
	    System.out.println(e);
	}
	return parsed;
	}

/*
    public synchronized float[] GetFloatArray(String in) throws IOException
    {
	//System.out.println("parsed: "+ parsed);
	float [] out_array = super.GetFloatArray(ParseExpression(in));
	if(out_array == null && error == null)
	    error = "Cannot evaluate " + in + " for shot " + shot;

	//if(out_array != null && out_array.length <= 1)
	//{
	//    error = "Cannot evaluate " + in + " for shot " + shot;
	//    return null;
	//}

	return out_array;
    }
*/
    public synchronized RealArray GetRealArray(String in) throws IOException
    {
        return super.GetRealArray(ParseExpression(in));
    }

public boolean SupportsCompression(){return false;}
public void    SetCompression(boolean state){}
public boolean SupportsContinuous() {return false; }
public boolean DataPending() {return  false;}
public int     InquireCredentials(JFrame f, DataServerItem server_item){return DataProvider.LOGIN_OK;}
public boolean SupportsFastNetwork(){return true;}
}


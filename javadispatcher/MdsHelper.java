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
import java.io.*;
class MdsHelper
{
    static Hashtable<String, Integer> name_to_id = new Hashtable<String, Integer>();
    static Hashtable<Integer, String> id_to_name = new Hashtable<Integer, String>();
    static String dispatcher_ip = null;
    static int dispatcherPort = 0;
    static String experiment = null;
    static Properties initialization ( String exp ) {


		experiment = exp;

        Properties properties = new Properties();
        try {

            String propFileName; 
            String path;

            path = System.getenv("MDSPLUS_DIR");
            if( path == null )
                    path = "";
            else
                    path = path+"/local/";

            if(experiment != null)
                    propFileName = path+"jDispatcher_"+experiment.toLowerCase()+".properties";
            else
                    propFileName = path+"jDispatcher.properties";


	    //System.out.println("Configuration file " + propFileName);
	
            try
            {
            	properties.load(new FileInputStream(propFileName));
            }
            catch (Exception exc) 
            {
                                //System.out.println("Cannot open properties file : "+ propFileName);
		propFileName = null; 
            }
            if( propFileName == null )
            {
                try {
                        propFileName =  path+"jDispatcher.properties";
                        properties.load(new FileInputStream(propFileName));
                }
		catch (Exception exc) 
		{
                                //System.out.println("Cannot open properties file : "+ propFileName);
                    propFileName = null; 
                }
                if( propFileName == null )
                {
                    try {
                            propFileName =  "jDispatcher.properties";
                            properties.load(new FileInputStream(propFileName));
                        }
			catch (Exception exc) 
			{
                            System.out.println("Cannot open properties file : "+ propFileName);
                            propFileName = null; 
                        }
                }
            }
            int i = 1;
            while(true)
            {
                String phase_name = properties.getProperty("jDispatcher.phase_"+i+".name");
                if(phase_name == null) break;
                String phase_id = properties.getProperty("jDispatcher.phase_"+i+".id");
                if(phase_id == null) break;
                id_to_name.put(new Integer(phase_id), phase_name);
                name_to_id.put(phase_name, new Integer(phase_id));
                i++;
            }
            dispatcher_ip = properties.getProperty("jDispatcher.dispatcher_ip");
            dispatcherPort = Integer.parseInt(properties.getProperty("jDispatcher.port"));
        }
        catch(Exception exc)
        {
            System.err.println("Cannot open jDispatcher properties files");
			return null;
        }
		return properties;
    }


    public static void setExperiment(String exp)
    {
	System.out.println("MdsHelper set experiment " + exp);
	experiment = exp;
    }
    public static synchronized native String getErrorString(int status);
    public static native void generateEvent(String event, int shot);
 //   public static String getErrorString(int status){return "Error message not yet implemented"; }
    public static int toPhaseId(String phase_name)
    {
        try{
            return (name_to_id.get(phase_name)).intValue();
        }catch(Exception exc) {return -1; }
    }

    public static String toPhaseName(int phase_id)
    {
        return id_to_name.get(new Integer(phase_id));
    }

    public static String getDispatcher()
    {
        return dispatcher_ip;
    }
    public static int getDispatcherPort()
    {
        return dispatcherPort;
    }
}

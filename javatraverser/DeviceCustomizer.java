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
import java.awt.*;
import javax.swing.*;
import java.util.*;
import java.io.*;
import jScope.*;
public class DeviceCustomizer extends Panel
{

   static String lastDeviceType = null;
   static String lastDeviceProvider = null;
   static String [] lastFields;
   static MdsDataProvider deviceProvider;


    String dummies[] = {":NAME", ":COMMENT",":ACT_CHANNELS", ":CLOCK_MODE",
    ":CLOCK_SOURCE", ":FREQUENCY",":TRIGGER_MODE", ":TRIG_SOURCE", ":INIT_ACTION",
    ":STORE_ACTION", ".CHANNEL_1", ".CHANNEL_1:START", ".CHANNEL_1:END",
    ".CHANNEL_1:DATA",".CHANNEL_2",".CHANNEL_2:START", ".CHANNEL_2:END",
    ".CHANNEL_2:DATA",".CHANNEL_3",".CHANNEL_3:START", ".CHANNEL_3:END",
    ".CHANNEL_3:DATA",".CHANNEL_4", ".CHANNEL_4:START", ".CHANNEL_4:END",
    ".CHANNEL_4:DATA"};
    public String[] getDeviceFields()
    {
        if(DeviceSetupBeanInfo.beanDeviceType == null)
        {
            DeviceSetupBeanInfo.beanDeviceType = JOptionPane.showInputDialog(
                "Please define the device type");
		}
        System.out.println("Device type: "+DeviceSetupBeanInfo.beanDeviceType);
        System.out.println("Inquiring Device Provider...");
		if(DeviceSetupBeanInfo.beanDeviceProvider == null)
        {
            DeviceSetupBeanInfo.beanDeviceProvider = JOptionPane.showInputDialog(
                "Please define the IP address of the device repository");
		}
        System.out.println("Device Provider: "
          + DeviceSetupBeanInfo.beanDeviceProvider );
        System.out.println("lastDeviceType = " + lastDeviceType);
		if(lastDeviceType != null && lastDeviceType.equals(DeviceSetupBeanInfo.beanDeviceType))
		    return lastFields;
		lastDeviceType = DeviceSetupBeanInfo.beanDeviceType;
		String linFields = "";
		if(deviceProvider == null || !deviceProvider.equals(DeviceSetupBeanInfo.beanDeviceProvider))
        {
		    deviceProvider = new MdsDataProvider(DeviceSetupBeanInfo.beanDeviceProvider);
		}
        byte[] linBytes = null;
        try{
            linBytes = deviceProvider.GetByteArray("JavaGetDeviceFields(\""+
		        DeviceSetupBeanInfo.beanDeviceType + "\")");
                   linFields = new String(linBytes);
		   // linFields = deviceProvider.GetString("JavaGetDeviceFields(\""+
		   //     DeviceSetupBeanInfo.beanDeviceType + "\")");
		}catch(Exception exc) {JOptionPane.showMessageDialog(null, "Cannot retrieve device field names: "+ exc);}
 		StringTokenizer st = new StringTokenizer(linFields);
		lastFields = new String[st.countTokens()];
		for(int i = 0; i < lastFields.length; i++)
		    lastFields[i] = st.nextToken();
                if(lastFields.length == 0) //If name retrieval failed
                    JOptionPane.showMessageDialog(null, "Unable to retrieve device field names: check deviceType and deviceProvider main form properties");  
		return lastFields;
    }
}

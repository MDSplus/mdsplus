import java.awt.*;
import javax.swing.*;
import java.util.*;
import java.io.*;

public class DeviceCustomizer extends Panel
{
  /* public static void main(String args[])
   {
        NetworkProvider np = new NetworkProvider("150.178.3.80");

        int i;
        
        i = Data.evaluate("2 + 3");
        i = Data.evaluate("2 > 3");
        i = Data.evaluate("2 < 3");
        i = Data.evaluate( "_pippo = 3; _pluto = 4; _pippo + _pluto");
        i = 0;

/*        DeviceSetupBeanInfo.beanDeviceType = "L6810";
        DeviceSetupBeanInfo.beanDeviceProvider = "150.178.3.80";
        String []fields = new DeviceCustomizer().getDeviceFields();
        for(int i = 0; i < fields.length; i++)
            System.out.println(fields[i]);
}*/  
    
 
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
        System.out.println("\nParte domanda");
        
        
        if(DeviceSetupBeanInfo.beanDeviceType == null)
        {
            DeviceSetupBeanInfo.beanDeviceType = JOptionPane.showInputDialog(
                "Please define the device type"); 
		}
        System.out.println("\nParte domanda 1");
		if(DeviceSetupBeanInfo.beanDeviceProvider == null)
        {
            DeviceSetupBeanInfo.beanDeviceProvider = JOptionPane.showInputDialog(
                "Please define the IP address of the device repository"); 
		}
        System.out.println("\nParte domanda 2");
		if(lastDeviceType != null && lastDeviceType.equals(DeviceSetupBeanInfo.beanDeviceType))
		    return lastFields;
		lastDeviceType = DeviceSetupBeanInfo.beanDeviceType;  
		String linFields = "";
		if(deviceProvider == null || !deviceProvider.equals(DeviceSetupBeanInfo.beanDeviceProvider))
        {
		    deviceProvider = new MdsDataProvider(DeviceSetupBeanInfo.beanDeviceProvider);
		}
        try{
		    linFields = deviceProvider.GetString("JavaGetDeviceFields(\""+ 
		        DeviceSetupBeanInfo.beanDeviceType + "\")");
		}catch(IOException e) {JOptionPane.showMessageDialog(null, "Cannot retrieve device field names");}
		StringTokenizer st = new StringTokenizer(linFields);
		lastFields = new String[st.countTokens()];
		for(int i = 0; i < lastFields.length; i++)
		    lastFields[i] = st.nextToken();
		return lastFields;
    }
}
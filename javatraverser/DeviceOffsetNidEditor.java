import java.beans.*;
import javax.swing.*;
public class DeviceOffsetNidEditor extends PropertyEditorSupport
{
    Database database = new Database("dummy", 0);
    String [] fields;
    public String [] getTags()
    {
        if(DeviceSetupBeanInfo.beanDeviceType == null)
        {
			JOptionPane.showMessageDialog(null, "Error displaying device nids", 
			    "You must define the device type FIRST!",JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return fields = database.getDeviceComponents(DeviceSetupBeanInfo.beanDeviceType);
    }
    
    public void setAsText(String field)
    {
        int i;
        if(fields == null) return;
        for(i = 0; i < fields.length && !fields[i].equals(field); i++);
        if(i == fields.length)
			JOptionPane.showMessageDialog(null, "Error displaying device nids", 
			    "Internal error: returned "+ field + " not belonging to the field set",
			    JOptionPane.WARNING_MESSAGE);
		setValue(new Integer(i));
    }
}

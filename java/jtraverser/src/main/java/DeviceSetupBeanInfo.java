import java.beans.*;
import java.awt.*;

public class DeviceSetupBeanInfo extends SimpleBeanInfo
{
    static String beanDeviceType = null;
    static String beanDeviceProvider = null;
    public PropertyDescriptor property(String name, String description)
    throws IntrospectionException
    {
	PropertyDescriptor p = new PropertyDescriptor(name, DeviceSetup.class);
	p.setShortDescription(description);
	return p;
    }
    public Image getIcon(int kind)
    {
	return loadImage("DeviceSetup.gif");
    }
    public PropertyDescriptor [] getPropertyDescriptors()
    {
	try {
	    PropertyDescriptor[] props = {
	        property("width", "width"),
	        property("height", "height"),
	        property("deviceType", "The MDS type of the device"),
	        property("deviceProvider", "The IP address of the device repository"),
	        property("deviceTitle", "The title of the device setup form"),
	        property("layout", "The Layout manager"),
	    };
	     return props;
	}catch(IntrospectionException e)
	{
	    return super.getPropertyDescriptors();
	}
    }
}

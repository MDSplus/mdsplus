import java.beans.*;
import java.awt.*;

public class DeviceButtonsBeanInfo extends SimpleBeanInfo
{
    public Image getIcon(int kind)
    {
	return loadImage("DeviceButtons.gif");
    }
    public BeanDescriptor getBeanDescriptor()
    {
	return new BeanDescriptor(DeviceButtons.class, DeviceButtonsCustomizer.class);
    }
    public PropertyDescriptor property(String name, String description)
    throws IntrospectionException
    {
	PropertyDescriptor p = new PropertyDescriptor(name, DeviceButtons.class);
	p.setShortDescription(description);
	return p;
    }
    public PropertyDescriptor [] getPropertyDescriptors()
    {
	try {
	    PropertyDescriptor[] props = {
	       property("checkExpressions", "Check expressions"),
	       property("checkMessages", "Check messages"),
	       property("methods", "Method list for the device")
	    };
	    return props;
	}catch(IntrospectionException e)
	{
	    System.out.println("DeviceButtons: property exception " + e);
	    return super.getPropertyDescriptors();
	}
    }

}

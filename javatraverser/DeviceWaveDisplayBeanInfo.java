import java.beans.*;
import java.awt.*;

public class DeviceWaveDisplayBeanInfo extends SimpleBeanInfo
{
    public Image getIcon(int kind)
    {
	return loadImage("DeviceWave.gif");
    }

    public BeanDescriptor getBeanDescriptor()
    {
	return new BeanDescriptor(DeviceWaveDisplay.class, DeviceWaveDisplayCustomizer.class);
    }
    public PropertyDescriptor property(String name, String description)
    throws IntrospectionException
    {
	PropertyDescriptor p = new PropertyDescriptor(name, DeviceChannel.class);
	p.setShortDescription(description);
	return p;
    }
    public PropertyDescriptor [] getPropertyDescriptors()
    {
	try {
	    PropertyDescriptor[] props = {
	        property("offsetNid", "Offset nid"),
	        property("prefHeight", "Preferred height")
	    };
	    return props;
	}catch(IntrospectionException e)
	{
	    System.out.println("DeviceChannel: property exception " + e);
	    return super.getPropertyDescriptors();
	}
    }

}

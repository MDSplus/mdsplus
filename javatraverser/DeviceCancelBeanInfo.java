import java.beans.*;
import java.awt.*;
public class DeviceCancelBeanInfo extends SimpleBeanInfo
{
    public PropertyDescriptor [] getPropertyDescriptors()
    {
	return new PropertyDescriptor[] {};
    }
    public Image getIcon(int kind)
    {
	return loadImage("DeviceCancel.gif");
    }

}

/* Do nothing and prevent editing by means of bean builders */

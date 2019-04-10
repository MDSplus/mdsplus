import java.beans.*;
import java.awt.*;

public class DeviceDispatchBeanInfo extends SimpleBeanInfo
{
    public Image getIcon(int kind)
    {
	return loadImage("DeviceDispatch.gif");
    }

    public PropertyDescriptor [] getPropertyDescriptors()
    {
	return new PropertyDescriptor[] {};
    }
}


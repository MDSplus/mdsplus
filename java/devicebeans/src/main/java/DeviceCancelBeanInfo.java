import java.beans.*;
import java.awt.*;

public class DeviceCancelBeanInfo extends SimpleBeanInfo
{
	@Override
	public PropertyDescriptor[] getPropertyDescriptors()
	{ return new PropertyDescriptor[] {}; }

	@Override
	public Image getIcon(int kind)
	{
		return loadImage("DeviceCancel.gif");
	}
}
/* Do nothing and prevent editing by means of bean builders */

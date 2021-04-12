import java.beans.*;
import java.awt.*;

public class DeviceOkBeanInfo extends SimpleBeanInfo
{
	@Override
	public Image getIcon(int kind)
	{
		return loadImage("DeviceOk.gif");
	}

	@Override
	public PropertyDescriptor[] getPropertyDescriptors()
	{ return new PropertyDescriptor[] {}; }
}
/* Do nothing and prevent editing by means of bean builders */

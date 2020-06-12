import java.beans.*;
import java.awt.*;

public class DeviceApplyBeanInfo extends SimpleBeanInfo
{
	@Override
	public Image getIcon(int kind)
	{
		return loadImage("DeviceApply.gif");
	}

	@Override
	public PropertyDescriptor[] getPropertyDescriptors()
	{ return new PropertyDescriptor[] {}; }
}
/* Do nothing and prevent editing by means of bean builders */

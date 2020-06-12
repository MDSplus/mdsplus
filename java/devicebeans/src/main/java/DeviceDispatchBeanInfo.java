import java.beans.*;
import java.awt.*;

public class DeviceDispatchBeanInfo extends SimpleBeanInfo
{
	@Override
	public Image getIcon(int kind)
	{
		return loadImage("DeviceDispatch.gif");
	}

	@Override
	public PropertyDescriptor[] getPropertyDescriptors()
	{ return new PropertyDescriptor[] {}; }
}

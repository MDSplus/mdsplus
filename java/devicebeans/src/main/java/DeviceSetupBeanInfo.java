import java.beans.*;
import java.awt.*;

public class DeviceSetupBeanInfo extends SimpleBeanInfo
{
	static String beanDeviceType = null;
	static String beanDeviceProvider = null;

	@Override
	public Image getIcon(int kind)
	{
		return loadImage("DeviceSetup.gif");
	}

	@Override
	public PropertyDescriptor[] getPropertyDescriptors()
	{
		try
		{
			final PropertyDescriptor[] props =
			{ property("width", "width"), property("height", "height"),
					property("deviceType", "The MDS type of the device"),
					property("deviceProvider", "The IP address of the device repository"),
					property("deviceTitle", "The title of the device setup form"),
					property("layout", "The Layout manager"), property("updateEvent", "The update event name") };
			return props;
		}
		catch (final IntrospectionException e)
		{
			return super.getPropertyDescriptors();
		}
	}

	public PropertyDescriptor property(String name, String description) throws IntrospectionException
	{
		final PropertyDescriptor p = new PropertyDescriptor(name, DeviceSetup.class);
		p.setShortDescription(description);
		return p;
	}
}

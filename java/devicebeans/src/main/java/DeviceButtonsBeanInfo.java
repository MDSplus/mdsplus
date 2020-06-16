import java.beans.*;
import java.awt.*;

public class DeviceButtonsBeanInfo extends SimpleBeanInfo
{
	@Override
	public BeanDescriptor getBeanDescriptor()
	{ return new BeanDescriptor(DeviceButtons.class, DeviceButtonsCustomizer.class); }

	@Override
	public Image getIcon(int kind)
	{
		return loadImage("DeviceButtons.gif");
	}

	@Override
	public PropertyDescriptor[] getPropertyDescriptors()
	{
		try
		{
			final PropertyDescriptor[] props =
			{ property("checkExpressions", "Check expressions"), property("checkMessages", "Check messages"),
					property("methods", "Method list for the device") };
			return props;
		}
		catch (final IntrospectionException e)
		{
			System.out.println("DeviceButtons: property exception " + e);
			return super.getPropertyDescriptors();
		}
	}

	public PropertyDescriptor property(String name, String description) throws IntrospectionException
	{
		final PropertyDescriptor p = new PropertyDescriptor(name, DeviceButtons.class);
		p.setShortDescription(description);
		return p;
	}
}

import java.beans.*;
import java.awt.*;

public class DeviceInputsBeanInfo extends SimpleBeanInfo
{
	@Override
	public BeanDescriptor getBeanDescriptor()
	{ return new BeanDescriptor(DeviceInputs.class, DeviceInputsCustomizer.class); }

	@Override
	public Image getIcon(int kind)
	{
		return loadImage("DeviceChannel.gif");
	}

	@Override
	public PropertyDescriptor[] getPropertyDescriptors()
	{
		try
		{
			final PropertyDescriptor[] props =
			{ property("offsetNid", "Offset nid")};
			return props;
		}
		catch (final IntrospectionException e)
		{
			System.out.println("DeviceInputs: property exception " + e);
			return super.getPropertyDescriptors();
		}
	}

	public PropertyDescriptor property(String name, String description) throws IntrospectionException
	{
		final PropertyDescriptor p = new PropertyDescriptor(name, DeviceInputs.class);
		p.setShortDescription(description);
		return p;
	}
}

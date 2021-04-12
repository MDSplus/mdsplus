import java.beans.*;
import java.awt.*;

public class DeviceChannelBeanInfo extends SimpleBeanInfo
{
	@Override
	public BeanDescriptor getBeanDescriptor()
	{
		final BeanDescriptor desc = new BeanDescriptor(DeviceChannel.class, DeviceChannelCustomizer.class);
		desc.setValue("allowedChildTypes", new String[]
		{ "DeviceComponent" });
		desc.setValue("disallowedChildTypes", new String[] {});
		desc.setValue("isContainer", Boolean.TRUE);
		desc.setValue("containerDelegate", "getContainer");
		return desc;
	}

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
			{ property("labelString", "Label string"), property("offsetNid", "Offset nid"),
					property("borderVisible", "Display border"), property("lines", "Number of lines"),
					property("columns", "Number of Columns"), property("inSameLine", "All Items in same line"),
					property("showState", "Display channel state"), property("updateIdentifier", "Show identifier"),
					property("showVal", "Show value"), property("layout", "Layout Manager") };
			return props;
		}
		catch (final IntrospectionException e)
		{
			System.out.println("DeviceChannel: property exception " + e);
			return super.getPropertyDescriptors();
		}
	}

	public PropertyDescriptor property(String name, String description) throws IntrospectionException
	{
		final PropertyDescriptor p = new PropertyDescriptor(name, DeviceChannel.class);
		p.setShortDescription(description);
		return p;
	}
}

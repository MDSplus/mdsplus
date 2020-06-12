import java.beans.*;
import java.awt.*;

public class DeviceWaveDisplayBeanInfo extends SimpleBeanInfo
{
	@Override
	public BeanDescriptor getBeanDescriptor()
	{ return new BeanDescriptor(DeviceWaveDisplay.class, DeviceWaveDisplayCustomizer.class); }

	@Override
	public Image getIcon(int kind)
	{
		return loadImage("DeviceWave.gif");
	}

	@Override
	public PropertyDescriptor[] getPropertyDescriptors()
	{
		try
		{
			final PropertyDescriptor[] props =
			{ property("offsetNid", "Offset nid"), property("prefHeight", "Preferred height") };
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

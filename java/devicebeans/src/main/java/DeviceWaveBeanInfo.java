import java.awt.Image;
import java.beans.BeanDescriptor;
import java.beans.IntrospectionException;
import java.beans.PropertyDescriptor;
import java.beans.SimpleBeanInfo;

public class DeviceWaveBeanInfo extends SimpleBeanInfo
{
	@Override
	public BeanDescriptor getBeanDescriptor()
	{ return new BeanDescriptor(DeviceWave.class, DeviceWaveCustomizer.class); }

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
			{ property("offsetNid", "Offset nid"), property("minXVisible", "Display min X"),
					property("maxXVisible", "Display max X"), property("minYVisible", "Display min Y"),
					property("maxYVisible", "Display max Y"), property("identifier", "Optional field identifier"),
					property("updateIdentifier", "Update identifier"),
					property("updateExpression", "Update expression"), property("prefHeight", "Preferred height") };
			return props;
		}
		catch (final IntrospectionException e)
		{
			System.out.println("DeviceWave: property exception " + e);
			return super.getPropertyDescriptors();
		}
	}

	public PropertyDescriptor property(String name, String description) throws IntrospectionException
	{
		final PropertyDescriptor p = new PropertyDescriptor(name, DeviceWave.class);
		p.setShortDescription(description);
		return p;
	}
}

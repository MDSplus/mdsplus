import java.beans.*;
import java.awt.*;

public class DeviceParametersBeanInfo extends SimpleBeanInfo
{
	@Override
	public BeanDescriptor getBeanDescriptor()
	{ return new BeanDescriptor(DeviceParameters.class, DeviceParametersCustomizer.class); }

	@Override
	public Image getIcon(int kind)
	{
		return loadImage("DeviceLabel.gif");
	}

	@Override
	public PropertyDescriptor[] getPropertyDescriptors()
	{
		try
		{
			final PropertyDescriptor[] props =
			{ property("offsetNid", "Offset nid"), 
                          property("parameterOffset", "Parameter offset"), 
                          property("numParameters", "Number of parameters")};
			return props;
		}
		catch (final IntrospectionException e)
		{
			System.out.println("DeviceField: property exception " + e);
			return super.getPropertyDescriptors();
		}
	}

	public PropertyDescriptor property(String name, String description) throws IntrospectionException
	{
		final PropertyDescriptor p = new PropertyDescriptor(name, DeviceParameters.class);
		p.setShortDescription(description);
		return p;
	}
}

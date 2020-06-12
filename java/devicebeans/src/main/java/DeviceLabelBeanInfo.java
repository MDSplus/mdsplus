import java.beans.*;
import java.awt.*;

public class DeviceLabelBeanInfo extends SimpleBeanInfo
{
	@Override
	public BeanDescriptor getBeanDescriptor()
	{ return new BeanDescriptor(DeviceLabel.class, DeviceLabelCustomizer.class); }

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
			{ property("offsetNid", "Offset nid"), property("textOnly", "Deals only with text"),
					property("labelString", "Field label"), property("numCols", "Number of columns"),
					property("identifier", "Optional field identifier"),
//               property("showState", "Display on/off state"),
//               property("editable", "Text field editable"),
					property("displayEvaluated", "Display evaluated data"),
					property("preferredWidth", "Preferred width") };
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
		final PropertyDescriptor p = new PropertyDescriptor(name, DeviceLabel.class);
		p.setShortDescription(description);
		return p;
	}
}

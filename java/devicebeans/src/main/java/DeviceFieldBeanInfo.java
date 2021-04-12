import java.awt.Image;
import java.beans.BeanDescriptor;
import java.beans.IntrospectionException;
import java.beans.PropertyDescriptor;
import java.beans.SimpleBeanInfo;

public class DeviceFieldBeanInfo extends SimpleBeanInfo
{
	@Override
	public BeanDescriptor getBeanDescriptor()
	{ return new BeanDescriptor(DeviceField.class, DeviceFieldCustomizer.class); }

	@Override
	public Image getIcon(int kind)
	{
		return loadImage("DeviceField.gif");
	}

	@Override
	public PropertyDescriptor[] getPropertyDescriptors()
	{
		try
		{
			final PropertyDescriptor[] props =
			{ property("offsetNid", "Offset nid"), property("textOnly", "Deals only with text"),
					property("labelString", "Field label"), property("numCols", "Number of columns"),
					property("identifier", "Optional field identifier"), property("showState", "Display on/off state"),
					property("editable", "Text field editable"), property("displayEvaluated", "Display evaluated data"),
					property("preferredWidth", "Preferred width"), property("refShot", "Reference shot number") };
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
		final PropertyDescriptor p = new PropertyDescriptor(name, DeviceField.class);
		p.setShortDescription(description);
		return p;
	}
}

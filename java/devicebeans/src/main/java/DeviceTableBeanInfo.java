import java.beans.*;
import java.awt.*;

public class DeviceTableBeanInfo extends SimpleBeanInfo
{
	@Override
	public BeanDescriptor getBeanDescriptor()
	{ return new BeanDescriptor(DeviceTable.class, DeviceTableCustomizer.class); }

	@Override
	public Image getIcon(int kind)
	{
		return loadImage("DeviceTable.gif");
	}

	@Override
	public PropertyDescriptor[] getPropertyDescriptors()
	{
		try
		{
			final PropertyDescriptor[] props =
			{ property("offsetNid", "Offset nid"), property("labelString", "Field label"),
					property("numCols", "Number of columns"), property("numRows", "Number of rows"),
					property("identifier", "Optional field identifier"), property("columnNames", "Column names"),
					property("rowNames", "Row names"), property("editable", "Editable"), property("binary", "Binary"),
					property("displayRowNumber", "displayRowNumber"),
					property("preferredColumnWidth", "preferredColumnWidth"),
					property("preferredHeight", "preferredHeight"), property("useExpressions", "Use Expressions"),
					property("refMode", "Reflexion Mode") };
			return props;
		}
		catch (final IntrospectionException e)
		{
			System.out.println("DeviceTable: property exception " + e);
			return super.getPropertyDescriptors();
		}
	}

	public PropertyDescriptor property(String name, String description) throws IntrospectionException
	{
		final PropertyDescriptor p = new PropertyDescriptor(name, DeviceTable.class);
		p.setShortDescription(description);
		return p;
	}
}

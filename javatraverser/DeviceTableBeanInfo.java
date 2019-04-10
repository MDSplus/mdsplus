import java.beans.*;
import java.awt.*;

public class DeviceTableBeanInfo extends SimpleBeanInfo
{
    public PropertyDescriptor property(String name, String description)
    throws IntrospectionException
    {
	PropertyDescriptor p = new PropertyDescriptor(name, DeviceTable.class);
	p.setShortDescription(description);
	return p;
    }
    public Image getIcon(int kind)
    {
	return loadImage("DeviceTable.gif");
    }

    public BeanDescriptor getBeanDescriptor()
    {
	return new BeanDescriptor(DeviceTable.class, DeviceTableCustomizer.class);
    }
    public PropertyDescriptor [] getPropertyDescriptors()
    {
	try {
	    PropertyDescriptor[] props = {
	       property("offsetNid", "Offset nid"),
	       property("labelString", "Field label"),
	       property("numCols", "Number of columns"),
	       property("numRows", "Number of rows"),
	       property("identifier", "Optional field identifier"),
	       property("columnNames", "Column names"),
	       property("rowNames", "Row names"),
	       property("editable", "Editable"),
	       property("binary", "Binary"),
	       property("displayRowNumber", "displayRowNumber"),
	       property("preferredColumnWidth", "preferredColumnWidth"),
	       property("preferredHeight", "preferredHeight"),
	       property("useExpressions", "Use Expressions"),
	       property("refMode", "Reflexion Mode")
	    };
	    return props;
	}catch(IntrospectionException e)
	{
	    System.out.println("DeviceTable: property exception " + e);
	    return super.getPropertyDescriptors();
	}
    }

}

import java.beans.*;
import java.awt.*;

public class DeviceLabelBeanInfo extends SimpleBeanInfo
{
    public PropertyDescriptor property(String name, String description)
    throws IntrospectionException
    {
	PropertyDescriptor p = new PropertyDescriptor(name, DeviceLabel.class);
	p.setShortDescription(description);
	return p;
    }
    public Image getIcon(int kind)
    {
	return loadImage("DeviceLabel.gif");
    }

    public BeanDescriptor getBeanDescriptor()
    {
	return new BeanDescriptor(DeviceLabel.class, DeviceLabelCustomizer.class);
    }
    public PropertyDescriptor [] getPropertyDescriptors()
    {
	try {
	    PropertyDescriptor[] props = {
	       property("offsetNid", "Offset nid"),
	       property("textOnly", "Deals only with text"),
	       property("labelString", "Field label"),
	       property("numCols", "Number of columns"),
	       property("identifier", "Optional field identifier"),
//               property("showState", "Display on/off state"),
//               property("editable", "Text field editable"),
	       property("displayEvaluated", "Display evaluated data"),
	       property("preferredWidth", "Preferred width")
	    };
	    return props;
	}catch(IntrospectionException e)
	{
	    System.out.println("DeviceField: property exception " + e);
	    return super.getPropertyDescriptors();
	}
    }

}

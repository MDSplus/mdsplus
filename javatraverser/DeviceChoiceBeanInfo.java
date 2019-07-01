import java.beans.*;
import java.awt.*;

public class DeviceChoiceBeanInfo extends SimpleBeanInfo
{
    public PropertyDescriptor property(String name, String description)
    throws IntrospectionException
    {
	PropertyDescriptor p = new PropertyDescriptor(name, DeviceChoice.class);
	p.setShortDescription(description);
	return p;
    }
    public Image getIcon(int kind)
    {
	return loadImage("DeviceChoice.gif");
    }

    public PropertyDescriptor [] getPropertyDescriptors()
    {
	try {
	    PropertyDescriptor[] props = {
	       property("choiceIntValues", "Integer items"),
	       property("choiceFloatValues", "Float items"),
	       property("convert", "Enable code conversion"),
	       property("offsetNid", "Offset of the associated node in the conglomerate - MANDATORY!!"),
	       property("labelString", "Associated label text"),
	       property("showState", "Enable state check box"),
	       property("convert", "Enable code conversion"),
	       property("choiceItems", "String items"),
	       property("updateIdentifier", "Update identifier"),
	       property("identifier", "Optional identifier")

	    };
	    props[0].setPropertyEditorClass(IntArrayEditor.class);
	    props[1].setPropertyEditorClass(FloatArrayEditor.class);
	    return props;
	}catch(IntrospectionException e)
	{
	    System.out.println("Exception in DeviceChoice: " + e);
	    return super.getPropertyDescriptors();
	}
    }
    public BeanDescriptor getBeanDescriptor()
    {
	return new BeanDescriptor(DeviceChoice.class, DeviceChoiceCustomizer.class);
    }
}

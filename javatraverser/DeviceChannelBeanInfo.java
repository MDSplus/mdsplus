import java.beans.*;
import java.awt.*;

public class DeviceChannelBeanInfo extends SimpleBeanInfo
{
    public Image getIcon(int kind)
    {
        return loadImage("DeviceChannel.gif");
    }
    
    public BeanDescriptor getBeanDescriptor()
    {
        return new BeanDescriptor(DeviceChannel.class, DeviceChannelCustomizer.class);
    }
    public PropertyDescriptor property(String name, String description)
    throws IntrospectionException
    {
        PropertyDescriptor p = new PropertyDescriptor(name, DeviceChannel.class);
        p.setShortDescription(description);
        return p;
    }
    public PropertyDescriptor [] getPropertyDescriptors()
    {
        try {
            PropertyDescriptor[] props = {
               property("labelString", "Label string"),
               property("offsetNid", "Offset nid"),
               property("borderVisible", "Display border"),
               property("lines", "Number of lines"),
               property("columns", "Number of Columns"),
               property("inSameLine", "All Items in same line"),
               property("showState", "Display channel state"),
               property("updateIdentifier", "Show identifier"),
               property("showVal", "Show value"),
               property("layout", "Layout Manager")
            };
            return props;
        }catch(IntrospectionException e)
        {
            System.out.println("DeviceChannel: property exception " + e);
            return super.getPropertyDescriptors();
        }
    }

}

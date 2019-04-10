import java.beans.*;
import java.awt.*;

public class DeviceApplyBeanInfo extends SimpleBeanInfo
{
    public PropertyDescriptor [] getPropertyDescriptors()
    {
        return new PropertyDescriptor[] {};
    }
    public Image getIcon(int kind)
    {
        return loadImage("DeviceApply.gif");
    }
}

/* Do nothing and prevent editing by means of bean builders */

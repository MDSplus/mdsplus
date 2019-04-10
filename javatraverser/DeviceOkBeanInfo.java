import java.beans.*;
import java.awt.*;

public class DeviceOkBeanInfo extends SimpleBeanInfo
{
    public Image getIcon(int kind)
    {
        return loadImage("DeviceOk.gif");
    }

    public PropertyDescriptor [] getPropertyDescriptors()
    {
        return new PropertyDescriptor[] {};
    }
}

/* Do nothing and prevent editing by means of bean builders */
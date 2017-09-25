/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
import java.beans.*;
import java.awt.*;

public class DeviceButtonsBeanInfo extends SimpleBeanInfo
{
    public Image getIcon(int kind)
    {
        return loadImage("DeviceButtons.gif");
    }
    public BeanDescriptor getBeanDescriptor()
    {
        return new BeanDescriptor(DeviceButtons.class, DeviceButtonsCustomizer.class);
    }
    public PropertyDescriptor property(String name, String description)
    throws IntrospectionException
    {
        PropertyDescriptor p = new PropertyDescriptor(name, DeviceButtons.class);
        p.setShortDescription(description);
        return p;
    }
    public PropertyDescriptor [] getPropertyDescriptors()
    {
        try {
            PropertyDescriptor[] props = {
               property("checkExpressions", "Check expressions"),
               property("checkMessages", "Check messages"),
               property("methods", "Method list for the device")
            };
            return props;
        }catch(IntrospectionException e)
        {
            System.out.println("DeviceButtons: property exception " + e);
            return super.getPropertyDescriptors();
        }
    }
    
}

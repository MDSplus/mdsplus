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
//package jTraverser;
import java.beans.*;
import java.lang.reflect.*;
import java.awt.*;

public class NodeBeanInfo extends SimpleBeanInfo
{
    int usage;
    RemoteTree experiment;
    String name;

    /* This constructor is intended to be used only by the traverser and not
    by a standard Bean Bulder, which  considers only the class to discriminate the
    corresponding BeanInfo object */
    public NodeBeanInfo(RemoteTree experiment, int usage, String name)
    {
	this.experiment  = experiment;
	this.usage = usage;
	this.name = name;
    }
    /*
    public Image getIcon(int kind)
    {
	switch (usage) {
	    case NodeInfo.USAGE_NONE: return loadImage("structure.gif");
	    case NodeInfo.USAGE_ACTION: return loadImage("action.gif");
	    case NodeInfo.USAGE_DEVICE: return loadImage("device.gif");
	    case NodeInfo.USAGE_DISPATCH: return loadImage("dispatch.gif");
	    case NodeInfo.USAGE_ANY:
	    case NodeInfo.USAGE_NUMERIC: return loadImage("numeric.gif");
	    case NodeInfo.USAGE_TASK: return loadImage("task.gif");
	    case NodeInfo.USAGE_TEXT: return loadImage("text.gif");
	    case NodeInfo.USAGE_WINDOW: return loadImage("window.gif");
	    case NodeInfo.USAGE_AXIS: return loadImage("axis.gif");
	    case NodeInfo.USAGE_SUBTREE: return loadImage("subtree.gif");
	    case NodeInfo.USAGE_COMPOUND_DATA: return loadImage("compound.gif");
	}
    return null;
    }
*/
    public BeanDescriptor getBeanDescriptor()
    {
      // per ora non definisco customizers
	return new BeanDescriptor(Node.class, null);
    }
    static PropertyDescriptor property(String name, String description)
	throws IntrospectionException
    {
	PropertyDescriptor p = new PropertyDescriptor(name, Node.class);
	p.setShortDescription(description);
	return p;
    }
    boolean isSupported(String short_descr)
    {
	if(usage == NodeInfo.USAGE_STRUCTURE || usage == NodeInfo.USAGE_NONE)
	    if(short_descr.equals("Display Data") || short_descr.equals("Modify Data"))
		return false;
	if(usage != NodeInfo.USAGE_ACTION && usage != NodeInfo.USAGE_TASK)
	    if(short_descr.equals("Do Action"))
		return false;
	if(usage != NodeInfo.USAGE_DEVICE)
	    if(short_descr.equals("Setup Device"))
		return false;
    try {
	    if(experiment.isReadonly() && (short_descr.equals("Modify Data") || short_descr.equals("Toggle On/Off")))
	        return false;
	}catch(Exception exc){return false;}
	return true;
    }


    public PropertyDescriptor[] getPropertyDescriptors()
    {
	try {
		PropertyDescriptor[] props = {
		    property("data", "Display Data"),
            property("info", "Display Nci"),
            property("info", "Display Tags"),
		    property("data", "Modify Data")};
		props[0].setPropertyEditorClass(NodeDisplayData.class);
        props[1].setPropertyEditorClass(NodeDisplayNci.class);
        props[2].setPropertyEditorClass(NodeDisplayTags.class);
		props[3].setPropertyEditorClass(NodeModifyData.class);
		return props;
	} catch(IntrospectionException e)
	{ return super.getPropertyDescriptors();}
    }
    static MethodDescriptor method(String name, String description)
	throws NoSuchMethodException, SecurityException
    {
	Method m = Node.class.getMethod(name, new Class[] {});
	MethodDescriptor md = new MethodDescriptor(m);
	md.setShortDescription(description);
	return md;
    }

    public MethodDescriptor[] getMethodDescriptors()
    {
	try{
		MethodDescriptor [] methods = {
		    method("setupDevice", "Setup Device"),
		    method("toggle", "Toggle On/Off"),
		    method("setDefault", "Set Default"),
		    method("doAction", "Do Action")};
		return methods;
	} catch (Exception e)
	{
	    return super.getMethodDescriptors();
	}
    }
}


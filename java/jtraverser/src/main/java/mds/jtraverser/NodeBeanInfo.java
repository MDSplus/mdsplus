package mds.jtraverser;

//package jTraverser;
import java.beans.BeanDescriptor;
import java.beans.IntrospectionException;
import java.beans.MethodDescriptor;
import java.beans.PropertyDescriptor;
import java.beans.SimpleBeanInfo;
import java.lang.reflect.Method;

public class NodeBeanInfo extends SimpleBeanInfo
{
	static MethodDescriptor method(String name, String description) throws NoSuchMethodException, SecurityException
	{
		final Method m = Node.class.getMethod(name, new Class[] {});
		final MethodDescriptor md = new MethodDescriptor(m);
		md.setShortDescription(description);
		return md;
	}

	static PropertyDescriptor property(String name, String description) throws IntrospectionException
	{
		final PropertyDescriptor p = new PropertyDescriptor(name, Node.class);
		p.setShortDescription(description);
		return p;
	}

	String usage;
	MDSplus.Tree experiment;
	String name;

	/*
	 * This constructor is intended to be used only by the traverser and not by a
	 * standard Bean Bulder, which considers only the class to discriminate the
	 * corresponding BeanInfo object
	 */
	public NodeBeanInfo(MDSplus.Tree experiment, String usage, String name)
	{
		this.experiment = experiment;
		this.usage = usage;
		this.name = name;
	}

	/*
	 * public Image getIcon(int kind) { switch (usage) { case NodeInfo.USAGE_NONE:
	 * return loadImage("structure.gif"); case NodeInfo.USAGE_ACTION: return
	 * loadImage("action.gif"); case NodeInfo.USAGE_DEVICE: return
	 * loadImage("device.gif"); case NodeInfo.USAGE_DISPATCH: return
	 * loadImage("dispatch.gif"); case NodeInfo.USAGE_ANY: case
	 * NodeInfo.USAGE_NUMERIC: return loadImage("numeric.gif"); case
	 * NodeInfo.USAGE_TASK: return loadImage("task.gif"); case NodeInfo.USAGE_TEXT:
	 * return loadImage("text.gif"); case NodeInfo.USAGE_WINDOW: return
	 * loadImage("window.gif"); case NodeInfo.USAGE_AXIS: return
	 * loadImage("axis.gif"); case NodeInfo.USAGE_SUBTREE: return
	 * loadImage("subtree.gif"); case NodeInfo.USAGE_COMPOUND_DATA: return
	 * loadImage("compound.gif"); } return null; }
	 */
	@Override
	public BeanDescriptor getBeanDescriptor()
	{
		// per ora non definisco customizers
		return new BeanDescriptor(Node.class, null);
	}

	@Override
	public MethodDescriptor[] getMethodDescriptors()
	{
		try
		{
			final MethodDescriptor[] methods =
			{ method("setupDevice", "Setup Device"), method("toggle", "Toggle On/Off"),
					method("setDefault", "Set Default"), method("doAction", "Do Action") };
			return methods;
		}
		catch (final Exception e)
		{
			return super.getMethodDescriptors();
		}
	}

	@Override
	public PropertyDescriptor[] getPropertyDescriptors()
	{
		try
		{
			final PropertyDescriptor[] props =
			{ property("data", "Display Data"), property("info", "Display Nci"), property("info", "Display Tags"),
					property("data", "Modify Data") };
			props[0].setPropertyEditorClass(NodeDisplayData.class);
			props[1].setPropertyEditorClass(NodeDisplayNci.class);
			props[2].setPropertyEditorClass(NodeDisplayTags.class);
			props[3].setPropertyEditorClass(NodeModifyData.class);
			return props;
		}
		catch (final IntrospectionException e)
		{
			return super.getPropertyDescriptors();
		}
	}

	boolean isSupported(String short_descr)
	{
		if (usage.equals("STRUCTURE ") || usage.equals("NONE"))
		{
			if (short_descr.equals("Display Data") || short_descr.equals("Modify Data")
					|| short_descr.equals("Setup Device") || short_descr.equals("Do Action"))
				return false;
		}
		else if (usage.equals("ACTION") || usage.equals("TASK"))
		{
			if (short_descr.equals("Setup Device"))
				return false;
			if (short_descr.equals("Do Action"))
				return true;
		}
		else if (usage.equals("DEVICE"))
		{
			if (short_descr.equals("Modify Data") || short_descr.equals("Do Action"))
				return false;
		}
		else
		{
			if (short_descr.equals("Setup Device") || short_descr.equals("Do Action"))
				return false;
		}
		try
		{
			if (experiment.isReadOnly() && (short_descr.equals("Modify Data") || short_descr.equals("Toggle On/Off")))
				return false;
		}
		catch (final Exception exc)
		{
			return false;
		}
		return true;
	}
}

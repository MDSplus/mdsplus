package mds.jtraverser;

//package jTraverser;
import java.beans.*;
import java.awt.*;

public class NodeInfoPropertyEditor implements PropertyEditor
{
	protected MDSplus.Data data;

	// event notification not used here
	@Override
	public void addPropertyChangeListener(PropertyChangeListener l)
	{}

	@Override
	public String getAsText()
	{ return null; }

	@Override
	public Component getCustomEditor()
	{ return new NodeEditor(); }

	@Override
	public String getJavaInitializationString()
	{ return null; }

	@Override
	public String[] getTags()
	{ return null; }

	@Override
	public Object getValue()
	{ return data; }

	@Override
	public boolean isPaintable()
	{ return false; }

	@Override
	public void paintValue(Graphics g, Rectangle r)
	{}

	@Override
	public void removePropertyChangeListener(PropertyChangeListener l)
	{}

	@Override
	public void setAsText(String s)
	{}

	@Override
	public void setValue(Object o)
	{ data = (MDSplus.Data) o; }

	@Override
	public boolean supportsCustomEditor()
	{
		return true;
	}
}

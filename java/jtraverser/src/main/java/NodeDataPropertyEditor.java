
//package jTraverser;
import java.beans.*;
import java.awt.*;

public abstract class NodeDataPropertyEditor implements PropertyEditor
{
	protected MDSplus.Data data;

	@Override
	public void setValue(Object o)
	{ data = (MDSplus.Data) o; }

	@Override
	public Object getValue()
	{ return data; }

	@Override
	public void setAsText(String s)
	{
		try
		{
			data = Tree.dataFromExpr(s);
		}
		catch (final Exception e)
		{
			data = null;
		}
	}

	@Override
	public String getAsText()
	{
		try
		{
			return "" + data;
		}
		catch (final Exception e)
		{
			return null;
		}
	}

	@Override
	public String[] getTags()
	{ return null; }

	@Override
	public boolean supportsCustomEditor()
	{
		return true;
	}

	@Override
	public abstract Component getCustomEditor(); // to be subclassed

	@Override
	public boolean isPaintable()
	{ return false; }

	@Override
	public void paintValue(Graphics g, Rectangle r)
	{}

	@Override
	public String getJavaInitializationString()
	{ return null; }

	// event notification not used here
	@Override
	public void addPropertyChangeListener(PropertyChangeListener l)
	{}

	@Override
	public void removePropertyChangeListener(PropertyChangeListener l)
	{}
}

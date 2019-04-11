//package jTraverser;
import java.beans.*;
import java.awt.*;

public abstract class NodeDataPropertyEditor implements PropertyEditor {
    protected Data data;

    public void setValue(Object o){data = (Data)o;}
    public Object getValue() {return data;}
    public void setAsText(String s)
    {
	try {
	    data = Tree.dataFromExpr(s);
	    } catch (Exception e) {data = null; }
    }
    public String getAsText() {
	try {
	    return Tree.dataToString(data);
	    } catch (Exception e) {return null; }
    }
    public String []getTags() {return null;}
    public boolean  supportsCustomEditor() {return true;}
    public abstract Component getCustomEditor(); //to be subclassed
    public boolean isPaintable() {return false; }
    public void paintValue(Graphics g, Rectangle r){}
    public String getJavaInitializationString() {return null; }
    // event notification not used here
    public void addPropertyChangeListener(PropertyChangeListener l){}
    public void removePropertyChangeListener(PropertyChangeListener l){}
}
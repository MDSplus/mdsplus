//package jTraverser;
import java.beans.*;
import java.awt.*;

public class NodeInfoPropertyEditor implements PropertyEditor {
    protected Data data;

    public void setValue(Object o){data = (Data)o;}
    public Object getValue() {return data;}
    public void setAsText(String s){}
    public String getAsText() {return null; }
    public String []getTags() {return null;}
    public boolean  supportsCustomEditor() {return true;}
    public Component getCustomEditor(){return new NodeEditor(); }
    public boolean isPaintable() {return false; }
    public void paintValue(Graphics g, Rectangle r){}
    public String getJavaInitializationString() {return null; }
    // event notification not used here
    public void addPropertyChangeListener(PropertyChangeListener l){}
    public void removePropertyChangeListener(PropertyChangeListener l){}
}
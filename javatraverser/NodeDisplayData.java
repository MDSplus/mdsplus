//package jTraverser;
import java.awt.*;

public class NodeDisplayData extends NodeDataPropertyEditor {
    public Component getCustomEditor(){
    NodeEditor ne = new DisplayData();
    return ne;
    }
}
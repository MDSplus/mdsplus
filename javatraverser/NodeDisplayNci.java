//package jTraverser;
import java.awt.*;

public class NodeDisplayNci extends NodeDataPropertyEditor {
    public Component getCustomEditor(){
    NodeEditor ne = new DisplayNci();
    return ne;
    }
}
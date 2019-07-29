//package jTraverser;
import java.awt.*;

public class NodeDisplayTags extends NodeDataPropertyEditor {
    public Component getCustomEditor(){
    NodeEditor ne = new DisplayTags();
    return ne;
    }
}

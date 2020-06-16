package mds.jtraverser;

//package jTraverser;
import java.awt.*;

public class NodeDisplayTags extends NodeDataPropertyEditor
{
	@Override
	public Component getCustomEditor()
	{
		final NodeEditor ne = new DisplayTags();
		return ne;
	}
}

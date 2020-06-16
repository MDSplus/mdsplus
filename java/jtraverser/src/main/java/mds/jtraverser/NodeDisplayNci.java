package mds.jtraverser;

//package jTraverser;
import java.awt.*;

public class NodeDisplayNci extends NodeDataPropertyEditor
{
	@Override
	public Component getCustomEditor()
	{
		final NodeEditor ne = new DisplayNci();
		return ne;
	}
}

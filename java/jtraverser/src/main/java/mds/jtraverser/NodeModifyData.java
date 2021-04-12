package mds.jtraverser;

//package jTraverser;
import java.awt.*;

public class NodeModifyData extends NodeDataPropertyEditor
{
	@Override
	public Component getCustomEditor()
	{ return new ModifyData(); }
}

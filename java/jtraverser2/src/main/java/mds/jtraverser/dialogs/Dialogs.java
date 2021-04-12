package mds.jtraverser.dialogs;

import java.awt.Component;
import java.awt.Point;
import java.awt.Window;
import javax.swing.JOptionPane;

import mds.jtraverser.TreeManager;

public final class Dialogs
{
	public static final void setLocation(final Window window)
	{
		Component parent = window.getParent();
		if (parent == null)
			parent = JOptionPane.getRootFrame();
		if (parent == null)
			return;
		final Point parloc = parent.getLocation();
		window.setLocation(parloc.x + 32, parloc.y + 32);
	}

	public final AddNodeDialog addNode;
	public final FlagsDialog modifyFlags;

	public Dialogs(final TreeManager treeman)
	{
		this.modifyFlags = new FlagsDialog(treeman);
		this.addNode = new AddNodeDialog();
	}

	public final void update()
	{
		this.modifyFlags.update();
	}
}

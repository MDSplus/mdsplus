
//package jTraverser;
import javax.swing.JFrame;

class TreeDialog extends JFrame
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	boolean in_use;
	NodeEditor node_editor;

	public TreeDialog(NodeEditor editor)
	{
		in_use = true;
		getContentPane().add(editor);
		node_editor = editor;
		if (editor instanceof DisplayNci)
			setResizable(false);
	}

	public final boolean inUse()
	{
		return in_use;
	}

	public final NodeEditor getEditor()
	{ return node_editor; }

	@Override
	public void dispose()
	{
		in_use = false;
		setVisible(false);
	}

	public void repack()
	{
		if (!isVisible())
			return;
		pack();
	}

	public final void setUsed(boolean used)
	{ in_use = used; }
}

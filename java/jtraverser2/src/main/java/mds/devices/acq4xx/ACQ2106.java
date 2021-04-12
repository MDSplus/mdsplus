package mds.devices.acq4xx;

import java.awt.BorderLayout;
import java.awt.Frame;
import java.awt.GridLayout;
import java.lang.reflect.Constructor;
import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;

import mds.MdsException;
import mds.data.descriptor_s.NODE;
import mds.devices.Device;
import mds.jtraverser.editor.Editor;
import mds.jtraverser.editor.EnumEditor;
import mds.jtraverser.editor.EnumEditor.MODE;

public class ACQ2106 extends Device
{
	final ACQ4xx[] modules;
	final ACQ4xx.Master master;
	final Class<?> module_class;

	public ACQ2106(final Frame frame, final NODE<?> head, final boolean editable, final int num_modules,
			final Class<?> module_class)
	{
		super(frame, head, editable, 8);
		this.module_class = module_class;
		final JComponent main = this.pane;
		main.setLayout(new BorderLayout());
		this.pane = new JPanel(new GridLayout(0, 1));
		this.addExpr(0, "CLOCK", "sampling clock", false, false);
		this.addExpr(1, "HOST", "hostname/ip of acq", true, false);
		this.addExpr(2, "COMMENT", "a comment", true, false);
		try
		{
			this.addEnum(3, "CLOCK:SRC", "master clock in", MODE.ASIS, head.getNode("CLOCK:SRC:ZCLK").decompile());
		}
		catch (final MdsException e)
		{
			this.addExpr(3, "CLOCK:SRC", "master clock in", false, false);
		}
		this.addExpr(4, "TRIGGER", "time offset of first trigger", false, false);
		this.addEnum(5, "TRIGGER:EDGE", "triggger edge", EnumEditor.MODE.STRING, "rising", "falling");
		this.addExpr(6, "TRIGGER:POST", "number of samples post trigger", false, false);
		this.addExpr(7, "TRIGGER:PRE", "number of samples pre trigger", false, false);
		main.add(this.pane, BorderLayout.NORTH);
		this.pane = new JTabbedPane();
		this.pane.add(main, "Master");
		this.modules = new ACQ4xx[num_modules];
		for (int i = 0; i < num_modules; i++)
			this.addACQ4xx(i);
		this.master = this.modules[0].getMaster();
		if (this.master != null)
			main.add(Editor.addLabel("MASTER", this.master.getPane()), BorderLayout.SOUTH);
	}

	protected final void addACQ4xx(final int index)
	{
		final String path = String.format("MODULE%d", Integer.valueOf(index + 1));
		try
		{
			final Constructor<?> modconst = this.module_class.getConstructor(Frame.class, NODE.class, boolean.class,
					int.class);
			this.modules[index] = (ACQ4xx) modconst.newInstance(this.frame,
					this.head == null ? null : this.head.getNode(path), Boolean.valueOf(this.editable),
					Integer.valueOf(index));
			this.pane.add(this.modules[index].getPane(), path);
		}
		catch (final Exception e)
		{
			System.err.println(path);
			e.printStackTrace();
		}
	}
}

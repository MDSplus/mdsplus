package mds.devices.acq4xx;

import java.awt.*;
import java.lang.reflect.Constructor;

import javax.swing.*;

import mds.Mds;
import mds.MdsException;
import mds.data.TREE;
import mds.data.descriptor_s.NODE;
import mds.data.descriptor_s.Nid;
import mds.devices.Device;
import mds.jtraverser.editor.Editor;
import mds.jtraverser.editor.EnumEditor;
import mds.jtraverser.editor.EnumEditor.MODE;
import mds.mdsip.MdsIp;

public class ACQ1001 extends Device
{
	public static void main(final String... args) throws MdsException
	{
		try (final Mds mds = new MdsIp())
		{
			try (final TREE tree = new TREE(mds, "test", 1, TREE.NEW))
			{
				final Nid dev = tree.getTop().addConglom("DEVICE", "ACQ2106_ACQ480x4");
				new ACQ1001(null, dev, true, ACQ480.class).showDialog();
			}
		}
	}

	final ACQ4xx module;
	final ACQ4xx.Master master;
	final Class<?> module_class;

	public ACQ1001(final Frame frame, final NODE<?> head, final boolean editable, final Class<?> module_class)
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
		final String path = "MODULE1";
		ACQ4xx mod;
		try
		{
			final Constructor<?> modconst = this.module_class.getConstructor(Frame.class, NODE.class, boolean.class,
					int.class);
			mod = (ACQ4xx) modconst.newInstance(this.frame, this.head == null ? null : this.head.getNode(path),
					Boolean.valueOf(this.editable), Integer.valueOf(0));
		}
		catch (final Exception e)
		{
			mod = null;
			JOptionPane.showMessageDialog(null, e.getMessage(), String.format("MODULE1 (%s)", module_class.getName()),
					JOptionPane.ERROR_MESSAGE);
		}
		this.module = mod;
		if (mod == null)
			this.master = null;
		else
		{
			this.pane.add(this.module.getPane(), path);
			this.master = this.module.getMaster();
			if (this.master != null)
				main.add(Editor.addLabel("MASTER", this.master.getPane()), BorderLayout.SOUTH);
		}
	}
}

package mds.jtraverser;

//package jTraverser;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class DisplayNci extends NodeEditor implements ActionListener
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	JLabel label;

	public DisplayNci()
	{
		setLayout(new BorderLayout());
		JPanel jp;
		add(jp = new JPanel(), "North");
		jp.setLayout(new GridBagLayout());
		final GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.EAST;
		c.gridx = 0;
		c.gridy = 0;
		jp.add(label = new JLabel(""), c);
		add(jp = new JPanel(), "South");
		c.gridy = 1;
		JButton close;
		jp.add(close = new JButton("Close"), c);
		close.addActionListener(this);
	}

	@Override
	public void actionPerformed(ActionEvent e)
	{
		frame.dispose();
	}

	@Override
	public void setNode(Node node)
	{
		this.node = node;
		frame.setTitle("Display Nci information");
		try
		{
			node.getInfo();
		}
		catch (final Exception exc)
		{
			jTraverser.stderr("Error retieving Nci", exc);
		}
		final StringBuffer sb = new StringBuffer(
				"<html><table width=\"320\"> <tr><td width=\"60\" align=\"left\"/><nobr>full path:</nobr></td><td align=\"left\">");
		sb.append(node.getFullPath());
		sb.append("</td></tr><tr><td align=\"left\" valign=\"top\">Status:</td><td align=\"left\"><nobr>");
		final String sep = "</nobr>, <nobr>";
		if (node.isOn())
			sb.append("on");
		else
			sb.append("off");
		sb.append(sep + "parent is ");
		if (node.isParentState())
			sb.append("off");
		else
			sb.append("on");
		if (node.isSetup())
			sb.append(sep + "setup");
		if (node.isEssential())
			sb.append(sep + "essential");
		if (node.isCached())
			sb.append(sep + "cached");
		if (node.isVersion())
			sb.append(sep + "version");
		if (node.isSegmented())
			sb.append(sep + "segmented");
		if (node.isWriteOnce())
			sb.append(sep + "write once");
		if (node.isCompressible())
			sb.append(sep + "compressible");
		if (node.isDoNotCompress())
			sb.append(sep + "do not compress");
		if (node.isCompressOnPut())
			sb.append(sep + "compress on put");
		if (node.isNoWriteModel())
			sb.append(sep + "no write model");
		if (node.isNoWriteShot())
			sb.append(sep + "no write shot");
		if (node.isPathReference())
			sb.append(sep + "path reference");
		if (node.isNidReference())
			sb.append(sep + "nid reference");
		if (node.isCompressSegments())
			sb.append(sep + "compress segments");
		if (node.isIncludeInPulse())
			sb.append(sep + "include in pulse");
		sb.append("</nobr></td></tr><tr><td align=\"left\">Segments:</td><td align=\"left\">" + node.getNumSegments());
		if (node.getNumSegments() == 0)
		{
			sb.append("</td></tr><tr><td align=\"left\">Data:</td><td align=\"left\">");
			String dtype, dclass;
			if (node.getLength() == 0)
				sb.append("There is no data stored for this node");
			else
			{
				dtype = node.getDType();
				dclass = node.getDClass();
				sb.append("<nobr>" + dtype + "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + dclass
						+ "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + node.getLength() + " Bytes</nobr>");
				sb.append("</td></tr><tr><td align=\"left\">Inserted:</td><td align=\"left\">");
				sb.append(node.getDate());
			}
		}
		sb.append("</td></tr></table></html>");
		label.setText(sb.toString());
	}
}

package mds.devices.acq4xx;

import java.awt.Frame;
import java.awt.GridLayout;
import javax.swing.JScrollPane;
import javax.swing.ScrollPaneConstants;

import mds.MdsException;
import mds.data.descriptor_s.NODE;
import mds.jtraverser.editor.Editor;
import mds.jtraverser.editor.EnumEditor;

public class ACQ480 extends ACQ4xx
{
	public class Channel extends ACQ4xx.Channel
	{
		protected Channel(final NODE<?> node)
		{
			super(node, 5);
			this.addEnum(0, "GAIN", "gain in db, [0..12]", EnumEditor.MODE.INDEX, "0db", "1db", "2db", "3db", "4db",
					"5db", "6db", "7db", "8db", "9db", "10db", "11db", "12db");
			this.addEnum(1, "HPF", "high pass filter mode; 0: off, else K2-K10", EnumEditor.MODE.INDEX, "off", "K2",
					"K3", "K4", "K5", "K6", "K7", "K8", "K9", "K10");
			this.addEnum(2, "INVERT", "invert channel input; 0:no, 1:yes", EnumEditor.MODE.INDEX, "no", "yes");
			this.addEnum(3, "LFNS", "low frequency noise suppression; 0:off, 1:on", EnumEditor.MODE.INDEX, "off", "on");
			this.addEnum(4, "T50R", "50 Ohm termination; 0:off, 1:on", EnumEditor.MODE.INDEX, "off", "on");
		}
	}

	public class Master extends ACQ4xx.Master
	{
		protected Master(final NODE<?> node)
		{
			super(node, 3);
			this.addEnum(0, "FIR", "adc fir filter mode", EnumEditor.MODE.INDEX, "DISABLE", "LP_ODD_D2", "HP_ODD_D2",
					"LP_EVEN_D4", "BP1_EVEN_D4", "BP1_EVEN_D4", "HP_ODD_D4", "CUSTOM_D2", "CUSTOM_D4", "CUSTOM_D8",
					"CUSTOM_D1");
			this.addEnum(1, "TRIG_MODE", "'RGM', 'RTM', or else 'TRANSIENT'", EnumEditor.MODE.STRING, "TRANSIENT",
					"RTM", "RGM");
			this.addExpr(2, "TRIG_MODE:TRANSLEN", "transient length for RTM", false, false);
		}
	}

	final private Channel[] channel;
	final private int index;

	public ACQ480(final Frame frame, final NODE<?> node, final boolean editable, final int index)
	{
		super(frame, node, editable, index == 0 ? 3 : 0);
		this.pane.setLayout(new GridLayout(0, 1));
		this.index = index;
		this.channel = new Channel[8];
		for (int i = 0; i < this.channel.length; i++)
			this.addChannel(i);
		final JScrollPane sp = new JScrollPane(this.pane);
		sp.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
		final int lineheight = this.pane.getComponent(0).getPreferredSize().height;
		sp.getVerticalScrollBar().setUnitIncrement(lineheight);
		this.pane = sp;
	}

	private void addChannel(final int i)
	{
		final String path = String.format("CHANNEL%02d", Integer.valueOf(this.index * this.channel.length + i + 1));
		try
		{
			final NODE<?> chan = this.head == null ? null : this.head.getNode(path);
			this.channel[i] = new Channel(chan);
			this.pane.add(Editor.addLabel(path, this.channel[i].getPane()));
		}
		catch (final MdsException e)
		{
			System.err.println(path);
			e.printStackTrace();
		}
	}

	@Override
	public Master getMaster()
	{ return new Master(this.head); }
}

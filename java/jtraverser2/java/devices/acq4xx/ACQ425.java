package devices.acq4xx;

import java.awt.Dimension;
import java.awt.Frame;
import java.awt.GridLayout;
import javax.swing.JScrollPane;
import javax.swing.ScrollPaneConstants;
import jtraverser.editor.Editor;
import jtraverser.editor.EnumEditor;
import mds.MdsException;
import mds.data.descriptor_s.NODE;

class ACQ425 extends ACQ4xx{
    public class Channel extends ACQ4xx.Channel{
	protected Channel(final NODE<?> node){
	    super(node, 3);
	    this.addEnum(0, "GAIN", "input range in volt", EnumEditor.MODE.ASIS, "10", "5", "2", "1");
	    this.addExpr(1, "OFFSET", "input offset in volt", false, false);
	    this.addEnum(2, "RANGE", "range expression used for data reduction DATA[RANGE]", EnumEditor.MODE.ASIS, "*:*:1");
	}
    }
    final private Channel[] channel;
    final private int       index;

    protected ACQ425(final Frame frame, final NODE<?> node, final boolean editable, final int index, final int numchan, final int maxfreq){
	super(frame, node, editable, 0);
	this.pane.setLayout(new GridLayout(0, 1));
	this.index = index;
	this.channel = new Channel[numchan];
	for(int i = 0; i < this.channel.length; i++)
	    this.addChannel(i);
	final JScrollPane sp = new JScrollPane(this.pane);
	sp.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
	final int lineheight = this.pane.getComponent(0).getPreferredSize().height;
	final int maxheight = lineheight * 8 + 3;
	sp.getVerticalScrollBar().setUnitIncrement(lineheight);
	sp.setMaximumSize(new Dimension(-1, maxheight));
	final Dimension prefsize = sp.getPreferredSize();
	if(prefsize.height > maxheight){
	    prefsize.height = maxheight;
	    sp.setPreferredSize(prefsize);
	}
	this.pane = sp;
    }

    private void addChannel(final int i) {
	final String path = String.format("CHANNEL%02d", Integer.valueOf(this.index * this.channel.length + i + 1));
	try{
	    final NODE<?> chan = this.head == null ? null : this.head.getNode(path);
	    this.channel[i] = new Channel(chan);
	    this.pane.add(Editor.addLabel(path, this.channel[i].getPane()));
	}catch(final MdsException e){
	    System.err.println(path);
	    e.printStackTrace();
	}
    }
}

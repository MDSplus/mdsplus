package jtraverser.editor;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.Window;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import jtraverser.Node;
import jtraverser.editor.usage.NumericEditor;
import mds.MdsException;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_s.NUMBER;

@SuppressWarnings("serial")
public class SegmentEditor extends Editor{
    public static final boolean checkData(final Node node) {
        if(node != null){
            if(node.getNumSegments() > 0) return true;
        }
        return false;
    }
    private final Node node;
    private int        segment = 0;

    public SegmentEditor(final Node node, final boolean editable, final Window window){
        super(null, editable, node.nid.getTree(), 5);
        this.node = node;
        try{
            node.readRecordInfo();
        }catch(final MdsException e){
            MdsException.stderr("SegmentEditor.RecordInfo", e);
            e.printStackTrace();
        }
        this.setLayout(new BorderLayout());
        final JPanel columns = new JPanel(new GridLayout(1, 2));
        columns.add(this.edit[0] = new NumericEditor(this.editable, this.ctx, window, "Data"));
        final JPanel dimension = new JPanel(new BorderLayout());
        dimension.add(this.edit[1] = new NumericEditor(this.editable, this.ctx, window, "Dimension"));
        columns.add(dimension);
        final JPanel limits = new JPanel(new GridLayout(2, 1));
        limits.add(this.edit[3] = Editor.addLabel("start", new ExprEditor(this.editable, this.ctx, false, false)), 0);
        limits.add(this.edit[4] = Editor.addLabel("end", new ExprEditor(this.editable, this.ctx, false, false)), 1);
        if(this.editable){
            final JPanel limitsedit = new JPanel(new BorderLayout());
            final JButton setlimits = new JButton("Update Limits");
            setlimits.addActionListener(new ActionListener(){
                @Override
                public void actionPerformed(final ActionEvent arg0) {
                    try{
                        SegmentEditor.this.node.nid.updateSegment(SegmentEditor.this.edit[3].getData(), SegmentEditor.this.edit[4].getData(), null, SegmentEditor.this.segment);
                    }catch(final MdsException e){
                        MdsException.stderr("SignalEditor", e);
                    }
                }
            });
            limitsedit.add(limits);
            limitsedit.add(setlimits, BorderLayout.SOUTH);
            dimension.add(limitsedit, BorderLayout.SOUTH);
        }else dimension.add(limits, BorderLayout.SOUTH);
        this.add(columns);
        final JPanel segpane = new JPanel(new BorderLayout());
        final JSlider segments = new JSlider(0, this.node.getNumSegments() - 1, this.segment);
        final JPanel scale = new JPanel(new BorderLayout());
        scale.add(this.edit[2] = new ExprEditor(this.editable, this.ctx, false, false));
        if(this.editable){
            final JButton setscale = new JButton("Set Scale");
            setscale.addActionListener(new ActionListener(){
                @Override
                public void actionPerformed(final ActionEvent arg0) {
                    try{
                        SegmentEditor.this.node.nid.setSegmentScale(SegmentEditor.this.edit[2].getData());
                    }catch(final MdsException e){
                        MdsException.stderr("SignalEditor", e);
                    }
                }
            });
            scale.add(setscale, BorderLayout.EAST);
        }
        segpane.add(Editor.addLabel("Segment Scale", scale), BorderLayout.NORTH);
        segpane.add(segments, BorderLayout.CENTER);
        final JTextField text = new JTextField(9);
        text.setHorizontalAlignment(SwingConstants.CENTER);
        text.setText(Integer.toString(SegmentEditor.this.segment));
        text.setEditable(false);
        segpane.add(text, BorderLayout.WEST);
        final JLabel label = new JLabel(new StringBuilder(21).append("Segments: ").append(this.node.getNumSegments() & 0xFFFFFFFFl).toString(), SwingConstants.CENTER);
        label.setBorder(BorderFactory.createEmptyBorder(0, 5, 0, 5));
        segpane.add(label, BorderLayout.EAST);
        segments.addMouseListener(new MouseAdapter(){
            @Override
            public void mouseReleased(final MouseEvent ce) {
                final int newsegment = ((JSlider)ce.getSource()).getValue();
                if(SegmentEditor.this.segment == newsegment) return;
                SegmentEditor.this.setSegment(newsegment);
            }
        });
        segments.addChangeListener(new ChangeListener(){
            @Override
            public void stateChanged(final ChangeEvent ce) {
                text.setText(Integer.toString(((JSlider)ce.getSource()).getValue()));
            }
        });
        this.add(segpane, BorderLayout.NORTH);
        this.setSegment(0);
    }

    @Override
    public final Descriptor<?> getData() throws MdsException {
        MdsException.handleStatus(MdsException.TreeINVRECTYP);
        return null;
    }

    public final Descriptor_A<?> getSegmentData() throws MdsException {
        return this.edit[0].getData().getDataA();
    }

    public final Descriptor<?> getSegmentDim() throws MdsException {
        return this.edit[1].getData();
    }

    public final NUMBER<?> getSegmentEnd() throws MdsException {
        return this.edit[4].getNumber();
    }

    public final int getSegmentIdx() {
        return this.segment;
    }

    public final Descriptor<?> getSegmentScale() throws MdsException {
        return this.edit[2].getData();
    }

    public final NUMBER<?> getSegmentStart() throws MdsException {
        return this.edit[3].getNumber();
    }

    @Override
    public final void reset(final boolean hard) {
        this.setSegment(this.segment);
    }

    @Override
    public final void setData(final Descriptor<?> data) {/*NOP*/}

    public final void setSegment(final int segment) {
        final List list;
        try{
            this.data = list = this.node.nid.getSegment(segment);
            this.segment = segment;
            this.edit[0].setData(list.get(0));
            this.edit[1].setData(list.get(1));
            this.edit[2].setData(this.node.nid.getSegmentScale());
            final List limits = this.node.nid.getSegmentLimits(segment);
            this.edit[3].setData(limits.get(0));
            this.edit[4].setData(limits.get(1));
        }catch(final MdsException e){
            MdsException.stderr("SignalEditor", e);
            this.data = null;
            this.segment = -1;
        }
    }
}
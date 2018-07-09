package jtraverser.dialogs;

import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.WindowConstants;
import jtraverser.Node;
import jtraverser.editor.Editor;
import jtraverser.editor.SegmentEditor;
import jtraverser.editor.usage.ActionEditor;
import jtraverser.editor.usage.AnyEditor;
import jtraverser.editor.usage.AxisEditor;
import jtraverser.editor.usage.DispatchEditor;
import jtraverser.editor.usage.NumericEditor;
import jtraverser.editor.usage.SignalEditor;
import jtraverser.editor.usage.TaskEditor;
import jtraverser.editor.usage.TextEditor;
import jtraverser.editor.usage.WindowEditor;
import mds.MdsException;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.NODE;

@SuppressWarnings("serial")
public class DataDialog extends JDialog{
    public static final DataDialog open(final Node node, final boolean editable) {
        final DataDialog dialog = new DataDialog(node, editable);
        Dialogs.setLocation(dialog);
        dialog.setVisible(true);
        return dialog;
    }

    private static final String tagList(final String[] tags) {
        if(tags == null || tags.length == 0) return "<no tags>";
        return String.join(", ", tags);
    }
    private final Node    node;
    private final Editor  edit;
    private final boolean editable;
    private final JButton ok_b, apply_b, reset_b, cancel_b;
    private final JLabel  onoff;
    private final JLabel  tags;

    private DataDialog(final Node node, final boolean editable){
        super(JOptionPane.getRootFrame());
        this.node = node;
        this.editable = editable;
        this.setLayout(new BorderLayout());
        final JPanel ip = new JPanel();
        ip.add(this.onoff = new JLabel(""));
        ip.add(new JLabel("Tags: "));
        ip.add(this.tags = new JLabel(""));
        this.add(ip, BorderLayout.PAGE_START);
        if(node.isSegmented()) this.edit = new SegmentEditor(node, this.editable, this);
        else{
            Descriptor<?> data = null;
            try{
                data = this.node.getData();
            }catch(final MdsException e){
                MdsException.stderr("DataDialog.getData", e);
                e.printStackTrace();
            }
            final CTX ctx = node.nid.getTree();
            switch(node.getUsage()){
                case NODE.USAGE_SIGNAL:
                    this.edit = new SignalEditor(data, this.editable, ctx, this);
                    break;
                case NODE.USAGE_ACTION:
                    this.edit = new ActionEditor(data, this.editable, ctx, this, node.getStatus(), node.treeview.isModel());
                    break;
                case NODE.USAGE_DISPATCH:
                    this.edit = new DispatchEditor(data, this.editable, ctx, this);
                    break;
                case NODE.USAGE_TASK:
                    this.edit = new TaskEditor(data, this.editable, ctx, this);
                    break;
                case NODE.USAGE_WINDOW:
                    this.edit = new WindowEditor(data, this.editable, ctx, this);
                    break;
                case NODE.USAGE_NUMERIC:
                    this.edit = new NumericEditor(data, this.editable, ctx, this);
                    break;
                case NODE.USAGE_AXIS:
                    this.edit = new AxisEditor(data, this.editable, ctx, this);
                    break;
                case NODE.USAGE_TEXT:
                    this.edit = new TextEditor(data, this.editable, ctx, this);
                    break;
                case NODE.USAGE_COMPOUND_DATA:
                case NODE.USAGE_DEVICE:
                default:
                    this.edit = new AnyEditor(data, this.editable, ctx, this);
            }
        }
        this.add(this.edit, BorderLayout.CENTER);
        final JPanel jp = new JPanel();
        this.add(jp, BorderLayout.PAGE_END);
        if(this.editable){
            if(this.edit instanceof SegmentEditor){
                jp.add(this.ok_b = new JButton("Update Data"));
                this.ok_b.addActionListener(new ActionListener(){
                    @Override
                    public void actionPerformed(final ActionEvent e) {
                        DataDialog.this.updateData();
                    }
                });
                jp.add(this.apply_b = new JButton("Update Dim"));
                this.apply_b.addActionListener(new ActionListener(){
                    @Override
                    public void actionPerformed(final ActionEvent e) {
                        DataDialog.this.updateDim();
                    }
                });
            }else{
                jp.add(this.ok_b = new JButton("Ok"));
                this.ok_b.addActionListener(new ActionListener(){
                    @Override
                    public void actionPerformed(final ActionEvent e) {
                        DataDialog.this.ok();
                    }
                });
                jp.add(this.apply_b = new JButton("Apply"));
                this.apply_b.addActionListener(new ActionListener(){
                    @Override
                    public void actionPerformed(final ActionEvent e) {
                        DataDialog.this.apply();
                    }
                });
            }
            jp.add(this.reset_b = new JButton("Reset"));
            this.reset_b.addActionListener(new ActionListener(){
                @Override
                public void actionPerformed(final ActionEvent e) {
                    DataDialog.this.reset();
                }
            });
            this.addKeyListener(new KeyAdapter(){
                @Override
                public void keyTyped(final KeyEvent e) {
                    if(e.getKeyCode() == KeyEvent.VK_ENTER) DataDialog.this.ok();
                }
            });
        }else this.ok_b = this.apply_b = this.reset_b = null;
        this.cancel_b = new JButton("Cancel");
        this.cancel_b.addActionListener(new ActionListener(){
            @Override
            public void actionPerformed(final ActionEvent e) {
                DataDialog.this.cancel();
            }
        });
        this.cancel_b.setSelected(true);
        jp.add(this.cancel_b);
        if(node.isOn()) this.onoff.setText("Node is On   ");
        else this.onoff.setText("Node is Off  ");
        if(this.editable) this.setTitle("Modify data of " + node.getFullPath());
        else this.setTitle("Display data of " + node.getFullPath());
        this.tags.setText(DataDialog.tagList(node.getTags()));
        this.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
        this.pack();
    }

    private final boolean apply() {
        if(this.edit instanceof SegmentEditor) return false;
        try{
            final Descriptor<?> data = this.edit.getData();
            if(data == null && !this.edit.isNull()) throw new Exception("Could not compile expression.");
            this.node.setData(data);
        }catch(final Exception e){
            JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), e.getMessage(), "Error writing datafile", JOptionPane.ERROR_MESSAGE);
            return false;
        }
        return true;
    }

    private final void cancel() {
        this.dispose();
    }

    private final void ok() {
        if(this.edit instanceof SegmentEditor) this.dispose();
        if(this.apply()) this.dispose();
    }

    private final void reset() {
        this.edit.reset(true);
        this.validate();
        this.repaint();
    }

    private final void updateData() {
        final SegmentEditor segedit = (SegmentEditor)this.edit;
        try{
            this.node.nid.putSegment(segedit.getSegmentIdx(), segedit.getSegmentData());
        }catch(final Exception e){
            JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), e.getMessage(), "Error updating segment", JOptionPane.ERROR_MESSAGE);
        }
    }

    private final void updateDim() {
        final SegmentEditor segedit = (SegmentEditor)this.edit;
        try{
            final Descriptor<?> dim = segedit.getSegmentDim();
            if(dim == null) throw new Exception("Could not compile dimension expression.");
            this.node.nid.updateSegment(segedit.getSegmentStart(), segedit.getSegmentEnd(), dim, segedit.getSegmentIdx());
        }catch(final Exception e){
            JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), e.getMessage(), "Error updating segment", JOptionPane.ERROR_MESSAGE);
        }
    }
}
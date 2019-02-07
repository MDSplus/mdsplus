package devices;

import java.awt.Dimension;
import java.awt.Frame;
import java.awt.GridLayout;
import java.lang.reflect.Constructor;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ScrollPaneConstants;
import jtraverser.editor.Editor;
import jtraverser.editor.EnumEditor;
import jtraverser.editor.ExprEditor;
import mds.MdsException;
import mds.data.TREE;
import mds.data.TREE.NodeInfo;
import mds.data.descriptor_r.Conglom;
import mds.data.descriptor_s.NODE;
import mds.data.descriptor_s.NODE.Flags;
import mds.data.descriptor_s.Nid;
import mds.data.descriptor_s.StringDsc;

public class Device{
    public static Device getEditor(final Frame frame, final Nid nid, final boolean editable) throws Exception {
        final Conglom conglom = (Conglom)nid.getRecord();
        if(conglom == null) throw new Exception("Record not a Conglom");
        final StringDsc model = (StringDsc)conglom.getModel().getDataS();
        if(model == null) throw new Exception("No model string");
        Class<?> device_cls;
        try{
            device_cls = Class.forName(new StringBuilder().append("devices.").append(model.toString()).toString());
        }catch(final ClassNotFoundException e){
            device_cls = Device.class;
        }
        final Constructor<?> constr = device_cls.getConstructor(Frame.class, NODE.class, boolean.class);
        return (Device)constr.newInstance(frame, nid, Boolean.valueOf(editable));
    }
    protected final NODE<?>   head;
    protected final Editor[]  edit;
    protected final NODE<?>[] node;
    protected final boolean   editable;
    protected JComponent      pane;
    protected final Frame     frame;

    public Device(final Frame frame, final NODE<?> head, final boolean editable){
        this.frame = frame;
        this.pane = new JPanel();
        this.head = head;
        this.editable = editable;
        final TREE tree = this.head.getTree();
        NodeInfo[] node_infos;
        NODE<?>[] nodes;
        try{
            final Nid olddefault = tree.getDefaultC();
            this.head.setDefault();
            try{
                node_infos = TREE.NodeInfo.getDeviceNodeInfos(head, tree.getMds(), tree);
            }finally{
                olddefault.setDefault();
            }
            nodes = new NODE[node_infos.length];
        }catch(final MdsException e){
            System.err.println(e);
            nodes = new NODE<?>[]{head};
            node_infos = null;
        }
        this.node = nodes;
        this.edit = new Editor[nodes.length];
        this.pane.setLayout(new GridLayout(0, 1));
        if(node_infos == null) return;
        for(int i = 1; i < this.node.length; i++){
            final byte usage = node_infos[i].usage;
            final int flags = node_infos[i].get_flags;
            if(usage == NODE.USAGE_STRUCTURE) continue;
            if((flags & (Flags.WRITE_ONCE | Flags.NO_WRITE_MODEL)) > 0) continue;
            if((flags & (Flags.NO_WRITE_SHOT | Flags.SETUP)) == 0) continue;
            String orig_name;
            final String name = node_infos[i].minpath;
            this.node[i] = new Nid(node_infos[i].nid_number, tree);
            this.node[i].setNodeInfoC(node_infos[i]);
            try{
                orig_name = this.node[i].getNciOriginalPartName();
            }catch(final MdsException e){
                orig_name = name;
            }
            this.addExpr(i, name, orig_name, false, false);
        }
        final JScrollPane sp = new JScrollPane(this.pane);
        sp.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
        final int lineheight = this.pane.getComponent(0).getPreferredSize().height;
        final int maxheight = lineheight * 7 + 3;
        sp.getVerticalScrollBar().setUnitIncrement(lineheight);
        sp.setMaximumSize(new Dimension(-1, maxheight));
        final Dimension prefsize = sp.getPreferredSize();
        if(prefsize.height > maxheight) prefsize.height = maxheight;
        prefsize.width *= 3;
        sp.setPreferredSize(prefsize);
        this.pane = sp;
    }

    protected Device(final Frame frame, final NODE<?> head, final boolean editable, final int nargs){
        this.frame = frame;
        this.pane = new JPanel();
        this.head = head;
        this.editable = editable;
        this.edit = new Editor[nargs];
        this.node = new NODE[nargs];
    }

    protected final void addEnum(final int idx, final String path, final String tooltip, final EnumEditor.MODE mode, final String... items) {
        try{
            if(this.node[idx] == null) this.node[idx] = this.head.getNode(path);
            final NodeInfo ni = this.node[idx].getNodeInfoC();
            this.edit[idx] = Editor.addLabel(String.format("%s   [%s]", path, NODE.getUsageStr(ni.usage)), new EnumEditor(this.node[idx].getRecord(), this.editable, this.head.getTree(), tooltip, mode, items));
            this.pane.add(Editor.addButtons(this.edit[idx], this.node[idx]));
        }catch(final MdsException e){
            JOptionPane.showMessageDialog(this.frame, e + "\n" + e.getMessage(), "addEnum " + path, JOptionPane.WARNING_MESSAGE);
        }
    }

    protected final void addExpr(final int idx, final String path, final String tooltip, final boolean default_to_string, final boolean isField) {
        try{
            if(this.node[idx] == null) this.node[idx] = this.head.getNode(path);
            final NodeInfo ni = this.node[idx].getNodeInfoC();
            this.edit[idx] = Editor.addLabel(String.format("%s   [%s]", path, NODE.getUsageStr(ni.usage)), new ExprEditor(this.node[idx].getRecord(), this.editable, this.head.getTree(), tooltip, default_to_string, isField));
            this.pane.add(Editor.addButtons(this.edit[idx], this.node[idx]));
        }catch(final MdsException e){
            JOptionPane.showMessageDialog(this.frame, e + "\n" + e.getMessage(), "addExpr " + path, JOptionPane.WARNING_MESSAGE);
        }
    }

    public JComponent getPane() {
        return this.pane;
    }

    public final JDialog showDialog() {
        String path;
        try{
            path = this.head.getNciPath();
        }catch(final MdsException e){
            path = this.head.toString();
        }
        final String title = new StringBuilder().append(this.getClass().getSimpleName()).append(": ").append(path).toString();
        final JDialog dialog = new JDialog(this.frame, title);
        dialog.add(this.pane);
        dialog.pack();
        dialog.setVisible(true);
        return dialog;
    }
}

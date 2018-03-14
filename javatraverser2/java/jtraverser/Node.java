package jtraverser;

import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.StringSelection;
import java.lang.reflect.Method;
import javax.swing.BorderFactory;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.SwingConstants;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import mds.MdsException;
import mds.data.DTYPE;
import mds.data.TREE;
import mds.data.TREE.NodeInfo;
import mds.data.TREE.RecordInfo;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_r.Action;
import mds.data.descriptor_r.Conglom;
import mds.data.descriptor_r.Signal;
import mds.data.descriptor_s.CString;
import mds.data.descriptor_s.NODE;
import mds.data.descriptor_s.NODE.Flags;
import mds.data.descriptor_s.Nid;

public class Node{
    @SuppressWarnings("serial")
    public static class TreeNodeLabel extends JLabel{
        static Font plain_f, bold_f;
        static{
            TreeNodeLabel.plain_f = new Font("Serif", Font.PLAIN, 12);
            TreeNodeLabel.bold_f = new Font("Serif", Font.BOLD, 12);
        }
        final Color CExclude  = new Color(128, 128, 128);
        final Color CInclude  = new Color(0, 0, 0);
        final Color CMSetup   = new Color(0, 0, 128);
        final Color CMSetupO  = new Color(96, 0, 128);
        final Color CNorm     = new Color(0, 0, 0);
        final Color CNormO    = new Color(96, 0, 96);
        final Color CNoWrite  = new Color(128, 0, 0);
        final Color CNoWriteO = new Color(192, 0, 0);
        final Color CSSetup   = new Color(128, 0, 128);
        final Color CSSetupO  = new Color(128, 0, 64);
        final Color CWrite    = new Color(0, 128, 0);
        final Color CWriteO   = new Color(96, 64, 0);
        final Node  node;

        public TreeNodeLabel(final Node node, final String name, final Icon icon, final boolean isSelected){
            super((node.isDefault() ? new StringBuilder(name.length() + 2).append('(').append(name).append(')').toString() : name), icon, SwingConstants.LEFT);
            this.node = node;
            final Flags flags = node.getFlags();
            final int lusage = node.getUsage();
            if(lusage == NODE.USAGE_SUBTREE) this.setForeground(flags.isIncludeInPulse() ? this.CInclude : this.CExclude);
            else{
                if(flags.isNoWriteModel() & flags.isNoWriteModel()) this.setForeground(this.CNoWrite);
                else if(flags.isNoWriteModel()) this.setForeground(node.treeview.isModel() ? (flags.isWriteOnce() ? this.CNoWriteO : this.CNoWrite) : (flags.isWriteOnce() ? this.CWriteO : this.CWrite));
                else if(flags.isNoWriteShot()) this.setForeground(!node.treeview.isModel() ? (flags.isWriteOnce() ? this.CNoWriteO : this.CNoWrite) : (flags.isWriteOnce() ? this.CWriteO : this.CWrite));
                else if(flags.isSetup()) this.setForeground(node.treeview.isModel() ? (flags.isWriteOnce() ? this.CMSetupO : this.CMSetup) : (flags.isWriteOnce() ? this.CSSetupO : this.CSSetup));
                else this.setForeground(flags.isWriteOnce() ? this.CNormO : this.CNorm);
            }
            this.setFont(flags.isOn() && flags.isParentOn() ? TreeNodeLabel.bold_f : TreeNodeLabel.plain_f);
            this.setBorder(BorderFactory.createLineBorder(isSelected ? Color.black : Color.white, 1));
        }

        @Override
        public final String getToolTipText() {
            return this.node.getToolTipText();
        }
    }
    private static Node              copied;
    private static boolean           cut;
    private static final ImageIcon[] ICONS = new ImageIcon[]{                 //
            Node.loadIcon("jtraverser/any.gif"),                              //
            Node.loadIcon("jtraverser/structure.gif"),                        //
            Node.loadIcon("jtraverser/action.gif"),                           //
            Node.loadIcon("jtraverser/device.gif"),                           //
            Node.loadIcon("jtraverser/dispatch.gif"),                         //
            Node.loadIcon("jtraverser/numeric.gif"),                          //
            Node.loadIcon("jtraverser/signal.gif"),                           //
            Node.loadIcon("jtraverser/task.gif"),                             //
            Node.loadIcon("jtraverser/text.gif"),                             //
            Node.loadIcon("jtraverser/window.gif"),                           //
            Node.loadIcon("jtraverser/axis.gif"),                             //
            Node.loadIcon("jtraverser/subtree.gif"),                          //
            Node.loadIcon("jtraverser/compound.gif")                          //
    };
    private static final String      tab   = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";

    public static void copySubtreeContent(final Node fromNode, final Node toNode) {
        try{
            fromNode.expand();
            toNode.expand();
        }catch(final Exception exc){
            MdsException.stderr("Error expanding nodes", exc);
        }
        try{
            final Descriptor<?> data = fromNode.getData();
            if(data != null){
                if(!(data instanceof Action)) toNode.setData(data);
            }
        }catch(final MdsException exc){/**/}
        for(int i = 0; i < fromNode.sons.length; i++)
            Node.copySubtreeContent(fromNode.sons[i], toNode.sons[i]);
        for(int i = 0; i < fromNode.members.length; i++)
            Node.copySubtreeContent(fromNode.members[i], toNode.members[i]);
    }

    public static final Node getNode(final DefaultMutableTreeNode treenode) {
        return (Node)treenode.getUserObject();
    }

    public static final Node getNode(final javax.swing.tree.TreeNode treenode) {
        return Node.getNode((DefaultMutableTreeNode)treenode);
    }

    public static final String getUniqueName(String name, final String[] usedNames) {
        int i;
        for(i = 0; i < usedNames.length; i++)
            if(name.equals(usedNames[i])) break;
        if(i == usedNames.length) return name;
        for(i = name.length() - 1; i > 0; i--)
            if((name.charAt(i) < '0' || name.charAt(i) > '9')) break;
        name = name.substring(0, i + 1);
        String prevName;
        if(name.length() < 10) prevName = name;
        else prevName = name.substring(0, 9);
        for(i = 1; i < 1000; i++){
            final String newName = prevName + i;
            int j;
            for(j = 0; j < usedNames.length; j++)
                if(newName.equals(usedNames[j])) break;
            if(j == usedNames.length) return newName;
        }
        return "XXXXXXX"; // Dummy name, hopefully will never reach this
    }

    public static final boolean isCopied() {
        return Node.copied != null;
    }

    private static final ImageIcon loadIcon(final String gifname) {
        final String base = System.getProperty("icon_base");
        if(base == null) return new ImageIcon(Node.class.getClassLoader().getResource(gifname));
        return new ImageIcon(base + "/" + gifname);
    }

    public static final void updateCell() {/*stub*/}
    private TreeNodeLabel          label;
    private Node[]                 members;
    public final Nid               nid;
    private Node                   parent;
    private Node[]                 sons;
    public final TreeView          treeview;
    private DefaultMutableTreeNode treenode;
    private boolean                needsOnCheck;
    DefaultMutableTreeNode         dummy = null;
    boolean                        is_leaf;
    private int                    tooltip_default;
    private long                   tooltip_life;
    private String                 tooltip_text;
    private int                    length, ownerid;
    private int                    minpath_default;
    private String                 minpath, path, fullpath, name, timeinserted;
    private byte                   usage;
    private Flags                  flags;
    private byte                   dtype, dclass;
    private String[]               tags;
    private int                    rlength;
    private int                    num_segments;
    private int                    status;

    public Node(final TreeView treeview, final Nid nid){
        this(treeview, nid, null, null);
    }

    public Node(final TreeView treeview, final Nid nid, final Node parent, final NodeInfo info){
        this.treeview = treeview;
        this.nid = nid;
        this.parent = parent;
        this.sons = new Node[0];
        this.members = new Node[0];
        this.update();
        if(info == null) try{
            this.readNodeInfo();
        }catch(final Exception exc){
            MdsException.stderr("Error reading node info", exc);
        }
        else this.copyNodeInfo(info);
    }

    public final Node addChild(final String new_name) throws MdsException {
        return this.addNode(new_name, NODE.USAGE_STRUCTURE);
    }

    public final Node addDevice(final String nodename, final String type) throws MdsException {
        if(nodename == null || nodename.length() == 0 || nodename.length() > 12){
            JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), "Name length must range between 1 and 12 characters", "Error adding Node", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        if(type == null || type.length() == 0){
            JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), "Missing device type", "Error adding Node", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        Nid new_nid;
        final Nid prev_default = this.nid.getTree().getDefaultNid();
        this.nid.setDefault();
        try{
            new_nid = this.nid.getTree().addConglom(nodename, type);
        }finally{
            prev_default.setDefault();
        }
        return this.addNode(new_nid, NODE.USAGE_DEVICE);
    }

    private final Node addNode(final Nid new_nid, final int new_usage) {
        final boolean ismember = new_usage != NODE.USAGE_STRUCTURE || new_usage != NODE.USAGE_SUBTREE;
        final Node newNode = new Node(this.treeview, new_nid, this, null);
        this.expand();
        if(ismember){
            final Node[] newNodes = new Node[this.members.length + 1];
            System.arraycopy(this.members, 0, newNodes, 0, this.members.length);
            newNodes[this.members.length] = newNode;
            this.members = newNodes;
        }else{
            final Node[] newNodes = new Node[this.sons.length + 1];
            System.arraycopy(this.sons, 0, newNodes, 0, this.sons.length);
            newNodes[this.sons.length] = newNode;
            this.sons = newNodes;
        }
        newNode.setTreeNode(new DefaultMutableTreeNode(newNode));
        this.treeview.addNodeToParent(newNode.getTreeNode(), this.getTreeNode());
        this.treeview.setCurrentNode(newNode);
        return newNode;
    }

    public final Node addNode(final String new_name, final byte new_usage) throws MdsException {
        Nid new_nid;
        final Nid prev_default = this.nid.getTree().getDefaultNid();
        this.nid.setDefault();
        try{
            new_nid = this.nid.getTree().addNode(new_name, new_usage);
        }finally{
            prev_default.setDefault();
        }
        return this.addNode(new_nid, new_usage);
    }

    private final boolean changePath(final Node newParent, final String newName) {
        if((newParent == this.parent) && (newName == this.getName())) return false; // nothing to do
        if(newName.length() > 12 || newName.length() == 0){
            JOptionPane.showMessageDialog(this.treeview, "Node name lengh must be between 1 and 12 characters", "Error renaming node: " + newName.length(), JOptionPane.WARNING_MESSAGE);
            return false;
        }
        try{
            final Nid prev_default = this.nid.getTree().getDefaultNid();
            this.parent.setDefault();
            try{
                this.nid.setPath(newName);
            }finally{
                prev_default.setDefault();
            }
        }catch(final Exception exc){
            JOptionPane.showMessageDialog(this.treeview, "Error changing node path: " + exc, "Error changing node path", JOptionPane.WARNING_MESSAGE);
            return false;
        }
        if(newParent != this.parent){
            this.parent = newParent;
            final DefaultTreeModel tree_model = (DefaultTreeModel)this.treeview.getModel();
            tree_model.removeNodeFromParent(this.getTreeNode());
            this.treeview.addNodeToParent(this.getTreeNode(), this.parent.getTreeNode());
        }
        this.name = this.minpath = this.path = this.fullpath = null;
        this.treeview.reportChange();
        return true;
    }

    public final void clearFlag(final byte idx) throws MdsException {
        this.clearFlags(1 << idx);
    }

    public final void clearFlags(final int clr_flags) throws MdsException {
        this.flags = null;
        this.nid.clearFlags(clr_flags);
    }

    public final void copy() {
        Node.cut = false;
        Node.copied = this;
        MdsException.stdout("copy: " + Node.copied + " from " + Node.copied.parent);
    }

    private final void copyNodeInfo(final NodeInfo info) {
        this.usage = info.usage;
        this.flags = new Flags(info.get_flags);
        this.status = info.status;
        this.is_leaf = info.num_descendants == 0;
        this.name = info.node_name;
        this.minpath = info.minpath;
        this.path = info.path;
        this.fullpath = info.fullpath;
    }

    public final void copyToClipboard() {
        try{
            final Clipboard cb = Toolkit.getDefaultToolkit().getSystemClipboard();
            StringSelection content;
            content = new StringSelection(this.getFullPath());
            cb.setContents(content, null);
        }catch(final Exception e){
            MdsException.stderr("Cannot copy fullPath to Clipboard", e);
        }
    }

    public final void cut() {
        Node.cut = true;
        Node.copied = this;
        MdsException.stdout("cut: " + Node.copied + " from " + Node.copied.parent);
    }

    public final void delete() {
        if(this.treeview.isEditable()) this.treeview.deleteNode(this);
        else MdsException.stdout("Cannot delete " + this + ". Tree not in edit mode.");
    }

    public final boolean deleteExecute() {
        try{
            this.nid.getTree().deleteNodeExecute();
            return true;
        }catch(final Exception exc){
            MdsException.stderr("Error executing delete", exc);
            return false;
        }
    }

    public final int deleteInitialize() {
        try{
            return this.nid.deleteInitialize();
        }catch(final Exception exc){
            MdsException.stderr("Error starting delete", exc);
        }
        return -1;
    }

    public final void doAction() {
        try{
            this.nid.doAction();
        }catch(final MdsException e){/**/}
    }

    public final void expand() {
        if(this.is_leaf) return;
        try{
            int i;
            Nid sons_nid[] = this.nid.getNciChildrenNids().toArray();
            if(sons_nid == null) sons_nid = new Nid[0];
            final NodeInfo[] sons_info = this.nid.getTree().getNodeInfos(sons_nid);
            this.sons = new Node[sons_nid.length];
            for(i = 0; i < sons_nid.length; i++)
                this.sons[i] = new Node(this.treeview, sons_nid[i], this, sons_info[i]);
            Nid members_nid[] = this.nid.getNciMemberNids().toArray();
            if(members_nid == null) members_nid = new Nid[0];
            final NodeInfo[] members_info = this.nid.getTree().getNodeInfos(members_nid);
            this.members = new Node[members_nid.length];
            for(i = 0; i < members_nid.length; i++)
                this.members[i] = new Node(this.treeview, members_nid[i], this, members_info[i]);
            if(this.members.length + this.sons.length == 0) this.is_leaf = true;
        }catch(final MdsException e){
            MdsException.stderr("expand", e);
            e.printStackTrace();
            this.members = new Node[0];
            this.sons = new Node[0];
        }
    }

    public final Descriptor<?> getData() throws MdsException {
        if(this.isSegmented()){
            final List seg = this.nid.getSegment(0);
            final Descriptor<?> scale = this.nid.getSegmentScale();
            if(Descriptor.isMissing(scale)) return new Signal(seg.get(0), null, seg.get(1));
            return new Signal(scale, seg.get(0), seg.get(1));
        }
        return this.nid.getRecord();
    }

    public final String getDate() {
        if(this.timeinserted == null) return this.readDate();
        return this.timeinserted;
    }

    public final byte getDClass() {
        if(this.dclass == -1) return this.readDClass();
        return this.dclass;
    }

    public final byte getDType() {
        if(this.dtype == -1) return this.readDType();
        return this.dtype;
    }

    public final Flags getFlags() {
        if(this.flags == null) return this.readFlags();
        return this.flags;
    }

    public final String getFullPath() {
        if(this.fullpath == null) return this.readFullPath();
        return this.fullpath;
    }

    public final Component getIcon(final boolean isSelected) {
        // if(!this.treeview.isUpdating() && this.label != null) return this.label;
        final int lusage = this.getUsage();
        final Icon icon = lusage <= Node.ICONS.length ? Node.ICONS[lusage] : null;
        this.label = new TreeNodeLabel(this, this.getName(), icon, isSelected);
        return this.label;
    }

    public final String getInfoTextBox() {
        try{
            this.readRecordInfo();
        }catch(final MdsException e){
            MdsException.stderr("Node.readRecordInfo", e);
        }
        final StringBuilder sb = new StringBuilder("<html><table width=\"240\"> <tr><td width=\"60\" align=\"left\"/><nobr>full path:</nobr></td><td align=\"left\">");
        sb.append(this.getFullPath());
        sb.append(" (").append(this.nid.getNidNumber()).append(")");
        sb.append("</td></tr><tr><td align=\"left\" valign=\"top\">Status:</td><td align=\"left\"><nobr>");
        final String sep = "</nobr>, <nobr>";
        final Flags lflags = this.getFlags();
        lflags.info(sb, sep);
        if(this.getUsage() == NODE.USAGE_STRUCTURE) sb.append("</nobr>");
        else if(this.getUsage() == NODE.USAGE_SUBTREE){
            sb.append("</nobr></td></tr><tr><td align=\"left\">File:</td><td align=\"left\">");
            final TREE tree = this.nid.getTree();
            String filename;
            try{
                filename = this.nid.getNidNumber() == 0 ? tree.getFileName() : tree.getFileName(this.name);
            }catch(final MdsException e){
                filename = String.format("Could not find tree file.", this.name, Integer.valueOf(this.nid.getTree().shot));
            }
            sb.append("<nobr>").append(filename).append("</nobr>");
        }else{
            sb.append("</nobr></td></tr><tr><td align=\"left\">Data:</td><td align=\"left\">");
            if(this.getLength() == 0) sb.append("<nobr>There is no data stored for this node</nobr>");
            else{
                final String ldtype = DTYPE.getName(this.getDType());
                final String ldclass = Descriptor.getDClassName(this.getDClass());
                sb.append("<nobr>").append(ldtype).append(Node.tab).append(ldclass).append(Node.tab).append(this.getLength()).append(" B (").append(this.getRLength()).append(" B)</nobr>");
                sb.append("</td></tr><tr><td align=\"left\">Inserted:</td><td align=\"left\">");
                sb.append(this.getDate());
            }
        }
        return sb.append("</td></tr></table></html>").toString();
    }

    public int getLength() {
        if(this.length == -1) return this.readLength();
        return this.length;
    }

    public final Node[] getMembers() {
        return this.members;
    }

    public final String getMinPath() {
        if(this.minpath == null || this.minpath_default != this.treeview.getDefault().getNidNumber()) return this.readMinPath();
        return this.minpath;
    }

    public String getName() {
        if(this.name == null) return this.readName();
        return this.name;
    }

    public int getNumSegments() {
        if(this.num_segments == -1) return this.readNumSegments();
        return this.num_segments;
    }

    public int getOwner() {
        if(this.ownerid == -1) return this.readOwner();
        return this.ownerid;
    }

    public final String getPath() {
        if(this.path == null) return this.readPath();
        return this.path;
    }

    public int getRLength() {
        if(this.rlength == -1) return this.readRLength();
        return this.rlength;
    }

    public final Descriptor<?> getSignal() throws MdsException {
        final NODE<?> res = this.nid.followReference();
        if(res.isSegmented()) return res.getSegment(0);
        return res.getRecord();
    }

    public final Node[] getSons() {
        return this.sons;
    }

    public int getStatus() {
        if(this.status == -1) return this.readStatus();
        return this.status;
    }

    public final String[] getTags() {
        if(this.tags == null) return this.readTags();
        return this.tags;
    }

    public final String getTagTextBox() {
        this.getTags();
        if(this.tags == null || this.tags.length == 0) return "No Tags";
        final StringBuilder text = new StringBuilder(256).append("<html>");
        for(final String tag : this.tags)
            text.append(tag).append("<br>");
        final int len = text.length();
        return text.replace(len - 4, len, "</ht").append("ml>").toString();
    }

    public final String getToolTipText() {
        final long now = System.nanoTime();
        int defaultnid;
        defaultnid = this.treeview.getDefault().getNidNumber();
        if(this.tooltip_text == null || now > this.tooltip_life || this.tooltip_default != defaultnid){
            String text = null;
            final String info = this.getInfoTextBox();
            final int lusage = this.getUsage();
            if(lusage == NODE.USAGE_STRUCTURE || lusage == NODE.USAGE_SUBTREE) this.tooltip_text = info;
            else{
                try{
                    final Descriptor<?> data = this.getData();
                    if(data == null) return info;
                    text = data.toStringX().replace("<", "&lt;").replace(">", "&gt;").replace("\t", "&nbsp&nbsp&nbsp&nbsp ").replace("\n", "<br>");
                }catch(final MdsException e){
                    text = e.getMessage();
                }
                final StringBuilder sb = new StringBuilder().append(info.substring(0, info.length() - 7)).append("<hr><table");
                if(text.length() > 80) sb.append(" width=\"320\"");
                this.tooltip_text = sb.append(">").append(text).append("</table></html>").toString();
            }
            this.tooltip_life = now + 10000000000l;
            this.tooltip_default = defaultnid;
        }
        return this.tooltip_text;
    }

    public final DefaultMutableTreeNode getTreeNode() {
        return this.treenode;
    }

    public final byte getUsage() {
        if(this.usage == -1) return this.readUsage();
        return this.usage;
    }

    public final boolean isDefault() {
        return this.treeview.getDefault().equals(this.nid);
    }

    public final boolean isOn() {
        if(this.needsOnCheck){
            this.needsOnCheck = false;
            this.readFlags();
        }
        return this.flags.isOn();
    }

    public final boolean isSegmented() {
        if(this.flags.isSegmented()) return true;
        return this.getNumSegments() > 0;
    }

    public final boolean isSubTree() {
        return this.getUsage() == NODE.USAGE_SUBTREE;
    }

    final boolean move(final Node newParent) {
        return this.changePath(newParent, this.getName());
    }

    public final void paste() {
        if(!this.treeview.equals(Node.copied.treeview)) MdsException.stderr("Copying between different tree is not yet supported.", null);
        else if(!this.treeview.isEditable()) MdsException.stderr("Cannot paste " + Node.copied + ". Tree not in edit mode.", null);
        else{
            MdsException.stdout((Node.cut ? "moved: " : "copied: ") + Node.copied + " from " + Node.copied.parent + " to " + this);
            if(Node.copied != null && Node.copied != this){
                if(Node.cut){
                    if(Node.copied.move(this)) Node.copied = null;
                }else this.treeview.pasteSubtree(Node.copied, this, true);
            }
        }
    }

    public final String readDate() {
        try{
            return this.timeinserted = this.nid.getNciTimeInsertedStr();
        }catch(final MdsException e){
            MdsException.stderr("Error updating timeinserted", e);
            return this.timeinserted;
        }
    }

    public final byte readDClass() {
        try{
            return this.dclass = this.nid.getNciClass();
        }catch(final MdsException e){
            MdsException.stderr("Error updating class", e);
            return this.dclass;
        }
    }

    public final byte readDType() {
        try{
            return this.dtype = this.nid.getNciDType();
        }catch(final MdsException e){
            MdsException.stderr("Error updating dtype", e);
            return this.dtype;
        }
    }

    public final Flags readFlags() {
        try{
            return this.flags = new Flags(this.nid.getNciFlags());
        }catch(final Exception e){
            MdsException.stderr("Error updating flags", e);
            return this.flags;
        }
    }

    public final String readFullPath() {
        try{
            return this.fullpath = this.nid.getNciFullPath();
        }catch(final MdsException e){
            MdsException.stderr("Error updating fullpath", e);
            return this.fullpath;
        }
    }

    public final int readLength() {
        try{
            return this.length = this.nid.getNciLength();
        }catch(final MdsException e){
            return this.length;
        }
    }

    public final String readMinPath() {
        try{
            this.minpath_default = this.treeview.getDefault().getNidNumber();
            return this.minpath = this.nid.getNciMinPath();
        }catch(final MdsException e){
            return this.minpath;
        }
    }

    public final String readName() {
        try{
            return this.name = this.nid.getNciNodeName();
        }catch(final MdsException e){
            return this.name;
        }
    }

    public final void readNodeInfo() throws MdsException {
        this.copyNodeInfo(this.nid.getNodeInfo());
    }

    public final int readNumSegments() {
        try{
            return this.num_segments = this.nid.getNumSegments();
        }catch(final MdsException e){
            return this.num_segments;
        }
    }

    public final int readOwner() {
        try{
            return this.ownerid = this.nid.getNciOwnerId();
        }catch(final MdsException e){
            return this.ownerid;
        }
    }

    public final String readPath() {
        try{
            return this.path = this.nid.getNciPath();
        }catch(final MdsException e){
            return this.path;
        }
    }

    public final void readRecordInfo() throws MdsException {
        final RecordInfo ri = this.nid.getRecordInfo();
        this.dtype = ri.dtype;
        this.dclass = ri.dclass;
        this.flags = new Flags(ri.get_flags);
        this.length = ri.length;
        this.rlength = ri.rlength;
        this.num_segments = ri.num_segments;
    }

    public final int readRLength() {
        try{
            return this.rlength = this.nid.getNciRLength();
        }catch(final MdsException e){
            return this.rlength;
        }
    }

    public final int readStatus() {
        try{
            return this.status = this.nid.getNciStatus();
        }catch(final MdsException e){
            return this.status;
        }
    }

    public final String[] readTags() {
        try{
            return this.tags = this.nid.getTags();
        }catch(final MdsException e){
            return this.tags = new String[0];
        }
    }

    public final byte readUsage() {
        try{
            return this.usage = this.nid.getNciUsage();
        }catch(final MdsException e){
            return this.usage;
        }
    }

    public final boolean rename(final String newName) {
        return this.changePath(this.parent, newName);
    }

    public final Nid resolveRefSimple() throws MdsException {
        return this.nid.followReference().toNid();
    }

    public final void setAllOnUnchecked() {
        Node currNode = this;
        while(currNode.parent != null)
            currNode = currNode.parent;
        currNode.setOnUnchecked();
    }

    public final void setData(final Descriptor<?> data) throws MdsException {
        this.timeinserted = null;
        this.nid.putRecord(data);
    }

    public final void setDefault() throws MdsException {
        this.nid.setDefault();
        this.treeview.updateDefault();
    }

    public final void setFlag(final byte idx) throws MdsException {
        this.setFlags(1 << idx);
    }

    public final void setFlags(final int flags) throws MdsException {
        this.flags = null;
        this.nid.setFlags(flags);
    }

    public final void setOnUnchecked() {
        this.needsOnCheck = true;
        this.flags = null;
        for(final Node son : this.sons)
            son.setOnUnchecked();
        for(final Node member : this.members)
            member.setOnUnchecked();
    }

    public void setSubtree() throws MdsException {
        this.nid.setSubtree();
        this.label = null;
    }

    public final void setTags(final String[] tags) throws MdsException {
        this.tags = null;
        this.nid.setTags(tags);
    }

    public final DefaultMutableTreeNode setTreeNode(final DefaultMutableTreeNode treenode) {
        if(!this.is_leaf) treenode.add(this.dummy = new DefaultMutableTreeNode(new JLabel("loading...")));
        return this.treenode = treenode;
    }

    public final void setupDevice() {
        Conglom conglom = null;
        try{
            conglom = (Conglom)this.nid.getRecord();
        }catch(final MdsException e){
            JOptionPane.showMessageDialog(this.treeview, e.getMessage(), "Error in device setup 1", JOptionPane.WARNING_MESSAGE);
        }
        if(conglom != null){
            final CString model = (CString)conglom.getModel();
            if(model != null){
                try{
                    final Class<?> Devices = Class.forName("devicebeans.DeviceSetup");
                    final Method DeviceSetup = Devices.getMethod("setup", Node.class, String.class);
                    DeviceSetup.invoke(null, this, model.toString());
                    return;
                }catch(final Exception e){
                    try{
                        this.nid.doDeviceMethod("dw_setup");
                        return;
                    }catch(final MdsException exc){
                        JOptionPane.showMessageDialog(this.treeview, e.getMessage(), "Error in device setup 2: " + e, JOptionPane.WARNING_MESSAGE);
                        e.printStackTrace();
                        return;
                    }
                }
            }
        }
        JOptionPane.showMessageDialog(this.treeview, "Missing model in descriptor", "Error in device setup 3", JOptionPane.WARNING_MESSAGE);
    }

    /**
     * toggles on/off state of the node
     */
    public final void toggle() throws MdsException {
        if(this.nid.isOn()) this.turnOff();
        else this.turnOn();
    }

    /**
     * toggles flags based on mask
     *
     * @param mask
     *            masks the flags that should be toggled
     */
    public final void toggleFlags(final int mask) throws MdsException {
        final int isflags = this.nid.getNciFlags();
        final int clear = isflags & mask, set = (~isflags) & mask;
        this.flags = null;
        if(set != 0) this.nid.setFlags(set);
        if(clear != 0) this.nid.clearFlags(clear);
    }

    @Override
    public final String toString() {
        return this.getName();
    }

    public final void turnOff() {
        try{
            this.nid.setOn(false);
        }catch(final Exception exc){
            MdsException.stderr("Error turning off", exc);
        }
        this.flags = null;
        this.setOnUnchecked();
        this.treeview.reportChange();
    }

    public final void turnOn() {
        try{
            this.nid.setOn(true);
        }catch(final Exception exc){
            MdsException.stderr("Error turning on", exc);
        }
        this.flags = null;
        this.setOnUnchecked();
        this.treeview.reportChange();
    }

    public final void update() {
        this.needsOnCheck = true;
        this.is_leaf = false;
        this.tooltip_life = 0;
        this.num_segments = this.length = this.rlength = this.ownerid = this.minpath_default = this.status = -1;
        this.tooltip_text = this.name = this.minpath = this.path = this.fullpath = this.timeinserted = null;
        this.usage = this.dtype = this.dclass = -1;
        this.flags = null;
    }
}

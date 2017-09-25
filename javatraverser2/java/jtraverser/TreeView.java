/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
package jtraverser;

import java.awt.Component;
import java.awt.datatransfer.StringSelection;
import java.awt.datatransfer.Transferable;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.util.Arrays;
import java.util.Enumeration;
import javax.swing.JComponent;
import javax.swing.JOptionPane;
import javax.swing.JTree;
import javax.swing.ToolTipManager;
import javax.swing.TransferHandler;
import javax.swing.event.TreeExpansionEvent;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.event.TreeWillExpandListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.ExpandVetoException;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;
import jtraverser.TreeManager.Job;
import mds.Mds;
import mds.MdsException;
import mds.data.TREE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Conglom;
import mds.data.descriptor_s.NODE;
import mds.data.descriptor_s.Nid;

@SuppressWarnings("serial")
public final class TreeView extends JTree implements TreeSelectionListener, DataChangeListener, TreeWillExpandListener{
    // Inner class FromTranferHandler managed drag operation
    private static final class FromTransferHandler extends TransferHandler{
        @Override
        public final Transferable createTransferable(final JComponent comp) {
            if(!(comp instanceof TreeView)) return null;
            final TreeView tree = ((TreeView)comp);
            final Node node = tree.getCurrentNode();
            switch(tree.getTreeManager().copy_format){
                case TreeManager.ExtrasMenu.CopyFormat.FULLPATH:
                    return new StringSelection(node.getFullPath());
                case TreeManager.ExtrasMenu.CopyFormat.PATH:
                    return new StringSelection(node.getPath());
                case TreeManager.ExtrasMenu.CopyFormat.MINPATH:
                    return new StringSelection(node.getMinPath());
                case TreeManager.ExtrasMenu.CopyFormat.NODENAME:
                    return new StringSelection(node.getName());
            }
            return null;
        }

        @Override
        public final int getSourceActions(final JComponent comp) {
            return TransferHandler.COPY_OR_MOVE;
        }
    }
    private final class MDSCellRenderer extends DefaultTreeCellRenderer{
        private boolean was_selected;

        @Override
        public Component getTreeCellRendererComponent(final JTree jtree, final Object value, final boolean is_selected, final boolean expanded, final boolean has_focus, final int row, final boolean leaf) {
            final Object usrObj = ((DefaultMutableTreeNode)value).getUserObject();
            Node node;
            if(usrObj instanceof Component) return (Component)usrObj;
            if(usrObj instanceof String){
                node = TreeView.this.getCurrentNode();
                if(node.getTreeNode() == value){
                    final String newName = (((String)usrObj).trim()).toUpperCase();
                    if(TreeView.this.lastName == null || !TreeView.this.lastName.equals(newName)){
                        TreeView.this.lastName = newName;
                        node.rename(newName);
                    }
                    node.getTreeNode().setUserObject(node);
                }
            }else node = (Node)usrObj;
            if(!this.was_selected && is_selected) TreeView.this.getTreeManager().dialogs.update();
            return node.getIcon(this.was_selected = is_selected);
        }
    }

    public static Node addNode(final byte usage, final String name, final Node toNode) {
        Node new_node;
        DefaultMutableTreeNode new_tree_node;
        if(name == null || name.length() == 0 || name.length() > 12){
            JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), "Name length must range between 1 and 12 characters", "Error adding Node", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        try{
            new_node = toNode.addNode(name, usage);
            new_tree_node = new DefaultMutableTreeNode(new_node);
            new_node.setTreeNode(new_tree_node);
            // this.addNodeToParent(new_tree_node, toTreeNode);
        }catch(final Exception e){
            JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), e.getMessage(), "Error adding Node", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return new_node;
    }

    public static final String decompile(final Descriptor<?> data) {
        return data.toString();
    }
    private Node                   curr_node;
    private final TREE             tree;
    private final Mds              mds;
    private String                 lastName;
    private DefaultMutableTreeNode top;
    public final MdsView           mdsview;

    public TreeView(final MdsView mdsview, final String expt, final int shot, final int mode) throws MdsException{
        super();
        this.mdsview = mdsview;
        this.mds = mdsview.getMds();
        this.tree = new TREE(this.mds, expt, shot, mode);
        try{
            this.tree.open();
        }catch(final MdsException me){
            if(mode != TREE.EDITABLE || me.getStatus() != MdsException.TreeFOPENW) throw me;
            final int n = JOptionPane.showConfirmDialog(JOptionPane.getRootFrame(), "Tree " + expt + " cannot be opened in edit mode. Create new instead?", "Editing Tree ", JOptionPane.YES_NO_OPTION);
            if(n != JOptionPane.YES_OPTION) throw me;
            this.tree.open(TREE.NEW);
        }
        this.setTransferHandler(new FromTransferHandler());
        this.setDragEnabled(true);
        /////////////////////////////
        ToolTipManager.sharedInstance().registerComponent(this);
        this.setEditable(this.tree.isEditable());
        this.addTreeSelectionListener(this);
        this.addTreeWillExpandListener(this);
        this.addMouseListener(mdsview.treeman.getContextMenu());
        this.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
        this.addKeyListener(new KeyAdapter(){
            @Override
            public void keyTyped(final KeyEvent e) {
                if(e.getKeyChar() == KeyEvent.VK_CANCEL) // i.e. Ctrl+C
                {
                    TreeView.this.getCurrentNode().copyToClipboard();
                    TreeView.this.getCurrentNode().copy();
                }else if(e.getKeyChar() == 24) // i.e. Ctrl+X
                    if(TreeView.this.isEditable()) TreeView.this.getCurrentNode().cut();
                else TreeView.this.getCurrentNode().copy();
                else if(e.getKeyChar() == 22) // i.e. Ctrl+V
                    TreeView.this.getCurrentNode().paste();
                else if(e.getKeyChar() == KeyEvent.VK_DELETE || e.getKeyChar() == KeyEvent.VK_BACK_SPACE) TreeView.this.getCurrentNode().delete();
                else if(e.getKeyChar() == KeyEvent.VK_SPACE && e.isControlDown()) try{
                    TreeView.this.getCurrentNode().setDefault();
                }catch(final MdsException exc){
                    MdsException.stderr("SetDefault", exc);
                }
                TreeView.this.getTreeManager().reportChange();
            }
        });
        final DefaultTreeModel model = (DefaultTreeModel)this.getModel();
        final Node top_node = new Node(this, this.tree.getTop());
        top_node.setTreeNode(this.top = new DefaultMutableTreeNode(top_node));
        this.loadChildren(this.top, false);
        model.setRoot(this.top);
        this.setCellRenderer(new MDSCellRenderer());
        this.setCurrentNode(0);
        this.updateDefault();
    }

    public Node addNode(final byte usage, final String name) {
        return TreeView.addNode(usage, name, this.getCurrentNode());
    }

    public void addNodeToParent(final DefaultMutableTreeNode treenode, final DefaultMutableTreeNode toTreeNode) {
        final int num_children = toTreeNode.getChildCount();
        int i = 0;
        if(num_children > 0){
            final String name = Node.getNode(treenode).getName();
            String curr_name;
            for(i = 0; i < num_children; i++){
                curr_name = ((Node)((DefaultMutableTreeNode)toTreeNode.getChildAt(i)).getUserObject()).getName();
                if(name.compareTo(curr_name) < 0) break;
            }
        }
        final DefaultTreeModel tree_model = (DefaultTreeModel)TreeView.this.getModel();
        tree_model.insertNodeInto(treenode, toTreeNode, i);
        TreeView.this.expandPath(new TreePath(treenode.getPath()));
        TreeView.this.treeDidChange();
    }

    private final DefaultMutableTreeNode addTreeNode(final DefaultMutableTreeNode treenode, final Node node) {
        final DefaultMutableTreeNode new_node = new DefaultMutableTreeNode(node);
        final int num_children = treenode.getChildCount();
        final DefaultTreeModel tree_model = (DefaultTreeModel)TreeView.this.getModel();
        tree_model.insertNodeInto(new_node, treenode, num_children);
        node.setTreeNode(new_node);
        treenode.add(new_node);
        return new_node;
    }

    public final TreeView close(final boolean quit) {
        try{
            if(quit) this.tree.quitTree();
            else this.tree.close();
        }catch(final Exception e){
            boolean leditable = false;
            String name = null;
            leditable = this.tree.isEditable();
            name = this.tree.expt;
            if(leditable && !quit){
                final int n = JOptionPane.showConfirmDialog(JOptionPane.getRootFrame(), "Tree " + name + " open in edit mode has been changed: Write it before closing?", "Closing Tree ", JOptionPane.YES_NO_OPTION);
                if(n == JOptionPane.YES_OPTION){
                    try{
                        this.tree.writeTree();
                        this.tree.close();
                    }catch(final Exception exc){
                        JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), "Error closing tree", exc.getMessage(), JOptionPane.WARNING_MESSAGE);
                    }
                }else{
                    try{
                        this.tree.quitTree();
                    }catch(final Exception exce){
                        JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), "Error quitting tree", exce.getMessage(), JOptionPane.WARNING_MESSAGE);
                    }
                }
            }else JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), "Error closing tree", e.getMessage(), JOptionPane.WARNING_MESSAGE);
        }
        return this;
    }

    @Override
    public void dataChanged(final DataChangeEvent e) {
        this.mdsview.reportChange();
    }

    public final void deleteNode() {
        this.deleteNode(this.getCurrentNode());
        this.setCurrentNode(0);
    }

    public void deleteNode(final Node delNode) {
        if(delNode == null) return;
        final Node del_node = delNode;
        final int n_children = del_node.deleteInitialize();
        if(n_children < 0) return;
        String msg = "You are about to delete node " + del_node.getName().trim();
        if(n_children > 0) msg += " which has " + n_children + " descendents.\n Please confirm";
        else msg += "\n Please confirm";
        final int n = JOptionPane.showConfirmDialog(JOptionPane.getRootFrame(), msg, "Delete node(s)", JOptionPane.YES_NO_OPTION);
        if(n == JOptionPane.YES_OPTION){
            if(!del_node.deleteExecute()) return;
            this.setSelectionRow(0);
            final DefaultTreeModel tree_model = (DefaultTreeModel)this.getModel();
            tree_model.removeNodeFromParent(delNode.getTreeNode());
        }
    }

    public final DefaultMutableTreeNode findNid(final Nid nid) {
        try{
            return this.findPath(nid.getNciFullPath());
        }catch(final MdsException e){
            e.printStackTrace();
            return null;
        }
    }

    public final DefaultMutableTreeNode findPath(final String path) {
        final String[] treepath = path.split("::", 2);
        return this.findPath(treepath[treepath.length - 1].split("[\\.:]"));
    }

    public final DefaultMutableTreeNode findPath(String[] path) {
        if(path == null || path.length == 0) return this.top;
        if(path[0].equalsIgnoreCase("TOP")) path = Arrays.copyOfRange(path, 1, path.length);
        return this.findSubPath(path, this.top);
    }

    private final DefaultMutableTreeNode findSubPath(final String[] path, final DefaultMutableTreeNode root) {
        if(path == null || path.length == 0) return root;
        final Enumeration<?> children = root.children();
        while(children.hasMoreElements()){
            final DefaultMutableTreeNode child = (DefaultMutableTreeNode)children.nextElement();
            final Node node = (Node)child.getUserObject();
            if(node.getName().equalsIgnoreCase(path[0])) return this.findSubPath(Arrays.copyOfRange(path, 1, path.length), child);
        }
        return null;
    }

    public final Node getCurrentNode() {
        return this.curr_node;
    }

    public final DefaultMutableTreeNode getCurrTreeNode() {
        final Node node = this.getCurrentNode();
        if(node == null) return null;
        return node.getTreeNode();
    }

    public final Nid getDefault() {
        return this.tree.getDefaultC();
    }

    public final String getExpt() {
        return this.tree.expt;
    }

    public final Mds getMds() {
        return this.mds;
    }

    public final int getMode() {
        return this.tree.getMode();
    }

    public final int getShot() {
        return this.tree.shot;
    }

    public final TREE getTree() {
        return this.tree;
    }

    private final TreeManager getTreeManager() {
        return this.mdsview.getTreeManager();
    }

    public final boolean isModel() {
        return this.tree.shot == -1;
    }

    public final boolean isReadOnly() {
        return this.tree.isReadonly();
    }

    private final void loadChildren(final DefaultMutableTreeNode tree_node, final boolean force) {
        final Node currnode = Node.getNode(tree_node);
        if(currnode.dummy != null || force){
            Node sons[], members[];
            DefaultMutableTreeNode last_node = null;
            try{
                currnode.expand();
            }catch(final Exception exc){
                MdsException.stderr("Error expanding tree", exc);
                exc.printStackTrace();
            }
            sons = currnode.getSons();
            members = currnode.getMembers();
            final DefaultTreeModel tree_model = (DefaultTreeModel)this.getModel();
            if(currnode.dummy != null){
                tree_model.removeNodeFromParent(currnode.dummy);
                currnode.dummy = null;
            }
            for(final Node member : members)
                last_node = this.addTreeNode(tree_node, member);
            for(final Node son : sons)
                last_node = this.addTreeNode(tree_node, son);
            if(last_node != null) this.expandPath(new TreePath(tree_node.getPath()));
            TreeView.this.treeDidChange();
        }
    }

    public void pasteSubtree(final Node fromNode, final Node toNode, final boolean isMember) {
        final DefaultMutableTreeNode savedTreeNode = this.getCurrTreeNode();
        try{
            fromNode.expand();
            final String[] usedNames = new String[toNode.getSons().length + toNode.getMembers().length];
            // collect names used so far
            int idx = 0;
            for(final Node son : toNode.getSons())
                usedNames[idx++] = son.getName();
            for(final Node member : toNode.getMembers())
                usedNames[idx++] = member.getName();
            if(fromNode.getUsage() == NODE.USAGE_DEVICE){
                final Conglom conglom = (Conglom)fromNode.getData();
                final Node newNode = toNode.addDevice((isMember ? ":" : ".") + Node.getUniqueName(fromNode.getName(), usedNames), conglom.getModel().toString());
                newNode.expand();
                Node.copySubtreeContent(fromNode, newNode);
            }else{
                final Node newNode = TreeView.addNode(fromNode.getUsage(), (isMember ? ":" : ".") + Node.getUniqueName(fromNode.getName(), usedNames), toNode);
                if(newNode == null) return;
                newNode.expand();
                try{
                    final Descriptor<?> data = fromNode.getData();
                    if(data != null && fromNode.getUsage() != NODE.USAGE_ACTION) newNode.setData(data);
                }catch(final MdsException exc){/**/}
                for(final Node son : fromNode.getSons()){
                    this.pasteSubtree(son, newNode, false);
                }
                for(final Node member : fromNode.getMembers()){
                    this.pasteSubtree(member, newNode, true);
                }
            }
        }catch(final Exception exc){
            JOptionPane.showMessageDialog(this, "" + exc, "Error copying subtree", JOptionPane.WARNING_MESSAGE);
        }
        this.setCurrentNode(savedTreeNode);
    }

    synchronized public final void reportChange() {
        this.treeDidChange();
        this.updateUI();
        this.mdsview.reportChange();
    }

    public void reset() {
        this.top.removeAllChildren();
        final Node topnode = ((Node)this.top.getUserObject());
        topnode.update();
        this.setCurrentNode(-1);
        this.loadChildren(this.top, true);
        this.setCurrentNode(0);
        this.expandRow(0);
        try{
            topnode.setDefault();
        }catch(final MdsException e){/**/}
        this.reportChange();
    }

    public final void setCurrentNode(final DefaultMutableTreeNode treenode) {
        final Node new_curr_node = Node.getNode(treenode);
        if(new_curr_node == null) return;
        this.setCurrentNode(new_curr_node);
    }

    private final void setCurrentNode(final int row) {
        this.setSelectionRow(row);
        final DefaultMutableTreeNode DMN = (DefaultMutableTreeNode)this.getLastSelectedPathComponent();
        if(DMN == null) return;
        this.setCurrentNode(DMN);
    }

    public final void setCurrentNode(final Node curr_node) {
        this.setSelectionPath(new TreePath(curr_node.getTreeNode().getPath()));
        this.curr_node = curr_node;
    }

    @Override
    public final String toString() {
        return this.tree.toString();
    }

    @Override
    public void treeWillCollapse(final TreeExpansionEvent e) throws ExpandVetoException {/*stub*/}

    @Override
    public void treeWillExpand(final TreeExpansionEvent e) throws ExpandVetoException {
        this.getTreeManager().dispatchJob(new Job(){
            @Override
            public final void program() {
                TreeView.this.loadChildren((DefaultMutableTreeNode)e.getPath().getLastPathComponent(), false);
            }
        });
    }

    public final void updateDefault() throws MdsException {
        this.tree.getDefaultNid();
        this.reportChange();
    }

    @Override
    public final void valueChanged(final TreeSelectionEvent e) {
        final Node currnode = Node.getNode((DefaultMutableTreeNode)e.getPath().getLastPathComponent());
        this.setCurrentNode(currnode);
        TreeView.this.treeDidChange();
    }

    public final void writeTree() {
        try{
            this.tree.writeTree();
        }catch(final Exception exc){
            JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), "Error writing tree", exc.getMessage(), JOptionPane.WARNING_MESSAGE);
        }
    }
}

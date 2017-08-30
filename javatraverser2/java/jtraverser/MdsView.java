package jtraverser;

import java.awt.Cursor;
import java.awt.Dimension;
import java.util.Stack;
import javax.swing.JOptionPane;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import mds.Mds;
import mds.MdsEvent;
import mds.MdsException;
import mds.MdsListener;
import mds.TreeShr;
import mds.data.TREE;
import mds.mdsip.MdsIp;

@SuppressWarnings("serial")
public class MdsView extends JTabbedPane implements MdsListener{
    final TreeManager             treeman;
    private final Mds             mds;
    private final Stack<TreeView> trees = new Stack<TreeView>();

    public MdsView(final TreeManager treeman, final Mds mds){
        this.treeman = treeman;
        this.mds = mds;
        mds.addMdsListener(this);
        this.setPreferredSize(new Dimension(300, 400));
        this.setTabLayoutPolicy(JTabbedPane.SCROLL_TAB_LAYOUT);
        this.addChangeListener(new ChangeListener(){
            @Override
            public void stateChanged(final ChangeEvent ce) {
                final TREE tree = MdsView.this.getCurrentTree();
                if(tree == null) return;
                try{
                    tree.setActive();
                }catch(final MdsException e){/**/}
                MdsView.this.reportChange();
            }
        });
    }

    public final MdsView close(final boolean quit) {
        while(!this.trees.empty())
            this.trees.pop().close(quit);
        this.mds.close();
        if(this.mds instanceof MdsIp) MdsIp.removeSharedConnection((MdsIp)this.mds);
        return this;
    }

    public final void closeTree(final boolean quit) {
        final TreeView treeview = this.getCurrentTreeView();
        if(treeview == null) return;
        this.closeTree(this.getSelectedIndex(), quit);
    }

    private void closeTree(final int idx, final boolean quit) {
        if(idx >= this.getTabCount() || idx < 0) return;
        this.trees.remove(this.getTreeAt(idx).close(quit));
        this.removeTabAt(idx);
        if(this.getTabCount() == 0) this.treeman.reportChange();
    }

    public final Node getCurrentNode() {
        final TreeView tree = this.getCurrentTreeView();
        if(tree == null) return null;
        return tree.getCurrentNode();
    }

    public final TREE getCurrentTree() {
        final TreeView tree = this.getCurrentTreeView();
        if(tree == null) return null;
        return tree.getTree();
    }

    public final TreeView getCurrentTreeView() {
        if(this.getTabCount() == 0) return null;
        return (TreeView)((JScrollPane)this.getSelectedComponent()).getViewport().getView();
    }

    public final Mds getMds() {
        return this.mds;
    }

    private final TreeView getTreeAt(final int index) {
        return (TreeView)((JScrollPane)this.getComponentAt(index)).getViewport().getView();
    }

    public final TreeManager getTreeManager() {
        return this.treeman;
    }

    public final void openTree(final String expt, int shot, final int mode) {
        // first we need to check if the tree is already open
        if(shot == 0) try{
            shot = new TreeShr(this.mds).treeGetCurrentShotId(null, expt);
        }catch(final MdsException e){/**/}
        for(int i = this.getTabCount(); i-- > 0;){
            final TreeView tree = this.getTreeAt(i);
            if(!tree.getExpt().equalsIgnoreCase(expt)) continue;
            if(tree.getShot() != shot) continue;
            tree.close(false);
            this.remove(i);
        }
        TreeView tree;
        try{
            tree = new TreeView(this, expt, shot, mode);
        }catch(final MdsException e){
            JOptionPane.showMessageDialog(this.treeman, e.getMessage(), "Error opening tree " + expt, JOptionPane.ERROR_MESSAGE);
            return;
        }
        tree.expandRow(0);
        this.addTab(tree.toString(), new JScrollPane(tree));
        this.setSelectedIndex(this.getTabCount() - 1);
    }

    @Override
    public void processMdsEvent(final MdsEvent e) {
        switch(e.getID()){
            case MdsEvent.IDLE:
                this.setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
                this.treeman.setProgress(this, 0, 1);
                break;
            case MdsEvent.TRANSFER:
                this.setCursor(new Cursor(Cursor.WAIT_CURSOR));
                this.treeman.setProgress(this, e.current_size, e.total_size);
                break;
            case MdsEvent.HAVE_CONTEXT:
            case MdsEvent.LOST_CONTEXT:
                break;
            default:
                System.out.println(e.getID() + e.getInfo());
        }
    }

    synchronized public final void reportChange() {
        this.treeman.reportChange();
    }

    @Override
    public final String toString() {
        return this.mds.toString();
    }
}

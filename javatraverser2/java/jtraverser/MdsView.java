package jtraverser;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.Stack;
import java.util.Vector;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.plaf.basic.BasicButtonUI;
import jtraverser.TreeManager.Job;
import mds.Mds;
import mds.MdsEvent;
import mds.MdsException;
import mds.MdsListener;
import mds.data.TREE;
import mds.mdsip.MdsIp;

@SuppressWarnings("serial")
public class MdsView extends JTabbedPane implements MdsListener{
    class ClosableTab extends JPanel{
	class CloseButton extends JButton implements MouseListener{
	    final static int size = 8;
	    final static int top  = 3;
	    final static int left = 2;

	    public CloseButton(){
	        this.setPreferredSize(new Dimension(CloseButton.size + CloseButton.left, CloseButton.size + CloseButton.top));
	        this.setToolTipText("close Tree");
	        this.setContentAreaFilled(false);
	        this.setBorderPainted(false);
	        this.setFocusable(false);
	        this.setUI(new BasicButtonUI());
	        this.addMouseListener(this);
	        this.setRolloverEnabled(true);
	    }

	    /** when click button, tab will close */
	    @Override
	    public void mouseClicked(final MouseEvent e) {
	        final int index = MdsView.this.indexOfTabComponent(ClosableTab.this);
	        if(index != -1) MdsView.this.closeTree(index, false);
	    }

	    @Override
	    public void mouseEntered(final MouseEvent e) {
	        this.setForeground(Color.RED);
	    }

	    @Override
	    public void mouseExited(final MouseEvent e) {
	        this.setForeground(Color.BLACK);
	    }

	    @Override
	    public void mousePressed(final MouseEvent e) {/**/}

	    @Override
	    public void mouseReleased(final MouseEvent e) {/**/}

	    @Override
	    protected void paintComponent(final Graphics g) {
	        super.paintComponent(g);
	        final Graphics2D g2 = (Graphics2D)g.create();
	        if(this.getModel().isPressed()) g2.translate(1, 1);
	        g2.setStroke(new BasicStroke(2));
	        g2.setColor(new Color(126, 118, 91));
	        if(this.getModel().isRollover()) g2.setColor(Color.RED);
	        g2.drawLine(CloseButton.left + 1, CloseButton.top + 1, this.getWidth() - 2, this.getHeight() - 2);
	        g2.drawLine(this.getWidth() - 2, CloseButton.top + 1, CloseButton.left + 1, this.getHeight() - 2);
	        g2.dispose();
	    }

	    @Override
	    public void updateUI() {/**/}
	}

	public ClosableTab(){
	    this.setLayout(new FlowLayout(FlowLayout.LEFT, 0, 0));
	    this.setOpaque(false);
	    final JLabel label = new JLabel(){
	        /** set text for JLabel, it will title of tab */
	        @Override
	        public String getText() {
	            final int index = MdsView.this.indexOfTabComponent(ClosableTab.this);
	            if(index != -1) return MdsView.this.getTitleAt(index);
	            return null;
	        }
	    };
	    this.add(label);
	    this.add(new CloseButton());
	}
    }
    final TreeManager             treeman;
    private final Mds             mds;
    private final Stack<TreeView> trees                   = new Stack<TreeView>();
    Vector<Job>                   change_report_listeners = new Vector<Job>();

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

    public void addChangeReportListener(final Job job) {
	this.change_report_listeners.add(job);
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

    public void dispatchChangeReportListener() {
	for(final Job job : this.change_report_listeners)
	    job.program();
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

    public final void openTree(final String expt, int shot, final int mode) {
	// first we need to check if the tree is already open
	if(shot == 0) try{
	    shot = this.mds.getAPI().treeGetCurrentShotId(null, expt);
	}catch(final MdsException e){/**/}
	int index = -1;
	for(int i = this.getTabCount(); i-- > 0;){
	    final TreeView tree = this.getTreeAt(i);
	    if(!tree.getExpt().equalsIgnoreCase(expt)) continue;
	    if(tree.getShot() != shot) continue;
	    tree.close(false);
	    this.remove(i);
	    index = i;
	}
	if(index < 0) index = this.getTabCount();
	TreeView tree;
	try{
	    tree = new TreeView(this, expt, shot, mode);
	}catch(final MdsException e){
	    JOptionPane.showMessageDialog(this.treeman, e.getMessage(), "Error opening tree " + expt, JOptionPane.ERROR_MESSAGE);
	    return;
	}
	tree.addChangeReportListener(new Job(){
	    @Override
	    public void program() {
	        MdsView.this.reportChange();
	    }
	});
	tree.expandRow(0);
	this.add(new JScrollPane(tree), tree.getLabel(), index);
	this.setTabComponentAt(index, new ClosableTab());
	this.setSelectedIndex(index);
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
	        System.err.println(String.format("Unknown MdsEventId: %d - %s", Integer.valueOf(e.getID()), e.getInfo()));
	}
    }

    public void removeChangeReportListener(final Job job) {
	this.change_report_listeners.remove(job);
    }

    synchronized public final void reportChange() {
	this.treeman.reportChange();
    }

    public void set_copy_format(final int format) {
	for(final TreeView tree : this.trees)
	    tree.set_copy_format(format);
    }

    @Override
    public final String toString() {
	return this.mds.toString();
    }
}

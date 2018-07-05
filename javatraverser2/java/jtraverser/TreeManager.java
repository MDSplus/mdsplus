package jtraverser;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Toolkit;
import java.awt.datatransfer.StringSelection;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.InputEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.Stack;
import java.util.Vector;
import java.util.jar.Attributes;
import java.util.jar.Manifest;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JProgressBar;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.ScrollPaneConstants;
import javax.swing.ToolTipManager;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.table.DefaultTableModel;
import javax.swing.tree.DefaultMutableTreeNode;
import jtraverser.dialogs.ActionList;
import jtraverser.dialogs.DataDialog;
import jtraverser.dialogs.Dialogs;
import jtraverser.dialogs.GraphPanel;
import jtraverser.dialogs.OpenTreeDialog;
import jtraverser.dialogs.SubTreeList;
import jtraverser.dialogs.TagsDialog;
import jtraverser.tools.DecompileTree;
import mds.Mds;
import mds.MdsException;
import mds.MdsShr;
import mds.TCL;
import mds.TreeShr;
import mds.TreeShr.TagRefStatus;
import mds.data.TREE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.NODE;
import mds.data.descriptor_s.Nid;
import mds.mdsip.MdsIp;
import mds.mdsip.MdsIp.Provider;
import mds.mdslib.MdsLib;

@SuppressWarnings("serial")
public class TreeManager extends JPanel{
    // XXX: AddNodeMenu
    public static final class AddNodeMenu extends Menu{
        private final class AddDevice implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                final Node currnode = AddNodeMenu.this.treeman.getCurrentNode();
                if(currnode == null) return;
                final JTextField device_type = new JTextField(12), device_name = new JTextField(12);
                final Object[] content = {"Device:", device_type, "Name:", device_name};
                final String title = new StringBuilder(256).append("Add device to: ").append(currnode.getFullPath()).toString();
                final int result = JOptionPane.showConfirmDialog(JOptionPane.getRootFrame(), content, title, JOptionPane.OK_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE);
                if(result == 0) try{
                    currnode.addDevice(device_name.getText().toUpperCase(), device_type.getText());
                }catch(final MdsException me){
                    MdsException.stderr("AddDevice", me);
                }
            }
        }
        public final class AddNodeAL implements ActionListener{
            private final byte lusage;

            public AddNodeAL(final byte usage_in){
                this.lusage = usage_in;
            }

            @Override
            public final void actionPerformed(final ActionEvent e) {
                AddNodeMenu.this.treeman.dialogs.addNode.open(AddNodeMenu.this.treeman.getCurrentNode(), this.lusage);
            }
        }
        private final String[] name  = new String[]{   //
                "Structure",                           //
                "Subtree",                             //
                "Action",                              //
                "Any",                                 //
                "Axis",                                //
                "Dispatch",                            //
                "Numeric",                             //
                "Signal",                              //
                "Task",                                //
                "Text",                                //
                "Window"                               //
        };
        private final byte[]   usage = new byte[]{     //
                NODE.USAGE_STRUCTURE,                  //
                NODE.USAGE_SUBTREE,                    //
                NODE.USAGE_ACTION,                     //
                NODE.USAGE_ANY,                        //
                NODE.USAGE_AXIS,                       //
                NODE.USAGE_DISPATCH,                   //
                NODE.USAGE_NUMERIC,                    //
                NODE.USAGE_SIGNAL,                     //
                NODE.USAGE_TASK,                       //
                NODE.USAGE_TEXT,                       //
                NODE.USAGE_WINDOW                      //
        };

        public AddNodeMenu(final TreeManager treeman, final JComponent menu){
            super(treeman, menu);
            this.addMenuItem("Device", new AddDevice());
            this.addSeparator();
            for(int i = 0; i < this.name.length; i++){
                this.addMenuItem(this.name[i], new AddNodeAL(this.usage[i]));
                if(i == 1) this.addSeparator();
            }
        }
    }
    // XXX: ContextMenuML
    public final class ContextMenuML extends MouseAdapter{
        private static final int CtrlClick = InputEvent.CTRL_DOWN_MASK | InputEvent.BUTTON1_DOWN_MASK;

        @Override
        public final void mouseClicked(final MouseEvent ev) {
            if(ev.isPopupTrigger()){
                final TreeView tree = (TreeView)ev.getSource();
                final DefaultMutableTreeNode tree_node = (DefaultMutableTreeNode)tree.getClosestPathForLocation(ev.getX(), ev.getY()).getLastPathComponent();
                final Node currnode = Node.getNode(tree_node);
                tree.setCurrentNode(currnode);
                final JPopupMenu pop = new JPopupMenu();
                pop.setLayout(new GridBagLayout());
                new TreeManager.DisplayMenu(TreeManager.this, pop, 0).checkSupport();
                if(!tree.isReadOnly()) new TreeManager.ModifyMenu(TreeManager.this, pop, 1).checkSupport();
                if(tree.isEditable()) new TreeManager.EditMenu(TreeManager.this, pop, 2).checkSupport();
                pop.show((Component)ev.getSource(), ev.getX(), ev.getY());
            }else if((ev.getModifiers() & InputEvent.BUTTON1_MASK) == InputEvent.BUTTON1_MASK && (ev.getModifiersEx() & ContextMenuML.CtrlClick) == ContextMenuML.CtrlClick){
                final TreeView tree = (TreeView)ev.getSource();
                final DefaultMutableTreeNode tree_node = (DefaultMutableTreeNode)tree.getClosestPathForLocation(ev.getX(), ev.getY()).getLastPathComponent();
                final Node currnode = Node.getNode(tree_node);
                tree.setCurrentNode(currnode);
                if(currnode.isSubTree()) try{
                    currnode.toggleFlags(NODE.Flags.INCLUDE_IN_PULSE);
                }catch(final MdsException e){
                    MdsException.stderr("INCLUDE_IN_PULSE", e);
                }
                else try{
                    currnode.toggle();
                }catch(final MdsException e){
                    MdsException.stderr("TOGGLE STATE", e);
                }
                tree.treeDidChange();
                TreeManager.this.dialogs.update();
            }
        }

        @Override
        public void mouseEntered(final MouseEvent e) {/*stub*/}

        @Override
        public void mouseExited(final MouseEvent e) {/*stub*/}

        @Override
        public void mousePressed(final MouseEvent e) {
            this.mouseClicked(e);
        }

        @Override
        public void mouseReleased(final MouseEvent e) {
            this.mouseClicked(e);
        }
    }
    // XXX: DisplayMenu
    public static final class DisplayMenu extends Menu{
        public final class DisplayDataMenu extends Menu{
            private class EvalActionListener implements ActionListener{
                private final String evalexpr;

                EvalActionListener(final String evalexpr){
                    this.evalexpr = evalexpr;
                }

                @Override
                public void actionPerformed(final ActionEvent e) {
                    String aeeval;
                    final Node currnode = DisplayMenu.this.treeman.getCurrentNode();
                    if(currnode == null) return;
                    try{
                        final NODE<?> datanode = currnode.nid.followReference();
                        final TREE tree = DisplayDataMenu.this.treeman.getCurrentTree();
                        final Descriptor<?> aedata = tree.mds.getDescriptor(tree.ctx, this.evalexpr, datanode);
                        if(aedata == null) aeeval = "no data";
                        else aeeval = aedata.toString();
                    }catch(final MdsException de){
                        aeeval = de.getMessage();
                    }
                    Toolkit.getDefaultToolkit().getSystemClipboard().setContents(new StringSelection(aeeval), null);
                    JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), "<html><body><p style='width: 360px;'>" + aeeval + "</p></body></html>", this.evalexpr.replace("$", currnode.getFullPath()), JOptionPane.PLAIN_MESSAGE);
                }
            }

            public DisplayDataMenu(final TreeManager treeman){
                super(treeman, new JMenu("Display Data..."));
                this.addMenuItem("Plot Signal", new DisplaySignal());
                this.addMenuItem("DATA($)", new EvalActionListener("DATA($)"));
                this.addMenuItem("RAW_OF($)", new EvalActionListener("RAW_OF($)"));
                this.addMenuItem("DIM_OF($)", new EvalActionListener("DIM_OF($)"));
                this.addMenuItem("SHAPE($)", new EvalActionListener("SHAPE($)"));
            }

            @Override
            public final void checkSupport() {
                final Node node = this.treeman.getCurrentNode();
                boolean[] mask = new boolean[this.items.size()];
                if(node != null){
                    final int usage = node.getUsage();
                    final boolean enable = !(usage == NODE.USAGE_STRUCTURE || usage == NODE.USAGE_SUBTREE);
                    mask = new boolean[]{enable, enable, enable, enable, enable};
                }
                for(int i = 0; i < mask.length; i++)
                    this.items.get(i).setEnabled(mask[i]);
            }

            public JMenuItem getMenu() {
                return (JMenuItem)this.menu;
            }
        }
        public final class DisplayNci implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                final Node currnode = DisplayMenu.this.treeman.getCurrentNode();
                if(currnode == null) return;
                JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), currnode.getInfoTextBox(), "Display NCI information", JOptionPane.PLAIN_MESSAGE);
            }
        }
        public final class DisplaySignal implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent ae) {
                final Node currnode = DisplayMenu.this.treeman.getCurrentNode();
                if(currnode == null) return;
                final NODE<?> node = currnode.nid;
                final String fullpath = currnode.getFullPath();
                DisplayMenu.this.treeman.dispatchJob(new Job(){
                    @Override
                    public final void program() {
                        try{
                            GraphPanel.plotNode(node, JOptionPane.getRootFrame(), fullpath);
                        }catch(final Exception e){
                            e.printStackTrace();
                        }
                    }
                });
            }
        }
        public final class DisplayTags implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                final Node currnode = DisplayMenu.this.treeman.getCurrentNode();
                if(currnode == null) return;
                JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), currnode.getTagTextBox(), "Display Node Tags", JOptionPane.PLAIN_MESSAGE);
            }
        }
        public final class ModifyFlags implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                DisplayMenu.this.treeman.dialogs.modifyFlags.open();
            }
        }
        public final class SetDefault implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                final Node currnode = DisplayMenu.this.treeman.getCurrentNode();
                if(currnode == null) return;
                try{
                    currnode.setDefault();
                }catch(final MdsException exc){
                    MdsException.stderr("SetDefault", exc);
                }
            }
        }
        private final DisplayDataMenu displaydatamenu;

        public DisplayMenu(final TreeManager treeman, final JComponent menu, final int column){
            super(treeman, menu, column);
            this.addMenuItem("Display Data", new Menu.DataPanelAL(false));
            this.addMenuItem((this.displaydatamenu = new DisplayDataMenu(treeman)).getMenu(), null);
            this.addSeparator();
            this.addMenuItem("Display Nci", new DisplayNci());
            this.addMenuItem("Display Flags", new ModifyFlags());
            this.addMenuItem("Display Tags", new DisplayTags());
            this.addSeparator();
            this.addMenuItem("Set Default", new DisplayMenu.SetDefault());
        }

        @Override
        public final void checkSupport() {
            this.displaydatamenu.checkSupport();
            final Node node = this.treeman.getCurrentNode();
            boolean[] mask = new boolean[this.items.size()];
            if(node != null){
                final int usage = node.getUsage();
                final boolean enable = !(usage == NODE.USAGE_STRUCTURE || usage == NODE.USAGE_SUBTREE);
                mask = new boolean[]{enable, enable, true, true, true, true};
            }
            for(int i = 0; i < mask.length; i++)
                this.items.get(i).setEnabled(mask[i]);
        }
    }
    // XXX: EditMenu
    public static final class EditMenu extends Menu{
        private final class CopyNode implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                EditMenu.this.treeman.getCurrentNode().copy();
            }
        }
        private final class DeleteNode implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                EditMenu.this.treeman.getCurrentTreeView().deleteNode();
            }
        }
        public final class EditTags implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                final Node currnode = EditMenu.this.treeman.getCurrentNode();
                if(currnode == null) return;
                TagsDialog.open(currnode);
            }
        }
        public final class PasteNode implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                EditMenu.this.treeman.getCurrentNode().paste();
            }
        }
        public final class RenameNode implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                EditMenu.this.treeman.rename();
            }
        }

        @SuppressWarnings("unused")
        public EditMenu(final TreeManager treeman, final JComponent menu, final int column){
            super(treeman, menu, column);
            final JMenu submenu = new JMenu("Add Node...");
            this.items.add(submenu);
            if(this.gbc == null) menu.add(submenu);
            else menu.add(submenu, this.gbc);
            new TreeManager.AddNodeMenu(treeman, submenu);
            this.addMenuItem("Delete Node", new DeleteNode());
            this.addMenuItem("Rename Node", new RenameNode());
            this.addMenuItem("Edit Tags", new EditTags());
            this.addSeparator();
            this.addMenuItem("Copy", new CopyNode());
            this.addMenuItem("Paste", new PasteNode());
        }

        @Override
        public final void checkSupport() {
            final Node node = this.treeman.getCurrentNode();
            boolean[] mask = new boolean[this.items.size()];
            if(node != null){
                final int usage = node.getUsage();
                final boolean isst = usage == NODE.USAGE_SUBTREE;
                mask = new boolean[]{!isst || node.nid.getNidNumber() == 0, node.nid.getNidNumber() != 0, !isst, !isst, node.nid.getNidNumber() > 0, Node.isCopied()};
            }
            for(int i = 0; i < mask.length; i++)
                this.items.get(i).setEnabled(mask[i]);
        }
    }
    // XXX: ExtraMenu
    public static final class ExtrasMenu extends Menu{
        public final class AlwaysOnTop implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                ExtrasMenu.this.treeman.frame.setAlwaysOnTop(!ExtrasMenu.this.treeman.frame.isAlwaysOnTop());
            }
        }
        public final class CopyFormat extends JMenu implements ActionListener{
            public static final int    FULLPATH = 0;
            public static final int    PATH     = 1;
            public static final int    MINPATH  = 2;
            public static final int    NODENAME = 3;
            final JRadioButtonMenuItem fullpath, path, minpath, nodename;

            public CopyFormat(){
                super("Copy Format");
                final ButtonGroup group = new ButtonGroup();
                group.add(this.add(this.fullpath = new JRadioButtonMenuItem("FullPath")));
                group.add(this.add(this.path = new JRadioButtonMenuItem("Path")));
                group.add(this.add(this.minpath = new JRadioButtonMenuItem("MinPath")));
                group.add(this.add(this.nodename = new JRadioButtonMenuItem("NodeName")));
                this.fullpath.setSelected(true);
                this.fullpath.addActionListener(this);
                this.path.addActionListener(this);
                this.minpath.addActionListener(this);
                this.nodename.addActionListener(this);
            }

            @Override
            public final void actionPerformed(final ActionEvent e) {
                if(e.getSource().equals(CopyFormat.this.fullpath)) ExtrasMenu.this.treeman.copy_format = CopyFormat.FULLPATH;
                else if(e.getSource().equals(CopyFormat.this.path)) ExtrasMenu.this.treeman.copy_format = CopyFormat.PATH;
                else if(e.getSource().equals(CopyFormat.this.minpath)) ExtrasMenu.this.treeman.copy_format = CopyFormat.MINPATH;
                else if(e.getSource().equals(CopyFormat.this.nodename)) ExtrasMenu.this.treeman.copy_format = CopyFormat.NODENAME;
            }
        }
        public final class ShowActionList implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                new ActionList(ExtrasMenu.this.treeman).open();
            }
        }
        public final class ShowDatabase implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                try{
                    final Mds mds = ExtrasMenu.this.treeman.getMds();
                    final TreeShr tree = new TreeShr(mds);
                    final TCL tcl = new TCL(mds);
                    final boolean wasprivate = tree.treeSetPrivateCtx(false);
                    final StringBuilder msg = new StringBuilder(256).append("Public:\n");
                    msg.append(tcl.showDatabase());
                    tree.treeSetPrivateCtx(true);
                    msg.append("\nPrivate:\n").append(tcl.showDatabase());
                    tree.treeSetPrivateCtx(wasprivate);
                    JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), msg.toString(), mds.toString(), JOptionPane.PLAIN_MESSAGE);
                }catch(final MdsException ex){/**/}
            }
        }
        public final class ShowSubTreeList implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                new SubTreeList(ExtrasMenu.this.treeman).open();
            }
        }
        public final class ShowTags implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                final DefaultTableModel tabmodel = new DefaultTableModel(0, 2);
                final JTable table = new JTable(tabmodel);
                table.setPreferredScrollableViewportSize(new Dimension(600, 500));
                table.getColumnModel().getColumn(0).setHeaderValue("Tag");
                table.getColumnModel().getColumn(1).setHeaderValue("Full Path");
                table.getColumnModel().getColumn(0).setPreferredWidth(100);
                table.getColumnModel().getColumn(1).setPreferredWidth(300);
                final JScrollPane scollpane = new JScrollPane(table);
                scollpane.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
                scollpane.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
                final Thread thread = new Thread("ShowTags"){
                    {/** worker to list the tags and the full path of the target node **/
                        this.setDaemon(true);
                        this.setPriority(Thread.MIN_PRIORITY);
                    }

                    @Override
                    public final void run() {
                        final TreeView treeview = ExtrasMenu.this.treeman.getCurrentTreeView();
                        final TREE tree = treeview.getTree();
                        TagRefStatus tag = TagRefStatus.init;
                        try{
                            final String root = new StringBuilder(tree.expt.length() + 3).append("\\").append(tree.expt).append("::").toString();
                            while((tag = tree.findTagWild("***", tag)).ok()){
                                tabmodel.addRow(new String[]{tag.data.replace(root, "\\"), new Nid(tag.nid, tree).toString()});
                                synchronized(this){
                                    if(this.isInterrupted()) return;
                                }
                            }
                        }catch(final MdsException exc){/**/}
                    }
                };
                thread.start();
                JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), scollpane, ExtrasMenu.this.treeman.getCurrentTree().toString(), JOptionPane.PLAIN_MESSAGE);
                if(thread.isAlive()) thread.interrupt();
            }
        }

        public ExtrasMenu(final TreeManager treeman, final JComponent menu){
            super(treeman, menu);
            this.addMenuItem(new JCheckBoxMenuItem("Always on top"), new AlwaysOnTop());
            menu.add(new CopyFormat());
            this.addMenuItem("Show DataBase", new ShowDatabase());
            this.addSeparator();
            this.addMenuItem("List Actions", new ShowActionList());
            this.addMenuItem("List Subtrees", new ShowSubTreeList());
            this.addMenuItem("List Tags", new ShowTags());
        }

        @Override
        public final void checkSupport() {
            final boolean open = this.treeman.getCurrentTree() != null;
            this.items.get(0).setEnabled(ExtrasMenu.this.treeman.frame.isAlwaysOnTopSupported());
            ((JCheckBoxMenuItem)this.items.get(0)).setSelected(ExtrasMenu.this.treeman.frame.isAlwaysOnTop());
            this.items.get(1).setEnabled(this.treeman.getCurrentMdsView() != null);
            for(final JMenuItem item : this.items.subList(2, this.items.size()))
                item.setEnabled(open);
        }
    }
    // XXX: FileMenu
    public static final class FileMenu extends Menu{
        private final class CloseMds implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                FileMenu.this.treeman.closeMds();
            }
        }
        private final class CloseTree implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                FileMenu.this.treeman.closeTree();
            }
        } /*private final class Compile implements ActionListener{
          @Override
          public  final void actionPerformed(final ActionEvent e) {
          FileMenu.this.treeman.compile();
          }
          }*/
        private final class Decompile implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                new DecompileTree().decompile(FileMenu.this.treeman.getCurrentTree(), FileMenu.this.treeman.frame, true);
            }
        }
        private final class OpenMds implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                final JTextField provider = new JTextField(FileMenu.prev_provider, 20);
                final Object[] ob = {"[ssh://][<user>@]<hostname>[:<port>]", provider};
                final int result = JOptionPane.showConfirmDialog(JOptionPane.getRootFrame(), ob, "Open new connection", JOptionPane.OK_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE);
                if(result == JOptionPane.OK_OPTION) new Thread(new Runnable(){
                    @Override
                    public void run() {
                        FileMenu.this.treeman.openMds(new Provider(provider.getText()));
                    }
                }).start();
            }
        }
        private final class OpenTree implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                FileMenu.this.treeman.opentree_dialog.open();
            }
        }
        private final class Quit implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                FileMenu.this.treeman.quit();
            }
        }
        private final class ResetTree implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                FileMenu.this.treeman.getCurrentTreeView().reset();
            }
        }
        private final class Write implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                FileMenu.this.treeman.writeTree();
            }
        }
        private static String prev_provider = "";

        public FileMenu(final TreeManager treeman, final JComponent menu, final int column){
            super(treeman, menu, column);
            this.addMenuItem("Connect", new OpenMds());
            this.addMenuItem("Open", new OpenTree());
            this.addMenuItem("Write", new Write());
            this.addMenuItem("Close", new CloseTree());
            this.addMenuItem("Disconnect", new CloseMds());
            this.addSeparator();
            this.addMenuItem("Export", new Decompile());
            /* this.addMenuItem("Import",new Compile());*/
            this.addSeparator();
            this.addMenuItem("Reset", new ResetTree());
            this.addMenuItem("Quit", new Quit());
        }

        @Override
        public final void checkSupport() {
            final boolean noconnected = this.treeman.getCurrentMdsView() == null;
            final boolean nodisconnect = noconnected || this.treeman.getMds() instanceof MdsLib;
            final boolean noopen = noconnected || this.treeman.getCurrentTree() == null;
            int i = 1;
            this.items.get(i++).setEnabled(!noconnected);
            this.items.get(i++).setEnabled(noopen ? false : this.treeman.getCurrentTree().isEditable());
            this.items.get(i++).setEnabled(!noopen);
            this.items.get(i++).setEnabled(!nodisconnect);
            // this.items.get(i++).setEnabled(!noopen);
            this.items.get(i++).setEnabled(!noopen);
            this.items.get(i++).setEnabled(!noopen);
        }
    }
    public interface Job{
        public void program();
    }
    // XXX: Menu
    public static class Menu{
        protected final class DataPanelAL implements ActionListener{
            private final boolean editable;

            public DataPanelAL(final boolean editable){
                this.editable = editable;
            }

            @Override
            public final void actionPerformed(final ActionEvent e) {
                final Node currnode = Menu.this.treeman.getCurrentNode();
                if(currnode == null) return;
                Menu.this.treeman.dispatchJob(new Job(){
                    @Override
                    public final void program() {
                        try{
                            DataDialog.open(currnode, DataPanelAL.this.editable);
                        }catch(final Exception ex){
                            ex.printStackTrace();
                        }
                    }
                });
            }
        }
        protected final List<JMenuItem>    items = new ArrayList<JMenuItem>();
        final protected TreeManager        treeman;
        protected final GridBagConstraints gbc;
        protected final JComponent         menu;

        public Menu(final TreeManager treeman, final JComponent menu){
            this(treeman, menu, -1);
        }

        public Menu(final TreeManager treeman, final JComponent menu, final int column){
            this.treeman = treeman;
            this.menu = menu;
            if(column < 0) this.gbc = null;
            else{
                this.gbc = new GridBagConstraints();
                this.gbc.gridx = column;
            }
        }

        protected final JMenuItem addMenuItem(final JMenuItem item, final ActionListener l) {
            this.items.add(item);
            item.addActionListener(l);
            if(this.gbc == null) this.menu.add(item);
            else this.menu.add(item, this.gbc);
            return item;
        }

        protected final JMenuItem addMenuItem(final String name, final ActionListener l) {
            return this.addMenuItem(new JMenuItem(name), l);
        }

        protected void addSeparator() {
            if(this.gbc == null) this.menu.add(new JSeparator());
        }

        public void checkSupport() {/*stub*/}
    }
    // XXX: ModifyMenu
    public static final class ModifyMenu extends Menu{
        public final class DoAction implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent ae) {
                final Node currnode = ModifyMenu.this.treeman.getCurrentNode();
                if(currnode == null) return;
                ModifyMenu.this.treeman.dispatchJob(new Job(){
                    @Override
                    public final void program() {
                        try{
                            currnode.doAction();
                        }catch(final Exception ex){
                            ex.printStackTrace();
                        }
                    }
                });
            }
        }
        public final class ModifyFlags implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                ModifyMenu.this.treeman.dialogs.modifyFlags.open();
            }
        }
        public final class SetupDevice implements ActionListener{
            @Override
            public final void actionPerformed(final ActionEvent e) {
                final Node currnode = ModifyMenu.this.treeman.getCurrentNode();
                if(currnode == null) return;
                currnode.setupDevice();
            }
        }
        private final class TurnOnOff implements ActionListener{
            private final boolean on;

            public TurnOnOff(final boolean on){
                this.on = on;
            }

            @Override
            public final void actionPerformed(final ActionEvent e) {
                final Node currnode = ModifyMenu.this.treeman.getCurrentNode();
                if(currnode == null) return;
                if(this.on) currnode.turnOn();
                else currnode.turnOff();
                ModifyMenu.this.treeman.reportChange();
            }
        }

        public ModifyMenu(final TreeManager treeman, final JComponent menu, final int column){
            super(treeman, menu, column);
            this.addMenuItem("Modify Data", new Menu.DataPanelAL(true));
            this.addMenuItem("Modify Flags", new ModifyFlags());
            this.addSeparator();
            this.addMenuItem("Turn On", new TurnOnOff(true));
            this.addMenuItem("Turn Off", new TurnOnOff(false));
            this.addSeparator();
            this.addMenuItem("Setup Device", new SetupDevice());
            this.addMenuItem("Do Action", new DoAction());
        }

        @Override
        public final void checkSupport() {
            final Node node = this.treeman.getCurrentNode();
            boolean[] mask = new boolean[this.items.size()];
            if(node != null){
                final int usage = node.getUsage();
                final boolean isst = usage == NODE.USAGE_STRUCTURE || usage == NODE.USAGE_SUBTREE;
                final boolean isact = usage == NODE.USAGE_ACTION || usage == NODE.USAGE_TASK;
                final boolean isdev = usage == NODE.USAGE_DEVICE;
                mask = new boolean[]{!isst, true, true, true, isdev, isact};
            }
            for(int i = 0; i < mask.length; i++)
                this.items.get(i).setEnabled(mask[i]);
        }
    }
    static{
        ToolTipManager.sharedInstance().setDismissDelay(60000);
        ToolTipManager.sharedInstance().setLightWeightPopupEnabled(true);
    }
    private final JProgressBar     progress = new JProgressBar();
    private final JLabel           status   = new JLabel();
    public int                     copy_format;
    public final Dialogs           dialogs;
    private final jTraverserFacade frame;
    private final OpenTreeDialog   opentree_dialog;
    private final Stack<MdsView>   mdsviews = new Stack<MdsView>();
    private final JTabbedPane      tabs     = new JTabbedPane();
    private final Vector<Thread>   jobs     = new Vector<Thread>();
    private final JButton          abort;

    public TreeManager(final jTraverserFacade frame){
        this(frame, null);
    }

    public TreeManager(final jTraverserFacade frame, final JScrollPane pane){
        super(new BorderLayout());
        this.frame = frame;
        this.setPreferredSize(new Dimension(300, 400));
        this.opentree_dialog = new OpenTreeDialog(this);
        this.dialogs = new Dialogs(this);
        this.add(this.tabs, BorderLayout.CENTER);
        final JPanel jp = new JPanel(new BorderLayout());
        jp.add(this.progress, BorderLayout.CENTER);
        this.abort = new JButton("abort");
        this.abort.setEnabled(!this.jobs.isEmpty());
        this.abort.setMargin(new Insets(0, 0, 0, 0));
        this.abort.addActionListener(new ActionListener(){
            @Override
            public void actionPerformed(final ActionEvent arg0) {
                TreeManager.this.abortJobs();
            }
        });
        jp.add(this.abort, BorderLayout.AFTER_LINE_ENDS);
        this.add(jp, BorderLayout.PAGE_END);
        this.tabs.setForeground(new Color(0, 0, 127));
        this.tabs.setTabLayoutPolicy(JTabbedPane.SCROLL_TAB_LAYOUT);
        this.tabs.addChangeListener(new ChangeListener(){
            @Override
            public final void stateChanged(final ChangeEvent ce) {
                final TREE tree = TreeManager.this.getCurrentTree();
                if(tree == null) return;
                try{
                    tree.setActive();
                }catch(final MdsException e){/**/}
                TreeManager.this.reportChange();
            }
        });
        String builddate = "unknown";
        try{
            final Class<jTraverserFacade> clazz = jTraverserFacade.class;
            final String className = clazz.getSimpleName() + ".class";
            final String classPath = clazz.getResource(className).toString();
            if(classPath.startsWith("jar")){
                final String manifestPath = classPath.substring(0, classPath.lastIndexOf("!") + 1) + "/META-INF/MANIFEST.MF";
                final Manifest manifest = new Manifest(new URL(manifestPath).openStream());
                final Attributes attr = manifest.getMainAttributes();
                builddate = attr.getValue("Built-Date").substring(0, 10);
            }
        }catch(final Exception e){
            e.printStackTrace();
        }
        this.progress.setLayout(new BorderLayout(0, 0));
        this.progress.add(this.status, BorderLayout.CENTER);
        this.status.setText(new StringBuilder(64).append("jTaverser started (Build-Date ").append(builddate).append(")").toString());
        MdsException.setStatusLabel(this.status);
        if(MdsLib.lib_loaded == null) this.addMds(new MdsLib().setActive());
    }

    public void abortJobs() {
        synchronized(this.jobs){
            for(final Thread thread : this.jobs)
                thread.interrupt();
        }
    }

    public final MdsView addMds(final Mds mds) {
        for(int i = 0; i < this.tabs.getTabCount(); i++){
            final MdsView mdsview = (MdsView)this.tabs.getComponentAt(i);
            if(mdsview.getMds().equals(mds)){
                this.tabs.setSelectedComponent(mdsview);
                return mdsview;
            }
        }
        final MdsView mdsview = new MdsView(this, mds);
        this.tabs.addTab(mdsview.toString(), mdsview);
        this.tabs.setSelectedIndex(this.tabs.getTabCount() - 1);
        return mdsview;
    }

    public final void closeMds() {
        final MdsView mdsview = this.getCurrentMdsView();
        if(mdsview == null) return;
        this.closeMds(this.tabs.getSelectedIndex());
    }

    public final void closeMds(final int idx) {
        if(idx >= this.tabs.getTabCount() || idx < 0) return;
        this.mdsviews.remove(this.getMdsViewAt(idx).close(false));
        this.tabs.removeTabAt(idx);
        if(this.tabs.getTabCount() == 0) this.frame.reportChange(null);
    }

    public final void closeTree() {
        final MdsView mdsview = this.getCurrentMdsView();
        if(mdsview == null) return;
        mdsview.closeTree(false);
    }

    public void dispatchJob(final Job job) {
        new Thread(){
            @Override
            public void run() {
                TreeManager.this.jobs.addElement(this);
                TreeManager.this.abort.setEnabled(true);
                try{
                    job.program();
                }finally{
                    TreeManager.this.jobs.remove(this);
                    TreeManager.this.abort.setEnabled(!TreeManager.this.jobs.isEmpty());
                }
            }
        }.start();
    }

    public final MouseListener getContextMenu() {
        return new TreeManager.ContextMenuML();
    }

    public final MdsView getCurrentMdsView() {
        if(this.tabs.getTabCount() == 0) return null;
        return (MdsView)this.tabs.getSelectedComponent();
    }

    public final Node getCurrentNode() {
        final MdsView mdsview = this.getCurrentMdsView();
        if(mdsview == null) return null;
        return mdsview.getCurrentNode();
    }

    public final TREE getCurrentTree() {
        final MdsView mdsview = this.getCurrentMdsView();
        if(mdsview == null) return null;
        return mdsview.getCurrentTree();
    }

    public TreeView getCurrentTreeView() {
        final MdsView mdsview = this.getCurrentMdsView();
        if(mdsview == null) return null;
        return mdsview.getCurrentTreeView();
    }

    public final Mds getMds() {
        final MdsView mdsview = this.getCurrentMdsView();
        if(mdsview == null) return null;
        return mdsview.getMds();
    }

    private final MdsView getMdsViewAt(final int index) {
        return (MdsView)this.tabs.getComponentAt(index);
    }

    public final MdsView openMds(final Provider provider) {
        FileMenu.prev_provider = provider.toString();
        // first we need to check if the tree is already open
        final MdsIp mds = MdsIp.sharedConnection(provider);
        if(mds.isReady() != null){
            JOptionPane.showMessageDialog(this.frame, "Could not connect to server " + provider.toString(), "Connection Error", JOptionPane.ERROR_MESSAGE);
            return null;
        }
        return this.addMds(mds);
    }

    public final void openTree(final String expt, final int shot, final int mode) {
        this.opentree_dialog.setFields(expt, shot);
        final MdsView mdsview = this.getCurrentMdsView();
        if(mdsview == null) return;
        mdsview.openTree(expt, shot, mode);
    }

    public final void quit() {
        while(!this.mdsviews.empty())
            this.mdsviews.pop().close(true);
        System.exit(0);
    }

    public final void rename() {
        final Node currnode = this.getCurrentNode();
        if(currnode == null) return;
        final JTextField new_name = new JTextField();
        final Object[] ob = {"Enter new name:", new_name};
        final int result = JOptionPane.showConfirmDialog(JOptionPane.getRootFrame(), ob, "Rename: " + currnode, JOptionPane.OK_CANCEL_OPTION);
        if(result != JOptionPane.OK_OPTION) return;
        final String name = new_name.getText();
        if(name == null || name.length() == 0) return;
        try{
            currnode.rename(name);
        }catch(final Exception exc){
            JOptionPane.showMessageDialog(this.frame, exc.getMessage(), "Error renaming Node", JOptionPane.WARNING_MESSAGE);
            return;
        }
        this.reportChange();
    }

    synchronized public final void reportChange() {
        this.dialogs.update();
        this.frame.reportChange(this.getCurrentMdsView());
        this.frame.repaint();
    }

    public final void setProgress(final MdsView mdsView, final int value, final int max) {
        if(!mdsView.equals(this.getCurrentMdsView())) return;
        this.progress.setValue(value);
        this.progress.setMaximum(max);
    }

    public final void setTreePathEnv(final String exp, final String path) {
        final Mds mds;
        if(exp == null || path == null || exp.length() == 0 || path.length() == 0 || (mds = this.getMds()) == null) return;
        try{
            new MdsShr(mds).mdsPutEnv(null, new StringBuilder(8 + exp.length() + path.length()).append(exp.toLowerCase()).append("_path=").append(path).toString());
        }catch(final MdsException e){
            JOptionPane.showMessageDialog(this, "Could not set environment variable.\n" + e.getMessage(), "Error setting tree_path", JOptionPane.WARNING_MESSAGE);
        }
    }

    public final void writeTree() {
        this.getCurrentTreeView().writeTree();
    }
}

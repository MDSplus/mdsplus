//package jTraverser;
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.event.*;
import java.util.*;
import java.beans.*;
import java.rmi.*;
import java.rmi.RemoteException.*;

public class Tree extends JScrollPane implements TreeSelectionListener,
    MouseListener, ActionListener, KeyListener, DataChangeListener
{
    static boolean is_remote;
    boolean is_angled_style;
    DefaultMutableTreeNode top;
    //Node top_node, curr_node;
    JMenuItem menu_items[];
    boolean is_editable;
    JPopupMenu pop = null;
    MethodDescriptor curr_method_descr;
    PropertyDescriptor curr_property_descr;
    static jTraverser frame;
    private Point curr_origin;
    static int curr_dialog_idx;
    DialogSet dialog_sets[];
    java.util.Stack trees, experiments;
    static JTree curr_tree;
    static RemoteTree curr_experiment;
    public static int context;
    Node curr_node = null;
    static DefaultMutableTreeNode curr_tree_node;
    JDialog open_dialog = null, add_node_dialog = null, add_subtree_dialog = null;
    JTextField open_exp, open_shot;
    JCheckBox open_readonly, open_edit, open_realtime;
    JTextField add_node_name, add_node_tag, add_subtree_name;
    int add_node_usage;
    JDialog modify_tags_dialog;
    JDialog add_device_dialog;
    JList modify_tags_list;
    JTextField curr_tag_selection;
    DefaultListModel curr_taglist_model;
    String [] tags;
    JDialog rename_dialog;
    JTextField new_node_name;
	JTextField add_device_type, add_device_name;
	static Hashtable nodeHash = new Hashtable();
	String lastName;
	public static boolean isRemote(){return is_remote;}

// Temporary, to vercome Java's bugs on inner classes
    JMenuItem open_b, close_b, quit_b;
    JMenuItem add_action_b, add_dispatch_b, add_numeric_b, add_signal_b, add_task_b, add_text_b,
	add_window_b, add_axis_b, add_device_b, add_child_b, add_subtree_b, delete_node_b, modify_tags_b,
	rename_node_b;
    JButton ok_cb, add_node_ok;

    public Tree(JFrame _frame)
    {
    	this((jTraverser)_frame);
    }

    public Tree(jTraverser _frame)
    {
	    frame = _frame;
	    trees = new java.util.Stack();
	    experiments = new java.util.Stack();
	    setPreferredSize(new Dimension(300,400));
	    setBackground(Color.white);
	    curr_tree = null;
	    curr_experiment = null;
	    String def_tree = System.getProperty("tree");
	    if(def_tree != null)
	    {
	        String def_shot = System.getProperty("shot");
	        int shot;
	        if(def_shot != null)
		    shot = Integer.parseInt(def_shot);
	        else
		    shot = -1;
	        open(def_tree, shot, false, false, false);
	    }

    }


    static String dataToString(Data data)
    {
        if(is_remote)
            try {
                return curr_experiment.dataToString(data);
            }catch(Exception exc){return exc.toString();}
        else
            return data.toString();
    }

    static Data dataFromExpr(String expr)
    {
        if(is_remote)
            try {
                return curr_experiment.dataFromExpr(expr);
            }catch(Exception exc){return null;}
        else
            return Data.fromExpr(expr);
    }


    void setAngled(boolean is_angled)
    {
	    is_angled_style = is_angled;
	    for(int i = 0; i < trees.size(); i++)
	    {
	        JTree curr_tree = (JTree)trees.elementAt(i);
	        if(is_angled_style)
		    curr_tree.putClientProperty("JTree.lineStyle", "Angled");
	        else
		    curr_tree.putClientProperty("JTree.lineStyle", "None");
	        curr_tree.treeDidChange();
	    }
    }


    Node getCurrentNode() {return curr_node;}
    void quit()
    {
	while(!experiments.empty())
	{
	    curr_experiment = (RemoteTree)experiments.pop();
	    try {
		curr_experiment.close(Tree.context);
	    } catch(Exception e)
	    {
	        boolean editable = false;
	        String name = null;
	        try {
	            editable = curr_experiment.isEditable();
	            name = curr_experiment.getName();
	        }catch(Exception exc){}
		    if(editable)
		    {
		        int n = JOptionPane.showConfirmDialog(frame, "Tree has been changed: write it before closing?",
			    "Tree " + name + " open in edit mode", JOptionPane.YES_NO_OPTION);
		        if(n == JOptionPane.YES_OPTION)
		        {
			        try{
			            curr_experiment.write(Tree.context);
			            curr_experiment.close(Tree.context);
			        }catch(Exception exc)
			        {
			            System.out.println("Error closing experiment: " + exc.getMessage());
			        }
		        }
		    }
		    else
		        System.out.println("Error closing experiment: " + e.getMessage());
	        }
	}
	System.exit(0);
    }


    void close()
    {
	if(curr_tree == null)
	    return;
	try {
	    curr_experiment.close(Tree.context);
	} catch(Exception e) {
	    boolean editable = false;
	    String name = null;
	    try {
	        editable = curr_experiment.isEditable();
	        name = curr_experiment.getName().trim();
	    }catch(Exception exc){}
		if(editable)
	    {
		    int n = JOptionPane.showConfirmDialog(frame, "Tree " + name +
		        " open in edit mode has been changed: write it before closing?",
			    "Closing Tree ", JOptionPane.YES_NO_OPTION);
		    if(n == JOptionPane.YES_OPTION)
		    {
		        try{
			    curr_experiment.write(Tree.context);
			    curr_experiment.close(Tree.context);
		        }catch(Exception exc)
		        {
			    JOptionPane.showMessageDialog(frame, "Error closing tree", exc.getMessage(),JOptionPane.WARNING_MESSAGE);
			    return;
		        }
		    }
		    else
		    {
		        try {
			    curr_experiment.quit(Tree.context);
		        } catch(Exception exce) {
			    JOptionPane.showMessageDialog(frame, "Error quitting tree", exce.getMessage(),JOptionPane.WARNING_MESSAGE);
			    return;
		        }
		    }
	    }
	    else
	    {
		JOptionPane.showMessageDialog(frame, "Error closing tree", e.getMessage(),JOptionPane.WARNING_MESSAGE);
		return;
	    }
	}
	trees.pop();
	experiments.pop();
	if(!trees.empty())
	{
	    curr_tree = (JTree)trees.peek();
	    curr_experiment = (RemoteTree)experiments.peek();
	    setViewportView(curr_tree);
	    try {
	        frame.reportChange(curr_experiment.getName(), curr_experiment.getShot(),
		    curr_experiment.isEditable(), curr_experiment.isReadonly());
	        if(is_editable != curr_experiment.isEditable())
		    pop = null;
	        is_editable = curr_experiment.isEditable();
	    }catch(Exception exc) {System.err.println("Error in RMI communication: "+exc);}

	}
	else
	{
	    curr_tree = null;
	    curr_experiment = null;
	    JPanel jp = new JPanel();
	    setViewportView(new JPanel());
	    frame.reportChange(null, 0, false, false);

	}

    DeviceSetup.closeOpenDevices();
	frame.pack();
	repaint();
    }


    void open()
    {
    	if(open_dialog == null)
	{
	    if(curr_origin == null)
		curr_origin = new Point(100, 100);
	    open_dialog = new JDialog(frame);
	    open_dialog.setTitle("Open new tree");
	    //open_dialog.setLocation(curr_origin);
	    JPanel mjp = new JPanel();
	    mjp.setLayout(new BorderLayout());
	    JPanel jp1 = new JPanel();
	    jp1.setLayout(new GridLayout(4,1));
	    jp1.add(new JLabel("Tree: "));
	    jp1.add(new JLabel("Shot: "));
        open_edit = new JCheckBox("edit");
        jp1.add(open_edit);
        open_realtime = new JCheckBox("realtime");
        jp1.add(open_realtime);
	    mjp.add(jp1, "West");
	    jp1 = new JPanel();
	    jp1.setLayout(new GridLayout(3,1));
	    open_exp = new JTextField(20);
	    jp1.add(open_exp);
	    jp1.add(open_shot = new JTextField(10));
	    jp1.add(open_readonly = new JCheckBox("readonly"));
	    mjp.add(jp1, "East");
	    jp1 = new JPanel();
	    ok_cb = new JButton("Ok");
	    ok_cb.addActionListener(this);//(new ActionListener() {
		//public void actionPerformed(ActionEvent e)  {
		  //  open_ok(); }});
		ok_cb.setSelected(true);
	    jp1.add(ok_cb);
	    JButton cancel = new JButton("Cancel");
	    cancel.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e)  {
		    open_dialog.setVisible(false);  }});
	    jp1.add(cancel);
	    mjp.add(jp1, "South");
	    open_dialog.getContentPane().add(mjp);
	    open_shot.addKeyListener(this);
	    open_exp.addKeyListener(this);
	    open_dialog.pack();
	    open_dialog.setLocation(curr_origin);
	    open_dialog.setVisible(true);
	}
	else
	{
	    open_exp.setText("");
	    open_shot.setText("");
	    open_dialog.setLocation(curr_origin);
	    open_dialog.setVisible(true);
        open_edit.setSelected(false);
        open_realtime.setSelected(false);
	    open_readonly.setSelected(false);
	}
    }

    private void open_ok()
    {
	    String exp = open_exp.getText(), shot_t = open_shot.getText();
	    if(exp == null || exp.length() == 0)
	    {
	        JOptionPane.showMessageDialog(open_dialog, "Missing experiment name", "Error opening tree",
		    JOptionPane.WARNING_MESSAGE);
	        repaint();
	        return;
	    }
	    int shot;
	    if(shot_t == null || shot_t.length() == 0)
	    {
	        JOptionPane.showMessageDialog(open_dialog, "Wrong shot number", "Error opening tree",
		    JOptionPane.WARNING_MESSAGE);
	        return;
	    }
	    try{
	    shot = Integer.parseInt(shot_t);
	    } catch(Exception e)	{
	        JOptionPane.showMessageDialog(curr_tree, "Wrong shot number", "Error opening tree",
		    JOptionPane.WARNING_MESSAGE);
	        return;
	    }



	    if(open_edit.isSelected() && open_readonly.isSelected())
	    {
	        JOptionPane.showMessageDialog(curr_tree, "Tree cannot be open in both edit and readonly mode", "Error opening tree",
		    JOptionPane.WARNING_MESSAGE);
	        return;
	    }
	    if(is_editable != open_edit.isSelected())
	        pop = null;
	    open(exp.toUpperCase(), shot, open_edit.isSelected(), open_readonly.isSelected(), open_realtime.isSelected());
	    open_dialog.setVisible(false);
	    frame.pack();
	    repaint();

    }



    public void open(String exp, int shot, boolean editable, boolean readonly, boolean realtime)
    {
	    int i;
	    Node top_node = null;
	    JTree prev_tree = curr_tree;

    //first we need to check if the tree is already open
	    RemoteTree loop_exp = null;
	    for(i = 0; i < trees.size(); i++)
	    {
	        loop_exp = (RemoteTree)experiments.elementAt(i);
	        try {
	            if(loop_exp.getName().equals(exp) && loop_exp.getShot() == shot)
		        break;
		    }catch(Exception exc){}
	    }
	    if(i < trees.size())
	    {
	        trees.removeElementAt(i);
	        experiments.removeElementAt(i);
	    }

	    String remote_tree_ip = System.getProperty("remote_tree.ip");
	    if(remote_tree_ip == null)
	    {
	        is_remote = false;
	        curr_experiment = new Database();
	    }
	    else
	    {
	        is_remote = true;
            if (System.getSecurityManager() == null)
                System.setSecurityManager(new RMISecurityManager()
                    {
                        public void checkConnect(String host, int port){}
                        public void checkPropertiesAccess() {}
                        public void checkLink(String lib){}
                        public void checkRead(String file){}
                        public void checkRead(FileDescriptor fd){}
                        public void checkRead(String file, Object context){}
                        public void checkPropertyAccess(String property) {}
                    });
            String name = "//" + remote_tree_ip + "/TreeServer";
            try {
                curr_experiment = (RemoteTree) Naming.lookup(name);
            }catch(Exception exc)
            {
	            JOptionPane.showMessageDialog(frame, exc.getMessage(), "Error opening remote "+exp, JOptionPane.ERROR_MESSAGE);
	            return;
	        }
	    }


	    if(shot == 0)
	    {
	        try {
	            shot = curr_experiment.getCurrentShot(exp);
	        }catch(Exception  exc) {
	            JOptionPane.showMessageDialog(open_dialog, "Shot 0 not defined for "+ exp, "Error opening tree",
		        JOptionPane.WARNING_MESSAGE);
	            return;
	        }
	    }




	    try {
            curr_experiment.setTree(exp, shot);
            curr_experiment.setEditable(editable);
            curr_experiment.setReadonly(readonly);
            curr_experiment.setRealtime(realtime);
        }catch(Exception exc)
        {
            System.err.println("Error in RMI communication: "+ exc);
        }

	    //curr_experiment = new Database(exp, shot);
	    //curr_experiment.is_editable = editable;
	    //curr_experiment.is_readonly = readonly;
	    try {
	        Tree.context = curr_experiment.open();
	        top_node = new Node(curr_experiment, this);
	    } catch (Exception e) {
	        JOptionPane.showMessageDialog(frame, e.getMessage(), "Error opening "+exp, JOptionPane.ERROR_MESSAGE);
	        return;
	    }


	    top = new DefaultMutableTreeNode(top_node);
	    nodeHash.put(top, top_node);
	    try {
	        top_node.expand();
	    } catch(Exception e) {System.out.println("Error expanding tree "+e);}
	    Node[]members = top_node.getMembers();
	    for(i = 0; i < members.length; i++)
	    {
	        DefaultMutableTreeNode currNode;
	        top.add(currNode = new DefaultMutableTreeNode(members[i]));
	        nodeHash.put(currNode, members[i]);
	    }
	    Node sons[] = top_node.getSons();
	    for(i = 0; i < sons.length; i++)
	    {
	        DefaultMutableTreeNode currNode;
	        top.add(currNode = new DefaultMutableTreeNode(sons[i]));
	        nodeHash.put(currNode, sons[i]);
	    }
	    curr_tree = new JTree(top);
        ToolTipManager.sharedInstance().registerComponent(curr_tree);


            curr_tree.addKeyListener(new KeyAdapter() {
              public void keyTyped(KeyEvent e) {
                if ( (e.getModifiers() & Event.CTRL_MASK) != 0) {
                  int cc = e.getKeyChar();
                  //if(e.getKeyChar() == 'c')
                  if (e.getKeyChar() == 3) {
                    TreeNode.copyToClipboard();
                  }
                }
                if (!jTraverser.isEditable())
                  return;
                if ( (e.getModifiers() & Event.CTRL_MASK) != 0) {
                  if (e.getKeyChar() == 'c')
                    TreeNode.copy();
                  if (e.getKeyChar() == 'v')
                    TreeNode.paste();
                }
                else if (e.getKeyChar() == KeyEvent.VK_DELETE ||
                         e.getKeyChar() == KeyEvent.VK_BACK_SPACE)
                  TreeNode.delete();
              }
            });





	    if(is_angled_style)
	        curr_tree.putClientProperty("JTree.lineStyle", "Angled");

	    // GAB curr_tree.setEditable(false);
	    try {
	        if(curr_experiment.isEditable())
	            curr_tree.setEditable(true);
	        else
	            curr_tree.setEditable(false);
	    }catch(Exception exc){curr_tree.setEditable(false);}
	    curr_tree.setCellRenderer(new TreeCellRenderer()
	    {
	        public Component getTreeCellRendererComponent(JTree tree, Object value, boolean isSelected,
		        boolean expanded, boolean boh, int row, boolean leaf)
	        {
	            Object usrObj = ((DefaultMutableTreeNode)value).getUserObject();
	            Node node;
	            if(usrObj instanceof String)
	            {
	                node = (Node)nodeHash.get(value);
	                String newName = (((String)usrObj).trim()).toUpperCase();
	                if(lastName == null || !lastName.equals(newName))
	                {
	                    lastName = newName;
	                    if(newName.length() > 12 || newName.length() == 0)
	                    {
	                        JOptionPane.showMessageDialog(frame, "Node name lengh must be between 1 and 12 characters",
		                    "Error renaming node", JOptionPane.WARNING_MESSAGE);
		                }
		                else
		                {
		                    try {
		                        node.renameLast(newName);
		                    }catch(Exception exc)
		                    {
                                JOptionPane.showMessageDialog(frame, "Error renaming node: "+ exc,
		                        "Error renaming node", JOptionPane.WARNING_MESSAGE);
		                    }
		                }
		            }
	                ((DefaultMutableTreeNode)value).setUserObject(node);
	            }
	            else
		            node = (Node)usrObj;
		        return node.getIcon();
	        }
	    });
	    curr_tree.addTreeSelectionListener(this);
	    curr_tree.addMouseListener(this);
	    curr_tree.getSelectionModel().setSelectionMode(
	        TreeSelectionModel.SINGLE_TREE_SELECTION);
	    setViewportView(curr_tree);
	    trees.push(curr_tree);
	    experiments.push(curr_experiment);
	    try {
	        is_editable = curr_experiment.isEditable();
	    }catch(Exception exc){is_editable = false;}
	    frame.reportChange(exp,shot,editable,readonly);
    }


    public void valueChanged(TreeSelectionEvent e)
    {
	    DefaultMutableTreeNode tree_node = (DefaultMutableTreeNode)e.getPath().getLastPathComponent();
	    if(tree_node.isLeaf())
	    {
	        curr_node = (Node)tree_node.getUserObject();
	        Node sons[], members[];
	        DefaultMutableTreeNode last_node = null;
	        try {
		        curr_node.expand();
	        } catch(Exception exc) {System.out.println("Error expanding tree " + exc);}
	        sons = curr_node.getSons();
	        members = curr_node.getMembers();
	        for(int i = 0; i < members.length; i++)
	        {
		        tree_node.add(last_node = new DefaultMutableTreeNode(members[i]));
		        nodeHash.put(last_node, members[i]);
		    }
	        for(int i = 0; i < sons.length; i++)
	        {
		        tree_node.add(last_node = new DefaultMutableTreeNode(sons[i]));
		        nodeHash.put(last_node, sons[i]);
		    }
	        if(last_node != null)
		    curr_tree.expandPath(new TreePath(last_node.getPath()));
	    }
    }


    public void mouseClicked(MouseEvent e)
    {}
    public void mouseEntered(MouseEvent e){}
    public void mouseExited(MouseEvent e){}
    public void mouseReleased(MouseEvent e){}
    public void mousePressed(MouseEvent e)
    {
	    if(curr_tree == null) return;
	    int item_idx;

	    curr_tree_node =
		    (DefaultMutableTreeNode)curr_tree.getClosestPathForLocation(e.getX(), e.getY()).getLastPathComponent();
	    curr_node = (Node)curr_tree_node.getUserObject();
	    //if(e.isPopupTrigger())
	    if((e.getModifiers() & InputEvent.BUTTON3_MASK) != 0)
	    {
	        Point screen_origin = getLocationOnScreen();
	        //curr_origin = new Point(e.getX()+screen_origin.x, e.getY()+screen_origin.y);
            curr_origin = screen_origin;
            curr_origin.x += 20;
            curr_origin.y += 20;


	        NodeBeanInfo nbi = curr_node.getBeanInfo();
	        final PropertyDescriptor [] node_properties = nbi.getPropertyDescriptors();
	        final MethodDescriptor [] node_methods = nbi.getMethodDescriptors();

	        if(pop == null)
	        {
		        //dialogs = new JDialog[node_properties.length];
		        dialog_sets = new DialogSet[node_properties.length];
		        for(int i = 0; i < node_properties.length; i++)
		            dialog_sets[i] = new DialogSet();
    		        pop = new JPopupMenu();
		        menu_items = new JMenuItem[node_properties.length + node_methods.length];
	            if(is_editable)
	            {
		            JMenuItem jm = new JMenu("Add Node");
		            jm.add(add_action_b = new JMenuItem("Action"));
		            add_action_b.addActionListener(this);
		            jm.add(add_dispatch_b = new JMenuItem("Dispatch"));
		            add_dispatch_b.addActionListener(this);
		            jm.add(add_numeric_b = new JMenuItem("Numeric"));
		            add_numeric_b.addActionListener(this);
		            jm.add(add_signal_b = new JMenuItem("Signal"));
		            add_signal_b.addActionListener(this);
		            jm.add(add_task_b = new JMenuItem("Task"));
		            add_task_b.addActionListener(this);
		            jm.add(add_text_b = new JMenuItem("Text"));
		            add_text_b.addActionListener(this);
		            jm.add(add_window_b = new JMenuItem("Window"));
		            add_window_b.addActionListener(this);
		            jm.add(add_axis_b = new JMenuItem("Axis"));
		            add_axis_b.addActionListener(this);
		            pop.add(jm);
		            pop.add(add_device_b = new JMenuItem("Add Device"));
		            add_device_b.addActionListener(this);
		            pop.add(add_child_b = new JMenuItem("Add Child"));
		            add_child_b.addActionListener(this);
		            pop.add(add_subtree_b = new JMenuItem("Add Subtree"));
		            add_subtree_b.addActionListener(this);
		            pop.add(delete_node_b = new JMenuItem("Delete Node"));
		            delete_node_b.addActionListener(this);
		            pop.add(modify_tags_b = new JMenuItem("Modify tags"));
		            modify_tags_b.addActionListener(this);
		            pop.add(rename_node_b = new JMenuItem("Rename node"));
		            rename_node_b.addActionListener(this);
		            pop.addSeparator();
	            }
	            item_idx = 0;
	            for(int i = 0; i < node_properties.length; i++)
	            {
		            pop.add(menu_items[item_idx] = new JMenuItem(
			            node_properties[i].getShortDescription()));
		            menu_items[item_idx].addActionListener(new ActionListener() {
		                public void actionPerformed(ActionEvent e) {
			            int idx;
			            for(idx = 0; idx < node_properties.length &&
			            !e.getActionCommand().equals(node_properties[idx].getShortDescription()); idx++);
			            if(idx < node_properties.length)
			            {
			                Tree.curr_dialog_idx = idx;

			                TreeDialog curr_dialog = dialog_sets[idx].getDialog(
				            node_properties[idx].getPropertyEditorClass(), curr_node);
			                curr_dialog.pack();
			                curr_dialog.setLocation(curr_origin);
			                curr_dialog.setVisible(true);
			            }
		            }
		            });
		            item_idx++;
	            }
    	        for(int i = 0; i < node_methods.length; i++)
	            {
		            pop.add(menu_items[item_idx] = new JMenuItem(
			        node_methods[i].getShortDescription()));
		            menu_items[item_idx].addActionListener(new ActionListener() {
			        public void actionPerformed(ActionEvent e) {
			            int idx;
			            for(idx = 0; idx < node_methods.length &&
			        !e.getActionCommand().equals(node_methods[idx].getShortDescription()); idx++);
			            if(idx < node_methods.length)
			            {
				            try {
				                node_methods[idx].getMethod().invoke(curr_node, null);
				            }catch(Exception exc) {System.out.println("Error executing " + exc); }
				            curr_tree.expandPath(new TreePath(curr_tree_node.getPath()));
				            curr_tree.treeDidChange();
			            }
			        }
		            });
		            item_idx++;
	            }
	            pop.addSeparator();
	            pop.add(open_b = new JMenuItem("Open"));
	            open_b.addActionListener(this);//) {
		        //public void actionPerformed(ActionEvent e) { Open(); }});

	            pop.add(close_b = new JMenuItem("Close"));
	            close_b.addActionListener(this);//new ActionListener() {
		        //public void actionPerformed(ActionEvent e) {close(); }});

	            pop.add(quit_b = new JMenuItem("Quit"));
	            quit_b.addActionListener(this);
	        }
	        item_idx = 0;
	        for(int i = 0; i < node_properties.length; i++)
	        {
	            if(!nbi.isSupported(node_properties[i].getShortDescription()))
		        menu_items[item_idx].setEnabled(false);
	            else
		        menu_items[item_idx].setEnabled(true);
	            item_idx++;
	        }
	        for(int i = 0; i < node_methods.length; i++)
	        {
	            if(!nbi.isSupported(node_methods[i].getShortDescription()))
		        menu_items[item_idx].setEnabled(false);
	            else
		        menu_items[item_idx].setEnabled(true);
	            item_idx++;
	        }

	        pop.show(curr_tree, e.getX(), e.getY());
	    }
	    else
	    {
	        TreeNode.setSelectedNode(curr_node);
	        curr_tree.treeDidChange();
	    }
    }

    public void addNode(int usage)
    {
	    if(curr_node == null) return;

	    add_node_usage = usage;
	    if(add_node_dialog == null)
	    {
	        add_node_dialog = new JDialog(frame);
	        add_node_dialog.setLocation(curr_origin);
	        JPanel jp = new JPanel();
	        jp.setLayout(new BorderLayout());
	        JPanel jp1 = new JPanel();
	        jp1.add(new JLabel("Node name: "));
	        jp1.add(add_node_name = new JTextField(12));
	        jp.add(jp1, "North");
	        jp1 = new JPanel();
	        jp1.add(new JLabel("Node tag: "));
	        jp1.add(add_node_tag = new JTextField(12));
	        jp.add(jp1, "Center");
	        jp1 = new JPanel();
	        jp1.add(add_node_ok = new JButton("Ok"));
	        add_node_ok.addActionListener(this);
	        JButton cancel_b = new JButton("Cancel");
	        cancel_b.addActionListener(new ActionListener() {
		    public void actionPerformed(ActionEvent e) {
		        add_node_dialog.setVisible(false); }} );
	        jp1.add(cancel_b);
	        jp.add(jp1, "South");
	        add_node_dialog.getContentPane().add(jp);
	        add_node_dialog.addKeyListener(new KeyAdapter() {
	            public void keyTyped(KeyEvent e)
	            {
	                if(e.getKeyCode() == KeyEvent.VK_ENTER)
	                    addNode();
	            }
	        });

	        add_node_dialog.pack();
	        add_node_dialog.setVisible(true);
	    }
	    add_node_name.setText("");
	    add_node_tag.setText("");
	    add_node_dialog.setTitle("Add to: "+ curr_node.getFullPath());
	    add_node_dialog.setLocation(curr_origin);
	    add_node_dialog.setVisible(true);
    }
    public void addSubtree()
    {
	    if(curr_node == null) return;
	    if(add_subtree_dialog == null)
	    {
	        add_subtree_dialog = new JDialog(frame);
	        add_subtree_dialog.setLocation(curr_origin);
	        JPanel jp = new JPanel();
	        jp.setLayout(new BorderLayout());
	        JPanel jp1 = new JPanel();
	        jp1.add(new JLabel("Node name: "));
	        jp1.add(add_subtree_name = new JTextField(12));
	        jp.add(jp1, "North");
	        jp1 = new JPanel();
	        JButton ok = new JButton("Ok");
	        ok.addActionListener(new ActionListener() {
	            public void actionPerformed(ActionEvent e)
	            {
	                Node node = addNode(NodeInfo.USAGE_STRUCTURE, "."+ add_subtree_name.getText().toUpperCase());
                    try {
                        node.setSubtree();
                    }catch(Exception exc)
                    {
                        System.err.println("Error setting subtree: " + exc);
                    }
	                add_subtree_dialog.setVisible(false);
	            }
            });
	        jp1.add(ok);
	        JButton cancel_b = new JButton("Cancel");
	        cancel_b.addActionListener(new ActionListener() {
		    public void actionPerformed(ActionEvent e) {
		        add_subtree_dialog.setVisible(false); }} );
	        jp1.add(cancel_b);
	        jp.add(jp1, "South");
	        add_subtree_dialog.getContentPane().add(jp);
	        add_subtree_dialog.pack();
	        add_subtree_dialog.setVisible(true);
	    }
	    add_subtree_dialog.setTitle("Add Subtree to: "+ curr_node.getFullPath());
	    add_subtree_dialog.setLocation(curr_origin);
	    add_subtree_dialog.setVisible(true);
    }


    public void addDevice()
    {
	    if(curr_node == null) return;
	    if(add_device_dialog == null)
	    {
	        add_device_dialog = new JDialog(frame);
	        add_device_dialog.setLocation(curr_origin);
	        JPanel jp = new JPanel();
	        jp.setLayout(new BorderLayout());
	        JPanel jp1 = new JPanel();
	        jp1.add(new JLabel("Device: "));
	        jp1.add(add_device_type = new JTextField(12));
	        jp.add(jp1, "North");
	        jp1 = new JPanel();
	        jp1.add(new JLabel("Name:   "));
	        jp1.add(add_device_name = new JTextField(12));
	        jp.add(jp1, "South");
	        jp1 = new JPanel();
	        JButton ok_button;
	        jp1.add(ok_button = new JButton("Ok"));
	        ok_button.addActionListener(new ActionListener()  {
	            public void actionPerformed(ActionEvent e)  {
	              if(addDevice(add_device_name.getText().toUpperCase(), add_device_type.getText()) == null);
	                add_device_dialog.setVisible(false);
	        }});
	        JButton cancel_b = new JButton("Cancel");
	        cancel_b.addActionListener(new ActionListener() {
		    public void actionPerformed(ActionEvent e) {
		        add_device_dialog.setVisible(false); }} );
	        jp1.add(cancel_b);
	        JPanel jp2;
	        jp2 = new JPanel();
	        jp2.setLayout(new BorderLayout());
	        jp2.add(jp, "North");
	        jp2.add(jp1, "South");
	        add_device_dialog.getContentPane().add(jp2);
	        add_device_dialog.addKeyListener(new KeyAdapter() {
	            public void keyTyped(KeyEvent e)
	            {
	                if(e.getKeyCode() == KeyEvent.VK_ENTER)
	                    if(addDevice(add_device_name.getText().toUpperCase(), add_device_type.getText()) == null);
	                        add_device_dialog.setVisible(false);
	            }
	        });

	        add_device_dialog.pack();
	    }
	    add_device_name.setText("");
	    add_device_type.setText("");
	    add_device_dialog.setTitle("Add device to: "+ curr_node.getFullPath());
	    add_device_dialog.setLocation(curr_origin);
	    add_device_dialog.setVisible(true);
    }


	public Node addNode(int usage, String name)
	{
	    return addNode(usage, name, curr_node);
	}



    public static Node addNode(int usage, String name, Node toNode)
    {
        return addNode(usage, name, toNode, curr_tree_node);
    }

    public static Node addNode(int usage, String name, Node toNode, DefaultMutableTreeNode toTreeNode)
    {
        Node new_node = null;
	    DefaultMutableTreeNode new_tree_node = null;
	    if(name == null || name.length() == 0 || name.length() > 12)
	    {
	        JOptionPane.showMessageDialog(frame, "Name length must range between 1 and 12 characters",
		    "Error adding Node", JOptionPane.WARNING_MESSAGE);
	        return null;
	    }
	    try {
	        new_node = toNode.addNode(usage, name);
	        int num_children = toTreeNode.getChildCount();
	        int i;
	        if(num_children > 0)
	        {
		        String curr_name;
		        for(i = 0; i < num_children; i++)
		        {
		            curr_name = ((Node)((DefaultMutableTreeNode)toTreeNode.getChildAt(i)).getUserObject()).getName();
		            if(name.compareTo(curr_name) < 0)
			        break;
		        }
		        curr_tree_node = new_tree_node = new DefaultMutableTreeNode(new_node);
		        nodeHash.put(new_tree_node, new_node);
		        DefaultTreeModel tree_model = (DefaultTreeModel)curr_tree.getModel();
		        tree_model.insertNodeInto(new_tree_node, toTreeNode, i);
		        //curr_tree.makeVisible(new TreePath(new_tree_node.getPath()));
		        curr_tree.expandPath(new TreePath(new_tree_node.getPath()));
		        curr_tree.treeDidChange();
		        return new_node;
	        }
	    }
	    catch(Exception e) {
	        JOptionPane.showMessageDialog(frame, e.getMessage(),
		    "Error adding Node", JOptionPane.WARNING_MESSAGE);
	        return null;
	    }
	return new_node;
    }


	public Node addDevice(String name, String type)
	{
	    return addDevice(name, type, curr_node);
	}

	public static DefaultMutableTreeNode getCurrTreeNode() {return curr_tree_node;}
	public static void setCurrTreeNode(DefaultMutableTreeNode node){curr_tree_node = node;}
    public static Node addDevice(String name, String type, Node toNode)
    {
	    DefaultMutableTreeNode new_tree_node = null;
	    if(name == null || name.length() == 0 || name.length() > 12)
	    {
	        JOptionPane.showMessageDialog(frame, "Name length must range between 1 and 12 characters",
		        "Error adding Node", JOptionPane.WARNING_MESSAGE);
	        return null;
	    }
	    if(type == null || type.length() == 0)
	    {
	        JOptionPane.showMessageDialog(frame, "Missing device type",
		        "Error adding Node", JOptionPane.WARNING_MESSAGE);
	        return null;
	    }
	    Node new_node = null;
	    try {
	        new_node = toNode.addDevice(name, type);
	        int num_children = curr_tree_node.getChildCount();
	        int i;
	        if(num_children > 0)
	        {
		        String curr_name;
		        for(i = 0; i < num_children; i++)
		        {
		            curr_name = ((Node)((DefaultMutableTreeNode)curr_tree_node.getChildAt(i)).getUserObject()).getName();
		            if(name.compareTo(curr_name) < 0)
			        break;
		        }
		        new_tree_node = new DefaultMutableTreeNode(new_node);
		        nodeHash.put(new_tree_node, new_node);
		        DefaultTreeModel tree_model = (DefaultTreeModel)curr_tree.getModel();
		        tree_model.insertNodeInto(new_tree_node, curr_tree_node, i);
		        curr_tree.makeVisible(new TreePath(new_tree_node.getPath()));
		        return new_node;
	        }

	    }
	    catch(Throwable e) {
	        JOptionPane.showMessageDialog(frame, "Add routine for the selected device cannot be activated:\n" + e.getMessage(),
		    "Error adding Device", JOptionPane.WARNING_MESSAGE);
	        return null;
	    }
	return new_node;
    }


	void deleteNode()
	{
	    deleteNode(curr_node);
	    curr_node = null;
	}




    static void deleteNode(Node delNode)
    {
	    if(delNode == null) return;
	    Node del_node = delNode;
	    int n_children = del_node.startDelete();
	    String msg = "You are about to delete node " + del_node.getName().trim();
	    if(n_children > 0)
	        msg += " which has " + n_children + " descendents.\n Please confirm";
	    else
	        msg += "\n Please confirm";
	    int n = JOptionPane.showConfirmDialog(frame, msg, "Delete node(s)", JOptionPane.YES_NO_OPTION);
	    if(n == JOptionPane.YES_OPTION)
	    {
	        del_node.executeDelete();
	        DefaultTreeModel tree_model = (DefaultTreeModel)curr_tree.getModel();
	        tree_model.removeNodeFromParent(curr_tree_node);
	    }
    }


    public void modifyTags()
    {
	if(curr_node == null) return;
	try {
	    tags = curr_node.getTags();
	} catch(Exception e)
	{
	    System.out.println("Error getting tags: "+e.getMessage());
	    tags = new String[0];
	}
	curr_taglist_model = new DefaultListModel();
	for(int i = 0; i < tags.length; i++)
	{
	    curr_taglist_model.addElement(tags[i]);
	}
	if (modify_tags_dialog == null)
	{
	    modify_tags_dialog = new JDialog(frame);
	    JPanel jp = new JPanel();
	    jp.setLayout(new BorderLayout());
	    JPanel jp1 = new JPanel();
	    jp1.setLayout(new BorderLayout());
	    modify_tags_list = new JList();
	    modify_tags_list.addListSelectionListener(new ListSelectionListener() {
		public void valueChanged(ListSelectionEvent e)
		{
		    int idx = modify_tags_list.getSelectedIndex();
		    if(idx != -1)
			curr_tag_selection.setText((String)curr_taglist_model.getElementAt(idx));
		}});
	    JScrollPane scroll_list = new JScrollPane(modify_tags_list);
	    jp1.add(new JLabel("Tag List:"), "North");
	    jp1.add(scroll_list,"Center");
	    JPanel jp2 = new JPanel();
	    jp2.setLayout(new GridLayout(2,1));
	    JButton add_tag = new JButton("Add Tag");
	    add_tag.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e)
		{
		    String curr_tag = curr_tag_selection.getText().toUpperCase();
		    if(curr_tag == null || curr_tag.length() == 0)
			return;
		    for(int i = 0; i < curr_taglist_model.getSize(); i++)
			if(curr_tag.equals((String)curr_taglist_model.getElementAt(i)))
			    return;
		    curr_taglist_model.addElement(curr_tag);
		}});
	    jp2.add(add_tag);
	    JButton remove_tag = new JButton("Remove Tag");
	    remove_tag.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e)
		{
		    int idx;
		    if((idx = modify_tags_list.getSelectedIndex()) != -1)
		    {
			curr_taglist_model.removeElementAt(idx);
		    }
		}});
	    jp2.add(remove_tag);
	    JPanel jp4 = new JPanel();
	    jp4.add(jp2);
	    jp1.add(jp4, "East");
	    curr_tag_selection = new JTextField(30);
	    JPanel jp5 = new JPanel();
	    jp5.add(new JLabel("Current Selection: "));
	    jp5.add(curr_tag_selection);
    	    jp1.add(jp5, "South");
	    jp.add(jp1,"North");
	    JPanel jp3 = new JPanel();
	    JButton ok_b = new JButton("Ok");
	    ok_b.addActionListener(new ActionListener() {
		    public void actionPerformed(ActionEvent e)
		    {
		        String []out_tags = new String[curr_taglist_model.getSize()];
		        for(int i = 0; i < curr_taglist_model.getSize(); i++)
		        {
			        out_tags[i] = (String)curr_taglist_model.getElementAt(i);
		        }
		        try {
			        curr_node.setTags(out_tags);
		        } catch(Exception exc)
		        {
			        JOptionPane.showMessageDialog(frame, exc.getMessage(),
			            "Error adding tags", JOptionPane.WARNING_MESSAGE);
		        }
		        modify_tags_dialog.setVisible(false);
		}});
	    jp3.add(ok_b);
	    JButton apply_b = new JButton("Apply");
	    apply_b.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e)
		{
            addTag();
		}});
	    jp3.add(apply_b);
	    JButton reset_b = new JButton("Reset");
	    reset_b.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    curr_taglist_model = new DefaultListModel();
		    for(int i = 0; i < tags.length; i++)
			curr_taglist_model.addElement(tags[i]);
		    modify_tags_list.setModel(curr_taglist_model);
		}});
	    jp3.add(reset_b);
	    JButton cancel_b = new JButton("Cancel");
	    cancel_b.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    modify_tags_dialog.setVisible(false); }});
	    jp3.add(cancel_b);
	    jp.add(jp3, "South");
	    modify_tags_dialog.getContentPane().add(jp);
	    modify_tags_dialog.addKeyListener(new KeyAdapter() {
	        public void keyTyped(KeyEvent e)
	        {
	            if(e.getKeyCode() == KeyEvent.VK_ENTER)
	                addTag();
	        }
	    });
	    modify_tags_dialog.pack();
	    modify_tags_dialog.setVisible(true);
	}
	modify_tags_dialog.setTitle("Modify tags of " + curr_node.getFullPath());
	modify_tags_list.setModel(curr_taglist_model);
	curr_tag_selection.setText("");
	modify_tags_dialog.setLocation(curr_origin);
	modify_tags_dialog.setVisible(true);
    }

	public void addTag()
	{
		String []out_tags = new String[curr_taglist_model.getSize()];
		for(int i = 0; i < curr_taglist_model.getSize(); i++)
		{
			out_tags[i] = (String)curr_taglist_model.getElementAt(i);
		}
		try {
			curr_node.setTags(out_tags);
		} catch(Exception exc)
		{
			JOptionPane.showMessageDialog(frame, exc.getMessage(),
			    "Error adding tags", JOptionPane.WARNING_MESSAGE);
		}
		modify_tags_dialog.setVisible(false);
	}

    void renameNode()
    {
	if(curr_node == null) return;
	if(rename_dialog == null)
	{
	    rename_dialog = new JDialog();
	    JPanel jp = new JPanel();
	    jp.setLayout(new BorderLayout());
	    JPanel jp1 = new JPanel();
	    jp1.add(new JLabel("New Name: "));
	    jp1.add(new_node_name = new JTextField(12));
	    jp.add(jp1, "North");
	    jp1 = new JPanel();
	    JButton ok_b = new JButton("Ok");
	    ok_b.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
            rename();
		}});
	    jp1.add(ok_b);
	    JButton cancel_b = new JButton("Cancel");
	    cancel_b.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    rename_dialog.setVisible(false);
		}});
	    jp1.add(cancel_b);
	    jp.add(jp1, "South");
	    rename_dialog.getContentPane().add(jp);
	    rename_dialog.addKeyListener(new KeyAdapter() {
	        public void keyTyped(KeyEvent e)
	        {
	            if(e.getKeyCode() == KeyEvent.VK_ENTER)
	                rename();
	        }
	    });
	    rename_dialog.pack();
	    rename_dialog.setVisible(true);
	}
	rename_dialog.setTitle("Rename node " + curr_node.getFullPath());
	rename_dialog.setLocation(curr_origin);
	new_node_name.setText("");
	rename_dialog.setVisible(true);
    }


    public void rename()
    {
		String name = new_node_name.getText();
		if(name == null || name.length() == 0)
		return;
		try {
		curr_node.rename(name);
		}catch(Exception exc) {
		    	JOptionPane.showMessageDialog(frame, exc.getMessage(),
		"Error renaming Node", JOptionPane.WARNING_MESSAGE);
			return;
		}
		curr_tree.treeDidChange();
		rename_dialog.setVisible(false);

    }

    public void reportChange()
    {
	if(curr_tree != null)
	    curr_tree.treeDidChange();
    }
 //temporary: to overcome java's bugs for inner classes
    public void actionPerformed(ActionEvent e)
    {
	Object jb = e.getSource();
	if(jb == (Object)open_b) open();
	if(jb == (Object)close_b) close();
	if(jb == (Object)quit_b) quit();
	if(jb == (Object)ok_cb) open_ok();
	if(jb == (Object)add_action_b) addNode(NodeInfo.USAGE_ACTION);
	if(jb == (Object)add_dispatch_b) addNode(NodeInfo.USAGE_DISPATCH);
	if(jb == (Object)add_numeric_b) addNode(NodeInfo.USAGE_NUMERIC);
	if(jb == (Object)add_signal_b) addNode(NodeInfo.USAGE_SIGNAL);
	if(jb == (Object)add_task_b) addNode(NodeInfo.USAGE_TASK);
	if(jb == (Object)add_text_b) addNode(NodeInfo.USAGE_TEXT);
	if(jb == (Object)add_window_b) addNode(NodeInfo.USAGE_WINDOW);
	if(jb == (Object)add_axis_b) addNode(NodeInfo.USAGE_AXIS);
	if(jb == (Object)add_node_ok) addNode();
	if(jb == (Object)add_child_b) addNode(NodeInfo.USAGE_STRUCTURE);
	if(jb == (Object)add_subtree_b) addSubtree();
	if(jb == (Object)add_device_b) addDevice();
	if(jb == (Object)delete_node_b) deleteNode();
	if(jb == (Object)modify_tags_b) modifyTags();
	if(jb == (Object)rename_node_b) renameNode();
    }

    public void addNode()
	{
	    Node newNode = addNode(add_node_usage, add_node_name.getText().toUpperCase());
	    if(!add_node_tag.getText().trim().equals(""))
        {
            try {
	            newNode.setTags(new String[]{add_node_tag.getText().trim().toUpperCase()});
	        }catch(Exception exc){System.err.println("Error adding tag: " + exc);}
	    }
	    add_node_dialog.setVisible(false);
	}


    public void keyPressed(KeyEvent ke) {
	if(ke.getKeyText(ke.getKeyCode()).equals("Enter"))
	    open_ok();
    }
    public void keyReleased(KeyEvent e){}
    public void keyTyped(KeyEvent e){}


   class DialogSet
    {
	Vector dialogs = new Vector();

	TreeDialog getDialog(Class ed_class, Node node)
	{
	    int idx;
	    TreeDialog curr_dialog = null;
	    NodeEditor curr_editor;

	    for(idx = 0; idx < dialogs.size(); idx++)
	    {
		curr_dialog = (TreeDialog)dialogs.elementAt(idx);
		if(!curr_dialog.inUse())
		    break;
	    }
	    if(idx == dialogs.size())
	    {
		try {
		    curr_editor = (NodeEditor)((PropertyEditor)ed_class.newInstance()).getCustomEditor();
		    curr_dialog = new TreeDialog(curr_editor);
		    curr_editor.setFrame(curr_dialog);
		    dialogs.addElement(curr_dialog);
		}catch(Exception exc) {System.out.println("Error creating node editor "+exc);}
	    }
	    curr_dialog.setUsed(true);
	    curr_dialog.getEditor().setNode(node);
	    return curr_dialog;
	}
    }
    public void dataChanged(DataChangeEvent e)
    {
        reportChange();
    }
    public Point getMousePosition()
    {
        return curr_origin;
    }
    public boolean isEditable() {return is_editable; }
}

//package jTraverser;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;

public class jTraverser extends JFrame implements ActionListener
{
    static String exp_name, shot_name;
    static boolean editable, readonly, model;
    static Tree tree;
    static JLabel status = new JLabel("jTaverser started");
    JMenu file_m, edit_m, data_m, customize_m;
    JMenuItem open, close, quit;
    JMenuItem add_action_b, add_dispatch_b, add_numeric_b, add_signal_b, add_task_b, add_text_b,
	add_window_b, add_axis_b, add_device_b, add_child_b, add_subtree_b, delete_node_b, modify_tags_b,
	rename_node_b, turn_on_b, turn_off_b, display_data_b, display_nci_b, display_tags_b, modify_data_b,
	set_default_b, setup_device_b, do_action_b, outline_b, tree_b, copy_b, paste_b;
    TreeDialog display_data_d, modify_data_d, display_nci_d, display_tags_d;
    DisplayData display_data;
    DisplayNci display_nci;
    DisplayTags display_tags;
    ModifyData modify_data;
    /**
     * Constructor.
     */
    public static void main(String args[]) {

    /* the original font is either ugly or defaults to an ugly font if not found 
    if(System.getProperty("os.name").equals("Linux"))
    {
	    UIManager.put("Label.font", new FontUIResource(new Font("FreeSerif", Font.BOLD, 11)));
	    UIManager.put("ComboBox.font", new FontUIResource(new Font("FreeSerif", Font.BOLD, 11)));
	    UIManager.put("Button.font", new FontUIResource(new Font("FreeSerif", Font.BOLD, 11)));
    }*/

    if(args.length >= 3)
		FrameRepository.frame = new jTraverser(args[0], args[1], args[2]);
    else if(args.length == 2)
		FrameRepository.frame = new jTraverser(args[0], args[1], null);
    else if(args.length == 1)
      	FrameRepository.frame = new jTraverser(args[0], null, null);
    else
	    FrameRepository.frame = new jTraverser(null, null, null);

    }


    public static void stdout(String line)
    {
         status.setText(line);
    }
    public static void stderr(String line, Exception exc)
    {
         jTraverser.status.setText("ERROR: "+line+" ("+exc.getMessage()+")");
         System.err.println(line+"\n"+exc);
    }

    public jTraverser(String exp_name, String shot_name, String access)
    {
	this.exp_name = exp_name;
	this.shot_name = shot_name;
	setTitle("jTraverser - no tree open");
	Boolean edit = false;
	Boolean readonly = false;
	if (access != null)
	{
		edit     = (new String(access).equals("-edit"));
		readonly = (new String(access).equals("-readonly"));
	}
	JMenuBar menu_bar = new JMenuBar();
	setJMenuBar(menu_bar);
	JMenu curr_menu = new JMenu("File");
	file_m = curr_menu;
	menu_bar.add(curr_menu);
	open = new JMenuItem("Open");
	curr_menu.add(open);
	open.addActionListener(this);
	close = new JMenuItem("Close");
	curr_menu.add(close);
	close.addActionListener(this);
	quit = new JMenuItem("Quit");
	curr_menu.add(quit);
	quit.addActionListener(this);
	edit_m = curr_menu = new JMenu("Edit");
	menu_bar.add(curr_menu);
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
	curr_menu.add(jm);
	curr_menu.add(add_device_b = new JMenuItem("Add Device"));
	add_device_b.addActionListener(this);
	curr_menu.add(add_child_b = new JMenuItem("Add Child"));
	add_child_b.addActionListener(this);
	curr_menu.add(add_subtree_b = new JMenuItem("Add Subtree"));
	add_subtree_b.addActionListener(this);
	curr_menu.add(delete_node_b = new JMenuItem("Delete Node"));
	delete_node_b.addActionListener(this);
	curr_menu.add(modify_tags_b = new JMenuItem("Modify tags"));
	modify_tags_b.addActionListener(this);
	curr_menu.add(rename_node_b = new JMenuItem("Rename node"));
	rename_node_b.addActionListener(this);
	curr_menu.add(copy_b = new JMenuItem("Copy"));
	copy_b.addActionListener(this);
	curr_menu.add(paste_b = new JMenuItem("Paste"));
	paste_b.addActionListener(this);

	data_m = curr_menu = new JMenu("Data");
	menu_bar.add(curr_menu);
	curr_menu.add(turn_on_b = new JMenuItem("Turn On"));
	turn_on_b.addActionListener(this);
	curr_menu.add(turn_off_b = new JMenuItem("Turn Off"));
	turn_off_b.addActionListener(this);
	curr_menu.add(display_data_b = new JMenuItem("Display Data"));
	display_data_b.addActionListener(this);
    curr_menu.add(display_nci_b = new JMenuItem("Display Nci"));
    display_nci_b.addActionListener(this);
    curr_menu.add(display_tags_b = new JMenuItem("Display Tags"));
    display_tags_b.addActionListener(this);
	curr_menu.add(modify_data_b = new JMenuItem("Modify Data"));
	modify_data_b.addActionListener(this);
	curr_menu.add(set_default_b = new JMenuItem("Set Default"));
	set_default_b.addActionListener(this);
	curr_menu.add(setup_device_b = new JMenuItem("Setup Device"));
	setup_device_b.addActionListener(this);
	curr_menu.add(do_action_b = new JMenuItem("Do Action"));
	do_action_b.addActionListener(this);
	curr_menu = new JMenu("Customize");
	menu_bar.add(curr_menu);
	jm = new JMenu("Display Mode");
	jm.add(outline_b = new JMenuItem("Outline"));
	outline_b.addActionListener(this);
	jm.add(tree_b = new JMenuItem("Tree"));
	tree_b.addActionListener(this);
	curr_menu.add(jm);
	tree = new Tree(this);
	if(exp_name != null)
	    tree.open(exp_name.toUpperCase(), (shot_name == null)?-1:Integer.parseInt(shot_name), edit, readonly, false);
	getContentPane().add(tree);
    getContentPane().add(status, BorderLayout.PAGE_END);
	addWindowListener(new WindowAdapter() {
	    public void windowClosing(WindowEvent e)
	    {
	        System.exit(0);
	    }});
	pack();
	show();
    }

    public static String getExperimentName(){return exp_name;}
    public static boolean isEditable(){return editable;}
    public Point dialogLocation()
    {
        return new Point(getLocation().x+32,getLocation().y+32);
    }

public void actionPerformed(ActionEvent e)
{
    Object source = e.getSource();
    if(source == (Object)open) tree.open();
    if(source == (Object)close) tree.close();
    if(source == (Object)quit) tree.quit();
    if(source == (Object)tree_b)tree.setAngled(true);
    if(source == (Object)outline_b) tree.setAngled(false);
    if(source == (Object)add_action_b) tree.addNode(NodeInfo.USAGE_ACTION);
    if(source == (Object)add_dispatch_b) tree.addNode(NodeInfo.USAGE_DISPATCH);
    if(source == (Object)add_numeric_b) tree.addNode(NodeInfo.USAGE_NUMERIC);
    if(source == (Object)add_signal_b) tree.addNode(NodeInfo.USAGE_SIGNAL);
    if(source == (Object)add_task_b)tree.addNode(NodeInfo.USAGE_TASK);
    if(source == (Object)add_text_b) tree.addNode(NodeInfo.USAGE_TEXT);
    if(source == (Object)add_child_b) tree.addNode(NodeInfo.USAGE_STRUCTURE);
    if(source == (Object)add_window_b) tree.addNode(NodeInfo.USAGE_WINDOW);
    if(source == (Object)add_axis_b)tree.addNode(NodeInfo.USAGE_AXIS);
    if(source == (Object)add_subtree_b) tree.addSubtree();
    if(source == (Object)delete_node_b) tree.deleteNode();
    if(source == (Object)modify_tags_b) tree.modifyTags();
    if(source == (Object)rename_node_b) Tree.dialogs.rename.show();
    if(source == (Object)add_device_b) tree.addDevice();
    if(source == (Object)copy_b) TreeNode.copy();
    if(source == (Object)paste_b) TreeNode.paste();

    // Node related
    Node currnode = Tree.getCurrentNode();
	if(currnode == null) return;
    if(source == (Object)turn_on_b)
    {
	    currnode.turnOn();
	    tree.reportChange();
    }
    if(source == (Object)turn_off_b)
    {
	    currnode.turnOff();
	    tree.reportChange();
    }

    if(source == (Object)display_data_b)
    {
	    if(display_data_d == null)
	    {
	        display_data_d = new TreeDialog(display_data = new DisplayData());
	        display_data.setFrame(display_data_d);
	    }
	    display_data.setNode(currnode);
	    display_data_d.pack();
	    display_data_d.setLocation(dialogLocation());
	    display_data_d.setVisible(true);
    }
    if(source == (Object)display_nci_b)
    {
        if(display_nci_d == null)
        {
            display_nci_d = new TreeDialog(display_nci = new DisplayNci());
            display_nci.setFrame(display_nci_d);
        }
        display_nci.setNode(currnode);
        display_nci_d.pack();
        display_nci_d.setLocation(dialogLocation());
        display_nci_d.setVisible(true);
    }
    if(source == (Object)display_tags_b)
    {
        if(display_tags_d == null)
        {
            display_tags_d = new TreeDialog(display_tags = new DisplayTags());
            display_tags.setFrame(display_tags_d);
        }
        display_tags.setNode(currnode);
        display_tags_d.pack();
        display_tags_d.setLocation(dialogLocation());
        display_tags_d.setVisible(true);
    }
    if(source == (Object)modify_data_b)
    {
	    if(modify_data_d == null)
	    {
	        modify_data_d = new TreeDialog(modify_data = new ModifyData());
	        modify_data.setFrame(modify_data_d);
	    }
	    modify_data.setNode(currnode);
	    modify_data_d.pack();
	    modify_data_d.setLocation(dialogLocation());
	    modify_data_d.setVisible(true);
    }
    if(source == (Object)set_default_b)
    {
	    try {
	        currnode.setDefault();
	    }catch(Exception exc) {jTraverser.stderr("Error setting default",exc);}
	    tree.reportChange();
    }
    if(source == (Object)setup_device_b)
        currnode.setupDevice();
}

void reportChange(String exp, int shot, boolean editable, boolean readonly)
{
    jTraverser.model = shot<0;
    jTraverser.editable = editable;
    jTraverser.readonly = readonly;
    jTraverser.exp_name = exp;
    String title;
    if(exp != null)
    {
	    title = "Tree: " + exp;
	    if(editable)
	        title = title + " (edit) ";
	    if(readonly)
	        title = title + " (readonly) ";
	    title = title + " Shot: " + shot;
        jTraverser.stdout(title);
    }
    else
    {
	    title = "no tree open";
	    edit_m.setEnabled(false);
	    data_m.setEnabled(false);
    }
    setTitle("jTraverser - "+title);
    if(exp == null) return;
    data_m.setEnabled(true);
    if(editable)
    {
	edit_m.setEnabled(true);
	modify_data_b.setEnabled(true);
	turn_on_b.setEnabled(true);
	turn_off_b.setEnabled(true);
    }
    if(readonly)
    {
	edit_m.setEnabled(false);
	modify_data_b.setEnabled(false);
	turn_on_b.setEnabled(false);
	turn_off_b.setEnabled(false);
    }
    if(!editable && !readonly)
    {
	edit_m.setEnabled(false);
	modify_data_b.setEnabled(true);
	turn_on_b.setEnabled(true);
	turn_off_b.setEnabled(true);
    }
}

public Component add(Component component)
{
    return getContentPane().add(component);
}
}

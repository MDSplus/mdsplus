//package jTraverser;
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.event.*;
import javax.swing.plaf.*;
import java.util.*;
public class jTraverser extends JFrame implements ActionListener
{
    
    String exp_name, shot_name;
    Tree tree;
    JMenu file_m, edit_m, data_m, customize_m;
    JMenuItem open, close, quit;
    JMenuItem add_action_b, add_dispatch_b, add_numeric_b, add_signal_b, add_task_b, add_text_b,
	add_window_b, add_axis_b, add_device_b, add_child_b, delete_node_b, modify_tags_b,
	rename_node_b, turn_on_b, turn_off_b, display_data_b, display_nci_b, modify_data_b,
	set_default_b, setup_device_b, do_action_b, outline_b, tree_b, copy_b, paste_b;
	
    TreeDialog display_data_d = null, modify_data_d = null, display_nci_d = null;
    DisplayData display_data;
    DisplayNci display_nci;
    ModifyData modify_data;
    /**
     * Constructor.
     */
    public static void main(String args[]) {
	
    if(System.getProperty("os.name").equals("Linux"))
    {
	    UIManager.put("Label.font", new FontUIResource(new Font("LuciduxSans", Font.BOLD, 11)));
	    UIManager.put("ComboBox.font", new FontUIResource(new Font("LuciduxSans", Font.BOLD, 11)));
	    UIManager.put("Button.font", new FontUIResource(new Font("LuciduxSans", Font.BOLD, 11)));
    }

	if(args.length >= 2)
	    FrameRepository.frame = new jTraverser(args[0], args[1]);
	else
	    FrameRepository.frame = new jTraverser(null, null);
	
    }
    
    
    public jTraverser(String exp_name, String shot_name)
    {
	this.exp_name = exp_name;
	this.shot_name = shot_name;
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
	    tree.open(exp_name.toUpperCase(), Integer.parseInt(shot_name), false, false);
	else
	    setTitle("jTraverser - no tree open");
	getContentPane().add(tree);
	
	
	addWindowListener(new WindowAdapter() {
	    public void windowClosing(WindowEvent e)
	    {
	        System.exit(0);
	    }});
	    
	addKeyListener(new KeyAdapter() {
	    public void keyTyped(KeyEvent e)
	    {
	        if(!tree.isEditable())
	            return;
	        if((e.getModifiers() & Event.ALT_MASK) != 0)
	        {
	            if(e.getKeyChar() == 'c')  
	                TreeNode.copy();
	            if(e.getKeyChar() == 'v')  
	                TreeNode.paste();
	        }
	        else if(e.getKeyChar() == KeyEvent.VK_DELETE || e.getKeyChar() == KeyEvent.VK_BACK_SPACE)
	            TreeNode.delete();
	    }
	});
	        
	      
	        
	    
	    
	pack();
	show(); 
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
    if(source == (Object)add_window_b) tree.addNode(NodeInfo.USAGE_WINDOW);
    if(source == (Object)add_axis_b)tree.addNode(NodeInfo.USAGE_AXIS);
    if(source == (Object)add_child_b) tree.addNode(NodeInfo.USAGE_STRUCTURE);
    if(source == (Object)delete_node_b) tree.deleteNode();
    if(source == (Object)modify_tags_b) tree.modifyTags();
    if(source == (Object)rename_node_b) tree.renameNode();
    if(source == (Object)add_device_b) tree.addDevice();
    if(source == (Object)copy_b) TreeNode.copy();
    if(source == (Object)paste_b) TreeNode.paste();
    
    if(source == (Object)turn_on_b) 
    {
	    Node curr_node = tree.getCurrentNode();
	    if(curr_node == null) return;
	    curr_node.turnOn(); 
	    tree.reportChange();
    }
    if(source == (Object)turn_off_b) 
    {
	    Node curr_node = tree.getCurrentNode();
	    if(curr_node == null) return;
	    curr_node.turnOff(); 
	    tree.reportChange();
    }
    if(source == (Object)display_data_b)
    {
	    if(tree.getCurrentNode() == null) return;
	    if(display_data_d == null) 
	    {
	        display_data_d = new TreeDialog(display_data = new DisplayData());
	        display_data.setFrame(display_data_d);
	    }
	    display_data.setNode(tree.getCurrentNode());
	    display_data_d.pack();
	    //display_data_d.setLocation(new Point(50,50));
	    display_data_d.show();
    }    
    if(source == (Object)display_nci_b)
    {
	    if(tree.getCurrentNode() == null) return;
	    if(display_nci_d == null) 
	    {
	        display_nci_d = new TreeDialog(display_nci = new DisplayNci());
	        display_nci.setFrame(display_nci_d);
	    }
	    display_nci.setNode(tree.getCurrentNode());
	    display_nci_d.pack();
	    display_nci_d.setLocation(new Point(50,50));
	    display_nci_d.show();
    }    
    if(source == (Object)modify_data_b)
    {
	    if(tree.getCurrentNode() == null) return;
	    if(modify_data_d == null) 
	    {
	        modify_data_d = new TreeDialog(modify_data = new ModifyData());
	        modify_data.setFrame(modify_data_d);
	    }
	    modify_data.setNode(tree.getCurrentNode());
	    modify_data_d.pack();
	    modify_data_d.setLocation(new Point(50,50));
	    modify_data_d.show();
    }
    if(source == (Object)set_default_b)
    { 
	    if(tree.getCurrentNode() == null) return;
	    try {
	        tree.getCurrentNode().setDefault();
	    }catch(Exception exc) {System.out.println("Error setting default "+exc.getMessage());}
	    tree.reportChange();
    }
    if(source == (Object)setup_device_b)
    {
	    if(tree.getCurrentNode() == null) return;
	    tree.getCurrentNode().setupDevice();
	}
}

void reportChange(String exp, int shot, boolean editable, boolean readonly)
{
    
    String title;
    if(exp != null)
    {
	title = "jTraverser - Tree: " + exp;
	if(editable)
	    title = title + " (edit) ";
	if(readonly)
	    title = title + " (readonly) ";
	title = title + " Shot: " + shot;
    }
    else
    {
	title = "jTraverser - no tree open";
	edit_m.setEnabled(false);
	data_m.setEnabled(false);
    }
    setTitle(title);
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

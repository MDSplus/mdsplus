//package jTraverser;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

/*
public class TreeNode extends JPanel
{
    Node node;
    JLabel label;
    static Node selected;
    static Font plain_f, bold_f;
    boolean tick1_expired = false;
    boolean tick2_expired = false;
    static {
	plain_f = new Font("Serif", Font.PLAIN, 12);
	bold_f = new Font("Serif" ,Font.BOLD, 12);
    }

    public static void setSelectedNode(Node sel)
    {
	selected = sel;
    }
    public TreeNode(Node _node, String name, Icon icon)
    {
	    label = new JLabel(name+"                ", icon, JLabel.LEFT);
	    node = _node;
	    if(node.isOn())
	        label.setFont(bold_f);
	    else
	        label.setFont(plain_f);
	    label.setForeground(Color.black);
	    if(node.isDefault())
	        //setText(node.getName().trim() + "                                   ");
	        //setForeground(Color.red);
	        label.setBorder(BorderFactory.createLineBorder(Color.black, 1));
	    else
	        label.setBorder(BorderFactory.createLineBorder(Color.white, 1));
	        //setText(node.getName().trim() + "                                   ");
	        //setForeground(Color.black);
	    setLayout(new BorderLayout());
	    add(label, "Center");
	    setBackground(Color.white);
	    add(new JTextField(12), "East");
	    addMouseListener(new MouseAdapter()
	    {
	        public void mouseClicked(MouseEvent e)
	        {
	            System.out.println("MOUSE");
	            
	            if(tick1_expired && !tick2_expired)
	                System.out.println("BINGO!!!");
	            if(tick1_expired && tick1_expired)
	            {
	                tick1_expired = false;
	                tick2_expired = false;
	                Timer timer1 = new Timer(300, new ActionListener()
	                {
	                    public void actionPerformed(ActionEvent e)
	                    {
	                        tick1_expired = true;
	                    }
	                });
	                Timer timer2 = new Timer(1000, new ActionListener()
	                {
	                    public void actionPerformed(ActionEvent e)
	                    {
	                        tick2_expired = true;
	                    }
	                });
	                timer1.setRepeats(false);
	                timer2.setRepeats(false);
	                timer1.start();
	                timer2.start();
	            }
	        }
	    });
    }
    
    public void paint(Graphics g)
    {
	    label.setText(node.getName());
	    if(node.isDefault())
	        label.setForeground(Color.red);
	    else
	        label.setForeground(Color.black);
        
    	    if(node.isOn())
	            label.setFont(bold_f);
	    else
	            label.setFont(plain_f);
    	    
	    if(selected == node)
	        label.setBorder(BorderFactory.createLineBorder(Color.black, 1));
	    else
	        label.setBorder(BorderFactory.createLineBorder(Color.white, 1));
	    super.paint(g);
    }
}

/* VECCHIA VERSIONE
*/
public class TreeNode extends JLabel
{
    Node node;
    static Node selected;
    static Font plain_f, bold_f;
    static {
	plain_f = new Font("Serif", Font.PLAIN, 12);
	bold_f = new Font("Serif" ,Font.BOLD, 12);
    }

    public static void setSelectedNode(Node sel)
    {
	selected = sel;
    }
    public TreeNode(Node _node, String name, Icon icon)
    {
	super(name+"                ", icon, JLabel.LEFT);
	node = _node;
	if(node.isOn())
	    setFont(bold_f);
	else
	    setFont(plain_f);
	setForeground(Color.black);
	if(node.isDefault())
	    //setText(node.getName().trim() + "                                   ");
	    //setForeground(Color.red);
	    setBorder(BorderFactory.createLineBorder(Color.black, 1));
	else
	    setBorder(BorderFactory.createLineBorder(Color.white, 1));
	    //setText(node.getName().trim() + "                                   ");
	    //setForeground(Color.black);
    }
    
    public void paint(Graphics g)
    {
	setText(node.getName());
	if(node.isDefault())
	    setForeground(Color.red);
	else
	    setForeground(Color.black);
    
    	if(node.isOn())
	    setFont(bold_f);
	else
	    setFont(plain_f);
	    
	if(selected == node)
	    setBorder(BorderFactory.createLineBorder(Color.black, 1));
	else
	    setBorder(BorderFactory.createLineBorder(Color.white, 1));
	super.paint(g);
    }
}

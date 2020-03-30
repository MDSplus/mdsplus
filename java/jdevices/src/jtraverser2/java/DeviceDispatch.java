import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.ComboBoxEditor;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.border.LineBorder;
import javax.xml.crypto.Data;

import mds.MdsException;
import mds.data.TREE.NodeInfo;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_a.Int32Array;
import mds.data.descriptor_a.NidArray;
import mds.data.descriptor_r.Range;
import mds.data.descriptor_s.NODE;

public class DeviceDispatch extends DeviceComponent
{
    DeviceDispatchField dispatch_fields[], active_field;
    Descriptor<?> actions[];
    int i, j, num_actions;
    JDialog dialog = null;
    JComboBox menu;
    protected boolean initializing = false;
    public DeviceDispatch()
    {
	menu = new JComboBox();
	menu.setEditor(new DispatchComboEditor());
	menu.setEditable(true);
	menu.setBorder(new LineBorder(Color.black, 1));
	add(menu);
    }

    protected void initializeData(Descriptor<?> data, boolean is_on)
//data and is_on arguments are meaningless in this context
//The class will search actions stored in the device
//and create and manage their dispatch configurations
    {
	initializing = true;
	NodeInfo nodeInfo;
	if(subtree == null) return;
	try {
	    nodeInfo = subtree.getNodeInfo(nidData);
	}catch(Exception e)
	{
	    System.out.println("Cannot read device NCI: "+e);
	    return;
	}
	int currNid = baseNidData.getNidNumber();
	int num_components;
	NodeInfo nodeInfos[];
	try {
		num_components = subtree.getNciNumberOfElts(currNid);
		nodeInfos = NodeInfo.getNodeInfos(new NidArray(subtree, (Int32Array) new Range(currNid, num_components+1).getDataA()), subtree.getMds(), subtree);
	} catch (MdsException e1) {
		// TODO Auto-generated catch block
		e1.printStackTrace();
		return;
	}	
	for(i = num_actions = 0; i < num_components; i++)
	    if(nodeInfos[i].usage == NODE.USAGE_ACTION)
	        num_actions++;
	actions = new Descriptor<?>[num_actions];
	dispatch_fields = new DeviceDispatchField[num_actions];
	currNid = nidData.getNidNumber();
	for(i = j = num_actions = 0; i < num_components; i++)
	{
	    if(nodeInfos[i].usage == NODE.USAGE_ACTION)
	    {
	        try
	        {
	            actions[j] = subtree.getRecord(currNid);
	        }catch(Exception e)
	        {
	            System.out.println("Cannot read device actions: "+e);
	            return;
	        }
	        dispatch_fields[j] = new DeviceDispatchField();
	        dispatch_fields[j].setSubtree(subtree);
	        dispatch_fields[j].setOffsetNid(i);
	        dispatch_fields[j].configure(nidData.getNidNumber());
	        j++;
	    }
	    currNid ++;
	}
	for(i = 0; i < num_components; i++)
	{
	    if(nodeInfos[i].usage == NODE.USAGE_ACTION)
	    {
	        String name = nodeInfos[i].node_name;
	        menu.addItem(name);
	    }
	}
	menu.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        int idx = menu.getSelectedIndex();
	        if(idx < 0 || idx >= dispatch_fields.length)
	            return;
	        activateForm(dispatch_fields[menu.getSelectedIndex()], (String)menu.getSelectedItem());
	    }
	});
	initializing = false;
    }
     protected void activateForm(DeviceDispatchField field, String name)
     {

	if(dialog == null)
	{
	    dialog = new JDialog(JOptionPane.getRootFrame());
	    dialog.getContentPane().setLayout(new BorderLayout());
	    dialog.getContentPane().add(field, "Center");
	    JPanel jp = new JPanel();
	    JButton button = new JButton("Done");
	    button.addActionListener(new ActionListener() {
	        public void actionPerformed(ActionEvent e)
	        {
	            dialog.dispose();
	        }
	    });
	    jp.add(button);
	    dialog.getContentPane().add(jp, "South");
	    active_field = field;
	}
	else
	{
	    dialog.getContentPane().remove(active_field);
	    dialog.getContentPane().add(field, "Center");
	    active_field = field;
	}
	dialog.setTitle("Dispatch info for " + name);
	dialog.pack();
	dialog.repaint();
	dialog.setLocation(JOptionPane.getRootFrame().getLocationOnScreen());
	dialog.setVisible(true);

     }
    protected void displayData(Descriptor<?> data, boolean is_on){}
    protected Descriptor<?> getData(){return null; }
    protected boolean getState(){return true;}
    public void apply()throws Exception
    {
	if(dispatch_fields == null) return;
	for(int i = 0; i < dispatch_fields.length; i++)
	    dispatch_fields[i].apply();
    }
    public void reset()
    {
	if(dispatch_fields == null) return;
	for(int i = 0; i < dispatch_fields.length; i++)
	    dispatch_fields[i].reset();
    }


    public void apply(int currBaseNid){}

    class DispatchComboEditor implements ComboBoxEditor
    {
	JLabel label = new  JLabel("  Dispatch");
	int idx; String name;
	public void addActionListener(ActionListener l){}
	public void removeActionListener(ActionListener l){}
	public Component getEditorComponent() {return label;}
	public Object getItem() {return label; }
	public void selectAll() {}
	public void setItem(Object obj){}
    }
}



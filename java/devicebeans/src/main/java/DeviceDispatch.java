import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;

public class DeviceDispatch extends DeviceComponent
{
    DeviceDispatchField dispatch_fields[], active_field;
    String actions[];
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

    protected void initializeData(String data, boolean is_on)
//data and is_on arguments are meaningless in this context
//The class will search actions stored in the device
//and create and manage their dispatch configurations
    {
	initializing = true;
	if(subtree == null) return;
	int currNid = baseNidData;
	int num_components;
        try {
            num_components = subtree.getNumConglomerateNids(nidData);
        }catch(Exception exc){num_components = 0; }
	for(i = num_actions = 0; i < num_components; i++)
	{
	    if(subtree.getUsage(currNid).equals("ACTION"))
	        num_actions++;
	    currNid++;
	}
	actions = new String[num_actions];
	dispatch_fields = new DeviceDispatchField[num_actions];
	currNid = nidData;
	for(i = j = num_actions = 0; i < num_components; i++)
	{
	    if(subtree.getUsage(currNid).equals("ACTION"))
	    {
	        try
	        {
	            actions[j] = subtree.getDataExpr(currNid);
	        }catch(Exception e)
	        {
	            System.out.println("Cannot read device actions: "+e);
	            return;
	        }
	        dispatch_fields[j] = new DeviceDispatchField();
	        dispatch_fields[j].setSubtree(subtree);
	        dispatch_fields[j].setOffsetNid(i);
	        dispatch_fields[j].configure(nidData);
	        j++;
	    }
	    currNid++;
	}
        currNid = baseNid;
	for(i = 0; i < num_components; i++)
	{
	    if(subtree.getUsage(currNid).equals("ACTION"))
	    {
                try {
                    String name = subtree.getNodeName(currNid);
                    menu.addItem(name);
                }catch(Exception exc){}
	    }
            currNid++;
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
//	    dialog = new JDialog(FrameRepository.frame);
	    dialog = new JDialog(master);
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
	dialog.setLocation(master.getLocationOnScreen());
	dialog.setVisible(true);

     }
    protected void displayData(String data, boolean is_on){}
    protected String getData(){return null; }
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



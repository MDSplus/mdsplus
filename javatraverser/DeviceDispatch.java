import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;

public class DeviceDispatch extends DeviceComponent
{
    DeviceDispatchField dispatch_fields[], active_field;
    Data actions[];
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
    
    protected void initializeData(Data data, boolean is_on)
//data and is_on arguments are meaningless in this context
//The class will search actions stored in the device
//and create and manage their dispatch configurations
    {    
        initializing = true;
        NodeInfo nodeInfo;
        if(subtree == null) return;
        try {
            nodeInfo = subtree.getInfo(nidData);
        }catch(DatabaseException e)
        {
            System.out.println("Cannot read device NCI");
            return;
        }
        NidData currNid = new NidData(nidData.getInt());
        int num_components = nodeInfo.getConglomerateNids();
        NodeInfo nodeInfos[] = new NodeInfo[num_components];
        for(i = num_actions = 0; i < num_components; i++)
        {
            currNid.incrementNid();
            try {
                nodeInfos[i] = subtree.getInfo(currNid);
            }catch(DatabaseException e)
            {
                System.out.println("Cannot read device NCI");
                return;
            }
            if(nodeInfos[i].getUsage() == NodeInfo.USAGE_ACTION)
                num_actions++;
        }
        actions = new Data[num_actions];
        dispatch_fields = new DeviceDispatchField[num_actions];
        currNid = new NidData(nidData.getInt());
        for(i = j = num_actions = 0; i < num_components; i++)
        {
            currNid.incrementNid();
            if(nodeInfos[i].getUsage() == NodeInfo.USAGE_ACTION)
            {
                try
                {
                    actions[j] = subtree.getData(currNid);
                }catch(DatabaseException e)
                {
                    System.out.println("Cannot read device actions");
                    return;
                }
                dispatch_fields[j] = new DeviceDispatchField();
                dispatch_fields[j].setSubtree(subtree);
                dispatch_fields[j].setOffsetNid(i+1);
                dispatch_fields[j].configure(nidData.getInt());
                j++;
            }
        }
        for(i = 0; i < num_components; i++)
        {
            if(nodeInfos[i].getUsage() == NodeInfo.USAGE_ACTION)
            {
                String name = nodeInfos[i].getName();
                menu.addItem(name);
            }
        }
        initializing = false;
    }
     protected void activateForm(DeviceDispatchField field, String name)
     {
        
        if(dialog == null)
        {
            dialog = new JDialog(FrameRepository.frame);
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
        dialog.setLocation(FrameRepository.frame.getLocationOnScreen());
        dialog.show();
        
     }
    protected void displayData(Data data, boolean is_on){}
    protected Data getData(){return null; }
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

    class DispatchComboEditor implements ComboBoxEditor
    {
        JLabel label = new  JLabel("  Dispatch");
        int idx; String name;
        public void addActionListener(ActionListener l){}
        public void removeActionListener(ActionListener l){}
        public Component getEditorComponent() {return label;}
        public Object getItem() {return label; }
        public void selectAll() {}
        public void setItem(Object obj)
        {
            if(initializing || !(obj instanceof String)) return;
            if(dispatch_fields == null) return;
            name = (String) obj;
            idx = menu.getSelectedIndex();
            if(idx >= 0) 
            {
                //Workaround for swing wrong timing
                Timer t = new javax.swing.Timer(20, new ActionListener() {
                    public void actionPerformed(ActionEvent e)
                    {
                        activateForm(dispatch_fields[idx], name);
                    }});
                t.setRepeats(false);
                t.start();
            }
        }
    }

}

    
    
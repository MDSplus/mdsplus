import javax.swing.*;
import java.awt.*;

public abstract class DeviceComponent extends JPanel
{
    RemoteTree subtree;
    
    public int mode = DATA;
    public static final int DATA = 0, STATE = 1, DISPATCH = 2;
    public int baseNid = 0, offsetNid = 0;
    protected Data curr_data, init_data;
    protected boolean curr_on, init_on;
    protected NidData nidData;
    protected String identifier;
    private boolean is_initialized = false;
    
    void setSubtree(RemoteTree subtree) {this.subtree = subtree; }
    RemoteTree getSubtree(){return subtree; }
    public void setBaseNid(int nid) {baseNid = nid; }
    public int getBaseNid() {return baseNid; }
    public void setOffsetNid(int nid) {offsetNid = nid; }
    public int getOffsetNid() {return offsetNid; }
    public void setIdentifier(String identifier)
    {
        this.identifier = identifier;
    }
    public String getIdentifier() {return identifier; }
    
    public void configure(int baseNid) 
    {
        this.baseNid = baseNid;
        nidData = new NidData(baseNid+offsetNid);
        if(mode == DATA)
        {
            try {
                init_data = curr_data = subtree.getData(nidData, Tree.context);
            }catch(Exception e) {init_data = curr_data = null;}
        }
        else init_data = null;
        //if(mode != DISPATCH)
        {
            try {
                init_on = curr_on = subtree.isOn(nidData, Tree.context); 
            } catch(Exception e) 
            {
                System.out.println("Error configuring device: " + e);
            }
        }
        if(!is_initialized)
        {
            initializeData(curr_data, curr_on);
            is_initialized = true;
        }
        else
            displayData(curr_data, curr_on);
    }
    
    public void reset()
    {
        curr_data = init_data;
        curr_on = init_on;
        displayData(curr_data, curr_on);
    }
    
    public void apply() throws Exception
    {
        if(mode == DATA)
        {
            curr_data = getData();
            try {
              subtree.putData(nidData, curr_data, Tree.context);
            } catch(Exception e) 
            {
                System.out.println("Error writing device data: " + e);
                System.out.println(curr_data);
                throw e;
            }
        }
        if(mode != DISPATCH)
        {
            curr_on = getState();
            try {
                subtree.setOn(nidData, curr_on, Tree.context);
            }catch(Exception e)
            {
                System.out.println("Error writing device state: " + e);
            }
        }
    } 
    
    protected void redisplay()
    {
        Container curr_container;
        Component curr_component = this;
        do {
            curr_container = curr_component.getParent();
            curr_component = curr_container;
        }while ((curr_container != null) && !(curr_container instanceof Window));
        if(curr_container != null)
        {
            ((Window)curr_container).pack();
            ((Window)curr_container).show();
        }
    }
        
        
        
    
    //To be subclassed
    protected abstract void initializeData(Data data, boolean is_on);
    protected abstract void displayData(Data data, boolean is_on);
    protected abstract Data getData();
    protected abstract boolean getState();
}


        
    
    
    
    
    
    
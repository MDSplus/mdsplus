import javax.swing.*;
import java.util.*;
import java.awt.*;


public class DeviceSetup extends JDialog
{
    protected String deviceType;
    protected String deviceTitle;
    protected String deviceProvider;
    protected int baseNid, num_components = 0;
    protected Vector device_components = new Vector();
    protected Database subtree = null;
    protected Vector dataChangeListeners = new Vector();
    public void setDeviceType(String deviceType) 
    {
        this.deviceType = deviceType;
        DeviceSetupBeanInfo.beanDeviceType = deviceType;

    }
    public String getDeviceType() 
    {
        DeviceSetupBeanInfo.beanDeviceType = deviceType;
        return deviceType; 
    }
    
    public void setDeviceProvider(String deviceProvider) 
    {
        this.deviceProvider = deviceProvider;
        DeviceSetupBeanInfo.beanDeviceProvider = deviceProvider;

    }
    public String getDeviceProvider() 
    {
        DeviceSetupBeanInfo.beanDeviceProvider = deviceProvider;
        return deviceProvider; 
    }
    
    public void setDeviceTitle(String deviceTitle) 
    {
        this.deviceTitle = deviceTitle;
        setTitle(deviceTitle);
        
    }
    public String getDeviceTitle() {return deviceTitle; }
    
    
    public DeviceSetup()
    {
        this(FrameRepository.frame);
        setTitle(deviceTitle);
//        getContentPane().setLayout(new BorderLayout());
        DeviceSetupBeanInfo.beanDeviceType = deviceType;
        
    }
    public DeviceSetup(Frame f)
    {
        super(FrameRepository.frame);
        setTitle(deviceTitle);
        //getContentPane().setLayout(new BorderLayout());
    }
    public void configure(Database subtree, int baseNid)
    {
        this.baseNid = baseNid;
        this.subtree = subtree;  
        
        //collect every DeviceComponent
        Stack search_stack = new Stack();  
        search_stack.push(this);
        do
        {
            Component [] curr_components = ((Container)search_stack.pop()).getComponents();
            if(curr_components == null) continue;
            for(int i = 0; i < curr_components.length; i++)
            {
                if(curr_components[i] instanceof DeviceComponent)
                    device_components.addElement(curr_components[i]);
                if(curr_components[i] instanceof Container)
                    search_stack.push(curr_components[i]);
            }
        }while(!search_stack.empty());
        //done
        num_components = device_components.size();
        for(int i = 0; i < num_components; i++)
        {
            ((DeviceComponent)device_components.elementAt(i)).setSubtree(subtree);
            ((DeviceComponent)device_components.elementAt(i)).configure(baseNid);
        }
    }
    
    public void apply()
    {
        for(int i = 0; i < num_components; i++)
            ((DeviceComponent)device_components.elementAt(i)).apply();
        fireDataChangeEvent();
    }
    public void reset()
    {
        for(int i = 0; i < num_components; i++)
            ((DeviceComponent)device_components.elementAt(i)).reset();
    }
    public void addDataChangeListener(DataChangeListener listener)
    {
        dataChangeListeners.addElement(listener);
    }
    protected void fireDataChangeEvent()
    {
        int num_listeners = dataChangeListeners.size();
        for(int i = 0; i < num_listeners; i++)
            ((DataChangeListener)dataChangeListeners.elementAt(i)).dataChanged(new DataChangeEvent(this, 0, null));
    }
    
    public int check(String expressions[], String [] messages)
    {
        if(expressions == null || messages == null) return 0;
        int num_expr = expressions.length;
        if(num_expr > messages.length)
            num_expr = messages.length;
        int idx;
        String currId;
        Data currData;
        StringBuffer varExpr = new StringBuffer();
        for(idx = 0; idx < device_components.size(); idx++)
        {
            currId = ((DeviceComponent)(device_components.elementAt(idx))).getIdentifier();
            if(currId != null)
            {
                currData = ((DeviceComponent)(device_components.elementAt(idx))).getData();
                if(currData != null)
                    varExpr.append("_"+currId+ " = " + currData.toString() + ";");
            }
        }
        for(idx = 0; idx < num_expr; idx++)
        {
            if(Data.evaluate(varExpr + expressions[idx]) == 0)
            {
			    JOptionPane.showMessageDialog(null, messages[idx], 
			        "Error in device configuration" ,JOptionPane.WARNING_MESSAGE);
			    return 0;
			}
		}
		return 1;
	}
}       
        
 
import javax.swing.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;

public class DeviceSetup
    extends JDialog
{
    protected String deviceType;
    protected String deviceTitle;
    protected String deviceProvider;
    public int baseNid, num_components = 0;
    protected Vector device_components = new Vector();
    protected Vector device_controls = new Vector();
    public RemoteTree subtree = null;
    protected Vector dataChangeListeners = new Vector();
    protected String[] methods;
    protected DeviceButtons buttons = null;
    JMenuItem pop_items[];
    JPopupMenu pop_methods = null;
    Hashtable updateHash = new Hashtable();
    //public int width = 400, height = 200;

    static Hashtable activeNidHash = new Hashtable();
    Vector deviceCloseListenerV = new Vector();
    Vector deviceUpdateListenerV = new Vector();
    boolean readOnly = false;
    Node deviceNode;
    boolean justApplied = false;

    static Vector openDevicesV = new Vector();

    public int getWidth()
    {
        return super.getWidth();
    }

    public void setWidth(int width)
    {
        int height = super.getHeight();
        setSize(width, height);
    }

    public int getHeight()
    {
        return super.getHeight();
    }

    public void setHeight(int height)
    {
        int width = super.getWidth();
        setSize(width, height);
    }

    public void setReadOnly(boolean readOnly)
    {
        this.readOnly = readOnly;
        for (int i = 0; i < device_controls.size(); i++)
        {
            ( (DeviceControl) device_controls.elementAt(i)).setReadOnly(
                readOnly);
        }
    }

    public boolean isReadOnly()
    {
        return readOnly;
    }

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
        //setTitle(deviceTitle);

    }

    public String getDeviceTitle()
    {
        return deviceTitle;
    }

    public DeviceSetup(boolean readOnly)
    {
        this(FrameRepository.frame, readOnly);
        setTitle(deviceTitle);
//        getContentPane().setLayout(new BorderLayout());
        DeviceSetupBeanInfo.beanDeviceType = deviceType;
    }

    public DeviceSetup()
    {
        this(FrameRepository.frame, false);
        setTitle(deviceTitle);
//        getContentPane().setLayout(new BorderLayout());
        DeviceSetupBeanInfo.beanDeviceType = deviceType;
        openDevicesV.addElement(this);

    }

    public DeviceSetup(JFrame f)
    {
        this(f, false);
        openDevicesV.addElement(this);
    }

    public DeviceSetup(JFrame f, boolean readOnly)
    {
        super(f);
        this.readOnly = readOnly;
        setTitle(deviceTitle);
        addWindowListener(new WindowAdapter(){
            public void windowClosing(WindowEvent e)
            {
                cancel();
            }
        });
    }
    public void configure(RemoteTree subtree, int baseNid)
    {
        configure(subtree, baseNid, null);
    }

    public void resetNidHash()
    {
        activeNidHash.remove(new Integer(baseNid));
    }
    public void configure(RemoteTree subtree, int baseNid, Node node)
    {
        deviceNode = node;
        activeNidHash.put(new Integer(baseNid), this);
        NidData oldNid = null;
        try
        {
            oldNid = subtree.getDefault(Tree.context);
            subtree.setDefault(new NidData(baseNid), Tree.context);
        }
        catch (Exception exc)
        {
            System.out.println(exc);
        }

        this.baseNid = baseNid;
        this.subtree = subtree;

        String path = null;
        try
        {
            NodeInfo info = subtree.getInfo(new NidData(baseNid), 0);
            path = info.getFullPath();
        }
        catch (Exception exc)
        {
            System.out.println(exc);
        }
        if (path == null)
            setTitle(deviceTitle);
        else
            setTitle(deviceTitle + " -- " + path);

        //collect every DeviceComponent
        java.util.Stack search_stack = new java.util.Stack();
        search_stack.push(this);
        do
        {
            Component[] curr_components = ( (Container) search_stack.pop()).
                getComponents();
            if (curr_components == null)continue;
            for (int i = 0; i < curr_components.length; i++)
            {
                if (curr_components[i] instanceof DeviceButtons)
                    methods = ( (DeviceButtons) curr_components[i]).getMethods();

                if (curr_components[i] instanceof DeviceComponent)
                {
                    device_components.addElement(curr_components[i]);
                }
                if (curr_components[i] instanceof DeviceControl)
                {
                    device_controls.addElement(curr_components[i]);
                }
                if (curr_components[i] instanceof Container)
                    search_stack.push(curr_components[i]);
                if (curr_components[i] instanceof DeviceButtons)
                    buttons = (DeviceButtons) curr_components[i];
            }
        }
        while (!search_stack.empty());
        //done
        num_components = device_components.size();
        for (int i = 0; i < num_components; i++)
        {
            ( (DeviceComponent) device_components.elementAt(i)).setSubtree(
                subtree);
            ( (DeviceComponent) device_components.elementAt(i)).configure(
                baseNid, readOnly);
            String currUpdateId = ( (DeviceComponent) device_components.
                                   elementAt(i)).getUpdateId(this);
            if (currUpdateId != null && !currUpdateId.equals(""))
            {
                Vector components = (Vector) updateHash.get(currUpdateId);
                if (components == null)
                {
                    components = new Vector();
                    updateHash.put(currUpdateId, components);
                }
                components.addElement(device_components.elementAt(i));
            }
        }
        try
        {
            //A secod turn in order to carry out actions which need inter-component info
            for (int i = 0; i < num_components; i++)
            {
                ( (DeviceComponent) device_components.elementAt(i)).
                    postConfigure();
            }
        }
        catch (Throwable exc)
        {
            System.out.println(exc);
        }

        if (methods != null && methods.length > 0)
        {
            pop_methods = new JPopupMenu("Methods");
            //pop_methods = new JPopupMenu();
            pop_items = new JMenuItem[methods.length];
            for (int i = 0; i < methods.length; i++)
            {
                pop_methods.add(pop_items[i] = new JMenuItem(methods[i]));
                pop_items[i].addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent e)
                    {
                        boolean success = true;
                        String errmsg = "";
                        int j;
                        for (j = 0;
                             j < pop_items.length &&
                             ( (JMenuItem) e.getSource()) != pop_items[j]; j++);
                        if (j == pop_items.length)return;
                        if (JOptionPane.showConfirmDialog(DeviceSetup.this,
                            "Execute " + methods[j] + "?",
                            "Execute a device method",
                            JOptionPane.YES_NO_OPTION,
                            JOptionPane.QUESTION_MESSAGE) ==
                            JOptionPane.YES_OPTION)
                        {
                            try
                            {
                                DeviceSetup.this.subtree.doDeviceMethod(
                                    new NidData(DeviceSetup.this.baseNid),
                                    methods[j], Tree.context);
                            }
                            catch (Exception exc)
                            {
                                errmsg = exc.toString();
                                success = false;
                            }
                            if (!success)
                                JOptionPane.showMessageDialog(DeviceSetup.this,
                                    "Error executing method " + methods[j] +
                                    ": " + errmsg,
                                    "Method execution report",
                                    JOptionPane.WARNING_MESSAGE);
                            else
                                JOptionPane.showMessageDialog(DeviceSetup.this,
                                    "Method " + methods[j] +
                                    " succesfully executed",
                                     "Method execution report",
                                     JOptionPane.INFORMATION_MESSAGE);
                        }
                    }
                });
            }
            pop_methods.pack();
            addMouseListener(new MouseAdapter()
            {
                public void mousePressed(MouseEvent e)
                {
                    if ( (e.getModifiers() & Event.META_MASK) != 0)
                    {
                        pop_methods.setInvoker(DeviceSetup.this);
                        pop_methods.show(DeviceSetup.this, e.getX(), e.getY());
                    }
                }
                /*public void mouseReleased(MouseEvent e)
                                 {
                    if(e.isPopupTrigger())
                        pop_methods.show(DeviceSetup.this, e.getX(), e.getY());
                                 }*/
            });
        }
        try
        {
            subtree.setDefault(oldNid, Tree.context);
        }
        catch (Exception exc)
        {
            System.out.println("Error in Configure: " + exc);
        }

    }

    public void apply()
    {
        NidData oldNid = null;
        try
        {
            oldNid = subtree.getDefault(Tree.context);
            subtree.setDefault(new NidData(baseNid), Tree.context);
        }
        catch (Exception exc)
        {}

        for (int i = 0; i < num_components; i++)
        {
            try
            {
                ( (DeviceComponent) device_components.elementAt(i)).apply();
            }
            catch (Exception exc)
            {
                JOptionPane.showMessageDialog(this, exc.toString(),
                                              "Error writing data at offset nid " +
                                              ( (DeviceComponent)
                                               device_components.elementAt(i)).
                                              getOffsetNid(),
                                              JOptionPane.WARNING_MESSAGE);
            }
        }
        for (int i = 0; i < num_components; i++)
        {
            ( (DeviceComponent) device_components.elementAt(i)).postApply();
        }
        fireDataChangeEvent();
        try
        {
            subtree.setDefault(oldNid, Tree.context);
        }
        catch (Exception exc)
        {}
        for (int i = 0; i < deviceUpdateListenerV.size(); i++)
        {
            if(isChanged())
                ( (DeviceUpdateListener) deviceUpdateListenerV.elementAt(i)).
                    deviceUpdated();
        }
        if(deviceNode != null)
        {
            deviceNode.setAllOnUnchecked();
            FrameRepository.frame.repaint();
        }
        justApplied = true;
    }
    public void apply(int currBaseNid)
    {
        NidData oldNid = null;
        try
        {
            oldNid = subtree.getDefault(Tree.context);
            subtree.setDefault(new NidData(currBaseNid), Tree.context);
        }
        catch (Exception exc)
        {}

        for (int i = 0; i < num_components; i++)
        {
            try
            {
                ( (DeviceComponent) device_components.elementAt(i)).apply(currBaseNid);
            }
            catch (Exception exc)
            {
                JOptionPane.showMessageDialog(this, exc.toString(),
                                              "Error writing data at offset nid " +
                                              ( (DeviceComponent)
                                               device_components.elementAt(i)).
                                              getOffsetNid(),
                                              JOptionPane.WARNING_MESSAGE);
            }
        }
        try
        {
            subtree.setDefault(oldNid, Tree.context);
        }
        catch (Exception exc)
        {}
        if(deviceNode != null) deviceNode.setAllOnUnchecked();
        justApplied = true;
    }
    public void reset()
    {
        NidData oldNid = null;
        try
        {
            oldNid = subtree.getDefault(Tree.context);
            subtree.setDefault(new NidData(baseNid), Tree.context);
        }
        catch (Exception exc)
        {
            System.out.println(exc);
        }
        for (int i = 0; i < num_components; i++)
            ( (DeviceComponent) device_components.elementAt(i)).reset();

        try
        {
            subtree.setDefault(oldNid, Tree.context);
        }
        catch (Exception exc)
        {
            System.out.println("Error in Configure: " + exc);
        }
    }

    public void addDataChangeListener(DataChangeListener listener)
    {
        dataChangeListeners.addElement(listener);
    }

    protected void fireDataChangeEvent()
    {
        int num_listeners = dataChangeListeners.size();
        for (int i = 0; i < num_listeners; i++)
            ( (DataChangeListener) dataChangeListeners.elementAt(i)).
                dataChanged(new DataChangeEvent(this, 0, null));
    }

    public void propagateData(int offsetNid, Object data)
    {
        for (int idx = 0; idx < device_components.size(); idx++)
            ( (DeviceComponent) device_components.elementAt(idx)).dataChanged(
                offsetNid,
                data);
    }

    public void propagateState(int offsetNid, boolean state)
    {
        for (int idx = 0; idx < device_components.size(); idx++)
            ( (DeviceComponent) device_components.elementAt(idx)).stateChanged(
                offsetNid,
                state);
    }

    public void updateIdentifiers()
    {
        StringBuffer varExpr = new StringBuffer();
        for (int idx = 0; idx < device_components.size(); idx++)
        {
            String currId = ( (DeviceComponent) (device_components.elementAt(
                idx))).getIdentifier();
            if (currId != null)
            {
                Data currData = ( (DeviceComponent) (device_components.
                    elementAt(idx))).getData();
                if (currData != null)
                    varExpr.append("_" + currId + " = " +
                                   Tree.dataToString(currData) + ";");
                if ( ( (DeviceComponent) (device_components.elementAt(idx))).
                    getState())
                    varExpr.append("_" + currId + "_state = 1; ");
                else
                    varExpr.append("_" + currId + "_state = 0; ");
            }
        }
        if (device_components.size() > 0)
            Data.evaluate(varExpr.toString());
    }

    public boolean check()
    {
        if(buttons != null)
            return buttons.check();
        return true;
    }

    public void setCancelText(String cancelText)
    {
        if(buttons != null)
            buttons.setCancelText(cancelText);
    }

    public boolean check(String expressions[], String[] messages)
    {
        if (expressions == null || messages == null)return true;
        int num_expr = expressions.length;
        if (num_expr > messages.length)
            num_expr = messages.length;
        int idx;
        String currId;
        Data currData;
        StringBuffer varExpr = new StringBuffer();
        for (idx = 0; idx < device_components.size(); idx++)
        {
            currId = ( (DeviceComponent) (device_components.elementAt(idx))).
                getIdentifier();
            if (currId != null && !currId.trim().equals(""))
            {
                currData = ( (DeviceComponent) (device_components.elementAt(idx))).
                    getData();
                if (currData != null)
                    varExpr.append("_" + currId + " = " +
                                   Tree.dataToString(currData) + ";");
                int currState;
                if ( ( (DeviceComponent) (device_components.elementAt(idx))).
                    getState())
                    varExpr.append("_" + currId + "_state = 1; ");
                else
                    varExpr.append("_" + currId + "_state = 0; ");
            }
        }
        for (idx = 0; idx < num_expr; idx++)
        {
			
            if (Data.evaluate(varExpr + expressions[idx]) == 0)
            {
                JOptionPane.showMessageDialog(this, messages[idx],
                                              "Error in device configuration",
                                              JOptionPane.WARNING_MESSAGE);
                return false;
            }
        }
        return true;
    }

    public void fireUpdate(String id, Data val)
    {
        Vector components = (Vector) updateHash.get(id);
        if (components != null)
        {
            for (int i = 0; i < components.size(); i++)
                ( (DeviceComponent) components.elementAt(i)).fireUpdate(id, val);
        }
    }

    public static void activateDeviceSetup(String deviceName, String experiment,
                                           int shot, String rootName, int x,
                                           int y)
    {
        Database tree = new Database(experiment, shot);
        try
        {
            tree.open();
        }
        catch (Exception exc)
        {
            JOptionPane.showMessageDialog(null,
                                          "Error opening tree " + experiment +
                                          " shot " + shot + ": " + exc,
                                          "Error in Device Setup",
                                          JOptionPane.ERROR_MESSAGE);
            return;
        }
        NidData nid;
        try
        {
            nid = tree.resolve(new PathData(rootName), 0);
        }
        catch (Exception exc)
        {
            JOptionPane.showMessageDialog(null,
                                          "Cannot find node " + rootName + ": " +
                                          exc, "Error in Device Setup",
                                          JOptionPane.ERROR_MESSAGE);
            return;
        }
        try
        {
            String deviceClassName = deviceName + "Setup";
            Class deviceClass = Class.forName(deviceClassName);
            DeviceSetup ds = (DeviceSetup) deviceClass.newInstance();
            ds.configure(tree, nid.getInt(), null);
            if (ds.getContentPane().getLayout() != null)
                ds.pack();

            ds.setLocation(x, y);
            ds.setVisible(true);
        }
        catch (Exception exc)
        {
            exc.printStackTrace();
            JOptionPane.showMessageDialog(null,
                                          "Cannot activate Setup dir device " +
                                          deviceName + ": " + exc,
                                          "Error in Device Setup",
                                          JOptionPane.ERROR_MESSAGE);
        }
    }

    void cancel()
    {
        activeNidHash.remove(new Integer(baseNid));

        openDevicesV.removeElement(this);
        if(deviceNode != null) deviceNode.setAllOnUnchecked();
        dispose();
        for (int i = 0; i < deviceCloseListenerV.size(); i++)
            ( (DeviceCloseListener) deviceCloseListenerV.elementAt(i)).
                deviceClosed(isChanged(), justApplied);
        
        justApplied = false;

    }

    static DeviceSetup getDevice(int baseNid)
    {
        return (DeviceSetup) activeNidHash.get(new Integer(baseNid));
    }

    boolean isChanged()
    {
        for (int i = 0; i < num_components; i++)
        {
            DeviceComponent currComponent = (DeviceComponent) device_components.
                elementAt(i);
            if (currComponent.isChanged())
                return true;
            if (currComponent.isStateChanged())
                return true;
        }
        return false;
    }

    void addDeviceCloseListener(DeviceCloseListener listener)
    {
        deviceCloseListenerV.addElement(listener);
    }
    void addDeviceUpdateListener(DeviceUpdateListener listener)
    {
        deviceUpdateListenerV.addElement(listener);
    }


    void addButton(JButton button)
    {
        if (buttons != null)
            buttons.add(button);
    }

    void setHighlight(boolean isHighlighted, int [] nids)
    {
        DeviceComponent currDevComponent;
        for(int i = 0; i < device_components.size(); i++)
        {
            currDevComponent = (DeviceComponent)device_components.elementAt(i);
            int nid = currDevComponent.getBaseNid() + currDevComponent.getOffsetNid();
            int j;
            for(j = 0; j < nids.length; j++)
                if(nids[j] == nid) break;
            if(j < nids.length)
                currDevComponent.setHighlight(isHighlighted);
        }

    }



    static void closeOpenDevices()
    {
        for(int i = 0; i < openDevicesV.size(); i++)
            ((DeviceSetup)openDevicesV.elementAt(i)).cancel();
    }
    public static void main(String args[])
    {
        DeviceSetup.activateDeviceSetup("T2Control", "T2", -1, "\\T2", 100, 100);
    }
}


import javax.swing.*;

import mds.devices.Interface;

import java.awt.*;
public abstract class DeviceComponent extends JPanel
{
    Interface subtree;

    public int mode = DATA;
    public static final int DATA = 0, STATE = 1, DISPATCH = 2;
    public int baseNid = 0, offsetNid = 0;
    protected String curr_data, init_data;
    protected boolean curr_on, init_on;
    protected int nidData;
    protected int baseNidData;
    protected String identifier;
    protected String updateIdentifier;
    protected boolean editable = true;
    protected boolean isHighlighted = false;
    private boolean is_initialized = false;
    private boolean enabled = true;
    
    private int refShot = -2;



    void setSubtree(Interface subtree) {this.subtree = subtree; }
    Interface getSubtree(){return subtree; }
    public void setRefShot(int refShot){this.refShot = refShot;}
    public int getRefShot(){return refShot;}
    public void setBaseNid(int nid) {baseNid = nid; }
    public int getBaseNid() {return baseNid; }
    public void setOffsetNid(int nid) {offsetNid = nid; }
    public int getOffsetNid() {return offsetNid; }
    public void setIdentifier(String identifier)
    {
	this.identifier = identifier;
    }
    public String getIdentifier()
    {
	return identifier;
    }
    public void setUpdateIdentifier(String updateIdentifier)
    {
	this.updateIdentifier = updateIdentifier;
    }
    public String getUpdateIdentifier() {return updateIdentifier; }


    public void configure(int baseNid, boolean readOnly)
    {
	configure(baseNid);
    }

    public void configure(int baseNid)
    {
	this.baseNid = baseNid;
	nidData = baseNid+offsetNid;
	baseNidData = baseNid;
	if(mode == DATA)
	{
	    try {
	        init_data = curr_data = subtree.getDataExpr(nidData);
	    }catch(Exception e) {init_data = curr_data = null;}



	}
	else init_data = null;
	//if(mode != DISPATCH)
	{
	    try {
	        init_on = curr_on = subtree.isOn(nidData);
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
        checkRefShot();
    }

    public void reset()
    {
	curr_data = init_data;
	curr_on = init_on;
	displayData(curr_data, curr_on);
    }
    
    public void update()
    {
        try {
            String updatedData = subtree.getDataExpr(baseNid+offsetNid);
            String prevInitData = init_data;
            init_data = updatedData;
            reset();
            init_data = prevInitData;
        }
        catch(Exception exc){}
    }

    public void apply() throws Exception
    {
	if(!enabled) return;
	if(mode == DATA)
	{
	    curr_data = getData();
/*            if(curr_data instanceof PathData)
	    {
	        try {
	            curr_data = subtree.resolve((PathData)curr_data, Tree.context);
	        }catch(Exception exc){}
	    }
  */
	    if(editable && isDataChanged())
	    {
	        try {
	        subtree.putDataExpr(nidData, curr_data);
	        } catch(Exception e)
	        {
	            System.out.println("Error writing device data: " + e);
	            System.out.println(curr_data);
	            throw e;
	        }
	    }
	}
	if(mode != DISPATCH && supportsState())
	{
	    curr_on = getState();
	    try {
	        subtree.setOn(nidData, curr_on);
	    }catch(Exception e)
	    {
	        System.out.println("Error writing device state: " + e);
	    }
	}
        checkRefShot();
    }

    public void apply(int currBaseNid) throws Exception
    {
	int currNidData = currBaseNid+offsetNid;
	if(!enabled) return;
	if(mode == DATA)
	{
	    curr_data = getData();
	    if(editable)// && isDataChanged())
	    {
	        try {
	        subtree.putDataExpr(currNidData, curr_data);
	        } catch(Exception e)
	        {
	            System.out.println("Error writing device data: " + e);
	            System.out.println("at node: " + subtree.getFullPath(nidData));
	            System.out.println(curr_data);
	            throw e;
	        }
	    }
	}
	if(mode != DISPATCH && supportsState())
	{
	    curr_on = getState();
	    try {
	        subtree.setOn(currNidData, curr_on);
	    }catch(Exception e)
	    {
	        System.out.println("Error writing device state: " + e);
	    }
	}
        checkRefShot();
    }




    protected void redisplay()
    {
	Container curr_container;
	Component curr_component = this;
	do {
	    curr_container = curr_component.getParent();
	    curr_component = curr_container;
	}while ((curr_container != null) && !(curr_container instanceof Window));
       /* if(curr_container != null)
	{
	    ((Window)curr_container).pack();
	    ((Window)curr_container).setVisible(true);
	}*/
    }

//Event handling in DW setup
    DeviceSetup master = null;
    public String getUpdateId(DeviceSetup master)
    {
	this.master = master;
	return updateIdentifier;
    }
    public void fireUpdate(String updateId, String newExpr){}
    //To be subclassed
    protected abstract void initializeData(String data, boolean is_on);
    protected abstract void displayData(String data, boolean is_on);
    protected abstract String getData();
    protected abstract boolean getState();
    //Copy-Paste management
    protected Object copyData() {return null;}
    protected void pasteData(Object objData){}



    public void postConfigure(){}
    void postApply(){}
    protected boolean supportsState(){return false;}
    public void setEnable()
    {
	enabled = true;
    }
    public void setDisable()
    {
	enabled = false;
    }

    public void reportDataChanged(Object data)
    {
      if(master == null) return;
      master.propagateData(offsetNid, data);
    }

    public void reportStateChanged(boolean state)
    {
      if(master == null) return;
      master.propagateState(offsetNid, state);
    }

    protected void dataChanged(int offsetNid, Object data){}
    protected void stateChanged(int offsetNid, boolean state){}
    protected boolean isDataChanged()
    {
	return true;
    }
    protected boolean isChanged()
    {
	try
	{
	    return! (init_data.equals(curr_data));
	}
	catch (Exception exc)
	{
	    return false;
	}
    }
    protected boolean isStateChanged()
    {
	return !(init_on == curr_on);
    }

    //Get an object incuding all related info (will be data except for DeviceWaveform
    protected Object getFullData(){return getData();}

    public void checkRefShot()
    {
        if(refShot < -1) return;
        try {
            String refPath = subtree.getFullPath(baseNid + offsetNid);
            String refExpr = "USING("+refPath+",,"+refShot+")";
            System.out.println(refExpr);
            String refDataExpr = subtree.execute(refExpr);
            String currDataExpr = subtree.execute(curr_data);
            if(!currDataExpr.equals(refDataExpr))
                setHighlight(true);
            else
                setHighlight(false);
        }
        catch(Exception exc){}
        
    }
    
    
    public void setHighlight(boolean isHighlighted)
    {
	this.isHighlighted = isHighlighted;
	Component currParent, currGrandparent = this;
	do {
	    currParent = currGrandparent;
	    currGrandparent = currParent.getParent();
	    if(currGrandparent instanceof JTabbedPane)
	    {
	        int idx = ((JTabbedPane)currGrandparent).indexOfComponent(currParent);
	        ((JTabbedPane)currGrandparent).setForegroundAt(idx, isHighlighted?Color.red:Color.black);
	    }
	}while(!(currGrandparent instanceof DeviceSetup));
    }

}









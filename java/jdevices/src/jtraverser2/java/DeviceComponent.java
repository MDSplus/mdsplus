import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Window;

import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.xml.crypto.Data;

import mds.data.TREE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Nid;
public abstract class DeviceComponent extends JPanel
{
	private static final long serialVersionUID = 1L;

	TREE subtree;

    public int mode = DATA;
    public static final int DATA = 0, STATE = 1, DISPATCH = 2;
    public int baseNid = 0, offsetNid = 0;
    protected Descriptor<?> curr_data, init_data;
    protected boolean curr_on, init_on;
    protected Nid nidData;
    protected Nid baseNidData;
    protected String identifier;
    protected String updateIdentifier;
    protected boolean editable = true;
    protected boolean isHighlighted = false;
    private boolean is_initialized = false;
    private boolean enabled = true;



    void setSubtree(TREE subtree) {this.subtree = subtree; }
    TREE getSubtree(){return subtree; }
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
	nidData = new Nid(baseNid+offsetNid);
	baseNidData = new Nid(baseNid);
	if(mode == DATA)
	{
	    try {
	        init_data = curr_data = subtree.getRecord(nidData.getNidNumber());
	    }catch(Exception e) {init_data = curr_data = null;}



	}
	else init_data = null;
	//if(mode != DISPATCH)
	{
	    try {
	        init_on = curr_on = nidData.isOn();
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
	        	nidData.putRecord(curr_data);
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
	    	nidData.setOn(curr_on);
	    }catch(Exception e)
	    {
	        System.out.println("Error writing device state: " + e);
	    }
	}
    }

    public void apply(int currBaseNid) throws Exception
    {
	Nid currNidData = new Nid(currBaseNid+offsetNid, subtree);
	if(!enabled) return;
	if(mode == DATA)
	{
	    curr_data = getData();
	    if(editable)// && isDataChanged())
	    {
	        try {
	        	currNidData.putRecord(currNidData);
	        } catch(Exception e)
	        {
	            System.out.println("Error writing device data: " + e);
	            System.out.println("at node: " + nidData.getFullPath());
	            System.out.println(curr_data);
	            throw e;
	        }
	    }
	}
	if(mode != DISPATCH && supportsState())
	{
	    curr_on = getState();
	    try {
	    	currNidData.setOn(curr_on);
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
    public void fireUpdate(String updateId, Descriptor<?> val){}
    //To be subclassed
    protected abstract void initializeData(Descriptor<?> data, boolean is_on);
    protected abstract void displayData(Descriptor<?> data, boolean is_on);
    protected abstract Descriptor<?> getData();
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
	    String initDecompiled = init_data.decompile();
	    String currDecompiled = curr_data.decompile();
	    //System.out.println("Comparing " + initDecompiled + "  " + currDecompiled);
	    return! (initDecompiled.equals(currDecompiled));
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









import javax.swing.*;

import mds.devices.Interface;

import java.awt.*;

public abstract class DeviceComponent extends JPanel
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	public static final int DATA = 0, STATE = 1, DISPATCH = 2, STRUCTURE = 3;
	Interface subtree;
	public int mode = DATA;
	public int baseNid = 0, offsetNid = 0;
	protected String curr_data = null, init_data;
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

	//Event handling in DW setup
	DeviceSetup master = null;

	public void apply() throws Exception
	{
		if (!enabled)
			return;
		if (mode == DATA)
		{
			curr_data = getData();
			/*
			 * if(curr_data instanceof PathData) { try { curr_data =
			 * subtree.resolve((PathData)curr_data, Tree.context); }catch(Exception exc){} }
			 */
			if (editable && isDataChanged())
			{
				try
				{
					subtree.putDataExpr(nidData, curr_data);
				}
				catch (final Exception e)
				{
					System.out.println("Error writing device data: " + e);
					System.out.println(curr_data);
					throw e;
				}
			}
                        checkRefShot();
		}
		if (mode != DISPATCH && supportsState())
		{
			curr_on = getState();
			try
			{
				subtree.setOn(nidData, curr_on);
			}
			catch (final Exception e)
			{
				System.out.println("Error writing device state: " + e);
			}
		}
	}

	public void apply(int currBaseNid) throws Exception
	{
		final int currNidData = currBaseNid + offsetNid;
		if (!enabled)
			return;
		if (mode == DATA)
		{
			curr_data = getData();
			if (editable)// && isDataChanged())
			{
				try
				{
					subtree.putDataExpr(currNidData, curr_data);
				}
				catch (final Exception e)
				{
					System.out.println("Error writing device data: " + e);
					System.out.println("at node: " + subtree.getFullPath(nidData));
					System.out.println(curr_data);
					throw e;
				}
			}
		}
		if (mode != DISPATCH && supportsState())
		{
			curr_on = getState();
			try
			{
				subtree.setOn(currNidData, curr_on);
			}
			catch (final Exception e)
			{
				System.out.println("Error writing device state: " + e);
			}
		}
		checkRefShot();
	}

	public void checkRefShot()
	{
		if (refShot < -1)
			return;
		try
		{
			final String refPath = subtree.getFullPath(baseNid + offsetNid);
			final String refExpr = "USING(" + refPath + ",," + refShot + ")";
			System.out.println(refExpr);
			final String refDataExpr = subtree.execute(refExpr);
			final String currDataExpr = subtree.execute(curr_data);
			if (!currDataExpr.equals(refDataExpr))
				setHighlight(true);
			else
				setHighlight(false);
		}
		catch (final Exception exc)
		{}
	}

	public void configure(int baseNid)
	{
		this.baseNid = baseNid;
		nidData = baseNid + offsetNid;
		baseNidData = baseNid;
		if (mode == DATA)
		{
			try
			{
				init_data = curr_data = subtree.getDataExpr(nidData);
			}
			catch (final Exception e)
			{
				init_data = curr_data = null;
			}
		}
		else
			init_data = null;
		// if(mode != DISPATCH)
		{
			try
			{
				init_on = curr_on = subtree.isOn(nidData);
			}
			catch (final Exception e)
			{
				System.out.println("Error configuring device: " + e);
			}
		}
		if (!is_initialized)
		{
			initializeData(curr_data, curr_on);
			is_initialized = true;
		}
		else
			displayData(curr_data, curr_on);
		checkRefShot();
	}

	public void configure(int baseNid, boolean readOnly)
	{
		configure(baseNid);
	}

	// Copy-Paste management
	protected Object copyData()
	{
		return null;
	}

	protected void dataChanged(int offsetNid, Object data)
	{}

	protected abstract void displayData(String data, boolean is_on);

	public void fireUpdate(String updateId, String newExpr)
	{}

	public int getBaseNid()
	{ return baseNid; }

	protected abstract String getData();

	// Get an object incuding all related info (will be data except for
	// DeviceWaveform
	protected Object getFullData()
	{ return getData(); }

	public String getIdentifier()
	{ return identifier; }

	public int getOffsetNid()
	{ return offsetNid; }

	public int getRefShot()
	{ return refShot; }

	protected abstract boolean getState();

	Interface getSubtree()
	{ return subtree; }

	public String getUpdateId(DeviceSetup master)
	{
		this.master = master;
		return updateIdentifier;
	}

public String getUpdateIdentifier()
	{ return updateIdentifier; }

	// To be subclassed
	protected abstract void initializeData(String data, boolean is_on);

	protected boolean isChanged()
	{
		try
		{
			return !(init_data.equals(curr_data));
		}
		catch (final Exception exc)
		{
			return false;
		}
	}

	protected boolean isDataChanged()
	{ return true; }

	protected boolean isStateChanged()
	{ return !(init_on == curr_on); }

	protected void pasteData(Object objData)
	{}

	void postApply()
	{}

	public void postConfigure()
	{}

	protected void redisplay()
	{
		Container curr_container;
		Component curr_component = this;
		do
		{
			curr_container = curr_component.getParent();
			curr_component = curr_container;
		}
		while ((curr_container != null) && !(curr_container instanceof Window));
		/*
		 * if(curr_container != null) { ((Window)curr_container).pack();
		 * ((Window)curr_container).setVisible(true); }
		 */
	}

	public void reportDataChanged(Object data)
	{
		if (master == null)
			return;
		master.propagateData(offsetNid, data);
	}

	public void reportStateChanged(boolean state)
	{
		if (master == null)
			return;
		master.propagateState(offsetNid, state);
	}

	public void reset()
	{
		curr_data = init_data;
		curr_on = init_on;
		displayData(curr_data, curr_on);
	}

	public void setBaseNid(int nid)
	{ baseNid = nid; }

	public void setDisable()
	{
		enabled = false;
	}

	public void setEnable()
	{
		enabled = true;
	}

	public void setHighlight(boolean isHighlighted)
	{
		this.isHighlighted = isHighlighted;
		Component currParent, currGrandparent = this;
		do
		{
			currParent = currGrandparent;
			currGrandparent = currParent.getParent();
			if (currGrandparent instanceof JTabbedPane)
			{
				final int idx = ((JTabbedPane) currGrandparent).indexOfComponent(currParent);
				((JTabbedPane) currGrandparent).setForegroundAt(idx, isHighlighted ? Color.red : Color.black);
			}
		}
		while (!(currGrandparent instanceof DeviceSetup));
	}

	public void setIdentifier(String identifier)
	{ this.identifier = identifier; }

	public void setOffsetNid(int nid)
	{ offsetNid = nid; }

	public void setRefShot(int refShot)
	{ this.refShot = refShot; }

	void setSubtree(Interface subtree)
	{ this.subtree = subtree; }

	public void setUpdateIdentifier(String updateIdentifier)
	{ this.updateIdentifier = updateIdentifier; }

	protected void stateChanged(int offsetNid, boolean state)
	{}

	protected boolean supportsState()
	{
		return false;
	}

	public void update()
	{
		try
		{
			final String updatedData = subtree.getDataExpr(baseNid + offsetNid);
			final String prevInitData = init_data;
			init_data = updatedData;
			reset();
			init_data = prevInitData;
		}
		catch (final Exception exc)
		{}
	}
}

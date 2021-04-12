import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

import mds.devices.Interface;
import mds.devices.Interface.Setup;

public class DeviceSetup extends JDialog implements Interface.Setup
{
	private static final long serialVersionUID = 1L;
	// public int width = 400, height = 200;
	protected static Hashtable<Integer, DeviceSetup> activeNidHash = new Hashtable<>();
	static Vector<DeviceSetup> openDevicesV = new Vector<>();

	static void closeSetups()
	{
		for (final DeviceSetup setup : openDevicesV)
			setup.cancel();
	}

	public final static Setup getSetup(final int nid, final boolean readOnly)
	{
		final DeviceSetup ds = activeNidHash.get(new Integer(nid));
		if (ds != null)
		{
			ds.setReadOnly(readOnly);
			ds.setVisible(true);
		}
		return ds;
	}

	public static Setup newSetup(final int nid, final String model, final Interface iface,
			final Object pointOrComponent, final boolean readOnly) throws Exception
	{
		Point point = null;
		Component parent = null;
		if (pointOrComponent instanceof Component)
		{
			parent = (Component) pointOrComponent;
		}
		else
		{
			point = (Point) pointOrComponent;
		}
		final String deviceClassName = model + "Setup";
		final Class<?> deviceClass = Class.forName(deviceClassName);
		final DeviceSetup ds = (DeviceSetup) deviceClass.newInstance();
		final Dimension prevDim = ds.getSize();
		ds.configure(iface, nid);
		ds.setReadOnly(readOnly);
		if (ds.getContentPane().getLayout() != null)
			ds.pack();
		ds.setSize(prevDim);
		if (parent != null)
		{
			ds.setLocationRelativeTo(parent);
		}
		else if (point != null)
		{
			ds.setLocation(point);
		}
		else
			ds.setLocationByPlatform(true);
		ds.setVisible(true);
		return ds;
	}

	protected String deviceType;
	protected String deviceTitle;
	protected String deviceProvider;
	public int baseNid, num_components = 0;
	protected Vector<DeviceComponent> device_components = new Vector<>();
	protected Vector<DeviceControl> device_controls = new Vector<>();
	public Interface subtree = null;
	protected String[] methods;
	protected DeviceButtons buttons = null;
	JMenuItem pop_items[];
	JPopupMenu pop_methods = null;
	Hashtable<String, Vector<DeviceComponent>> updateHash = new Hashtable<>();
	Vector<DeviceCloseListener> deviceCloseListenerV = new Vector<>();
	Vector<DeviceUpdateListener> deviceUpdateListenerV = new Vector<>();
	boolean readOnly = false;
	boolean justApplied = false;
	protected String updateEvent = null;

	/*
	 * public DeviceSetup(boolean readOnly) { this(FrameRepository.frame, readOnly);
	 * setTitle(deviceTitle); // getContentPane().setLayout(new BorderLayout());
	 * DeviceSetupBeanInfo.beanDeviceType = deviceType; }
	 */
	public DeviceSetup()
	{
		this(null, false);
		setTitle(deviceTitle);
		// getContentPane().setLayout(new BorderLayout());
		DeviceSetupBeanInfo.beanDeviceType = deviceType;
		openDevicesV.addElement(this);
	}

	public DeviceSetup(Frame f)
	{
		this(f, false);
		openDevicesV.addElement(this);
	}

	public DeviceSetup(Frame f, boolean readOnly)
	{
		super(f);
		this.readOnly = readOnly;
		setTitle(deviceTitle);
		addWindowListener(new WindowAdapter()
		{
			@Override
			public void windowClosing(WindowEvent e)
			{
				cancel();
			}
		});
	}

	void addButton(JButton button)
	{
		if (buttons != null)
			buttons.add(button);
	}

	void addDeviceCloseListener(DeviceCloseListener listener)
	{
		deviceCloseListenerV.addElement(listener);
	}

	void addDeviceUpdateListener(DeviceUpdateListener listener)
	{
		deviceUpdateListenerV.addElement(listener);
	}

	public void apply()
	{
		final int oldDef = this.pushDefault();
		for (final DeviceComponent comp : device_components)
			try
			{
				comp.apply();
			}
			catch (final Exception exc)
			{
				exc.printStackTrace();
                                java.lang.String path;
                                try {
                                    path = subtree.getFullPath(comp.getBaseNid()+comp.getOffsetNid());
                                }
                                catch(Exception exc1)
                                {
                                    path = "";
                                }
				JOptionPane.showMessageDialog(this, "Error writing data at  " + path + ": "+exc.toString(),
						"Error writing data", JOptionPane.WARNING_MESSAGE);
			}
		for (final DeviceComponent comp : device_components)
			comp.postApply();
		popDefault(oldDef);
		if (isChanged())
			for (final DeviceUpdateListener dul : deviceUpdateListenerV)
				dul.deviceUpdated();
		justApplied = true;
	}

	public void apply(int currBaseNid)
	{
		final int oldDef = this.pushDefault();
		for (final DeviceComponent comp : device_components)
			try
			{
				comp.apply(currBaseNid);
			}
			catch (final Exception exc)
			{
				exc.printStackTrace();
				JOptionPane.showMessageDialog(this, exc.toString(),
						"Error writing data at offset nid " + comp.getOffsetNid(), JOptionPane.WARNING_MESSAGE);
			}
		popDefault(oldDef);
		justApplied = true;
	}

	void cancel()
	{
		activeNidHash.remove(new Integer(baseNid));
		openDevicesV.removeElement(this);
		dispose();
		for (final DeviceCloseListener dcl : deviceCloseListenerV)
			dcl.deviceClosed(isChanged(), justApplied);
		justApplied = false;
	}

	public boolean check()
	{
		if (buttons != null)
			return buttons.check();
		return true;
	}

	public boolean check(String expressions[], String[] messages)
	{
		if (expressions == null || messages == null)
			return true;
		final int num_expr = Math.min(expressions.length, messages.length);
		final StringBuffer varExpr = new StringBuffer();
		for (final DeviceComponent comp : device_components)
		{
			final String currId = comp.getIdentifier();
			if (currId != null && !currId.trim().equals(""))
			{
				final String currData = comp.getData();
				if (currData != null)
					varExpr.append("_" + currId + " = " + currData + ";");
				if (comp.getState())
					varExpr.append("_" + currId + "_state = 1; ");
				else
					varExpr.append("_" + currId + "_state = 0; ");
			}
		}
		for (int i = 0; i < num_expr; i++)
		{
			try
			{
				if (subtree.getInt(varExpr + expressions[i]) == 0)
					JOptionPane.showMessageDialog(this, messages[i], "Error in device configuration",
							JOptionPane.WARNING_MESSAGE);
			}
			catch (final Exception exc)
			{
				JOptionPane.showMessageDialog(this, exc.toString(), "Error in device configuration",
						JOptionPane.WARNING_MESSAGE);
				return false;
			}
		}
		return true;
	}

	public void configure(Interface subtree, int baseNid)
	{
		this.baseNid = baseNid;
		this.subtree = subtree;
		activeNidHash.put(new Integer(baseNid), this);
		final int oldDef = pushDefault();
		String path = null;
		try
		{
			path = subtree.getFullPath(baseNid);
		}
		catch (final Exception exc)
		{
			System.out.println(exc);
		}
		if (path == null)
			setTitle(deviceTitle);
		else
			setTitle(deviceTitle + " -- " + path);
		// collect every DeviceComponent
		final Stack<Container> search_stack = new Stack<>();
		search_stack.push(this);
		while (!search_stack.isEmpty())
		{
			final Component[] curr_components = search_stack.pop().getComponents();
			if (curr_components == null)
				continue;
			for (final Component comp : curr_components)
			{
				if (comp instanceof DeviceButtons)
				{
					buttons = (DeviceButtons) comp;
					methods = ((DeviceButtons) comp).getMethods();
					buttons.setReadOnly(readOnly);
				}
				else if (comp instanceof DeviceComponent)
					device_components.addElement((DeviceComponent) comp);
				else if (comp instanceof DeviceControl)
					device_controls.addElement((DeviceControl) comp);
				if (comp instanceof Container)
					search_stack.push((Container) comp);
			}
		}
		// done
		num_components = device_components.size();
		for (final DeviceComponent comp : device_components)
		{
			comp.setSubtree(subtree);
			comp.configure(baseNid, readOnly);
			final String currUpdateId = comp.getUpdateId(this);
			if (currUpdateId != null && !currUpdateId.equals(""))
			{
				Vector<DeviceComponent> components = updateHash.get(currUpdateId);
				if (components == null)
				{
					components = new Vector<>();
					updateHash.put(currUpdateId, components);
				}
				components.addElement(comp);
			}
		}
		try
		{
			// A second turn in order to carry out actions which need inter-component info
			for (final DeviceComponent comp : device_components)
				comp.postConfigure();
		}
		catch (final Throwable exc)
		{
			System.out.println(exc);
		}
		if (methods != null && methods.length > 0)
		{
			pop_methods = new JPopupMenu("Methods");
			// pop_methods = new JPopupMenu();
			pop_items = new JMenuItem[methods.length];
			for (int i = 0; i < methods.length; i++)
			{
				pop_methods.add(pop_items[i] = new JMenuItem(methods[i]));
				pop_items[i].addActionListener(new ActionListener()
				{
					@Override
					public void actionPerformed(ActionEvent e)
					{
						boolean success = true;
						String errmsg = "";
						int j;
						for (j = 0; j < pop_items.length && ((JMenuItem) e.getSource()) != pop_items[j]; j++);
						if (j == pop_items.length)
							return;
						if (JOptionPane.showConfirmDialog(DeviceSetup.this, "Execute " + methods[j] + "?",
								"Execute a device method", JOptionPane.YES_NO_OPTION,
								JOptionPane.QUESTION_MESSAGE) == JOptionPane.YES_OPTION)
						{
							try
							{
								DeviceSetup.this.subtree.doDeviceMethod(DeviceSetup.this.baseNid, methods[j]);
							}
							catch (final Exception exc)
							{
								errmsg = exc.toString();
								success = false;
							}
							if (!success)
								JOptionPane.showMessageDialog(DeviceSetup.this,
										"Error executing method " + methods[j] + ": " + errmsg,
										"Method execution report", JOptionPane.WARNING_MESSAGE);
							else
								JOptionPane.showMessageDialog(DeviceSetup.this,
										"Method " + methods[j] + " succesfully executed", "Method execution report",
										JOptionPane.INFORMATION_MESSAGE);
						}
					}
				});
			}
			pop_methods.pack();
			addMouseListener(new MouseAdapter()
			{
				@Override
				public void mousePressed(MouseEvent e)
				{
					if ((e.getModifiers() & Event.META_MASK) != 0)
					{
						pop_methods.setInvoker(DeviceSetup.this);
						pop_methods.show(DeviceSetup.this, e.getX(), e.getY());
					}
				}
				/*
				 * public void mouseReleased(MouseEvent e) { if(e.isPopupTrigger())
				 * pop_methods.show(DeviceSetup.this, e.getX(), e.getY()); }
				 */
			});
		}
		popDefault(oldDef);
	}

	@Override
	public void dataChanged(int... nids)
	{
		this.update();
	}

	public void fireUpdate(String id, String val)
	{
		final Vector<DeviceComponent> components = updateHash.get(id);
		if (components != null)
			for (final DeviceComponent comp : components)
				comp.fireUpdate(id, val);
	}

	public String getDeviceProvider()
	{
		DeviceSetupBeanInfo.beanDeviceProvider = deviceProvider;
		return deviceProvider;
	}

	public String getDeviceTitle()
	{ return deviceTitle; }

	public String getDeviceType()
	{
		DeviceSetupBeanInfo.beanDeviceType = deviceType;
		return deviceType;
	}

	public String getUpdateEvent()
	{ return updateEvent; }

	boolean isChanged()
	{
		for (final DeviceComponent comp : device_components)
		{
			if (comp.isChanged())
				return true;
			if (comp.isStateChanged())
				return true;
		}
		return false;
	}

	public boolean isReadOnly()
	{ return readOnly; }

	private final void popDefault(final int oldDef)
	{
		try
		{
			subtree.setDefault(oldDef);
		}
		catch (final Exception exc)
		{
			System.out.println("Error resetting default: " + exc);
		}
	}

	public void propagateData(int offsetNid, Object data)
	{
		for (final DeviceComponent comp : device_components)
			comp.dataChanged(offsetNid, data);
	}

	public void propagateState(int offsetNid, boolean state)
	{
		for (final DeviceComponent comp : device_components)
			comp.stateChanged(offsetNid, state);
	}

	private final int pushDefault()
	{
		int oldDef = 0;
		try
		{
			oldDef = subtree.getDefault();
			subtree.setDefault(baseNid);
		}
		catch (final Exception exc)
		{
			System.out.println("Error setting default: " + exc);
		}
		return oldDef;
	}

	public void reset()
	{
		final int oldDef = pushDefault();
		for (final DeviceComponent comp : device_components)
			comp.reset();
		popDefault(oldDef);
	}

	public void resetNidHash()
	{
		activeNidHash.remove(new Integer(baseNid));
	}

	public void setCancelText(String cancelText)
	{
		if (buttons != null)
			buttons.setCancelText(cancelText);
	}

	public void setDeviceProvider(String deviceProvider)
	{
		this.deviceProvider = deviceProvider;
		DeviceSetupBeanInfo.beanDeviceProvider = deviceProvider;
	}

	public void setDeviceTitle(String deviceTitle)
	{
		this.deviceTitle = deviceTitle;
		// setTitle(deviceTitle);
	}

	public void setDeviceType(String deviceType)
	{
		this.deviceType = deviceType;
		DeviceSetupBeanInfo.beanDeviceType = deviceType;
	}

	public void setHeight(int height)
	{
		final int width = super.getWidth();
		setSize(width, height);
	}

	void setHighlight(boolean isHighlighted, int[] nids)
	{
		for (final DeviceComponent comp : device_components)
		{
			final int nid = comp.getBaseNid() + comp.getOffsetNid();
			for (final int nid2 : nids)
				if (nid2 == nid)
				{
					comp.setHighlight(isHighlighted);
					break;
				}
		}
	}

	public void setReadOnly(final boolean readOnly)
	{
		this.readOnly = readOnly;
		if (buttons != null)
			buttons.setReadOnly(readOnly);
		for (final DeviceControl comp : device_controls)
			comp.setReadOnly(readOnly);
	}

	public void setUpdateEvent(String updateEvent)
	{ this.updateEvent = updateEvent; }

	public void setWidth(int width)
	{
		final int height = super.getHeight();
		setSize(width, height);
	}

	public void update()
	{
		final int oldDef = pushDefault();
		for (final DeviceComponent comp : device_components)
			comp.update();
		popDefault(oldDef);
	}

	public void updateIdentifiers()
	{
		final StringBuffer varExpr = new StringBuffer();
		for (final DeviceComponent comp : device_components)
		{
			final String currId = comp.getIdentifier();
			if (currId != null)
			{
				final String currData = comp.getData();
				if (currData != null)
					varExpr.append("_" + currId + " = " + currData + ";");
				if (comp.getState())
					varExpr.append("_" + currId + "_state = 1; ");
				else
					varExpr.append("_" + currId + "_state = 0; ");
			}
		}
		if (!device_components.isEmpty())
		{
			try
			{
				subtree.getInt(varExpr.toString());
			}
			catch (final Exception exc)
			{}
		}
	}
}

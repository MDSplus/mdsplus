import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.util.*;
import java.awt.*;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

public class DeviceChannel extends DeviceComponent
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	// Keep copied data for channel components: indexed by the offset between this
	// nid and the component nid
	static Hashtable componentHash = new Hashtable();
	public String labelString = null;
	public boolean borderVisible = false;
	public boolean inSameLine = false;
	protected JCheckBox checkB = null;
	protected Vector device_components = null;
	public int lines = 1, columns = 0; // if columns == 0 FlowLayout is assumed
	public String showVal;
	public boolean showState = true;
	private boolean initial_state;
	private boolean reportingChange = false;
	private final JPopupMenu copyPastePopup;
	protected boolean initializing = false;
	protected JPanel componentsPanel;

	public DeviceChannel()
	{
		initializing = true;
		mode = STATE;
		setLayout(new BorderLayout());
		componentsPanel = new JPanel();
		add(checkB = new JCheckBox(), "North");
		checkB.setText(labelString);
		add(componentsPanel, "Center");
		copyPastePopup = new JPopupMenu();
		final JMenuItem copyI = new JMenuItem("Copy");
		copyI.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				copy();
			}
		});
		copyPastePopup.add(copyI);
		final JMenuItem pasteI = new JMenuItem("Paste");
		pasteI.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				paste();
			}
		});
		copyPastePopup.add(pasteI);
		final JMenuItem propagateI = new JMenuItem("Propagate");
		propagateI.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				propagate();
			}
		});
		copyPastePopup.add(propagateI);
		copyPastePopup.pack();
		copyPastePopup.setInvoker(this);
		addMouseListener(new MouseAdapter()
		{
			@Override
			public void mousePressed(MouseEvent e)
			{
				if ((e.getModifiers() & Event.META_MASK) != 0)
				{
					copyPastePopup.setInvoker(DeviceChannel.this);
					copyPastePopup.show(DeviceChannel.this, e.getX(), e.getY());
				}
			}
		});
		initializing = false;
	}

	@Override
	public Component add(Component c)
	{
		if (!initializing)
			return componentsPanel.add(c);
		return super.add(c);
	}

	@Override
	public Component add(Component c, int intex)
	{
		if (!initializing)
			return componentsPanel.add(c);
		return super.add(c);
	}

	@Override
	public Component add(String name, Component c)
	{
		if (!initializing)
			return componentsPanel.add(c);
		return super.add(c);
	}

	private void buildComponentList()
	{
		if (device_components == null)
		{
			device_components = new Vector();
			final java.util.Stack search_stack = new java.util.Stack();
			search_stack.push(this);
			do
			{
				final Component[] curr_components = ((Container) search_stack.pop()).getComponents();
				if (curr_components == null)
					continue;
				for (int i = 0; i < curr_components.length; i++)
				{
					if (curr_components[i] instanceof DeviceComponent)
						device_components.addElement(curr_components[i]);
					else if (curr_components[i] instanceof Container)
						search_stack.push(curr_components[i]);
				}
			}
			while (!search_stack.empty());
		}
	}

	public void copy()
	{
		buildComponentList();
		for (int i = 0; i < device_components.size(); i++)
		{
			final DeviceComponent currComponent = (DeviceComponent) device_components.elementAt(i);
			final int intOffset = currComponent.getOffsetNid() - getOffsetNid();
			componentHash.put(new Integer(intOffset), currComponent.getFullData());
		}
	}

	@Override
	protected void displayData(String data, boolean is_on)
	{
		initial_state = is_on;
		if (checkB != null)
			checkB.setSelected(is_on);
		propagateState(is_on);
	}

	@Override
	public void fireUpdate(String updateId, String newExpr)
	{
		if (updateId == null || !updateIdentifier.equals(updateId))
			return;
		String newVal = newExpr.toString();
		newVal = newVal.substring(1, newVal.length() - 1);
		if (showVal != null && showVal.equals(newVal))
		{
			setEnabledAll(true);
			final LayoutManager layout = getParent().getLayout();
			if (layout != null && (layout instanceof CardLayout))
				((CardLayout) layout).show(getParent(), showVal);
			// Display this component using showVal as constraint
		}
		else
			setEnabledAll(false);
	}

	public boolean getBorderVisible()
	{ return borderVisible; }

	public int getColumns()
	{ return columns; }

	public Container getContainer()
	{
		// JOptionPane.showMessageDialog(null, "GET CONTAINER", "",
		// JOptionPane.INFORMATION_MESSAGE);
		return componentsPanel;
	}

	@Override
	protected String getData()
	{ return null; }

	public boolean getInSameLine()
	{ return inSameLine; }

	public String getLabelString()
	{ return labelString; }

	public int getLines()
	{ return lines; }

	public boolean getShowState()
	{ return showState; }

	public String getShowVal()
	{ return showVal; }

	@Override
	protected boolean getState()
	{
		if (!showState || checkB == null)
			return initial_state;
		else
			return checkB.isSelected();
	}

	@Override
	protected void initializeData(String data, boolean is_on)
	{
		if (!showState)
		{
			remove(checkB);
			checkB = null;
		}
		else
		{
			checkB.setText(labelString);
			checkB.setSelected(is_on);
			checkB.addChangeListener(new ChangeListener()
			{
				@Override
				public void stateChanged(ChangeEvent e)
				{
					reportingChange = true;
					reportStateChanged(checkB.isSelected());
					reportingChange = false;
					propagateState(checkB.isSelected());
				}
			});
		}
		propagateState(is_on);
	}

	public void paste()
	{
		buildComponentList();
		for (int i = 0; i < device_components.size(); i++)
		{
			final DeviceComponent currComponent = (DeviceComponent) device_components.elementAt(i);
			final int intOffset = currComponent.getOffsetNid() - getOffsetNid();
			final Object currData = componentHash.get(new Integer(intOffset));
			if (currData != null)
				currComponent.dataChanged(currComponent.getOffsetNid(), currData);
		}
	}

	@Override
	public void postConfigure()
	{
		propagateState(curr_on);
	}

	public void propagate()
	{
		copy();
		final Container parent = getParent();
		final Component components[] = parent.getComponents();
		for (int i = 0; i < components.length; i++)
		{
			if (components[i] instanceof DeviceChannel && components[i] != this)
			{
				((DeviceChannel) components[i]).paste();
			}
		}
	}

	private void propagateState(boolean state)
	{
		buildComponentList();
		final int size = device_components.size();
		for (int i = 0; i < size; i++)
			((DeviceComponent) device_components.elementAt(i)).setEnabled(state);
	}

	public void setBorderVisible(boolean borderVisible)
	{
		this.borderVisible = borderVisible;
		if (borderVisible)
			// componentsPanel.setBorder(new LineBorder(Color.black, 1));
			setBorder(new LineBorder(Color.black, 1));
		else
			// componentsPanel.setBorder(null);
			setBorder(null);
	}

	public void setColumns(int columns)
	{
		initializing = true;
		this.columns = columns;
		if (lines != 0 && columns != 0)
			componentsPanel.setLayout(new GridLayout(lines, columns));
		else
			componentsPanel.setLayout(new FlowLayout());
		initializing = false;
	}

	@Override
	public void setEnabled(boolean state)
	{
		if (checkB != null)
			checkB.setEnabled(state);
		buildComponentList();
		if (device_components != null)
		{
			final int size = device_components.size();
			for (int i = 0; i < size; i++)
				((DeviceComponent) device_components.elementAt(i)).setEnabled(state);
		}
	}

	protected void setEnabledAll(boolean enabled)
	{
		buildComponentList();
		if (device_components != null)
		{
			final int size = device_components.size();
			for (int i = 0; i < size; i++)
			{
				if (enabled)
					((DeviceComponent) device_components.elementAt(i)).setEnable();
				else
					((DeviceComponent) device_components.elementAt(i)).setDisable();
			}
		}
	}

	public void setInSameLine(boolean inSameLine)
	{
		this.inSameLine = inSameLine;
		if (checkB != null)
		{
			remove(checkB);
			if (inSameLine)
				add(checkB, "West");
			else
				add(checkB, "North");
		}
	}

	public void setLabelString(String labelString)
	{
		this.labelString = labelString;
		if (checkB != null)
			checkB.setText(labelString);
		redisplay();
	}

	@Override
	public void setLayout(LayoutManager layout)
	{
		if (!initializing)
			return;
		super.setLayout(layout);
	} // Do not accept interferences

	public void setLines(int lines)
	{
		initializing = true;
		this.lines = lines;
		if (lines != 0 && columns != 0)
			componentsPanel.setLayout(new GridLayout(lines, columns));
		else if (lines == 0 && columns == 0)
			componentsPanel.setLayout(new FlowLayout());
		else
			componentsPanel.setLayout(new BorderLayout());
		initializing = false;
	}

	public void setShowState(boolean showState)
	{ this.showState = showState; }

	public void setShowVal(String showVal)
	{ this.showVal = showVal; }

	@Override
	protected void stateChanged(int offsetNid, boolean state)
	{
		if (this.offsetNid != offsetNid || reportingChange)
			return;
		if (checkB != null)
			checkB.setSelected(state);
	}

	@Override
	protected boolean supportsState()
	{
		return showState;
	}
}

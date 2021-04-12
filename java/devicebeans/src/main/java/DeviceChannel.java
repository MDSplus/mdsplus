import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;
import javax.swing.border.LineBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class DeviceChannel extends DeviceComponent
{
	// Keep copied data for channel components
	// indexed by the offset between this nid and the component nid
	static Hashtable<Integer, Object> componentHash = new Hashtable<>();
	private static final long serialVersionUID = 1L;
	public boolean borderVisible = false;
	protected JCheckBox checkB = null;
	protected JPanel componentsPanel;
	private final JPopupMenu copyPastePopup;
	protected Vector<DeviceComponent> device_components = null;
	private boolean initial_state;
	protected boolean initializing = false;
	public boolean inSameLine = false;
	public String labelString = null;
	public int lines = 1, columns = 0; // if columns == 0 FlowLayout is assumed
	private boolean reportingChange = false;
	public boolean showState = true;
	public String showVal;

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
			device_components = new Vector<>();
			final Stack<Component> search_stack = new Stack<>();
			search_stack.push(this);
			do
			{
				final Component[] curr_components = ((Container) search_stack.pop()).getComponents();
				if (curr_components == null)
					continue;
				for (final Component curr_component : curr_components)
				{
					if (curr_component instanceof DeviceComponent)
						device_components.addElement((DeviceComponent) curr_component);
					else if (curr_component instanceof Container)
						search_stack.push(curr_component);
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
			final DeviceComponent currComponent = device_components.elementAt(i);
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
		if (showVal != null && showVal.equals(newVal))
		{
			setEnabledAll(true);
			final LayoutManager layout = getParent().getLayout();
			if (layout != null && (layout instanceof CardLayout))
				((CardLayout) layout).show(getParent(), showVal);
		}
		else
			setEnabledAll(false);
	}

	public boolean getBorderVisible()
	{ return borderVisible; }

	public int getColumns()
	{ return columns; }

	public final Container getContainer()
	{ return componentsPanel; }

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
			final DeviceComponent currComponent = device_components.elementAt(i);
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
		for (final Component component2 : components)
		{
			if (component2 instanceof DeviceChannel && component2 != this)
			{
				((DeviceChannel) component2).paste();
			}
		}
	}

	private void propagateState(boolean state)
	{
		buildComponentList();
		final int size = device_components.size();
		for (int i = 0; i < size; i++)
			device_components.elementAt(i).setEnabled(state);
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
				device_components.elementAt(i).setEnabled(state);
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
					device_components.elementAt(i).setEnable();
				else
					device_components.elementAt(i).setDisable();
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

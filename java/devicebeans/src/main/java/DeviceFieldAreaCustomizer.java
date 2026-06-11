import java.awt.BorderLayout;
import java.awt.Button;
import java.awt.Checkbox;
import java.awt.Choice;
import java.awt.GridLayout;
import java.awt.Label;
import java.awt.Panel;
import java.awt.TextField;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.beans.Customizer;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;

public class DeviceFieldAreaCustomizer extends DeviceCustomizer implements Customizer
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	DeviceFieldArea bean = null;
	Object obj;
	PropertyChangeSupport listeners = new PropertyChangeSupport(this);
	TextField identifier, numCols;
	Choice nids;
	Button doneButton;
	Checkbox showState;
	Checkbox textOnly;
	Checkbox editable;
	Checkbox displayEvaluated;

	public DeviceFieldAreaCustomizer()
	{}

	@Override
	public void addPropertyChangeListener(PropertyChangeListener l)
	{
		listeners.addPropertyChangeListener(l);
	}

	@Override
	public void removePropertyChangeListener(PropertyChangeListener l)
	{
		listeners.removePropertyChangeListener(l);
	}

	@Override
	public void setObject(Object o)
	{
		bean = (DeviceFieldArea) o;
		setLayout(new BorderLayout());
		Panel jp = new Panel();
		jp.setLayout(new GridLayout(3, 1));
		Panel jp1 = new Panel();
		jp1.add(textOnly = new Checkbox("Text only: ", bean.getTextOnly()));
		jp1.add(editable = new Checkbox("Editable: ", bean.getEditable()));
		jp1.add(new Label("Offset nid: "));
		jp1.add(nids = new Choice());
		final String names[] = getDeviceFields();
		if (names != null)
			for (int i = 0; i < names.length; i++)
				nids.addItem(names[i]);
		int offsetNid = bean.getOffsetNid();
		if (offsetNid > 0)
			offsetNid--;
		nids.select(offsetNid);
		jp.add(jp1);
		jp1 = new Panel();
		jp1.add(displayEvaluated = new Checkbox("Display Evaluated: ", bean.getDisplayEvaluated()));
		jp.add(jp1);
		add(jp, "Center");
		jp = new Panel();
		jp.add(doneButton = new Button("Apply"));
		doneButton.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				if (bean == null)
					return;
				final boolean oldTextOnly = bean.getTextOnly();
				bean.setTextOnly(textOnly.getState());
				listeners.firePropertyChange("textOnly", oldTextOnly, bean.getTextOnly());
				final boolean oldEditable = bean.getEditable();
				bean.setEditable(editable.getState());
				listeners.firePropertyChange("editable", oldEditable, bean.getEditable());
				final int oldOffsetNid = bean.getOffsetNid();
				bean.setOffsetNid(nids.getSelectedIndex() + 1);
				listeners.firePropertyChange("offsetNid", oldOffsetNid, bean.getOffsetNid());
				final String oldIdentifier = bean.getIdentifier();
				bean.setIdentifier(identifier.getText());
				listeners.firePropertyChange("identifier", oldIdentifier, bean.getIdentifier());
				DeviceFieldAreaCustomizer.this.repaint();
			}
		});
		add(jp, "South");
	}
}

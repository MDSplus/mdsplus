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

public class DeviceInputsCustomizer extends DeviceCustomizer implements Customizer
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	DeviceInputs bean = null;
	Object obj;
	PropertyChangeSupport listeners = new PropertyChangeSupport(this);
	TextField parameterOffsetTF, numParametersTF;
	Choice nids;
	Button doneButton;
	public DeviceInputsCustomizer()
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
		bean = (DeviceInputs) o;
		setLayout(new BorderLayout());
		Panel jp = new Panel();
		jp.setLayout(new GridLayout(1, 1));
		Panel jp1 = new Panel();
		jp.add(jp1);
		jp1 = new Panel();
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
				final int oldOffsetNid = bean.getOffsetNid();
				bean.setOffsetNid(nids.getSelectedIndex() + 1);
				listeners.firePropertyChange("offsetNid", oldOffsetNid, bean.getOffsetNid());
				final String oldIdentifier = bean.getIdentifier();
				DeviceInputsCustomizer.this.repaint();
			}
		});
		add(jp, "South");
	}
}

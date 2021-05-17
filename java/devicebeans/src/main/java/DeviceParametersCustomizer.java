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

public class DeviceParametersCustomizer extends DeviceCustomizer implements Customizer
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	DeviceParameters bean = null;
	Object obj;
	PropertyChangeSupport listeners = new PropertyChangeSupport(this);
	TextField parameterOffsetTF, numParametersTF;
	Choice nids;
	Button doneButton;
	public DeviceParametersCustomizer()
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
		bean = (DeviceParameters) o;
		setLayout(new BorderLayout());
		Panel jp = new Panel();
		jp.setLayout(new GridLayout(2, 1));
		Panel jp1 = new Panel();
		jp1.add(new Label("Parameter Offset: "));
		jp1.add(parameterOffsetTF = new TextField(4));
		final int parameterOffset = bean.getParameterOffset();
		parameterOffsetTF.setText((new Integer(parameterOffset)).toString());
		jp1.add(new Label("Num. Parameters: "));
		jp1.add(numParametersTF = new TextField(4));
		final int numParameters = bean.getNumParameters();
		numParametersTF.setText((new Integer(numParameters)).toString());
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
				final String parameterOffsetStr = parameterOffsetTF.getText();
				final int oldParameterOffset = bean.getParameterOffset();
				bean.setParameterOffset(Integer.parseInt(parameterOffsetStr));
				listeners.firePropertyChange("parameterOffset", oldParameterOffset, bean.getParameterOffset());
				final String numParametersStr = numParametersTF.getText();
				final int oldNumParameters = bean.getNumParameters();
				bean.setNumParameters(Integer.parseInt(numParametersStr));
				listeners.firePropertyChange("numParameters", oldNumParameters, bean.getNumParameters());
				final int oldOffsetNid = bean.getOffsetNid();
				bean.setOffsetNid(nids.getSelectedIndex() + 1);
				listeners.firePropertyChange("offsetNid", oldOffsetNid, bean.getOffsetNid());
				final String oldIdentifier = bean.getIdentifier();
				DeviceParametersCustomizer.this.repaint();
			}
		});
		add(jp, "South");
	}
}

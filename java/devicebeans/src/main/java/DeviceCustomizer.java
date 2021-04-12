import java.awt.Panel;
import java.util.StringTokenizer;
import javax.swing.JOptionPane;

import mds.connection.Descriptor;
import mds.connection.MdsConnection;

public class DeviceCustomizer extends Panel
{
	private static final long serialVersionUID = 1L;
	static String lastDeviceType = null;
	static String lastDeviceProvider = null;
	static String[] lastFields;
	static MdsConnection connection;

	public String[] getDeviceFields()
	{
		if (DeviceSetupBeanInfo.beanDeviceType == null)
		{
			DeviceSetupBeanInfo.beanDeviceType = JOptionPane.showInputDialog("Please define the device type");
		}
		System.out.println("Device type: " + DeviceSetupBeanInfo.beanDeviceType);
		System.out.println("Inquiring Device Provider...");
		if (DeviceSetupBeanInfo.beanDeviceProvider == null)
		{
			DeviceSetupBeanInfo.beanDeviceProvider = JOptionPane
					.showInputDialog("Please define the IP address of the device repository");
		}
		if (lastDeviceType != null && lastDeviceType.equals(DeviceSetupBeanInfo.beanDeviceType))
			return lastFields;
		lastDeviceType = DeviceSetupBeanInfo.beanDeviceType;
		String linFields = "";
		if (connection == null || !connection.getProvider().equals(DeviceSetupBeanInfo.beanDeviceProvider))
		{
			try
			{
				connection = new MdsConnection(DeviceSetupBeanInfo.beanDeviceProvider);
				connection.ConnectToMds(false);
			}
			catch (final Exception exc)
			{
				JOptionPane.showMessageDialog(null,
						"Cannot connect to mdsip server " + DeviceSetupBeanInfo.beanDeviceProvider + ": " + exc);
				connection = null;
				lastDeviceType = null;
			}
		}
		Descriptor linDsc = null;
		try
		{
			linDsc = connection.MdsValue("JavaGetDeviceFields(\"" + DeviceSetupBeanInfo.beanDeviceType + "\")");
			linFields = new String(linDsc.dataToByteArray());
		}
		catch (final Exception exc)
		{
			JOptionPane.showMessageDialog(null, "Cannot retrieve device field names: " + exc);
			lastDeviceType = null;
		}
		final StringTokenizer st = new StringTokenizer(linFields);
		lastFields = new String[st.countTokens()];
		for (int i = 0; i < lastFields.length; i++)
			lastFields[i] = st.nextToken();
		if (lastFields.length == 0) // If name retrieval failed
		{
			JOptionPane.showMessageDialog(null,
					"Unable to retrieve device field names: check deviceType and deviceProvider main form properties");
			lastDeviceType = null;
		}
		return lastFields;
	}
}

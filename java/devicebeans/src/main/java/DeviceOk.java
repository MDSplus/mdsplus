public class DeviceOk extends DeviceControl
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;

	public DeviceOk()
	{
		setText("Ok");
	}

	@Override
	protected void doOperation(DeviceSetup deviceSetup)
	{
		check();
		deviceSetup.apply();
		deviceSetup.cancel();
	}

	@Override
	void setReadOnly(boolean readOnly)
	{
		setEnabled(!readOnly);
	}
}

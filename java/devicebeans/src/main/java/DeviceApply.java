public class DeviceApply extends DeviceControl
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;

	public DeviceApply()
	{
		setText("Apply");
	}

	@Override
	protected void doOperation(DeviceSetup deviceSetup)
	{
		if (check())
			deviceSetup.apply();
	}

	@Override
	void setReadOnly(boolean readOnly)
	{
		setEnabled(!readOnly);
	}
}

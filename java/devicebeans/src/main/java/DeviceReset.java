public class DeviceReset extends DeviceControl
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;

	public DeviceReset()
	{
		setText("Reset");
	}

	@Override
	protected void doOperation(DeviceSetup deviceSetup)
	{
		deviceSetup.reset();
	}
}

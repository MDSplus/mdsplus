public class DeviceCancel extends DeviceControl
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;

	public DeviceCancel()
	{
		setText("Cancel");
	}

	@Override
	protected void doOperation(DeviceSetup deviceSetup)
	{
		deviceSetup.cancel();
	}
}

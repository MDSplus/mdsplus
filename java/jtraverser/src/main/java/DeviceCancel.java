public class DeviceCancel extends DeviceControl
{
    public DeviceCancel()
    {
	setText("Cancel");
    }
    protected void doOperation(DeviceSetup deviceSetup)
    {
	deviceSetup.cancel();
    }
}

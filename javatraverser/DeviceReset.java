public class DeviceReset extends DeviceControl
{
    public DeviceReset()
    {
	setText("Reset");
    }
    protected void doOperation(DeviceSetup deviceSetup)
    {
	deviceSetup.reset();
    }
}
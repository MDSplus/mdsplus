public class DeviceOk extends DeviceControl
{
    public DeviceOk()
    {
        setText("Ok");
    }
    protected void doOperation(DeviceSetup deviceSetup)
    {
        check();
        deviceSetup.apply();
        deviceSetup.dispose();
    }
}

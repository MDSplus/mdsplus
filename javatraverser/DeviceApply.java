public class DeviceApply extends DeviceControl
{
    public DeviceApply()
    {
        setText("Apply");
    }
    protected void doOperation(DeviceSetup deviceSetup)
    {
        check();
        deviceSetup.apply();
    }
    
}

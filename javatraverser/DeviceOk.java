public class DeviceOk extends DeviceControl
{
    public DeviceOk()
    {
	setText("Ok");
    }
    void setReadOnly(boolean readOnly)
    {
	   setEnabled(!readOnly);
    }
    protected void doOperation(DeviceSetup deviceSetup)
    {
	check();
	deviceSetup.apply();
	deviceSetup.cancel();
    }

}

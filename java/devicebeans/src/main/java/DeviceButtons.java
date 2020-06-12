import javax.swing.*;

public class DeviceButtons extends JPanel
{
	private static final long serialVersionUID = 1L;
	protected DeviceControl apply, ok, cancel;
	private String methods[];

	public void setMethods(String methods[])
	{ this.methods = methods; }

	public String[] getMethods()
	{ return methods; }

	public DeviceButtons()
	{
		add(ok = new DeviceOk());
		add(apply = new DeviceApply());
		add(new DeviceReset());
		add(cancel = new DeviceCancel());
	}

	public boolean check()
	{
		return apply.check();
	}

	public void setCheckExpressions(String[] checkExpressions)
	{
		apply.setCheckExpressions(checkExpressions);
		ok.setCheckExpressions(checkExpressions);
	}

	public String[] getCheckExpressions()
	{ return apply.getCheckExpressions(); }

	public void setCheckMessages(String[] checkMessages)
	{
		apply.setCheckMessages(checkMessages);
		ok.setCheckMessages(checkMessages);
	}

	public String[] getCheckMessages()
	{ return apply.getCheckMessages(); }

	public void setCancelText(String cancelText)
	{
		cancel.setText(cancelText);
	}

	public void setReadOnly(boolean readOnly)
	{
		ok.setVisible(!readOnly);
		apply.setVisible(!readOnly);
	}
}

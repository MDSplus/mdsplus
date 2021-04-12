import java.awt.GridLayout;

import javax.swing.JCheckBox;
import javax.swing.JPanel;

public class DeviceDispatchField extends DeviceComponent
{
	private static final long serialVersionUID = 1L;
	protected DeviceLabeledExprEditor ident, phase, when, completion;
	protected String action;
	protected String dispatch;
	protected JCheckBox state;

	@Override
	protected void displayData(String data, boolean is_on)
	{
		try
		{
			dispatch = subtree.execute("DISPATCH_OF(" + data + ")");
		}
		catch (final Exception exc)
		{
			ident.setData("");
			phase.setData("");
			completion.setData("");
			when.setData("");
			return;
		}
		String server;
		try
		{
			server = subtree.getString("IDENT_OF(" + dispatch + ")");
		}
		catch (final Exception exc)
		{
			server = "";
		}
		ident.setData("\"" + server + "\"");
		String phaseStr;
		try
		{
			phaseStr = subtree.getString("PHASE_OF(" + dispatch + ")");
		}
		catch (final Exception exc)
		{
			phaseStr = "";
		}
		phase.setData("\"" + phaseStr + "\"");
		String completionStr;
		try
		{
			completionStr = subtree.getString("COMPLETION_OF(" + dispatch + ")");
		}
		catch (final Exception exc)
		{
			completionStr = "";
		}
		completion.setData("\"" + completionStr + "\"");
		try
		{
			final int sequence = subtree.getInt("WHEN_OF(" + dispatch + ")");
			when.setData("" + sequence);
		}
		catch (final Exception exc)
		{
			String whenStr;
			try
			{
				whenStr = subtree.execute("WHEN_OF(" + dispatch + ")");
			}
			catch (final Exception exc1)
			{
				whenStr = "";
			}
			when.setData(whenStr);
		}
		state.setSelected(is_on);
	}

	@Override
	protected String getData()
	{
		int type;
		try
		{
			subtree.getInt(when.getData());
			type = 2; // Sequential
		}
		catch (final Exception exc)
		{
			type = 3;
		}
		try
		{
			final String retAction = "BUILD_ACTION(BUILD_DISPATCH(" + type + "," + ident.getData() + ","
					+ phase.getData() + "," + when.getData() + "," + completion.getData() + "),"
					+ subtree.execute("TASK_OF(" + action + ")") + ")";
			return retAction;
		}
		catch (final Exception exc)
		{
			return null;
		}
	}

	@Override
	protected boolean getState()
	{ return state.isSelected(); }

	@Override
	protected void initializeData(String data, boolean is_on)
	{
		action = data;
		dispatch = "DISPATCH_OF(" + data + ")";
		setLayout(new GridLayout(4, 1));
		String server;
		try
		{
			server = subtree.getString("IDENT_OF(" + dispatch + ")");
			if (server != null && server.trim().length() > 0)
				server = "\"" + server + "\"";
		}
		catch (final Exception exc)
		{
			server = "";
		}
		add(ident = new DeviceLabeledExprEditor("Ident:         ", new DeviceExprEditor(server, true)));
		String phaseStr;
		try
		{
			phaseStr = subtree.getString("PHASE_OF(" + dispatch + ")");
			if (phaseStr != null && phaseStr.trim().length() > 0)
				phaseStr = "\"" + phaseStr + "\"";
		}
		catch (final Exception exc)
		{
			phaseStr = "";
		}
		add(phase = new DeviceLabeledExprEditor("Phase:        ", new DeviceExprEditor(phaseStr, true)));
		String completionStr;
		try
		{
			completionStr = subtree.getString("COMPLETION_OF(" + dispatch + ")");
			if (completionStr != null && completionStr.trim().length() > 0)
				completionStr = "\"" + completionStr + "\"";
		}
		catch (final Exception exc)
		{
			completionStr = "";
		}
		add(completion = new DeviceLabeledExprEditor("Completion:", new DeviceExprEditor(completionStr, true)));
		final JPanel jp = new JPanel();
		try
		{
			final int sequence = subtree.getInt("WHEN_OF(" + dispatch + ")");
			jp.add(when = new DeviceLabeledExprEditor("Sequence:  ", new DeviceExprEditor("" + sequence, false, 1, 6)));
		}
		catch (final Exception exc)
		{
			String whenStr;
			try
			{
				whenStr = subtree.execute("WHEN_OF(" + dispatch + ")");
			}
			catch (final Exception exc1)
			{
				whenStr = "";
			}
			jp.add(when = new DeviceLabeledExprEditor("When:  ", new DeviceExprEditor(whenStr, false, 1, 6)));
		}
		jp.add(state = new JCheckBox("Is On", is_on));
		add(jp);
	}

	/* Allow writing only if model */
	@Override
	protected boolean isDataChanged()
	{
		try
		{
			if (subtree.getShot() == -1)
				return true;
			else
				return false;
		}
		catch (final Exception exc)
		{
			return false;
		}
	}

	@Override
	public void setEnabled(boolean state)
	{}
}

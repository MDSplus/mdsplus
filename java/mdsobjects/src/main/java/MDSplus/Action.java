package MDSplus;

/**
 * Class for DTYPE_ACTION
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.21
 */
public class Action extends Compound
{
	public Action(Data dispatch, Data task, Data errorLog, Data completionMessage, Data performance, Data help,
			Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_ACTION;
		descs = new Data[]
		{ dispatch, task, errorLog, completionMessage, performance };
		opcode = 0;
	}

	public Action(Data dispatch, Data task, Data errorLog, Data completionMessage, Data performance)
	{
		this(dispatch, task, errorLog, completionMessage, performance, null, null, null, null);
	}

	public Action(Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_ACTION;
		descs = new Data[5];
		opcode = 0;
	}

	public Action()
	{
		this(null, null, null, null);
	}

	public static Action getData(Data help, Data units, Data error, Data validation)
	{
		return new Action(help, units, error, validation);
	}

	public Data getDispatch()
	{
		try
		{
			return descs[0];
		}
		catch (final Exception exc)
		{
			return null;
		}
	}

	public Data getTask()
	{
		try
		{
			return descs[1];
		}
		catch (final Exception exc)
		{
			return null;
		}
	}

	public Data getErrorLog()
	{
		try
		{
			return descs[2];
		}
		catch (final Exception exc)
		{
			return null;
		}
	}

	public Data getCompletionMessage()
	{
		try
		{
			return descs[3];
		}
		catch (final Exception exc)
		{
			return null;
		}
	}

	public Data getPerformance()
	{
		try
		{
			return descs[4];
		}
		catch (final Exception exc)
		{
			return null;
		}
	}

	/**
	 *
	 * @param data
	 */
	public void setDispatch(Data data)
	{
		resizeDescs(1);
		descs[0] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setTask(Data data)
	{
		resizeDescs(2);
		descs[1] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setErrorLog(Data data)
	{
		resizeDescs(3);
		descs[2] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setCompletionMessage(Data data)
	{
		resizeDescs(4);
		descs[3] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setPerformance(Data data)
	{
		resizeDescs(5);
		descs[4] = data;
	}
}

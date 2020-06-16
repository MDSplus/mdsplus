package MDSplus;

/**
 * Class for DTYPE_PROCEDURE
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.40
 */
public class Procedure extends Compound
{
	public Procedure(Data timeout, Data language, Data procedure, Data[] args, Data help, Data units, Data error,
			Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_PROCEDURE;
		descs = new Data[3 + args.length];
		descs[0] = timeout;
		descs[1] = language;
		descs[2] = procedure;
		for (int i = 0; i < args.length; i++)
			descs[3 + i] = args[i];
	}

	public Procedure(Data timeout, Data language, Data procedure, Data[] args)
	{
		this(timeout, language, procedure, args, null, null, null, null);
	}

	public Procedure(Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_PROCEDURE;
		descs = new Data[3];
	}

	public static Procedure getData(Data help, Data units, Data error, Data validation)
	{
		return new Procedure(help, units, error, validation);
	}

	public Data[] getArguments()
	{
		final Data[] args = new Data[descs.length - 3];
		for (int i = 0; i < args.length; i++)
			args[i] = descs[3 + i];
		return args;
	}

	public Data getTimeout()
	{
		resizeDescs(1);
		return descs[0];
	}

	public Data getLanguage()
	{
		resizeDescs(2);
		return descs[1];
	}

	public Data getProcedure()
	{
		resizeDescs(3);
		return descs[2];
	}

	/**
	 *
	 * @param idx
	 */
	public Data getArgumentAt(int idx)
	{
		resizeDescs(4 + idx);
		return descs[3 + idx];
	}

	/**
	 *
	 * @param data
	 */
	public void setArguments(Data[] args)
	{
		resizeDescs(3 + args.length);
		for (int i = 0; i < args.length; i++)
			descs[3 + i] = args[i];
	}

	/**
	 *
	 * @param data
	 */
	public void setTimeout(Data data)
	{
		resizeDescs(1);
		descs[0] = data;
	}

	/**
	 *
	 * @param arg
	 * @param idx
	 */
	public void setArgumentAt(Data arg, int idx)
	{
		resizeDescs(3 + idx + 1);
		descs[3 + idx] = arg;
	}

	public void setLanguage(Data data)
	{
		resizeDescs(2);
		descs[1] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setProcedure(Data data)
	{
		resizeDescs(3);
		descs[2] = data;
	}
}

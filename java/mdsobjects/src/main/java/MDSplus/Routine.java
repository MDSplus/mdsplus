package MDSplus;

/**
 * Class for DTYPE_ROUTINE
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.41
 */
public class Routine extends Compound
{
	public Routine(Data timeout, Data image, Data routine, Data[] args, Data help, Data units, Data error,
			Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_ROUTINE;
		descs = new Data[3 + args.length];
		descs[0] = timeout;
		descs[1] = image;
		descs[2] = routine;
		for (int i = 0; i < args.length; i++)
			descs[3 + i] = args[i];
	}

	public Routine(Data timeout, Data image, Data routine, Data[] args)
	{
		this(timeout, image, routine, args, null, null, null, null);
	}

	public Routine(Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_ROUTINE;
		descs = new Data[3];
	}

	public static Routine getData(Data help, Data units, Data error, Data validation)
	{
		return new Routine(help, units, error, validation);
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

	public Data getImage()
	{
		resizeDescs(2);
		return descs[1];
	}

	public Data getRoutine()
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

	public void setImage(Data data)
	{
		resizeDescs(2);
		descs[1] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setRoutine(Data data)
	{
		resizeDescs(3);
		descs[2] = data;
	}
}

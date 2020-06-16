package MDSplus;

/**
 * Class for DTYPE_CALL
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.33
 */
public class Call extends Compound
{
	/**
	 * Data type argument, used to specify the return type of the called routine
	 */
	public Call(int retType, Data image, Data routine, Data[] args, Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		opcode = retType;
		clazz = CLASS_R;
		dtype = DTYPE_CALL;
		descs = new Data[2 + args.length];
		descs[0] = image;
		descs[1] = routine;
		for (int i = 0; i < args.length; i++)
			descs[2 + i] = args[i];
	}

	public Call(int retType, Data image, Data routine, Data[] args)
	{
		this(retType, image, routine, args, null, null, null, null);
	}

	public Call(Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_CALL;
		descs = new Data[0];
		opcode = 0;
	}

	public static Call getData(Data help, Data units, Data error, Data validation)
	{
		return new Call(help, units, error, validation);
	}

	public Data getImage()
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

	public Data getRoutine()
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

	public Data[] getArguments()
	{
		final Data[] args = new Data[descs.length - 2];
		for (int i = 0; i < descs.length - 2; i++)
			args[i] = descs[2 + i];
		return args;
	}

	/**
	 *
	 * @param idx
	 */
	public Data getArgumentAt(int idx)
	{
		try
		{
			return descs[2 + idx];
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
	public void setImage(Data data)
	{
		resizeDescs(1);
		descs[0] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setRoutine(Data data)
	{
		resizeDescs(2);
		descs[1] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setArguments(Data[] args)
	{
		resizeDescs(2 + args.length);
		final Data newDescs[] = new Data[2 + args.length];
		newDescs[0] = descs[0];
		newDescs[1] = descs[1];
		for (int i = 0; i < args.length; i++)
			newDescs[2 + i] = args[i];
		descs = newDescs;
	}

	/**
	 *
	 * @param idx
	 * @param data
	 */
	public void setArgumentAt(int idx, Data data)
	{
		resizeDescs(2 + idx + 1);
		descs[2 + idx] = data;
	}

	public int getRetType()
	{ return opcode; }

	/**
	 *
	 * @param dtype
	 */
	public void setRetType(int dtype)
	{ opcode = dtype; }
}

package MDSplus;

/**
 * Class for DTYPE_METHODS
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.39
 */
public class Method extends Compound
{
	public Method(Data timeout, Data method, Data object, Data[] args, Data help, Data units, Data error,
			Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_METHOD;
		descs = new Data[3 + args.length];
		descs[0] = timeout;
		descs[1] = method;
		descs[2] = object;
		for (int i = 0; i < args.length; i++)
			descs[3 + i] = args[i];
	}

	public Method(Data timeout, Data method, Data object, Data[] args)
	{
		this(timeout, method, object, args, null, null, null, null);
	}

	public Method(Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_METHOD;
		descs = new Data[3];
	}

	public static Method getData(Data help, Data units, Data error, Data validation)
	{
		return new Method(help, units, error, validation);
	}

	public Data getMethod()
	{
		resizeDescs(2);
		return descs[1];
	}

	public Data getObject()
	{
		resizeDescs(3);
		return descs[2];
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
	public void setMethod(Data data)
	{
		resizeDescs(1);
		descs[1] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setObject(Data data)
	{
		resizeDescs(2);
		descs[2] = data;
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

	public static void main(java.lang.String args[])
	{
		final Data m = new Method(null, new String("INITY"), new String("OG"), new Data[0]);
		System.out.println(m);
	}
}

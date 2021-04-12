package MDSplus;

/**
 * Class for DTYPE_FUNCTION
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.37
 */
public class Function extends Compound
{
	public Function(int opcode, Data[] args, Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		this.opcode = opcode;
		clazz = CLASS_R;
		dtype = DTYPE_FUNCTION;
		descs = new Data[args.length];
		for (int i = 0; i < args.length; i++)
			descs[i] = args[i];
	}

	public Function(int opcode, Data[] args)
	{
		this(opcode, args, null, null, null, null);
	}

	public Function(Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_FUNCTION;
		descs = new Data[0];
		opcode = 0;
	}

	public static Function getData(Data help, Data units, Data error, Data validation)
	{
		return new Function(help, units, error, validation);
	}

	public Data[] getArguments()
	{ return descs; }

	/**
	 *
	 * @param idx
	 */
	public Data getArgumentAt(int idx)
	{
		resizeDescs(idx + 1);
		return descs[idx];
	}

	/**
	 *
	 * @param data
	 */
	public void setArguments(Data[] args)
	{
		descs = new Data[args.length];
		for (int i = 0; i < args.length; i++)
			descs[i] = args[i];
	}

	/**
	 *
	 * @param idx
	 * @param arg
	 */
	public void setArgumentAt(int idx, Data arg)
	{
		resizeDescs(idx + 1);
		descs[idx] = arg;
	}

	/**
	 * Expose function opcode
	 */
	@Override
	public int getOpcode()
	{ return opcode; }
}

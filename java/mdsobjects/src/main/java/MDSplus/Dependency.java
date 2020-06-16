package MDSplus;

/**
 * Class for DTYPE_DEPENDENCY
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.35
 */
public class Dependency extends Compound
{
	public static final int TreeDEPENDENCY_AND = 10;
	public static final int TreeDEPENDENCY_OR = 11;

	public Dependency(int opcode, Data arg1, Data arg2, Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		this.opcode = opcode;
		clazz = CLASS_R;
		dtype = DTYPE_DEPENDENCY;
		descs = new Data[2];
		descs[0] = arg1;
		descs[1] = arg2;
	}

	public Dependency(int opcode, Data arg1, Data arg2)
	{
		this(opcode, arg1, arg2, null, null, null, null);
	}

	public Dependency(Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_DEPENDENCY;
		descs = new Data[2];
		opcode = 0;
	}

	public static Dependency getData(Data help, Data units, Data error, Data validation)
	{
		return new Dependency(help, units, error, validation);
	}

	public Data getArg1()
	{
		resizeDescs(1);
		return descs[0];
	}

	public Data getArg2()
	{
		resizeDescs(2);
		return descs[1];
	}

	/**
	 * @param data
	 */
	public void setArg1(Data data)
	{
		resizeDescs(1);
		descs[0] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setArg2(Data data)
	{
		resizeDescs(2);
		descs[1] = data;
	}
}

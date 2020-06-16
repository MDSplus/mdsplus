package MDSplus;

/**
 * Class for DTYPE_DISPATCH
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.36
 */
public class Dispatch extends Compound
{
	public static final int SCHED_ASYNCH = 1, SCHED_SEQ = 2, SCHED_COND = 3;

	public Dispatch(Data ident, Data phase, Data when, Data completion, Data help, Data units, Data error,
			Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_DISPATCH;
		descs = new Data[]
		{ ident, phase, when, completion };
	}

	public Dispatch(Data ident, Data phase, Data when, Data completion)
	{
		this(ident, phase, when, completion, null, null, null, null);
	}

	public Dispatch(int opcode, Data ident, Data phase, Data when, Data completion)
	{
		this(ident, phase, when, completion, null, null, null, null);
		this.opcode = opcode;
	}

	public Dispatch()
	{
		this(null, null, null, null);
	}

	public static Dispatch getData(Data help, Data units, Data error, Data validation)
	{
		return new Dispatch(null, null, null, null, help, units, error, validation);
	}

	public Data getIdent()
	{
		resizeDescs(1);
		return descs[0];
	}

	public Data getPhase()
	{
		resizeDescs(2);
		return descs[1];
	}

	public Data getWhen()
	{
		resizeDescs(3);
		return descs[2];
	}

	public Data getCompletion()
	{
		resizeDescs(4);
		return descs[3];
	}

	/**
	 *
	 * @param data
	 */
	public void setIdent(Data data)
	{
		resizeDescs(1);
		descs[0] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setPhase(Data data)
	{
		resizeDescs(2);
		descs[1] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setWhen(Data data)
	{
		resizeDescs(3);
		descs[2] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setCompletion(Data data)
	{
		resizeDescs(4);
		descs[3] = data;
	}

	@Override
	public int getOpcode()
	{ return opcode; }
}

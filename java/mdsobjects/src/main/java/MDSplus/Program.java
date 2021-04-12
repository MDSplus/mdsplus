package MDSplus;

/**
 * Class for DTYPE_PROGRAM
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.41
 */
public class Program extends Compound
{
	public Program(Data timeout, Data program, Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_PROGRAM;
		descs = new Data[]
		{ timeout, program };
	}

	public Program(Data timeout, Data program)
	{
		this(timeout, program, null, null, null, null);
	}

	public Program(Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_PROGRAM;
		descs = new Data[2];
	}

	public Program()
	{
		this(null, null, null, null);
	}

	public static Program getData(Data help, Data units, Data error, Data validation)
	{
		return new Program(help, units, error, validation);
	}

	public Data getProgram()
	{
		resizeDescs(2);
		return descs[1];
	}

	public Data getTimeout()
	{
		resizeDescs(1);
		return descs[0];
	}

	/**
	 *
	 * @param data
	 */
	public void setProgram(Data data)
	{
		resizeDescs(2);
		descs[1] = data;
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
}

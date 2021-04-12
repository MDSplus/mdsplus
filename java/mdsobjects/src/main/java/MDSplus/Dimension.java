package MDSplus;

/**
 * Class for DTYPE_DIMENSION
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.36
 */
public class Dimension extends Compound
{
	public Dimension(Data window, Data axis, Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_DIMENSION;
		descs = new Data[]
		{ window, axis };
		opcode = 0;
	}

	public Dimension(Data window, Data axis)
	{
		this(window, axis, null, null, null, null);
	}

	public Dimension(Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_DIMENSION;
		descs = new Data[2];
	}

	public Dimension()
	{
		this(null, null, null, null);
	}

	public static Dimension getData(Data help, Data units, Data error, Data validation)
	{
		return new Dimension(help, units, error, validation);
	}

	public Data getWindow()
	{
		resizeDescs(1);
		return descs[0];
	}

	public Data getAxis()
	{
		resizeDescs(2);
		return descs[1];
	}

	/**
	 *
	 * @param data
	 */
	public void setWindow(Data data)
	{
		resizeDescs(1);
		descs[0] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setAxis(Data data)
	{
		resizeDescs(2);
		descs[1] = data;
	}
}

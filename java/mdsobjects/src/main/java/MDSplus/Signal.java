package MDSplus;

/**
 * Data for DTYPE_SIGNAL
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.42
 */
public class Signal extends Compound
{
	public Signal(Data data, Data raw, Data[] dimensions, Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_SIGNAL;
		descs = new Data[2 + dimensions.length];
		descs[0] = data;
		descs[1] = raw;
		for (int i = 0; i < dimensions.length; i++)
			descs[2 + i] = dimensions[i];
	}

	public Signal(Data data, Data raw, Data[] dimensions)
	{
		this(data, raw, dimensions, null, null, null, null);
	}

	public Signal(Data data, Data raw, Data dimension)
	{
		this(data, raw, new Data[]
		{ dimension }, null, null, null, null);
	}

	public Signal(Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_SIGNAL;
		descs = new Data[2];
	}

	public static Signal getData(Data help, Data units, Data error, Data validation)
	{
		return new Signal(help, units, error, validation);
	}

	/**
	 *
	 * @param raw
	 */
	public void setData(Data data)
	{
		resizeDescs(1);
		descs[0] = data;
	}

	public Data getData()
	{
		resizeDescs(1);
		return descs[0];
	}

	/**
	 *
	 * @param raw
	 */
	public void setRaw(Data raw)
	{
		resizeDescs(2);
		descs[1] = raw;
	}

	public Data getRaw()
	{
		resizeDescs(2);
		return descs[1];
	}

	/**
	 *
	 * @param data
	 */
	public Data[] getDimensions()
	{
		resizeDescs(3);
		final Data[] dimensions = new Data[descs.length - 2];
		for (int i = 0; i < dimensions.length; i++)
			dimensions[i] = descs[2 + i];
		return dimensions;
	}

	/**
	 *
	 * @param idx
	 */
	@Override
	public Data getDimensionAt(int idx)
	{
		resizeDescs(3 + idx);
		return descs[2 + idx];
	}

	/**
	 *
	 * @param data
	 */
	public void setDimensions(Data[] dims)
	{
		resizeDescs(2 + dims.length);
		for (int i = 0; i < dims.length; i++)
			descs[2 + i] = dims[i];
	}

	/**
	 *
	 * @param arg
	 * @param idx
	 */
	public void setDimensionAt(Data dim, int idx)
	{
		resizeDescs(2 + idx + 1);
		descs[2 + idx] = dim;
	}
}

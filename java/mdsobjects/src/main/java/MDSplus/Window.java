package MDSplus;

/**
 * Class for DTYPE_WINDOW
 *
 * @author manduchi
 * @version 1.0
 * @updated 03-ott-2008 12.23.45
 */
public class Window extends Compound
{
	public Window(Data startidx, Data endidx, Data timeat0, Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_WINDOW;
		descs = new Data[]
		{ startidx, endidx, timeat0 };
	}

	public Window(Data startidx, Data endidx, Data timeat0)
	{
		this(startidx, endidx, timeat0, null, null, null, null);
	}

	public Window(Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_R;
		dtype = DTYPE_WINDOW;
		descs = new Data[3];
	}

	public Window()
	{
		this(null, null, null, null);
	}

	public static Window getData(Data help, Data units, Data error, Data validation)
	{
		return new Window(help, units, error, validation);
	}

	public Data getStartIdx()
	{
		resizeDescs(1);
		return descs[0];
	}

	public Data getEndIdx()
	{
		resizeDescs(2);
		return descs[1];
	}

	public Data getValueAt0()
	{
		resizeDescs(3);
		return descs[2];
	}

	/**
	 *
	 * @param data
	 */
	public void setStartIdx(Data data)
	{
		resizeDescs(1);
		descs[0] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setEndIdx(Data data)
	{
		resizeDescs(2);
		descs[1] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setValueAt0(Data data)
	{
		resizeDescs(3);
		descs[2] = data;
	}
}

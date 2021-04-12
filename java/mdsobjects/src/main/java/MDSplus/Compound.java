package MDSplus;

/**
 * Compound is the common superclass for all CLASS_R types. Its fields contain
 * all the required information (Descriptor array, keyword). Its getter/setter
 * methods allow to read/replace descriptors, based on their index. Derived
 * classes will only define methods with appropriate names for reading/writing
 * descriptors (i.e. Data objects).
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.34
 */
public class Compound extends Data
{
	/**
	 * Opcode, used only by some derived classes.
	 */
	int opcode;
	/**
	 * The data (descriptor) array.
	 */
	protected Data[] descs;

	public Compound(Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
	}

	public int getNumDescs()
	{ return descs.length; }

	/**
	 *
	 * @param idx
	 */
	public Data getDescAt(int idx)
	{
		return descs[idx];
	}

	/**
	 *
	 * @param idx
	 */
	public void setDescAt(int idx, Data desc)
	{
		descs[idx] = desc;
	}

	public Data[] getDescs()
	{ return descs; }

	/**
	 *
	 * @param dscs
	 */
	public void setDescs(Data[] dscs)
	{ this.descs = dscs; }

	protected void resizeDescs(int newDim)
	{
		if (descs == null)
		{
			descs = new Data[newDim];
		}
		else
		{
			if (newDim <= descs.length)
				return;
			final Data newDescs[] = new Data[newDim];
			for (int i = 0; i < descs.length; i++)
				newDescs[i] = descs[i];
			descs = newDescs;
		}
	}

	@Override
	public void setCtxTree(Tree ctxTree)
	{
		this.ctxTree = ctxTree;
		for (int i = 0; i < descs.length; i++)
		{
			if (descs[i] != null)
				descs[i].setCtxTree(ctxTree);
		}
	}

	public int getOpcode()
	{ return opcode; }
}

package MDSplus;

/**
 * Class for CLASS_APD, DTYPE_DSC
 *
 * @author manduchi
 * @version 1.0
 * @updated 03-ott-2008 12.23.37
 */
public class Apd extends Data
{
	protected Data[] descs;
	protected int nDescs = 0;
	final static int CHUNK_SIZE = 64;

	public Apd()
	{
		this(new Data[0]);
	}

	public Apd(Data[] descs)
	{
		this(descs, null, null, null, null);
	}

	public Apd(Data[] descs, Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_APD;
		dtype = DTYPE_DSC;
		this.descs = descs;
		nDescs = descs.length;
	}

	public static Data getData(Data[] descs, Data help, Data units, Data error, Data validation)
	{
		return new Apd(descs, help, units, error, validation);
	}

	public Data[] getDescs()
	{
		final Data[] retDescs = new Data[nDescs];
		for (int i = 0; i < nDescs; i++)
			retDescs[i] = descs[i];
		return retDescs;
	}

	/**
	 *
	 * @param descs
	 */
	public void setDescs(Data[] inDescs)
	{
		descs = new Data[inDescs.length];
		for (int i = 0; i < inDescs.length; i++)
			descs[i] = inDescs[i];
		nDescs = descs.length;
	}

	/**
	 *
	 * @param idx
	 */
	public Data getDescAt(int idx)
	{
		return descs[idx];
	}
        public int[] getShape() 
        {
            return new int[]{descs.length};
        }
	protected void resizeDescs(int newDim)
	{
		if (descs == null)
		{
			descs = new Data[CHUNK_SIZE];
		}
		else
		{
			if (newDim <= descs.length)
				return;
			final Data newDescs[] = new Data[(newDim / CHUNK_SIZE) * CHUNK_SIZE + CHUNK_SIZE];
			for (int i = 0; i < nDescs; i++)
				newDescs[i] = descs[i];
			descs = newDescs;
		}
	}

	/**
	 *
	 * @param idx
	 * @param dsc
	 */
	public void setDescAt(int idx, Data dsc)
	{
		resizeDescs(idx + 1);
		descs[idx] = dsc;
                nDescs = idx+1;
	}
        
        
        public static void main(java.lang.String args[])
        {
            try {
                MDSplus.Tree t = new MDSplus.Tree("test", -1);
                MDSplus.Apd apd = new MDSplus.Apd();
                MDSplus.Apd apd1 = new MDSplus.Apd();
                apd.setDescAt(0, new MDSplus.Int32(123));
                apd.setDescAt(2, new MDSplus.Float32(321));
                MDSplus.TreeNode n = t.getNode("numeric");
                apd1.setDescAt(0, new MDSplus.String("CICCIOBELLO"));
                apd.setDescAt(3, apd1);
                n.putData(apd);
                System.out.println("Written: "+apd);
                MDSplus.Apd apd3 = (MDSplus.Apd)n.getData();
                System.out.println("Read: "+apd3);
            }catch(Exception exc) 
            {
                System.out.println(exc);
            }
        }
}

package MDSplus;

public class Dictionary extends Apd
{
	public Dictionary()
	{
		this(new Data[0]);
	}

	public Dictionary(Data[] descs)
	{
		super(descs, null, null, null, null);
                dtype = DTYPE_DICTIONARY;
	}

	public Dictionary(Data[] descs, Data help, Data units, Data error, Data validation)
	{
		super(descs, help, units, error, validation);
 		dtype = DTYPE_DICTIONARY;
	}
        public int[] getShape() 
        {
            return new int[]{descs.length/2};
        }

	public static Data getData(Data[] descs, Data help, Data units, Data error, Data validation)
	{
		return new Dictionary(descs, help, units, error, validation);
	}

	public int len()
	{
		return nDescs / 2;
	}

	public Data getItem(String key)
	{
		for (int i = 0; i < len(); i++)
		{
			if (key.equals(descs[2 * i]))
				return descs[2 * i + 1];
		}
		return null;
	}

	public void setItem(String key, Data data)
	{
		for (int i = 0; i < len(); i++)
		{
			if (key.equals(descs[2 * i]))
			{
				descs[2 * i + 1] = data;
				return;
			}
		}
		resizeDescs(nDescs + 2);
		descs[nDescs] = key;
		descs[nDescs + 1] = data;
		nDescs += 2;
	}
        public static void main(java.lang.String args[])
        {
            try {
                Dictionary d = new Dictionary();
                System.out.println("dtype: " + d.dtype);
                d.setItem(new MDSplus.String("ciccio"), new Int32(1));
                d.setItem(new MDSplus.String("bombo"), new Int32(-1));
                Tree t = new Tree("test", -1);
                TreeNode n = t.getNode("numeric");
                n.putData(d);
                System.out.println("Written: " + d+"  dtype: "+d.dtype);
                Dictionary d1 = (Dictionary)n.getData();
                System.out.println("Read: "+ d1);
                System.out.println("ciccio: " + d1.getItem(new MDSplus.String("ciccio")));
                System.out.println("bombo: " + d1.getItem(new MDSplus.String("bombo")));
                System.out.println("cacca: " + d1.getItem(new MDSplus.String("cacca")));
                
            } catch(Exception exc)
            {
                System.out.println(exc);
            }
        }
}

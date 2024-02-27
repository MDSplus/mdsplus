package MDSplus;

public class List extends Apd
{
	public List()
	{
		this(new Data[0]);
	}

	public List(Data[] descs)
	{
		super(descs, null, null, null, null);
	}

	public List(Data[] descs, Data help, Data units, Data error, Data validation)
	{
		super(descs, help, units, error, validation);
		dtype = DTYPE_LIST;
	}

	public static Data getData(Data[] descs, Data help, Data units, Data error, Data validation)
	{
		return new List(descs, help, units, error, validation);
	}

	public int len()
	{
		return nDescs;
	}

	public void append(Data data)
	{
		setDescAt(len(), data);
	}

	public void remove(Data data)
	{
		for (int i = 0; i < nDescs; i++)
		{
			if (descs[i].equals(data))
			{
				for (int j = i; j < nDescs - 1; j++)
					descs[j] = descs[j + 1];
				nDescs--;
			}
		}
	}

	public void remove(int idx)
	{
		if (idx < 0 || idx >= nDescs)
			return;
		for (int j = idx; j < nDescs - 1; j++)
			descs[j] = descs[j + 1];
		nDescs--;
	}

	public void insert(int idx, Data data) throws MdsException
	{
		if (idx < 0 || idx > nDescs)
			throw new MdsException("Index out of range in List.insert()");
		resizeDescs(nDescs + 1);
		for (int i = nDescs; i > idx; i--)
			descs[i] = descs[i - 1];
		descs[idx] = data;
		nDescs++;
	}

	public Data getElementAt(int idx)
	{
		if (idx < 0 || idx >= nDescs)
			return null;
		return descs[idx];
	}
}

package MDSplus;

public class Dictionary extends Apd
{
    public Dictionary()
    {
	this(new Data[0]);
    }
    public Dictionary(Data [] descs)
    {
	super(descs, null, null, null, null);
    }
    public Dictionary(Data [] descs, Data help, Data units, Data error, Data validation)
    {
	super(descs, help, units, error, validation);
	dtype = DTYPE_DICTIONARY;
    }

    public static Data getData(Data [] descs, Data help, Data units, Data error, Data validation)
    {
	return new Dictionary(descs, help, units, error, validation);
    }
    public int len()
    {
	return nDescs/2;
    }
    public Data getItem(String key)
    {
	for(int i = 0; i < len(); i++)
	{
	    if(key.equals(descs[2*i]))
	        return descs[2*i+1];
	}
	return null;
    }

    public void setItem(String key, Data data)
    {
	for(int i = 0; i < len(); i++)
	{
	    if(key.equals(descs[2*i]))
	    {
	        descs[2*i+1] = data;
	        return;
	    }
	}
	resizeDescs(nDescs+2);
	descs[nDescs] = key;
	descs[nDescs+1] = data;
	nDescs+=2;
    }
}
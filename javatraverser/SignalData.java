//package jTraverser;

public class SignalData extends CompoundData
{
    public static Data getData() {return new SignalData(); }
    public SignalData() {dtype = DTYPE_SIGNAL; }
    public SignalData(Data data, Data raw, Data [] dimensions)
    {
	int ndescs;
	dtype = DTYPE_SIGNAL;
	ndescs = 2 + dimensions.length;
	descs = new Data[ndescs];
	descs[0] = data;
	descs[1] = raw;
	if(dimensions != null)
	{
	    for(int i = 0; i < dimensions.length; i++)
		descs[2+i] = dimensions[i];
	}
    }
    public final Data getDatum() {return descs[0]; }
    public final Data getRaw()  {return descs[1]; }
    public final Data getDimension(int idx)
    {
	if(idx > descs.length - 2)
	    return null;
	return descs[idx + 2];
    }
}
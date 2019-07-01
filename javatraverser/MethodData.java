//package jTraverser;

public class MethodData extends CompoundData
{
    public static Data getData() {return new MethodData(); }
    public MethodData() {dtype = DTYPE_METHOD; }
    public MethodData(Data timeout, Data method, Data object,
	Data [] arguments)
    {
	int ndescs;
	dtype = DTYPE_METHOD;
	if(arguments != null)
	    ndescs = 3 + arguments.length;
	else
	    ndescs = 3;

	descs = new Data[ndescs];
	descs[0] = timeout;
	descs[1] = method;
	descs[2] = object;
	for(int i = 3; i < ndescs; i++)
	    descs[i] = arguments[i-3];
    }

    public final Data getTimeout() {return descs[0]; }
    public final Data getMethod() {return descs[1]; }
    public final Data getDevice() {return descs[2]; }
    public final Data [] getArguments()
    {
	Data[]ris = new Data[descs.length-3];
	for(int i = 0; i < descs.length-3; i++)
	    ris[i] = descs[3+i];
	return ris;
    }
}
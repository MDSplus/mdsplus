//package jTraverser;

public class CallData extends CompoundData
{
    int type;
    public static Data getData() {return new CallData(); }
    public CallData() {dtype = DTYPE_CALL; }
    public CallData(int type, Data image, Data routine, Data[] arguments)
    {
	int ndescs;
	dtype = DTYPE_CALL;
	this.type = type;
	if(arguments != null)
	    ndescs = 2 + arguments.length;
	else
	    ndescs = 2;
	descs = new Data[ndescs];
	descs[0] = image;
	descs[1] = routine;
	for(int i = 2; i < ndescs; i++)
	    descs[i] = arguments[i-2];
    }

    public final int getType() {return type; }
    public final Data getImage() {return descs[0]; }
    public final Data getRoutine() {return descs[1]; }
    public final Data [] getArguments()
    {
	Data ris [] = new Data[descs.length - 2];
	for(int i = 0; i < descs.length - 2; i++)
	    ris[i] = descs[i+2];
	return ris;
    }
}
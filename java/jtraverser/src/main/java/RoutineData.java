//package jTraverser;

public class RoutineData extends CompoundData
{
    public static Data getData() {return new RoutineData(); }
    public RoutineData() {dtype = DTYPE_ROUTINE; }
    public RoutineData(Data time_out, Data image, Data routine, Data [] arguments)
    {
	int ndescs;
	dtype = DTYPE_ROUTINE;
	if(arguments != null)
	    ndescs = 3 + arguments.length;
	else
	    ndescs = 3;

	descs = new Data[ndescs];
	descs[0] = time_out;
	descs[1] = image;
	descs[2] = routine;
	for(int i = 3; i < ndescs; i++)
	    descs[i] = arguments[i-3];
    }

    public final Data getTimeout() {return descs[0]; }
    public final Data getImage()   {return descs[1]; }
    public final Data getRoutine() {return descs[2]; }
    public final Data [] getArguments()
    {
	Data ris[] = new Data[descs.length-3];
	for(int i = 0; i < descs.length-3; i++)
	    ris[i] = descs[3+i];
	return ris;
    }
}
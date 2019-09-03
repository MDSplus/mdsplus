//package jTraverser;

public class WithErrorData extends CompoundData
{
    public static Data getData() {return new WithErrorData(); }
    public WithErrorData() {dtype = DTYPE_WITH_ERROR; }
    public WithErrorData(Data data, Data error)
    {
	dtype = DTYPE_WITH_ERROR;
	descs = new Data[2];
	descs[0] = data;
	descs[1] = error;
    }

    public final Data getDatum() {return descs[0]; }
    public final Data getErrror() {return descs[1]; }
}
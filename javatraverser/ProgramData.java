//package jTraverser;

public class ProgramData extends CompoundData
{
    public static Data getData() {return new ProgramData(); }
    public ProgramData() {dtype = DTYPE_PROGRAM; }
    public ProgramData(Data time_out, Data program)
    {
	dtype = DTYPE_PROGRAM;
	descs = new Data[2];
	descs[0] = time_out;
	descs[1] = program;
    }
    public final Data getTimeout() {return descs[0]; }
    public final Data getProgram() {return descs[1]; }
}
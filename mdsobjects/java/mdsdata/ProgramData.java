package mdsdata;

public class ProgramData extends CompoundData
{
    public static Data getData()
    {
	return new ProgramData();
    }
    public ProgramData()
    {
        dtype = Data.DTYPE_PROGRAM;
    }
    public ProgramData(Data timeout, Data program)
    {
        dtype = Data.DTYPE_PROGRAM;
        descs = new Data[2];
        descs[0] = timeout;
        descs[1] = program;
    }
    public Data getTimeout() {return descs[0];}
    public Data getProgram() { return descs[1];}
    public void setTimeout(Data timeout) {descs[0] = timeout;}
    public void setProgram(Data program) {descs[1] = program;}
}

package mdsdata;

public class ProcedureData extends CompoundData
{
    public static Data getData()
    {
	return new ProcedureData();
    }
    public ProcedureData()
    {
        dtype = Data.DTYPE_PROCEDURE;
    }
    public ProcedureData(Data timeout, Data language, Data procedure, Data []arguments)
    {
        dtype = Data.DTYPE_PROCEDURE;
        descs = new Data[3 + arguments.length];
        descs[0] = timeout;
        descs[1] = language;
        descs[2] = procedure;
        for(int i = 0; i < arguments.length; i++)
            descs[3+i] = arguments[i];
    }
    public Data getTimeout() {return descs[0];}
    public Data getLanguage() { return descs[1];}
    public Data getProcedure() { return descs[2];}
    public int getNumArguments() {return descs.length - 3; }
    public Data getArgumentAt(int idx) { return descs[3+idx];}
    public void setTimeout(Data timeout) {descs[0] = timeout;}
    public void setLanguage(Data language) {descs[1] = language;}
    public void setRoutine(Data procedure) {descs[2] = procedure;}
    public void setArgumentAt(Data argument, int idx) {descs[3+idx] = argument;}
}

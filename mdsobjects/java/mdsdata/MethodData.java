package mdsdata;

public class MethodData extends CompoundData
{
    public static Data getData()
    {
	return new MethodData();
    }
    public MethodData()
    {
        dtype = Data.DTYPE_METHOD;
    }
    public MethodData(Data timeout, Data method, Data object, Data []arguments)
    {
        dtype = Data.DTYPE_METHOD;
        descs = new Data[3 + arguments.length];
        descs[0] = timeout;
        descs[1] = method;
        descs[2] = object;
        for(int i = 0; i < arguments.length; i++)
            descs[3+i] = arguments[i];
    }
    public Data getTimeout() {return descs[0];}
    public Data getMethod() { return descs[1];}
    public Data getObject() { return descs[2];}
    public int getNumArguments() {return descs.length - 3; }
    public Data getArgumentAt(int idx) { return descs[3+idx];}
    public void setTimeout(Data timeout) {descs[0] = timeout;}
    public void setMethod(Data method) {descs[1] = method;}
    public void setObject(Data object) {descs[2] = object;}
    public void setArgumentAt(Data argument, int idx) {descs[3+idx] = argument;}
}

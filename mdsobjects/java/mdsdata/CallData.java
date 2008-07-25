package mdsdata;

public class CallData extends CompoundData
{
    public static Data getData(byte data)
    {
	return new CallData();
    }
    public CallData()
    {
        dtype = Data.DTYPE_CALL;
    }
    public CallData(Data image, Data routine, Data []arguments)
    {
        dtype = Data.DTYPE_CALL;
        descs = new Data[2 + arguments.length];
        descs[0] = image;
        descs[1] = routine;
        for(int i = 0; i < arguments.length; i++)
            descs[2+i] = arguments[i];
    }
    public Data getImage() { return descs[0];}
    public Data getRoutine() { return descs[1];}
    public int getNumArguments() {return descs.length - 2; }
    public Data getArgumentAt(int idx) { return descs[2+idx];}
    public void setImage(Data image) {descs[0] = image;}
    public void setRoutine(Data routine) {descs[1] = routine;}
    public void setArgumentAt(Data argument, int idx) {descs[2+idx] = argument;}
}

package mdsdata;

public class RoutineData extends CompoundData
{
    public static Data getData()
    {
	return new RoutineData();
    }
    public RoutineData()
    {
        dtype = Data.DTYPE_ROUTINE;
    }
    public RoutineData(Data timeout, Data image, Data routine, Data []arguments)
    {
        dtype = Data.DTYPE_ROUTINE;
        descs = new Data[3 + arguments.length];
        descs[0] = timeout;
        descs[1] = image;
        descs[2] = routine;
        for(int i = 0; i < arguments.length; i++)
            descs[3+i] = arguments[i];
    }
    public Data getTimeout() {return descs[0];}
    public Data getImage() { return descs[1];}
    public Data getRoutine() { return descs[2];}
    public int getNumArguments() {return descs.length - 3; }
    public Data getArgumentAt(int idx) { return descs[3+idx];}
    public void setTimeout(Data timeout) {descs[0] = timeout;}
    public void setImage(Data image) {descs[1] = image;}
    public void setRoutine(Data routine) {descs[2] = routine;}
    public void setArgumentAt(Data argument, int idx) {descs[3+idx] = argument;}
}

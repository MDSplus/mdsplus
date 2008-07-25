package mdsdata;

public class DispatchData extends CompoundData
{
    public static Data getData()
    {
	return new DispatchData();
    }
    public DispatchData()
    {
        dtype = Data.DTYPE_DISPATCH;
    }
    public DispatchData(Data ident, Data phase, Data when, Data completion)
    {
        dtype = Data.DTYPE_DISPATCH;
        descs = new Data[4];
        descs[0] = ident;
        descs[1] = phase;
        descs[2] = when;
        descs[3] = completion;
    }
    public Data getIdent() {return descs[0];}
    public Data getPhase() { return descs[1];}
    public Data getWhen() { return descs[2];}
    public Data getCompletion() { return descs[3];}
    public void setIdent(Data ident) {descs[0] = ident;}
    public void setTask(Data phase) {descs[1] = phase;}
    public void setWhen(Data when) {descs[2] = when;}
    public void setCompletion(Data completion) {descs[3] = completion;}
}

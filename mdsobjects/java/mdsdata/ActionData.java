package mdsdata;

public class ActionData extends CompoundData
{
    public static Data getData()
    {
	return new ActionData();
    }
    public ActionData()
    {
       dtype = Data.DTYPE_ACTION;
    }
    public ActionData(Data dispatch, Data task, Data errorlogs, Data completionMessage, Data performance)
    {
        dtype = Data.DTYPE_ACTION;
        descs = new Data[5];
        descs[0] = dispatch;
        descs[1] = task;
        descs[2] = errorlogs;
        descs[3] = completionMessage;
        descs[4] = performance;
    }
    public Data getDispatch() {return descs[0];}
    public Data getTask() { return descs[1];}
    public Data getErrorlogs() { return descs[2];}
    public Data getCompletionMessage() { return descs[3];}
    public Data getPerformance() { return descs[4];}
    public void setDispatch(Data dispatch) {descs[0] = dispatch;}
    public void setTask(Data task) {descs[1] = task;}
    public void setErrorlogs(Data errorlogs) {descs[2] = errorlogs;}
    public void setCompletionMessage(Data completionMessage) {descs[3] = completionMessage;}
    public void setPerformance(Data performance) {descs[4] = performance;}
}

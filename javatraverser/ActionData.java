//package jTraverser;

public class ActionData extends CompoundData
{
    public static Data getData()
    {
	return new ActionData();
    }

    public ActionData() {dtype = DTYPE_ACTION;}
    public ActionData(Data dispatch, Data task, Data errorlogs,
	Data completion_message, Data performance)
    {
	dtype = DTYPE_ACTION;
	descs = new Data[5];
	descs[0] = dispatch;
	descs[1] = task;
	descs[2] = errorlogs;
	descs[3] = completion_message;
	descs[4] = performance;
    }
    public final Data getDispatch() {return descs[0]; }
    public final Data getTask()	    {return descs[1]; }
    public final Data getErrorlogs()
    {
	if(descs.length >= 3)
	    return descs[2];
	else
	    return null;
    }
    public final Data getCompletionMessage()
    {
	if(descs.length >= 4)
	    return descs[3];
	else
	    return null;
    }

    public final Data getPerformance()
    {
	if(descs.length >= 5)
	    return descs[4];
	else
	    return null;
     }
}
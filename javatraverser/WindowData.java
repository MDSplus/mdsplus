//package jTraverser;

public class WindowData extends CompoundData
{
    public static Data getData() {return new WindowData(); }
    public WindowData() {dtype = DTYPE_WINDOW; }
    public WindowData(Data start_idx, Data end_idx, Data value_at_0)
    {
	dtype = DTYPE_WINDOW;
	descs = new Data[3];
	descs[0] = start_idx;
	descs[1] = end_idx;
	descs[2] = value_at_0;
    }
    public final Data getStartIdx()	{return descs[0]; }
    public final Data getEndIdx()	{return descs[1]; }
    public final Data getValueAt0()	{return descs[2]; }
}
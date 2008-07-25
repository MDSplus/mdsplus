package mdstree;
import mdsdata.*;
public  interface DataStreamConsumer
{
        public void acceptSegment(ArrayData data, Data start, Data end, Data dimension) throws TreeException;
	public void acceptRow(Data data, long time, boolean isLast) throws TreeException;
}
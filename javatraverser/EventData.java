//package jTraverser;

public class EventData extends StringData
{
    public static Data getData(String datum)
    {
	return new EventData(datum);
    }
    public EventData(String path)
    {
	datum = path;
	dtype = DTYPE_EVENT;
    }
}

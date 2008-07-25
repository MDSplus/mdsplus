package mdsdata;

public class EventData extends ScalarData
{
    String data;
    public static Data getData(String data)
    {
	return new EventData(data);
    }
    public EventData(String data)
    {
        dtype = Data.DTYPE_EVENT;
        this.data = data;
    }

    public String getString()
    {
        return data;
    }
}
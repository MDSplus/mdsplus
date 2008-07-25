package mdstree;

public interface DataStreamProducer
{
    public void addDataStreamConsumer(DataStreamConsumer consumer) throws TreeException;
    public void removeDataStreamConsumer(DataStreamConsumer consumer) throws TreeException;
}


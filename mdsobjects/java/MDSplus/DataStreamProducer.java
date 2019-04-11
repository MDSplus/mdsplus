package MDSplus;

/**
 * This interface is not actually implemented by any Tree class. It is meant to be
 * used in conjunction with the new real time events in order to define data pipes
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.35
 */
public interface DataStreamProducer {

	/**
	 *
	 * @param consumer
	 */
	public void addDataStreamConsumer(DataStreamConsumer consumer);

	/**
	 *
	 * @param consumer
	 */
	public void removeDataStreamConsumer(DataStreamConsumer consumer);

}
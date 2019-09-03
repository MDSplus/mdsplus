package MDSplus;

/**
 * @author manduchi
 * @version 1.0
 * @created 30-mar-2009 13.40.29
 */
public interface DataStreamConsumer {

	/**
	 *
	 * @param seg
	 * @param time
	 */
	public void acceptSegment(Array seg, Data time);

	/**
	 *
	 * @param row
	 * @param time
	 */
	public void acceptRow(Data row, long time);

}
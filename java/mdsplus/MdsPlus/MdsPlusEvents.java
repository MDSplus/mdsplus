package MdsPlus;

/*
 *
 * MdsPlusEvents
 *
 */

	/** the interface that must be supported to be an MdsPlusEvent target.
	* The EventAction method is invoked in a separate thread when a specified
	* MdsPlusEvent occurrs.
	*/
public interface MdsPlusEvents
{

	/** the method that is invoked when the specified MdsPlus event occurs on the
	* remote system.
	* @param arg the arg parameter specified in the MdsPlusEvent constructor.
	* @param ev_data a 12 byte array of data associated with the MdsPlus event.
	* @see MdsPlus.MdsPlusEvent
	*/
	public void EventAction(Object arg, byte[] ev_data);

}


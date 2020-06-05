package mds.jdispatcher;

import java.util.EventListener;

public interface NetworkListener extends EventListener
{
	/**
	 * Invoked when an Waveform event occurs.
	 */
	public void processNetworkEvent(NetworkEvent e);
}

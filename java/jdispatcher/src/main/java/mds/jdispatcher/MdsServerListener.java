package mds.jdispatcher;

import java.util.EventListener;

public interface MdsServerListener extends EventListener
{
	public void processMdsServerEvent(MdsServerEvent e);
}

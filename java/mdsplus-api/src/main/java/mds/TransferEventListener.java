package mds;

import java.nio.channels.ReadableByteChannel;
import java.util.EventListener;

public interface TransferEventListener extends EventListener
{
	public void handleTransferEvent(ReadableByteChannel rbc, String msg, int read, int to_read);
}

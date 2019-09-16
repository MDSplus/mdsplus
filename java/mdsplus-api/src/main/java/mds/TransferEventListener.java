package mds;

import java.io.InputStream;
import java.util.EventListener;

public interface TransferEventListener extends EventListener{
	public void handleTransferEvent(InputStream source, String msg, int read, int to_read);
}
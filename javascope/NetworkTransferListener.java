import java.util.EventListener;

public interface NetworkTransferListener extends EventListener
{

    public void processNetworkTransferEvent(NetworkEvent e);

}

import java.util.EventListener;

public interface NetworTransferListener extends EventListener
{

    public void processNetworkTransfer(NetworkEvent e);

}

import java.util.EventListener;

public interface NetworkEventListener extends EventListener {

    /**
     * Invoked when an Waveform event occurs.
     */
    public void processNetworkEvent(NetworkEvent e);

}

import java.util.EventListener;

public interface MdsEventListener extends EventListener {

    /**
     * Invoked when an Mds event occurs.
     */
    public void processMdsEvent(MdsEvent e);

}

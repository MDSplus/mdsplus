import java.util.EventListener;

public interface ConnectionListener extends EventListener
{

    public void processConnectionEvent(ConnectionEvent e);

}

package jScope;

/* $Id$ */
import java.awt.AWTEvent;

/**
 * An UpdateEvent instance is passed to UpdateEventListener implementations as argument of processUpdateEvent
 * in order to notify the receipt of an asynchronous event triggering the display of a waveform.
 * 
 * @see UpdateEventListener
 * @see DataProvider
 */
public class UpdateEvent extends AWTEvent 
{
    /**
     * The name of the event just received.
     */
    String name;
    
    public UpdateEvent(Object source, String event) {
        super(source, 0);
        name = event;
    }

    public String getName() { return name;}
}

package mds.wave;

/* $Id$ */
import java.util.EventListener;

public interface WaveContainerListener extends EventListener
{
    public void processWaveContainerEvent(WaveContainerEvent e);
}

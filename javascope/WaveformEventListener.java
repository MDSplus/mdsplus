import java.util.EventListener;

public interface WaveformEventListener extends EventListener {

    /**
     * Invoked when an Waveform event occurs.
     */
    public void processWaveformEvent(WaveformEvent e);

}

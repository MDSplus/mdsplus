import MultiWaveform;
import Signal;
interface WaveSetup {
	public void SetSetup(Waveform w, int x, int y);
	public void DisplayCoords(Waveform w, double x, double y, double dx, double dy,
	                          int idx, boolean measure, boolean broadcast);
	public void Hide();
	public void BroadcastScale(Waveform w);
	public void NotifyChange(Waveform target_w, Waveform source_w);
	public void SetSourceCopy(Waveform source_w);
	public void Refresh(Waveform w, String label);
	public WaveInterface GetSource();
	
}
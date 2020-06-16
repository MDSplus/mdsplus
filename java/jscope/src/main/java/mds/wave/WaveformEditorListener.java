package mds.wave;

public interface WaveformEditorListener
{
	void waveformUpdated(float[] waveX, float[] waveY, int newIdx);
}

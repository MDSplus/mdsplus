/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package mds.wave;

/**
 *
 * @author manduchi Defines the methods called by WaveData to report regions of
 *         increased resolution or new data available
 *
 */
public interface WaveDataListener
{
	void dataRegionUpdated(double[] x, float[] y, double resolution);

	void dataRegionUpdated(long[] x, float[] y, double resolution);

	void legendUpdated(String name);
}

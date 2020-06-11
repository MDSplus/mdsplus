package mds.provider.mds;

import mds.wave.WaveDataListener;

/**
 * AsynchDataSource interface is used by MdsDataProvider to handle asynchronous
 * data generation
 *
 * @author manduchi
 */
public interface AsynchDataSource
{
	void startGeneration(String expression);

	void addDataListener(WaveDataListener l);
}

/*
 * AsynchDataSource interface is used by MdsDataProvider to handle asynchronour data generation
 */

package mds.wavedisplay;

/**
 *
 * @author manduchi
 */
public interface AsynchDataSource
{
    void startGeneration(String expression);
    void addDataListener(WaveDataListener listener);
}

/*
 * Extends MdsDataProvider to display an asynchronous waveform IN ADDITION to the waveform provided by the superclass
 * This is a template class.
 */
package MDSplus;
import jScope.*;
import java.util.*;

/**
 *
 * @author manduchi
 */
public class MdsStreamingDataProvider extends MdsDataProvider
{
    Vector<Data> samplesV = new Vector<Data>();
    Vector<Data> timesV = new Vector<Data>();
    //Inner class MdsStreamingDataProvider handles data streaming in jScope
    Connection conn = null;
    class AsynchWaveData implements AsynchDataSource, DataStreamListener
    {
        double sinePeriod = 1;
        Vector<WaveDataListener> listeners = new Vector<WaveDataListener>();
        public void startGeneration(java.lang.String expression)
        {
            try {
                Connection conn = new Connection(getProvider());
                conn.registerStreamListener(this, expression, experiment, (int)shot);
                conn.startStreaming();
            }catch(Exception exc)
            {
                System.out.println("Cannot connect to " + getProvider());
            }
        }
        public void dataReceived(Data samples, Data times)
        {
            samplesV.addElement(samples);
            timesV.addElement(times);
            handleDataReceived(samples, times);
        }
        public void handleDataReceived(Data samples, Data times)
        {
            try {
                float y[] = samples.getFloatArray();
                if(times instanceof Int64Array || times instanceof Int64 || times instanceof Uint64Array || times instanceof Uint64)
                {
                    long x[] = times.getLongArray();
                    for(int j = 0; j < listeners.size(); j++)
                        listeners.elementAt(j).dataRegionUpdated(x, y, Double.MAX_VALUE);
                }
                else
                {
                    double x[] = times.getDoubleArray();
                    for(int j = 0; j < listeners.size(); j++)
                        listeners.elementAt(j).dataRegionUpdated(x, y, Double.MAX_VALUE);
                }
            }catch(Exception exc)
            {
                System.out.println("Error handling streaming: "+exc);
            }
        }
        public void addDataListener(WaveDataListener listener)
        {
            listeners.addElement(listener);
            for(int i = 0; i < samplesV.size(); i++)
                handleDataReceived(samplesV.elementAt(i), timesV.elementAt(i));
        }
    } //End inner class AsynchWaveData
  
    public AsynchDataSource getAsynchSource()
    {
        return new AsynchWaveData();
    }
    
 }


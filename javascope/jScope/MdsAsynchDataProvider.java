/*
 * Extends MdsDataProvider to display an asynchronous waveform IN ADDITION to the waveform provided by the superclass
 * This is a template class.
 */

package jScope;
import java.util.*;

/**
 *
 * @author manduchi
 */
public class MdsAsynchDataProvider extends MdsDataProvider
{
    //Inner class AsynchWaevdata handles the generation of the waveform 
    class AsynchWaveData implements AsynchDataSource
    {
        double sinePeriod = 1;
        Vector<WaveDataListener> listeners = new Vector<WaveDataListener>();
        public void startGeneration(String expression)
        {
            //JUST A SAMPLE SINE GENERATION...TO BE REPLACED BY REAL WAVEFORMS
            //JUST ASSUMED THAT THE PASSED EXPRESSION IS THE SINE PERIOD
            try {
                sinePeriod = Double.parseDouble(expression);
            }catch(Exception exc){}
            (new Thread() {
                public void run()
                {
                    for(int i = 0; i < 100; i++)
                    {
                        
                        try {
                             Thread.sleep(100);
                        }catch(InterruptedException exc){}
                        if(!asynchEnabled)
                        {
                            System.out.println("NOT ENABLED");
                            continue;
                        }
                        double x[] = new double[]{i};
                        float y[] = new float[]{(float)Math.sin(2 * Math.PI * sinePeriod * i/100.)};
                        for(int j = 0; j < listeners.size(); j++)
                        {
                            listeners.elementAt(j).dataRegionUpdated(x, y, Double.MAX_VALUE);
                            listeners.elementAt(j).legendUpdated("CICCIO"+i);
                        }
                    }
                    
                }
            }).start();
        }
        public void addDataListener(WaveDataListener listener)
        {
            listeners.addElement(listener);
        }
    } //End inner class AsynchWaveData
    
    boolean asynchEnabled = true;
    public void enableAsyncUpdate(boolean asynchEnabled)
    {
        asynchEnabled = asynchEnabled;
    }
    
    public AsynchDataSource getAsynchSource()
    {
        return new AsynchWaveData();
    }
}
 
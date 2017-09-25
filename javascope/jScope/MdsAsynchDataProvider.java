/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
 
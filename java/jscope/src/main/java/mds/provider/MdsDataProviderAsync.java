/*
 * Extends MdsDataProvider to display an asynchronous waveform IN ADDITION to the waveform provided by the superclass
 * This is a template class.
 */
package mds.provider;

import java.util.Vector;

import mds.wave.WaveDataListener;

/**
 *
 * @author manduchi
 */
public class MdsDataProviderAsync extends MdsDataProvider
{
	// Inner class AsynchWaevdata handles the generation of the waveform
	class AsynchWaveData implements AsyncDataSource
	{
		double sinePeriod = 1;
		Vector<WaveDataListener> listeners = new Vector<>();

		@Override
		public void addDataListener(WaveDataListener listener)
		{
			listeners.addElement(listener);
		}

		@Override
		public void startGeneration(String expression)
		{
			// JUST A SAMPLE SINE GENERATION...TO BE REPLACED BY REAL WAVEFORMS
			// JUST ASSUMED THAT THE PASSED EXPRESSION IS THE SINE PERIOD
			try
			{
				sinePeriod = Double.parseDouble(expression);
			}
			catch (final Exception exc)
			{}
			(new Thread()
			{
				@Override
				public void run()
				{
					for (int i = 0; i < 100; i++)
					{
						try
						{
							Thread.sleep(100);
						}
						catch (final InterruptedException exc)
						{}
						if (!asynchEnabled)
						{
							System.out.println("NOT ENABLED");
							continue;
						}
						final double x[] = new double[]
						{ i };
						final float y[] = new float[]
						{ (float) Math.sin(2 * Math.PI * sinePeriod * i / 100.) };
						for (int j = 0; j < listeners.size(); j++)
						{
							listeners.elementAt(j).dataRegionUpdated(x, y, Double.MAX_VALUE);
							listeners.elementAt(j).legendUpdated("CICCIO" + i);
						}
					}
				}
			}).start();
		}
	}

	boolean asynchEnabled = true;

	@Override
	public void enableAsyncUpdate(boolean asynchEnabled)
	{
		this.asynchEnabled = asynchEnabled;
	}

	@Override
	public AsyncDataSource getAsynchSource()
	{ return new AsynchWaveData(); }
}

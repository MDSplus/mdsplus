/*
 * Extends MdsDataProvider to display an asynchronous waveform IN ADDITION to the waveform provided by the superclass
 * This is a template class.
 */
package mds.provider;

import java.util.Vector;

import MDSplus.*;
import mds.wave.WaveDataListener;

/**
 *
 * @author manduchi
 */
public class MdsDataProviderStream extends MdsDataProvider
{
	/** Handles data streaming in jScope */
	class AsynchWaveData implements AsyncDataSource, DataStreamListener
	{
		boolean listenerReady = false;
		Vector<Data> samplesV = new Vector<>();
		Vector<Data> timesV = new Vector<>();
		double sinePeriod = 1;
		Vector<WaveDataListener> listeners = new Vector<>();

		@Override
		public void addDataListener(WaveDataListener listener)
		{
			listeners.addElement(listener);
			for (int i = 0; i < samplesV.size(); i++)
				handleDataReceived(samplesV.elementAt(i), timesV.elementAt(i));
			listenerReady = true;
		}

		@Override
		public void dataReceived(Data samples, Data times)
		{
			if (!listenerReady)
			{
				samplesV.addElement(samples);
				timesV.addElement(times);
			}
			handleDataReceived(samples, times);
		}

		public void handleDataReceived(Data samples, Data times)
		{
			try
			{
				float y[] = samples.getFloatArray();
				if (times instanceof Int64Array || times instanceof Int64 || times instanceof Uint64Array
						|| times instanceof Uint64)
				{
					final long x[] = times.getLongArray();
					for (int j = 0; j < listeners.size(); j++)
						listeners.elementAt(j).dataRegionUpdated(x, y, Double.MAX_VALUE);
				}
				else
				{
					double x[] = times.getDoubleArray();
					// Handle possible differences in X and Y array dimensions
					int dataLen = x.length;
					if (dataLen > y.length)
						dataLen = y.length;
					if (dataLen != x.length)
					{
						final double newX[] = new double[dataLen];
						System.arraycopy(x, 0, newX, 0, dataLen);
						x = newX;
					}
					if (dataLen != y.length)
					{
						final float newY[] = new float[dataLen];
						System.arraycopy(y, 0, newY, 0, dataLen);
						y = newY;
					}
					/////////////////////////////////////////
					for (int j = 0; j < listeners.size(); j++)
						listeners.elementAt(j).dataRegionUpdated(x, y, Double.MAX_VALUE);
				}
			}
			catch (final Exception exc)
			{
				System.out.println("Error handling streaming: " + exc);
			}
		}

		@Override
		public void startGeneration(java.lang.String expression)
		{
			try
			{
				final Connection conn = new Connection(getProvider());
				synchronized (activeConnections)
				{
					activeConnections.addElement(conn);
				}
				conn.registerStreamListener(this, expression, experiment, (int) shot);
				conn.startStreaming();
			}
			catch (final Exception exc)
			{
				System.out.println("Cannot connect to " + getProvider());
			}
		}
	} // End inner class AsynchWaveData
	final private Vector<Connection> activeConnections = new Vector<>();

	Connection conn = null;

	@Override
	public AsyncDataSource getAsynchSource()
	{ return new AsynchWaveData(); }

	@Override
	public void resetPrevious()
	{
		synchronized (activeConnections)
		{
			for (final Connection connection : activeConnections)
				connection.disconnect();
			activeConnections.clear();
		}
	}
}

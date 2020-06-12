import java.awt.*;

import mds.wave.*;

public class DeviceWaveDisplay extends DeviceComponent
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	protected Waveform wave;
	protected String oldData;
	float x[] = null, y[] = null;
	protected int prefHeight = 200;

	public DeviceWaveDisplay()
	{}

	@Override
	protected void displayData(String data, boolean is_on)
	{
		try
		{
			final String xData = "DIM_OF(" + data + ")";
			final String yData = data;
			x = subtree.getFloatArray(xData);
			y = subtree.getFloatArray(yData);
			wave.Update(x, y);
		}
		catch (final Exception exc)
		{}
	}

	@Override
	protected String getData()
	{ return oldData; }

	public int getPrefHeight()
	{ return prefHeight; }

	@Override
	protected boolean getState()
	{ return true; }

	@Override
	protected void initializeData(String data, boolean is_on)
	{
		oldData = data;
		setLayout(new BorderLayout());
		wave = new Waveform();
		wave.setPreferredSize(new Dimension(300, 200));
		add(wave, "Center");
		displayData(data, is_on);
	}

	@Override
	public boolean isDataChanged()
	{ return false; }

	@Override
	void postApply()
	{
		displayData(oldData, true);
	}

	@Override
	public void print(Graphics g)
	{
		wave.paintComponent(g);
	}

	@Override
	public void reset()
	{}

	@Override
	public void setEnabled(boolean state)
	{}

	public void setPrefHeight(int prefHeight)
	{ this.prefHeight = prefHeight; }
}

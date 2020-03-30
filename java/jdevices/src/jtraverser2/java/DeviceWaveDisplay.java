import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Graphics;


import jScope.Waveform;
import mds.data.descriptor.Descriptor;

public class DeviceWaveDisplay extends DeviceComponent
{
    protected Waveform wave;
    protected Descriptor<?> oldData;
    float x[] = null,y[] = null;
    protected int prefHeight = 200;

    public void setPrefHeight(int prefHeight)
    {
	this.prefHeight = prefHeight;
    }

    public int getPrefHeight(){return prefHeight;}
    public DeviceWaveDisplay()
    {}


    protected void initializeData(Descriptor<?> data, boolean is_on)
    {
	oldData = data;
	setLayout(new BorderLayout());
	wave = new Waveform();
	wave.setPreferredSize(new Dimension(300, 200));
	add(wave, "Center");
	displayData(data, is_on);
    }


    protected void displayData(Descriptor<?> data, boolean is_on)
    {
	try {
		Descriptor<?> xData = data.getDimension();
		Descriptor<?> yData = data.getData();
	    x = xData.toFloatArray();
	    y = yData.toFloatArray();
	    wave.Update(x, y);
	}catch(Exception exc){}
    }

    protected Descriptor<?> getData()
    {
	return oldData;
    }

    protected boolean getState(){return true;}
    public void setEnabled(boolean state) {}
    public boolean isDataChanged() {return false;}

    void postApply()
    {
	displayData(oldData, true);
    }

    public void reset()
    {
    }

    public void print(Graphics g)
    {
	wave.paintComponent(g);
    }
}


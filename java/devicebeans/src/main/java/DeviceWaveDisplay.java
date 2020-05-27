import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Graphics;

import jScope.Waveform;

public class DeviceWaveDisplay extends DeviceComponent
{
    protected Waveform wave;
    protected String oldData;
    float x[] = null,y[] = null;
    protected int prefHeight = 200;

    public void setPrefHeight(int prefHeight)
    {
	this.prefHeight = prefHeight;
    }

    public int getPrefHeight(){return prefHeight;}
    public DeviceWaveDisplay()
    {}


    protected void initializeData(String data, boolean is_on)
    {
	oldData = data;
	setLayout(new BorderLayout());
	wave = new Waveform();
	wave.setPreferredSize(new Dimension(300, 200));
	add(wave, "Center");
	displayData(data, is_on);
    }


    protected void displayData(String data, boolean is_on)
    {
	try {
	    String xData = "DIM_OF(" + data + ")";
	    String yData = data;
	    x = subtree.getFloatArray(xData);
	    y = subtree.getFloatArray(yData);
	    wave.Update(x, y);
	}catch(Exception exc){}
    }

     protected String getData()
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


package MDSplus;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author manduchi
 */
import jScope.*;

public class Scope
{
    CompositeWaveDisplay window;
    public Scope(java.lang.String name, int x, int y, int width, int height)
    {
	   window = CompositeWaveDisplay.createWindow(name);
	   window.showWindow(x,y,width, height);
    }
    public void plot(Data x, Data y, int row, int col, java.lang.String color) throws Exception
    {
	window.removeAllSignals(row, col);
	window.addSignal(x.getFloatArray(), y.getFloatArray(), row, col, color,"", true ,0);
    }
    public void oplot(Data x, Data y, int row, int col, java.lang.String color) throws Exception
    {
	window.addSignal(x.getFloatArray(), y.getFloatArray(), row, col, color, "", true, 0);
    }


    //Test
    public static void main(java.lang.String args[])
    {
	Scope scope = new Scope("CACCA", 100, 100, 100, 100);
	float x[] = new float[1000];
	float y1[] = new float[1000];
	float y2[] = new float[1000];
	for(int i = 0; i < 1000; i++)
	{
	    x[i] = i;
	    y1[i] = x[i]*x[i];
	    y2[i] = 2*y1[i];
	}
	try {
	    scope.plot(new Float32Array(x), new Float32Array(y1) , 1, 1, "black");
	    scope.oplot(new Float32Array(x), new Float32Array(y2) , 1, 1, "red");
	}catch(Exception exc)
	{System.err.println(exc);}
    }

}



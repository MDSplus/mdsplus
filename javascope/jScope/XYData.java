/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jScope;

/**
 *
 * @author manduchi
 */
public class XYData 
{
    double resolution; //Number of points/interval
    boolean increasingX;
    int nSamples;
    double [] x = null;
    long[] xLong = null;
    float[] y;
    double xMin, xMax;
    XYData(double x[], float y[], double resolution, boolean increasingX)
    {
 //       this(x, y, resolution, increasingX, -Double.MAX_VALUE, Double.MAX_VALUE);
       this(x, y, resolution, increasingX, x[0], x[x.length - 1]);
    }
    XYData(double x[], float y[], double resolution, boolean increasingX, double xMin, double xMax)
    {
        this.resolution = resolution;
        this.increasingX = increasingX;
        this.x = x;
        this.y = y;
        this.xMin = xMin;
        this.xMax = xMax;
        nSamples = (x.length < y.length)?x.length:y.length;
    }
    XYData(double x[], float y[], double resolution)
    {
        this.resolution = resolution;
        this.x = x;
        this.y = y;
        increasingX = true;
        nSamples = (x.length < y.length)?x.length:y.length;
        if(nSamples > 0)
        {
            xMin = xMax = x[0];
            for(int i = 1; i < x.length; i++)
            {
                if(x[i-1] > x[i])
                {
                    increasingX = false;
                }
                if(x[i] > xMax)
                    xMax = x[i];
                if(x[i] < xMin)
                    xMin = x[i];
            }
        }
    }
    XYData(long x[], float y[], double resolution)
    {
        this.resolution = resolution;
        this.xLong = x;
        this.y = y;
        this.x = new double[x.length];
        for(int i = 0; i < x.length; i++)
            this.x[i] = x[i];
        increasingX = true;
        nSamples = (x.length < y.length)?x.length:y.length;
        if(nSamples > 0)
        {
            xMin = xMax = x[0];
            for(int i = 1; i < x.length; i++)
            {
                if(x[i-1] > x[i])
                {
                    increasingX = false;
                }
                if(x[i] > xMax)
                    xMax = x[i];
                if(x[i] < xMin)
                    xMin = x[i];
            }
        }
    }
    XYData(long[]x, float[]y, double  resolution, boolean increasingX)
    {
        this.resolution = resolution;
        this.increasingX = increasingX;
        this.xLong = x;
        this.y = y;
        this.x = new double[x.length];
        for(int i = 0; i < x.length; i++)
            this.x[i] = x[i];
        nSamples = (x.length < y.length)?x.length:y.length;
   }
}

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
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jScope;

/**
 *
 * @author manduchi
 */

/**
 * Class XYData is the container of signals returned by WaveData instances.  Code immplementing WaveData
 * interface will use this class only via its constructor methods. 
 * Besides X and Y arrays, resolution and increasingX information is defined here. Resolution is computed 
 * in jScope as NumberOfSamples/XRange and compared with the declared resolution on the XYData instance in order
 * to activate the asyncronous retrieval mechanism (that occurs when a resampled signal is zoomed and 
 * more resolution is required in the zoomed region).
 * increasingX flag specifies whether the X samples are in increasing order. jScope
 * handles dynamic resampling only when this flag is true. 
 *
 * @see WaveData
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
   /**
     * XYData Constructor
     *
     * @param x: x (times) array
     * @param y: y (values) array
     * @param resolution: declared resolution
     * @param increasingX: increasing x values flag
     */
    public  XYData(double x[], float y[], double resolution, boolean increasingX)
    {
       this(x, y, resolution, increasingX, x[0], x[x.length - 1]);
    }
    
   /**
     * XYData Constructor
     *
     * @param x: x (times) array
     * @param y: y (values) array
     * @param resolution: declared resolution
     * @param increasingX: incresing x values flag
     * @param xMin: lower bound X region of interest
     * @param xMax: upper bound X region of interest
     */
    public XYData(double x[], float y[], double resolution, boolean increasingX, double xMin, double xMax)
    {
        this.resolution = resolution;
        this.increasingX = increasingX;
        this.x = x;
        this.y = y;
        this.xMin = xMin;
        this.xMax = xMax;
        nSamples = (x.length < y.length)?x.length:y.length;
    }
    
   /**
     * XYData Constructor with automatic derivation of increasingX flag
     *
     * @param x: x (times) array
     * @param y: y (values) array
     * @param resolution: declared resolution
     */
    public XYData(double x[], float y[], double resolution)
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
    
   /**
     * XYData Constructor with absolute times and automatic derivation of increasingX flag
     *
     * @param x: x (times) array
     * @param y: y (values) array
     * @param resolution: declared resolution
     */
    public XYData(long x[], float y[], double resolution)
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
    
   /**
     * XYData Constructor with absolute times
     *
     * @param x: x (times) array
     * @param y: y (values) array
     * @param resolution: declared resolution
     * @param increasingX: incresing x values flag
     * @param xMin: lower bound X region of interest
     * @param xMax: upper bound X region of interest
     */
    public XYData(long[]x, float[]y, double  resolution, boolean increasingX)
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

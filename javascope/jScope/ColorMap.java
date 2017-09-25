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
package jScope;

import java.awt.*;
import java.awt.image.*;
import java.util.*;
import javax.swing.*;

public class ColorMap
{

  float pR[];
  float pG[];
  float pB[];

  float vR[];
  float vG[];
  float vB[];

  float min;
  float max;
  int numColors = 0;
  Color colors[];
  String name = "unnamed";
  IndexColorModel indexColorModel = null;
  byte r[] = new byte[256];
  byte g[] = new byte[256];
  byte b[] = new byte[256];

  int bitShift = 0;
  boolean bitClip = false;  

  ColorMap()
  {
      this.name = "Green scale";
      numColors = 256;
      this.colors = new Color[256];

      for(int i = 0; i < numColors; i++)
      {
          this.r[i] = (byte)(0xFF & i);
          this.g[i] = (byte)(0xFF & i);
          this.b[i] = (byte)(0xFF & i);
          colors[i] = new Color(i, i, i);
      }
  }

  public String toString() {return name;}

  ColorMap(String name, int r[], int g[], int b[])
  {
      this.name = name;
      numColors = 256;
      this.colors = new Color[256];

      for(int i = 0; i < numColors; i++)
      {
          this.r[i] = (byte)(0xFF & r[i]);
          this.g[i] = (byte)(0xFF & g[i]);
          this.b[i] = (byte)(0xFF & b[i]);
          colors[i] = new Color(r[i], g[i], b[i]);
      }
  }


  ColorMap(int numColors, float min, float max)
  {
      /*
    createColorMap(numColors, 0,  min,  max);

    this.max = max;
    this.min = min;
    this.numColors = numColors;
    pR = new float[5];
    pR[0] = 0.f;
    pR[1] = 0.2f;
    pR[2] = 0.6f;
    pR[3] = 0.8f;
    pR[4] = 1.f;

    vR = new float[5];
    vR[0] = 255;
    vR[1] = 0;
    vR[2] = 0;
    vR[3] = 255;
    vR[4] = 255;

    pG = new float[5];
    pG[0] = 0.f;
    pG[1] = 0.2f;
    pG[2] = 0.4f;
    pG[3] = 0.8f;
    pG[4] = 1.f;

    vG = new float[5];
    vG[0] = 0;
    vG[1] = 0;
    vG[2] = 255;
    vG[3] = 255;
    vG[4] = 0;

    pB = new float[4];
    pB[0] = 0.f;
    pB[1] = 0.4f;
    pB[2] = 0.6f;
    pB[3] = 1.f;

    vB = new float[4];
    vB[0] = 255;
    vB[1] = 255;
    vB[2] = 0;
    vB[3] = 0;

    computeColorMap();
    */
  }

  public void createColorMap(int numColors, int numPoints,
                             float min, float max)
  {

      this.max = max;
      this.min = min;
      this.numColors = numColors;
      float delta = max - min;

      pR = new float[5];
      pR[0] = min;
      pR[1] = min + delta * 0.2f;
      pR[2] = min + delta * 0.6f;
      pR[3] = min + delta * 0.8f;
      pR[4] = min + delta * 1.f;

      vR = new float[5];
      vR[0] = 255;
      vR[1] = 0;
      vR[2] = 0;
      vR[3] = 255;
      vR[4] = 255;

      pG = new float[5];
      pG[0] = min;
      pG[1] = min + delta * 0.2f;
      pG[2] = min + delta * 0.4f;
      pG[3] = min + delta * 0.8f;
      pG[4] = min + delta * 1.f;

      vG = new float[5];
      vG[0] = 0;
      vG[1] = 0;
      vG[2] = 255;
      vG[3] = 255;
      vG[4] = 0;

      pB = new float[4];
      pB[0] = min;
      pB[1] = min + delta * 0.4f;
      pB[2] = min + delta * 0.6f;
      pB[3] = min + delta * 1.f;

      vB = new float[4];
      vB[0] = 255;
      vB[1] = 255;
      vB[2] = 0;
      vB[3] = 0;

      computeColorMap();
  }



  public float[] getRedValues() {return vR;}
  public float[] getRedPoints() {return pR;}

  public float[] getGreenValues() {return vG;}
  public float[] getGreenPoints() {return pG;}

  public float[] getBlueValues() {return vB;}
  public float[] getBluePoints() {return pB;}

  public float getMin()
  {
    return min;
  }

  public float getMax()
  {
    return max;
  }

  public void setMin(float min)
  {
    this.min = min;
  }

  public void setMax(float max)
  {
    this.max = max;
  }

  public void setRedParam(float p[], float v[])
  {
    this.pR = p;
    this.vR = v;
    int r[];
    r = getValues(numColors, p, v);
    for (int i = 0; i < numColors; i++)
    {
      colors[i] = new Color(r[i], colors[i].getGreen(), colors[i].getBlue());
    }
  }

  public void setGreenParam(float p[], float v[])
  {
    this.pG = p;
    this.vG = v;
    int g[];
    g = getValues(numColors, p, v);
    for (int i = 0; i < numColors; i++)
    {
      colors[i] = new Color(colors[i].getRed(), g[i], colors[i].getBlue());
    }
  }

  public void setBlueParam(float p[], float v[])
  {
    this.pB = p;
    this.vB = v;
    int b[];
    b = getValues(numColors, p, v);
    for (int i = 0; i < numColors; i++)
    {
      colors[i] = new Color(colors[i].getRed(), colors[i].getGreen(), b[i]);
    }
  }

  public void computeColorMap()
  {
    int r[], g[], b[];
    colors = new Color[numColors];

    r = getValues(numColors, pR, vR);
    g = getValues(numColors, pG, vG);
    b = getValues(numColors, pB, vB);

    for (int i = 0; i < numColors; i++)
    {
      colors[i] = new Color(r[i], g[i], b[i]);
    }
  }

  public byte[] getRedIntValues()
  {
     int c[];
     byte b[] = new byte[numColors];
     c = getValues(numColors, pR, vR);

     for(int i = 0; i < numColors; b[i] = (byte)c[i], i++);

     return b;
  }
  public byte[] getGreenIntValues()
  {
      int c[];
     byte b[] = new byte[numColors];

     c =  getValues(numColors, pG, vG);
     for(int i = 0; i < numColors; b[i] = (byte)c[i], i++);
     return b;
 }
  public byte[] getBlueIntValues()
  {
      int c[];
     byte b[] = new byte[numColors];

     c =  getValues(numColors, pB, vB);
     for(int i = 0; i < numColors; b[i] = (byte)c[i], i++);
     return b;
 }


  public Color getColor(float val, float min, float max)
  {
    int idx = (int) ( (val - min) / (max - min) * (numColors - 1));
    return colors[idx];
  }

  public Color getColor(float val)
  {
    if(val < min) return colors[0];
    else if(val > max) return colors[colors.length - 1];

    int idx = (int) ( (val - max)/ (max - min) + 1 ) * (colors.length - 1);
    return colors[idx];
  }

  public Color[] getColors()
  {
    return colors;
  }

  public IndexColorModel getIndexColorModel(int numBit)
  {
      indexColorModel = new IndexColorModel(numBit, numColors, r, g, b);
      return indexColorModel;
  }

  private int[] getValues(int nVal, float p[], float v[])
  {
    int out[] = new int[nVal];
    float dx = (p[p.length - 1] - p[0]) / (nVal - 1);
    float val = 0;
    int idx = 0;
    float c1 = (v[0] - v[1]) / (p[0] - p[1]);

    for (int i = 0; i < nVal; i++,
         c1 = (v[idx] - v[idx + 1]) / (p[idx] - p[idx + 1]))
    {
      if (p[idx] == p[idx + 1])
      {
        idx++;
        i--;
        continue;
      }

      val = i * dx;
      if (val > p[idx + 1])
      {
        idx++;
        i--;
        continue;
      }
      out[i] =  (int)(c1 * val - p[idx] * c1 + v[idx]);
      if (out[i] > 255)
      {
        out[i] = 255;
      }
      else if (out[i] < 0)
      {
        out[i] = 0;
      }
    }
    return out;
  }
}
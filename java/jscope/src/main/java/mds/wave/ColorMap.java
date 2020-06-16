package mds.wave;

import java.awt.Color;
import java.awt.image.IndexColorModel;

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
	public String name = "unnamed";
	IndexColorModel indexColorModel = null;
	byte r[] = new byte[256];
	byte g[] = new byte[256];
	byte b[] = new byte[256];
	public int bitShift = 0;
	public boolean bitClip = false;

	ColorMap()
	{
		this.name = "Green scale";
		numColors = 256;
		this.colors = new Color[256];
		for (int i = 0; i < numColors; i++)
		{
			this.r[i] = (byte) (0xFF & i);
			this.g[i] = (byte) (0xFF & i);
			this.b[i] = (byte) (0xFF & i);
			colors[i] = new Color(i, i, i);
		}
	}

	ColorMap(int numColors, float min, float max)
	{
		/*
		 * createColorMap(numColors, 0, min, max);
		 *
		 * this.max = max; this.min = min; this.numColors = numColors; pR = new
		 * float[5]; pR[0] = 0.f; pR[1] = 0.2f; pR[2] = 0.6f; pR[3] = 0.8f; pR[4] = 1.f;
		 *
		 * vR = new float[5]; vR[0] = 255; vR[1] = 0; vR[2] = 0; vR[3] = 255; vR[4] =
		 * 255;
		 *
		 * pG = new float[5]; pG[0] = 0.f; pG[1] = 0.2f; pG[2] = 0.4f; pG[3] = 0.8f;
		 * pG[4] = 1.f;
		 *
		 * vG = new float[5]; vG[0] = 0; vG[1] = 0; vG[2] = 255; vG[3] = 255; vG[4] = 0;
		 *
		 * pB = new float[4]; pB[0] = 0.f; pB[1] = 0.4f; pB[2] = 0.6f; pB[3] = 1.f;
		 *
		 * vB = new float[4]; vB[0] = 255; vB[1] = 255; vB[2] = 0; vB[3] = 0;
		 *
		 * computeColorMap();
		 */
	}

	ColorMap(String name, int r[], int g[], int b[])
	{
		this.name = name;
		numColors = 256;
		this.colors = new Color[256];
		for (int i = 0; i < numColors; i++)
		{
			this.r[i] = (byte) (0xFF & r[i]);
			this.g[i] = (byte) (0xFF & g[i]);
			this.b[i] = (byte) (0xFF & b[i]);
			colors[i] = new Color(r[i], g[i], b[i]);
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

	public void createColorMap(int numColors, int numPoints, float min, float max)
	{
		this.max = max;
		this.min = min;
		this.numColors = numColors;
		final float delta = max - min;
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

	public byte[] getBlueIntValues()
	{
		int c[];
		final byte b[] = new byte[numColors];
		c = getValues(numColors, pB, vB);
		for (int i = 0; i < numColors; b[i] = (byte) c[i], i++);
		return b;
	}

	public float[] getBluePoints()
	{ return pB; }

	public float[] getBlueValues()
	{ return vB; }

	public Color getColor(float val)
	{
		if (val < min)
			return colors[0];
		else if (val > max)
			return colors[colors.length - 1];
		final int idx = (int) ((val - max) / (max - min) + 1) * (colors.length - 1);
		return colors[idx];
	}

	public Color getColor(float val, float min, float max)
	{
		final int idx = (int) ((val - min) / (max - min) * (numColors - 1));
		return colors[idx];
	}

	public Color[] getColors()
	{ return colors; }

	public byte[] getGreenIntValues()
	{
		int c[];
		final byte b[] = new byte[numColors];
		c = getValues(numColors, pG, vG);
		for (int i = 0; i < numColors; b[i] = (byte) c[i], i++);
		return b;
	}

	public float[] getGreenPoints()
	{ return pG; }

	public float[] getGreenValues()
	{ return vG; }

	public IndexColorModel getIndexColorModel(int numBit)
	{
		indexColorModel = new IndexColorModel(numBit, numColors, r, g, b);
		return indexColorModel;
	}

	public float getMax()
	{ return max; }

	public float getMin()
	{ return min; }

	public byte[] getRedIntValues()
	{
		int c[];
		final byte b[] = new byte[numColors];
		c = getValues(numColors, pR, vR);
		for (int i = 0; i < numColors; b[i] = (byte) c[i], i++);
		return b;
	}

	public float[] getRedPoints()
	{ return pR; }

	public float[] getRedValues()
	{ return vR; }

	private int[] getValues(int nVal, float p[], float v[])
	{
		final int out[] = new int[nVal];
		final float dx = (p[p.length - 1] - p[0]) / (nVal - 1);
		float val = 0;
		int idx = 0;
		float c1 = (v[0] - v[1]) / (p[0] - p[1]);
		for (int i = 0; i < nVal; i++, c1 = (v[idx] - v[idx + 1]) / (p[idx] - p[idx + 1]))
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
			out[i] = (int) (c1 * val - p[idx] * c1 + v[idx]);
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

	public void setMax(float max)
	{ this.max = max; }

	public void setMin(float min)
	{ this.min = min; }

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

	@Override
	public String toString()
	{
		return name;
	}
}

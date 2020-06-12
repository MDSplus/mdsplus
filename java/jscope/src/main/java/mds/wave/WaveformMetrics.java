package mds.wave;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.Polygon;
import java.awt.Rectangle;
import java.awt.image.IndexColorModel;
import java.io.Serializable;
import java.util.Vector;

public class WaveformMetrics implements Serializable
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	static double MAX_VALUE = 10000.; // (double)Integer.MAX_VALUE;
	static double MIN_VALUE = -10000.; // (double)Integer.MIN_VALUE;
	static int INT_MAX_VALUE = (int) MAX_VALUE;
	static int INT_MIN_VALUE = (int) MIN_VALUE;
	//    static IndexColorModel cm = null;
	static final double LOG10 = 2.302585092994, MIN_LOG = 10E-100;
	double xmax, xmin, ymax, ymin;
	double xrange, // xmax - xmin
			yrange; // ymax - ymin
	double y_range;
	boolean x_log, y_log;
	double x_offset;
	double x_range;
	int start_x;
	double FACT_X, FACT_Y, OFS_X, OFS_Y;
int horizontal_offset, vertical_offset;

	public WaveformMetrics(double _xmax, double _xmin, double _ymax, double _ymin, Rectangle limits, Dimension d,
			boolean _x_log, boolean _y_log, int horizontal_offset, int vertical_offset)
	{
		final int ylabel_width = limits.width, xlabel_height = limits.height;
		double delta_x, delta_y;
		int border_y;
		this.horizontal_offset = horizontal_offset;
		this.vertical_offset = vertical_offset;
		if (_ymin > _ymax)
			_ymin = _ymax;
		if (_xmin > _xmax)
			_xmin = _xmax;
		start_x = ylabel_width;
		x_log = _x_log;
		y_log = _y_log;
		border_y = xlabel_height;
//    y_range = (d.height - border_y)/(double)d.height;
		y_range = (d.height - border_y - 2 * vertical_offset) / (double) d.height;
//	x_range = (d.width - start_x)/(double)d.width;
		x_range = (d.width - start_x - 2 * horizontal_offset) / (double) d.width;
		x_offset = start_x / (double) d.width;
		if (x_log)
		{
			if (_xmax < MIN_LOG)
				_xmax = MIN_LOG;
			if (_xmin < MIN_LOG)
				_xmin = MIN_LOG;
			xmax = Math.log(_xmax) / LOG10;
			xmin = Math.log(_xmin) / LOG10;
		}
		else
		{
			xmax = _xmax;
			xmin = _xmin;
		}
		delta_x = xmax - xmin;
		xmax += delta_x / 100.;
		xmin -= delta_x / 100.;
		if (y_log)
		{
			if (_ymax < MIN_LOG)
				_ymax = MIN_LOG;
			if (_ymin < MIN_LOG)
				_ymin = MIN_LOG;
			ymax = Math.log(_ymax) / LOG10;
			ymin = Math.log(_ymin) / LOG10;
		}
		else
		{
			ymax = _ymax;
			ymin = _ymin;
		}
		delta_y = ymax - ymin;
		ymax += delta_y / 50;
		ymin -= delta_y / 50.;
		xrange = xmax - xmin;
		yrange = ymax - ymin;
		if (xrange <= 0)
		{
			xrange = 1E-10;
			x_offset = 0.5;
		}
		if (yrange <= 0)
		{
			yrange = 1E-10;
		}
	}

	final public void ComputeFactors(Dimension d)
	{
//	OFS_X = x_offset * d.width - xmin*x_range*d.width/xrange + 0.5;
		OFS_X = x_offset * d.width - xmin * x_range * d.width / xrange + horizontal_offset + 0.5;
		FACT_X = x_range * d.width / xrange;
//	OFS_Y = y_range * ymax*d.height/yrange + 0.5;
		OFS_Y = y_range * ymax * d.height / yrange + vertical_offset + 0.5;
		FACT_Y = -y_range * d.height / yrange;
	}

	private void drawRectagle(Graphics g, IndexColorModel cm, int x, int y, int w, int h, int cIdx)
	{
		g.setColor(new Color(cm.getRed(cIdx), cm.getGreen(cIdx), cm.getBlue(cIdx), cm.getAlpha(cIdx)));
		g.fillRect(x, y, w, h);
	}

	public void ToImage(Signal s, Image img, Dimension d, ColorMap colorMap)
	{
		int i;
		int xSt, xEt, ySt, yEt;
		final Graphics2D g2 = (Graphics2D) img.getGraphics();
		final IndexColorModel cm = colorMap.getIndexColorModel(8);
		ComputeFactors(d);
		g2.setColor(Color.white);
		g2.fillRect(0, 0, d.width - 1, d.height - 1);
		final double[] x2D = s.getX2D();
		final float[] y2D = s.getY2D();
		final float[] z2D = s.getZ();
		float z2D_min, z2D_max;
		z2D_min = z2D_max = z2D[0];
		for (int idx = 0; idx < z2D.length; idx++)
		{
			if (z2D[idx] < z2D_min)
				z2D_min = z2D[idx];
			if (z2D[idx] > z2D_max)
				z2D_max = z2D[idx];
		}
		for (i = 0; i < x2D.length && x2D[i] < xmin; i++);
		xSt = i;
		for (i = 0; i < x2D.length && x2D[i] < xmax; i++);
		xEt = i;
		for (i = 0; i < y2D.length && y2D[i] < ymin; i++);
		ySt = i;
		for (i = 0; i < y2D.length && y2D[i] < ymax; i++);
		yEt = i;
		if (yEt == 0)
			return;
		int p = 0;
		int h = 0;
		int w = 0;
		int yPix0;
		int yPix1;
		int xPix0;
		int xPix1;
		int pix;
		try
		{
			yPix1 = (YPixel(y2D[ySt + 1]) + YPixel(y2D[ySt])) / 2;
			yPix1 = 2 * YPixel(y2D[ySt]) - yPix1;
			float currMax = z2D_min, currMin = z2D_max;
			for (int y = ySt; y < yEt; y++)
			{
				p = y * x2D.length + xSt;
				for (int x = xSt; x < xEt && p < z2D.length; x++)
				{
					if (z2D[p] > currMax)
						currMax = z2D[p];
					if (z2D[p] < currMin)
						currMin = z2D[p];
					p++;
				}
			}
			for (int y = ySt; y < yEt; y++)
			{
				yPix0 = yPix1;
				try
				{
					yPix1 = (YPixel(y2D[y + 1]) + YPixel(y2D[y])) / 2;
					h = Math.abs(yPix0 - yPix1) + 2;
				}
				catch (final Exception e)
				{
					yPix1 = 2 * YPixel(y2D[yEt - 1]) - yPix1;
					h = Math.abs(yPix0 - yPix1) + 2;
				}
				p = y * x2D.length + xSt;
				xPix1 = (XPixel(x2D[xSt]) + XPixel(x2D[xSt + 1])) / 2;
				xPix1 = 2 * XPixel(x2D[xSt]) - xPix1;
				for (int x = xSt; x < xEt && p < z2D.length; x++)
				{
					xPix0 = xPix1;
					try
					{
						xPix1 = (XPixel(x2D[x + 1]) + XPixel(x2D[x])) / 2;
						w = Math.abs(xPix1 - xPix0);
					}
					catch (final Exception e)
					{
						w = 2 * (XPixel(x2D[xEt - 1]) - xPix1);
					}
					/*
					 * pix = (int) (255 * (z2D[p++] - z2D_min) / (z2D_max - z2D_min));
					 */
					pix = (int) (255 * (z2D[p++] - currMin) / (currMax - currMin));
					pix = (pix > 255) ? 255 : pix;
					pix = (pix < 0) ? 0 : pix;
					drawRectagle(g2, cm, xPix0, yPix1, w, h, pix);
				}
			}
		}
		catch (final Exception exc)
		{}
	}

	public Vector<Polygon> ToPolygons(Signal sig, Dimension d)
	{
		return ToPolygons(sig, d, false);
	}

	public Vector<Polygon> ToPolygons(Signal sig, Dimension d, boolean appendMode)
	{
		try
		{
			// System.out.println("ToPolygons "+sig.name+" "+appendMode);
			return ToPolygonsDoubleX(sig, d);
		}
		catch (final Exception exc)
		{
			exc.printStackTrace();
		}
		return null;
	}

	public Vector<Polygon> ToPolygonsDoubleX(Signal sig, Dimension d)
	{
		int i, j, curr_num_points, start_x;
		double max_y, min_y, curr_y;
		Vector<Polygon> curr_vect = new Vector<>(5);
		int xpoints[], ypoints[];
		Polygon curr_polygon = null;
		int pol_idx = 0;
		min_y = max_y = sig.getY(0);
		xpoints = new int[sig.getNumPoints()];
		ypoints = new int[sig.getNumPoints()];
		// xpoints = new int[2*sig.getNumPoints()];
		// ypoints = new int[2*sig.getNumPoints()];
		curr_num_points = 0;
		i = j = 0;
		int end_point = sig.getNumPoints();
		ComputeFactors(d);
		try
		{
			final double x[] = sig.getX();
			final float y[] = sig.getY();
			for (i = 0; i < x.length && x[i] < xmin; i++);
			if (i > 0)
				i--;
			min_y = max_y = y[i];
			j = i + 1;
			start_x = XPixel(x[i]);
			double first_y, last_y;
			while (j < end_point) // sig.getNumPoints() && sig.x_double[j] < xmax + dt)
			{
				first_y = last_y = y[i];
				for (j = i + 1; j < x.length && // !Float.isNaN(sig.y[j]) &&
						(pol_idx >= sig.getNumNaNs() || j != sig.getNaNs()[pol_idx]) && XPixel(x[j]) == start_x; j++)
				{
					last_y = curr_y = y[j];
					if (curr_y < min_y)
						min_y = curr_y;
					if (curr_y > max_y)
						max_y = curr_y;
				}
				if (max_y > min_y)
				{
					if (first_y == min_y)
					{
						xpoints[curr_num_points] = start_x;
						ypoints[curr_num_points] = YPixel(first_y);
						curr_num_points++;
						if (last_y == max_y)
						{
							xpoints[curr_num_points] = start_x;
							ypoints[curr_num_points] = YPixel(last_y);
							curr_num_points++;
						}
						else
						{
							xpoints[curr_num_points] = start_x;
							ypoints[curr_num_points] = YPixel(max_y);
							curr_num_points++;
							xpoints[curr_num_points] = start_x;
							ypoints[curr_num_points] = YPixel(last_y);
							curr_num_points++;
						}
					}
					else if (first_y == max_y)
					{
						xpoints[curr_num_points] = start_x;
						ypoints[curr_num_points] = YPixel(first_y);
						curr_num_points++;
						if (last_y == min_y)
						{
							xpoints[curr_num_points] = start_x;
							ypoints[curr_num_points] = YPixel(last_y);
							curr_num_points++;
						}
						else
						{
							xpoints[curr_num_points] = start_x;
							ypoints[curr_num_points] = YPixel(min_y);
							curr_num_points++;
							xpoints[curr_num_points] = start_x;
							ypoints[curr_num_points] = YPixel(last_y);
							curr_num_points++;
						}
					}
					else // first_y != min_y && first_y != max_y
					{
						xpoints[curr_num_points] = start_x;
						ypoints[curr_num_points] = YPixel(first_y);
						curr_num_points++;
						if (last_y == min_y)
						{
							xpoints[curr_num_points] = start_x;
							ypoints[curr_num_points] = YPixel(max_y);
							curr_num_points++;
							xpoints[curr_num_points] = start_x;
							ypoints[curr_num_points] = YPixel(last_y);
							curr_num_points++;
						}
						else if (last_y == max_y)
						{
							xpoints[curr_num_points] = start_x;
							ypoints[curr_num_points] = YPixel(min_y);
							curr_num_points++;
							xpoints[curr_num_points] = start_x;
							ypoints[curr_num_points] = YPixel(last_y);
							curr_num_points++;
						}
						else
						{
							xpoints[curr_num_points] = start_x;
							ypoints[curr_num_points] = YPixel(min_y);
							curr_num_points++;
							xpoints[curr_num_points] = start_x;
							ypoints[curr_num_points] = YPixel(max_y);
							curr_num_points++;
							xpoints[curr_num_points] = start_x;
							ypoints[curr_num_points] = YPixel(last_y);
							curr_num_points++;
						}
					}
				}
				else
				{
					xpoints[curr_num_points] = start_x;
					ypoints[curr_num_points] = YPixel(max_y);
					curr_num_points++;
				}
				if (j == x.length || j >= end_point || Double.isNaN(y[j])) // || sig.x_double[j] >= xmax)
				{
					curr_polygon = new Polygon(xpoints, ypoints, curr_num_points);
					curr_vect.addElement(curr_polygon);
					pol_idx++;
					curr_num_points = 0;
					if (j < x.length) // need to raise pen
					{
						while (j < x.length && Double.isNaN(y[j]))
							j++;
					}
				}
				if (j < end_point) // sig.getNumPoints())
				{
					start_x = XPixel(x[j]);
					max_y = min_y = y[j];
					i = j;
					if (sig.isIncreasingX() && x[j] > xmax)
						end_point = j + 1;
				}
			}
		}
		catch (final Exception exc)
		{
			// Exception is generated when signal is emty
			// System.out.println("Waveform Metrics exception: " + exc);
		}
		if (sig.getMode1D() == Signal.MODE_STEP)
		{
			final Vector<Polygon> v = new Vector<>();
			int x[];
			int y[];
			for (i = 0; i < curr_vect.size(); i++)
			{
				curr_polygon = curr_vect.elementAt(i);
				final int np = curr_polygon.npoints * 2 - 1;
				x = new int[np];
				y = new int[np];
				for (i = 0, j = 0; i < curr_polygon.npoints; i++, j++)
				{
					x[j] = curr_polygon.xpoints[i];
					y[j] = curr_polygon.ypoints[i];
					j++;
					if (j == np)
						break;
					x[j] = curr_polygon.xpoints[i + 1];
					y[j] = curr_polygon.ypoints[i];
				}
				curr_polygon = new Polygon(x, y, np);
				v.addElement(curr_polygon);
			}
			curr_vect = v;
		}
		return curr_vect;
	}

	final public boolean XLog()
	{
		return x_log;
	}

	final public double XMax()
	{
		return xmax;
	}

	final public double XMin()
	{
		return xmin;
	}

	final public int XPixel(double x)
	{
		final double xpix = x * FACT_X + OFS_X;
		if (xpix >= MAX_VALUE)
			return INT_MAX_VALUE;
		if (xpix <= MIN_VALUE)
			return INT_MIN_VALUE;
		return (int) xpix;
	}

	final public int XPixel(double x, Dimension d)
	{
		double ris;
		if (x_log)
		{
			if (x < MIN_LOG)
				x = MIN_LOG;
			x = Math.log(x) / LOG10;
		}
		ris = (x_offset + x_range * (x - xmin) / xrange) * d.width + 0.5;
		if (ris > 20000)
			ris = 20000;
		if (ris < -20000)
			ris = -20000;
		return (int) ris;
	}

	final public double XRange()
	{
		return xmax - xmin;
	}

	final public double XValue(int x, Dimension d)
	{
		final double ris = ((x - 0.5) / d.width - x_offset) * xrange / x_range + xmin;
		if (x_log)
			return Math.exp(LOG10 * ris);
		else
			return ris;
	}

	final public boolean YLog()
	{
		return y_log;
	}

	final public double YMax()
	{
		return ymax;
	}

	final public double YMin()
	{
		return ymin;
	}

	final public int YPixel(double y)
	{
		if (y_log)
		{
			if (y < MIN_LOG)
				y = MIN_LOG;
			y = Math.log(y) / LOG10;
		}
		final double ypix = y * FACT_Y + OFS_Y;
		if (ypix >= MAX_VALUE)
			return INT_MAX_VALUE;
		if (ypix <= MIN_VALUE)
			return INT_MIN_VALUE;
		return (int) ypix;
	}

	final public int YPixel(double y, Dimension d)
	{
		if (y_log)
		{
			if (y < MIN_LOG)
				y = MIN_LOG;
			y = Math.log(y) / LOG10;
		}
		double ris = (y_range * (ymax - y) / yrange) * d.height + 0.5;
		if (ris > 20000)
			ris = 20000;
		if (ris < -20000)
			ris = -20000;
		return (int) ris;
	}

	final public double YRange()
	{
		return ymax - ymin;
	}

	final public double YValue(int y, Dimension d)
	{
		final double ris = ymax - ((y - 0.5) / d.height) * yrange / y_range;
		if (y_log)
			return Math.exp(LOG10 * ris);
		else
			return ris;
	}
}

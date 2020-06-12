package mds.jtraverser.dialogs;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.FontMetrics;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.RenderingHints;
import java.awt.Stroke;
import java.util.ArrayList;
import java.util.List;
import javax.swing.JDialog;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.WindowConstants;
import mds.MdsException;
import mds.MdsMisc;
import mds.MdsMisc.DataStruct;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Signal;
import mds.data.descriptor_s.NODE;

/**
 * @author Rodrigo
 */
public class GraphPanel extends JPanel
{
	private static final class Params
	{
		public final float min, delta;
		public final boolean isX;

		public Params(final float min, final float max, final boolean isX)
		{
			this.min = min;
			this.delta = max - min;
			this.isX = isX;
		}
	}

	private static final class XY
	{
		public final float X, Y;

		public XY(final float X, final float Y)
		{
			this.X = X;
			this.Y = Y;
		}

		@Override
		public final String toString()
		{
			return new StringBuilder(64).append('(').append(this.X).append(", ").append(this.Y).append(')').toString();
		}
	}

	private static final long serialVersionUID = 1L;
	private static final boolean yaxis = false;
	private static final boolean xaxis = true;
	private static final Stroke GRAPH_STROKE = new BasicStroke(2f);
	private static final Color gridColor = new Color(200, 200, 200, 200);
	private static final Color lineColor = new Color(0, 0, 255, 180);
	private static final int labelPadding = 25;
	private static final int numberYDivisions = 10;
	private static final int padding = 25;
	private static final int pointWidth = 4;
	private static final int padding_total = 2 * GraphPanel.padding + GraphPanel.labelPadding;
	private static final boolean use_new_misc = false;

	private final static void drawGraph(final Graphics2D g2, final List<Point> curve)
	{
		final Stroke oldStroke = g2.getStroke();
		g2.setColor(GraphPanel.lineColor);
		g2.setStroke(GraphPanel.GRAPH_STROKE);
		for (int i = 0; i < curve.size() - 1; i++)
			g2.drawLine(curve.get(i).x, curve.get(i).y, curve.get(i + 1).x, curve.get(i + 1).y);
		g2.setStroke(oldStroke);
	}

	private static final List<XY> getScores(final float[] x, final float[] y, final int imax)
	{
		if (y == null)
			return null;
		final int len_in = x == null ? y.length : x.length;
		final double p = (x == null ? len_in - 1 : (x[x.length - 1] - x[0])) / (imax * 2.);
		double v = p - Double.MIN_VALUE + (x == null ? 0 : x[0]);
		if (y.length / imax > 5)
		{
			final List<XY> new_scores = new ArrayList<>(imax * 2);
			float max = Float.NEGATIVE_INFINITY, min = Float.POSITIVE_INFINITY;
			int th = 0;
			for (int i = 0; i < len_in; i++)
			{
				if (min > y[i])
					min = y[i];
				if (max < y[i])
					max = y[i];
				th++;
				if (i + 1 >= len_in || (x == null ? i + 1 : x[i + 1]) >= v)
				{
					if (th > 2)
					{
						new_scores.add(new XY((float) (v - p / 2), max));
						new_scores.add(new XY((float) (v - p / 2), min));
					}
					else
						for (; th-- > 0;)
							new_scores.add(new XY(x == null ? i - th : x[i - th], y[i - th]));
					max = Float.NEGATIVE_INFINITY;
					min = Float.POSITIVE_INFINITY;
					v += p;
					th = 0;
				}
			}
			return new_scores;
		}
		if (x == null)
		{
			final List<XY> new_scores = new ArrayList<>(y.length);
			for (int i = 0; i < y.length; i++)
				new_scores.add(new XY(i, y[i]));
			return new_scores;
		}
		final List<XY> new_scores = new ArrayList<>(x.length);
		for (int i = 0; i < x.length; i++)
			new_scores.add(new XY(x[i], y[i]));
		return new_scores;
	}

	public static final JDialog newPlot(Descriptor<?> sig, final Frame frame, final String title)
	{
		sig = sig.getLocal();
		final float[] dat = sig != null ? sig.toFloatArray() : null;
		if (Thread.currentThread().isInterrupted())
			return null;
		final float[] dim = sig instanceof Signal ? ((Signal) sig).getDimension().toFloatArray() : null;
		if (Thread.currentThread().isInterrupted())
			return null;
		return GraphPanel.newPlot(dat, dim, frame, title);
	}

	public static final JDialog newPlot(final Descriptor<?> sig, final Frame frame, final String tree, final long shot,
			final String expr)
	{
		final String title = new StringBuilder(expr.length() + tree.length() + 10).append(expr).append(" @ Tree(")
				.append(tree).append(',').append(shot).append(')').toString();
		return GraphPanel.newPlot(sig, frame, title);
	}

	public static JDialog newPlot(final float[] dat, final float[] dim, final Frame frame, final String title)
	{
		final JDialog dialog = new JDialog(frame, title);
		final GraphPanel gp = new GraphPanel();
		dialog.getContentPane().add(gp);
		dialog.pack();
		gp.setScores(dim, dat);
		if (Thread.currentThread().isInterrupted())
		{
			dialog.dispose();
			return null;
		}
		dialog.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
		dialog.setLocationRelativeTo(frame);
		dialog.setVisible(true);
		return dialog;
	}

	public static final void plotNode(final NODE<?> node, final Frame frame, final String title)
	{
		try
		{
			final MdsMisc misc = new MdsMisc(node.getTree().getMds());
			final String expr = node.decompile();
			final DataStruct ds;
			if (use_new_misc)
				ds = misc.miscGetXYSignal(node.getTree(), expr, null, Float.NEGATIVE_INFINITY, Float.POSITIVE_INFINITY,
						3200);
			else
				ds = misc.miscGetXYSignalLongTimes(node.getTree(), "__s=" + expr, "DIM_OF(__s)", Long.MIN_VALUE,
						Long.MAX_VALUE, 3200);
			GraphPanel.newPlot(ds.y, ds.to_float(), frame, title);
		}
		catch (final MdsException.MdsAbortException e)
		{/**/}
		catch (final MdsException e)
		{
			JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), e.getMessage(), "Display Signal",
					JOptionPane.ERROR_MESSAGE);
		}
	}

	public static final void plotNodeSig(final NODE<?> node, final Frame frame, final String title)
	{
		try
		{
			final NODE<?> datanode = node.followReference();
			final Descriptor<?> sig = datanode.getRecord();
			GraphPanel.newPlot(sig, frame, title);
		}
		catch (final MdsException.MdsAbortException e)
		{/**/}
		catch (final MdsException e)
		{
			JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), e.getMessage(), "Display Signal",
					JOptionPane.ERROR_MESSAGE);
		}
	}

	private List<XY> scores;

	public GraphPanel()
	{
		this(null);
	}

	public GraphPanel(final List<XY> scores)
	{
		this.setPreferredSize(new Dimension(800, 400));
		this.scores = scores;
	}

	private final void drawAxes(final Graphics2D g2)
	{
		g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
		g2.setColor(Color.WHITE);
		g2.fillRect(this.getOrigin(GraphPanel.xaxis), GraphPanel.padding, this.getSize(GraphPanel.xaxis),
				-this.getSize(GraphPanel.yaxis));
		final Params paramsX = this.getParams(GraphPanel.xaxis), paramsY = this.getParams(GraphPanel.yaxis);
		this.drawAxis(g2, paramsX);
		this.drawAxis(g2, paramsY);
		this.drawScore(g2, paramsX, paramsY);
	}

	/** draws an axis with labels **/
	private final void drawAxis(final Graphics2D g2, final Params params)
	{
		final int b0 = this.getOrigin(!params.isX);
		final int bt = b0 + (params.isX ? -GraphPanel.pointWidth : GraphPanel.pointWidth);
		final int b1 = b0 + this.getSize(!params.isX);
		for (int i = 0; i <= GraphPanel.numberYDivisions; i++)
		{
			final float rel = (float) i / GraphPanel.numberYDivisions;
			final float value = params.min + params.delta * rel;
			final int a = this.getPoint(rel, params.isX);
			g2.setColor(i > 0 ? GraphPanel.gridColor : Color.BLACK);
			if (params.isX)
				g2.drawLine(a, bt - 1, a, b1);
			else
				g2.drawLine(bt + 1, a, b1, a);
			g2.setColor(Color.BLACK);
			final String label = String.format("%.2g", Float.valueOf(value));
			final FontMetrics metrics = g2.getFontMetrics();
			final int labelWidth = metrics.stringWidth(label);
			if (params.isX)
			{
				g2.drawString(label, a - labelWidth / 2, b0 + metrics.getHeight() + 3);
				g2.drawLine(a, b0, a, bt);
			}
			else
			{
				g2.drawString(label, bt - labelWidth - 7, a + (metrics.getHeight() / 2) - 3);
				g2.drawLine(b0, a, bt, a);
			}
		}
	}

	private final void drawScore(final Graphics2D g2, final Params paramsX, final Params paramsY)
	{
		final List<Point> curve = new ArrayList<>();
		for (int i = 0; i < this.scores.size(); i++)
			curve.add(this.getPoint(i, paramsX, paramsY));
		GraphPanel.drawGraph(g2, curve);
	}

	private final int getOrigin(final boolean isX)
	{
		if (isX)
			return GraphPanel.padding + GraphPanel.labelPadding;
		return GraphPanel.padding - this.getSize(isX);
	}

	private final Params getParams(final boolean isX)
	{
		if (isX)
			return new Params(this.scores.get(0).X, this.scores.get(this.scores.size() - 1).X, isX);
		float min = Float.POSITIVE_INFINITY, max = Float.NEGATIVE_INFINITY;
		for (final XY pair : this.scores)
		{
			if (min > pair.Y)
				min = pair.Y;
			if (max < pair.Y)
				max = pair.Y;
		}
		if (min < max)
			return new Params(min, max, isX);
		return new Params(min - .5f, max + .5f, isX);
	}

	private final int getPoint(final float rel, final boolean isX)
	{
		return this.getOrigin(isX) + (int) (rel * this.getSize(isX));
	}

	private final int getPoint(final int idx, final Params params)
	{
		final float val = params.isX ? this.scores.get(idx).X : this.scores.get(idx).Y;
		return this.getPoint((val - params.min) / params.delta, params.isX);
	}

	private final Point getPoint(final int idx, final Params paramsX, final Params paramsY)
	{
		return new Point(this.getPoint(idx, paramsX), this.getPoint(idx, paramsY));
	}

	public final List<XY> getScores()
	{ return this.scores; }

	private final int getSize(final boolean isX)
	{
		return isX ? this.getWidth() - GraphPanel.padding_total : GraphPanel.padding_total - this.getHeight();
	}

	@Override
	protected final void paintComponent(final Graphics g)
	{
		super.paintComponent(g);
		if (this.scores == null || this.scores.size() == 0)
			g.drawString("no data", this.getWidth() / 2, this.getHeight() / 2);
		else if (this.scores.size() == 1)
			g.drawString(this.scores.get(0).toString(), this.getWidth() / 2, this.getHeight() / 2);
		else
			this.drawAxes((Graphics2D) g);
	}

	public final void setScores(final float[] x, final float[] y)
	{
		this.setScores(GraphPanel.getScores(x, y, this.getWidth()));
	}

	public final void setScores(final List<XY> scores)
	{
		this.scores = scores;
		this.invalidate();
		this.repaint();
	}
}

package mds.wave;

import javax.swing.*;

import mds.wave.Signal;

import java.awt.event.*;
import java.awt.*;
import java.util.*;

public class WaveformEditor extends Waveform
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	public static final float MIN_STEP = 1E-6F;
	static float[] copyX, copyY;

	public static void main(String args[])
	{
		final float x[] = new float[10];
		final float y[] = new float[10];
		for (int i = 0; i < 10; i++)
		{
			x[i] = (float) (i / 10.);
			y[i] = 0;
		}
		final WaveformEditor we = new WaveformEditor();
		// WaveformEditor we = new WaveformEditor(x,y, -10F, 20F);
		we.setWaveform(x, y, -10F, 20F);
		final JFrame frame = new JFrame("Test WaveformEditor");
		frame.setSize(400, 300);
		frame.getContentPane().add(we);
		frame.pack();
		frame.setVisible(true);
	}

	float[] currentX, currentY;
	float minY, maxY;
	int closestIdx = -1;
	Vector<WaveformEditorListener> listeners = new Vector<>();
	protected boolean editable = false;

	public WaveformEditor()
	{
		super();
		setupCopyPaste();
	}

	public WaveformEditor(float[] x, float[] y, float minY, float maxY)
	{
		super(new Signal(x, y, x.length, x[0], x[x.length - 1], minY, maxY));
		SetMarker(1);
		currentX = x;
		currentY = y;
		this.minY = minY;
		this.maxY = maxY;
		setupCopyPaste();
	}

	public synchronized void addWaveformEditorListener(WaveformEditorListener listener)
	{
		listeners.add(listener);
	}

	int convertXPix(float x)
	{
		return wm.XPixel(x, getWaveSize());
	}

	int convertYPix(float y)
	{
		return wm.YPixel(y, getWaveSize());
	}

	public synchronized void notifyUpdate(float[] waveX, float[] waveY, int newIdx)
	{
		for (final WaveformEditorListener l : listeners)
			l.waveformUpdated(waveX, waveY, newIdx);
	}

	@Override
	public void print(Graphics g)
	{
		System.out.println("WAVE PRINT");
		paint(g, getSize(), 1);
	}

	public synchronized void removeWaveformEditorListener(WaveformEditorListener listener)
	{
		listeners.remove(listener);
	}

	public void setEditable(boolean editable)
	{ this.editable = editable; }

	@Override
	protected void setMouse()
	{
		addMouseListener(new MouseAdapter()
		{
			@Override
			public void mousePressed(MouseEvent e)
			{
				int newIdx = -1;
				final int currX = e.getX();
				final int currY = e.getY();
				int minDist = Integer.MAX_VALUE;
				int prevIdx = -1;
				for (int i = closestIdx = 0; i < currentX.length; i++)
				{
					if (prevIdx == -1 && i < currentX.length - 1 && convertXPix(currentX[i + 1]) > currX)
						prevIdx = i;
					// (float currDist = (float)Math.abs(currX - currentX[i]);
					final int currentXPix = convertXPix(currentX[i]);
					final int currentYPix = convertYPix(currentY[i]);
					final int currDist = (currX - currentXPix) * (currX - currentXPix)
							+ (currY - currentYPix) * (currY - currentYPix);
					if (currDist < minDist)
					{
						minDist = currDist;
						closestIdx = i;
					}
				}
				notifyUpdate(currentX, currentY, closestIdx);
				if (!editable)
					return;
				if ((e.getModifiersEx() & InputEvent.BUTTON3_DOWN_MASK) != 0) // If MB3
				{
					if ((e.getModifiersEx() & InputEvent.SHIFT_DOWN_MASK) != 0) // Pont deletion
					{
						if (closestIdx != 0 && closestIdx != currentX.length - 1)
						{
							final float[] newCurrentX = new float[currentX.length - 1];
							final float[] newCurrentY = new float[currentY.length - 1];
							int j;
							for (int i = j = 0; i < closestIdx; i++, j++)
							{
								newCurrentX[i] = currentX[i];
								newCurrentY[i] = currentY[i];
							}
							for (int i = closestIdx + 1; i < currentX.length; i++, j++)
							{
								newCurrentX[j] = currentX[i];
								newCurrentY[j] = currentY[i];
							}
							currentX = newCurrentX;
							currentY = newCurrentY;
						}
					}
					else
					{
						final float newX = convertX(e.getX());
						final float[] newCurrentX = new float[currentX.length + 1];
						final float[] newCurrentY = new float[currentY.length + 1];
						int j;
						for (int i = j = 0; i <= prevIdx; i++, j++)
						{
							newCurrentX[i] = currentX[i];
							newCurrentY[i] = currentY[i];
						}
						newCurrentX[j] = newX;
						newCurrentY[j] = currentY[j - 1] + (newX - currentX[j - 1]) * (currentY[j] - currentY[j - 1])
								/ (currentX[j] - currentX[j - 1]);
						j++;
						for (int i = prevIdx + 1; i < currentX.length; i++, j++)
						{
							newCurrentX[j] = currentX[i];
							newCurrentY[j] = currentY[i];
						}
						currentX = newCurrentX;
						currentY = newCurrentY;
						newIdx = prevIdx + 1;
					}
					final Signal newSig = new Signal(currentX, currentY, currentX.length, currentX[0],
							currentX[currentX.length - 1], minY, maxY);
					newSig.setMarker(1);
					Update(newSig);
					notifyUpdate(currentX, currentY, newIdx);
				}
			}

			@Override
			public void mouseReleased(MouseEvent e)
			{
				closestIdx = -1;
			}
		});
		addMouseMotionListener(new MouseMotionAdapter()
		{
			@Override
			public void mouseDragged(MouseEvent e)
			{
				if (!editable)
					return;
				synchronized (WaveformEditor.this)
				{
					if (closestIdx == -1)
						return;
					float currX;
					float currY;
					try
					{
						currX = convertX(e.getX());
						currY = convertY(e.getY());
					}
					catch (final Exception exc)
					{
						return;
					}
					if (closestIdx > 0 && closestIdx < currentX.length - 1)
					{
						if (currX < currentX[closestIdx - 1] + MIN_STEP)
							currX = currentX[closestIdx - 1] + MIN_STEP;
						if (currX > currentX[closestIdx + 1] - MIN_STEP)
							currX = currentX[closestIdx + 1] - MIN_STEP;
					}
					else
						currX = currentX[closestIdx];
					currentX[closestIdx] = currX;
					if (currY < minY)
						currY = minY;
					if (currY > maxY)
						currY = maxY;
					currentY[closestIdx] = currY;
					final Signal newSig = new Signal(currentX, currentY, currentX.length, currentX[0],
							currentX[currentX.length - 1], minY, maxY);
					newSig.setMarker(1);
					Update(newSig);
					notifyUpdate(currentX, currentY, -1);
				}
			}
		});
	}

	protected void setupCopyPaste()
	{
		// enableEvents(AWTEvent.KEY_EVENT_MASK);
		addMouseListener(new MouseAdapter()
		{
			@Override
			public void mousePressed(MouseEvent e)
			{
				requestFocus();
			}
		});
		addKeyListener(new KeyListener()
		{
			@Override
			public void keyPressed(KeyEvent ke)
			{
				if ((ke.getModifiersEx() & InputEvent.CTRL_DOWN_MASK) != 0 && (ke.getKeyCode() == KeyEvent.VK_C))
				{
					copyX = new float[currentX.length];
					copyY = new float[currentY.length];
					for (int i = 0; i < currentX.length; i++)
					{
						if (i >= currentY.length)
							break;
						copyX[i] = currentX[i];
						copyY[i] = currentY[i];
					}
				}
				if ((ke.getModifiersEx() & InputEvent.CTRL_DOWN_MASK) != 0 && (ke.getKeyCode() == KeyEvent.VK_V))
				{
					if (copyX == null)
						return;
					final Signal sig = new Signal(copyX, copyY, copyX.length, copyX[0], copyX[copyX.length - 1], minY,
							maxY);
					sig.setMarker(1);
					currentX = new float[copyX.length];
					currentY = new float[copyY.length];
					for (int i = 0; i < copyX.length; i++)
					{
						currentX[i] = copyX[i];
						currentY[i] = copyY[i];
					}
					Update(sig);
					notifyUpdate(currentX, currentY, currentX.length - 1);
				}
			}

			@Override
			public void keyReleased(KeyEvent ke)
			{}

			@Override
			public void keyTyped(KeyEvent ke)
			{}
		});
	}

	public void setWaveform(float[] x, float[] y, float minY, float maxY)
	{
		final Signal sig = new Signal(x, y, x.length, x[0], x[x.length - 1], minY, maxY);
		sig.setMarker(1);
		currentX = new float[x.length];
		currentY = new float[y.length];
		for (int i = 0; i < x.length; i++)
		{
			currentX[i] = x[i];
			currentY[i] = y[i];
		}
		this.minY = minY;
		this.maxY = maxY;
		Update(sig);
	}
}

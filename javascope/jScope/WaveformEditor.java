package jScope;

import jScope.Signal;
import javax.swing.*;
import java.awt.event.*;
import java.awt.*;
import java.util.*;

public class WaveformEditor
    extends Waveform
{
    float[] currentX, currentY;
    float minY, maxY;
    int closestIdx = -1;
    public static final float MIN_STEP = 1E-6F;
    Vector<WaveformEditorListener> listeners = new Vector<>();
    protected boolean editable = false;

    static float[] copyX, copyY;

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

    public void setWaveform(float[] x, float[] y, float minY, float maxY)
    {
        Signal sig = new Signal(x, y, x.length, x[0], x[x.length - 1], minY,
                                maxY);
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

    protected void setupCopyPaste()
    {
        //enableEvents(AWTEvent.KEY_EVENT_MASK);
        addMouseListener(new MouseAdapter()
        {
            public void mousePressed(MouseEvent e)
            {
                requestFocus();
            }
        });
        addKeyListener(new KeyListener()
        {
            public void keyPressed(KeyEvent ke)
            {
                if ( (ke.getModifiersEx() & KeyEvent.CTRL_DOWN_MASK) != 0 &&
                    (ke.getKeyCode() == KeyEvent.VK_C))
                {
                    copyX = new float[currentX.length];
                    copyY = new float[currentY.length];
                    for (int i = 0; i < currentX.length; i++)
                    {
                        if (i >= currentY.length)break;
                        copyX[i] = currentX[i];
                        copyY[i] = currentY[i];
                    }
                }
                if ( (ke.getModifiersEx() & KeyEvent.CTRL_DOWN_MASK) != 0 &&
                    (ke.getKeyCode() == KeyEvent.VK_V))
                {
                    if (copyX == null)return;
                    Signal sig = new Signal(copyX, copyY, copyX.length, copyX[0],
                                            copyX[copyX.length - 1], minY, maxY);
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

            public void keyReleased(KeyEvent ke)
            {}

            public void keyTyped(KeyEvent ke)
            {}
        }
        );
    }

    int convertXPix(float x)
    {
        return wm.XPixel(x, getWaveSize());
    }
    int convertYPix(float y)
    {
        return wm.YPixel(y, getWaveSize());
    }

    protected void setMouse()
    {
        addMouseListener(new MouseAdapter()
        {
            public void mousePressed(MouseEvent e)
            {
                int newIdx = -1;
                int currX = e.getX();
                int currY = e.getY();
                int minDist = Integer.MAX_VALUE;
                int prevIdx = -1;
                for (int i = closestIdx = 0; i < currentX.length; i++)
                {
                    if (prevIdx == -1 && i < currentX.length - 1 &&
                        convertXPix(currentX[i + 1]) > currX)
                        prevIdx = i;
                    //(float currDist = (float)Math.abs(currX - currentX[i]);
                    int currentXPix = convertXPix(currentX[i]);
                    int currentYPix = convertYPix(currentY[i]);
                     int currDist = (currX - currentXPix) *
                        (currX - currentXPix) +
                        (currY - currentYPix) * (currY - currentYPix);
                    if (currDist < minDist)
                    {
                        minDist = currDist;
                        closestIdx = i;
                    }
                }
                 notifyUpdate(currentX, currentY, closestIdx);
                if (!editable)return;
                if ( (e.getModifiersEx() & MouseEvent.BUTTON3_DOWN_MASK) != 0) //If MB3
                {
                    if ( (e.getModifiersEx() & MouseEvent.SHIFT_DOWN_MASK) != 0) //Pont deletion
                    {
                        if (closestIdx != 0 &&
                            closestIdx != currentX.length - 1)
                        {
                            float[] newCurrentX = new float[currentX.length - 1];
                            float[] newCurrentY = new float[currentY.length - 1];
                            int j;
                            for (int i = j = 0; i < closestIdx; i++, j++)
                            {
                                newCurrentX[i] = currentX[i];
                                newCurrentY[i] = currentY[i];
                            }
                            for (int i = closestIdx + 1; i < currentX.length; i++,
                                 j++)
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
                        float newX = convertX(e.getX());
                        float[] newCurrentX = new float[currentX.length + 1];
                        float[] newCurrentY = new float[currentY.length + 1];
                        int j;
                        for (int i = j = 0; i <= prevIdx; i++, j++)
                        {
                            newCurrentX[i] = currentX[i];
                            newCurrentY[i] = currentY[i];
                        }
                        newCurrentX[j] = newX;
                        newCurrentY[j] = currentY[j - 1] +
                            (newX - currentX[j - 1]) *
                            (currentY[j] - currentY[j - 1]) /
                            (currentX[j] - currentX[j - 1]);
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
                    Signal newSig = new Signal(currentX, currentY,
                                               currentX.length, currentX[0],
                                               currentX[currentX.length - 1],
                                               minY, maxY);
                    newSig.setMarker(1);
                    Update(newSig);
                    notifyUpdate(currentX, currentY, newIdx);
                }
            }

            public void mouseReleased(MouseEvent e)
            {
                closestIdx = -1;
            }
        });

        addMouseMotionListener(new MouseMotionAdapter()
        {
            public void mouseDragged(MouseEvent e)
            {
                if (!editable)return;
                synchronized (WaveformEditor.this)
                {
                    if (closestIdx == -1)return;
                    float currX;
                    float currY;
                    try
                    {
                        currX = convertX(e.getX());
                        currY = convertY(e.getY());
                    }
                    catch (Exception exc)
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

                    if (currY < minY) currY = minY;
                    if (currY > maxY) currY = maxY;
                    currentY[closestIdx] = currY;
                    Signal newSig = new Signal(currentX, currentY,
                                               currentX.length, currentX[0],
                                               currentX[currentX.length - 1],
                                               minY, maxY);
                    newSig.setMarker(1);
                    Update(newSig);
                    notifyUpdate(currentX, currentY, -1);
                }
            }
        });

    }

    public synchronized void addWaveformEditorListener(WaveformEditorListener
        listener)
    {
        listeners.add(listener);
    }

    public synchronized void removeWaveformEditorListener(
        WaveformEditorListener listener)
    {
        listeners.remove(listener);
    }

    public synchronized void notifyUpdate(float[] waveX, float[] waveY,
                                          int newIdx)
    {
        for (WaveformEditorListener l : listeners)
            l.waveformUpdated(waveX, waveY, newIdx);
    }

    public void setEditable(boolean editable)
    {
        this.editable = editable;
    }

    public void print(Graphics g)
    {
        System.out.println("WAVE PRINT");
        paint(g, getSize(), 1);
    }

    public static void main(String args[])
    {
        float x[] = new float[10];
        float y[] = new float[10];
        for (int i = 0; i < 10; i++)
        {
            x[i] = (float) (i / 10.);
            y[i] = (float) 0;
        }
        WaveformEditor we = new WaveformEditor();
        //WaveformEditor we = new WaveformEditor(x,y, -10F, 20F);
        we.setWaveform(x, y, -10F, 20F);
        JFrame frame = new JFrame("Test WaveformEditor");
        frame.setSize(400, 300);
        frame.getContentPane().add(we);
        frame.pack();
        frame.setVisible(true);
    }
}

import javax.swing.*;
import java.awt.event.*;
import java.awt.*;
import java.util.*;

public class WaveformEditor extends Waveform
{
    float [] currentX, currentY;
    float minY, maxY;
    int closestIdx = -1;
    public static final float MIN_STEP = 1E-6F;
    Vector listeners = new Vector();
    protected boolean editable = false;
    
    public WaveformEditor(){super();}
    public WaveformEditor(float[]x, float [] y, float minY, float maxY) 
    {
        super(new Signal(x,y, x.length, x[0], x[x.length - 1], minY, maxY));
        SetMarker(1);
        currentX = x;
        currentY = y;
        this.minY = minY;
        this.maxY = maxY;
    }
    
    public void setWaveform(float[]x, float [] y, float minY, float maxY)
    {
        Signal sig = new Signal(x,y, x.length, x[0], x[x.length - 1], minY, maxY);
        sig.setMarker(1);
        currentX = x;
        currentY = y;
        this.minY = minY;
        this.maxY = maxY;
        Update(sig);
    }
        
    protected void setMouse()
    {
        addMouseListener(new MouseAdapter()  {
            public void mousePressed(MouseEvent e)
            {
                int newIdx = -1;
                float currX = convertX(e.getX());
                float currY = convertY(e.getY());
                float minDist = currentX[currentX.length-1] - currentX[0];
                int prevIdx = -1;
                for(int i = closestIdx = 0; i < currentX.length; i++)
                {
                    if(prevIdx == -1 && i < currentX.length - 1 && currentX[i + 1] > currX)
                        prevIdx = i;
                    //(float currDist = (float)Math.abs(currX - currentX[i]);
                    float currDist = (currX - currentX[i])*(currX - currentX[i])+
                       (currY - currentY[i])*(currY - currentY[i]); 
                    if( currDist < minDist)
                    {
                        minDist = currDist;
                        closestIdx = i;
                        notifyUpdate(currentX, currentY, closestIdx);
                    }
                }
                if(!editable) return; 
	           if((e.getModifiers() & Event.META_MASK) != 0) //If MB3
	           {
	                if((e.getModifiers() & Event.SHIFT_MASK) != 0) //Pont deletion
                    {
                        if(closestIdx != 0 && closestIdx != currentX.length - 1)
                        {
                            float[] newCurrentX = new float[currentX.length - 1];
                            float[] newCurrentY = new float[currentY.length - 1];
                            int j;
                            for(int i = j = 0; i < closestIdx; i++, j++)
                            {
                                newCurrentX[i] = currentX[i];
                                newCurrentY[i] = currentY[i];
                            }
                            for(int i = closestIdx + 1; i < currentX.length; i++, j++)
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
                        for(int i = j = 0; i <= prevIdx; i++, j++)
                        {
                            newCurrentX[i] = currentX[i];
                            newCurrentY[i] = currentY[i];
                        }
                        newCurrentX[j] = newX;
                        newCurrentY[j] = currentY[j - 1] + 
                            (newX - currentX[j - 1])*(currentY[j] - currentY[j-1])/(currentX[j] - currentX[j-1]);
                        j++;
                        for(int i = prevIdx + 1; i < currentX.length; i++, j++)
                        {
                            newCurrentX[j] = currentX[i];
                            newCurrentY[j] = currentY[i];
                        }
                        currentX = newCurrentX;            
                        currentY = newCurrentY;   
                        newIdx = prevIdx + 1;
                    }
                Signal newSig = new Signal(currentX,currentY, currentX.length, currentX[0],
                        currentX[currentX.length - 1], minY, maxY);
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
        
        addMouseMotionListener(new MouseMotionAdapter()  {
            public void mouseDragged(MouseEvent e)
            {
                if(!editable) return;
                synchronized(WaveformEditor.this)
                {
                    if(closestIdx == -1) return;
                    float currX; 
                    float currY;
                    try {
                        currX = convertX(e.getX());
                        currY = convertY(e.getY());
                    }catch(Exception exc){return;}
                    if(closestIdx > 0 && closestIdx < currentX.length - 1)
                    {
                        if(currX < currentX[closestIdx - 1]+ MIN_STEP)
                            currX = currentX[closestIdx - 1]+ MIN_STEP;
                        if(currX > currentX[closestIdx + 1]- MIN_STEP)
                            currX = currentX[closestIdx + 1]- MIN_STEP;
                    }
                    else
                        currX = currentX[closestIdx];
                    currentX[closestIdx] = currX;
                    
                    if(currY < minY) currY = minY;
                    if(currY > maxY) currY = maxY;
                    currentY[closestIdx] = currY;
                    Signal newSig = new Signal(currentX,currentY, currentX.length, currentX[0],
                        currentX[currentX.length - 1], minY, maxY);
                    newSig.setMarker(1);   
                    Update(newSig);
                    notifyUpdate(currentX, currentY, -1);
                }
            }
        });
                        
   }

    public synchronized void addWaveformEditorListener(WaveformEditorListener listener)
    {
        listeners.add(listener);
    }
    
    public synchronized void removeWaveformEditorListener(WaveformEditorListener listener)
    {
        listeners.remove(listener);
    }
    
    public synchronized void notifyUpdate(float [] waveX, float [] waveY, int newIdx)
    {
        for(int i = 0; i < listeners.size(); i++)
            ((WaveformEditorListener)listeners.elementAt(i)).waveformUpdated(waveX, waveY, newIdx);
    }
    
    public void setEditable(boolean editable){this.editable = editable;}
    
    public static void main(String args[])
    {
        float x[] = new float[10];
        float y[] = new float[10];
        for(int i = 0; i < 10; i++)
        {
            x[i] = (float)(i/10.);
            y[i] = (float)0;
        }
        WaveformEditor we = new WaveformEditor();
        //WaveformEditor we = new WaveformEditor(x,y, -10F, 20F);
        we.setWaveform(x,y, -10F, 20F);
        JFrame frame = new JFrame("Test WaveformEditor");
        frame.setSize(400, 300);
        frame.getContentPane().add(we);
        frame.pack();
        frame.show();
    }
}
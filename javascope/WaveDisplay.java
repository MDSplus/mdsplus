import java.applet.*;
import java.awt.*;
import java.net.URL;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.plaf.*;

public class WaveDisplay extends JApplet
{
    Waveform w;
    WavePopup wave_popup;
    private JFrame frame;

    WaveDisplay(JFrame f)
    {
        frame = f;
        init();
    }
    
    public void init()
    {
        JPanel panel = new JPanel();
        panel.setOpaque(false);
        w = new Waveform();
        
            float x[] = new float[1000];
            float y[] = new float[1000];
            for(int j = 0 ; j < 1000; j++)
            {
                x[j] = (float)(6.28/1000*j);
                y[j] = (float)(Math.sin(x[j]));
                continue;               
            }
            Signal s = new Signal(x, y);
        w.Update(s);

        wave_popup = new WavePopup();
        getContentPane().add(wave_popup);
        panel.addMouseListener( new MouseAdapter()
	        {
	           public void mousePressed(MouseEvent e)
	           {
	                Waveform w = (Waveform)e.getSource();
    	            if(wave_popup != null && w.GetMode() != Waveform.MODE_COPY)
		                wave_popup.Show(w, e.getX(), e.getY());
		       }
	        });


        w.SetGridMode(Grid.IS_DOTTED, false, false);
        panel.setLayout(new BorderLayout());
        panel.add(w, BorderLayout.CENTER);
        getContentPane().setLayout(new BorderLayout());
        getContentPane().add(panel, BorderLayout.CENTER);
    }

    /**
     * Add a signal to the waveform. Return signal index.
     * @param     x[] float vector of x values 
     * @param     y[] float vector of y values 
     * @param     name signal name string 
     */
    public int addSignal(float x[], float y[], String name)
    {
        return 0;
    }
    
    /**
     * Set color to the signal_idx.
     * @param     signal_idx index of signal 
     * @param     color  value
     */
    public void setColor(int signal_idx, Color color)
    {
    }

    /**
     * Set marker to the signal_idx plot.
     * @param     signal_idx index of signal 
     * @param     marker marker type  
     */
    public void setMarker(int signal_idx, int marker)
    {
    }

    public void setStep(int signal_idx, int step)
    {
    }

    public void setInterpolation(int signal_idx, boolean interpolation)
    {
    }

    public void setXlimits(float xmin, float xmax)
    {
    }
    
    public void setYlimits(float ymin, float ymax)
    {
    }

    public void calibrate(int signal_idx, float gain, float offset)
    {
    }

    public void removeSignal(int signal_idx)
    {
    }


    public int addSignal(URL url, String name)
    {
        return 0;
    }

    public int connectSignal(URL url, String name, int refesh_time)
    {
        return 0;
    }

    public void disconnectSignal(int signal_idx)
    {
    }

    public void reconnectSignal(int signal_idx)
    {
    }

//    public void addProtocol(DataAccess handler)
//    {
//    }


    static void main(String argc[])
    {
        JFrame f = new JFrame();
        WaveDisplay wd = new WaveDisplay(f);
        f.getContentPane().add(wd);
        f.pack();
        f.setSize(300, 300);
        f.show();
    }

}


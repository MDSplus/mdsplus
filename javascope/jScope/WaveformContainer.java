package jScope;

import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.print.PageFormat;
import java.awt.print.Printable;
import java.awt.print.PrinterException;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.StringReader;
import java.util.Properties;
import java.util.Vector;

import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.RepaintManager;

/**
 * A MultiWaveform container
 *
 * @see RowColumnLayout
 * @see RowColumnContainer
 * @see WaveformManager
 * @see MultiWaveform
 */
public class WaveformContainer extends RowColumnContainer implements WaveformManager,
                                                                     WaveformListener,
                                                                     Printable
{
   private   Waveform     sel_wave;
             int          mode = Waveform.MODE_ZOOM, grid_mode = Grid.IS_DOTTED ,
                          x_grid_lines = 5, y_grid_lines = 5;
   protected boolean      reversed = false;
   private   static Waveform     copy_waveform = null;
   protected Font         font = new Font("Helvetica", Font.PLAIN, 12);
   protected WavePopup    wave_popup;

   private   Vector<WaveContainerListener> wave_container_listener = new Vector<>();
   protected boolean      print_with_legend = false;
   protected boolean      print_bw = false;
   protected String       save_as_txt_directory = null;

    /**
     * Constructs a new WaveformContainer with a number of column and component in column.
     *
     * @param rows an array of number of component in column
     */
    public WaveformContainer(int rows[], boolean add_component)
    {
        super(rows, null);
        CreateWaveformContainer(add_component);
    }

    public WaveformContainer()
    {
      super();
      CreateWaveformContainer(true);
    }

    /**
     * Initialize WaveformContaine
     */
    private void CreateWaveformContainer(boolean create_component)
    {
        if(create_component)
        {
            Component c[] = CreateWaveComponents(getComponentNumber());
            super.add(c);
        }

        addMouseListener( new MouseAdapter()
	    {
	          public void mousePressed(MouseEvent e)
	          {
                        Dimension scr_dim = getToolkit().getScreenSize();
		        Waveform w = (Waveform)e.getSource();
		        int x = e.getX();
		        int y = e.getY();


		        if(wave_popup != null)
		        {
		            Point p = new Point();
	                Component co = w;
                    Dimension wp_size = wave_popup.getSize();

                    if(wp_size.height == 0 || wp_size.width == 0)
                    {
                        wave_popup.Show(w, x, y, 0, 0);
                        wp_size = wave_popup.getSize();
                    }

	                while(co != null)
	                {
	                    p.x += co.getLocation().x;
	                    p.y += co.getLocation().y;
	                    co = co.getParent();
	                }

                    int tran_x = 0;
                    int tran_y = 0;
		            if(y + p.y + wp_size.height > scr_dim.height)
		                tran_y = y + p.y + wp_size.height - scr_dim.height + 20;
		            if(x + p.x + wp_size.width > scr_dim.width)
		                tran_x = x + p.x + wp_size.width - scr_dim.width + 20;

		            wave_popup.Show(w, x, y, tran_x, tran_y);
                 }
	          }
	     });
     }

   /**
    * Return a new MultiWaveform component
    *
    * @return a MulitiWaveform components
    */
   protected Component CreateWaveComponent()
   {
        Component[] c = CreateWaveComponents(1);
        return c[0];
   }

   /**
    * Create an array of MultiWaveform
    *
    * @param num dimension of return array
    * @return an array of MultiWaveform
    */
   protected Component[] CreateWaveComponents(int num)
   {
        Component c[] = new Component[num];
        MultiWaveform      wave;
        for(int i = 0; i < c.length;i++)
        {
	       wave = new MultiWaveform();
	       wave.addWaveformListener(this);
	       SetWaveParams(wave);
	       c[i] = wave;
        }
        return c;
   }

   /**
    * Add MultiWaveform to the container
    *
    * @param c an array of MultiWaveform to add
    */
   public void AddComponents(Component c[])
   {
      super.add(c);
/*
      for(int i = 0; i < c.length; i++)
        if(c[i] instanceof Waveform)
        {
	        ((Waveform)c[i]).addWaveformListener(this);
	    }
*/
   }

   /**
    * Adds the specified waveform container listener to receive WaveContainerEvent
    * events from this WaveformContainer.
    *
    * @param l the waveform container listener
    */
   public synchronized void addWaveContainerListener(WaveContainerListener l)
   {
	    if (l == null) {
	        return;
	    }
        wave_container_listener.addElement(l);
   }

    /**
     * Removes the specified waveform container listener so that it no longer
     * receives WaveContainerEvent events from this WaveformContainer.
     *
     * @param l the waveform container listener
     */
    public synchronized void removeContainerListener(ActionListener l)
    {
	    if (l == null) {
	        return;
	    }
        wave_container_listener.removeElement(l);
    }

    /**
     * Processes wave container events occurring on this WaveformContainer by dispatching
     * them to any registered WaveContainerListener objects.
     *
     * @param e the wave container event
     */
    protected void dispatchWaveContainerEvent(WaveContainerEvent e)
    {
        if (wave_container_listener != null)
        {
            for(int i = 0; i < wave_container_listener.size(); i++)
            {
                ((WaveContainerListener)wave_container_listener.elementAt(i)).processWaveContainerEvent(e);
            }
        }
    }

    /**
     * process waveform event on this container
     *
     * @param e the waveform event
     */
    public void processWaveformEvent(WaveformEvent e)
    {
        Waveform w = (Waveform)e.getSource();
        switch(e.getID())
        {
            case WaveformEvent.BROADCAST_SCALE:
                AllSameScale(w);
            return;
            case WaveformEvent.COPY_PASTE:
                if(copy_waveform != null)
	                NotifyChange(w, copy_waveform);
            return;
            case WaveformEvent.COPY_CUT:
                 SetCopySource(w);
            return;
            case WaveformEvent.PROFILE_UPDATE:
            return;
            case WaveformEvent.POINT_UPDATE:
            case WaveformEvent.MEASURE_UPDATE:
                if(w.GetMode() == Waveform.MODE_POINT)
                {
                    double x = e.point_x;
                    double y = e.point_y;
                    if(w.IsImage())
                        x = e.delta_x;
		    else
			if(e.is_mb2)
			    AllSameXScaleAutoY(w);
                    //Set x to time_value allows pannels synchronization from 2D 
                    //signal viewed in MODE_YX  
                    if(!Double.isNaN(e.time_value))
                        x = e.time_value;
                    UpdatePoints(x, y, (Waveform)e.getSource());
                }
                /*
                if(!w.IsImage() && show_measure)
                {
                    e = new WaveformEvent(e.getSource(),
                                          WaveformEvent.MEASURE_UPDATE,
                                          e.point_x, e.point_y, e.delta_x,
                                          e.delta_y,
                                          0, e.signal_idx);
                }
                */
           break;
           case WaveformEvent.POINT_IMAGE_UPDATE:
           break;
        }

        WaveContainerEvent we = new WaveContainerEvent(this, e);
        dispatchWaveContainerEvent(we);
    }

    /**
     * Set popup menu to this container
     *
     * @param wave_popup the popup menu
     */
    public void setPopupMenu(WavePopup wave_popup)
    {
        this.wave_popup = wave_popup;
        wave_popup.setParent(this);
    }

   public Waveform GetWavePanel(int idx)
   {
       Component c = getGridComponent(idx);
       if(c instanceof MultiWaveform || c instanceof Waveform)
            return (Waveform)c;
        else
            return null;
   }

    /**
     * Set current MultiWaveform parameters
     *
     * @param w the MultiWaveform to set params
     */
    public void SetWaveParams(Waveform w)
    {
        boolean int_label = (grid_mode == 2 ? false : true);
        w.SetMode(mode);
        w.SetReversed(reversed);
        w.SetGridMode(grid_mode, int_label, int_label);
        w.SetGridSteps(x_grid_lines, y_grid_lines);
    }

    /**
     * Return indexn of an added MultiWaveform
     *
     * @param w The MultiWaveform
     * @return the MultiWaveform index
     */
    public int GetWaveIndex(Waveform w)
    {
	    int idx;
	    for(idx = 0; idx < getGridComponentCount() &&  GetWavePanel(idx) != w; idx++);
	    if(idx < getGridComponentCount())
	        return idx;
	    else
	        return -1;
    }

    public Point getWavePosition(Waveform w)
    {
        return getComponentPosition(w);
    }

    /**
     * Remove current MultiWaveform selected
     */
    public void RemoveSelection()
    {
        Waveform w;
	    for(int i = 0; i < getGridComponentCount() && copy_waveform != null; i++)
        {
	        w = GetWavePanel(i);
	        if(w != null)
	        {
	            if(w.IsCopySelected())
	            {
		            copy_waveform = null;
		            w.SetCopySelected(false);
		            break;
	            }
	        }
	    }
    }

    /**
     * Enable / disable show measurament
     *
     * @param state shoe measurament state
     */
    public void SetShowMeasure(boolean state)
    {
        if(state)
        {
            Waveform w;
	        for(int i = 0; i < getGridComponentCount(); i++)
	        {
	            w = GetWavePanel(i);
	            if(w != null)
	                w.show_measure = false;
	        }
        }
    }

    /*synchronized */public void UpdatePoints(double x, Waveform curr_w)
    {
        UpdatePoints(x, Double.NaN, curr_w);
    }

    /**
     * Update crosshair position
     *
     * @param curr_x x axis position
     * @param w a waveform to update cross
     * @see Waveform
     * @see MultiWaveform
     */
    /*synchronized */public void UpdatePoints(double x, double y, Waveform curr_w)
    {
        Waveform w;

	    for(int i = 0; i < getGridComponentCount(); i++)
	    {
	        w = GetWavePanel(i);
	        if(w != null && w != curr_w)
	              w.UpdatePoint(x, y);
	    }
    }

    synchronized public void appendUpdateWaveforms()
    {
        Waveform w;
        for(int i = 0; i < getGridComponentCount(); i++)
        {
            w = GetWavePanel(i);
            if(w != null )
                  w.appendUpdate();
        }
    }

    synchronized public void updateWaveforms()
    {
        Waveform w;
        for(int i = 0; i < getGridComponentCount(); i++)
        {
            w = GetWavePanel(i);
            if(w != null )
                  w.Update();
        }
    }

    /**
     * Autoscale operation on all waveforms
     *
     * @see Waveform
     * @see MultiWaveform
     */
    public void AutoscaleAll()
    {
        Waveform w;
	    for(int i = 0; i < getGridComponentCount(); i++)
	    {
	        w = GetWavePanel(i);
	        if(w != null)
	            w.Autoscale();
	    }
    }

    /**
     * Autoscale operation on all images
     *
     * @see Waveform
     * @see MultiWaveform
     */
    public void AutoscaleAllImages()
    {
        Waveform w;
	    for(int i = 0; i < getGridComponentCount(); i++)
	    {
	        w = GetWavePanel(i);
	        if(w != null && w.IsImage())
	            w.Autoscale();
	    }
    }

    /**
     * Autoscale y axis on all waveform
     *
     * @see Waveform
     * @see MultiWaveform
     */
    public void AutoscaleAllY()
    {
        Waveform w;
	    for(int i = 0; i < getGridComponentCount(); i++)
	    {
	        w = GetWavePanel(i);
	        if(w != null)
	            w.AutoscaleY();
	    }
    }

    /**
     * Set the same scale factor of the argument waveform to all waveform
     *
     * @param curr_w a waveform
     * @see Waveform
     * @see MultiWaveform
     */
    public void AllSameScale(Waveform curr_w)
    {
        Waveform w;
	    for(int i = 0; i < getGridComponentCount(); i++)
	    {
	        w = GetWavePanel(i);
	        if(w != null)
	   	        if(w != curr_w)
		            w.SetScale(curr_w);
		}
    }

    /**
     * Autoscale y axis and set x axis equals to argument waveform
     *
     * @param curr_w a waveform
     * @see Waveform
     * @see MultiWaveform
     */
    public void AllSameXScaleAutoY(Waveform curr_w)
    {
        Waveform w;
	    for(int i = 0; i < getGridComponentCount(); i++)
	    {
	        w = GetWavePanel(i);
	        if(w != null)
	            w.SetXScaleAutoY(curr_w);
	    }
    }

    /**
     * Set y scale factor of all waveform equals to argument waveform
     *
     * @param curr_w a waveform
     * @see Waveform
     * @see MultiWaveform
     */
    public void AllSameYScale(Waveform curr_w)
    {
        Waveform w;
	    for(int i = 0; i < getGridComponentCount(); i++)
	    {
	        w = GetWavePanel(i);
	        if(w != null)
	  	        if(w != curr_w)
		            w.SetYScale(curr_w);
		}
    }

    /**
     * Set x scale factor of all waveform equals to argument waveform
     *
     * @param curr_w a waveform
     * @see Waveform
     * @see MultiWaveform
     */
    public void AllSameXScale(Waveform curr_w)
    {

        Waveform w;
	    for(int i = 0; i < getGridComponentCount(); i++)
	    {
	        w = GetWavePanel(i);
	        if(w != null && w != curr_w)
	            w.SetXScale(curr_w);
		}
    }

    /**
     * Reset all waveform scale factor.
     *
     * @see Waveform
     * @see MultiWaveform
     */
    public void ResetAllScales()
    {
        Waveform w;
	    for(int i = 0; i < getGridComponentCount(); i++)
	    {
	        w = GetWavePanel(i);
	        if(w != null)
	            w.ResetScales();
	    }
    }

	/**
	 * Perform copy operation
	 *
	 * @param dest destination waveform
	 * @param source source waveform
	 */
    public void NotifyChange(Waveform dest, Waveform source)
    {
        dest.Copy(source);
    }

    /**
     * Remove a waveform.
     *
     * @param w waveform to remove
     */
    public void removePanel(Waveform w)
    {
       if(w == sel_wave)
            sel_wave = null;
        if(w.IsCopySelected())
        {
            copy_waveform = null;
		    w.SetCopySelected(false);
	    }
        super.removeComponent(w);
    }

    public int GetMode()
    {
        return mode;
    }

    public int GetWaveformCount()
    {
        return this.getGridComponentCount();
    }

    public void SetFont(Font font)
    {
        Waveform.SetFont(font);
    }

    public void setLegendMode(int legend_mode)
    {
        Waveform w;
        for(int i = 0; i < getGridComponentCount(); i++)
	    {
	        w = GetWavePanel(i);
	        if(w != null && w instanceof MultiWaveform)
	            ((MultiWaveform)w).setLegendMode(legend_mode);
	    }
    }

    public void setPrintWithLegend(boolean print_with_legend)
    {
        this.print_with_legend = print_with_legend;
    }

    public void setPrintBW(boolean print_bw)
    {
        this.print_bw = print_bw;
    }

    public void SetColors(Color colors[], String colors_name[])
    {
        for(int i = 0, k = 0; i < rows.length; i++)
            for(int j = 0; j < rows[i]; j++, k++) {
                Waveform w = GetWavePanel(k);
                if (w != null)
                    Waveform.SetColors(colors, colors_name);
            }
    }

    public void SetParams(int mode,
                          int grid_mode,
                          int legend_mode,
                          int x_grid_lines,
                          int y_grid_lines,
                          boolean reversed)
    {
        SetReversed(reversed);
        SetMode(mode);
        SetGridMode(grid_mode);
        SetGridStep(x_grid_lines, y_grid_lines);
        setLegendMode(legend_mode);
    }

    public void SetReversed(boolean reversed)
    {
        Waveform w;
        this.reversed = reversed;
        for(int i = 0; i < getGridComponentCount(); i++)
	    {
	        w = GetWavePanel(i);
	        if(w != null)
	            w.SetReversed(reversed);
	    }
    }

    public void stopPlaying()
    {
        Waveform w;
        for(int i = 0; i < getGridComponentCount(); i++)
	    {
	        w = GetWavePanel(i);
	        if(w != null)
	            w.StopFrame();
	    }
    }

    public void SetMode(int mode)
    {
        Waveform w;
        this.mode = mode;

        for(int i = 0; i < getGridComponentCount(); i++)
	    {
	        w = GetWavePanel(i);
	        if(w != null)
	        {
                if(copy_waveform == w &&
                   w.mode == Waveform.MODE_COPY &&
                   mode != Waveform.MODE_COPY)
                {
                    RemoveSelection();
                    copy_waveform = null;
                }
	            w.SetMode(mode);
	        }
	    }
    }

    public void SetGridMode(int grid_mode)
    {
        Waveform w;
        this.grid_mode = grid_mode;
        boolean int_label = (grid_mode == 2 ? false : true);
        for(int i = 0; i < getGridComponentCount(); i++)
	    {
	        w = GetWavePanel(i);
	        if(w != null)
	            w.SetGridMode(grid_mode, int_label, int_label);
        }
    }

    public void SetGridStep(int x_grid_lines, int y_grid_lines)
    {
        Waveform w;
        this.x_grid_lines = x_grid_lines;
        this.y_grid_lines = y_grid_lines;
        for(int i = 0; i < getGridComponentCount(); i++)
	    {
	        w = GetWavePanel(i);
	        if(w != null)
	            w.SetGridSteps(x_grid_lines, y_grid_lines);
	    }
    }

    public Waveform GetSelectPanel()
    {
        Waveform w;

        if(sel_wave == null)
        {
            int i;
            for(i = 0; i < getGridComponentCount() && GetWavePanel(i) != null &&
                           (GetWavePanel(i).GetShowSignalCount() != 0 ||
                           GetWavePanel(i).IsImage()); i++);
            if(i == getGridComponentCount())
            {
               Component c[] = this.CreateWaveComponents(1);
               w = (Waveform)c[0];
            } else {
               w = GetWavePanel(i);
            }
        } else
            return sel_wave;
        return w;
    }

    public void ResetDrawPanel(int _row[])
    {
	    int n_wave = 0;
	    int num = 0;

	    for(int i=0; i < _row.length; i++)
	    {
	        n_wave = (_row[i] - rows[i]);
	        if(n_wave > 0)
	            num += n_wave;
        }

        Component c[] = null;

        if(num > 0)
            c = CreateWaveComponents(num);

        update(_row, c);

        if(sel_wave != null)
            sel_wave.SelectWave();

      //  System.gc();
    }

    /**
     * Deselect waveform.
     *
     * @see Waveform
     * @see MultiWaveform
     */
    public void Deselect()
    {
	   if(sel_wave != null)
	        sel_wave.DeselectWave();
	   sel_wave = null;
    }

    public void maximizeComponent(Waveform w)
    {
        super.maximizeComponent(w);
    }

    public Component getMaximizeComponent()
    {
        return super.getMaximizeComponent();
    }

    public boolean isMaximize(Waveform w)
    {
        return super.isMaximize();
    }

    /**
     * Select a waveform
     *
     * @param w waveform to select
     * @see Waveform
     * @see MultiWaveform
     */
    public void Select(Waveform w)
    {
	   Deselect();
	   sel_wave = w;
	   sel_wave.SelectWave();
    }

    /**
     * Get current selected waveform.
     *
     * @return current selected waveform or null
     * @see Waveform
     * @see MultiWaveform
     */
    public Waveform GetSelected()
    {
        return sel_wave;
    }



    public int print(Graphics g, PageFormat pf, int pageIndex)
        throws PrinterException
    {
        int st_x = 0, st_y = 0;
        double height = pf.getImageableHeight();
        double width = pf.getImageableWidth();
        Graphics2D g2 = (Graphics2D)g;
        String ver = System.getProperty("java.version");


        if(pageIndex == 0)
        {
            //fix page margin error on jdk 1.2.X
            if(ver.indexOf("1.2") != -1)
            {
                if(pf.getOrientation() == PageFormat.LANDSCAPE)
                {
                    st_y = -13;
                    st_x =  15;
                    width -= 5;
                }
                else
                {
                    //st_x = 10;
                    st_y = -5;
                    width -= 25;
                    height -= 25;
                }
            }
            g2.translate(pf.getImageableX(), pf.getImageableY());
            PrintAll(g2, st_x, st_y,(int)height,(int)width);

            return Printable.PAGE_EXISTS;
        } else
            return Printable.NO_SUCH_PAGE;
    }


    /**
     * Set copy source waveform
     *
     * @param w copy source waveform
     * @see Waveform
     * @see MultiWaveform
     */
    public void SetCopySource(Waveform w)
    {
        /*
        if(w != null)
            w.SetCopySelected(true);
        else
            if(copy_waveform != null)
                copy_waveform.SetCopySelected(false);
        */
        if(w != null)
        {
            if(w == copy_waveform)
            {
                w.SetCopySelected(false);
                copy_waveform = null;
                return;
            }
            else
                w.SetCopySelected(true);
        }
        if(copy_waveform != null)
            copy_waveform.SetCopySelected(false);
        copy_waveform = w;
    }

    /**
     * Get current waveform selected as copy source
     *
     * @return copy source waveform
     */
    public Waveform GetCopySource()
    {
        return copy_waveform;
    }

    public void RepaintAllWaves()
    {
        Waveform w;
        for(int i = 0; i < getGridComponentCount(); i++)
        {
            w = GetWavePanel(i);
            if(w != null)
	            w.repaint();
	    }
    }

    public void RemoveAllSignals()
    {
        Waveform w;
        for(int i = 0; i < getGridComponentCount(); i++)
        {
            w = GetWavePanel(i);
            if(w != null)
	            w.Erase();
	    }
	    System.gc();
    }

    public void LoadFileConfiguration(){}

    public void PrintAll(Graphics g, int st_x, int st_y, int height, int width)
    {
        Waveform w;
        int i, j, k = 0;
        int pix = 1;

        if(GetWavePanel(0).grid_mode == 2)//Grid.IS_NONE  mode
          pix = 0;

        int curr_height = 0;
        int curr_width = 0;
        int px = 0;
        int py = 0;
        int pos = 0;

        for(i = k = 0, px = st_x ; i < rows.length; i++)
        {
            if(rows[i] == 0) continue;
	        g.translate(px, 0);
                curr_width = (int)(width * ((RowColumnLayout)getLayout()).getPercentWidth(i) + 0.9);
                if( curr_width == 0 )
                {
                  k += rows[i];
                  continue;
                }
	        for(j = pos = 0, py = st_y; j < rows[i]; j++)
	        {
	            curr_height = (int)(height * ((RowColumnLayout)getLayout()).getPercentHeight(k) + 0.9);
                    if( curr_height == 0 )
                    {
                      k++;
                      continue;
                    }
	            g.translate(0, py);
	            if(j == rows[i] - 1 && pos + curr_height != height)
	                curr_height = height - pos;
	            g.setClip(0, 0, curr_width, curr_height);
	            w = GetWavePanel(k);

	            if(w != null)
	            {
	                int print_mode = Waveform.PRINT;
	                if(print_with_legend)
	                    print_mode |= MultiWaveform.PRINT_LEGEND;
	                if(print_bw)
	                    print_mode |= MultiWaveform.PRINT_BW;

	                disableDoubleBuffering(w);
	                w.paint(g, new Dimension(curr_width,curr_height), print_mode);
	                enableDoubleBuffering(w);
	            }
	            py = curr_height - pix;
	            pos += (curr_height - pix);
	            k++;
	        }
                px = curr_width - pix;
	        g.translate(0, -pos - st_y + py);
        }
    }

    public static void disableDoubleBuffering(Component c) {
        RepaintManager currentManager = RepaintManager.currentManager(c);
        currentManager.setDoubleBufferingEnabled(false);
    }

    public static void enableDoubleBuffering(Component c) {
        RepaintManager currentManager = RepaintManager.currentManager(c);
        currentManager.setDoubleBufferingEnabled(true);
    }
    

   private String getFileName(Waveform w)
   {
        String out;

        Properties prop = new Properties();
        String pr = w.getProperties();
        try
        {
           prop.load( new StringReader(pr) );
           out = w.GetTitle();
           out = out + "_" + prop.getProperty("x_pix");
           out = out + "_" + prop.getProperty("y_pix");
           out = out + "_" + prop.getProperty("time");
           out = out.replace('.', '-') + ".txt";
}
        catch (Exception e)
        {
            out = null;
        }

        return out;
   }

   public void SaveAsText(Waveform w, boolean all)
   {
        String title = "Save";
        if (all)
            title = "Save all signals in text format";
        else
        {
            Point p = this.getWavePosition(w);
            if (p != null)
                title = "Save signals on panel (" + p.x + ", " + p.y +") in text format";
        }
        JFileChooser file_diag = new JFileChooser();
        if (save_as_txt_directory != null &&
            save_as_txt_directory.trim().length() != 0)
            file_diag.setCurrentDirectory(new File(save_as_txt_directory));

        file_diag.setDialogTitle(title);

        int returnVal = JFileChooser.CANCEL_OPTION;
        boolean done = false;
        String txtsig_file = null;

        while (!done)
        {
            String fname = getFileName(w);
            if( fname != null )
                file_diag.setSelectedFile(new File(fname));

            returnVal = file_diag.showSaveDialog(this);
            if (returnVal == JFileChooser.APPROVE_OPTION)
            {
                File file = file_diag.getSelectedFile();
                txtsig_file = file.getAbsolutePath();

                if (file.exists())
                {
                    Object[] options =
                        {
                        "Yes", "No"};
                    int val = JOptionPane.showOptionDialog(null,
                        txtsig_file +
                        " already exists.\nDo you want to replace it?",
                        "Save as",
                        JOptionPane.YES_NO_OPTION,
                        JOptionPane.WARNING_MESSAGE,
                        null,
                        options,
                        options[1]);

                    if (val == JOptionPane.YES_OPTION)
                        done = true;
                }
                else
                    done = true;
            }
            else
                done = true;

        }

        if (returnVal == JFileChooser.APPROVE_OPTION)
        {
            if (txtsig_file != null)
            {
                save_as_txt_directory = new String(txtsig_file);
                String s1 = "", s2 = "";
                StringBuffer space = new StringBuffer();

                try
                {
                    BufferedWriter out = new BufferedWriter(new FileWriter(txtsig_file));
                    out.write("% Title: "+ w.GetTitle());
                    out.newLine();

                    Properties prop = new Properties();
                    String pr = w.getProperties();
                    try
                    {
                       prop.load( new StringReader(pr) );
                       out.write("% Expression: "+ prop.getProperty("expr"));
                       out.newLine();
                       out.write("% x_pixel: "+ prop.getProperty("x_pix"));
                       out.newLine();
                       out.write("% y_pixel: "+ prop.getProperty("y_pix"));
                       out.newLine();
                       out.write("% time: "+ prop.getProperty("time"));
                       out.newLine();
                    } catch (Exception e){}

                    double xmax = w.GetWaveformMetrics().XMax();
                    double xmin = w.GetWaveformMetrics().XMin();

                    s1 = "";
                    s2 = "";
                    int nPoint = w.waveform_signal.getNumPoints();
                    for (int j = 0 ; j < nPoint ; j++)
                    {
                        double x = w.waveform_signal.getX(j);
                        if ( x > xmin && x < xmax)
                        {
                            s1 = "" + x;
                            s2 = "" + w.waveform_signal.getY(j);
                            out.write(s1);
                            space.setLength(0);
                            for (int u = 0; u < 25 - s1.length(); u++)
                                space.append(' ');
                            space.append(' ');
                            out.write(space.toString());
                            out.write(" ");
                            out.write(s2);
                            space.setLength(0);
                            for (int u = 0; u < 25 - s2.length(); u++)
                                space.append(' ');
                            out.write(space.toString());
                            out.newLine();
                        }
                    }
                    out.close();
                }
                catch (IOException e)
                {
                    System.out.println(e);
                }
            }
        }
    }
}


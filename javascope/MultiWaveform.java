import java.awt.*;
import java.util.*;
import java.io.*;
import java.lang.String;

/**
Class MultiWaveform extends the capability of class Waveform to deal with multiple
waveforms.
*/
public class MultiWaveform extends Waveform 
{
	static final int MAX_DRAG_POINT = 200;
	static final int HORIZONTAL = 0;
	static final int VERTICAL = 1;
	
	static final int PRINT_LEGEND = 4;
	static final int PRINT_BW = 8;
	
	static final int LEGEND_IN_GRAPHICS = 0;
	static final int LEGEND_BOTTOM = 1;
	static final int LEGEND_RIGHT = 2;
	
	
    protected Vector  signals = new Vector();
    protected Vector  orig_signals = null;
    protected double  orig_xmin, orig_xmax;
    protected int     curr_point_sig_idx = -1;	
    
    protected boolean show_legend = false;
    protected double  legend_x;
    protected double  legend_y;
    private   Point   legend_point;
    private   int     right_size  = 0;
    private   int     bottom_size = 0;
    protected boolean fixed_legend = false;
    private   int     legend_mode = 0;
 
	protected WaveInterface wi;
    
    
    public MultiWaveform()
    {
	    super();
	    if(signals.size() != 0)
	        signals.removeAllElements(); 
	    orig_signals = null;
  	}
    
    
    public void Erase()
    {
	    if(signals.size() != 0)
	        signals.removeAllElements(); 
	    orig_signals = null;
	    show_legend = false;
	    legend_point = null;
	    super.Erase();
    }
    
    
    public void Copy(Waveform wave)
    {
        super.Copy(wave);
        if(!wave.is_image)
        {
            int i;
            int n_sig;
            MultiWaveform w = (MultiWaveform)wave;

	        if(signals.size() != 0)
	            signals.removeAllElements(); 
            Vector s = w.GetSignals(); 
            for(i = 0; i < s.size(); i++)
                signals.addElement(new Signal((Signal)s.elementAt(i)));

            show_legend = w.show_legend;
            if(w.show_legend)
            {
                legend_x = w.legend_x;
                legend_y = w.legend_y;
                legend_point = new Point(w.legend_point);
            }

	        UpdateLimits();
	        if(waveform_signal != null)
	            super.Update(waveform_signal);
        }    
    }
    
    public void removeSignal(int idx)
    {
        if(idx < signals.size())
            signals.removeElementAt(idx);
    }
    
    public void replaceSignal(int idx, Signal s)
    {
        if(idx < signals.size()) 
        {
            signals.removeElementAt(idx);
            signals.insertElementAt(s, idx);
        }
    }
    
    public boolean exists(Signal s)
    {
	if(s == null) return true;

	if(s.getName() == null || s.getName().length() == 0)
	{
	    s.setName("Signal_"+signals.size());
	    return false;
	}
        for(int i = 0; i < signals.size(); i++)
        {
            Signal s1 = (Signal)signals.elementAt(i);
            if(s1.getName() != null && s.getName() != null && s1.getName().equals(s.getName()))
               return true;
        }
        return false;
    }
    
    public void addSignal(Signal s)
    {
        if(!exists(s))
        {
            signals.addElement(s);
            setLimits();
        }
    }

    public void addSignals(Signal s[])
    {
        if(s == null || s.length == 0) return;
        for(int i = 0; i < s.length; i++)
            addSignal(s[i]);
	    //UpdateLimits();
	    setLimits();
	    if(waveform_signal != null)
	    {
	        curr_point_sig_idx = 0;
	        super.Update(waveform_signal);
	    }

    }

    public boolean IsShowLegend()
    {
        return show_legend;
    }

    public void setShowLegend(boolean show_legend)
    {
        this.show_legend = show_legend;
    }


    public void RemoveLegend()
    {
        show_legend = false;
        not_drawn = true;
        repaint();
    }

    public void          setWaveInterface(WaveInterface wi){this.wi = wi;}
    public WaveInterface getWaveInterface(){return wi;}

    public double GetLegendXPosition(){return legend_x;}
    public double GetLegendYPosition(){return legend_y;}

    public int getLegendMode(){return legend_mode;}

    public void setLegendMode(int legend_mode)
    {
        this.legend_mode = legend_mode;
        if(legend_mode != this.LEGEND_IN_GRAPHICS)
            fixed_legend = true;
        else
            fixed_legend = false;
    }
    
    public boolean isFixedLegend(){return fixed_legend;}
    
    public void SetLegend(Point p)
    {
        Dimension d = getSize();
        legend_x = wm.XValue(p.x, d);
        legend_y = wm.YValue(p.y, d);
        legend_point = new Point(p);
        show_legend = true;
        not_drawn = true;
        repaint();
    }



    protected void PaintSignal(Graphics g, Dimension d, int print_mode)
    {
   
        Dimension dim;
        
	    if(print_mode == NO_PRINT)
            dim = getWaveSize();
        else
            dim = getPrintWaveSize(d);

        super.PaintSignal(g, d, print_mode);
    	if(show_legend && !fixed_legend && !is_min_size) 
    	{
    	    Point p = new Point();
		    if(legend_point == null || prev_width != d.width || prev_height != d.height)
		    {
                p.x = wm.XPixel(legend_x, dim);
                p.y = wm.YPixel(legend_y, dim);
                legend_point = p;
      	    } else {
    	        p = legend_point;
            }
    	    DrawLegend(g, p, print_mode, VERTICAL);
    	 }
    	 
    	 if(fixed_legend && show_legend || (print_mode & PRINT_LEGEND) == PRINT_LEGEND)
	     {
	        g.setClip(0, 0, d.width, d.height);
	        if(legend_mode == LEGEND_BOTTOM && bottom_size != 0)
	            DrawLegend(g, new Point(0, dim.height), print_mode, HORIZONTAL);
	        if(legend_mode == LEGEND_RIGHT && right_size != 0)
	            DrawLegend(g, new Point(dim.width, 0), print_mode, VERTICAL);
	     }
	}
    
    /*
    public void Update(Signal signals[], 
                       String signals_name[], 
                       int markers[], 
                       int markers_step[],
                       boolean interpolates[],
                       int colors_idx[])
    {
        int i;
        int n_sig = signals.length;
        
        if(signals_name.length != n_sig || 
           markers.length != n_sig || 
           markers_step.length != n_sig ||
           interpolates.length != n_sig ||
           colors_idx.length != n_sig)
           return; //must be define exception
           
      
         this.signals      = signals; 
         this.signals_name = signals_name; 
         this.markers      = markers;
         this.markers_step = markers_step;
         this.interpolates = interpolates;
         this.colors_idx   = colors_idx;
         

	     UpdateLimits();
	     
	     //for(i = 0; i < signals.length; i++)
	     //   if(this.signals[i] != null) break;
	     //if(i < signals.length)    
	     
	     if(waveform_signal != null)
	        super.Update(waveform_signal);
	     
    }

	
    public void Update(Signal signals[], 
                       int markers[], 
                       int markers_step[], 
                       boolean interpolates[])
    {
	    int i, n_sig;
	    if(signals == null || signals.length == 0) return;

        n_sig = signals.length;

        if(markers.length != n_sig || 
           markers_step.length != n_sig ||
           interpolates.length != n_sig)
           return; //must be define exception

	    
//	    this.signals = new Signal[signals.length];
	    signals_name = new String[n_sig];
	    colors_idx = new int[n_sig];

	    for(i = 0; i < n_sig; i++)
	        if(signals[i] != null) {
//		        this.signals[i] = new Signal(signals[i]);
		        signals_name[i] = new String("Signal "+(i+1));
		        //colors_idx[i] = i%4;
		    }  else 
		        signals_name[i] = new String("Null signal "+(i+1));
		    
	    orig_signals = null;
	    this.signals = signals; 
	    this.markers = markers;
	    this.markers_step = markers_step;
	    this.interpolates = interpolates;
	    curr_point_sig_idx = 0;
	    UpdateLimits();
	    
	    //for(i = 0; i < signals.length; i++)
	   //     if(this.signals[i] != null) break;
	    //if(i < signals.length)    
	    
	    if(waveform_signal != null)
	        super.Update(waveform_signal);
    }
*/    	
    public synchronized void UpdateSignals(Signal signals[], int timestamp, boolean force_update)
    {
        
	    //System.out.println("timestamp "+update_timestamp + " "+ timestamp);
    
        Signal s;
    
	    if(!force_update && update_timestamp != timestamp)
	        return;
	        
	    for(int i = 0; i < signals.length; i++)
	    {
	        s = (Signal)this.signals.elementAt(i);
	        signals[i].setAttributes(s);
	    }
	    
	    if(this.signals.size() != 0)
	        this.signals.removeAllElements(); 
	        
	    for(int i = 0; i < signals.length; i++)
	    {
	        this.signals.addElement(signals[i]);
	    }
	    
	    if(force_update)
	        UpdateLimits();
	        
	    if(waveform_signal != null)
	    {
	        if(curr_point_sig_idx > signals.length || curr_point_sig_idx == -1)
	            curr_point_sig_idx = 0;
	        
	        super.UpdateSignal(waveform_signal);
	    }
    }
    
    public Vector GetSignals()
    {
        return signals;
    }

    public String[] GetSignalsName()
    {
        String names[] = new String[signals.size()];
	    String n;
	    Signal s;
        for(int i = 0; i < signals.size(); i++)
	    {
	        s = (Signal)signals.elementAt(i);
	        n = s.getName();
	        if(n != null)
		        names[i] = n;
	        else 
	        {
		        names[i] = new String("Signal_"+i);
		        s.setName(names[i]);
	        }

        }
	    return names;
    }
    
    public String getSignalName(int idx)
    {
        if(idx < signals.size() && signals.elementAt(idx) != null )
	    {
	        Signal s =  (Signal)signals.elementAt(idx);
	        if(s.getName() == null)
		        s.setName(new String("Signal_"+idx));
	        return s.getName();
	    }
        else
            if(is_image && frames != null)
                return frames.getName();
        return null;
    }

    
    public void SetInterpolate(int idx, boolean interpolate)
    {
        if(idx < signals.size())
        {
            ((Signal)signals.elementAt(idx)).setInterpolate(interpolate);
        }
    }

    public boolean GetInterpolate(int idx)
    {
        if(idx < signals.size())
        {
            return ((Signal)signals.elementAt(idx)).getInterpolate();
        }
        return false;
    }

    public void SetMarker(int idx, int marker)
    {
        if(idx < signals.size())
        {
            ((Signal)signals.elementAt(idx)).setMarker(marker);
        }
    }

    public int GetMarker(int idx)
    {
        if(idx < signals.size())
        {
            return ((Signal)signals.elementAt(idx)).getMarker();
        }
        return 0;
    }


    public void SetMarkerStep(int idx, int marker_step)
    {
        if(idx < signals.size())
        {
            ((Signal)signals.elementAt(idx)).setMarkerStep(marker_step);
        }
    }

    public int GetMarkerStep(int idx)
    {
        if(idx < signals.size())
        {
            return ((Signal)signals.elementAt(idx)).getMarkerStep();
        }
        return 0;
    }

    public void SetColorIdx(int idx, int color_idx)
    {
        if(is_image)
        {
            super.SetColorIdx(color_idx);
            super.SetCrosshairColor(color_idx);
            return;
        }    
        
        if(idx < signals.size())
        {
            ((Signal)signals.elementAt(idx)).setColorIdx(color_idx);
            if(idx == curr_point_sig_idx)
	            crosshair_color = colors[color_idx % colors.length];
        }
    }

    
    public int GetColorIdx(int idx)
    {
        if(is_image)
            return super.GetColorIdx();
            
        if(idx < signals.size())
        {
            return ((Signal)signals.elementAt(idx)).getColorIdx();
        }
        return 0;
    }
    
    
    public int GetShowSignalCount()
    {
        if(signals != null)
            return signals.size();
        else
            return 0;
    }

    public int getSignalCount()
    {
        return GetShowSignalCount();
    }

    protected void setLimits()
    {
      setXlimits(lx_min, lx_max);
      setYlimits(ly_min, ly_max);
      UpdateLimits();
	  change_limits = true;
    }

    
    public void setXlimits(float xmin, float xmax)
    {
        if(signals == null) return;
        
        Signal s;
        
        for(int i = 0; i < signals.size(); i++)
        {
            s = (Signal)signals.elementAt(i);
            s.setXlimits(xmin, xmax);
        }
    }
    
    public void setYlimits(float ymin, float ymax)
    {
        if(signals == null) return;
        
        Signal s;
        
        for(int i = 0; i < signals.size(); i++)
        {
            s = (Signal)signals.elementAt(i);
            s.setYlimits(ymin, ymax);
        }
    }
    
    
    public void setPointSignalIndex(int idx)
    {
        if(idx >= 0 && idx < signals.size())
        {
	        Signal curr_signal;
            curr_point_sig_idx = idx;

	        curr_signal= (Signal)signals.elementAt(curr_point_sig_idx);
	        
	        if(curr_signal == null) return;
	    
            if(curr_signal.getColor() != null)
                crosshair_color = curr_signal.getColor();
            else
	            crosshair_color = colors[curr_signal.getColorIdx() % colors.length];
        }
    }

    public void SetSignalState(String label, boolean state)
    {
        Signal sig;
        if(signals != null)
        {
            for(int i = 0; i < signals.size(); i++)
            {
                sig = (Signal)signals.elementAt(i);
                if(sig == null) continue;
                if(sig.getName().equals(label))
                {
                    sig.setInterpolate(state);
                    sig.setMarker(Signal.NONE);
                }
            }
            if(mode == Waveform.MODE_POINT )
            {
               Dimension d = getSize();
               double curr_x = wm.XValue(end_x, d),
	                  curr_y = wm.YValue(end_y, d); 
//               FindPointY(curr_x, curr_y, true);
               FindPoint(curr_x, curr_y, true);
            }
        }
    }
    
  
    public boolean[] GetSignalsState()
    {
        boolean s_state[] = null;
        if(signals != null)
        {
            s_state = new boolean[signals.size()];
            for(int i = 0; i < signals.size(); i++)
                s_state[i] = GetSignalState(i);
        }
        return s_state;
    }

    public boolean GetSignalState(int idx)
    {
        if(idx > signals.size()) return false;
        Signal s = (Signal)signals.elementAt(idx);
        if(s == null) return false;
            return !(!s.getInterpolate() && s.getMarker()  == Signal.NONE);
    }
    
    public void Update()
    {
        if(!is_image)
        {
	        UpdateLimits();
	        if(waveform_signal != null)
	        {
	            curr_point_sig_idx = 0;
	            super.Update(waveform_signal);
	            if(wi != null)
	                wi.ContinuousUpdate(signals, this); //for handling continuous signals
	        }
	    }
	    else
	    {
	        if(frames != null)
	            super.Update();
	    }
    }
    
    public void Update(Signal signals[])
    {
	    int i;
	    if(signals == null) return;
	    
	    if(this.signals.size() != 0)
	        this.signals.removeAllElements(); 
	    for(i = 0; i < signals.length; i++)
	        this.signals.addElement(signals[i]);
	    MultiWaveform.this.Update();
   }


    void UpdateLimits()
    {
	    int i;
	    
	    waveform_signal = null;
	    for(i = 0; i < signals.size(); i++)
	        if(signals.elementAt(i) != null)
		        break;
	    if(i == signals.size()) return;
	    waveform_signal = new Signal((Signal)signals.elementAt(i));

	    for(i = 1; i <  signals.size(); i++)
	    {
	        if((Signal)signals.elementAt(i) == null) continue;
	        if(waveform_signal.xmax < ((Signal)signals.elementAt(i)).xmax)
		        waveform_signal.xmax = ((Signal)signals.elementAt(i)).xmax;
	        if(waveform_signal.ymax < ((Signal)signals.elementAt(i)).ymax)
		        waveform_signal.ymax = ((Signal)signals.elementAt(i)).ymax;
	        if(waveform_signal.xmin > ((Signal)signals.elementAt(i)).xmin)
		        waveform_signal.xmin = ((Signal)signals.elementAt(i)).xmin;
	        if(waveform_signal.ymin > ((Signal)signals.elementAt(i)).ymin)
		        waveform_signal.ymin = ((Signal)signals.elementAt(i)).ymin;
	    }
    }
    

    
    synchronized public void paint(Graphics g, Dimension d, int print_mode)
    {
        bottom_size = right_size = 0;
        if(fixed_legend && show_legend ||(print_mode & PRINT_LEGEND) == PRINT_LEGEND)
        {
            setFont(g);
            if(legend_mode == LEGEND_BOTTOM)
            {
                Dimension dim = getLegendDimension(g, d, HORIZONTAL);
                bottom_size = dim.height;
                g.drawLine(0, dim.height - 1, d.width, dim.height - 1);
            }
            
            if(legend_mode == LEGEND_RIGHT)
            {
                Dimension dim = getLegendDimension(g, d, VERTICAL);
                right_size = dim.width;
                g.drawLine(dim.width - 1, 0, dim.width - 1, d.height);
            }
        }
        super.paint(g, d, print_mode);
    }

    protected int getRightSize(){return right_size;}
    protected int getBottomSize(){return bottom_size;}


    protected Dimension getLegendDimension(Graphics g, Dimension d, int orientation)
    {
        Dimension dim = new Dimension(0, 0);
        int curr_width = 0, sum_width = 0;
        Font f = g.getFont();
        int h = f.getSize() + 2;
        FontMetrics fm = getFontMetrics(f);
        Signal sign;
        
        if(getSignalCount() == 0) return dim;
        
        for(int i = 0; i < getSignalCount(); i++)
        {
            if(!isSignalShow(i))
                continue;
                                                    
            String lab = getSignalInfo(i);
            
            char[] lab_ar = lab.toCharArray();
            
            curr_width = fm.charsWidth(lab_ar, 0, lab_ar.length);
            if(orientation == VERTICAL)
            {
                curr_width += 2 * marker_width;
                dim.height += h;
                if(curr_width > dim.width)
                    dim.width = curr_width;
            } 
            
            if(orientation == HORIZONTAL)
            {
                curr_width += 3 * marker_width;
                if(sum_width + curr_width < d.width)
                {
                    sum_width += curr_width;
                }
                else
                {
                    if(sum_width > dim.width)
                        dim.width = sum_width;
                    sum_width = curr_width;
                    dim.height += h;
                }
                
            } 
        }
        dim.height += (orientation == HORIZONTAL) ? (int)(3./2*h+0.5) : h/2;
        return dim;
    }
    
    protected String getSignalInfo(int i)
    {
        Signal sign = (Signal)signals.elementAt(i);
        String lab = sign.getName();
        if(sign.getType() == Signal.TYPE_2D)
        {
            switch(sign.getMode())
            {
                case Signal.MODE_YTIME:lab = lab + " [Y-TIME X = "+ Waveform.ConvertToString(sign.getXData(), false)+" ]";break;
                case Signal.MODE_XY:lab = lab + " [X-Y T = "+ Waveform.ConvertToString(sign.getTime(), false)+" ]";break;
                case Signal.MODE_YX:lab = lab + " [Y-X T = "+ Waveform.ConvertToString(sign.getTime(), false)+" ]";break;
            }
        }
        return lab;
    }

    protected boolean isSignalShow(int i)
    {
        Signal sign = (Signal)signals.elementAt(i);
        return ( sign != null && (sign.getInterpolate() || 
                                  sign.getMarker() != Signal.NONE));
    }

    protected Color getSignalColor(int i)
    {
        if(i > signals.size()) return Color.black;
        Signal sign = (Signal)signals.elementAt(i);
        if(sign.getColor() != null)
            return sign.getColor();
        else
            return colors[sign.getColorIdx()%colors.length];
    }


    protected void DrawLegend(Graphics g, Point p, int print_mode, int orientation)
    {
        Dimension d = getSize();
        int h = g.getFont().getSize() + 2;
        Color prev_col = g.getColor();
        Point pts[] = new Point[1];
        FontMetrics fm = getFontMetrics(g.getFont());
        String s;
        pts[0] = new Point();
        Signal sign;
        int curr_width = 0, sum_width = p.x;
        int curr_marker = 0;
                        
        g.setColor(Color.black);
        
        if(orientation == VERTICAL)
            g.translate(-marker_width, 0);
        
        for(int i = 0, py = p.y + h, px = p.x; i < getSignalCount(); i++)
        {
            if(!isSignalShow(i))
                continue;
            
            if((print_mode & PRINT_BW) != PRINT_BW)     
                g.setColor(getSignalColor(i));
            
            s = getSignalInfo(i);            

            
            if(orientation == HORIZONTAL)
            {
                char s_ar[] = s.toCharArray();
                curr_width = fm.charsWidth(s_ar, 0, s_ar.length) + 3*marker_width;
                if(sum_width + curr_width < d.width)
                {
                    px = sum_width;
                    sum_width += curr_width;
                } else {
                    py += h;
                    px = p.x;
                    sum_width = p.x + curr_width;
                }
            }
            
            pts[0].x = px + 2*marker_width;
            pts[0].y = py - marker_width/2;
            DrawMarkers(g, pts, 1, GetMarker(i), 1);

            if((GetMarker(i) == Signal.NONE) && ((print_mode & PRINT_BW) == PRINT_BW))
            {
                DrawMarkers(g, pts, 1, curr_marker + 1, 1);
                curr_marker = (curr_marker + 1) % (Signal.markerList.length - 1);
            }
            
            g.drawString(s, px + 3 * marker_width, py);
            
            if(orientation == VERTICAL)
                py += h;
        } 
        if(orientation == VERTICAL)
            g.translate(marker_width, 0);
        g.setColor(prev_col); 
    }


    protected void DrawSignal(Graphics g)
    {
        DrawSignal(g, getSize(), Waveform.NO_PRINT);
    }


    protected void DrawSignal(Graphics g, Dimension d, int print_mode)
    {	
        int num_marker = Signal.markerList.length - 1;
	    int i, j, x[], y[];
	    Point curr_points[];
	    Vector segments = null;	
	    float step; 
	    int num_steps, marker_step = 1;
	    Signal s;
	    	    
	    g.setColor(Color.black);
	    
	    for(i = 0; i <  signals.size(); i++)
	    {
	        s = (Signal)signals.elementAt(i); 
	        if(s == null) continue;
	        
	        if((print_mode & PRINT_BW) != PRINT_BW)
	        {
	            marker_step = (s.getMarkerStep() > 0 ) ? s.getMarkerStep() : 1;
                if(s.getColor() != null)
                    g.setColor(s.getColor());
                else
                    g.setColor(colors[s.getColorIdx()%colors.length]);
            } else {
	            if(s.getMarker() != Signal.NONE)
                    marker_step = (int)( ((s.n_points>1000)?100:s.n_points/10.) + 0.5);
	        }
	        
	        if(mode == MODE_PAN && dragging && s.n_points > MAX_DRAG_POINT) //dragging large signals
	        {
	            int drag_point = MAX_DRAG_POINT;
	            if(signals.size() ==  1)
	                drag_point = (s.n_points > MAX_DRAG_POINT * 3) ? MAX_DRAG_POINT * 3 : s.n_points;
		        x = new int[s.n_points];
		        y = new int[s.n_points];
		        curr_points = new Point[s.n_points];
		        step = (float)s.n_points/drag_point;
		        num_steps = drag_point;
		        for(j = 0; j < num_steps; j++)
		        {
		            x[j] = wm.XPixel(s.x[(int)(step*j)], d);
		            y[j] = wm.YPixel(s.y[(int)(step*j)], d);
		            curr_points[j] = new Point(x[j], y[j]);	
		        }
		        //if(s.getInterpolate())
		            for(int jj = 0; jj < num_steps -1; jj++)
			            if(!Double.isNaN(s.y[(int)(step*jj)]) && 
			                !Double.isNaN(s.y[(int)(step*(jj+1))]))
			                    g.drawLine( x[jj], y[jj], x[jj+1], y[jj+1]);
	         }
	         else 
	         {
		        segments = wm.ToPolygons(s, d);
		        Polygon curr_polygon;

		        if(s.getInterpolate() || mode == MODE_PAN && dragging)
		            for(int k = 0; k < segments.size(); k++)
		            {
			            curr_polygon = (Polygon)segments.elementAt(k);
			            g.drawPolyline(curr_polygon.xpoints, curr_polygon.ypoints, curr_polygon.npoints);
		            }
	        }
	    
	        if(dragging && mode == MODE_PAN)
		        continue;

	        if(s.getMarker() != Signal.NONE)
		        DrawMarkers(g, segments, s.getMarker(), marker_step);
	        /*
		        if(s.getMarkerStep() != 0)
		            DrawMarkers(g, segments, s.getMarker(), s.getMarkerStep());
		        else
		            DrawMarkers(g, segments, s.getMarker(), 1);
            */
	        if(s.error)
	            DrawError(g, d, s);
	    }

	    if((print_mode & PRINT_BW) == PRINT_BW)
	    {
	        int curr_marker = 0;
	        for(i = 0; i < signals.size(); i++)
	        {
	            s = (Signal)signals.elementAt(i);
	            if(s == null) continue;
		        segments = wm.ToPolygons(s, d);
                marker_step = (int)( ((s.n_points>1000)?100:s.n_points/10.) + 0.5);
		        DrawMarkers(g, segments, curr_marker+1, marker_step);
                curr_marker = (curr_marker+1)%num_marker;
	        }
        }	    
    }


    protected void DrawMarkers(Graphics g, Vector segments, int mark_type, int step)
    {
	    int num_points, num_segments = segments.size();
	    int i;
	    Point points[];
	    Polygon curr_polygon;	
	
	    for(i = num_points = 0; i < num_segments; i++)
	        num_points += ((Polygon)segments.elementAt(i)).npoints;
	        
	    points = new Point[num_points];
	    for(i = 0; i < num_points; i++)
	        points[i] = new Point();
	    for(i = num_points = 0; i < num_segments; i++)
	    {
	        curr_polygon = (Polygon)segments.elementAt(i);
	        for(int j = 0; j < curr_polygon.npoints; j+= step)
	        {
		        points[num_points].x = curr_polygon.xpoints[j];
		        points[num_points].y = curr_polygon.ypoints[j];
		        num_points++;		        
    	    }
	    }
	    
	    super.DrawMarkers(g, points, num_points, mark_type, 1);
	    
    }



    protected Point FindPoint(double curr_x, double curr_y, boolean is_first)	
    {
	    Signal curr_signal;
	    int curr_idx, i, min_idx = 0;
	    double curr_dist = 0, min_dist = Double.MAX_VALUE;
	    
	    if(signals == null || signals.size() == 0)
            return null;
	
	//if(signals[curr_point_sig_idx] == null) return 0;
	    if(!is_first)
	    {
	        curr_signal = (Signal)signals.elementAt(curr_point_sig_idx);
	        return FindPoint(curr_signal, curr_x, curr_y);    
		}
		
	    for(curr_point_sig_idx = i = 0; i < signals.size(); i++)
	    {
	        curr_signal = (Signal)signals.elementAt(i);
	        if(curr_signal  == null || !GetSignalState(i))
	        {
	            //curr_point_sig_idx++;
	            continue;
	        }
	        curr_idx =  curr_signal.FindClosestIdx(curr_x, curr_y);	
	        curr_dist = (curr_signal.y[curr_idx] - curr_y)*
		        (curr_signal.y[curr_idx] - curr_y);
	        if(i == 0 || curr_dist < min_dist)
	        {
		        min_dist = curr_dist;
		        min_idx = curr_idx;
		        curr_point_sig_idx = i;
	        }
	    }
	    setPointSignalIndex(curr_point_sig_idx);
	    curr_signal= (Signal)signals.elementAt(curr_point_sig_idx);
	    	    
	    Point p = FindPoint(curr_signal, curr_x, curr_y);
	    
	    if(p == null)
            curr_point_sig_idx = -1;
        return p;
	    
    }

	public void UpdatePoint(double curr_x)
	{
			
        if(!is_image)
        {
                     // if(wm == null) { System.out.println("wm == null"); return;}

            
		   // if(dragging || mode != MODE_POINT || signals == null || signals.size() == 0)
	              if(mode != MODE_POINT || signals == null || signals.size() == 0)
			    return;
			    
			Signal s;
			
			for(int i = 0; i < signals.size(); i++)
			{
			    s = (Signal)signals.elementAt(i);
			    if(s == null) continue;
			    if(s.getType() == Signal.TYPE_2D && 
			        (s.getMode() == Signal.MODE_XY || s.getMode() == Signal.MODE_YX))
			    {
			        s.showXY(s.getMode(), (float)curr_x);
				not_drawn = true;
			    }
			}
		}
		super.UpdatePoint(curr_x);
	}

    protected int GetSelectedSignal() {return curr_point_sig_idx; }
    
    public Signal GetSignal() {return (Signal)signals.elementAt(curr_point_sig_idx);}

    public int getSignalMode(int idx)
    {
       int type = -1; 
       if(idx >= 0 && idx < signals.size())
       {
           type = ((Signal)signals.elementAt(idx)).getMode();
       }
       return type;
    }

    public int getSignalMode()
    {
       return getSignalMode(curr_point_sig_idx);
    }

    public int getSignalType(int idx)
    {
       int type = -1; 
       if(idx >= 0 && idx < signals.size())
       {
           type = ((Signal)signals.elementAt(idx)).getType();
       }
       return type;
    }

    public int getSignalType()
    {
       return getSignalType(curr_point_sig_idx);
    }
    
    public void setSignalMode(int idx, int mode)
    {
       if(idx >= 0 && idx < signals.size())
       {
          Signal s = ((Signal)signals.elementAt(idx)); 
          if(s != null && s.getType() == Signal.TYPE_2D)
          {
            if(mode == Signal.MODE_YTIME && s.getMode() == Signal.MODE_YX)
                s.setMode(mode, (float)wave_point_y);
            else
                s.setMode(mode, (float)wave_point_x);
          }
       }
       Update();
    }
    
    public void setSignalMode(int mode)
    {
        setSignalMode(curr_point_sig_idx, mode);
    }

    protected void ReportLimits(ZoomRegion r, boolean add_undo)    
    {
        if(!add_undo)
        {
	        if(waveform_signal == null) return;
	        update_timestamp++;
	        if(signals == null)
	            return;
	        if(orig_signals != null)   //Previous zoom
	        {
	            signals = orig_signals;
	            orig_signals = null;
	        }
	    }
	    super.ReportLimits(r, add_undo);
	    if(add_undo)
            NotifyZoom(r.start_xs, r.end_xs, r.start_ys, r.end_ys, update_timestamp);	    
    }

    public void Autoscale()
    {
	    int i;
	
	    if(is_image && frames != null)
	    {
	        super.Autoscale();
	        return;
	    }
	
	    if(waveform_signal == null) return;
	    
	    update_timestamp++;

	    if(signals == null)
	        return;
	    if(orig_signals != null)   //Previous zoom
	    {
	        signals = orig_signals;
	        orig_signals = null;
	    }    
	
	    for(i = 0; i < signals.size(); i++)
	        if(signals.elementAt(i) != null) break;
	    if(i == signals.size()) return;
	        waveform_signal = (Signal)signals.elementAt(i);


    	for(i = 0; i < signals.size(); i++)
	    {
	        if((Signal)signals.elementAt(i) == null) continue;
	        ((Signal)signals.elementAt(i)).Autoscale();
	        if(((Signal)signals.elementAt(i)).xmin < waveform_signal.xmin)
		        waveform_signal.xmin = ((Signal)signals.elementAt(i)).xmin;
	        if(((Signal)signals.elementAt(i)).xmax > waveform_signal.xmax)
		        waveform_signal.xmax = ((Signal)signals.elementAt(i)).xmax;
	        if(((Signal)signals.elementAt(i)).ymin < waveform_signal.ymin)
		        waveform_signal.ymin = ((Signal)signals.elementAt(i)).ymin;
	        if(((Signal)signals.elementAt(i)).ymax > waveform_signal.ymax)
		        waveform_signal.ymax = ((Signal)signals.elementAt(i)).ymax;
	    }
	    ReportChanges();
    }

    public void AutoscaleY()
    {
	    int i;
	    if(waveform_signal == null) return;
	    double prev_xmin = waveform_signal.xmin, prev_xmax = waveform_signal.xmax;
    	    if(signals == null)
	            return;
	
	    double ymin = Double.POSITIVE_INFINITY, ymax = Double.NEGATIVE_INFINITY;
	    for(i = 0; i < signals.size(); i++)
	    {
	        if(((Signal)signals.elementAt(i)) == null) continue;
	            ((Signal)signals.elementAt(i)).AutoscaleY(prev_xmin, prev_xmax);
            if(((Signal)signals.elementAt(i)).ymin < ymin)
 	    	    ymin = ((Signal)signals.elementAt(i)).ymin;
            if(((Signal)signals.elementAt(i)).ymax > ymax)
    	    	ymax = ((Signal)signals.elementAt(i)).ymax;
	    }
	    
	    if(ymin >= ymax)
	    {
	        ymax = ymin+2*ymin;
	        ymin = ymin-2*ymin;
	    }
	    waveform_signal.ymin = ymin;
	    waveform_signal.ymax = ymax;	
	    ReportChanges();
	    	    
    }
    
    public void SetXScaleAutoY(Waveform w)
    {
	    double xmin, xmax;
	
	    if(waveform_signal == null) return;
	
	    if(signals == null)
	        return;
	    if(w != this && orig_signals != null) //Previous zoom for differentr windows
	    {
	        signals = orig_signals;
	        //operation on signals must not affect original signals
	        orig_signals = new Vector();
	        for(int i = 0; i < signals.size(); i++)
	            orig_signals.addElement(signals.elementAt(i));
        }
	    waveform_signal.xmin = w.waveform_signal.xmin;
	    waveform_signal.xmax = w.waveform_signal.xmax;

	    AutoscaleY();
	    
	    update_timestamp++;
	    NotifyZoom(waveform_signal.xmin, 
	               waveform_signal.xmax, 
	               waveform_signal.ymin,
	               waveform_signal.ymax, 
                   update_timestamp);
    }

    public void ResetScales()
    {
        if(signals == null || waveform_signal == null)
	        return;
	    if(orig_signals != null)
	    {
	        signals = orig_signals;
	        int i;
	        for(i = 0; i < signals.size() && ((Signal)signals.elementAt(i)) == null; i++);
	        waveform_signal = ((Signal)signals.elementAt(i));
	        
	    }
	    waveform_signal.ResetScales();
	    ReportChanges();
    }




    protected void NotifyZoom(double start_xs, double end_xs, double start_ys, double end_ys,
	    int timestamp) 
    {
        double x_range = end_xs - start_xs;
        if(orig_signals == null)
        {
	        orig_signals = new Vector();
	        for(int i = 0; i < signals.size(); i++)
	            orig_signals.addElement(signals.elementAt(i));
	        orig_xmin = waveform_signal.xmin;
	        orig_xmax = waveform_signal.xmax;
        }
       
        
        if(wi != null)
	        wi.AsynchUpdate(signals, (float)(start_xs - x_range), (float)(end_xs + x_range), 
	            (float)orig_xmin, (float)orig_xmax, update_timestamp, mode == MODE_PAN, this);
        
    }	

    protected void HandleCopy()
    {
        /*
        if(IsCopySelected())
	        return;
        if(signals != null && signals.length != 0 && controller.GetCopySource() == null
            || is_image && frames != null && controller.GetCopySource() == null )
        {
	        controller.SetCopySource(this);
	        SetCopySelected(true);
        }
        */
    }

    protected void HandlePaste()
    {
        /*
        if(IsCopySelected())
        {
	        SetCopySelected(false);
	        controller.SetCopySource(null);
        }
        else
        {
	        if(controller.GetCopySource() != null)
	            controller.NotifyChange(this, controller.GetCopySource());
        }
        */
    }
    
    protected void SetMode(int mod)
    {
        super.SetMode(mod);
    }
 }

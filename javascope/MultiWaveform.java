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
    protected Vector  signals = new Vector();
    protected Vector  orig_signals = null;
    protected double  orig_xmin, orig_xmax;
    protected int     curr_point_sig_idx = -1;	
    
    protected boolean make_legend = false;
    protected double  legend_x;
    protected double  legend_y;
    private   Point   legend_point;

    
    
    public MultiWaveform()
    {
	    super();
	    signals.removeAllElements(); 
	    orig_signals = null;
    }
    
    
    public void Erase()
    {
	    signals.removeAllElements(); 
	    orig_signals = null;
	    make_legend = false;
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

            signals.removeAllElements();
            Vector s = w.GetSignals(); 
            for(i = 0; i < s.size(); i++)
                signals.addElement(new Signal((Signal)s.elementAt(i)));

            make_legend = w.make_legend;
            if(w.make_legend)
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
        for(int i = 0; i < signals.size(); i++)
        {
            Signal s1 = (Signal)signals.elementAt(i);
            if(s1.getName().equals(s.getName()))
               return true;
        }
        return false;
    }
    
    public void addSignal(Signal s)
    {
        if(! exists(s))
            signals.addElement(s);
    }

    public void addSignals(Signal s[])
    {
        if(s == null) return;
        for(int i = 0; i < s.length; i++)
            addSignal(s[i]);    
	    curr_point_sig_idx = 0;
	    UpdateLimits();
	    if(waveform_signal != null)
	        super.Update(waveform_signal);

    }

    public boolean IsShowLegend()
    {
        return make_legend;
    }


    public void RemoveLegend()
    {
        make_legend = false;
        //not_drawn = true;
        Repaint(true);
    }

    public double GetLegendXPosition(){return legend_x;}
    public double GetLegendYPosition(){return legend_y;}

    public void SetLegend(Point p)
    {
        Dimension d = getSize();
        legend_x = wm.XValue(p.x, d);
        legend_y = wm.YValue(p.y, d);
        legend_point = new Point(p);
        make_legend = true;
        //not_drawn = true;
        Repaint(true);
    }

    protected void PaintSignal(Graphics g, Dimension d, boolean print_flag)
    {
        super.PaintSignal(g, d, print_flag);
    	if(make_legend && !is_min_size) 
    	{
    	    Point p = new Point();
    	    if(legend_x == -1 || ((mode == MODE_ZOOM || mode == MODE_PAN)
    	                            && prev_width == d.width 
		                            && prev_height == d.height) && !print_flag)
		    {
    	        p = legend_point;
    	    } else {
                p.x = wm.XPixel(legend_x, d);
                p.y = wm.YPixel(legend_y, d);
                legend_point = p;
            }
    	    DrawLegend(g, p);
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
    public void UpdateSignals(Signal signals[], int timestamp)
    {
	//System.out.println("timestamp"+update_timestamp+ timestamp);
    
	    if(update_timestamp != timestamp)
	        return;
	        
	    this.signals.removeAllElements();
	    for(int i = 0; i < signals.length; i++)
	        this.signals.addElement(signals[i]);
	    
	    curr_point_sig_idx = 0;
	    super.UpdateSignal(waveform_signal);
    }
    
    public Vector GetSignals()
    {
        return signals;
    }

    public String[] GetSignalsName()
    {
        String names[] = new String[signals.size()];
        for(int i = 0; i < signals.size(); i++)
            names[i] = new String(((Signal)signals.elementAt(i)).getName()); 
        return names;
    }
    
    public String getSignalName(int idx)
    {
        if(idx < signals.size() && signals.elementAt(idx) != null )
            return ((Signal)signals.elementAt(idx)).getName();
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
    
    
    public int GetSignalCount()
    {
        if(signals != null)
            return signals.size();
        else
            return 0;
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
    
    
    public int PointSignalIndex()
    {
        return curr_point_sig_idx;
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
	    curr_point_sig_idx = 0;
	    UpdateLimits();
	    if(waveform_signal != null)
	        super.Update(waveform_signal);
    }
    
    public void Update(Signal signals[])
    {
	    int i;
	    if(signals == null) return;
	    
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

    protected void DrawLegend(Graphics g, Point p)
    {
        int h = g.getFont().getSize() + 2;
        Color prev_col = g.getColor();
        Point pts[] = new Point[1];
        String s, er;
        pts[0] = new Point();
        Signal sign;
                
        for(int i = 0, py = p.y ; i < signals.size(); i++)
        {
            sign = (Signal)signals.elementAt(i);
            if( sign == null ||
                !sign.getInterpolate() && 
                  sign.getMarker() == Signal.NONE)
                continue;
            py += h;
            
            if(sign.getColor() != null)
                g.setColor(sign.getColor());
            else
                g.setColor(colors[sign.getColorIdx()%colors.length]);
            
            pts[0].x = p.x - marker_width/2;
            pts[0].y = py - marker_width/2;           
            DrawMarkers(g, pts, 1, sign.getMarker(), 1);
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
            g.drawString(lab, p.x + marker_width, py);
        } 
        g.setColor(prev_col); 
    }


    protected void DrawSignal(Graphics g)
    {
        DrawSignal(g, getSize());
    }


    protected void DrawSignal(Graphics g, Dimension d)
    {	
	    int i, j, x[], y[];
	    Point curr_points[];
	    Vector segments = null;	
	    int step, num_steps;
	    Signal s;
	    
	    for(i = 0; i <  signals.size(); i++)
	    {
	       s = (Signal)signals.elementAt(i); 
	        if(s == null) continue;
            if(s.getColor() != null)
                g.setColor(s.getColor());
            else
                g.setColor(colors[s.getColorIdx()%colors.length]);

	        
	        if(mode == MODE_PAN && dragging && s.n_points > 100) //dragging large signals
	        {
		        x = new int[s.n_points];
		        y = new int[s.n_points];
		        curr_points = new Point[s.n_points];
		        step = s.n_points/100;
		        num_steps = 100;
		        for(j = 0; j < num_steps; j++)
		        {
		            x[j] = wm.XPixel(s.x[step*j], d);
		            y[j] = wm.YPixel(s.y[step*j], d);
		            curr_points[j] = new Point(x[j], y[j]);	
		        }
		        if(s.getInterpolate())
		            for(int jj = 0; jj < num_steps -1; jj++)
			            if(!Double.isNaN(s.y[step*jj]) && 
			                !Double.isNaN(s.y[step*(jj+1)]))
			                    g.drawLine( x[jj], y[jj], x[jj+1], y[jj+1]);
	         }
	         else // not dragging
	         {
		        segments = wm.ToPolygons(s, d);
		        Polygon curr_polygon;

		        if(s.getInterpolate())
		            for(int k = 0; k < segments.size(); k++)
		            {
			            curr_polygon = (Polygon)segments.elementAt(k);
			            g.drawPolyline(curr_polygon.xpoints, curr_polygon.ypoints, curr_polygon.npoints);
		            }
	        }
	    
	        if(dragging && mode == MODE_PAN)
		        continue;//return;    
	        if(s.getMarker() != Signal.NONE)
		        if(s.getMarkerStep() != 0)
		            DrawMarkers(g, segments, s.getMarker(), s.getMarkerStep());
		    else
		        DrawMarkers(g, segments, s.getMarker(), 1);
	        if(s.error)
	            DrawError(g, d, s);
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
	        for(int j = 0; j < curr_polygon.npoints; j++)//j+= step)
	        {
		        points[num_points].x = curr_polygon.xpoints[j];
		        points[num_points].y = curr_polygon.ypoints[j];
		        num_points++;
    	    }
	    }
	    
	    super.DrawMarkers(g, points, num_points, mark_type, step);
	    
    }



    protected Point FindPoint(double curr_x, double curr_y, boolean is_first)	
    {
	    Signal curr_signal;
	    int curr_idx, i, min_idx = 0;
	    double curr_dist, min_dist = 1E20;
	
	//if(signals[curr_point_sig_idx] == null) return 0;
	    if(!is_first)
	    {
	        curr_signal = (Signal)signals.elementAt(curr_point_sig_idx);
	        return FindPoint(curr_signal, curr_x, curr_y);    
		}
		
	    for(curr_point_sig_idx = i = 0; i < signals.size(); i++)
	    {
	        curr_signal = (Signal)signals.elementAt(i);
	        if(curr_signal  == null || !GetSignalState(i)) continue;
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
	    curr_signal= (Signal)signals.elementAt(curr_point_sig_idx);
	    crosshair_color = colors[curr_signal.getColorIdx() % colors.length];
	    
        if(curr_signal.getColor() != null)
            crosshair_color = curr_signal.getColor();
        else
	        crosshair_color = colors[curr_signal.getColorIdx() % colors.length];
	    
	    return FindPoint(curr_signal, curr_x, curr_y);    
    }


	public void UpdatePoint(double curr_x)
	{
			
        if(!is_image)
        {
            if(wm == null) return;
            
		    if(dragging || mode != MODE_POINT || signals == null || signals.size() == 0)
			    return;
			    
			Signal s;
			
			for(int i = 0; i < signals.size(); i++)
			{
			    s = (Signal)signals.elementAt(i);
			    if(s == null) continue;
			    if(s.getType() == Signal.TYPE_2D && 
			        (s.getMode() == Signal.MODE_XY || s.getMode() == Signal.MODE_YX))
			    {
			        s.showXY((float)curr_x);
    	            not_drawn = true;
			    }
			}
		}
		super.UpdatePoint(curr_x);
	}


/*
    protected double FindPointY(double curr_x, double curr_y, boolean is_first)	
    {
	    Signal curr_signal;
	    int curr_idx, i, min_idx = 0;
	    double curr_dist, min_dist = 1E20;
	
	//if(signals[curr_point_sig_idx] == null) return 0;
	    if(!is_first)
	    {
	        curr_signal = (Signal)signals.elementAt(curr_point_sig_idx);
	        min_idx = curr_signal.FindClosestIdx(curr_x, curr_y);
            if((min_idx == curr_signal.n_points -1))// || min_idx == 0)
	    	    return curr_signal.y[min_idx];
	        else
	        {
	            if(curr_signal.getMarker() != Signal.NONE && !curr_signal.getInterpolate())
	            {
	                if(curr_x < curr_signal.x[min_idx] +  (curr_signal.x[min_idx+1] - curr_signal.x[min_idx])/2)
	                {
//	                    end_x = wm.XPixel(curr_signal.x[min_idx], getSize());
	                    return curr_signal.y[min_idx];
	                } else {
//	                    end_x = wm.XPixel(curr_signal.x[min_idx+1], getSize());
	                    return curr_signal.y[min_idx+1];
	                }
	            } else
	                return curr_signal.y[min_idx] + 
	                    (curr_signal.y[min_idx+1] - curr_signal.y[min_idx]) * (curr_x - curr_signal.x[min_idx])/
	                    (curr_signal.x[min_idx+1] - curr_signal.x[min_idx]);
		    }
//		        return curr_signal.y[min_idx] + 
//	                (curr_signal.y[min_idx+1] - curr_signal.y[min_idx]) * (curr_x - curr_signal.x[min_idx])/
//	                (curr_signal.x[min_idx+1] - curr_signal.x[min_idx]);
	        
	    }
	    for(curr_point_sig_idx = i = 0; i < signals.size(); i++)
	    {
	        curr_signal = (Signal)signals.elementAt(i);
	        if(curr_signal  == null || !GetSignalState(i)) continue;
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
	    curr_signal= (Signal)signals.elementAt(curr_point_sig_idx);
	    crosshair_color = colors[curr_signal.getColorIdx() % colors.length];
	    
        if(curr_signal.getColor() != null)
            crosshair_color = curr_signal.getColor();
        else
	        crosshair_color = colors[curr_signal.getColorIdx() % colors.length];
	    
	    
        if((min_idx == curr_signal.n_points -1) || min_idx == 0)
	        return curr_signal.y[min_idx];
	    else 
	    {
	        if(curr_signal.getMarker() != Signal.NONE && !curr_signal.getInterpolate())
	        {
	            if(curr_x - curr_signal.y[min_idx] <  curr_signal.y[min_idx+1] - curr_x)
	                return curr_signal.y[min_idx];
	            else
	                return curr_signal.y[min_idx+1];
	        } else
	            return curr_signal.y[min_idx] + 
	                (curr_signal.y[min_idx+1] - curr_signal.y[min_idx]) * (curr_x - curr_signal.x[min_idx])/
	                (curr_signal.x[min_idx+1] - curr_signal.x[min_idx]);
	     }
//	        return curr_signal.y[min_idx] + 
//	            (curr_signal.y[min_idx+1] - curr_signal.y[min_idx]) * (curr_x - curr_signal.x[min_idx])/
//	            (curr_signal.x[min_idx+1] - curr_signal.x[min_idx]);
    }
*/        
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
            s.setMode(mode);
       }
    }
    
    public void setSignalMode(int mode)
    {
        setSignalMode(curr_point_sig_idx, mode);
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
	
	    if(waveform_signal == null) return; // Cesare
	
	    if(signals == null)
	        return;
	    if(w != this && orig_signals != null)   //Previous zoom for differentr windows
	        signals = orig_signals;

	    waveform_signal.xmin = w.waveform_signal.xmin;
	    waveform_signal.xmax = w.waveform_signal.xmax;

	    AutoscaleY();	
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
    /*
    double x_range = end_xs - start_xs;
    if(orig_signals == null)
    {
	orig_signals = new Signal[signals.length];
	for(int i = 0; i < signals.length; i++)
	    orig_signals[i] = signals[i];
	orig_xmin = waveform_signal.xmin;
	orig_xmax = waveform_signal.xmax;
    }
    if(wi != null)
	wi.AsynchUpdate(signals, (float)(start_xs - x_range), (float)(end_xs + x_range), 
	    (float)orig_xmin, (float)orig_xmax, update_timestamp, mode == MODE_PAN, this);
    */
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

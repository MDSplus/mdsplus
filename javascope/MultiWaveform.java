import java.awt.*;


public class MultiWaveform extends Waveform
/**
Class MultiWaveform extends the capability of class Waveform to deal with multiple
waveforms.
*/
{
    int num_signals;
    Signal signals[];
    Signal orig_signals[];
    int curr_point_sig_idx;	
    public    Color colors[];
    public int num_colors;
    int markers[];
    boolean interpolates[];
    WaveInterface wi;
    static WaveInterface copy_wi;
    static Waveform source_copy_w;
    
    public MultiWaveform(WaveSetup c)
    {
	super(c);
	signals = orig_signals = null;
    	num_signals = 0;
	colors = new Color[4];
	colors[0] = Color.black;
	colors[1] = Color.red;
	colors[2] = Color.green;
	colors[3] = Color.blue;
	num_colors = 4;
    }
    
    
    public void Erase()
    {
	signals = orig_signals = null;
    	num_signals = 0;
	super.Erase();
    }
    
    
    public void Update(WaveInterface _wi)
    {
	wi = _wi;
	orig_signals = null;
	if(wi.colors != null)
	{
	    colors = wi.colors;
	    num_colors =  wi.colors.length;
	}
    	super.x_label = wi.xlabel;
	super.y_label = wi.ylabel;
	super.x_log = wi.x_log;
	super.y_log = wi.y_log;
	super.title = wi.title;
	this.Update(wi.signals, wi.num_waves, wi.markers, wi.interpolates);
    }
	
    public void Update(Signal _signals[], int _num_signals, int _markers[], boolean _interp[])
    {
	int i;
	signals = new Signal[_num_signals];

        num_signals = _num_signals;
	for(i = 0; i < num_signals; i++)
	    if(_signals[i] != null) 
		signals[i] = new Signal(_signals[i]);
 
	orig_signals = null;
	markers = _markers;
	interpolates = _interp;
	curr_point_sig_idx = 0;
	UpdateLimits();
	for(i = 0; i < num_signals; i++)
	    if(signals[i] != null) break;
	if(i < num_signals)    
	    super.Update(waveform_signal);
    }
    	
    public void UpdateSignals(Signal _signals[], int _num_signals, int timestamp)
    {
	//System.out.println("timestamp"+update_timestamp+ timestamp);
    
	if(update_timestamp != timestamp)
	    return;
	signals = _signals;
	num_signals = _num_signals;
	curr_point_sig_idx = 0;
	super.UpdateSignal(waveform_signal);
    }	
   public void SetColors(Color _colors[], int _num_colors)
    {
	colors = _colors;
	num_colors = _num_colors;
    }	
    public void Update(Signal _signals[], int _num_signals)
    {
	int i;
	signals = _signals;
	num_signals = _num_signals;
	markers = new int[num_signals];
	interpolates = new boolean[num_signals];
	for(i = 0; i < num_signals; i++)
	{
	    markers[i] = NONE;
	    interpolates[i] = true;
	}
	curr_point_sig_idx = 0;
	UpdateLimits();
	for(i = 0; i < num_signals; i++)
	    if(signals[i] != null) break;
	if(i < num_signals)    
	    super.Update(waveform_signal);
    }


    void UpdateLimits()
    {
	int i;
	for(i = 0; i < num_signals; i++)
	    if(signals[i] != null)
		break;
	if(i == num_signals) return;
	waveform_signal = new Signal(signals[i]);
	for(i = 1; i < num_signals; i++)
	{
	    if(signals[i] == null) continue;
	    if(waveform_signal.xmax < signals[i].xmax)
		waveform_signal.xmax = signals[i].xmax;
	    if(waveform_signal.ymax < signals[i].ymax)
		waveform_signal.ymax = signals[i].ymax;
	    if(waveform_signal.xmin > signals[i].xmin)
		waveform_signal.xmin = signals[i].xmin;
	    if(waveform_signal.ymin > signals[i].ymin)
		waveform_signal.ymin = signals[i].ymin;
	}
    }	
 

    protected void DrawSignal(Graphics g)
    {	
	int i, j, x[], y[];
	Point curr_points[];
	Dimension d = getSize();
		
	int step, num_steps;
	for(i = 0; i < num_signals; i++)
	{
	    if(signals[i] == null) continue;
	    g.setColor(colors[i % num_colors]);
	    x = new int[signals[i].n_points];
	    y = new int[signals[i].n_points];
	    curr_points = new Point[signals[i].n_points];
	    if(mode == MODE_PAN && dragging && signals[i].n_points > 100)
	    {
		step = signals[i].n_points/100;
		num_steps = 100;
	    }
	    else
	    {
		num_steps = signals[i].n_points;
		step = 1;
	    }
	    for(j = 0; j < num_steps; j++)
	    {
	    	x[j] = wm.XPixel(signals[i].x[step*j], d);
	    	y[j] = wm.YPixel(signals[i].y[step*j], d);
	    	curr_points[j] = new Point(x[j], y[j]);	
	    }
	    
	    
	    
	    if(interpolates == null || interpolates[i])
//		g.drawPolyline(x, y, signals[i].n_points);
		for(int jj = 0; jj < num_steps -1; jj++)
		    if(!Double.isNaN(signals[i].y[step*jj]) && 
			    !Double.isNaN(signals[i].y[step*(jj+1)]))
			g.drawLine( x[jj], y[jj], x[jj+1], y[jj+1]);
		    else
			System.out.println("NaN");
		    
	    if(dragging && mode == MODE_PAN)
		return;    
	    if(markers != null && markers[i] != NONE)
	    	DrawMarkers(g, curr_points, signals[i].n_points, markers[i]);
	    if(signals[i].error)
		DrawError(g, getSize(), signals[i]);
	}
    }

    protected double FindPointY(double curr_x, double curr_y, boolean is_first)	
   {
	Signal curr_signal;
	int curr_idx, i, min_idx = 0;
	double curr_dist, min_dist = 0;
	
	if(signals[curr_point_sig_idx] == null) return 0;
	if(!is_first)
	{
	    min_idx = signals[curr_point_sig_idx].FindClosestIdx(curr_x, curr_y);
	    curr_signal = signals[curr_point_sig_idx];
            if((min_idx == curr_signal.n_points -1) || min_idx == 0)
	    	return curr_signal.y[min_idx];
	    else
		return curr_signal.y[min_idx] + 
	(curr_signal.y[min_idx+1] - curr_signal.y[min_idx]) * (curr_x - curr_signal.x[min_idx])/
	(curr_signal.x[min_idx+1] - curr_signal.x[min_idx]);
	}
	for(curr_point_sig_idx = i = 0; i < num_signals; i++)
	{
	    if(signals[i] == null) continue;
	    curr_idx =  signals[i].FindClosestIdx(curr_x, curr_y);	
	    curr_dist = (signals[i].y[curr_idx] - curr_y)*
		(signals[i].y[curr_idx] - curr_y);
	    if(i == 0 || curr_dist < min_dist)
	    {
		min_dist = curr_dist;
		min_idx = curr_idx;
		curr_point_sig_idx = i;
	    }
	}
	crosshair_color = colors[curr_point_sig_idx % num_colors];
	curr_signal= signals[curr_point_sig_idx];
        if((min_idx == curr_signal.n_points -1) || min_idx == 0)
	    return curr_signal.y[min_idx];
	else
	    return curr_signal.y[min_idx] + 
	(curr_signal.y[min_idx+1] - curr_signal.y[min_idx]) * (curr_x - curr_signal.x[min_idx])/
	(curr_signal.x[min_idx+1] - curr_signal.x[min_idx]);
    }
    protected int GetSelectedSignal() {return curr_point_sig_idx; }


    public void Autoscale()
    {
	int i;
	if(waveform_signal == null) return;
	update_timestamp++;
	if(signals == null)
	    return;
	if(orig_signals != null)   //Previous zoom
	{
	    signals = orig_signals;
	    orig_signals = null;
	}    
	
	for(i = 0; i < num_signals; i++)
	    if(signals[i] != null) break;
	if(i == num_signals) return;
	waveform_signal = signals[i];

    	for(i = 0; i < num_signals; i++)
	{
	    if(signals[i] == null) continue;
	    signals[i].Autoscale();
	    if(signals[i].xmin < waveform_signal.xmin)
		waveform_signal.xmin = signals[i].xmin;
	    if(signals[i].xmax > waveform_signal.xmax)
		waveform_signal.xmax = signals[i].xmax;
	    if(signals[i].ymin < waveform_signal.ymin)
		waveform_signal.ymin = signals[i].ymin;
	    if(signals[i].ymax > waveform_signal.ymax)
		waveform_signal.ymax = signals[i].ymax;
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
	//for(i = 0; i < num_signals; i++)
	//    if(signals[i] != null) break;
	//if(i == num_signals) return;
	//waveform_signal = signals[i];
	double ymin = Double.POSITIVE_INFINITY, ymax = Double.NEGATIVE_INFINITY;
	for(i = 0; i < num_signals; i++)
	{
	    if(signals[i] == null) continue;
	    signals[i].AutoscaleY(prev_xmin, prev_xmax);
            if(signals[i].ymin < ymin)
 	    	ymin = signals[i].ymin;
            if(signals[i].ymax > ymax)
    	    	ymax = signals[i].ymax;
	}
	if(ymin >= ymax)
	{
	    ymin = 0;
	    ymax = ymin + 1E-10;
	}
	waveform_signal.ymin = ymin;
	waveform_signal.ymax = ymax;	
	ReportChanges();
    }
    public void SetXScaleAutoY(Waveform w)
    {
	double xmin, xmax;
	
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
	if(signals != null && waveform_signal == null)
	    return;
	if(orig_signals != null)
	{
	    signals = orig_signals;
	    waveform_signal = signals[0]; 
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
	orig_signals = new Signal[signals.length];
	for(int i = 0; i < signals.length; i++)
	    orig_signals[i] = signals[i];
    }
    if(wi != null)
	wi.AsynchUpdate(signals, start_xs - x_range, end_xs + x_range, 
	    orig_signals[0].xmin,orig_signals[0].xmax, update_timestamp, mode == MODE_PAN, this);
}	


protected void HandleCopy()
{
    if(copy_wi == null)
    {
	copy_wi = wi;
	source_copy_w = (Waveform)this;
	SetSelected(true);
    }
    else
    {
	if(IsSelected())
	{
	    copy_wi = null;
	    SetSelected(false);
	}
	else
	{
	    Update(copy_wi);
	    controller.NotifyChange((Waveform)this, source_copy_w);
	}
    }
}	    
	
protected void SetMode(int mod)
{
    copy_wi = null;
    super.SetMode(mod);
}
	
	

public WaveInterface GetWaveInterface()
{
    if(wi == null)
	return null;
    return new WaveInterface(wi);
}    


}

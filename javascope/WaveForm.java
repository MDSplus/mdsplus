import java.applet.Applet;
import java.awt.*;
import java.io.*;
import Signal;
import PopScopes;

class Grid {
    int x_dim, y_dim;
    float x_values[], y_values[];
    static int grid_step = 3;
    Font font;

    public Grid(Signal s)
    {
	Float f; int i;

	font = null;
	x_values = new float[50];
	y_values = new float[50];
	y_dim = BuildGrid(s.ymin, s.ymax, y_values);
	for(i = 0; i < y_dim; i++)
	{
	    f =new Float(y_values[i]);
	    System.out.println(f.toString());
	}
	x_dim = BuildGrid(s.x[0], s.x[s.n_points - 1], x_values);
	for(i = 0; i < x_dim; i++)
	{
	    f = new Float(x_values[i]);
	    System.out.println(f.toString());
	}

    }

    private int BuildGrid(float min, float max, float val[])
    {
	float step, curr, curr_max, curr_min;
	boolean greater;
	int count = 0, i;
	Float f;

	curr_max = (float)(max + 0.1 * (max - min));
	curr_min = (float)(min - 0.1 * (max - min));
	step = (max - min)/grid_step;
	if(step > 1)
	{
	    greater = true;
	    while(step/10. > 1.)
	    {	
		step /= 10;
		count++;
	    }
	}
	else
	{
	    greater = false;
	    while(step < 1)
	    {
		step *= 10;
		count++;
	    }
	}
	step = (float)((int)step);

	if(greater)
	    for(i = 0; i < count; i++)
		step *= 10;
	else
	    for(i = 0; i < count; i++)
		step /=10;
	curr = (int)(curr_min / step) * step;
	while(curr < curr_min)
	    curr += step;
	for(i = 0; curr < curr_max; i++)
	{
	    val[i] = curr;
	    curr += step;
	}
	return i;
    }

    public void paint(Graphics g, Dimension d, 
	Signal s)
    {
	int i,j, dim;
	float xrange, yrange, xmin, ymin;
	Float fc;

	if(font == null)
	{
	    font = g.getFont();
	    font = new Font(font.getName(), font.getStyle(), 10);
	}
	g.setFont(font);
	xrange = s.x[s.n_points - 1] - s.x[0];
	yrange = s.ymax - s.ymin;
	xmin = s.x[0];
	ymin = s.ymin;	

	for(i = 0; i < y_dim; i++)
	{
	    dim = (int)((0.2 + 0.7 * (s.ymax - y_values[i])/yrange)
		* d.height + 0.5);
	    for(j = 0; j < d.width; j+=4)
		g.fillRect(j, dim, 1, 1);
	    fc = new Float(y_values[i]);
	    g.drawString(fc.toString(), 0, dim);
	}
	for(i = 0; i < x_dim; i++)
	{
	    dim = (int)((0.2 + 0.7 * (x_values[i] - xmin)/xrange)
		* d.width + 0.5);
	    for(j = d.height/10; j < d.height; j+=4)
		g.fillRect(dim, j, 1, 1);
	    fc = new Float(x_values[i]);
	    g.drawString(fc.toString(), dim, d.height - 10);
	}
    }
}





	

class WaveForm extends Canvas 
{
	PopScopes controller;
    Signal curr_sig, orig_sig; 
    boolean not_drawn; 
    Polygon polygon;
    int prev_width, prev_height, prev_point_x, prev_point_y;
    Grid grid;
    Rectangle curr_rect;
    public Label coords;
    int start_x, start_y, end_x, end_y, prev_clip_w, prev_clip_h,
		curr_clip_w, curr_clip_h;	
    Image off_image;
    Graphics off_graphics;	
    int mode;
	boolean ext_update, dragging;
    public static final int MODE_ZOOM = 1, MODE_POINT = 2;


    public WaveForm(Signal s)
    {
	super();
	Dimension d = size();
	orig_sig = new Signal(s);
	curr_sig = new Signal(s, 1000);
 // Reshape al piu' con 1000 punti
	not_drawn = true;
	grid = new Grid(curr_sig);
	curr_rect = null;
	mode = MODE_ZOOM;
	prev_point_x = prev_point_y = -1;
	ext_update = dragging = false;
   }

    public WaveForm()
    {
    	curr_sig = orig_sig = null;
		mode = MODE_ZOOM;
		ext_update = dragging = false;
		
    }

    
	public void Update(Signal s)
    {
	orig_sig = new Signal(s);
	curr_sig = new Signal(s, 1000);
	not_drawn = true;
	grid = new Grid(curr_sig);
	curr_rect = null;
	prev_point_x = prev_point_y = -1;

	repaint();
    }

    public void SetMode(int mod)
    {
		mode = mod;
		if(curr_sig != null)
			repaint();
    }

    void DrawWave()
    {
	Integer ic;
 	Dimension d = size();
	int i, x[] = new int[curr_sig.n_points],
	y[] = new int[curr_sig.n_points];
	float ybase, yrange, xrange;
	ybase = (float)((curr_sig.ymax + curr_sig.ymin)/2.);
	yrange = curr_sig.ymax - curr_sig.ymin;
	xrange = curr_sig.x[curr_sig.n_points - 1] - curr_sig.x[0];
	for(i = 0; i < curr_sig.n_points; i++)
	{
	    x[i] = (int)((0.2 + 0.7 * (curr_sig.x[i] - curr_sig.x[0])
		/(float)xrange) * d.width + 0.5);
	    y[i] = (int)((0.2 + 0.7 *(curr_sig.ymax - curr_sig.y[i])
		/yrange) * d.height + 0.5);    
	}
	polygon = new Polygon(x,y,i); 
	end_x = x[0];
	end_y = y[0];
    }

    public void paint(Graphics g)
    {

	Float fc_x, fc_y;
	float ofs, curr_x, curr_y, xrange, yrange;
	int idx, plot_y;
	Dimension d = size();
	Graphics g1;

	if(curr_sig == null)
	    return;

	g.setColor(new Color(0,0,0));
	if(not_drawn || prev_width != d.width 
		|| prev_height!= d.height )
	{
	    not_drawn = false;
	    g.clipRect(0, 0, d.width, d.height);
	    prev_width = d.width;
	    prev_height = d.height;
	    DrawWave();
	    off_image = createImage(d.width, d.height);
	    off_graphics = off_image.getGraphics(); 
	    off_graphics.setColor(new Color(0,0,0));
	    off_graphics.drawPolygon(polygon);
	    grid.paint(off_graphics, d, curr_sig);
	}
	if(mode == MODE_ZOOM)
	    if(curr_rect != null)
	    {
	    	g.clearRect(start_x, start_y, prev_clip_w,
		    prev_clip_h);
		g1 = g.create(0, 0, d.width, d.height);
	    	g1.clipRect(start_x, start_y, prev_clip_w + 4, prev_clip_h + 4);
	    	g1.drawImage(off_image, 0, 0, this);	
//	    	g.clipRect(start_x, start_y, curr_clip_w, curr_clip_h);
	    	prev_clip_w = curr_clip_w;
	    	prev_clip_h = curr_clip_h;
	    	g.drawRect(curr_rect.x, curr_rect.y, curr_rect.width,
		    curr_rect.height);
	    }
	    else
	        g.drawImage(off_image, 0, 0, this);
	if(mode == MODE_POINT && !not_drawn)
	{
	    ofs = (float)(((end_x - 0.5)/d.width - 0.2)/0.7);
	    idx = (int)(ofs * curr_sig.n_points);
	    xrange = curr_sig.x[curr_sig.n_points - 1] - 
		curr_sig.x[0];
	    yrange = curr_sig.ymax - curr_sig.ymin; 
	    curr_x = ofs * xrange + curr_sig.x[0];
	    if(idx < 0) idx = 0;
	    if(idx >= curr_sig.n_points) 
		idx = curr_sig.n_points - 1;
	    if(idx > 0 && idx < curr_sig.n_points - 1)
		curr_y = curr_sig.y[idx] + (curr_sig.y[idx + 1] -
		    curr_sig.y[idx]) * ( ofs * curr_sig.n_points - idx);
	    else
		curr_y = curr_sig.y[idx];

		if(dragging && !ext_update && controller != null)
			controller.UpdatePoint(curr_x, this);

	    plot_y = (int)((0.2 + 0.7 * (curr_sig.ymax - curr_y)/
		yrange) * d.height + 0.5);
	
	    if(coords != null)
	    {   
	   	fc_x = new Float(curr_x);
	   	fc_y = new Float(curr_y);
  		coords.setText("[" + fc_x.toString() + ", " +
		    fc_y.toString() + "]");
	    }	

	    if(prev_point_x == -1)
	        g.drawImage(off_image, 0, 0, this);
	    else
	    {
	    	g.clearRect(0, prev_point_y - 4, d.width, 8);
		g1 = g.create(0, 0, d.width, d.height);
		g1.clipRect(0, prev_point_y - 4, d.width, 8);
	        g1.drawImage(off_image, 0, 0, this);
	    	g.clearRect(prev_point_x - 4, 0, 8, d.height);
		g1 = g.create(0, 0, d.width, d.height);
		g1.clipRect(prev_point_x - 4, 0, 8, d.height);
	        g1.drawImage(off_image, 0, 0, this);
	        g.drawImage(off_image, 0, 0, this);
	    }
	    g.drawLine(0,plot_y, d.width, plot_y);
	    g.drawLine(end_x, 0, end_x, d.height);
	    prev_point_x = end_x;
	    prev_point_y = plot_y;
    	}
    }

    public boolean mouseDown(Event e, int x, int y)
    {
	start_x = end_x = prev_point_x = x;
	start_y = end_y = prev_point_y = y;
	prev_clip_w = curr_clip_w = prev_clip_h = curr_clip_h = 0;
	dragging = true;
//	repaint();
	return false;
    }

    
	public void UpdatePoint(float curr_x)
	{
		float xrange;
		Dimension d = size();
		if(dragging || mode != MODE_POINT || curr_sig == null)
			return;
		if(curr_x < curr_sig.x[0])
			curr_x = curr_sig.x[0];
		xrange = curr_sig.x[curr_sig.n_points - 1] - curr_sig.x[0];

		end_x = (int)((0.2 + 0.7 * (curr_x - curr_sig.x[0])
		/(float)xrange) * d.width + 0.5);
		ext_update = true;
		repaint();
		ext_update = false;
	}

	
	
	public boolean mouseDrag(Event e, int x, int y)
    {
	int curr_width, curr_height;

	curr_clip_w = x - start_x;
	curr_clip_h = y - start_y;

	end_x = x;
	end_y = y;
	if(mode == MODE_ZOOM && x > start_x && y > start_y)
	{
	    if(curr_rect == null)
		curr_rect = new Rectangle(start_x, start_y, 
			x - start_x, y - start_y);
	    else
		curr_rect.resize(x - start_x, y - start_y);
	    repaint();
	}
	else
	    curr_rect = null;
	if(mode == MODE_POINT)
	    repaint();
	return false;
    }

    public boolean mouseUp(Event e, int x, int y)
    {
	float start_xs, end_xs, start_ys, end_ys;
	float xrange, yrange;
	Dimension d = size();
Float fc;
	if(mode == MODE_ZOOM && x > start_x && y > start_y)
	{

	    yrange = curr_sig.ymax - curr_sig.ymin;
	    xrange = curr_sig.x[curr_sig.n_points - 1] - 
		curr_sig.x[0];

	    
	    start_xs = (float)(((start_x-0.5)/d.width - 0.2) * xrange /0.7 + curr_sig.x[0]);
	    end_xs = (float)(((end_x-0.5)/d.width - 0.2) * xrange /0.7 + curr_sig.x[0]);
	    start_ys = (float)(curr_sig.ymax - ((end_y - 0.5)/d.height - 0.2) * yrange /0.7);
	    end_ys = (float)(curr_sig.ymax - ((start_y - 0.5)/d.height - 0.2) * yrange /0.7);
	    curr_sig = orig_sig.Reshape(1000, start_xs, end_xs, 
		start_ys, end_ys);
	    grid = new Grid(curr_sig);
	    not_drawn = true;
	}
	else
	    if(mode == MODE_ZOOM && x == start_x && y == start_y)
	    {
	    	curr_sig = orig_sig.Reshape(1000);
			grid = new Grid(curr_sig);
			not_drawn = true;
	    }
	curr_rect = null;
	prev_point_x = prev_point_y = -1;
	repaint();
	dragging = false;
	return false;
    }
  
    public void update(Graphics g)
    {
	if((mode == MODE_ZOOM && curr_rect == null) || 
		(mode == MODE_POINT && prev_point_x == -1))
	    super.update(g);
	else
	    paint(g);
    }

}



package jScope;

/* $Id$ */
import jScope.Frames;
import java.awt.*;
import java.awt.datatransfer.DataFlavor;
import java.util.*;
import java.io.*;
import java.lang.String;
import javax.swing.TransferHandler;

/**
 Class MultiWaveform extends the capability of class Waveform to deal with multiple
 waveforms.
 */
public class MultiWaveform
    extends Waveform
{
    static final int MAX_DRAG_POINT = 200;
    static final int HORIZONTAL = 0;
    static final int VERTICAL = 1;

    static final int PRINT_LEGEND = 4;
    static final int PRINT_BW = 8;

    static final int LEGEND_IN_GRAPHICS = 0;
    static final int LEGEND_BOTTOM = 1;
    static final int LEGEND_RIGHT = 2;

    protected Vector<Signal> signals = new Vector<Signal>();
    protected Vector<Signal> orig_signals = null;
    protected double orig_xmin, orig_xmax;
    protected int curr_point_sig_idx = -1;

    protected boolean show_legend = false;
    protected double legend_x;
    protected double legend_y;
    private Point legend_point;
    private int right_size = 0;
    private int bottom_size = 0;
    protected boolean fixed_legend = false;
    private int legend_mode = 0;

    protected WaveInterface wi;

    boolean continuosAutoscale = false;

    public MultiWaveform()
    {
        super();
        if (signals.size() != 0)
            signals.removeAllElements();
        orig_signals = null;

        //{{REGISTER_LISTENERS
        SymContainer aSymContainer = new SymContainer();
        this.addContainerListener(aSymContainer);
        //}}
        //GAB 2014 add Drag And Drop capability
 

    }

    public void Erase()
    {
        if (signals.size() != 0)
            signals.removeAllElements();
        orig_signals = null;
        show_legend = false;
        legend_point = null;
        super.Erase();
    }

    public void Copy(Waveform wave)
    {
        super.Copy(wave);
        if (!wave.is_image)
        {
            MultiWaveform w = (MultiWaveform) wave;

            if (signals.size() != 0)
                signals.removeAllElements();
            Vector<Signal> s = w.GetSignals();
            for (int i = 0; i < s.size(); i++)
            {    
                signals.addElement(new Signal(s.elementAt(i)));
                signals.elementAt(i).registerSignalListener(this);
            }

            show_legend = w.show_legend;
            show_sig_image = w.show_sig_image;
            if (w.show_legend)
            {
                legend_x = w.legend_x;
                legend_y = w.legend_y;
                legend_point = new Point(w.legend_point);
            }

            UpdateLimits();
            if (waveform_signal != null)
                super.Update(waveform_signal);
        }
    }

    public void removeSignal(int idx)
    {
        if (idx < signals.size())
            signals.removeElementAt(idx);
    }

    public void replaceSignal(int idx, Signal s)
    {
        if (idx < signals.size())
        {
            signals.removeElementAt(idx);
            signals.insertElementAt(s, idx);
        }
    }

    public boolean exists(Signal s)
    {
        if (s == null)
            return true;

        if (s.getName() == null || s.getName().length() == 0)
        {
            s.setName("Signal_" + signals.size());
            return false;
        }

        for (int i = 0; i < signals.size(); i++)
        {
            Signal s1 = signals.elementAt(i);
            if (s1.getName() != null && s.getName() != null &&
                s1.getName().equals(s.getName()))
                return true;
        }
        return false;
    }

    public void addSignal(Signal s)
    {
        if (!exists(s))
        {
            signals.addElement(s);
            setLimits();
            s.registerSignalListener(this);
        }
    }

    public void addSignals(Signal s[])
    {
        if (s == null || s.length == 0)
            return;
        for (int i = 0; i < s.length; i++)
        {
            addSignal(s[i]);
            if(s[i] != null)
                s[i].registerSignalListener(this);
        }
        setLimits();
        if (waveform_signal != null)
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

    public void setWaveInterface(WaveInterface wi)
    {
        this.wi = wi;
    }

    public WaveInterface getWaveInterface()
    {
        return wi;
    }

    public double GetLegendXPosition()
    {
        return legend_x;
    }

    public double GetLegendYPosition()
    {
        return legend_y;
    }

    public int getLegendMode()
    {
        return legend_mode;
    }

    public void setLegendMode(int legend_mode)
    {
        this.legend_mode = legend_mode;
        if (legend_mode != this.LEGEND_IN_GRAPHICS)
            fixed_legend = true;
        else
            fixed_legend = false;
    }

    public boolean isFixedLegend()
    {
        return fixed_legend;
    }

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

        if (print_mode == NO_PRINT)
            dim = getWaveSize();
        else
            dim = getPrintWaveSize(d);

        super.PaintSignal(g, d, print_mode);
        if (show_legend && !fixed_legend && !is_min_size)
        {
            Point p = new Point();
            if (legend_point == null || prev_width != d.width ||
                prev_height != d.height)
            {
                p.x = wm.XPixel(legend_x, dim);
                p.y = wm.YPixel(legend_y, dim);
                legend_point = p;
            }
            else
            {
                p = legend_point;
            }
            drawLegend(g, p, print_mode, VERTICAL);
        }

        if (fixed_legend && show_legend ||
            (print_mode & PRINT_LEGEND) == PRINT_LEGEND)
        {
            g.setClip(0, 0, d.width, d.height);
            if (legend_mode == LEGEND_BOTTOM && bottom_size != 0)
                drawLegend(g, new Point(0, dim.height), print_mode, HORIZONTAL);
            if (legend_mode == LEGEND_RIGHT && right_size != 0)
                drawLegend(g, new Point(dim.width, 0), print_mode, VERTICAL);
        }
    }

    public synchronized void UpdateSignals(Signal signals[], int timestamp,
                                           boolean force_update)
    {
        //System.out.println("timestamp "+update_timestamp + " "+ timestamp);
        if (!force_update && update_timestamp != timestamp)
            return;

        for (int i = 0; i < signals.length; i++) {
            signals[i].setAttributes(this.signals.elementAt(i));
        }

        if (this.signals.size() != 0)
            this.signals.removeAllElements();

        for (int i = 0; i < signals.length; i++)
        {
            this.signals.addElement(signals[i]);
            if(signals[i] != null)
                signals[i].registerSignalListener(this);
        }

        if (force_update)
        {
            SetEnabledDispatchEvents(false);
            UpdateLimits();
        }

        if (waveform_signal != null)
        {
            if (curr_point_sig_idx > signals.length || curr_point_sig_idx == -1)
                curr_point_sig_idx = 0;

            if (force_update && continuosAutoscale)
                super.Update(waveform_signal);
            else
                super.UpdateSignal(waveform_signal);
        }

        if (asinchAutoscale)
        {
            AutoscaleY();
            asinchAutoscale = false;
        }

        if (force_update)
            SetEnabledDispatchEvents(true);

    }

    public Vector<Signal> GetSignals()
    {
        return signals;
    }

    public String[] GetSignalsName()
    {
        try {
            String names[] = new String[signals.size()];
            for (int i = 0; i < signals.size(); i++) {
                Signal s = signals.elementAt(i);
                String n = s.getName();
                if (n != null)
                    names[i] = n;
                else {
                    names[i] = new String("Signal_" + i);
                    s.setName(names[i]);
                }
            }
            return names;
        } catch (Exception e) {
            return null;
        }
    }

    public String getSignalName(int idx)
    {
        if (idx < signals.size() && signals.elementAt(idx) != null)
        {
            Signal s = signals.elementAt(idx);
            if (s.getName() == null)
                s.setName(new String("Signal_" + idx));
            return s.getName();
        } else if (is_image && frames != null)
            return frames.getName();
        return null;
    }

    public void SetInterpolate(int idx, boolean interpolate)
    {
        if (idx < signals.size())
            signals.elementAt(idx).setInterpolate(interpolate);
    }

    public boolean GetInterpolate(int idx)
    {
        if (idx < signals.size())
            return signals.elementAt(idx).getInterpolate();
        return false;
    }

    public void SetMarker(int idx, int marker)
    {
        if (idx < signals.size())
            signals.elementAt(idx).setMarker(marker);
    }

    public int GetMarker(int idx)
    {
        if (idx < signals.size())
            return signals.elementAt(idx).getMarker();
        return 0;
    }

    public void SetMarkerStep(int idx, int marker_step)
    {
        if (idx < signals.size())
            signals.elementAt(idx).setMarkerStep(marker_step);
    }

    public int GetMarkerStep(int idx)
    {
        if (idx < signals.size())
            return signals.elementAt(idx).getMarkerStep();
        return 0;
    }

    public void SetColorIdx(int idx, int color_idx)
    {
        if (is_image)
        {
            super.SetColorIdx(color_idx);
            super.SetCrosshairColor(color_idx);
            return;
        }

        if (idx < signals.size())
        {
            signals.elementAt(idx).setColorIdx(color_idx);
            if (idx == curr_point_sig_idx)
                crosshair_color = colors[color_idx % colors.length];
        }
    }

    public int GetColorIdx(int idx)
    {
        if (is_image)
            return super.GetColorIdx();

        if (idx < signals.size())
            return signals.elementAt(idx).getColorIdx();
        return 0;
    }

    public int GetShowSignalCount()
    {
        if (signals != null)
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

    public void setXlimits(double xmin, double xmax)
    {
        if (signals == null)
            return;

        for (int i = 0; i < signals.size(); i++) {
            Signal s = signals.elementAt(i);
            if (s != null)
                s.setXLimits(xmin, xmax, Signal.SIMPLE);
        }
    }

    public void setYlimits(float ymin, float ymax)
    {
        if (signals == null)
            return;

        for (int i = 0; i < signals.size(); i++)
            signals.elementAt(i).setYlimits(ymin, ymax);
    }

    public void setPointSignalIndex(int idx)
    {
        if (idx >= 0 && idx < signals.size()) {
            Signal curr_signal;
            curr_point_sig_idx = idx;

            curr_signal = signals.elementAt(curr_point_sig_idx);

            if (curr_signal == null)
                return;

            if (curr_signal.getColor() != null)
                crosshair_color = curr_signal.getColor();
            else
                crosshair_color = colors[curr_signal.getColorIdx() % colors.length];
        }
    }

    public void SetSignalState(String label, boolean state)
    {
        if (signals != null) {
            for (int i = 0; i < signals.size(); i++) {
                Signal sig = signals.elementAt(i);
                if (sig == null)
                    continue;

                if (sig.getName().equals(label)) {
                    sig.setInterpolate(state);
                    sig.setMarker(Signal.NONE);
                }
            }
            if (mode == Waveform.MODE_POINT) {
                Dimension d = getSize();
                double curr_x = wm.XValue(end_x, d),
                    curr_y = wm.YValue(end_y, d);
                FindPoint(curr_x, curr_y, true);
            }
        }
    }

    public boolean[] GetSignalsState()
    {
        boolean s_state[] = null;
        if (signals != null)
        {
            s_state = new boolean[signals.size()];
            for (int i = 0; i < signals.size(); i++)
                s_state[i] = GetSignalState(i);
        }
        return s_state;
    }

    public boolean GetSignalState(int idx)
    {
        if (idx > signals.size())
            return false;
        Signal s = signals.elementAt(idx);
        if (s == null)
            return false;
        return! (!s.getInterpolate() && s.getMarker() == Signal.NONE);
    }

    public void Update()
    {
        if (!is_image)
        {
            UpdateLimits();
            if (waveform_signal != null)
            {
                curr_point_sig_idx = 0;
                super.Update(waveform_signal);
             }
            else
            {
                not_drawn = true;
                repaint();
            }
        }
        else
        {
            if (frames != null)
                super.Update();
        }
    }

    public void appendUpdate()
    {
        for(int i = 0; i < signals.size(); i++) {
            if( signals.elementAt(i).fullPaint() ) {
                //System.out.println(s.name + "UPDATE");
                Update();
                return;
            }
        }
        //System.out.println("APPEND UPDATE");
        this.appendPaint(getGraphics(), getSize());
    }


    public void Update(Frames frames)
    {
        this.frames = frames;
        this.is_image = true;
        Update();
    }

    public void Update(Signal signals[])
    {
        int i;
        if (signals == null)
            return;

        if (this.signals.size() != 0)
            this.signals.removeAllElements();
        for (i = 0; i < signals.length; i++)
        {
            /*
            if(signals[i].getType() == Signal.TYPE_2D &&
               signals[i].getMode2D() == Signal.MODE_IMAGE)
                 hasSignalImage = true;
            */
            this.signals.addElement(signals[i]);
            if(signals[i] != null)
                signals[i].registerSignalListener(this);
        }
        MultiWaveform.this.Update();
    }

    void UpdateLimits()
    {

        if (signals == null || signals.size() == 0)
            return;

        int i;

        waveform_signal = null;
        if (curr_point_sig_idx == -1 ||
            curr_point_sig_idx >= signals.size() ||
            signals.elementAt(curr_point_sig_idx) == null)
        {
            for (i = 0; i < signals.size(); i++)
                if (signals.elementAt(i) != null)
                    break;
            if (i == signals.size())
                return;
        }
        else
            i = curr_point_sig_idx;

        waveform_signal = new Signal(signals.elementAt(i));

        
        //Check if any of the elements of signals vector refers to absolute time. 
        //In this case set minimum and maximum X value of reference waveform_signal to its limits
        boolean anyLongX = false;
        for (i = 0; i < signals.size(); i++)
        {
            if (signals.elementAt(i) == null)
                continue;
            if (signals.elementAt(i).isLongX()) {
                anyLongX = true;
                waveform_signal.setXLimits(signals.elementAt(i).getXmin(), waveform_signal.getXmax(), Signal.AT_CREATION);
                break;
            }
        }       
        for (i = 0; i < signals.size(); i++)
        {
            if (signals.elementAt(i) == null)
                continue;
            //Avoid introducing 0 as limits for absolute times
            if (anyLongX && !signals.elementAt(i).isLongX())
                continue;
            if (waveform_signal.getXmax() < signals.elementAt(i).getXmax())
                waveform_signal.setXLimits(waveform_signal.getXmin(), signals.elementAt(i).getXmax(), Signal.AT_CREATION);
            if (waveform_signal.getXmin() > signals.elementAt(i).getXmin())
                waveform_signal.setXLimits(signals.elementAt(i).getXmin(), waveform_signal.getXmax(), Signal.AT_CREATION);
            if (waveform_signal.getYmax() < signals.elementAt(i).getYmax())
                waveform_signal.setYmax(signals.elementAt(i).getYmax(), Signal.AT_CREATION);
            if (waveform_signal.getYmin() > signals.elementAt(i).getYmin())
                waveform_signal.setYmin(signals.elementAt(i).getYmin(), Signal.AT_CREATION);
        }
    }

    synchronized public void paint(Graphics g, Dimension d, int print_mode)
    {
        bottom_size = right_size = 0;
        if (fixed_legend && show_legend ||
            (print_mode & PRINT_LEGEND) == PRINT_LEGEND)
        {
            setFont(g);
            if (legend_mode == LEGEND_BOTTOM)
            {
                Dimension dim = getLegendDimension(g, d, HORIZONTAL);
                bottom_size = dim.height;
                g.drawLine(0, dim.height - 1, d.width, dim.height - 1);
            }

            if (legend_mode == LEGEND_RIGHT)
            {
                Dimension dim = getLegendDimension(g, d, VERTICAL);
                right_size = dim.width;
                g.drawLine(dim.width - 1, 0, dim.width - 1, d.height);
            }
        }
        super.paint(g, d, print_mode);
    }

    protected int getRightSize()
    {
        return right_size;
    }

    protected int getBottomSize()
    {
        return bottom_size;
    }

    protected Dimension getLegendDimension(Graphics g, Dimension d,
                                           int orientation)
    {
        Dimension dim = new Dimension(0, 0);
        int curr_width = 0, sum_width = 0;
        Font f = g.getFont();
        int h = f.getSize() + 2;
        FontMetrics fm = getFontMetrics(f);
        Signal sign;

        if (getSignalCount() == 0)
            return dim;

        for (int i = 0; i < getSignalCount(); i++)
        {
            if (!isSignalShow(i))
                continue;

            String lab = getSignalInfo(i);

            char[] lab_ar = lab.toCharArray();

            curr_width = fm.charsWidth(lab_ar, 0, lab_ar.length);
            if (orientation == VERTICAL)
            {
                curr_width += 2 * marker_width;
                dim.height += h;
                if (curr_width > dim.width)
                    dim.width = curr_width;
            }

            if (orientation == HORIZONTAL)
            {
                curr_width += 3 * marker_width;
                if (sum_width + curr_width < d.width)
                {
                    sum_width += curr_width;
                }
                else
                {
                    if (sum_width > dim.width)
                        dim.width = sum_width;
                    sum_width = curr_width;
                    dim.height += h;
                }

            }
        }
        dim.height += (orientation == HORIZONTAL) ? (int) (3. / 2 * h + 0.5) :
            h / 2;
        return dim;
    }

    protected String getSignalInfo(int i)
    {
        Signal sign = signals.elementAt(i);
        String lab = sign.getName();
        if (sign.getType() == Signal.TYPE_2D)
        {
            switch (sign.getMode2D())
            {
                case Signal.MODE_XZ:
                    lab = lab + " [X-Z Y = " +
                        Waveform.ConvertToString(sign.getYinXZplot(), false) + " ]";
                    break;
                case Signal.MODE_YZ:
                    lab = lab + " [Y-Z X = " +  sign.getStringOfXinYZplot() +
                        //Waveform.ConvertToString(sign.getTime(), false) +
                        " ]";
                    break;
/*
                case Signal.MODE_YX:
                    lab = lab + " [Y-X T = " +  sign.getStringTime() +
                       // Waveform.ConvertToString(sign.getTime(), false)
                       " ]";
                    break;
*/
            }
        }
        return lab;
    }

    protected boolean isSignalShow(int i)
    {
        Signal sign = signals.elementAt(i);
        return (sign != null && (sign.getInterpolate() ||
                                 sign.getMarker() != Signal.NONE));
    }

    protected Color getSignalColor(int i)
    {
        if (i > signals.size())
            return Color.black;
        Signal sign = signals.elementAt(i);
        if (sign.getColor() != null)
            return sign.getColor();
        else
            return colors[sign.getColorIdx() % colors.length];
    }

    protected void drawLegend(Graphics g, Point p, int print_mode,
                              int orientation)
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

        if (orientation == VERTICAL)
            g.translate( -marker_width, 0);

        for (int i = 0, py = p.y + h, px = p.x; i < getSignalCount(); i++)
        {
            if (!isSignalShow(i))
                continue;

            if ( (print_mode & PRINT_BW) != PRINT_BW)
                g.setColor(getSignalColor(i));

            s = getSignalInfo(i);

            if (orientation == HORIZONTAL)
            {
                char s_ar[] = s.toCharArray();
                curr_width = fm.charsWidth(s_ar, 0, s_ar.length) +
                    3 * marker_width;
                if (sum_width + curr_width < d.width)
                {
                    px = sum_width;
                    sum_width += curr_width;
                }
                else
                {
                    py += h;
                    px = p.x;
                    sum_width = p.x + curr_width;
                }
            }

            pts[0].x = px + 2 * marker_width;
            pts[0].y = py - marker_width / 2;
            drawMarkers(g, pts, 1, GetMarker(i), 1, -1);

            if ( (GetMarker(i) == Signal.NONE) &&
                ( (print_mode & PRINT_BW) == PRINT_BW))
            {
                drawMarkers(g, pts, 1, curr_marker + 1, 1, -1);
                curr_marker = (curr_marker + 1) % (Signal.markerList.length - 1);
            }

            g.drawString(s, px + 3 * marker_width, py);

            if (orientation == VERTICAL)
                py += h;
        }
        if (orientation == VERTICAL)
            g.translate(marker_width, 0);
        g.setColor(prev_col);
    }

    protected void drawSignal(Graphics g)
    {
        drawSignal(g, getSize(), Waveform.NO_PRINT);
    }

    protected void drawSignal(Graphics g, Dimension d, int print_mode)
    {
        int num_marker = Signal.markerList.length - 1;
        int i, j, x[], y[];
        Point curr_points[];
        Vector segments = null;
        float step;
        int num_steps, marker_step = 1;

        g.setColor(Color.black);

        for (i = 0; i < signals.size(); i++)
        {
            Signal s = signals.elementAt(i);
            if (s == null)
                continue;

            if ( (print_mode & PRINT_BW) != PRINT_BW)
            {
                marker_step = (s.getMarkerStep() > 0) ? s.getMarkerStep() : 1;
                if (s.getColor() != null)
                    g.setColor(s.getColor());
                else
                    g.setColor(colors[s.getColorIdx() % colors.length]);
            }
            else
            {
                if (s.getMarker() != Signal.NONE)
                    marker_step = (int) ( ( (s.getNumPoints() > 1000) ? 100 :
                                           s.getNumPoints() / 10.) + 0.5);
            }

            if (mode == MODE_PAN && dragging && s.getNumPoints() > MAX_DRAG_POINT) //dragging large signals
            {
                int drag_point = MAX_DRAG_POINT;
                if (signals.size() == 1)
                    drag_point = (s.getNumPoints() > MAX_DRAG_POINT * 3) ?
                        MAX_DRAG_POINT * 3 : s.getNumPoints();
                x = new int[s.getNumPoints()];
                y = new int[s.getNumPoints()];
                curr_points = new Point[s.getNumPoints()];
                step = (float) s.getNumPoints() / drag_point;
                num_steps = drag_point;
                for (j = 0; j < num_steps; j++)
                {
                    x[j] = wm.XPixel(s.getX( (int) (step * j)), d);
                    y[j] = wm.YPixel(s.getY( (int) (step * j)), d);
                    curr_points[j] = new Point(x[j], y[j]);
                }
                //if(s.getInterpolate())
                for (int jj = 0; jj < num_steps - 1; jj++)
                    if (!Double.isNaN(s.getY( (int) (step * jj))) &&
                        !Double.isNaN(s.getY( (int) (step * (jj + 1)))))
                        g.drawLine(x[jj], y[jj], x[jj + 1], y[jj + 1]);
            }
            else
            {
                if ( s.getType() == Signal.TYPE_2D &&
                    (s.getMode2D() == Signal.MODE_IMAGE || s.getMode2D() == Signal.MODE_CONTOUR) )
                {
                    if (! (mode == MODE_PAN && dragging))
                    {
                        switch (s.getMode2D())
                        {
                            case Signal.MODE_IMAGE:
                                Image img = this.createImage(d.width, d.height);
                                wm.ToImage(s, img, d, colorMap);
                                g.drawImage(img, 0, 0, d.width, d.height, this);
                                break;
                            case Signal.MODE_CONTOUR:
                                drawSignalContour(s, g, d);
                                break;
                        }
                    }
                }
                else
                {
                    segments = wm.ToPolygons(s, d, appendDrawMode);
                    Polygon curr_polygon;

                    if (segments != null && ( s.getInterpolate() || mode == MODE_PAN && dragging) )
                        for (int k = 0; k < segments.size(); k++)
                        {
                            curr_polygon = (Polygon) segments.elementAt(k);
                            g.drawPolyline(curr_polygon.xpoints,
                                           curr_polygon.ypoints,
                                           curr_polygon.npoints);
                        }

                }
            }

            if (dragging && mode == MODE_PAN)
                continue;

            if (s.getMarker() != Signal.NONE &&
                s.getMode2D() != Signal.MODE_IMAGE)

                //DrawMarkers(g, segments, s.getMarker(), marker_step);
                drawMarkers(g, segments, s);
            if (s.hasError())
                drawError(s, g, d);
        }

        if ( (print_mode & PRINT_BW) == PRINT_BW)
        {
            int curr_marker = 0;
            for (i = 0; i < signals.size(); i++)
            {
                Signal s = signals.elementAt(i);
                if (s == null)
                    continue;
                segments = wm.ToPolygons(s, d, appendDrawMode);
                marker_step = (int) ( ( (s.getNumPoints() > 1000) ? 100 :
                                       s.getNumPoints() / 10.) + 0.5);
                drawMarkers(g, segments, curr_marker + 1, marker_step,
                            s.getMode1D());
                curr_marker = (curr_marker + 1) % num_marker;
            }
        }
        segments = null;
    }

    protected void drawMarkers(Graphics g, Vector segments, Signal s)
    {
        drawMarkers(g, segments, s.getMarker(), s.getMarkerStep(), s.getMode1D());
    }

    protected void drawMarkers(Graphics g, Vector segments, int mark_type,
                               int step, int mode)
    {
        int num_points, num_segments = 0;
        int i;
        Point points[];
        Polygon curr_polygon;

        if(segments != null)
            num_segments = segments.size();

        for (i = num_points = 0; i < num_segments; i++)
            num_points += ( (Polygon) segments.elementAt(i)).npoints;

        points = new Point[num_points];
        for (i = 0; i < num_points; i++)
            points[i] = new Point();
        for (i = num_points = 0; i < num_segments; i++)
        {
            curr_polygon = (Polygon) segments.elementAt(i);
            for (int j = 0; j < curr_polygon.npoints; j += step)
            {
//                if(mode == Signal.MODE_STEP && i%2 == 1)
//                    continue;
                points[num_points].x = curr_polygon.xpoints[j];
                points[num_points].y = curr_polygon.ypoints[j];
                num_points++;
            }
        }

        super.drawMarkers(g, points, num_points, mark_type, 1, mode);

    }

    protected Point FindPoint(double curr_x, double curr_y, boolean is_first)
    {
        return FindPoint(curr_x, curr_y, this.getWaveSize(), is_first);
    }

    protected Point FindPoint(double curr_x, double curr_y, Dimension d, boolean is_first)
    {
        Signal curr_signal;
        int curr_idx = -1, i, img_idx = -1, min_idx = 0;
        double curr_dist = 0, img_dist = Double.MAX_VALUE,  min_dist = Double.MAX_VALUE;

        if (signals == null || signals.size() == 0)
            return null;

        //if(signals[curr_point_sig_idx] == null) return 0;
        if (!is_first)
        {
            curr_signal = signals.elementAt(curr_point_sig_idx);
            return FindPoint(curr_signal, curr_x, curr_y, d);
        }

        for (curr_point_sig_idx = i = 0; i < signals.size(); i++)
        {
            curr_signal = signals.elementAt(i);
            if (curr_signal == null || !GetSignalState(i))
                continue;

            curr_idx = curr_signal.FindClosestIdx(curr_x, curr_y);
            if (curr_signal.getType() == Signal.TYPE_2D &&
                ( curr_signal.getMode2D() == Signal.MODE_IMAGE ||  curr_signal.getMode2D() == Signal.MODE_CONTOUR ))
            {
                double x2D[] = curr_signal.getX2D();
                int inc = (int)(x2D.length / 10.) + 1;
                inc = (curr_idx + inc > x2D.length) ? x2D.length - curr_idx - 1 : inc;
                if(curr_idx >= 0 && curr_idx < x2D.length)
                    img_dist = (x2D[curr_idx] - x2D[curr_idx + inc]) *
                               (x2D[curr_idx] - x2D[curr_idx + inc]);
                    img_idx = i;
            }
            else
            {
                if (curr_signal.hasX())
                    curr_dist = (curr_signal.getY(curr_idx) - curr_y) *
                        (curr_signal.getY(curr_idx) - curr_y) + (curr_signal.getX(curr_idx) - curr_x) *
                        (curr_signal.getX(curr_idx) - curr_x);

                if (i == 0 || curr_dist < min_dist)
                {
                    min_dist = curr_dist;
                    min_idx = curr_idx;
                    curr_point_sig_idx = i;
                }
                //System.out.println("cx "+curr_x+"cy "+curr_y+" dist "+curr_dist + " img dist "+ img_dist);
            }
        }

        try
        {
            if (img_idx != -1)
            {
                 if(curr_idx != -1)
                 {
                     curr_signal = signals.elementAt(curr_point_sig_idx);
                     if (min_dist >
                         10 * (curr_signal.getY(0) - curr_signal.getY(1)) *
                         (curr_signal.getY(0) - curr_signal.getY(1)))
                     {
                         curr_point_sig_idx = img_idx;
                     }
                 }
                 else
                     curr_point_sig_idx = img_idx;


            }
        }catch(Exception exc){}

        setPointSignalIndex(curr_point_sig_idx);
        curr_signal = signals.elementAt(curr_point_sig_idx);

        // ???? 23-4-2003
        //UpdateLimits();
        not_drawn = true;

        Point p = FindPoint(curr_signal, curr_x, curr_y, d);
        return p;
    }

    public void UpdatePoint(double curr_x)
    {
        UpdatePoint(curr_x, Double.NaN);
    }

    public synchronized void UpdatePoint(double curr_x, double curr_y)
    {
        if (!is_image)
        {
            // if(wm == null) { System.out.println("wm == null"); return;}
            // if(dragging || mode != MODE_POINT || signals == null || signals.size() == 0)
            if (mode != MODE_POINT || signals == null || signals.size() == 0)
                return;

            Signal s;

            for (int i = 0; i < signals.size(); i++)
            {
                s = signals.elementAt(i);
                if (s == null)
                    continue;
                if ( s.getType() == Signal.TYPE_2D && s.getMode2D() == Signal.MODE_YZ )
                {
                    s.showYZ(curr_x);
                    not_drawn = true;
                }
             /*   if ( s.getType() == Signal.TYPE_2D && s.getMode2D() == Signal.MODE_PROFILE )
                {
                    s.showProfile(s.getMode2D(), (float) curr_x);
                    not_drawn = true;
                }
            */
            }
        }
        super.UpdatePoint(curr_x, curr_y);
    }

    protected int GetSelectedSignal()
    {
        return curr_point_sig_idx;
    }

    public Signal GetSignal()
    {
        if( signals != null && signals.size() > 0 )
            return signals.elementAt(curr_point_sig_idx);
        else
            return null;
    }

    public int getSignalMode1D(int idx)
    {
        try {
            return signals.elementAt(idx).getMode1D();
        } catch (ArrayIndexOutOfBoundsException e) {
            return -1;
	}
    }

    public int getSignalMode2D(int idx)
    {
        try {
            return signals.elementAt(idx).getMode2D();
        } catch (ArrayIndexOutOfBoundsException e) {
            return -1;
	}
    }

    public int getSignalMode1D()
    {
        return getSignalMode1D(curr_point_sig_idx);
    }

    public int getSignalMode2D()
    {
        return getSignalMode2D(curr_point_sig_idx);
    }
/*
    public int getSignalMode(int idx)
    {
        int mode = -1;
        if (idx >= 0 && idx < signals.size())
        {
            Signal s = signals.elementAt(idx);
            if (s.getType() == Signal.TYPE_1D)
                mode = signals.elementAt(idx).getMode1D();
            else if (s.getType() == Signal.TYPE_2D)
                mode = signals.elementAt(idx).getMode2D();

        }
        return mode;
    }

    public int getSignalMode()
    {
        return getSignalMode(curr_point_sig_idx);
    }
*/
    public int getSignalType(int idx)
    {
        int type = -1;
        if (idx >= 0 && idx < signals.size())
        {
            if (signals.elementAt(idx) != null)
                type = signals.elementAt(idx).getType();
        }
        return type;
    }

    public int getSignalType()
    {
        return getSignalType(curr_point_sig_idx);
    }

    public void setSignalMode(int idx, int mode)
    {
        if (idx >= 0 && idx < signals.size())
        {
            Signal s = signals.elementAt(idx);
            if (s != null)
            {
                if (s.getType() == Signal.TYPE_1D)
                {
                    s.setMode1D(mode);
                }
                else
                {
                    if (s.getType() == Signal.TYPE_2D)
                    {
                        switch(mode)
                        {
                            case Signal.MODE_XZ      : s.setMode2D(mode, (float) wave_point_y); break;
                            case Signal.MODE_YZ      : s.setMode2D(mode, (float) wave_point_x); break;
                            case Signal.MODE_IMAGE   : s.setMode2D(mode, (float) wave_point_x); break;
                            case Signal.MODE_CONTOUR : s.setMode2D(mode, (float) wave_point_x); break;
                        }
                        sendUpdateEvent();
                        Autoscale();
                        /*
                        if (mode == Signal.MODE_XZ &&
                            s.getMode2D() == Signal.MODE_YZ)
                            s.setMode2D(mode, (float) wave_point_y);
                        else
                            s.setMode2D(mode, (float) wave_point_x);
                        */
                    }
                }
            }
        }
        not_drawn = true;
        repaint();
    }

    public void setSignalMode(int mode)
    {
        setSignalMode(curr_point_sig_idx, mode);
    }

    protected void ReportLimits(ZoomRegion r, boolean add_undo)
    {
        continuosAutoscale = false;
        if (!add_undo)
        {
            if (waveform_signal == null)
                return;
            update_timestamp++;
            if (signals == null)
                return;
            if (orig_signals != null) //Previous zoom
            {
                signals = orig_signals;
                orig_signals = null;
            }
        }
        super.ReportLimits(r, add_undo);
        if (add_undo)
            NotifyZoom(r.start_xs, r.end_xs, r.start_ys, r.end_ys,
                       update_timestamp);
    }

    public void Autoscale()
    {
        int i;

        if (is_image && frames != null)
        {
            super.Autoscale();
            return;
        }

        if (waveform_signal == null)
            return;

        update_timestamp++;
        continuosAutoscale = true;

        if (signals == null)
            return;
        if (orig_signals != null) //Previous zoom
        {
            signals = orig_signals;
            orig_signals = null;
        }
//	    this.UpdateLimits();

        for (i = 0; i < signals.size(); i++)
            if (signals.elementAt(i) != null)
                break;
        if (i == signals.size())
            return;

        waveform_signal.setMode1D(signals.elementAt(i).getMode1D());
        waveform_signal.setMode2D(signals.elementAt(i).getMode2D());

        waveform_signal.unblock();
            
        boolean firstHit = true;
        for (i = 0; i < signals.size(); i++)
        {
            if (signals.elementAt(i) == null)
                continue;
            signals.elementAt(i).Autoscale();
            if(firstHit) {
                firstHit = false;
                waveform_signal.setXLimits(signals.elementAt(i).getXmin(), signals.elementAt(i).getXmax(), Signal.SIMPLE); 
                waveform_signal.setYmin(signals.elementAt(i).getYmin(), Signal.SIMPLE);
                waveform_signal.setYmax(signals.elementAt(i).getYmax(), Signal.SIMPLE);
            } else {
                if (!signals.elementAt(i).xLimitsInitialized() || signals.elementAt(i).getXmin() < waveform_signal.getXmin())
                    waveform_signal.setXLimits(signals.elementAt(i).getXmin(),waveform_signal.getXmax(), Signal.SIMPLE);
                if (!signals.elementAt(i).xLimitsInitialized() || signals.elementAt(i).getXmax() > waveform_signal.getXmax())
                    waveform_signal.setXLimits(waveform_signal.getXmin(), signals.elementAt(i).getXmax(), Signal.SIMPLE);
                if (signals.elementAt(i).getYmin() < waveform_signal.getYmin())
                    waveform_signal.setYmin(signals.elementAt(i).getYmin(), Signal.SIMPLE);
                if (signals.elementAt(i).getYmax() > waveform_signal.getYmax())
                    waveform_signal.setYmax(signals.elementAt(i).getYmax(), Signal.SIMPLE);
            }
        }

        ReportChanges();
    }

    public void AutoscaleY()
    {
        int i;
        if (waveform_signal == null)
            return;
        double prev_xmin = waveform_signal.getXmin(),
            prev_xmax = waveform_signal.getXmax();
        if (signals == null)
            return;

        double ymin = Double.POSITIVE_INFINITY, ymax = Double.NEGATIVE_INFINITY;
        for (i = 0; i < signals.size(); i++)
        {
            if (signals.elementAt(i) == null)
                continue;
            signals.elementAt(i).AutoscaleY(prev_xmin, prev_xmax);
            if (signals.elementAt(i).getYmin() < ymin)
                ymin = signals.elementAt(i).getYmin();
            if (signals.elementAt(i).getYmax() > ymax)
                ymax = signals.elementAt(i).getYmax();
        }

        if (ymin >= ymax)
        {
            ymax = ymin + 2 * ymin;
            ymin = ymin - 2 * ymin;
        }
        waveform_signal.setYmin(ymin, Signal.SIMPLE);
        waveform_signal.setYmax(ymax, Signal.SIMPLE);
        ReportChanges();

    }

    public void SetXScale(Waveform w) 
    {
        if (waveform_signal == null) {
            return;
    }
    waveform_signal.setXLimits(w.waveform_signal.getXmin(), w.waveform_signal.getXmax(), Signal.SIMPLE);
    for (int i = 0; i < signals.size(); i++)
    {
        Signal s = signals.elementAt(i);
        if(s != null)
            s.setXLimits(w.waveform_signal.getXmin(), w.waveform_signal.getXmax(), Signal.SIMPLE);
    }
    ReportChanges();
  }
    private boolean asinchAutoscale = false;
    public void SetXScaleAutoY(Waveform w)
    {
        if (waveform_signal == null)
            return;

        if (signals == null)
            return;
        if (w != this && orig_signals != null) //Previous zoom for differentr windows
        {
            signals = orig_signals;
            //operation on signals must not affect original signals
            orig_signals = new Vector<>();
            for (int i = 0; i < signals.size(); i++)
                orig_signals.addElement(signals.elementAt(i));

        }
        waveform_signal.setXLimits(w.waveform_signal.getXmin(), w.waveform_signal.getXmax(), Signal.SIMPLE);
        waveform_signal.setFreezeMode(w.waveform_signal.getFreezeMode());
        for (int i = 0; i < signals.size(); i++)
        {
            Signal s = signals.elementAt(i);
            if(s != null)
            {
                s.setXLimits(w.waveform_signal.getXmin(), w.waveform_signal.getXmax(), Signal.SIMPLE);
                s.setFreezeMode(w.waveform_signal.getFreezeMode());
            }
        }
        AutoscaleY();

        update_timestamp++;
        asinchAutoscale = true;
        NotifyZoom(waveform_signal.getXmin(),
                   waveform_signal.getXmax(),
                   waveform_signal.getYmin(),
                   waveform_signal.getYmax(),
                   update_timestamp);
    }

    public void ResetScales()
    {
        if (signals == null || waveform_signal == null)
            return;
        if (orig_signals != null)
        {
            signals = orig_signals;
 /*           int i;
            for (i = 0; i < signals.size() && signals.elementAt(i) == null;
                 i++)
                ;
            waveform_signal = signals.elementAt(i);
   */       
            for(int i = 0; i < signals.size(); i++)
            {
                if(signals.elementAt(i) != null)
                    signals.elementAt(i).ResetScales();
            }
            
        }
        waveform_signal.ResetScales();
        ReportChanges();
    }

    protected void NotifyZoom(double start_xs, double end_xs, double start_ys,
                              double end_ys,
                              int timestamp)
    {
        double x_range = end_xs - start_xs;
        if (orig_signals == null)
        {
            orig_signals = new Vector<>();
            for (int i = 0; i < signals.size(); i++)
                orig_signals.addElement(signals.elementAt(i));
            orig_xmin = waveform_signal.getXmin();
            orig_xmax = waveform_signal.getXmax();
        }

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
    
    //Inherits from parent in order to force UpdateLimits
    public  void signalUpdated(boolean changeLimits)
    {
      change_limits = changeLimits;
      not_drawn = true;
    //Check if any of the elements of signals vector refers to absolute time. 
    //In this case set minimum and maximum X value of reference waveform_signal to its limits
        boolean anyLongX = false;
        for (int i = 1; i < signals.size(); i++)
        {
            if (signals.elementAt(i) == null)
                continue;
            if(signals.elementAt(i).isLongX())
            {
                anyLongX = true;
                waveform_signal.setXLimits(signals.elementAt(i).getXmin(), 
                        waveform_signal.getXmax(), Signal.AT_CREATION);
                break;
            }
        }       
        
        for (int i = 1; i < signals.size(); i++) {
            if (signals.elementAt(i) == null)
                continue;
            //Avoid introducing 0 as limits for absolute times
            if(anyLongX && !(signals.elementAt(i)).isLongX())
                continue;
            if (waveform_signal.getXmax() < signals.elementAt(i).getXmax())
                waveform_signal.setXLimits(waveform_signal.getXmin(), signals.elementAt(i).getXmax(), Signal.SIMPLE);
            if(waveform_signal.getXmin() > signals.elementAt(i).getXmin())
                waveform_signal.setXLimits(signals.elementAt(i).getXmin(), waveform_signal.getXmax(), Signal.SIMPLE);
            if (waveform_signal.getYmax() < signals.elementAt(i).getYmax())
                waveform_signal.setYmax(signals.elementAt(i).getYmax(), Signal.SIMPLE);
            if (waveform_signal.getYmin() > signals.elementAt(i).getYmin())
                waveform_signal.setYmin(signals.elementAt(i).getYmin(), Signal.SIMPLE);
        }

        repaint();
    }
/*
    void freeze()
    {
        for(int i = 0; i < signals.size(); i++)
            signals.elementAt(i).freeze();
        waveform_signal.freeze();
    }

    void unfreeze()
    {
        for(int i = 0; i < signals.size(); i++)
            signals.elementAt(i).unfreeze();
        waveform_signal.unfreeze();
    }
*/
}

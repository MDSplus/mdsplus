package jScope;

/* $Id$ */
import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.border.*;
import javax.swing.*;
import java.awt.Insets;
import java.awt.image.*;
import java.awt.geom.*;



public class Waveform 
    extends JComponent implements SignalListener{

  public static final int MAX_POINTS = 1000;
  static public boolean is_debug = false;

  public static final Color[] COLOR_SET = {
      Color.black, Color.blue, Color.cyan, Color.darkGray,
      Color.gray, Color.green, Color.lightGray,
      Color.magenta, Color.orange, Color.pink,
      Color.red, Color.yellow};

  public static final String[] COLOR_NAME = {
      "Black", "Blue", "Cyan", "DarkGray",
      "Gray", "Green", "LightGray",
      "Magenta", "Orange", "Pink",
      "Red", "Yellow"};


  public static boolean zoom_on_mb1 = true;

  public static final int MARKER_WIDTH = 8;
  public static final int MODE_ZOOM = 1;
  public static final int MODE_POINT = 2;
  public static final int MODE_PAN = 3;
  public static final int MODE_COPY = 4;
  public static final int MODE_WAIT = 5;
  private static final int MIN_W = 10;
  private static final int MIN_H = 10;

  public static final int NO_PRINT = 0;
  public static final int PRINT = 2;

  protected Signal waveform_signal;

  protected static Color colors[];
  protected static String colors_name[];
  protected static boolean colors_changed = true;

  protected WaveformMetrics wm;
  protected boolean not_drawn = true;
  protected boolean reversed = false;
  protected boolean show_sig_image = false;
  protected Polygon polygon;
  protected int prev_width;
  protected int prev_height;
  protected int prev_point_x;
  protected int prev_point_y;
  protected Grid grid;
  protected Color crosshair_color;
  protected Rectangle curr_rect = null;
  protected int start_x;
  protected int start_y;
  protected int end_x;
  protected int end_y;
  protected int orig_x;
  protected int orig_y;
  protected Image off_image;
  protected Graphics off_graphics;
  protected Rectangle wave_b_box;
  protected String x_label, y_label, z_label, title;
  protected int mode, grid_mode;
  protected boolean dragging, copy_selected = false, resizing = true;
  protected int pan_x, pan_y;
  protected double pan_delta_x, pan_delta_y;
  protected int update_timestamp;
  protected Point points[];
  protected int num_points;
  protected int grid_step_x = 3, grid_step_y = 3;
  protected boolean first_set_point;
  protected int marker_width;
  protected boolean x_log, y_log;
  protected boolean is_mb2, is_mb3;
  protected boolean int_xlabel = true, int_ylabel = true;
  protected Rectangle curr_display_limits;
  protected Cursor def_cursor;
  protected boolean is_select;
  protected static Font font = null;
  protected boolean just_deselected;
  protected WavePopup menu;

  private boolean execute_print = false;
  static double mark_point_x, mark_point_y;
  protected boolean show_measure = false;
  protected boolean change_limits = false;

  protected double wave_point_x = 0, wave_point_y = 0;
  protected double curr_point;
  protected double curr_point_y;

// Variables for image capability
  protected int frame = 0;
  protected int prev_frame = -1;

  protected boolean is_image = false;
  protected boolean is_playing = false;
  protected Frames frames;
  protected double frame_time = 0;
  protected String wave_error = null;

  private static boolean bug_image = true;

  private Vector<WaveformListener> waveform_listener = new Vector<WaveformListener>();

  private Vector<ZoomRegion> undo_zoom = new Vector<ZoomRegion>();
  private boolean send_profile = false;

  protected Border unselect_border;
  protected Border select_border;
  private boolean border_changed = false;
  private javax.swing.Timer play_timer;
  private boolean restart_play = false;
  private boolean pan_enabled = true;
  static protected int horizontal_offset = 0;
  static protected int vertical_offset = 0;

  private double xmax = 1, ymax = 1, xmin = 0, ymin = 0;
  boolean is_min_size;
  private boolean event_enabled = true;

  public float lx_max = Float.MAX_VALUE;
  public float lx_min = Float.MIN_VALUE;
  public float ly_max = Float.MAX_VALUE;
  public float ly_min = Float.MIN_VALUE;

  protected ColorMap colorMap = new ColorMap();
  
  private String properties;
  public void setProperties( String properties) { this.properties = properties;}
  public String getProperties() { return properties;}


  class ZoomRegion {
    double start_xs;
    double end_xs;
    double start_ys;
    double end_ys;

    ZoomRegion(double start_xs, double end_xs, double start_ys, double end_ys) {
      this.start_xs = start_xs;
      this.end_xs = end_xs;
      this.start_ys = start_ys;
      this.end_ys = end_ys;
    }
  }

  static int ixxxx = 0;
  public Waveform(Signal s) {
    this();
    waveform_signal = s;
    waveform_signal.registerSignalListener(this);
    not_drawn = true;
  }

  public Waveform() {
    setName("Waveform_" + (ixxxx++));
    setBorder(BorderFactory.createLoweredBevelBorder());
    setSelectBorder(BorderFactory.createBevelBorder(BevelBorder.RAISED,
        Color.red,
        Color.red));

    play_timer = new javax.swing.Timer(1000, new ActionListener() {
      public void actionPerformed(ActionEvent evt) {
        frame = frames.getNextFrameIdx();
        if (frame == frames.getNumFrame() - 1) {
          frame = 0;
        }
        repaint();

        if (mode == MODE_POINT) {
          sendUpdateEvent();
          if (send_profile) {
            sendProfileEvent();
          }
        }
      }
    });

    prev_point_x = prev_point_y = -1;
    update_timestamp = 0;
    waveform_signal = null;
    mode = MODE_ZOOM;
    dragging = false;
    grid_mode = Grid.IS_DOTTED;
    first_set_point = true;
    marker_width = MARKER_WIDTH;
    x_log = y_log = false;
    setMouse();
    setKeys();
    SetDefaultColors();

    }

  static String ConvertToString(double f, boolean is_log) {
    double curr_f, curr_f1;
    double abs_f;
    int exp;
    String out;
    abs_f = Math.abs(f);
    if (abs_f != Double.POSITIVE_INFINITY) {
      if (abs_f > 1000.) {
        for (curr_f = f, exp = 0; Math.abs(curr_f) > (double) 10;
             curr_f /= (double) 10, exp++) {
          ;
        }
        out = Float.toString((float)(Math.round(curr_f * 100) / 100.)) + "e" + Integer.toString(exp);
      }
      else
      if (abs_f < 1E-3 && abs_f > 0) {
        for (curr_f = f, exp = 0; Math.abs(curr_f) < 1.; curr_f *= (double) 10,
             exp--) {
          ;
        }
        out = Float.toString((float)curr_f) + "e" + Integer.toString(exp);
      }
      else {
        int i;
        out = Float.toString((float)f);
        if (f < 1. && f > -1.) { //remove last 0s
          for (i = out.length() - 1; out.charAt(i) == '0'; i--) {
            ;
          }
          out = out.substring(0, i + 1);
        }
      }
    }
    else {
      out = ( (f > 0) ? "+" : "-") + "inf";
    }
    out = out.trim();
    return out;
  }

  public void SetDefaultColors() {
    if (colors != null && colors_name != null &&
        (colors != COLOR_SET || colors_name != COLOR_NAME)) {
      return;
    }
    colors = COLOR_SET;
    colors_name = COLOR_NAME;

  }

  static int ColorNameToIndex(String name) {
    if (name == null) {
      return 0;
    }
    for (int i = 0; i < COLOR_NAME.length; i++) {
      if (name.toLowerCase().equals(COLOR_NAME[i].toLowerCase())) {
        return i;
      }
    }
    return 0;
  }

  public void Copy(Waveform wave) {
    if (wave.is_image) {
      frames = new Frames(wave.frames);
      frame = wave.frame;
      prev_frame = wave.prev_frame;
      frame_time = wave.frame_time;
      is_image = true;
    }
    else {
      this.font = wave.font;
      is_image = false;
    }
    not_drawn = true;
    repaint();
  }

  protected Dimension getLegendDimension(Graphics g) {
    return new Dimension(0, 0);
  }

  protected int getRightSize() {
    return 0;
  }

  protected int getBottomSize() {
    return 0;
  }

  protected Dimension getPrintWaveSize(Dimension dim) {
    // Dimension dim = getSize();
    return new Dimension(dim.width - getRightSize(), dim.height - getBottomSize());
  }

  protected Dimension getWaveSize() {
    Dimension dim = getSize();
    Insets i = getInsets();
    return new Dimension(dim.width  - getRightSize()  - i.top - i.bottom,
                         dim.height - getBottomSize() - i.right - i.left);
  }

  static public void SetFont(Font f) {
    font = f;
  }

  public void setFont(Graphics g) {
    Font f = g.getFont();
    if (font == null || !font.equals(f)) {
      if (font == null) {
        font = g.getFont();
        font = new Font(font.getName(), font.getStyle(), 10);
        g.setFont(font);
      }
      else {
        g.setFont(font);
      }
    }
  }

  public boolean IsShowSigImage() {
    return show_sig_image;
  }

  public void setShowSigImage(boolean show_sig_image) {
    this.show_sig_image = show_sig_image;
  }

  static boolean isColorsChanged() {
    return colors_changed;
  }

  static public void SetColors(Color _colors[], String _colors_name[]) {
    colors_changed = true;
    colors = _colors;
    colors_name = _colors_name;
  }

  public WaveformMetrics GetWaveformMetrics() {
    return wm;
  }


  static public String[] getColorsName() {
    colors_changed = false;
    return colors_name;
  }

  static public Color[] getColors() {
    colors_changed = false;
    return colors;
  }

  public int GetColorIdx() {
    if (waveform_signal != null) {
      return waveform_signal.getColorIdx();
    }
    else
    if (frames != null) {
      return frames.GetColorIdx();
    }
    return 0;
  }

  public void SetColorIdx(int idx) {
    idx = idx % colors.length;
    if (waveform_signal != null) {
      waveform_signal.setColorIdx(idx);
    }
    else {
      if (frames != null) {
        idx = (idx == 0 ? 1 : idx);
        frames.SetColorIdx(idx);
        SetCrosshairColor(idx);
      }
    }
    this.SetCrosshairColor(idx);
  }

  public String getSignalName() {
    if (is_image && frames != null) {
      return frames.getName();
    }
    else {
      return (waveform_signal != null ? waveform_signal.getName() : "");
    }
  }

  public int getSignalType() {
    return (waveform_signal != null ? waveform_signal.getType() : -1);
  }

  public Signal GetSignal() {
    return waveform_signal;
  }

  public boolean GetInterpolate() {
    return (waveform_signal != null ? waveform_signal.getInterpolate() : true);
  }

  public int GetMarker() {
    return (waveform_signal != null ? waveform_signal.getMarker() : 0);
  }

  public int GetMarkerStep() {
    return (waveform_signal != null ? waveform_signal.getMarkerStep() : 0);
  }

  static public void SetHorizontalOffset(int h_offset) {
    horizontal_offset = h_offset;
  }

  static public int GetHorizontalOffset() {
    return horizontal_offset;
  }

  static public void SetVerticalOffset(int v_offset) {
    vertical_offset = v_offset;
  }

  static public int GetVerticalOffset() {
    return vertical_offset;
  }

  public void SetMarkerStep(int step) {
    if (waveform_signal != null) {
      if (step == 0 || step < 0) {
        step = 1;
      }
      waveform_signal.setMarkerStep(step);
    }
  }

  public int getSignalMode1D() {
    int mode = -1;
    if (waveform_signal != null) {
        mode = waveform_signal.getMode1D();
    }
    return mode;
  }

  public void setSignalMode1D(int mode) {
    if (waveform_signal != null) {
        waveform_signal.setMode1D(mode);
      not_drawn = true;
      repaint();
    }
  }

  public int getSignalMode2D() {
    int mode = -1;
    if (waveform_signal != null) {
        mode = waveform_signal.getMode2D();
    }
    return mode;
  }

  public void setSignalMode2D(int mode) {
    if (waveform_signal != null) {
        waveform_signal.setMode2D(mode);
        if (waveform_signal.getType() == Signal.TYPE_2D)
        {
            Autoscale();
            sendUpdateEvent();
        }
        not_drawn = true;
        repaint();
    }
  }


  public void SetSignalState(boolean state) {
    if (waveform_signal != null) {
      waveform_signal.setInterpolate(state);
      waveform_signal.setMarker(Signal.NONE);
    }
  }

  public int GetShowSignalCount() {
    if (waveform_signal != null) {
      return 1;
    }
    else {
      return 0;
    }
  }

  public Dimension getMinimumSize() {
    Insets i = getInsets();
    return new Dimension(MIN_W + i.right + i.left,
                         MIN_H + i.top + i.bottom);
  }

  public void SetPointMeasure() {
    Dimension d = getWaveSize();
    if (is_image) {
      mark_point_x = (double) end_x;
      mark_point_y = (double) end_y;
      frames.setMeasurePoint(end_x, end_y, d);
    }
    else {
      double curr_x = wm.XValue(end_x, d);
      double curr_y = wm.YValue(end_y, d);

      Point p = FindPoint(curr_x, curr_y, true);

      mark_point_x = curr_x = wave_point_x;
      mark_point_y = curr_y = wave_point_y; //wm.YValue(p.y, d);
    }
  }

  protected float convertX(int x) {
    Insets i = getInsets();
    Dimension d = getWaveSize();
    //return (float)wm.XValue(x - i.right, d);
    return (float) wm.XValue(x, d);
  }

  protected float convertY(int y) {
    Insets i = getInsets();
    Dimension d = getWaveSize();
    //return (float)wm.YValue(y - i.top, d);
    return (float) wm.YValue(y, d);
  }

  public String getIntervalPoints() {
    Dimension d = getWaveSize();
    double curr_x = wm.XValue(end_x, d);
    double curr_y = wm.YValue(end_y, d);

    Point p = FindPoint(curr_x, curr_y, false);

    curr_x = wave_point_x;
    curr_y = wave_point_y;

    return " " + mark_point_x + " " + mark_point_y + " " + curr_x + " " +
        curr_y;
  }

  synchronized public void StopFrame() {
    if (is_image && is_playing) {
      is_playing = false;
      play_timer.stop();
    }
  }

  public void PlayFrame() {
    if (!is_playing) {
      is_playing = true;
      play_timer.start();
    }
  }

  public void setSendProfile(boolean state) {
    send_profile = state;
  }

  public boolean isSendProfile() {
    return send_profile;
  }

  protected void setKeys() {
      final Waveform w = this;
      addKeyListener( new KeyAdapter() {
          public void keyPressed(KeyEvent e)
          {
              if ( e.getKeyCode() == KeyEvent.VK_PAGE_DOWN) {
                if (is_image) {
                  if (frames != null && frames.GetFrameIdx() > 0) {
                    frame = frames.getLastFrameIdx();
                    not_drawn = false;
                  }
                }
                else {
                  Signal s = GetSignal();
                  if (s.getType() == Signal.TYPE_2D) {
                    s.decShow();
                    not_drawn = true;
                  }
                }
              }

              if ( e.getKeyCode() == KeyEvent.VK_PAGE_UP ) {
                if (is_image) {
                  if (frames != null) {
                    frame = frames.getNextFrameIdx();
                    not_drawn = false;
                 }
                }
                else {
                  Signal s = GetSignal();
                  if (s.getType() == Signal.TYPE_2D) {
                    s.incShow();
                    not_drawn = true;
                  }
                }
              }
              //ReportChanges();
              repaint();
              sendUpdateEvent();
          
          }
          public void keyReleased(KeyEvent e)
          {}
          public void keyTyyped(KeyEvent e)
          {}

      });
  }

  

  protected void setMouse() {
    final Waveform w = this;

    addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
        if (getCursor().getType() == Cursor.WAIT_CURSOR) {
          return;
        }

        Insets i = getInsets();
        just_deselected = false;
        Dimension d = getWaveSize();

        requestFocus();

        is_mb2 = is_mb3 = false;

        if ( (e.getModifiers() & Event.ALT_MASK) != 0) {
          is_mb2 = true;
        }
        else
        if ( (e.getModifiers() & Event.META_MASK) != 0) { //Se e' MB3
          is_mb3 = true;

        }
        if (mode == MODE_COPY && !is_mb3) {
          if (is_mb2) {
            if (!IsCopySelected()) {
              sendPasteEvent();
            }
          }
          else {
            sendCutEvent();
          }
          return;
        }

        if (is_mb3) { //e.isPopupTrigger())
          Waveform.this.getParent().dispatchEvent(e);
          return;
        }
        else {
          int x = e.getX() - i.right;
          int y = e.getY() - i.top;

          if (mode != MODE_POINT) {
            update_timestamp++;

          }
          start_x = end_x = prev_point_x = orig_x = x;
          start_y = end_y = prev_point_y = orig_y = y;
          dragging = true;
          first_set_point = true;

          setMousePoint();

          if (mode == MODE_PAN && waveform_signal != null) {
            waveform_signal.StartTraslate();

          }
          if (mode == MODE_POINT && waveform_signal != null) {
            repaint();

          }
          if ( (e.getModifiers() & Event.CTRL_MASK) != 0) {
            if (is_image) {
              if (frames != null && frames.GetFrameIdx() > 0) {
                frame = frames.getLastFrameIdx();
              }
            }
            else {
              Signal s = GetSignal();
              if (s.getType() == Signal.TYPE_2D) {
                s.decShow();
                not_drawn = true;
                repaint();
              }
            }
          }

          if ( (e.getModifiers() & Event.SHIFT_MASK) != 0) {
            if (is_image) {
              if (frames != null) {
                frame = frames.getNextFrameIdx();
              }
            }
            else {
              Signal s = GetSignal();
              if (s.getType() == Signal.TYPE_2D) {
                s.incShow();
                not_drawn = true;
                repaint();
              }
            }
          }

          if (mode == MODE_POINT)
          {
              if (is_image && frames != null) {
                //  if(!frames.contain(new Point(start_x, start_y), d))
                //      return;

                not_drawn = false;
                repaint();
              }
              else
              {
                  Signal s = GetSignal();
                  if (is_mb2 &&
                      s.getType() == Signal.TYPE_2D &&
                      s.getMode2D() == Signal.MODE_CONTOUR)
                  {
                      s.addContourLevel(s.getZValue());
                      not_drawn = true;
                      repaint();
                  }
              }
              sendUpdateEvent();
          }
          else {
            end_x = end_y = 0;
            show_measure = false;
          }
        }
      }

      public void mouseReleased(MouseEvent e) {
        if (getCursor().getType() == Cursor.WAIT_CURSOR) {
          return;
        }

        Insets i = getInsets();
        int idx;
        Dimension d = getWaveSize();

        dragging = false;

        int x = e.getX() - i.right;
        int y = e.getY() - i.top;

        if (mode == MODE_POINT && is_image && frames != null) {
          /*Save current point position*/
          frames.setFramePoint(new Point(end_x, end_y), d);
        }

        if (is_mb3) { //e.isPopupTrigger()) //Se e' MB3
          //    Waveform.this.getParent().dispatchEvent(e);
          return;
        }

        if ( (waveform_signal == null && !is_image) ||
            (frames == null && is_image)) {
          return;
        }

        if (mode == MODE_ZOOM && x != orig_x && y != orig_y) {
          if (!is_image) {
            performZoom();
          }
          not_drawn = true;
        }

        if (mode == MODE_ZOOM && zoom_on_mb1 && x == orig_x && y == orig_y &&
            !is_image) {
          if ( (e.getModifiers() & Event.ALT_MASK) != 0) {
            Resize(x, y, false);
          }
          else {
            Resize(x, y, true);
          }
        }

        if (mode == MODE_PAN && !is_image) {
          NotifyZoom(MinXSignal(), MaxXSignal(), MinYSignal(), MaxYSignal(),
                     update_timestamp);
          grid = null;
          not_drawn = true;
            //July 2014 in order to force resolution adjustment
            try {
                waveform_signal.setXLimits(MinXSignal(), MaxXSignal(), Signal.SIMPLE);
                setXlimits(MinXSignal(), MaxXSignal());
            } catch(Exception exc) {
              System.out.println(exc);
            }
        }

        prev_point_x = prev_point_y = -1;
        if (!is_image) {
          curr_rect = null;
        }
        dragging = false;
        repaint();
        if (is_image && restart_play) {
          play_timer.start();
          restart_play = false;
        }

      }
    });

    addMouseMotionListener(new MouseMotionAdapter() {
      public void mouseDragged(MouseEvent e) {

        if (getCursor().getType() == Cursor.WAIT_CURSOR) {
          return;
        }

        if (waveform_signal == null && !is_image || is_mb2) {
          return;
        }

        //if(is_image && is_playing) return;

        Insets i = getInsets();
        Dimension d = getWaveSize();

        int curr_width, curr_height;
        int x = e.getX() - i.right;
        int y = e.getY() - i.top;

        if ( (e.getModifiers() & Event.META_MASK) != 0 || is_mb3 ||
            e.isPopupTrigger()) { //Se e' MB3
          return;
        }

        if (mode == MODE_ZOOM && x < orig_x) {
          start_x = x;
          end_x = orig_x;
        }
        else {
          end_x = x;
          start_x = orig_x;
        }

        if (mode == MODE_ZOOM && y < orig_y) {
          end_y = orig_y;
          start_y = y;
        }
        else {
          end_y = y;
          start_y = orig_y;
        }

        if (is_image && frames != null) {
          /*Chech if point is out of the image*/
          if (mode == MODE_ZOOM &&
              !frames.contain(new Point(start_x, start_y), d)) {
            return;
          }
          setMousePoint();
        }

        if (mode == MODE_ZOOM) {
          if (is_image && is_playing) {
            restart_play = true;
            play_timer.stop();
          }

          if (curr_rect == null) {
            curr_rect = new Rectangle(start_x, start_y, end_x - start_x,
                                      end_y - start_y);
          }
          else {
            curr_rect.setBounds(start_x, start_y, end_x - start_x,
                                end_y - start_y);
          }
          not_drawn = false;
          repaint();

        }
        else {
          curr_rect = null;

        }
        if (mode == MODE_POINT) {
          not_drawn = false;
          sendUpdateEvent();
          paintImmediately(0, 0, d.width, d.height);
          if (is_image && send_profile) {
            sendProfileEvent();
          }
        }

        if (mode == MODE_PAN && !is_image) {
          if (wm.x_log) {
            pan_delta_x = wm.XValue(start_x, d) / wm.XValue(end_x, d);
          }
          else {
            pan_delta_x = wm.XValue(start_x, d) - wm.XValue(end_x, d);
          }
          if (wm.y_log) {
            pan_delta_y = wm.YValue(start_y, d) / wm.YValue(end_y, d);
          }
          else {
            pan_delta_y = wm.YValue(start_y, d) - wm.YValue(end_y, d);
          }
          not_drawn = false;
          repaint();
        }
      }
    });

  }

  public void Erase() {
    update_timestamp = 0;
    waveform_signal = null;
    dragging = false;
    first_set_point = true;
    marker_width = MARKER_WIDTH;
    x_log = y_log = false;
    off_image = null;
    not_drawn = true;
    frames = null;
    prev_frame = -1;
    grid = null;
    x_label = null;
    y_label = null;
    not_drawn = true;
    // wave_error = null;
    show_sig_image = false;
    repaint();
  }

  public boolean undoZoomPendig() {
    return undo_zoom.size() > 0;
  }

  public int GetMode() {
    return mode;
  }

  public void SetMarker(int marker) {
    if (waveform_signal != null) {
      waveform_signal.setMarker(marker);
    }
  }

  public void SetMarkerWidth(int marker_width) {
    this.marker_width = marker_width;
  }

  public void SetInterpolate(boolean interpolate) {
    if (waveform_signal != null) {
      waveform_signal.setInterpolate(interpolate);
    }
  }

  public void SetXLog(boolean x_log) {
    this.x_log = x_log;
  }

  public void SetYLog(boolean y_log) {
    this.y_log = y_log;
  }

  public void SetCrosshairColor(Color crosshair_color) {
    this.crosshair_color = crosshair_color;
  }

  public void SetCrosshairColor(int idx) {
    crosshair_color = colors[idx % colors.length];
  }

  public boolean Playing() {
    return is_playing;
  }

  public void SelectWave() {
    if (!is_select) {
      is_select = true;
      border_changed = true;
      unselect_border = getBorder();
      if (!unselect_border.getBorderInsets(this).equals(select_border.
          getBorderInsets(this))) {
        not_drawn = true;
      }
      setBorder(select_border);
    }
  }

  public void DeselectWave() {
    is_select = false;
    border_changed = true;
    if (!unselect_border.getBorderInsets(this).equals(select_border.
        getBorderInsets(this))) {
      not_drawn = true;
    }
    setBorder(unselect_border);
  }

  public void Update(float x[], float y[]) {
    wave_error = null;
    if (x.length <= 1 || y.length <= 1) {
      wave_error = " Less than two points";
      waveform_signal = null;
      Update();
      return;
    }

    Update(new Signal(x, y));
    repaint();
   }

  synchronized public void Update(Signal s) {
    update_timestamp++;
    waveform_signal = s;
    waveform_signal.registerSignalListener(this);
    wm = null;
    curr_rect = null;
    prev_point_x = prev_point_y = -1;
    not_drawn = true;
    repaint();
  }

  public void Update() {
    wm = null;
    curr_rect = null;
    prev_point_x = prev_point_y = -1;
    not_drawn = true;
    repaint();
  }


  boolean appendDrawMode = false;


  public void appendUpdate() {
      appendPaint(getGraphics(), getSize());
  }


  public void UpdateSignal(Signal s) { //Same as Update, except for grid and metrics
    waveform_signal = s;
   waveform_signal.registerSignalListener(this);
    curr_rect = null;
    prev_point_x = prev_point_y = -1;
    not_drawn = true;
    repaint();
  }

  public void UpdateImage(Frames frames) {
    SetFrames(frames);
    if (frames != null && frames.getNumFrame() > 0) {
      //frames.curr_frame_idx = 0;
    }
    this.is_image = true;
    curr_rect = null;
    prev_point_x = prev_point_y = -1;
    not_drawn = true;
    repaint();
  }

  public int GetGridMode() {
    return grid_mode;
  }

  public void SetGridMode(int grid_mode, boolean int_xlabel, boolean int_ylabel) {
    this.grid_mode = grid_mode;
    this.int_xlabel = int_xlabel;
    this.int_ylabel = int_ylabel;
    wm = null;
    grid = null;
    not_drawn = true;
    //repaint();
  }

  private void setMousePoint() {
    if (is_image && frames != null) {
      Dimension d = getWaveSize();
      Point p_pos;
      p_pos = new Point(end_x, end_y);
      frames.getFramePoint(p_pos, d);
      end_x = p_pos.x;
      end_y = p_pos.y;
    }
  }

  protected void resetMode() {
    SetMode(mode);
  }

  public void SetEnabledPan(boolean pan_enabled) {
    this.pan_enabled = pan_enabled;
  }

  public void SetEnabledDispatchEvents(boolean event_enabled) {
    this.event_enabled = event_enabled;
  }

  protected void SetMode(int mode) {
    if (def_cursor == null) {
      def_cursor = getCursor();
    }
    switch (mode) {
      case MODE_PAN:
        if (pan_enabled) {
          setCursor(new Cursor(Cursor.MOVE_CURSOR));
          this.mode = mode;
        }
        break;
      case MODE_COPY:
        setCursor(new Cursor(Cursor.HAND_CURSOR));
        this.mode = mode;
        break;
      case MODE_WAIT:
        setCursor(new Cursor(Cursor.WAIT_CURSOR));
        this.mode = mode;
        break;
      case MODE_ZOOM:
        setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
        this.mode = mode;
        break;
      default:
        setCursor(def_cursor);
        this.mode = mode;
        break;
    }
    if (waveform_signal != null || is_image) {
      if (mode == MODE_POINT && is_image && frames != null) {
        Point p = frames.getFramePoint(getWaveSize());
        end_x = p.x;
        end_y = p.y;
      }
      repaint();
      if (mode == MODE_POINT) {
        //            sendUpdateEvent();
        if (is_image && send_profile) {
          sendProfileEvent();
        }
      }
    }
  }

  void DrawWave(Dimension d) {
    int i;
    int curr_mode = waveform_signal.getMode1D();
    if (curr_mode == Signal.MODE_STEP) {
      num_points = waveform_signal.getNumPoints() * 2 - 1;
    }
    else {
      num_points = waveform_signal.getNumPoints();

    }
    int x[] = new int[waveform_signal.getNumPoints()];
    int y[] = new int[waveform_signal.getNumPoints()];
    points = new Point[waveform_signal.getNumPoints()];

    if (curr_mode == Signal.MODE_STEP) {
      for (i = 1; i < waveform_signal.getNumPoints() - 1; i += 2) {
        x[i-1] = wm.XPixel(waveform_signal.getX(i), d);
        y[i-1] = wm.YPixel(waveform_signal.getY(i), d);
        x[i] = x[i - 1];
        y[i] = y[i + 1];
        points[i] = new Point(x[i - 1], y[i + 1]);
      }
    }
    else {
      for (i = 0; i < waveform_signal.getNumPoints(); i++) {
        x[i] = wm.XPixel(waveform_signal.getX(i), d);
        y[i] = wm.YPixel(waveform_signal.getY(i), d);
        points[i] = new Point(x[i], y[i]);
      }
    }

    //num_points = waveform_signal.n_points;
    polygon = new Polygon(x, y, i);
    end_x = x[0];
    end_y = y[0];
  }


  synchronized public void PaintImage(Graphics g, Dimension d, int print_mode) {
    if (frames != null) {
      DrawFrame(g, d, frame);
      prev_frame = frames.GetFrameIdx();
    }
    else {
      prev_frame = -1;
      curr_rect = null;
    }

     grid = new Grid(xmax, ymax, xmin, ymin, x_log, y_log, grid_mode, x_label,
                    y_label,
                    title, wave_error, grid_step_x, grid_step_y, int_xlabel,
                    int_ylabel, true);
    if (!is_min_size) {
      grid.paint(g, d, this, null);

    }
  }



  synchronized protected void PaintSignal(Graphics g, Dimension dim, int print_mode) {
    Dimension d;
    String orizLabel = x_label;
    String vertLabel = y_label;
    String sigTitle = title;

    if (print_mode == NO_PRINT) {
      d = getWaveSize();
    }
    else {
      d = getPrintWaveSize(dim);

    }
    xmax = 1;
    ymax = 1;
    xmin = 0;
    ymin = 0;

    if (mode != MODE_PAN || dragging == false) {
      if (waveform_signal != null) {

          //TACON MOSTRUOSO per gestire il fatto che jScope vede solo gli offsets nei times!!!!
          
        xmax =  MaxXSignal();
        xmin =  MinXSignal();
        ymax =  MaxYSignal();
        ymin =  MinYSignal();
        if(xmax != Double.MAX_VALUE && xmin != Double.MIN_VALUE)
        {
            double xrange = xmax - xmin;
            xmax += xrange * horizontal_offset / 200.;
            xmin -= xrange * horizontal_offset / 200.;
        }
        double yrange = ymax - ymin;
        ymax += yrange * vertical_offset / 200.;
        ymin -= yrange * vertical_offset / 200.;

        orizLabel = (x_label == null) ? waveform_signal.getXlabel() : x_label;
        vertLabel = (y_label == null) ? waveform_signal.getYlabel() : y_label;
        sigTitle = (title == null || title.length() == 0) ?
            waveform_signal.getTitlelabel() : title;

        if (waveform_signal.getType() == Signal.TYPE_2D) {
          switch (waveform_signal.getMode2D()) {
            case Signal.MODE_IMAGE:

              //orizLabel = waveform_signal.getXlabel();
              vertLabel = waveform_signal.getZlabel();
              break;
            case Signal.MODE_YZ:
              orizLabel = vertLabel;
              vertLabel = waveform_signal.getZlabel();
              break;
            case Signal.MODE_XZ:
              //orizLabel = waveform_signal.getZlabel();
              vertLabel = waveform_signal.getZlabel();
              break;
          }
        }
      }
    }

    if (resizing || grid == null || wm == null || change_limits) {
      change_limits = false;

      grid = new Grid(xmax, ymax, xmin, ymin, x_log, y_log, grid_mode,
                      orizLabel, vertLabel,
                      sigTitle, wave_error, grid_step_x, grid_step_y,
                      int_xlabel, int_ylabel, reversed);
      curr_display_limits = new Rectangle();
      grid.GetLimits(g, curr_display_limits, y_log);
      wm = new WaveformMetrics(xmax, xmin, ymax, ymin, curr_display_limits,
                               d, x_log, y_log, 0, 0);
    }
    else {
      grid.updateValues(orizLabel, vertLabel,
                        sigTitle, wave_error,
                        grid_step_x, grid_step_y,
                        int_xlabel, int_ylabel,
                        reversed);
      grid.setLabels(sigTitle, orizLabel, vertLabel);
    }

    if(waveform_signal != null)
    {
        grid.setXaxisHMS(waveform_signal.isLongX());
    }
    if (!copy_selected || print_mode != NO_PRINT) {
      if (reversed && print_mode == NO_PRINT) {
        g.setColor(Color.black);
      }
      else {
        g.setColor(Color.white);
      }
    }
    else {
      g.setColor(Color.lightGray);

    }
    g.fillRect(1, 1, d.width - 2, d.height - 2);


    if (waveform_signal != null) {
      wave_b_box = new Rectangle(wm.XPixel(MinXSignal(), d),
                                 wm.YPixel(MaxYSignal(), d),
                                 wm.XPixel(MaxXSignal(), d) -
                                 wm.XPixel(MinXSignal(), d) + 1,
                                 wm.YPixel(MinYSignal(), d) -
                                 wm.YPixel(MaxYSignal(), d) + 1);

      if (print_mode == NO_PRINT) {
         g.clipRect(curr_display_limits.width, 0,
                   d.width - curr_display_limits.width,
                   d.height - curr_display_limits.height);
      }
      drawSignal(g, d, print_mode);
  }

    if(print_mode == PRINT && mode == MODE_POINT)
    {
      /*
      * Needed to print crosss hair
      */
       double curr_x = wm.XValue(end_x, d);
       double curr_y = wm.YValue(end_y, d);

       Point p = FindPoint(curr_x, curr_y, d, true);

       if (p != null) {
         Color prev_color = g.getColor();
         if (crosshair_color != null) {
           g.setColor(crosshair_color);
         }
         g.drawLine(0, p.y, d.width, p.y);
         g.drawLine(p.x, 0, p.x, d.height);
         g.setColor(prev_color);
       }
    }

    if (!is_min_size && grid != null) {
      grid.paint(g, d, this, wm);
    }
  }

  //Send Waveform event routines
  public void sendPasteEvent() {
    WaveformEvent we = new WaveformEvent(this, WaveformEvent.COPY_PASTE);
    dispatchWaveformEvent(we);
  }

  public void sendCutEvent() {
    WaveformEvent we = new WaveformEvent(this, WaveformEvent.COPY_CUT);
    dispatchWaveformEvent(we);
  }

  public void sendUpdateEvent() {

    double curr_x, curr_y;
    WaveformEvent we;
    Dimension d = getWaveSize();

    if (is_image) {
      if (frames != null && frames.getNumFrame() != 0) {
        Point p = frames.getFramePoint(new Point(end_x, end_y), d);
//        int frame_type = frames.getFrameType(frames.GetFrameIdx());
        int frame_type = frames.getFrameType();

        we = new WaveformEvent(this, WaveformEvent.POINT_UPDATE,
                               p.x, p.y,
                               frames.GetFrameTime(), 0,
                               frames.getPixel(frames.GetFrameIdx(), p.x, p.y),
                               0);

        if (frame_type == FrameData.BITMAP_IMAGE_32 || frame_type == FrameData.BITMAP_IMAGE_16 || frame_type == FrameData.BITMAP_IMAGE_8 ) {
          we.setPointValue(frames.getPointValue(frames.GetFrameIdx(), p.x, p.y));
        }
        we.setFrameType(frame_type);

        dispatchWaveformEvent(we);
      }
    }
    else {
      if (waveform_signal != null && wm != null)
      {

        curr_x = wm.XValue(end_x, d);
        curr_y = wm.YValue(end_y, d);

        Point p = FindPoint(curr_x, curr_y, first_set_point);
        first_set_point = false;

        curr_x = curr_point = wave_point_x;
        curr_y = curr_point_y = wave_point_y;

        //System.out.println("Curr point "+curr_point+" on "+getName());

        double dx = 0, dy = 0;
        int event_id;

        dx = curr_x - mark_point_x;
        dy = curr_y - mark_point_y;

        if (show_measure) {
          event_id = WaveformEvent.MEASURE_UPDATE;
        }
        else {
          event_id = WaveformEvent.POINT_UPDATE;

        }
        we = new WaveformEvent(this, event_id, curr_x, curr_y, dx, dy, 0,
                               GetSelectedSignal());
        Signal s = GetSignal();
        we.setTimeValue(s.getXinYZplot());
        we.setXValue(s.getYinXZplot());
        we.setDataValue(s.getZValue());
        we.setIsMB2(is_mb2);
        
        
        if(s.isLongXForLabel())
            we.setDateValue(0);
        

        dispatchWaveformEvent(we);
      }
    }
  }

  public void sendProfileEvent() {
    if (frames != null && frames.getNumFrame() != 0) {
      WaveformEvent we;
      Dimension d = getWaveSize();
      Point p = frames.getFramePoint(new Point(end_x, end_y), d);
      int frame_type = frames.getFrameType();

      if (frame_type == FrameData.BITMAP_IMAGE_32 ||
          frame_type == FrameData.BITMAP_IMAGE_16 ) {
        we = new WaveformEvent(this,
                               p.x, p.y, (float)(Math.round(frames.GetFrameTime() * 10000) / 10000.) ,
                               frames.getName(),
                               frames.getValuesX(p.y),
                               frames.getStartPixelX(),
                               frames.getValuesY(p.x),
                               frames.getStartPixelY());
        if (show_measure) {
          Point p_pos = new Point( (int) mark_point_x, (int) mark_point_y);
          Point mp = frames.getFramePoint(p_pos, d);
          we.setValuesLine(frames.getValuesLine(mp.x, mp.y, p.x, p.y));
        }
        we.setFrameType(frame_type);
      }
      else {
        we = new WaveformEvent(this,
                               p.x, p.y, (float)(Math.round(frames.GetFrameTime() * 10000) / 10000.) ,
                               frames.getName(),
                               frames.getPixelsX(p.y),
                               frames.getStartPixelX(),
                               frames.getPixelsY(p.x),
                               frames.getStartPixelY());
        if (show_measure) {
          Point p_pos = new Point( (int) mark_point_x, (int) mark_point_y);
          Point mp = frames.getFramePoint(p_pos, d);
          we.setPixelsLine(frames.getPixelsLine(mp.x, mp.y, p.x, p.y));
        }
        we.setFrameType(frame_type);
      }
      dispatchWaveformEvent(we);
    }
  }

  private void ImageActions(Graphics g, Dimension d) {
    double curr_x, curr_y;
    int plot_y;

    if (frames != null && frames.getNumFrame() != 0 &&
        (mode == MODE_POINT || mode == MODE_ZOOM)
        && !not_drawn && !is_min_size
        || (frames != null && send_profile && prev_frame != frame)) {

      //if(!is_playing )
      {

        if (mode == MODE_POINT) {
          Color prev_color = g.getColor();
          if (crosshair_color != null)
            g.setColor(crosshair_color);

          g.drawLine(0, end_y, d.width, end_y);
          g.drawLine(end_x, 0, end_x, d.height);

          if (show_measure) {
            g.setColor(Color.green);
            Point mp = frames.getMeasurePoint(d);
            g.drawLine(mp.x, mp.y, end_x, end_y);
          }
          g.setColor(prev_color);
        }
      }
    }
  }


  private void SignalActions(Graphics g, Dimension d) {
    double curr_x, curr_y;
    int plot_y, plot_x;

    if (waveform_signal != null && mode == MODE_POINT
        && !not_drawn && !is_min_size && wm != null) {

      curr_x = curr_point;
      curr_y = wm.YValue(end_y, d);

      Point p = FindPoint(curr_x, curr_y, first_set_point);
      first_set_point = false;

      if (p != null) {
        if (!Double.isNaN(curr_point_y))
           p.y = wm.YPixel(curr_point_y, d);

        curr_x = wave_point_x;
        curr_y = wave_point_y;

        Color prev_color = g.getColor();
        if (crosshair_color != null) {
          g.setColor(crosshair_color);

        }
        g.drawLine(0, p.y, d.width, p.y);
        g.drawLine(p.x, 0, p.x, d.height);

        g.setColor(prev_color);
        prev_point_x = p.x;
        prev_point_y = p.y;
      }
    }

    if (mode == MODE_PAN && dragging && waveform_signal != null) {
      waveform_signal.Traslate(pan_delta_x, pan_delta_y, wm.x_log, wm.y_log);
      wm = new WaveformMetrics(MaxXSignal(), MinXSignal(), MaxYSignal(),
                               MinYSignal(), curr_display_limits, d, wm.x_log,
                               wm.y_log, 0, 0);

      if (reversed) {
        g.setColor(Color.black);
      }
      else {
        g.setColor(Color.white);
      }
      g.fillRect(1, 1, d.width - 2, d.height - 2);
      g.setColor(Color.black);
      g.clipRect(wave_b_box.x, wave_b_box.y, wave_b_box.width,
                 wave_b_box.height);
      drawSignal(g);
    }
  }

  public void setSelectBorder(Border border) {
    select_border = border;
  }

  public void Repaint(boolean state) {
    not_drawn = state;
    repaint();
  }

  public void paintComponent(Graphics g) {
    if (execute_print) {
      return;
    }
    
    Insets i = this.getInsets();
    
    Dimension d = getSize();
    paint(g, d, NO_PRINT);
    
    if (mode == MODE_POINT && send_profile) 
        sendProfileEvent();

  }


  boolean appendPaintFlag = false;
  synchronized public void appendPaint(Graphics g, Dimension d)
  {
      Insets i = this.getInsets();
      setFont(g);
      g.setColor(Color.black);
      if (!is_image)
      {
          if(wm != null)
          {
              appendDrawMode = true;
              appendPaintFlag = true;
              drawSignal(g, d, NO_PRINT);
              appendDrawMode = false;
          }
          else
              PaintSignal(g, d, NO_PRINT);

      }
      else
          PaintImage(g, d, NO_PRINT);


  }


  synchronized public void paint(Graphics g, Dimension d, int print_mode)
  {
      execute_print = (print_mode != NO_PRINT);
      Insets i = this.getInsets();
      setFont(g);
      Dimension dim;

      Graphics2D g2D = (Graphics2D) g;

      if (   not_drawn
          || prev_width != d.width
          || prev_height != d.height
          || execute_print
          || (is_image && prev_frame != frame)
          || appendPaintFlag)
      {

          appendPaintFlag = false;
          not_drawn = false;
          if (print_mode == NO_PRINT)
          {

              resizing = prev_width != d.width || prev_height != d.height;

              if (resizing)
              {
                  if (is_image && frames != null)
                  {
                      try {
                          Point p = frames.getFramePoint(new Point(end_x, end_y),
                              new Dimension(prev_width, prev_height));
                          p = frames.getImagePoint(p, d);
                          end_x = p.x;
                          end_y = p.y;
                      }catch (Exception exc){}
                  }
              }

              dim = getWaveSize();
              if (dim.width < MIN_W || dim.height < MIN_H)
                  is_min_size = true;
              else
                  is_min_size = false;

              {
                  if (bug_image)
                  {
                      bug_image = false;
                      off_image = createImage(1, 1);
                      off_graphics = off_image.getGraphics();
                      off_graphics.drawString("", 0, 0);
                      off_graphics.dispose();
                  }
                  off_image = createImage(d.width, d.height);
                  off_graphics = off_image.getGraphics();
                  setFont(off_graphics);
                  paintBorder(off_graphics);
                  border_changed = false;
                  off_graphics.translate(i.right, i.top);
              }
          }
          else
          {
              resizing = true;
              dim = d;
              off_graphics = g;
              g.setColor(Color.black);
              off_graphics.drawRect(0, 0, d.width - 1, d.height - 1);
          }

          off_graphics.setClip(0, 0, dim.width, dim.height);
          if (!is_image)
          {
              PaintSignal(off_graphics, d, print_mode);
          }
          else
          {
              PaintImage(off_graphics, d, print_mode);

          }
          if (print_mode == NO_PRINT)
          {
              off_graphics.translate( -i.right, -i.top);
              off_graphics.setClip(0, 0, d.width, d.height);

              prev_width = d.width;
              prev_height = d.height;
          }
      }
      else
      {
          if (border_changed)
          {
              paintBorder(off_graphics);
              border_changed = false;
          }
      }

      if (execute_print)
      {
          execute_print = false;
          System.gc();
          return;
      }

      if (! (mode == MODE_PAN && dragging && waveform_signal != null) ||
          is_image)
      {
          if (off_image != null)
              g.drawImage(off_image, 0, 0, this);
      }
      g.translate(i.right, i.top);

      if (mode == MODE_ZOOM)
      {
          if (curr_rect != null)
          {
              if (is_image && crosshair_color != null)
              {
                  g.setColor(crosshair_color);
              }
              else
              {
                  if (reversed)
                      g.setColor(Color.white);
                  else
                      g.setColor(Color.black);
              }
              g.drawRect(curr_rect.x, curr_rect.y, curr_rect.width,
                         curr_rect.height);
          }
      }

      if (is_image)
          ImageActions(g, d = getWaveSize());
      else
          SignalActions(g, d = getWaveSize());

      if (show_measure && mode == MODE_POINT)
      {
          int mark_px, mark_py;
          Color c = g.getColor();
          g.setColor(Color.red);
          if (is_image)
          {
              mark_px = (int) mark_point_x;
              mark_py = (int) mark_point_y;
              Point mp = frames.getMeasurePoint(d);
              mark_px = mp.x;
              mark_py = mp.y;
          }
          else
          {
              mark_px = wm.XPixel(mark_point_x, d);
              mark_py = wm.YPixel(mark_point_y, d);
          }

          g.drawLine(mark_px, 0, mark_px, d.height);
          g.drawLine(0, mark_py, d.width, mark_py);
          g.setColor(c);
      }
      g.translate( -i.right, -i.top);
  }

  protected int GetSelectedSignal() {
    return 0;
  }

  protected Point FindPoint(Signal s, double curr_x, double curr_y,  Dimension d) {
    double x = 0.0, y = 0.0;

    if ( s == null ) {
      return null;
    }

    if (s.getType() == Signal.TYPE_2D)
    {
        Point p;

        switch (s.getMode2D())
        {
            case Signal.MODE_IMAGE:
                wave_point_x = s.getClosestX(curr_x);
                wave_point_y = s.getClosestY(curr_y);
                d = getWaveSize();
                p = new Point(wm.XPixel(wave_point_x, d),
                              wm.YPixel(wave_point_y, d));
                return p;
            case Signal.MODE_CONTOUR:
                wave_point_x = curr_x;
                wave_point_y = curr_y;
                s.surfaceValue(wave_point_x, wave_point_y);
                d = getWaveSize();
                p = new Point(wm.XPixel(wave_point_x, d),
                              wm.YPixel(wave_point_y, d));
                return p;
            case Signal.MODE_PROFILE:
                break;

        }
    }

    if ( !s.hasX()) {
      return null;
    }

    int idx = s.FindClosestIdx(curr_x, curr_y);
    if (curr_x > s.getCurrentXmax() || curr_x < s.getCurrentXmin() ||
        idx == s.getNumPoints() - 1) {
      y = s.getY(idx);
      x = s.getX(idx);
    }
    else
    {
      if (s.getMarker() != Signal.NONE && !s.getInterpolate() &&
          s.getMode1D() != Signal.MODE_STEP || s.findNaN()) {
        double val;
        boolean increase = s.getX(idx) < s.getX(idx + 1);

        if (increase) {
          val = s.getX(idx) + s.getX(idx + 1) - s.getX(idx) / 2;
        }
        else {
          val = s.getX(idx + 1) + s.getX(idx) - s.getX(idx + 1) / 2;

          //Patch to elaborate strange RFX signal (roprand bar error signal)
        }
        if (s.getX(idx) == s.getX(idx + 1) && !Double.isNaN(s.getY(idx + 1))) {
          val += curr_x;

        }
        if (curr_x < val) {
          y = s.getY(idx + (increase ? 0 : 1));
          x = s.getX(idx + (increase ? 0 : 1));
        }
        else {
          y = s.getY(idx + (increase ? 1 : 0));
          x = s.getX(idx + (increase ? 1 : 0));
        }
      }
      else {
        x = curr_x;
        try {
          if (s.getMode1D() == Signal.MODE_STEP) {
            y = s.getY(idx);
          }
          else {
            y = s.getY(idx) + (s.getY(idx + 1) - s.getY(idx)) * (x - s.getX(idx)) /
                (s.getX(idx + 1) - s.getX(idx));
          }
        }
        catch (ArrayIndexOutOfBoundsException e) {
          //System.out.println("Exception on " + getName() + " " + " " + idx);
        }
      }
    }
    /* No more float X
    else
    {
     if (s.getMarker() != Signal.NONE && !s.getInterpolate() &&
         s.getMode1D() != Signal.MODE_STEP || s.findNaN()) {
       double val;
       boolean increase = s.getX(idx) < s.getX(idx + 1);

       if (increase) {
         val = s.getX(idx) + (s.getX(idx + 1) - s.getX(idx)) / 2;
       }
       else {
         val = s.getX(idx + 1) + (s.getX(idx) - s.getX(idx + 1)) / 2;

         //Patch to elaborate strange RFX signal (roprand bar error signal)
       }
       if (s.getX(idx) == s.getX(idx + 1) && !Double.isNaN(s.getY(idx + 1))) {
         val += curr_x;

       }
       if (curr_x < val) {
         y = s.getY(idx + (increase ? 0 : 1));
         x = s.getX(idx + (increase ? 0 : 1));
       }
       else {
         y = s.getY(idx + (increase ? 1 : 0));
         x = s.getX(idx + (increase ? 1 : 0));
       }
     }
     else {
       x = curr_x;
       try {
         if (s.getMode1D() == Signal.MODE_STEP || s.getX(idx + 1) == s.getX(idx)) {
           y = s.getY(idx);
         }
         else {
           y = s.getY(idx) + ((s.getY(idx + 1) - s.getY(idx)) * (x - s.getX(idx)) /
               (s.getX(idx + 1) - s.getX(idx)));
         }
       }
       catch (ArrayIndexOutOfBoundsException e) {
         //System.out.println("Exception on " + getName() + " " + s.getLength() + " " + idx);
       }
     } 
   } */
    wave_point_x = x;
    wave_point_y = y;
    return new Point(wm.XPixel(x, d), wm.YPixel(y, d));
  }

  protected Point FindPoint(double curr_x, double curr_y, boolean is_first) {
    return FindPoint(waveform_signal, curr_x, curr_y, getWaveSize());
  }

  protected Point FindPoint(double curr_x, double curr_y, Dimension d, boolean is_first) {
    return FindPoint(waveform_signal, curr_x, curr_y, d);
  }


  Rectangle frame_zoom = null;

  protected boolean DrawFrame(Graphics g, Dimension d, int frame_idx) {
    wave_error = null;
    Image img;

    if (mode == MODE_ZOOM && curr_rect != null) {

      Rectangle rect = new Rectangle(start_x, start_y, Math.abs(start_x - end_x),
                                     Math.abs(start_y - end_y));
      frame_idx = frames.GetFrameIdx();
      frames.SetZoomRegion(frame_idx, d, rect);
      curr_rect = null;
    }
    img = frames.GetFrame(frame_idx, d);

    if (img == null) {
      wave_error = " No frame at time " + curr_point;//frames.GetTime(frame_idx);
      return false;
    }

    Dimension dim = frames.getFrameSize(frame_idx, getWaveSize());
//    int type = frames.frame_type[frame_idx];
    int type = frames.getFrameType();

    DrawImage(g, img, dim, type);

    return true;

  }

  protected void DrawImage(Graphics g, Image img, Dimension dim, int type) {

    Rectangle r = frames.GetZoomRect();
    Graphics2D g2 = (Graphics2D)g;
    Dimension imgDim;
    imgDim = new Dimension(((BufferedImage)img).getWidth(),((BufferedImage)img).getHeight()); 
    
    // Turn on antialiasing.
    g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
                        RenderingHints.VALUE_ANTIALIAS_ON);

    if ( type != FrameData.JAI_IMAGE )
    {
        
    int dx1;// the x coordinate of the first corner of the destination rectangle.
    int dy1;// the y coordinate of the first corner of the destination rectangle.
    int dx2;// the x coordinate of the second corner of the destination rectangle.
    int dy2;// the y coordinate of the second corner of the destination rectangle.
    int sx1;// the x coordinate of the first corner of the source rectangle.
    int sy1;// the y coordinate of the first corner of the source rectangle.
    int sx2;// the x coordinate of the second corner of the source rectangle.
    int sy2;// the y coordinate of the second corner of the source rectangle.

   
      dx1 = 1;
      dy1 = 1;
      dx2 = dim.width;
      dy2 = dim.height;

      sx1 = r == null ? 0 : r.x;
      sy1 = r == null ? 0 : r.y;          
      sx2 = r == null ? imgDim.width  : r.x + r.width ;
      sy2 = r == null ? imgDim.height : r.y + r.height ;

    
      if( frames.getVerticalFlip() && frames.getHorizontalFlip() )  
      {
        dx1 = dim.width;
        dy1 = dim.height;
        dx2 = 1;
        dy2 = 1;
      }
      else
      {
          if( frames.getVerticalFlip() )
          {              

                dx1 = dim.width;
                dy1 = 1;
                dx2 = 1;
                dy2 = dim.height;
              
          }
          else
          {
              if( frames.getHorizontalFlip() )
              {
                dx1 = 1;
                dy1 = dim.height;
                dx2 = dim.width;
                dy2 = 1;                
              }
          }
      }
          
      //g2.drawImage( (Image) img, 1, 1, dim.width, dim.height, this);
      g2.drawImage( (Image) img, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, this);
/*        
      if (r == null) {
        g2.drawImage( (Image) img, 1, 1, dim.width, dim.height, this);
      }
      else {
        g2.drawImage( (Image) img,
                    1,
                    1,
                    dim.width,
                    dim.height,
                    r.x,
                    r.y,
                    r.x + r.width,
                    r.y + r.height,
                    this);
 
      }
 */
    }
    else 
    {
      g2.clearRect(0, 0, dim.width, dim.height);
      g2.drawRenderedImage( (RenderedImage) img,
                             new AffineTransform(1f, 0f, 0f, 1f, 0F, 0F));
    }
  }


  protected void drawSignalContour(Signal s, Graphics g, Dimension d)
  {
    Vector cs = s.getContourSignals();
    Vector ls = s.getContourLevelValues();
    int numLevel = cs.size();
    Vector cOnLevel;
    float level;

    for (int l = 0; l < numLevel; l++)
    {
      cOnLevel = (Vector) cs.elementAt(l);
      level = ( (Float) ls.elementAt(l)).floatValue();
      float z[] = s.getZ();
      float zMin, zMax;
      zMin = zMax = z[0];
      for(int i = 0; i < z.length; i++)
      {
          if(z[i] < zMin) 
              zMin = z[i];
          if(z[i] > zMax) 
              zMax = z[i];
      }
      g.setColor(colorMap.getColor(level, zMin, zMax));
      drawContourLevel(cOnLevel, g, d);
    }
  }

  public void drawContourLevel(Vector cOnLevel, Graphics g, Dimension d)
  {
    Vector c;
    Point2D.Float p;
    wm.ComputeFactors(d);
    for (int i = 0; i < cOnLevel.size(); i++)
    {
      c = (Vector) cOnLevel.elementAt(i);
      int cx[] = new int[c.size()];
      int cy[] = new int[c.size()];
      for (int j = 0; j < c.size(); j++)
      {
        p = (Point2D.Float) c.elementAt(j);
        cx[j] = wm.XPixel(p.x);
        cy[j] = wm.YPixel(p.y);
      }
      g.drawPolyline(cx, cy, c.size());
    }
  }

  protected void drawSignal(Graphics g)
  {
    drawSignal(g, getWaveSize(), NO_PRINT);
  }

  protected void drawSignal(Graphics g, Dimension d, int print_mode) {
    drawSignal(waveform_signal, g, d);
  }


  protected void drawSignal(Signal s, Graphics g, Dimension d)
  {

    if (s.getType() == Signal.TYPE_2D)
    {
        switch (s.getMode2D())
        {
            case Signal.MODE_IMAGE:
                if (! (mode == MODE_PAN && dragging))
                {
                    Image img = this.createImage(d.width, d.height);
                    wm.ToImage(s, img, d, colorMap);
                    g.drawImage(img, 0, 0, d.width, d.height, this);
                }
                return;
            case Signal.MODE_CONTOUR:
                if (! (mode == MODE_PAN && dragging))
                {
                    drawSignalContour(s, g, d);
                }
                return;
        }

    }
    drawWave(s, g, d);
    drawMarkers(s, g, d);
    drawError(s, g, d);
  }

  void drawWave(Signal s, Graphics g, Dimension d)
  {
    Vector segments = wm.ToPolygons(s, d, appendDrawMode);
    Polygon curr_polygon;

    if (s.getColor() != null)
    {
      g.setColor(s.getColor());
    }
    else
    {
      g.setColor(colors[s.getColorIdx() % colors.length]);

    }
    for (int k = 0; k < segments.size(); k++)
    {
      curr_polygon = (Polygon) segments.elementAt(k);
      if (s.getInterpolate())
      {
        g.drawPolyline(curr_polygon.xpoints, curr_polygon.ypoints,
                       curr_polygon.npoints);
      }
    }
  }

  protected void drawMarkers(Signal s, Graphics g, Dimension d)
  {
    if (s.getMarker() != Signal.NONE)
    {
      Vector segments = wm.ToPolygons(s, d, appendDrawMode);
      drawMarkers(g, segments, s.getMarker(), s.getMarkerStep(), s.getMode1D());
    }
  }

  public void setFixedLimits(float xmin, float xmax, float ymin, float ymax)
  {
    lx_max = xmax;
    lx_min = xmin;
    ly_max = ymax;
    ly_min = ymin;
    setFixedLimits();
  }


    public void setFixedLimits() {
        try {
            waveform_signal.setXLimits(lx_min, lx_max, Signal.SIMPLE);
        }catch(Exception exc)
        {
            System.out.println(exc);
        }
        setYlimits(ly_min, ly_max);
    change_limits = true;
  }

  public void setXlimits(double xmin, double xmax) {
    if (waveform_signal == null) {
      return;
    }
    try {
        waveform_signal.setXLimits(xmin, xmax, Signal.SIMPLE);
        }catch(Exception exc)
        {
            System.out.println(exc);
        }
  }

  public void setYlimits(float ymin, float ymax) {
    if (waveform_signal == null) {
      return;
    }
    waveform_signal.setYlimits(ymin, ymax);
  }

  protected double MaxXSignal() {
    if (waveform_signal == null) {
      return 1.;
    }
    return waveform_signal.getXmax();
  }

  protected double MaxYSignal() {
    if (waveform_signal == null) {
      return 1.;
    }
    if (waveform_signal.getYmax() <= waveform_signal.getYmin()) {
      return waveform_signal.getYmax() + 1E-3 + Math.abs(waveform_signal.getYmax());
    }
    return waveform_signal.getYmax();
  }

  protected double MinXSignal() {
    if (waveform_signal == null) {
       return 0.;
    }
    return waveform_signal.getXmin();
  }

  public boolean isWaveformVisible() {
    Dimension d = getWaveSize();
    return! (d.width <= 0 || d.height <= 0);
  }

  protected double MinYSignal() {
    if (waveform_signal == null) {
      return 0.;
    }
    if (waveform_signal.getYmax() <= waveform_signal.getYmin()) {
      return waveform_signal.getYmin() - 1E-3 - Math.abs(waveform_signal.getYmax());
    }
    return waveform_signal.getYmin();
  }

  protected void HandleCopy() {}

  protected void HandlePaste() {}



  protected void drawMarkers(Graphics g, Point pnt[], int n_pnt, int marker,
                             int step, int mode) {
    for (int i = 0; i < n_pnt; i += step) {
      if (mode == Signal.MODE_STEP && i % 2 == 1) {
        continue;
      }

      switch (marker) {
        case Signal.CIRCLE:
          g.drawOval(pnt[i].x - marker_width / 2,
                     pnt[i].y - marker_width / 2, marker_width, marker_width);
          break;
        case Signal.SQUARE:
          g.drawRect(pnt[i].x - marker_width / 2,
                     pnt[i].y - marker_width / 2, marker_width, marker_width);
          break;
        case Signal.TRIANGLE:
          g.drawLine(pnt[i].x - marker_width / 2,
                     pnt[i].y + marker_width / 2, pnt[i].x,
                     pnt[i].y - marker_width / 2);
          g.drawLine(pnt[i].x + marker_width / 2,
                     pnt[i].y + marker_width / 2, pnt[i].x,
                     pnt[i].y - marker_width / 2);
          g.drawLine(pnt[i].x - marker_width / 2,
                     pnt[i].y + marker_width / 2, pnt[i].x + marker_width / 2,
                     pnt[i].y + marker_width / 2);
          break;
        case Signal.CROSS:
          g.drawLine(pnt[i].x, pnt[i].y - marker_width / 2,
                     pnt[i].x, pnt[i].y + marker_width / 2);
          g.drawLine(pnt[i].x - marker_width / 2, pnt[i].y,
                     pnt[i].x + marker_width / 2, pnt[i].y);
          break;
        case Signal.POINT:
          g.fillRect(pnt[i].x - 1, pnt[i].y - 1, 3, 3);
          break;
      }
    }

  }


protected void drawMarkers(Graphics g, Vector segments, int marker, int step,
                           int mode)
{
  Polygon currPolygon;
  int pntX[];
  int pntY[];

  for (int k = 0; k < segments.size(); k++)
  {
    currPolygon = (Polygon) segments.elementAt(k);
    pntX = currPolygon.xpoints;
    pntY = currPolygon.ypoints;
    for (int i = 0; i < currPolygon.npoints; i += step)
    {
      if (mode == Signal.MODE_STEP && i % 2 == 1)
      {
        continue;
      }

      switch (marker)
      {
        case Signal.CIRCLE:
          g.drawOval(pntX[i] - marker_width / 2,
                     pntY[i] - marker_width / 2, marker_width, marker_width);
          break;
        case Signal.SQUARE:
          g.drawRect(pntX[i] - marker_width / 2,
                     pntY[i] - marker_width / 2, marker_width, marker_width);
          break;
        case Signal.TRIANGLE:
          g.drawLine(pntX[i] - marker_width / 2,
                     pntY[i] + marker_width / 2, pntX[i],
                     pntY[i] - marker_width / 2);
          g.drawLine(pntX[i] + marker_width / 2,
                     pntY[i] + marker_width / 2, pntX[i],
                     pntY[i] - marker_width / 2);
          g.drawLine(pntX[i] - marker_width / 2,
                     pntY[i] + marker_width / 2, pntX[i] + marker_width / 2,
                     pntY[i] + marker_width / 2);
          break;
        case Signal.CROSS:
          g.drawLine(pntX[i], pntY[i] - marker_width / 2,
                     pntX[i], pntY[i] + marker_width / 2);
          g.drawLine(pntX[i] - marker_width / 2, pntY[i],
                     pntX[i] + marker_width / 2, pntY[i]);
          break;
        case Signal.POINT:
          g.fillRect(pntX[i] - 1, pntY[i] - 1, 3, 3);
          break;
      }
    }
  }
}



    void drawError(Signal sig, Graphics g, Dimension d)
    {
      if (!sig.hasError())
      {
        return;
      }

      int up, low, x, y;
      float up_error[] = sig.getUpError();
      float low_error[] = sig.getLowError();

      for (int i = 0; i < sig.getNumPoints(); i++)
      {
        try {
            up = wm.YPixel(up_error[i] + sig.getY(i), d);
            if (!sig.hasAsymError())
            {
              low = wm.YPixel(sig.getY(i) - up_error[i], d);
            }
            else
            {
              low = wm.YPixel(sig.getY(i) - low_error[i], d);
            }
            x = wm.XPixel(sig.getX(i), d);

            g.drawLine(x, up, x, low);
            g.drawLine(x - 2, up, x + 2, up);
            g.drawLine(x - 2, low, x + 2, low);
        }catch(Exception exc){}
      }
    }

  public void UpdatePoint(double curr_x)
  {
      UpdatePoint(curr_x, Double.NaN);
  }

  public synchronized void UpdatePoint(double curr_x, double curr_y) {
    Dimension d = getWaveSize();
    Insets i = getInsets();

    if (curr_x == curr_point && !dragging) {
      return;
    }
    curr_point = curr_x;
    curr_point_y = curr_y;

    if (!is_image) {
       if (mode != MODE_POINT || waveform_signal == null) {
        return;
      }

          if(waveform_signal.getType() == Signal.TYPE_2D &&
            waveform_signal.getMode2D() ==  Signal.MODE_YZ)
          {
              waveform_signal.showYZ((float)curr_x);
              not_drawn = true;
          }
       paintImmediately(0, 0, d.width, d.height);
    }
    else {
      if (frames != null && !is_playing) {
        frame = frames.GetFrameIdxAtTime( (float) curr_x);
        not_drawn = true;
        repaint();
        if (send_profile) {
          sendProfileEvent();
        }
      }
    }
  }

  protected void NotifyZoom(double start_xs,
                            double end_xs,
                            double start_ys,
                            double end_ys,
                            int timestamp) {
  }

  protected void ReportLimits(ZoomRegion r, boolean add_undo) {

    if (add_undo) {
      ZoomRegion r_prev = new ZoomRegion(waveform_signal.getXmin(),
                                         waveform_signal.getXmax(),
                                         waveform_signal.getYmax(),
                                         waveform_signal.getYmin());

      undo_zoom.addElement(r_prev);
      //waveform_signal.freeze();
    }
    else {
      undo_zoom.removeElement(r);
      if(undo_zoom.size() == 0)
          unfreeze();
    }
    //GABRIELE AUGUST 2014
    setXlimits(r.start_xs, r.end_xs);
    waveform_signal.setXLimits( r.start_xs, r.end_xs, Signal.SIMPLE);
    waveform_signal.setYmin( r.end_ys, Signal.SIMPLE);
    waveform_signal.setYmax( r.start_ys, Signal.SIMPLE);
    change_limits = true;
    if(add_undo)
        freeze();

  }

  public void Autoscale() {
    if (is_image && frames != null && frames.getNumFrame() != 0) {
      frames.Resize();
    }
    else {
      if (waveform_signal == null) {
        return;
      }
      waveform_signal.Autoscale();
      unfreeze();
    }
    ReportChanges();
  }

  public void AutoscaleY() {
    if (waveform_signal == null) {
      return;
    }
    waveform_signal.AutoscaleY();
    ReportChanges();
  }

  public void SetScale(Waveform w) {
    double ymin, ymax, xmin, xmax;
    if (waveform_signal == null) {
      return;
    }
    waveform_signal.setXLimits(w.waveform_signal.getXmin(), w.waveform_signal.getXmax(), Signal.SIMPLE);
    waveform_signal.setYmin(w.waveform_signal.getYmin(), Signal.SIMPLE);
    waveform_signal.setYmax(w.waveform_signal.getYmax(), Signal.SIMPLE);
    ReportChanges();
  }

  public void SetXScale(Waveform w) {
    if (waveform_signal == null)
      return;

    waveform_signal.setXLimits(w.waveform_signal.getXmin(), w.waveform_signal.getXmax(), Signal.SIMPLE);
    ReportChanges();
  }

  public void SetYScale(Waveform w) {
    if (waveform_signal == null)
      return;

    waveform_signal.setYmin(w.waveform_signal.getYmin(), Signal.SIMPLE);
    waveform_signal.setYmax(w.waveform_signal.getYmax(), Signal.SIMPLE);
    ReportChanges();
  }

  public void SetXScaleAutoY(Waveform w) {
    if (waveform_signal == null)
      return;
 
    waveform_signal.setXLimits(w.waveform_signal.getXmin(), w.waveform_signal.getXmax(), Signal.SIMPLE);
    waveform_signal.AutoscaleY();
    ReportChanges();
  }

  public void ResetScales() {
    if (waveform_signal == null)
      return;

    waveform_signal.ResetScales();
    undo_zoom.clear();
    unfreeze();
    ReportChanges();
  }

  protected void Resize(int x, int y, boolean enlarge) {
    Dimension d = getWaveSize();
    double curr_x = wm.XValue(x, d),
        curr_y = wm.YValue(y, d),
        prev_xmax = wm.XMax(),
        prev_xmin = wm.XMin(),
        prev_ymax = wm.YMax(),
        prev_ymin = wm.YMin(),
        new_xmax, new_xmin, new_ymax, new_ymin;

    if (enlarge) {
      new_xmax = curr_x + (prev_xmax - curr_x) / 2.;
      new_xmin = curr_x - (curr_x - prev_xmin) / 2.;
      new_ymax = curr_y + (prev_ymax - curr_y) / 2.;
      new_ymin = curr_y - (curr_y - prev_ymin) / 2.;
    }
    else {
      new_xmax = curr_x + (prev_xmax - curr_x) * 2.;
      new_xmin = curr_x - (curr_x - prev_xmin) * 2.;
      new_ymax = curr_y + (prev_ymax - curr_y) * 2.;
      new_ymin = curr_y - (curr_y - prev_ymin) * 2.;
    }
    ZoomRegion r = new ZoomRegion(new_xmin, new_xmax, new_ymax, new_ymin);
    ReportLimits(r, true);

    not_drawn = true;
   }

  public void undoZoom() {
    if (undo_zoom.size() == 0) {
      return;
    }

    ZoomRegion r = undo_zoom.lastElement();
    {
      ReportLimits(r, false);
      not_drawn = true;
      repaint();
    }
  }

    public void performZoom() {
        if(wm == null)
            return;

        Dimension d = getWaveSize();
        double start_xs = wm.XValue(start_x, d);
        double end_xs = wm.XValue(end_x, d);
        double start_ys = wm.YValue(start_y, d);
        double end_ys = wm.YValue(end_y, d);

        ZoomRegion r = new ZoomRegion(start_xs, end_xs, start_ys, end_ys);
        ReportLimits(r, true);
    }

  public void SetTitle(String title) {
    this.title = title;
  }

  public String GetTitle() {
    return title;
  }

  public void SetXLabel(String x_label) {
    this.x_label = x_label;
  }

  public String GetXLabel() {
    return x_label;
  }

  public void SetYLabel(String y_label) {
    this.y_label = y_label;
  }

  public String GetYLabel() {
    return y_label;
  }

  void ReportChanges() {
 /*   if (undo_zoom.size() != 0) {
      undo_zoom.removeAllElements();
    }
    */
    wm = null;
    not_drawn = true;
    if (mode == MODE_POINT && is_image && frames != null) {
      Point p = frames.getFramePoint(getWaveSize());
      end_x = p.x;
      end_y = p.y;
    }
    repaint();
    if (mode == MODE_POINT) {
      sendUpdateEvent();
      if (is_image && send_profile) {
        sendProfileEvent();
      }
    }

    if (waveform_signal != null) {
      NotifyZoom(waveform_signal.getXmin(),
                 waveform_signal.getXmax(),
                 waveform_signal.getYmin(),
                 waveform_signal.getYmax(),
                 update_timestamp);
    }

  }

  public void SetCopySelected(boolean selec) {
    copy_selected = selec;
    not_drawn = true;
    repaint();
  }

  public boolean IsCopySelected() {
    return copy_selected;
  }

  public boolean IsSelected() {
    return is_select;
  }

  public boolean ShowMeasure() {
    return show_measure;
  };

  public boolean IsImage() {
    return is_image;
  };

  public void SetShowMeasure(boolean state) {
    show_measure = state;
  }

  public void SetGridSteps(int _grid_step_x, int _grid_step_y) {
    grid_step_x = _grid_step_x;
    if (grid_step_x <= 1) {
      grid_step_x = 2;
    }
    grid_step_y = _grid_step_y;
    if (grid_step_y <= 1) {
      grid_step_y = 2;
    }
    wm = null;
    grid = null;
    not_drawn = true;
  }

  public int GetGridStepX() {
    return grid_step_x;
  }

  public int GetGridStepY() {
    return grid_step_y;
  }

  public Image GetImage() {
    return off_image;
  }

  public void SetFrames(Frames frames) {
    this.frames = frames;
  }

  public boolean IsReversed() {
    return reversed;
  }

  public void SetReversed(boolean reversed) {
    if (this.is_image && !reversed) {
      return;
    }
    if (this.reversed != reversed) {
      this.reversed = reversed;
      if (grid != null) {
        grid.SetReversed(reversed);
      }
    }
  }

  public void setColorMap(ColorMap cm)
  {
      this.colorMap = cm;
      if(frames != null)
          frames.setColorMap(cm);
      not_drawn = true;
      repaint();
  }

  public void setFrameBitShift(int bitShift, boolean bitClip)
  {
      if(frames != null)
      {
          frames.shiftImagePixel(bitShift, bitClip);
          not_drawn = true;
          repaint();
      }
  }

  public ColorMap getColorMap()
  {
      return frames.getColorMap();
  }

  public void applyColorModel(ColorMap cm)
  {
     if(cm == null) return;
     if(frames != null)
     {
         frames.applyColorModel(cm);
     }
     else
     {
         this.colorMap = cm;
     }
     not_drawn = true;
     repaint();
  }

  public synchronized void addWaveformListener(WaveformListener l) {
    if (l == null) {
      return;
    }
    waveform_listener.addElement(l);
  }

  public synchronized void removeWaveformListener(WaveformListener l) {
    if (l == null) {
      return;
    }
    waveform_listener.removeElement(l);
  }

  protected synchronized void dispatchWaveformEvent(WaveformEvent e) {
    if (e == null || !event_enabled) {
      return;
    }
    if (waveform_listener != null) {
      for (int i = 0; i < waveform_listener.size(); i++) {
        waveform_listener.elementAt(i).processWaveformEvent(e);
      }
    }
  }

  class SymContainer
      extends java.awt.event.ContainerAdapter {
    public void componentAdded(java.awt.event.ContainerEvent event) {
      Object object = event.getSource();
      if (object == Waveform.this) {
        Waveform_ComponentAdded(event);
      }
    }
  }

  void Waveform_ComponentAdded(java.awt.event.ContainerEvent event) {
    // to do: code goes here.
  }
  
  public  void signalUpdated(boolean changeLimits)
  {
      change_limits = changeLimits;
      not_drawn = true;
      repaint();
  }
  
  void freeze()
  {
      waveform_signal.freeze();
  }

    void unfreeze()
    {
        waveform_signal.unfreeze();
    }
}

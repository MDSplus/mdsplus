/* $Id$ */
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import java.security.*;

public class WaveDisplay
    extends JApplet
    implements WaveformListener {
    MultiWaveform w;
  WavePopup wave_popup;
  JTextField shot_txt;
  ProfileDialog profDialog;

  public WaveDisplay() {
    DataAccessURL.addProtocol(new RdaAccess());
    DataAccessURL.addProtocol(new MdsAccess());
    DataAccessURL.addProtocol(new TwuAccess());

    JPanel panel = new JPanel();
    w = new MultiWaveform();
    w.setWaveInterface(new WaveInterface(w));
    w.addWaveformListener(this);

    setBackground(Color.lightGray);
    profDialog = new ProfileDialog(null, null);
    wave_popup = new WavePopup(new SetupWaveformParams(null, "Waveform Params"),
                               profDialog);
    getContentPane().add(wave_popup);
    panel.addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
        Waveform w = (Waveform) e.getSource();
        if (wave_popup != null && w.GetMode() != Waveform.MODE_COPY) {
          wave_popup.Show(w, e.getX(), e.getY(), 0, 0);
        }
      }
    });

    JRadioButton point = new JRadioButton("Point", false);
    point.addItemListener(new ItemListener() {
      public void itemStateChanged(ItemEvent e) {
        w.SetMode(Waveform.MODE_POINT);
      }
    });

    JRadioButton zoom = new JRadioButton("Zoom", true);
    zoom.addItemListener(new ItemListener() {
      public void itemStateChanged(ItemEvent e) {
        w.SetMode(Waveform.MODE_ZOOM);
      }
    });

    JRadioButton pan = new JRadioButton("Pan", false);
    pan.addItemListener(new ItemListener() {
      public void itemStateChanged(ItemEvent e) {
        w.SetMode(Waveform.MODE_PAN);
      }
    });

    ButtonGroup pointer_mode = new ButtonGroup();
    pointer_mode.add(point);
    pointer_mode.add(zoom);
    pointer_mode.add(pan);

    JPanel panel1 = new JPanel();
    panel1.setLayout(new FlowLayout(FlowLayout.LEFT, 1, 3));
    panel1.add(point);
    panel1.add(zoom);
    panel1.add(pan);

    w.SetGridMode(Grid.IS_DOTTED, true, true);
    panel.setLayout(new BorderLayout());
    panel.add(w, BorderLayout.CENTER);
    getContentPane().setLayout(new BorderLayout());
    getContentPane().add(panel, BorderLayout.CENTER);
    getContentPane().add(panel1, BorderLayout.SOUTH);
  }

  public void init() {
  }

  public void start() {
    getSignalsParameter();
  }

  public void stop() {
    DataAccessURL.close();
  }

  public void destroy() {
  }

  public void processWaveformEvent(WaveformEvent e) {
    String s = null;
    int event_id = e.getID();

    WaveformEvent we = (WaveformEvent) e;
    MultiWaveform w = (MultiWaveform) we.getSource();
    WaveInterface wi = w.getWaveInterface();
    int we_id = we.getID();

    switch (we_id) {
      case WaveformEvent.MEASURE_UPDATE :
      case WaveformEvent.POINT_UPDATE:
      case WaveformEvent.POINT_IMAGE_UPDATE:
        s = we.toString();
        if (wi.shots != null) {
          s = (s +
               " Expr : " + w.getSignalName(we.signal_idx) +
               " Shot = " + wi.shots[we.signal_idx]);
        }
        else {
          s = (s +
               " Expr : " + w.getSignalName(we.signal_idx));
        }
        showStatus(s);
        break;
      case WaveformEvent.STATUS_INFO:
        s = we.status_info;
        showStatus(s);
        break;
      default:
        showStatus(we.toString());
                
    }

  }

  protected boolean translateToBoolean(String value) {
    if (value.toUpperCase().equals("TRUE")) {
      return true;
    }
    else {
      return false;
    }
  }
  
//  public void addSignals( String url, boolean isImage )
   public void addSignals()
  {
      String url = "mds://150.178.3.242/a/14000/\\emra_it";
      boolean isImage = false;
              
    try
    {
        System.out.println("Add signal : " + url);
        DataAccess da = DataAccessURL.getDataAccess(url);
        if (da != null) 
        {
                  
//       da.setPassword(autentication);
          da.setProvider(url);

          WaveInterface wi = w.getWaveInterface();
          wi.SetDataProvider(da.getDataProvider());
          wi.experiment = da.getExperiment();

          System.out.println("Signal Name : " + da.getSignalName() );

          if( !isImage )
          {
            wi.AddSignal(da.getSignalName());
 
            System.out.println("Shots : "+da.getShot());
            wi.setShotArray(da.getShot());
          
            if( wi.StartEvaluate() == 1 )
                wi.EvaluateOthers();

            if (  wi.signals != null && (wi.signals[0]) != null ) 
            {
                w.Update(wi.signals);
            } else {
                JOptionPane.showMessageDialog(this, "Evaluation Error : " + wi.getErrorTitle(false), "alert",
                                          JOptionPane.ERROR_MESSAGE);
            }           
        } else {
              Frames f = new Frames();
              DataAccessURL.getImages(url, f);
              if (f != null) {
                w.UpdateImage(f);
              }
            }
          }
        }
        catch (Exception e) {
          if (e instanceof AccessControlException) {
            JOptionPane.showMessageDialog(this,
                                          e.toString() + "\n url " + url +
                                          "\nUse policytool.exe in  JDK or JRE installation directory to add socket access permission\n",
                                          "alert",
                                          JOptionPane.ERROR_MESSAGE);
          }
          else {
            e.printStackTrace();  
            JOptionPane.showMessageDialog(this,
                                          e.toString(),
                                          "alert",
                                          JOptionPane.ERROR_MESSAGE);
          }
         }     
      }

   public void resetSignal()
   {
          WaveInterface wi = w.getWaveInterface();
          
          if( wi.signals != null)
          {
            wi.Erase();
          }
          w.Erase();
   }
   
   public void addSignal(String signalParams, String globalAutentication, boolean asyncUpdate, boolean image)
   {
        String url = null, color, marker, name, title;
        String param;
        String signalAutentication, autentication;
        
        if( signalParams == null ) return ;

    try {
      if ( !image  ) {

        signalAutentication = getParameterValue(signalParams, "autentication");
        if (signalAutentication != null) {
          autentication = signalAutentication;
        }
        else {
          autentication = globalAutentication;
        }
         
        
        url = getParameterValue(signalParams, "url");

        DataAccess da = DataAccessURL.getDataAccess(url);
        if (da != null) 
        {
                    
          da.setPassword(autentication);
          da.setProvider(url);

          WaveInterface wi = w.getWaveInterface();
          
          if( wi.signals != null)
            wi.Erase();
          
          wi.SetDataProvider(da.getDataProvider());
          wi.experiment = da.getExperiment();

          System.out.println("Signal Name : " + da.getSignalName() );
          System.out.println("Shots : "+da.getShot());

          wi.AddSignal(da.getSignalName());
 
          wi.full_flag = !asyncUpdate; 
          wi.setShotArray(da.getShot());
          
          if( wi.StartEvaluate() == 1 )
            wi.EvaluateOthers();
          
          Signal s;
          if (wi.signals != null && (s = wi.signals[0]) != null) 
          {
            color = getParameterValue(signalParams, "color");
            if (color != null) {
              s.setColor(new Color(Integer.decode(color).intValue()));

            }
            marker = getParameterValue(signalParams, "marker");
            s.setMarker(marker);

            name = getParameterValue(signalParams, "name");
            if (name != null) {
              s.setName(name);
            }
            else {
              s.setName(wi.in_y[0]);
            }
            title = getParameterValue(signalParams, "title");
            if (title != null) {
              w.SetTitle(title);
            }
          }
          else
          {
            JOptionPane.showMessageDialog(this, "Evaluation Error : " + wi.getErrorTitle(false), "alert",
                                          JOptionPane.ERROR_MESSAGE);
             
          }          
          w.Update(wi.signals);
        }
      }
      else {
        String aspect_ratio, horizontal_flip, vertical_flip;
        
          url = getParameterValue(signalParams, "url");
          Frames f = new Frames();

          horizontal_flip = getParameterValue(signalParams, "h_flip");
          if (horizontal_flip != null &&
              horizontal_flip.toLowerCase().equals("true")) {
            f.setHorizontalFlip(true);

          }
          vertical_flip = getParameterValue(signalParams, "v_flip");
          if (vertical_flip != null &&
              vertical_flip.toLowerCase().equals("true")) {
            f.setVerticalFlip(true);

          }

          DataAccessURL.getImages(url, f);
          if (f != null) {
            name = getParameterValue(signalParams, "name");
            if (name != null) {
              f.setName(name);

            }
            aspect_ratio = getParameterValue(signalParams, "ratio");
            if (aspect_ratio != null &&
                aspect_ratio.toLowerCase().equals("false")) {
              f.setAspectRatio(false);

            
            w.UpdateImage(f);
          }
        }
      }
    }
    catch (Exception e) {
      if (e instanceof AccessControlException) {
        JOptionPane.showMessageDialog(this,
                                      e.toString() + "\n url " + url +
                                      "\nUse policytool.exe in  JDK or JRE installation directory to add socket access permission\n",
                                      "alert",
                                      JOptionPane.ERROR_MESSAGE);
      }
      else {
        e.printStackTrace();  
        JOptionPane.showMessageDialog(this,
                                      e.toString(),
                                      "alert",
                                      JOptionPane.ERROR_MESSAGE);
      }
    }                
   }
   
   
  private void getSignalsParameter() {
    String sig_param;
    String param;
    String global_autentication;
    boolean async_update = false;
    boolean image = false;

    global_autentication = getParameter("AUTENTICATION");

    param = getParameter("ASYNC_UPDATE");
    if (param != null) {
      async_update = this.translateToBoolean(param);
    }
        
    if ( (sig_param = getParameter("SIGNAL")) == null)
    {
        sig_param = getParameter("IMAGE");
        image = true;
    }   
    
    addSignal(sig_param, global_autentication, async_update, image);

  }

  protected String getParameterValue(String context, String param) 
  {
    boolean found = false;
    String value = null;

    StringTokenizer st = new StringTokenizer(context);
    while (st.hasMoreTokens()) {
      value = st.nextToken();
      if (value.equals(param)) {
        if (st.nextToken().equals("=")) {
          value = st.nextToken();
          break;
        }
        else {
          return null;
        }
      }
      value = null;
    }
    return value;
  }

  public void addProtocol(DataAccess dataAccess) {
    DataAccessURL.addProtocol(dataAccess);
  }

  public void print_xxx(Graphics g) {
    Dimension dim = new Dimension();
    /*
             Dimension applet_size = getSize();
             if(print_scaling != 100)
             {
     System.out.println("Proporzionale " + this.getBounds());
     float ratio = (float)applet_size.width/applet_size.height;
     dim.width = (int)(applet_size.width/100.* print_scaling);
     dim.height = (int)(ratio * dim.width);
             } else {
     dim.width = 530;
     dim.height = 816;
             }
     */
    dim.width = 530;
    dim.height = 816;
    w.paint(g, dim, Waveform.PRINT);
  }
}

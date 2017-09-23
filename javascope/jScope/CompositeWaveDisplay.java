package jScope;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.Window;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.print.PageFormat;
import java.awt.print.PrinterJob;
import java.io.IOException;
import java.security.AccessControlException;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.StringTokenizer;
import java.util.Vector;

import javax.swing.AbstractButton;
import javax.swing.ButtonGroup;
import javax.swing.JApplet;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JRootPane;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;



public class CompositeWaveDisplay extends JApplet implements WaveContainerListener
{

    public class myQueue
    {
        Vector<Object> data = new Vector<>();

        synchronized public void add(Object o)
        {
            data.add(o);
            this.notify();
        }

        synchronized public Object[] dequeue() throws InterruptedException
        {
            Object objects[] = null;
            while (data.size() == 0 )
              this.wait();
            objects = data.toArray();
            Object o;
            for(int i = objects.length - 1; i > 0; i--)
            {
                o = objects[i];
                objects[i] = objects[objects.length - i];
                objects[objects.length - i] = o;
            }
            data.clear();
            return objects;
        }
    }

    static public final int CMND_STOP          = -1;
    static public final int CMND_CLEAR         = 0;
    static public final int CMND_ADD           = 1;

    private WaveformContainer wave_container;
    private boolean           automatic_color = false;
    private boolean           isApplet = true;
    private JLabel            point_pos;

    static  private JFrame    f = null;
    PrinterJob                prnJob;
    PageFormat                pf;
    Hashtable<String, Signal> signals1DHash = new Hashtable<>();
    Hashtable<String, Signal> signals2DHash = new Hashtable<>();
    Vector<Signal>            signals1DVector = new Vector<>();
    Vector<Signal>            signals2DVector = new Vector<>();

    myQueue updSignalDataQeue = new  myQueue();
    AppendThread appendThread;
    ButtonGroup  pointer_mode;

    private class UpdSignalData
    {
        String name = null;
        int    numPointsPerSignal = 0;
        int    operation;
        int    type;
        float  x[]     = null;
        float  y[]     = null;
        float  times[] = null;

        UpdSignalData(int numPointsPerSignal, int operation, int type, float x[], float y[])
        {
            this.numPointsPerSignal = numPointsPerSignal;
            this.operation = operation;
            this.type = type;
            this.x = x;
            this.y = y;
//            System.out.println("numPointsPerSignal " + numPointsPerSignal);
        }

        UpdSignalData(int numPointsPerSignal, int operation, int type, float x[], float y[], float times[])
        {
            this.numPointsPerSignal = numPointsPerSignal;
            this.operation = operation;
            this.type = type;
            this.x = x;
            this.y = y;
            this.times = times;
//            System.out.println("numPointsPerSignal " + numPointsPerSignal);
        }

        UpdSignalData(String name, int operation, int type, float x[], float y[])
        {
            this.name = name;
            this.operation = operation;
            this.type = type;
            this.x = x;
            this.y = y;
//            System.out.println("numPoints " + x.length);
        }

        UpdSignalData(String name, int operation, int type, float x[], float y[], float times[])
        {
            this.name = name;
            this.operation = operation;
            this.type = type;
            this.x = x;
            this.y = y;
            this.times = times;
//            System.out.println("numPoints " + x.length);
        }


        public String toString()
        {
            return "Num point per signal "+numPointsPerSignal+" Operation "+operation+" Type "+type;
        }

    }


    public class AppendThread extends Thread
    {
        long sleepTime = 100;
        boolean suspend = false;

        AppendThread(long sleepTime)
        {
            this.sleepTime = sleepTime;
        }

        synchronized public void suspendThread()
        {
            suspend = true;
        }

        synchronized public void resumeThread()
        {
            suspend = false;
            this.notify();
        }

        public void run()
        {
            UpdSignalData usd;

            while(true)
            {
                try {
                    Object obj[] = updSignalDataQeue.dequeue();
                    for (int j = 0; j < obj.length; j++) {
                        usd = (UpdSignalData) obj[j];
                        if (usd == null)break;
                        if(usd.operation == CompositeWaveDisplay.CMND_STOP)
                            return;
                        processPacket(usd);
                    }

                    for (Signal s : signals2DVector)
                        s.setMode2D(Signal.MODE_PROFILE);

                    wave_container.appendUpdateWaveforms();

                    synchronized (this) {
                        if (suspend) {
                            wave_container.updateWaveforms();
                            wait();
                        }
                    }
                    Thread.sleep(500);

                } catch (Exception exc) {
                    System.out.println(exc);
                }
            }
        }

/*
        private UpdSignalData mergeMessages(UpdSignalData[] usds)
        {
            UpdSignalData out[] = new UpdSignalData[2 + signals1DVector.size() +  signals2DVector.size()];
            for(int i = 0; i < usds.length; i++)
            {

            }
            return null;
        }


        private UpdSignalData mergeMessage(UpdSignalData upd1, UpdSignalData upd2)
        {
            if(upd1.name != upd2.name || upd1.type != upd2.type) return null;
            upd1.numPointsPerSignal = upd1.numPointsPerSignal + upd2.numPointsPerSignal;
            if(upd1.type == Signal.TYPE_1D)
            {
               if(upd1.operation == CompositeWaveDisplay.CMND_CLEAR) return upd1 ;
               if(upd2.operation == CompositeWaveDisplay.CMND_CLEAR) return upd2 ;

                int numSignal1 = upd1.x.length / upd1.numPointsPerSignal;
                int numSignal2 = upd2.x.length / upd2.numPointsPerSignal;

                if(numSignal1 != numSignal2) return null;

                float y[] = float[upd1.length]

            }
        }

*/

        private void processPacket( UpdSignalData usd )
        {
            switch(usd.type)
            {
               case Signal.TYPE_1D:
                   if(usd.name != null)
                       processsSignal1DPacket( usd );
                   else
                       processsSignals1DPacket( usd );
               break;
               case Signal.TYPE_2D:
                   if(usd.name != null)
                       processsSignal2DPacket( usd );
                   else
                       processsSignals2DPacket( usd );
               break;

            }
        }

        private void processsSignal1DPacket( UpdSignalData usd )
        {
            Signal s = signals1DHash.get(usd.name);
            if(s != null)
                appendToSignal(s, usd.operation, usd.x, usd.y);
        }

        private void processsSignals1DPacket( UpdSignalData usd )
        {
            float x[];
            float y[];
            y = new float[usd.numPointsPerSignal];

            for (int i = 0; i < signals1DVector.size(); i++)
            {
                Signal s = signals1DVector.elementAt(i);
                System.arraycopy(usd.y, i * usd.numPointsPerSignal,
                                 y, 0, usd.numPointsPerSignal);
                if (usd.x != null && usd.x.length > 1)
                {
                    x = new float[usd.numPointsPerSignal];
                    System.arraycopy(usd.x, i * usd.numPointsPerSignal,
                                         x, 0, usd.numPointsPerSignal);
                }
                else
                {
                    x = new float[1];
                    x[0] = usd.x[0];
                }
                appendToSignal(s, usd.operation, x, y);
            }
        }

        private void appendToSignal(Signal s, int operation, float x[], float y[])
        {
            switch (operation)
            {
                case (CompositeWaveDisplay.CMND_CLEAR):
                    s.resetSignalData();
                case (CompositeWaveDisplay.CMND_ADD):
                    s.appendValues(x, y);
                break;
            }
        }

        private void appendToSignal(Signal s, int operation, float x[], float y[], int numPoints[], float time[])
       {
           switch (operation)
           {
               case (CompositeWaveDisplay.CMND_CLEAR):
                   s.resetSignalData();
               case (CompositeWaveDisplay.CMND_ADD):
                   double d[] = new double[x.length];
                   for(int i = 0; i < x.length; i++)
                       d[i] = x[i];
                   s.appendValues(d, y, numPoints, time);
               break;
           }
       }

        private void processsSignals2DPacket( UpdSignalData usd )
        {
            float x[];
            float y[];
            int nPoints[] = new int[1];
            int totPoints = usd.numPointsPerSignal * usd.times.length;
            y = new float[totPoints];
            x = new float[totPoints];

//          System.out.println("N point per sig "+ usd.numPointsPerSignal +" tot point "+ totPoints +" time len "+ usd.times.length);

            for (int i = 0; i < signals2DVector.size(); i++)
            {
                Signal s = signals2DVector.elementAt(i);
                System.arraycopy(usd.y, i * totPoints, y, 0, totPoints);
                System.arraycopy(usd.x, i * totPoints, x, 0, totPoints);
                nPoints[0] = usd.numPointsPerSignal;
                appendToSignal(s, usd.operation, x, y, nPoints, usd.times);
            }
        }

        private void processsSignal2DPacket( UpdSignalData usd )
        {
            Signal s = signals2DHash.get(usd.name);
            if(s != null)
            {
                int nPoints[] = new int[usd.times.length];
                for(int i = 0; i < usd.times.length; i++)
                    nPoints[i] = usd.x.length / usd.times.length;
                appendToSignal(s, usd.operation, usd.x, usd.y, nPoints, usd.times);
            }
        }
    }
    public static CompositeWaveDisplay createWindow(String title)
    {
    	return createWindow(title, false);
    }

    public static CompositeWaveDisplay createWindow(String title, boolean enableLiveUpdate)
    {
       if(title != null)
            f = new JFrame(title);
        else
            f = new JFrame();

        f.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e)
            {
                Object o[] = e.getWindow().getComponents();
                JRootPane root = (JRootPane)o[0];
                o = root.getContentPane().getComponents();
                for(int i = 0; i < o.length; i++)
                {
                    if (o[i] instanceof CompositeWaveDisplay)
                    {
                        ( (CompositeWaveDisplay) o[i]).applicationExit(e.getWindow());
                        break;
                    }
                }
            }
        });

        CompositeWaveDisplay cwd = new CompositeWaveDisplay(false);
        cwd.init();
	if(enableLiveUpdate)
	    cwd.enableLiveUpdate();
        f.getContentPane().add(cwd);
        return cwd;
    }

    public CompositeWaveDisplay()
    {
        super();
    }

    public void applicationExit(Window w)
    {
        try
        {
            this.enqueueUpdateSignal(null, CompositeWaveDisplay.CMND_STOP, null, null);
            this.appendThread.join(1000);
            w.dispose();
        } catch (Exception exc){}
    }


    private CompositeWaveDisplay(boolean isApplet)
    {
        super();
        this.isApplet = isApplet;
    }


    private void setDataAccess()
    {
        DataAccessURL.addProtocol(new MdsAccess());
        DataAccessURL.addProtocol(new TwuAccess());
    }

    public void addProtocol(DataAccess dataAccess)
    {
        DataAccessURL.addProtocol(dataAccess);
    }

    public void setEnabledMode(boolean state)
    {
        Enumeration<AbstractButton> e = pointer_mode.getElements();
        while(e.hasMoreElements())
            e.nextElement().setEnabled(state);
    }

    JCheckBox liveUpdate;

    public void init()
    {

        if(isApplet)
        {
            Waveform.zoom_on_mb1 = false;
            setDataAccess();
        }

        setBackground(Color.lightGray);
        wave_container = new WaveformContainer();
        wave_container.addWaveContainerListener(this);

        WavePopup wave_popup = new MultiWavePopup(new SetupWaveformParams(f, "Waveform Params"), new ProfileDialog(null, null));
        wave_container.setPopupMenu(wave_popup);
        wave_container.SetMode(Waveform.MODE_ZOOM);

        getContentPane().setLayout(new BorderLayout());

        JRadioButton point = new JRadioButton("Point", false);
        point.addItemListener(new ItemListener ()
		    {
		       public void itemStateChanged(ItemEvent e)
		       {
		           wave_container.SetMode(Waveform.MODE_POINT);
		       }
		    });

        JRadioButton zoom  = new JRadioButton("Zoom", true);
        zoom.addItemListener(new ItemListener ()
		    {
		       public void itemStateChanged(ItemEvent e)
		       {
		           wave_container.SetMode(Waveform.MODE_ZOOM);
		       }
		    });

        JRadioButton pan  = new JRadioButton("Pan", false);
        pan.addItemListener(new ItemListener ()
		    {
		       public void itemStateChanged(ItemEvent e)
		       {
		           wave_container.SetMode(Waveform.MODE_PAN);
		       }
		    });

        liveUpdate = new JCheckBox("Live Update", false);
        liveUpdate.addChangeListener(new ChangeListener ()
        {
            public void stateChanged(ChangeEvent e)
            {
                setLiveUpdate(liveUpdate.isSelected(), false);
            }
        });
	//liveUpdate.setSelected(false);
	liveUpdate.setVisible(false);


        pointer_mode = new ButtonGroup();
        pointer_mode.add(point);
        pointer_mode.add(zoom);
        pointer_mode.add(pan);


        JPanel panel1 = new JPanel();
        panel1.setLayout(new FlowLayout(FlowLayout.LEFT, 1, 4));
        panel1.add(point);
        panel1.add(zoom);
        panel1.add(pan);
        panel1.add(liveUpdate);


        JPanel panel = new JPanel()
        {
            public void print(Graphics g){}
            public void printAll(Graphics g){}
        };

        panel.setLayout(new BorderLayout());
        panel.add("West", panel1);
        if (!isApplet) {
            point_pos = new JLabel("[0.000000000, 0.000000000]");
            point_pos.setFont(new Font("Courier", Font.PLAIN, 12));
            panel.add("Center", point_pos);

            prnJob = PrinterJob.getPrinterJob();
            pf = prnJob.defaultPage();

            JButton print = new JButton("Print");
            print.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {

                    Thread t = new Thread() {

                        public void run() {

                            try {
                                pf = prnJob.pageDialog(pf);
                                if (prnJob.printDialog()) {
                                    prnJob.setPrintable(wave_container, pf);
                                    prnJob.print();
                                }
                            } catch (Exception ex) {}
                        }
                    };
                    t.start();
                }
            });
            panel.add("East", print);

        }

        getContentPane().add("Center", wave_container);
        getContentPane().add("South", panel);

        if(isApplet) getSignalsParameter();
        wave_container.update();

        appendThread = new AppendThread(100);
        appendThread.start();

        setEnabledMode(true);
        wave_container.SetMode(Waveform.MODE_ZOOM);


    }

    public void enableLiveUpdate()
    {
     	liveUpdate.setVisible(true);
    }

    private int currentMode = Waveform.MODE_ZOOM;
    public void setLiveUpdate(boolean state)
    {
        setLiveUpdate(state, true);
    }
    public void setLiveUpdate(boolean state, boolean setButton)
    {
        if(setButton)
        {
            liveUpdate.setSelected(state);
            return;
        }
	if(wave_container.GetMode() != Waveform.MODE_WAIT)
	{
	    currentMode = wave_container.GetMode();
	}
        if( state )
        {

            setEnabledMode(false);
            wave_container.SetMode(Waveform.MODE_WAIT);
            appendThread.resumeThread();
        }
        else
        {
            appendThread.suspendThread();
            setEnabledMode(true);
            wave_container.SetMode(currentMode);
        }
    }

    public void setSize(int width, int height)
    {
        super.setSize(width, height);
        validate();
    }


    private boolean translateToBoolean(String value)
    {
        if(value.toUpperCase().equals("TRUE"))
            return  true;
        else
            return false;
    }


    private void getSignalsParameter()
    {
        String url = null, gain, offset, row, col, color, marker, name;
        String sig_param;
        String sig_attr = "SIGNAL_";
        int i = 1;
        String global_autentication, signal_autentication, autentication, param;

        global_autentication = getParameter("AUTENTICATION");

        //param = getParameter("PRINT_SCALING");
        //if(param != null)
        //    try {
        //        print_scaling = Integer.parseInt(param);
        //    } catch (NumberFormatException e){}

        param = getParameter("FIXED_LEGEND");
        if(param != null)
        {
            wave_container.setLegendMode(MultiWaveform.LEGEND_BOTTOM);
        }

        param = getParameter("PRINT_WITH_LEGEND");
        if(param != null)
            wave_container.setPrintWithLegend(translateToBoolean(param));

        param = getParameter("PRINT_BW");
        if(param != null)
            wave_container.setPrintBW(translateToBoolean(param));


        try
        {
            while((sig_param = getParameter(sig_attr + i)) != null)
            {
                signal_autentication = getParameterValue(sig_param, "autentication");
                if(signal_autentication != null)
                   autentication = signal_autentication;
                else
                   autentication = global_autentication;

                url = getParameterValue(sig_param, "url");
                Signal s = DataAccessURL.getSignal(url, autentication);
                if(s != null)
                {
                    float k = 1.0F, q = 0.0F;
                    gain = getParameterValue(sig_param, "gain");
                    offset = getParameterValue(sig_param, "offset");
                    if(gain != null || offset != null)
                    {
                        if(offset != null)
                            q = Float.valueOf(offset).floatValue();
                        if(gain != null)
                            k = Float.valueOf(gain).floatValue();

                        s.setCalibrate(k, q);
                    }

                    color = getParameterValue(sig_param, "color");
                    if(color != null)
                    {
                        s.setColor(new Color(Integer.decode(color).intValue()));
                        automatic_color = false;
                    } else
                        automatic_color = true;


                    marker = getParameterValue(sig_param, "marker");
                    s.setMarker(marker);

                    name = getParameterValue(sig_param, "name");
                    if(name != null)
                        s.setName(name);
                    else
                    {
                        int idx = url.indexOf("/") + 2;
                        idx = url.indexOf("/", idx) + 1;
                        s.setName(url.substring(idx, url.length()));
                    }
                    row = getParameterValue(sig_param, "row");
                    col = getParameterValue(sig_param, "col");
                    if(row != null && col != null)
                    {
                        addSignal(s, Integer.parseInt(row), Integer.parseInt(col));
                    }

                }
                i++;
            }
        }
        catch(Exception e)
        {
            if(e instanceof  AccessControlException)
            {
		        JOptionPane.showMessageDialog(this,
		                                        e.toString()+"\n url "+url +
		                                        "\nUse policytool.exe in  JDK or JRE installation directory to add socket access permission\n",
		                                        "alert",
		                                        JOptionPane.ERROR_MESSAGE);
            } else {
		        JOptionPane.showMessageDialog(this,
		                                        e.toString()+" url "+url,
		                                        "alert",
		                                        JOptionPane.ERROR_MESSAGE);
		    }
        }
    }

    private String getParameterValue(String context, String param)
    {
        String value = null;

        StringTokenizer st = new StringTokenizer(context);
        while (st.hasMoreTokens()) {
            value = st.nextToken();
            if(value.equals(param))
            {
                if(st.nextToken().equals("="))
                {
                    value = st.nextToken();
                    break;
                } else
                    return null;

            }
            value = null;
        }
        return value;
    }

    public void processWaveContainerEvent(WaveContainerEvent e)
    {
        String s = null;
	    int event_id = e.getID();

	    switch(event_id)
	    {
	        case WaveContainerEvent.WAVEFORM_EVENT:

	            WaveformEvent we = (WaveformEvent)e.we;
	            Waveform w = (Waveform)we.getSource();

	            s = we.toString();
                if(w instanceof MultiWaveform)
                {
                    MultiWaveform mw = (MultiWaveform)w;
                    String n = mw.getSignalName(we.signal_idx);
                    if(n != null)
                       s = s + " " + n;
                }
                if(isApplet)
                    showStatus(s);
                else
                    point_pos.setText(s);
	        break;
	    }
    }


    /**
     * Add new signal, defined by  x and y float vectors, to the
     * panel in (row, column) position. The panel
     * is created if not already  present.
     *
     * @param x x array values
     * @param y y array values
     * @param row row position, starting from 1
     * @param column column position, starting from 1
     * @param color Signal color
     * @param label Signal name
     * @param inter Interpolation flag, if true a line is draw between adiacent point
     * @param marker Marker point
     */

    public void addSignal(float [] x, float [] y,int row, int column,
        String color, String label, boolean inter, int marker)
    {
        addSignal(new Signal(x, y), row, column, color, label, inter, marker);
    }

    public void addSignal(double [] x, float [] y,int row, int column,
        String color, String label, boolean inter, int marker)
    {
        addSignal(new Signal(x, y), row, column, color, label, inter, marker);
    }

    public void addSignal(Signal sig, int row, int column,
        String color, String label, boolean inter, int marker)
    {

        if(color != null)
	    {
	        if(color.equals("Automatic"))
	        {
		        automatic_color = true;
	        }
	        else
	        {
		        automatic_color = false;
		        sig.setColorIdx(Waveform.ColorNameToIndex(color));
	        }
	    }
        if(label != null && label.length() != 0)
            sig.setName(label);

        sig.setInterpolate(inter);
        sig.setMarker(marker);

        addSignal(sig, row, column);

    }


    public void setLimits(int row, int column, float xmin, float xmax, float ymin, float ymax)
    {
        Component c = null;
        MultiWaveform w = null;
        WaveInterface wi = null;

        c = wave_container.getGridComponent(row, column);

        if(c != null)
        {
            w = (MultiWaveform)c;
            wi = w.getWaveInterface();
            if( wi == null)
            {
                w.setFixedLimits( xmin,  xmax,  ymin,  ymax);
            }
            else
            {
                wi.in_xmax = ""+xmax;
                wi.in_xmin = ""+xmin;
                wi.in_ymax = ""+ymax;
                wi.in_ymin = ""+ymin;
                try
                {
	                wi.StartEvaluate();
	                wi.setLimits();
                } catch(Exception e){}
            }
            w.Update();
        }
    }
	
  private String getExpression(String paramString, boolean infoFlag) throws IOException
  {
    StringTokenizer st = new StringTokenizer(paramString, "/");
    String str1 = st.nextToken();
    String ipAddress = st.nextToken();
    String expAndRegion = st.nextToken();
    String experiment = expAndRegion.substring(0, expAndRegion.indexOf('~'));
    String region = expAndRegion.substring(expAndRegion.indexOf('~') + 1);
    String str6 = st.nextToken();
    String str7 = st.nextToken();
    String str8 = str1 + "/" + ipAddress + "/" + expAndRegion + "/" + str6 + "/";
    String str9 = "Experimet : " + experiment + " Source : " + region + " Shot = " + str6;

    if (str7.startsWith("vexpr")) {
      Object localObject = paramString.substring(paramString.lastIndexOf(',') + 1, paramString.lastIndexOf(')'));
      String str10 = paramString.substring(paramString.indexOf('(') + 1, paramString.lastIndexOf(','));
      str8 = str8 + "vexpr(decompile(`getnci(" + str10 + ",\"record\"))," + (String)localObject + ")";
      str9 = str9 + " uRun = " + (String)localObject + "\n    Data Path : " + str10;
    } else {
      str8 = str8 + "decompile(`getnci(" + str7 + ",\"record\"))";
      str9 = str9 + "\n    Data path : " + str7;
    }

    if (infoFlag) {
      return str9;
    }

    Object localObject = (MdsAccess)DataAccessURL.getDataAccess(str8);

    return str9 + "\n Value : " + ((MdsAccess)localObject).getExpression(str8);
  }

    public String addSignal(String paramString1, int paramInt1, int paramInt2, String paramString2, String paramString3, boolean paramBoolean, int paramInt3, String paramString4)
  {
    String str1 = null;

    Signal localSignal = null;
    String str2 = null;

    int i = paramString1.indexOf("/") + 2;
    i = paramString1.indexOf("/", i) + 1;
    str2 = paramString1.substring(i, paramString1.length());
    try
    {
      localSignal = DataAccessURL.getSignal(paramString1, paramString4);
      if (localSignal != null)
      {
        if (paramString2 != null)
        {
          localSignal.setColor(new Color(Integer.decode(paramString2).intValue()));
          this.automatic_color = false;
        } else {
          this.automatic_color = true;
        }
        localSignal.setMarker(paramInt3);

        if (paramString3 != null)
          localSignal.setName(paramString3);
        else {
          localSignal.setName(str2);
        }
        addSignal(localSignal, paramInt1, paramInt2);

        str1 = getExpression(paramString1, true);
      }

    }
    catch (Exception localException)
    {
      if (localException.toString().indexOf("TdiINVCLADSC") != -1)
      {
        try
        {
          return getExpression(paramString1, false);
        }
        catch (IOException localIOException)
        {
          JOptionPane.showMessageDialog(this, localIOException.toString(), "alert", 0);
        }
      }
      else
      {
        JOptionPane.showMessageDialog(this, localException.toString(), "alert", 0);
      }

    }

    return str1;
  }

  
  
  
    public void addFrames(String url, int row, int column)
    {
        Component c = null;
        MultiWaveform w = null;
        WaveInterface wi = null;
        DataAccess da = null;

        if(DataAccessURL.getNumProtocols() == 0)
            setDataAccess();

        c = wave_container.getGridComponent(row, column);

        try
        {
            da = DataAccessURL.getDataAccess(url);
            da.setProvider(url);

            if(c != null)
            {
                w = (MultiWaveform)c;

                wi  = w.getWaveInterface();
                if(wi == null)
                {
                    if( w.IsImage() || (w.GetSignals() != null && w.GetSignals().size() != 0))
                    {
		                JOptionPane.showMessageDialog(this,
		                                            "The selected waveform panel contains signals or frame.\n" +
		                                            "\nDefine a new waveform panel to show frame image from "+
		                                            da.getDataProvider().getClass().getName() +
		                                            " data provider.",
		                                            "alert",
		                                            JOptionPane.ERROR_MESSAGE);
                        return;
                    }

                    w.setWaveInterface((wi = new WaveInterface(w, da.getDataProvider())));
                }
                else
                {
                    if(!wi.getDataProvider().getClass().getName().equals(da.getDataProvider().getClass().getName()))
                    {
		                JOptionPane.showMessageDialog(this,
		                                            "The selected waveform panel is already connected to " +
		                                            wi.getDataProvider().getClass().getName() +
		                                            " data provider.\nDefine a new waveform panel to show frame image from "+
		                                            da.getDataProvider().getClass().getName() +
		                                            " data provider.",
		                                            "alert",
		                                            JOptionPane.ERROR_MESSAGE);
                        return;
                    }
                }
            }
            else
            {
                w = (MultiWaveform)wave_container.CreateWaveComponent();
                w.setWaveInterface((wi = new WaveInterface(w, da.getDataProvider())));
                wave_container.add(w, row, column);
            }


            if(da != null && wi != null)
            {
                wi.setExperiment(da.getExperiment());
                wi.AddFrames(da.getSignalName());
                wi.setShotArray(da.getShot());

	            wi.StartEvaluate();
		        wi.EvaluateOthers();
		        if(wi.error != null)
		            throw(new IOException(wi.error));
		        w.Update(wi.getFrames());
            }
        }
        catch(Exception e)
        {
            if(e instanceof  AccessControlException)
            {
		        JOptionPane.showMessageDialog(this,
		                                        e.toString()+"\n url "+url +
		                                        "\nUse policytool.exe in  JDK or JRE installation directory to add socket access permission\n",
		                                        "alert",
		                                        JOptionPane.ERROR_MESSAGE);
            } else {
		        JOptionPane.showMessageDialog(this,
		                                        e.toString(),
		                                        "alert",
		                                        JOptionPane.ERROR_MESSAGE);
            }
        }
    }


    public void addSignal(String url, int row, int column,
        String color, String label, boolean inter, int marker)
    {

        Component c = null;
        MultiWaveform w = null;
        WaveInterface wi = null;
        DataAccess da = null;
        Signal s;

        if(DataAccessURL.getNumProtocols() == 0)
            setDataAccess();

        c = wave_container.getGridComponent(row, column);

        try
        {
            da = DataAccessURL.getDataAccess(url);
            da.setProvider(url);

            if(c != null)
            {
                w = (MultiWaveform)c;

                wi  = w.getWaveInterface();
                if(wi == null)
                {
                    if( w.GetSignals() != null && w.GetSignals().size() != 0)
                    {
		                JOptionPane.showMessageDialog(this,
		                                            "In the selected waveform panel there are raw signals.\n" +
		                                            "\nDefine a new waveform panel to show signals from "+
		                                            da.getDataProvider().getClass().getName() +
		                                            " data provider.",
		                                            "alert",
		                                            JOptionPane.ERROR_MESSAGE);
                        return;
                    }

                    w.setWaveInterface((wi = new WaveInterface()));
                    wi.SetDataProvider(da.getDataProvider());
                }
                else
                {
                    if(!wi.getDataProvider().getClass().getName().equals(da.getDataProvider().getClass().getName()))
                    {
		                JOptionPane.showMessageDialog(this,
		                                            "The selected waveform panel is already connected to " +
		                                            wi.getDataProvider().getClass().getName() +
		                                            " data provider.\nDefine a new waveform panel to show signals from "+
		                                            da.getDataProvider().getClass().getName() +
		                                            " data provider.",
		                                            "alert",
		                                            JOptionPane.ERROR_MESSAGE);
                        return;
                    }
                }
            } else {
                w = (MultiWaveform)wave_container.CreateWaveComponent();
                w.setWaveInterface((wi = new WaveInterface()));
                wave_container.add(w, row, column);
                wi.SetDataProvider(da.getDataProvider());
            }


            if(da != null && wi != null)
            {

                wi.setExperiment(da.getExperiment());
                wi.AddSignal(da.getSignalName());
                wi.setShotArray(da.getShot());

	            wi.StartEvaluate();
		        wi.EvaluateOthers();

		        //If added signal has been evaluated without
		        //error it is stored in wi.signals vector

                if(wi.signals != null && (s = wi.signals[wi.signals.length-1]) != null)
                {
                    s.setColorIdx(Waveform.ColorNameToIndex(color));
                    s.setMarker(marker);

                    if(label != null && label.length() != 0)
                        s.setName(label);
                    else
                        s.setName(wi.in_y[0]);
                }

		        w.Update(wi.signals);

            }
        }
        catch(Exception e)
        {
            if(e instanceof  AccessControlException)
            {
		        JOptionPane.showMessageDialog(this,
		                                        e.toString()+"\n url "+url +
		                                        "\nUse policytool.exe in  JDK or JRE installation directory to add socket access permission\n",
		                                        "alert",
		                                        JOptionPane.ERROR_MESSAGE);
            } else {
		        JOptionPane.showMessageDialog(this,
		                                        e.toString(),
		                                        "alert",
		                                        JOptionPane.ERROR_MESSAGE);
            }
        }
    }


	/**
	 * Set Window dialog title.
	 *
	 * @param title Title string
	 */
	public void setTitle(String title)
	{
	    if(f != null)
	        f.setTitle(title);
	}

	/**
	 * Remove all signals added to the panels.
	 */
	public void removeAllSignals()
	{
	    if(wave_container != null)
	        wave_container.RemoveAllSignals();
            if(signals1DHash.size() > 0 ) signals1DHash.clear();
            if(signals2DHash.size() > 0 ) signals2DHash.clear();
            if(signals1DVector.size() > 0 ) signals1DVector.clear();
            if(signals2DVector.size() > 0 ) signals2DVector.clear();
	}
	/**
	 * Remove all signals added to the panels.
	 */
	public void removeAllSignals(int row, int col)
	{
	    if(wave_container != null)
	    {
	    	MultiWaveform wave = (MultiWaveform)wave_container.getGridComponent(row, col);
	    	wave.Erase();
	    }
 	}


    /**
     * Add new signal, defined as x and y float vectors, to
     * panel in (row, column) position. The panel
     * is created if not already  present.
     *
     * @param x x array values
     * @param y y array values
     * @param row Row  position
     * @param column Column  position
     * @param color Signal color
     * @param label Signal name
     */
    public void addSignal(float [] x, float [] y,int row, int column,
        String color, String label)
    {
    	addSignal(x, y, row, column, color, label, true, 0);
    }

    public void addSignal(double [] x, float [] y,int row, int column,
        String color, String label)
    {
    	addSignal(x, y, row, column, color, label, true, 0);
    }

    /**
     * Show window with defined position and size.
     *
     * @param x position
     * @param y position
     * @param width dimension
     * @param height dimension
     */
    public void showWindow(int x, int y, int width, int height)
    {
        if (!this.isShowing()) {
            f.pack();
            f.setBounds(new Rectangle(x, y, width, height));
            f.setVisible(true);
        }
        wave_container.update();
   }


    /**
     * Add new signal, defined as Signal class, to
     * panel in (row, column) position. The panel
     * is created if not already  present.
     *
     * @param s Signal to add
     * @param row Row MultiWaveform position
     * @param column Column MultiWaveform position
     * @param col
     */
    public void addSignal(Signal s, int row, int col)
    {
        Component c = null;
        MultiWaveform w = null;

        c = wave_container.getGridComponent(row, col);

        if(c != null)
        {
            w = (MultiWaveform)c;

            if(w.getWaveInterface() != null)
            {
		        JOptionPane.showMessageDialog(this,
		                                    "The selected waveform panel is  connected to " +
		                                    w.getWaveInterface().getDataProvider().getClass().getName() +
		                                    " data provider.\nDefine a new waveform panel to show raw signals",
		                                    "alert",
		                                    JOptionPane.ERROR_MESSAGE);
                return;
            }

            if(automatic_color)
                s.setColorIdx(w.GetShowSignalCount());
            w.addSignal(s);
        } else {
            w = (MultiWaveform)wave_container.CreateWaveComponent();
            w.addSignal(s);
            w.setLegendMode(MultiWaveform.LEGEND_BOTTOM);
            wave_container.add(w, row, col);
        }
        if(isApplet) showStatus("Add "+s.getName()+" col "+col+" row "+row);


        switch(s.getType())
        {
            case Signal.TYPE_1D:
                this.signals1DHash.put(s.name, s);
                this.signals1DVector.addElement(s);
            break;
            case Signal.TYPE_2D:
                this.signals2DHash.put(s.name, s);
                this.signals2DVector.addElement(s);
            break;

        }
        w.Update();

        if(isShowing() && !isValid())
            wave_container.update();
    }


    public void addSignal(int row, int col, String names, int color, int bufferSize, int type)
    {
        System.out.println("Name "+ names +" bufferSize "+bufferSize);
        Signal s = new Signal(names);
        s.setType(type);
        if(type == Signal.TYPE_2D)
           s.setMode2D(Signal.MODE_PROFILE);
        s.setColor(new Color(color));
        s.setUpdSignalSizeInc(bufferSize);
        addSignal(s, row,  col);
    }

    public void enqueueUpdateSignals(int  numPointsPerSignal, int operation,  float x[], float y[])
    {
       updSignalDataQeue.add(new UpdSignalData(numPointsPerSignal, operation, Signal.TYPE_1D, x, y));
    }

    public void enqueueUpdateSignals(int  numPointsPerSignal, int operation,  float x[], float y[], float times[])
    {
        updSignalDataQeue.add(new UpdSignalData(numPointsPerSignal, operation, Signal.TYPE_2D, x, y, times));
    }

    public void enqueueUpdateSignal(String name, int operation,  float x[], float y[])
    {
        updSignalDataQeue.add(new UpdSignalData(name, operation, Signal.TYPE_1D, x, y));
    }

    public void enqueueUpdateSignal(String name, int operation,  float x[], float y[], float times[])
    {
        updSignalDataQeue.add(new UpdSignalData(name, operation, Signal.TYPE_2D, x, y, times));
    }
}


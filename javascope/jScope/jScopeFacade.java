package jScope;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.awt.image.BufferedImage;
import java.awt.print.PageFormat;
import java.awt.print.Printable;
import java.awt.print.PrinterException;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.net.URL;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Properties;
import java.util.StringTokenizer;
import java.util.Vector;

import javax.print.DocPrintJob;
import javax.print.PrintException;
import javax.print.PrintService;
import javax.print.PrintServiceLookup;
import javax.print.ServiceUI;
import javax.print.attribute.HashPrintRequestAttributeSet;
import javax.print.attribute.PrintRequestAttributeSet;
import javax.print.attribute.standard.MediaPrintableArea;
import javax.print.attribute.standard.MediaSize;
import javax.print.attribute.standard.MediaSizeName;
import javax.print.attribute.standard.OrientationRequested;
import javax.print.attribute.standard.PrinterResolution;
import javax.swing.AbstractButton;
import javax.swing.BorderFactory;
import javax.swing.ButtonGroup;
import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JRadioButton;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JSlider;
import javax.swing.JTextField;
import javax.swing.JWindow;
import javax.swing.KeyStroke;
import javax.swing.ListSelectionModel;
import javax.swing.LookAndFeel;
import javax.swing.RepaintManager;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.MenuEvent;
import javax.swing.event.MenuListener;
import javax.swing.plaf.FontUIResource;
import javax.swing.plaf.basic.BasicArrowButton;


        
public class jScopeFacade
    extends JFrame
    implements ActionListener, ItemListener,
    WindowListener, WaveContainerListener,
    UpdateEventListener, ConnectionListener, Printable
{

    static final String VERSION = "jScope (version 7.4.5)";
    static public boolean is_debug = false;

    public static final int MAX_NUM_SHOT = 30;
    public static final int MAX_VARIABLE = 10;
    static  long refreshPeriod = -1;

    JWindow aboutScreen;

    /**Main menu bar*/
    protected JMenuBar mb;
    /**Menus on menu bar */
    protected JMenu edit_m, look_and_feel_m, pointer_mode_m,
        customize_m, autoscale_m, network_m, help_m;
    JMenu servers_m, updates_m;
    private JCheckBoxMenuItem update_i, update_when_icon_i;
    /**Menu items on menu edit_m */
    private JMenuItem exit_i, win_i;
    protected JMenuItem default_i, use_i, pub_variables_i, save_as_i,
        use_last_i,
        save_i, color_i, print_all_i, open_i,
        close_i, server_list_i, font_i, save_all_as_text_i,
        free_cache_i;
    private JCheckBoxMenuItem brief_error_i;
    /**Menu item on menu pointer_mode_m */
    private JMenuItem zoom_i, point_i, copy_i, pan_i;
    /**Menu item on menu autoscale_m */
    private JMenuItem all_i, allY_i;

    private JMenuItem print_i, properties_i;
    private String propertiesFilePath = null;
 
    private JPanel panel, panel1;
    private ButtonGroup pointer_mode = new ButtonGroup();
    private JRadioButton zoom, point, copy, pan;
    private JLabel shot_l;
    private JTextField shot_t, signal_expr;
    private JButton apply_b;
    private JFileChooser file_diag;
    protected String curr_directory;
    protected String last_directory;
    private JLabel point_pos;
    private JTextField info_text, net_text;
    private WindowDialog win_diag;
    public ColorDialog color_dialog;
    public ColorMapDialog colorMapDialog;
    public FontSelection font_dialog;
    jScopeWaveContainer wave_panel;
    SetupDefaults setup_default;
    private PubVarDialog pub_var_diag;
    static int num_scope = 0;
    private String config_file;
    static DataServerItem[] server_ip_list;
    ServerDialog server_diag;
    static boolean not_sup_local = false;
    private boolean executing_update = false;

    private static jScopeFacade win;
    public static boolean busy(){return win.executing_update;}

    DocPrintJob prnJob = null;
  //  PageFormat pageFormat;
    PrintRequestAttributeSet attrs;
    PrintService[] printersServices;
    PrintService   printerSelection;


    Properties js_prop = null;
    int default_server_idx;
    boolean is_playing = false;
    int height = 500, width = 700, xpos = 50, ypos = 50;
    jScopeDefaultValues def_values = new jScopeDefaultValues();
    SetupDataDialog setup_dialog;
    JProgressBar progress_bar;

    private jScopeBrowseUrl help_dialog;

    private boolean modified = false;

    // LookAndFeel class names
    static String macClassName =
        "com.sun.java.swing.plaf.mac.MacLookAndFeel";
    static String metalClassName =
        "javax.swing.plaf.metal.MetalLookAndFeel";
    static String motifClassName =
        "com.sun.java.swing.plaf.motif.MotifLookAndFeel";
    static String windowsClassName =
        "com.sun.java.swing.plaf.windows.WindowsLookAndFeel";

    // L&F radio buttons
    JRadioButtonMenuItem macMenuItem;
    JRadioButtonMenuItem metalMenuItem;
    JRadioButtonMenuItem motifMenuItem;
    JRadioButtonMenuItem windowsMenuItem;

    static Component T_parentComponent;
    static Object T_message;
    static String T_title;
    static int T_messageType;


    BasicArrowButton incShot, decShot;
    int incShotValue = 0;

    public static final int JAVA_TIME = 1, VMS_TIME = 2, EPICS_TIME = 3;
    static int timeMode = JAVA_TIME;
    static final long VMS_BASE = 0x7c95674beb4000L;
    //static final long EPICS_BASE = 631062000000L;
    //static final long EPICS_BASE = 631148400000L;
    static final long EPICS_BASE = 631152000000L;


    static int getTimeMode() {return timeMode; }


    static long convertFromSpecificTime(long inTime)
    {
        if (timeMode == VMS_TIME)
                return (inTime - VMS_BASE) / 10000L;
        else if(timeMode == EPICS_TIME)
        {
            long currTime =  inTime/1000000L + EPICS_BASE;
            return currTime;
        }
        else 
            return inTime;
    }

    static long convertToSpecificTime(long inTime)
    {
        if (timeMode == VMS_TIME)
                return (inTime*10000L + VMS_BASE);
        else if (timeMode == EPICS_TIME)
            return (inTime - EPICS_BASE)*1000000L;
        else
            return inTime;
    }

    static public void ShowMessage(Component parentComponent, Object message,
                                   String title, int messageType)
    {
        T_parentComponent = parentComponent;
        T_message = message;
        T_title = title;
        T_messageType = messageType;

        // do the following on the gui thread
        SwingUtilities.invokeLater(new Runnable()
        {
            public void run()
            {
                JOptionPane.showMessageDialog(T_parentComponent, T_message,
                                              T_title,
                                              T_messageType);
            }
        });

    }


    public int print(Graphics g, PageFormat pf, int pageIndex) throws
    PrinterException
{
/*
    int st_x = 0, st_y = 0;
    double height = pf.getImageableHeight();
    double width  = pf.getImageableWidth();
*/
    Graphics2D g2 = (Graphics2D) g;

//        jScope.displayPageFormatAttributes(4,pf);

    if (pageIndex == 0)
    {
        g2.translate(pf.getImageableX(), pf.getImageableY());

        RepaintManager currentManager = RepaintManager.currentManager(this);
        currentManager.setDoubleBufferingEnabled(false);
        g2.scale(72.0/600, 72.0/600);
//        Dimension d = this.getSize();
//        this.setSize((int)(d.width*600.0/72.0), (int)(d.height*600.0/72.0));
//        this.printAll(g2);

        g2.translate(pf.getImageableWidth() / 2, pf.getImageableHeight() / 2);
        g2.translate( pf.getWidth() / 2, pf.getHeight() / 2);

        g.drawOval(0,0,100,100);
        g.drawOval(0,0,200,200);
        g.drawOval(0,0,300,300);
        g.drawOval(0,0,400,400);
        g.drawOval(0,0,500,500);
        g.drawOval(0,0,600,600);

        currentManager.setDoubleBufferingEnabled(true);

        return Printable.PAGE_EXISTS;
    }
    else
        return Printable.NO_SUCH_PAGE;
}

    static public void displayPageFormatAttributes(int idx,
        PageFormat myPageFormat)
    {
        System.out.println(
            "+----------------------------------------------------------+");
        System.out.println("Index = " + idx);
        System.out.println("Width = " + myPageFormat.getWidth());
        System.out.println("Height = " + myPageFormat.getHeight());
        System.out.println("ImageableX = " + myPageFormat.getImageableX());
        System.out.println("ImageableY = " + myPageFormat.getImageableY());
        System.out.println("ImageableWidth = " + myPageFormat.getImageableWidth());
        System.out.println("ImageableHeight = " + myPageFormat.getImageableHeight());
        int o = myPageFormat.getOrientation();
        System.out.println("Orientation = " +
                           (o == PageFormat.PORTRAIT ? "PORTRAIT" :
                            o == PageFormat.LANDSCAPE ? "LANDSCAPE" :
                            o == PageFormat.REVERSE_LANDSCAPE ?
                            "REVERSE_LANDSCAPE" :
                            "<invalid>"));
        System.out.println(
            "+----------------------------------------------------------+");
    }


    class FileFilter implements FilenameFilter
    {
        String fname = null;

        FileFilter(String fname)
        {
            this.fname = fname;
        }

        public boolean accept(File dir, String name)
         {
             if(name.indexOf(fname) == 0)
                 return true;
             else
                 return false;
         }

    }

    class PubVarDialog
        extends JDialog
        implements ActionListener
    {

        private Vector<String> name_list = new Vector<>();
        private Vector<String> expr_list = new Vector<>();
        private JButton apply, cancel, save, reset;
        jScopeFacade dw;
        boolean is_pv_apply = false;

        PubVarDialog(Frame fw)
        {

            super(fw, "Public Variables", false);
            dw = (jScopeFacade) fw;

            GridBagConstraints c = new GridBagConstraints();
            GridBagLayout gridbag = new GridBagLayout();
            getContentPane().setLayout(gridbag);

            c.insets = new Insets(2, 2, 2, 2);
            c.fill = GridBagConstraints.NONE;

            c.anchor = GridBagConstraints.CENTER;
            c.gridwidth = 1;
            JLabel lab = new JLabel("Name");
            gridbag.setConstraints(lab, c);
            getContentPane().add(lab);

            c.gridwidth = GridBagConstraints.REMAINDER;
            lab = new JLabel("Expression");
            gridbag.setConstraints(lab, c);
            getContentPane().add(lab);

            JTextField txt;
            c.anchor = GridBagConstraints.WEST;
            c.fill = GridBagConstraints.BOTH;
            for (int i = 0; i < MAX_VARIABLE; i++)
            {
                c.gridwidth = 1;
                txt = new JTextField(10);
                gridbag.setConstraints(txt, c);
                getContentPane().add(txt);

                c.gridwidth = GridBagConstraints.REMAINDER;
                txt = new JTextField(30);
                gridbag.setConstraints(txt, c);
                getContentPane().add(txt);
            }

            JPanel p = new JPanel();
            p.setLayout(new FlowLayout(FlowLayout.CENTER));

            apply = new JButton("Apply");
            apply.addActionListener(this);
            p.add(apply);

            save = new JButton("Save");
            save.addActionListener(this);
            p.add(save);

            reset = new JButton("Reset");
            reset.addActionListener(this);
            p.add(reset);

            cancel = new JButton("Cancel");
            cancel.addActionListener(this);
            p.add(cancel);

            c.gridwidth = GridBagConstraints.REMAINDER;
            gridbag.setConstraints(p, c);
            getContentPane().add(p);

        }

        public String getCurrentPublicVar()
        {
            String txt1, txt2, str;
            StringBuffer buf = new StringBuffer();
            Container p = getContentPane();

            for (int i = 2; i < MAX_VARIABLE * 2; i += 2)
            {
                txt1 = ( (JTextField) p.getComponent(i)).getText();
                txt2 = ( (JTextField) p.getComponent(i + 1)).getText();
                if (txt1.length() != 0 && txt2.length() != 0)
                {
                    if (txt1.indexOf("_") != 0)
                        str = "public _" + txt1 + " = ( " + txt2 + ";)";
                    else
                        str = "public " + txt1 + " = (" + txt2 + ";)";
                    buf.append(str);
                }
            }
            return (new String(buf));
        }

        public String getPublicVar()
        {

            if (is_pv_apply)
                return getCurrentPublicVar();

            String txt1, txt2, str;
            StringBuffer buf = new StringBuffer();

            for (int i = 0; i < name_list.size() && i < MAX_VARIABLE; i++)
            {
                txt1 = (String) name_list.elementAt(i);
                txt2 = (String) expr_list.elementAt(i);
                if (txt1.length() != 0 && txt2.length() != 0)
                {
                    if (txt1.indexOf("_") != 0)
                        str = "public _" + txt1 + " = (" + txt2 + ");";
                    else
                        str = "public " + txt1 + " = (" + txt2 + ");";
                    buf.append(str);
                }
            }
            return (new String(buf));
        }

        private void SavePubVar()
        {
            String txt1, txt2;

            if (name_list.size() != 0)
                name_list.removeAllElements();
            if (expr_list.size() != 0)
                expr_list.removeAllElements();

            Container p = getContentPane();

            for (int i = 2, j = 0; j < MAX_VARIABLE; i += 2, j++)
            {
                txt1 = ( (JTextField) p.getComponent(i)).getText();
                txt2 = ( (JTextField) p.getComponent(i + 1)).getText();
                if (txt1.length() != 0 && txt2.length() != 0)
                {
                    name_list.insertElementAt(new String(txt1), j);
                    expr_list.insertElementAt(new String(txt2), j);
                }
            }
            dw.setChange(true);
        }

        private void SetPubVar()
        {
            Container p = getContentPane();
            for (int i = 2, j = 0; j < name_list.size() && j < MAX_VARIABLE; i += 2, j++)
            {
                ( (JTextField) p.getComponent(i)).setText( (String) name_list.
                    elementAt(j));
                ( (JTextField) p.getComponent(i + 1)).setText( (String)
                    expr_list.elementAt(j));
            }
        }

        public void Show()
        {
            is_pv_apply = true;
            SetPubVar();
            pack();
            setLocationRelativeTo(dw);
            setVisible(true);
        }

        public void actionPerformed(ActionEvent e)
        {
            Object ob = e.getSource();

            if (ob == apply)
            {
                dw.setPublicVariables(getPublicVar());
                dw.UpdateAllWaves();
            }

            if (ob == save)
            {
                SavePubVar();
            }

            if (ob == reset)
            {
                SetPubVar();
            }

            if (ob == cancel)
            {
                is_pv_apply = false;
                setVisible(false);
            }
        }

        public void toFile(PrintWriter out, String prompt)
        {
            for (int i = 0; i < name_list.size() && i < MAX_VARIABLE; i++)
            {
                out.println(prompt + i + ": " + name_list.elementAt(i)
                            + " = " + expr_list.elementAt(i));
            }
            out.println("");
        }

        public void fromFile(Properties pr, String prompt) throws IOException
        {
            String prop;
            int idx = 0;
            
             name_list.removeAllElements();
             expr_list.removeAllElements();
            
            while ( (prop = pr.getProperty(prompt + idx)) != null && idx < MAX_VARIABLE )
            {
                StringTokenizer st = new StringTokenizer(prop, "=");
                String name = st.nextToken();
                String expr = st.nextToken("");
                expr =   expr.substring(expr.indexOf('=') + 1) ;//remove first = character in the expression
                name_list.insertElementAt(name.trim(), idx);
                expr_list.insertElementAt(expr.trim(), idx);
                idx++;
            }
        }
    }

    /**
     * Show the spash screen while the rest of the demo loads
     */
    public void createAboutScreen()
    {
        JLabel aboutLabel = new AboutWindow();
        aboutScreen = new JWindow();
        aboutScreen.addMouseListener(new MouseAdapter()
        {
            public void mouseClicked(MouseEvent e)
            {
                hideAbout();
            }
        });

        aboutScreen.getContentPane().add(aboutLabel);

        aboutScreen.pack();
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        aboutScreen.setLocation(screenSize.width / 2 -
                                aboutScreen.getSize().width / 2,
                                screenSize.height / 2 -
                                aboutScreen.getSize().height / 2);
    }

    public void showAboutScreen()
    {
        aboutScreen.setVisible(true);
    }

    /**
     * pop down the spash screen
     */
    public void hideAbout()
    {
        aboutScreen.setVisible(false);
        aboutScreen.dispose();
        aboutScreen = null;
    }

    public jScopeFacade(int spos_x, int spos_y, String propFile)
    {

        if (num_scope == 0)
        {
            createAboutScreen();

            // do the following on the gui thread
            SwingUtilities.invokeLater(new Runnable()
            {
                public void run()
                {
                    showAboutScreen();
                }
            });
        }
        
        this.setPropertiesFile(propFile);
        
        jScopeCreate(spos_x, spos_y);

    }

    public void jScopeCreate(int spos_x, int spos_y)
    {

        printersServices = PrintServiceLookup.lookupPrintServices(null, null);
        printerSelection = PrintServiceLookup.lookupDefaultPrintService();
        attrs = new HashPrintRequestAttributeSet();
        PrinterResolution res = new PrinterResolution(600, 600,
            PrinterResolution.DPI);
        attrs.add(MediaSizeName.ISO_A4);
        attrs.add(OrientationRequested.LANDSCAPE);
        attrs.add(new MediaPrintableArea(5, 5,
                                         MediaSize.ISO.A4.getX(MediaSize.MM) -
                                         5,
                                         MediaSize.ISO.A4.getY(MediaSize.MM) -
                                         5,
                                         MediaPrintableArea.MM));
        attrs.add(res);
        if(printerSelection != null)
            prnJob = printerSelection.createPrintJob();

/*
        PrintServiceAttributeSet pras = printerSelection.getAttributes();

        MediaSize at = (MediaSize) printerSelection.getDefaultAttributeValue(
            MediaSize.ISO.A4.getCategory());

        System.out.println("Size name " + at);

        {
            PrintService[] pservice = PrintServiceLookup.lookupPrintServices(null,null);
            for (int i=0; i<pservice.length; i++)
            { System.out.println(pservice[i]);
              System.out.println(" --- Job Attributes ---");
              Class[] cats = pservice[i].getSupportedAttributeCategories();
              for (int j=0; j < cats.length; j++)
              { Attribute attr=(Attribute)pservice[i].getDefaultAttributeValue(cats[j]);
                if (attr != null)
                { // Get attribute name and values
                  String attrName = attr.getName();
                  String attrValue = attr.toString();
                  System.out.println(" "+attrName+": "+attrValue);
                  Object o = pservice[i].getSupportedAttributeValues(attr.getCategory(),null, null);
                  if (o.getClass().isArray())
                  { for (int k=0; k < Array.getLength(o); k++)
                    { Object o2 = Array.get(o, k);
                      System.out.println(" "+o2);
                    }
                  }
                  else
                  {
                    System.out.println(" "+o);
                  }
                }
              }
            }
        }
*/
        help_dialog = new jScopeBrowseUrl(this);
        try
        {

	    String path = "jdocs/jScope.html";
            URL url = getClass().getClassLoader().getResource(path);
            help_dialog.connectToBrowser(url);
        }
        catch (Exception e)
        {}

        setBounds(spos_x, spos_y, 750, 550);

        InitProperties();
        GetPropertiesValue();

        font_dialog   = new FontSelection(this, "Waveform Font Selection");
        setup_default = new SetupDefaults(this, "Default Setup", def_values);
        color_dialog  = new ColorDialog(this, "Color Configuration Dialog");
        colorMapDialog = new ColorMapDialog(this, js_prop.getProperty("jScope.color_palette_file"));

        pub_var_diag  = new PubVarDialog(this);

        getContentPane().setLayout(new BorderLayout());
        setBackground(Color.lightGray);

        addWindowListener(this);
        
        
        file_diag = new JFileChooser();

        file_diag.addChoosableFileFilter( new  javax.swing.filechooser.FileFilter()
        {
            public boolean accept(File f) {
                return f.isDirectory() || f.getName().toLowerCase().endsWith(".dat");
            }
    
            public String getDescription() {
                return ".dat files";
            }
        });

        file_diag.addChoosableFileFilter( new  javax.swing.filechooser.FileFilter()
        {
            public boolean accept(File f) {
                return f.isDirectory() || f.getName().toLowerCase().endsWith(".jscp");
            }
    
            public String getDescription() {
                return ".jscp files";
            }
        });

        
        
        mb = new JMenuBar();
        setJMenuBar(mb);
        edit_m = new JMenu("File");
        mb.add(edit_m);

        JMenuItem browse_signals_i = new JMenuItem("Browse signals");
        edit_m.add(browse_signals_i);
        browse_signals_i.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                wave_panel.ShowBrowseSignals();
            }
        }
        );

        open_i = new JMenuItem("New Window");
        open_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N,
            ActionEvent.CTRL_MASK));
        edit_m.add(open_i);
        open_i.addActionListener(this);

        look_and_feel_m = new JMenu("Look & Feel");
        edit_m.add(look_and_feel_m);

        JMenuItem sign = new JMenuItem("History...");
        sign.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                SignalsBoxDialog sig_box_diag = new SignalsBoxDialog(jScopeFacade.this,
                    "Visited Signals", false);
                sig_box_diag.setVisible(true);
            }
        });

        edit_m.add(sign);

        edit_m.addSeparator();

        //Copy image to clipborad can be done only with
        //java release 1.4
//    if(AboutWindow.javaVersion.indexOf("1.4") != -1)
        {
            JMenuItem cb_copy = new JMenuItem("Copy to Clipboard");
            cb_copy.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    Dimension dim = wave_panel.getSize();
                    BufferedImage ri = new BufferedImage(dim.width, dim.height,
                        BufferedImage.TYPE_INT_RGB);
                    Graphics2D g2d = (Graphics2D) ri.getGraphics();
                    g2d.setBackground(Color.white);
                    wave_panel.PrintAll(g2d, dim.height, dim.width);
                    try
                    {
                        ImageTransferable imageTransferable = new
                            ImageTransferable(ri);
                        Clipboard cli = Toolkit.getDefaultToolkit().
                            getSystemClipboard();
                        cli.setContents(imageTransferable, imageTransferable);
                    }
                    catch (Exception exc)
                    {
                        System.out.println("Exception " + exc);
                    }
                }
            });
            edit_m.add(cb_copy);
            edit_m.addSeparator();
        }

        print_i = new JMenuItem("Print Setup ...");
        print_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_P,
            ActionEvent.CTRL_MASK));
        print_i.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                Thread print_cnf = new Thread()
                {
                    public void run()
                    {
                        setName("Print Dialog Thread");
                        PrintService svc = PrintServiceLookup.lookupDefaultPrintService();
                        printerSelection = ServiceUI.printDialog(
                            null, 100, 100, printersServices, svc, null, attrs);
                        if ( printerSelection != null)
                        {
                            System.out.println(  printerSelection.getName() + " |||| "
                                               + printerSelection.getSupportedDocFlavors() + " |||| "
                                               + printerSelection.hashCode());


                            prnJob = printerSelection.createPrintJob();

                            PrintAllWaves(attrs);

                            /*
                            try
                            {
                                DocFlavor flavor = DocFlavor.SERVICE_FORMATTED.PRINTABLE;
                                Doc doc = new SimpleDoc(jScope.this, flavor, null);
                                prnJob.print(doc, attrs);
                            }
                            catch(Exception exc)
                            {
                                System.out.println(exc);
                            }
                            */

                        }
                    }
                };
                print_cnf.start();
            }
        });
        edit_m.add(print_i);

        /*****************************************************************************************
            page_i = new JMenuItem("Page Setup ...");
            page_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_J, ActionEvent.CTRL_MASK));
            page_i.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    Thread page_cnf = new Thread()
                    {
                        public void run()
                        {
                            setName("Page  Dialog Thread");
                            //pageFormat = prnJob.pageDialog(pageFormat);
                            //prnJob.validatePage(pageFormat);
                            //displayPageFormatAttributes(pageFormat);
                        }
                    };
                    page_cnf.start();
                }
            });
            edit_m.add(page_i);
         *********************************************************************************************/

        print_all_i = new JMenuItem("Print");
        print_all_i.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                Thread print_page = new Thread()
                {
                    public void run()
                    {
                        setName("Print All Thread");
                        PrintAllWaves(attrs);
                    }
                };
                print_page.start();
            }
        });
        edit_m.add(print_all_i);

        edit_m.addSeparator();

        properties_i = new JMenuItem("Properties...");
        properties_i.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                PropertiesEditor pe = new PropertiesEditor(jScopeFacade.this,
                    propertiesFilePath);
                pe.setVisible(true);
            }
        });
        edit_m.add(properties_i);

        edit_m.addSeparator();

        close_i = new JMenuItem("Close");
        close_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Q,
            ActionEvent.CTRL_MASK));
        edit_m.add(close_i);
        close_i.addActionListener(this);

        exit_i = new JMenuItem("Exit");
        exit_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_E,
            ActionEvent.CTRL_MASK));
        edit_m.add(exit_i);
        exit_i.addActionListener(this);

        // Look and Feel Radio control

        ButtonGroup group = new ButtonGroup();
        ToggleUIListener toggleUIListener = new ToggleUIListener();

        metalMenuItem = (JRadioButtonMenuItem) look_and_feel_m.add(new
            JRadioButtonMenuItem("Java Look and Feel"));
        metalMenuItem.setSelected(UIManager.getLookAndFeel().getName().equals(
            "Metal"));
        metalMenuItem.setSelected(true);
        metalMenuItem.setEnabled(isAvailableLookAndFeel(metalClassName));
        group.add(metalMenuItem);
        metalMenuItem.addItemListener(toggleUIListener);
//	metalMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_1, ActionEvent.ALT_MASK));

        motifMenuItem = (JRadioButtonMenuItem) look_and_feel_m.add(new
            JRadioButtonMenuItem("Motif Look and Feel"));
        motifMenuItem.setSelected(UIManager.getLookAndFeel().getName().equals(
            "CDE/Motif"));
        motifMenuItem.setEnabled(isAvailableLookAndFeel(motifClassName));
        group.add(motifMenuItem);
        motifMenuItem.addItemListener(toggleUIListener);
//	motifMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_2, ActionEvent.ALT_MASK));

        windowsMenuItem = (JRadioButtonMenuItem) look_and_feel_m.add(new
            JRadioButtonMenuItem("Windows Style Look and Feel"));
        windowsMenuItem.setSelected(UIManager.getLookAndFeel().getName().equals(
            "Windows"));
        windowsMenuItem.setEnabled(isAvailableLookAndFeel(windowsClassName));
        group.add(windowsMenuItem);
        windowsMenuItem.addItemListener(toggleUIListener);
//	windowsMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_3, ActionEvent.ALT_MASK));

        macMenuItem = (JRadioButtonMenuItem) look_and_feel_m.add(new
            JRadioButtonMenuItem("Macintosh Look and Feel"));
        macMenuItem.setSelected(UIManager.getLookAndFeel().getName().equals(
            "Macintosh"));
        macMenuItem.setEnabled(isAvailableLookAndFeel(macClassName));
        group.add(macMenuItem);
        macMenuItem.addItemListener(toggleUIListener);
//	macMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_4, ActionEvent.ALT_MASK));

        pointer_mode_m = new JMenu("Pointer mode");
        mb.add(pointer_mode_m);
        point_i = new JMenuItem("Point");
        point_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_1,
            ActionEvent.CTRL_MASK));
        point_i.addActionListener(this);
        pointer_mode_m.add(point_i);
        zoom_i = new JMenuItem("Zoom");
        zoom_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_2,
            ActionEvent.CTRL_MASK));
        pointer_mode_m.add(zoom_i);
        zoom_i.addActionListener(this);
        pan_i = new JMenuItem("Pan");
        pan_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_3,
            ActionEvent.CTRL_MASK));
        pointer_mode_m.add(pan_i);
        pan_i.addActionListener(this);
        copy_i = new JMenuItem("Copy");
        copy_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_4,
            ActionEvent.CTRL_MASK));
        pointer_mode_m.add(copy_i);
        copy_i.addActionListener(this);
        pointer_mode_m.add(copy_i);

        customize_m = new JMenu("Customize");
        mb.add(customize_m);
        default_i = new JMenuItem("Global Settings ...");
        default_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_G,
            ActionEvent.CTRL_MASK));
        customize_m.add(default_i);
        default_i.addActionListener(this);
        win_i = new JMenuItem("Window ...");
        win_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_W,
            ActionEvent.CTRL_MASK));
        win_i.addActionListener(this);
        customize_m.add(win_i);

        font_i = new JMenuItem("Font selection ...");
        font_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F,
            ActionEvent.CTRL_MASK));
        font_i.addActionListener(this);
        customize_m.add(font_i);
        color_i = new JMenuItem("Colors List ...");
        color_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O,
            ActionEvent.CTRL_MASK));
        color_i.addActionListener(this);
        customize_m.add(color_i);
        pub_variables_i = new JMenuItem("Public variables ...");
        pub_variables_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_U,
            ActionEvent.CTRL_MASK));
        pub_variables_i.addActionListener(this);
        customize_m.add(pub_variables_i);
        brief_error_i = new JCheckBoxMenuItem("Brief Error", true);
        brief_error_i.addItemListener(this);
        customize_m.add(brief_error_i);

        customize_m.add(new JSeparator());
        use_last_i = new JMenuItem("Use last saved settings");
        use_last_i.addActionListener(this);
        use_last_i.setEnabled(false);
        customize_m.add(use_last_i);

        use_i = new JMenuItem("Use saved settings from ...");
        use_i.addActionListener(this);
        customize_m.add(use_i);

        customize_m.add(new JSeparator());

        save_i = new JMenuItem("Save current settings");
        save_i.setEnabled(false);
        save_i.addActionListener(this);
        customize_m.add(save_i);

        save_as_i = new JMenuItem("Save current settings as ...");
        customize_m.add(save_as_i);
        save_as_i.addActionListener(this);

        customize_m.add(new JSeparator());
        save_all_as_text_i = new JMenuItem("Save all as text ...");
        save_all_as_text_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A,
            ActionEvent.CTRL_MASK));
        customize_m.add(save_all_as_text_i);
        save_all_as_text_i.addActionListener(this);

        updates_m = new JMenu("Updates");
        mb.add(updates_m);
        update_i = new JCheckBoxMenuItem("Disable", false);
        update_when_icon_i = new JCheckBoxMenuItem("Disable when icon", true);
        updates_m.add(update_i);
        updates_m.add(update_when_icon_i);

        autoscale_m = new JMenu("Autoscale");
        mb.add(autoscale_m);
        all_i = new JMenuItem("All");
        all_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_B,
            ActionEvent.CTRL_MASK));
        all_i.addActionListener(this);
        autoscale_m.add(all_i);
        allY_i = new JMenuItem("All Y");
        allY_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Y,
            ActionEvent.CTRL_MASK));
        autoscale_m.add(allY_i);
        allY_i.addActionListener(this);

        network_m = new JMenu("Network");
        mb.add(network_m);


        servers_m = new JMenu("Servers");
        network_m.add(servers_m);
        //servers_m.addActionListener(this);
        servers_m.addMenuListener(new MenuListener()
        {
            public void menuSelected(MenuEvent e)
            {
                server_diag.addServerIpList(server_ip_list);
            }

            public void menuCanceled(MenuEvent e)
            {
            }

            public void menuDeselected(MenuEvent e)
            {
            }
        }
        );

        server_list_i = new JMenuItem("Edit server list ...");
        network_m.add(server_list_i);
        server_list_i.addActionListener(this);

         point_pos = new JLabel("[0.000000000, 0.000000000]");
        point_pos.setFont(new Font("Courier", Font.PLAIN, 12));
        info_text = new JTextField(" Status : ", 85);
        info_text.setBorder(BorderFactory.createLoweredBevelBorder());

//    ImageIcon icon = new ImageIcon("printer1.gif");

        JPanel progress_pan = new JPanel(new FlowLayout(2, 0, 0));
        progress_bar = new JProgressBar(0, 100);
        progress_bar.setBorder(BorderFactory.createLoweredBevelBorder());
        progress_bar.setStringPainted(true);
        progress_pan.add(progress_bar);
        /*
             print_icon = new JLabel(icon)
             {
            // overrides imageUpdate to control the animated gif's animation
            public boolean imageUpdate(Image img, int infoflags,
                        int x, int y, int width, int height)
            {
                System.out.println("Update image "+ infoflags);
             if (isShowing() && (infoflags & ALLBITS) != 0 && (infoflags & FRAMEBITS) == 0)
                    repaint();
                if (isShowing() && (infoflags & FRAMEBITS) != 0 && false)
                    repaint();
                return isShowing();
            }
             };
             print_icon.setBorder(BorderFactory.createLoweredBevelBorder());
          //   print_icon.setSize(20, 30);
             icon.setImageObserver(print_icon);
             progress_pan.add(print_icon);
         */
        help_m = new JMenu("Help");
        mb.add(help_m);
        JMenuItem about_i = new JMenuItem("About jScope");
        help_m.add(about_i);
        about_i.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
//                help_dialog.setVisible(true);
                 JOptionPane.showMessageDialog(jScopeFacade.this, "The jScope tutorial is available at www.mdsplus.org in \"Documentation/The MDSplus tutorial\" section",
                        "", JOptionPane.INFORMATION_MESSAGE);
            }
        }
        );

        setup_dialog = new SetupDataDialog(this, "Setup");

        wave_panel = buildWaveContainer();

        wave_panel.addWaveContainerListener(this);

        wave_panel.SetParams(Waveform.MODE_ZOOM,
                             setup_default.getGridMode(),
                             setup_default.getLegendMode(),
                             setup_default.getXLines(),
                             setup_default.getYLines(),
                             setup_default.getReversed());

        wave_panel.setPopupMenu(new jScopeWavePopup(setup_dialog,
            new ProfileDialog(null, null), colorMapDialog ));

        getContentPane().add("Center", wave_panel);

        panel = new JPanel();
        panel.setLayout(new FlowLayout(FlowLayout.LEFT, 2, 3));
        shot_l = new JLabel("Shot");

        shot_t = new JTextField(10);
        shot_t.addActionListener(this);
        /*
             shot_t.addFocusListener( new FocusAdapter()
            {
               public void focusLost(FocusEvent e)
               {
                    wave_panel.SetMainShot(shot_t.getText());
               }
            }
             );
         */

        apply_b = new JButton("Apply");
        apply_b.addActionListener(this);

        point = new JRadioButton("Point");
        point.addItemListener(this);
        zoom = new JRadioButton("Zoom", true);
        zoom.addItemListener(this);
        pan = new JRadioButton("Pan");
        pan.addItemListener(this);
        copy = new JRadioButton("Copy");
        copy.addItemListener(this);

        pointer_mode.add(point);
        pointer_mode.add(zoom);
        pointer_mode.add(pan);
        pointer_mode.add(copy);

        panel.add(point);
        panel.add(zoom);
        panel.add(pan);
        panel.add(copy);
        panel.add(shot_l);


        panel.add(decShot = new BasicArrowButton(BasicArrowButton.WEST));
        panel.add(shot_t);
        panel.add(incShot = new BasicArrowButton(BasicArrowButton.EAST));

        decShot.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                if( shot_t.getText() != null && shot_t.getText().trim().length() != 0 )
                {
                    if(!executing_update)
                    {
                        incShotValue--;
                        ArrowsIncDecShot();
                        UpdateAllWaves();
                    }
                }
            }
        }
        );

        incShot.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                if( shot_t.getText() != null && shot_t.getText().trim().length() != 0 )
                {
                    if(!executing_update)
                    {
                        incShotValue++;
                        ArrowsIncDecShot();
                        UpdateAllWaves();
                    }
                }
            }
        }
        );


        panel.add(apply_b);
        panel.add(new JLabel(" Signal: "));

        JPanel panel3 = new JPanel();
        GridBagLayout gridbag = new GridBagLayout();
        GridBagConstraints c = new GridBagConstraints();
        Insets insets = new Insets(2, 2, 2, 2);

        panel3.setLayout(gridbag);
        c.anchor = GridBagConstraints.WEST;
        c.fill = GridBagConstraints.HORIZONTAL;
        c.insets = insets;
        c.weightx = 1.0;
        c.gridwidth = 1;

        signal_expr = new JTextField(25);
        signal_expr.addActionListener(this);

        gridbag.setConstraints(signal_expr, c);
        panel3.add(signal_expr);

        JPanel panel4 = new JPanel(new BorderLayout());
        panel4.add("West", panel);
        panel4.add("Center", panel3);

        JPanel panel2 = new JPanel();

        panel2.setLayout(new BorderLayout());
        panel2.add(BorderLayout.WEST, progress_pan);
        panel2.add(BorderLayout.CENTER, info_text);
        panel2.add(BorderLayout.EAST,
                   net_text = new JTextField(" Data server :", 25));
        net_text.setBorder(BorderFactory.createLoweredBevelBorder());
        info_text.setEditable(false);
        net_text.setEditable(false);

        panel1 = new JPanel();
        panel1.setLayout(new BorderLayout());
        panel1.add("North", panel4);
        panel1.add("Center", point_pos);
        panel1.add("South", panel2);

        getContentPane().add("South", panel1);

        color_dialog.SetReversed(setup_default.getReversed());

        if (jScopeFacade.is_debug)
        {
            Thread mon_mem = new MonMemory();
            mon_mem.start();
            JButton exec_gc = new JButton("Exec gc");
            exec_gc.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    System.gc();
                }
            }
            );
            panel1.add("West", exec_gc);
        }
        InitDataServer();
        UpdateFont();
        UpdateColors();

    }

    protected jScopeWaveContainer buildWaveContainer()
    {
        int rows[] =
            {
            1, 0, 0, 0};
        return (new jScopeWaveContainer(rows, def_values));
    }

    class MonMemory
        extends Thread
    {
        public void run()
        {
            setName("Monitor Thread");
            try
            {
                while (true)
                {

                    //System.out.println
                    SetWindowTitle("Free :" +
                                   (int) (Runtime.getRuntime().freeMemory() /
                                          1024) + " " +
                                   "Total :" +
                                   (int) (Runtime.getRuntime().totalMemory()) /
                                   1024 + " " +
                                   "USED :" +
                                   (int) ( (Runtime.getRuntime().totalMemory() -
                                            Runtime.getRuntime().freeMemory()) /
                                          1024.));
                    sleep(2000, 0);
                    //waitTime(2000);
                }
            }
            catch (InterruptedException e)
            {}
        }

        synchronized void waitTime(long t) throws InterruptedException
        {
            wait(t);
        }
    }

    public void SetApplicationFonts(String font, int style, int size)
    {
        SetApplicationFonts(new Font(font, style, size));
    }

    public void SetApplicationFonts(Font font)
    {
        //int fontSize=9;
        //Font userEntryFont = new Font("Dialog", Font.PLAIN, fontSize);
        //Font defaultFont = new Font("Dialog", Font.PLAIN, fontSize);
        //Font boldFont = new Font("Dialog", Font.BOLD, fontSize);
        Font userEntryFont = font;
        Font defaultFont = font;
        Font boldFont = font;
        //writeToFile("c:\\temp\\outFile.txt",String.valueOf(UIManager.getDefaults())); // write out defaults
        // User entry widgets
        UIManager.put("Text.font", new FontUIResource(userEntryFont));
        UIManager.put("TextField.font", new FontUIResource(userEntryFont));
        UIManager.put("TextArea.font", new FontUIResource(userEntryFont));
        UIManager.put("TextPane.font", new FontUIResource(userEntryFont));
        UIManager.put("List.font", new FontUIResource(userEntryFont));
        UIManager.put("Table.font", new FontUIResource(userEntryFont));
        UIManager.put("ComboBox.font", new FontUIResource(userEntryFont));
        // Non-user entry widgets
        UIManager.put("Button.font", new FontUIResource(defaultFont));
        UIManager.put("Label.font", new FontUIResource(defaultFont));
        UIManager.put("Menu.font", new FontUIResource(defaultFont));
        UIManager.put("MenuItem.font", new FontUIResource(defaultFont));
        UIManager.put("ToolTip.font", new FontUIResource(defaultFont));
        UIManager.put("ToggleButton.font", new FontUIResource(defaultFont));
        UIManager.put("TitledBorder.font", new FontUIResource(boldFont));
        UIManager.put("PopupMenu.font", new FontUIResource(defaultFont));
        UIManager.put("TableHeader.font", new FontUIResource(defaultFont));
        UIManager.put("PasswordField.font", new FontUIResource(defaultFont));
        UIManager.put("CheckBoxMenuItem.font", new FontUIResource(defaultFont));
        UIManager.put("CheckBox.font", new FontUIResource(defaultFont));
        UIManager.put("RadioButtonMenuItem.font",
                      new FontUIResource(defaultFont));
        UIManager.put("RadioButton.font", new FontUIResource(defaultFont));
        // Containters
        UIManager.put("ToolBar.font", new FontUIResource(defaultFont));
        UIManager.put("MenuBar.font", new FontUIResource(defaultFont));
        UIManager.put("Panel.font", new FontUIResource(defaultFont));
        UIManager.put("ProgressBar.font", new FontUIResource(defaultFont));
        UIManager.put("TextPane.font", new FontUIResource(defaultFont));
        UIManager.put("OptionPane.font", new FontUIResource(defaultFont));
        UIManager.put("ScrollPane.font", new FontUIResource(defaultFont));
        UIManager.put("EditorPane.font", new FontUIResource(defaultFont));
        UIManager.put("ColorChooser.font", new FontUIResource(defaultFont));
        jScopeUpdateUI();
    }

    public void InvalidateDefaults()
    {
        wave_panel.InvalidateDefaults();
    }

    public static String findFileInClassPath(String file)
    {
        StringTokenizer path = new StringTokenizer(System.getProperty(
            "java.class.path"), File.pathSeparator);
        String p, f_name;
        File f;
        while (path.hasMoreTokens())
        {
            p = path.nextToken();
            f = new File(p);
            if (!f.isDirectory())
                continue;
            f_name = p + File.separator + file;
            f = new File(f_name);
            if (f.exists())
                return f_name;
        }
        return null;
    }

    public void setPropertiesFile(String propFile)
    {
        propertiesFilePath = propFile;
    }

    public void InitProperties( )
    {
        String f_name = propertiesFilePath;
        if(f_name == null)
        {
            f_name = System.getProperty("user.home") + File.separator +
            "jScope" + File.separator + "jScope.properties";
        }
        
        try
        {
            if (jScopeFacade.is_debug)
            {

                System.out.println("jScope.properties " +
                                   System.getProperty("jScope.properties"));
                System.out.println("jScope.config_directory " +
                                   System.getProperty("jScope.config_directory"));
            }

            if ( ( (new File(f_name)).exists()) ||
                (f_name = System.getProperty("jScope.properties")) != null)
            {
                js_prop = new Properties();
                js_prop.load(new FileInputStream(f_name));
            }
            else
            {
                f_name = System.getProperty("user.home") + File.separator +
                    "jScope" + File.separator;
                File jScopeUserDir = new File(f_name);
                if (!jScopeUserDir.exists())
                    jScopeUserDir.mkdirs();
                f_name = f_name + "jScope.properties";

                js_prop = new Properties();
                InputStream pis = getClass().getClassLoader().
                    getResourceAsStream("jScope.properties");
                if (pis != null)
                {
                    js_prop.load(pis);
                    pis.close();

                    pis = getClass().getClassLoader().getResourceAsStream(
                        "jScope.properties");
                    f_name = System.getProperty("user.home") + File.separator +
                        "jScope" + File.separator + "jScope.properties";
                    FileOutputStream fos = new FileOutputStream(f_name);
                    byte b[] = new byte[1024];
                    for (int len = pis.read(b); len > 0; len = pis.read(b))
                        fos.write(b, 0, len);
                    fos.close();
                    pis.close();
                }
                else
                {
                    System.out.println("Not found jScope.properties file");
                }
             }

             propertiesFilePath = new String(f_name);

             f_name = System.getProperty("user.home") + File.separator +
                     "jScope" + File.separator + "jScope_servers.conf";

             if( ! (new File(f_name)).exists() )
             {

                InputStream pis = getClass().getClassLoader().
                    getResourceAsStream("jScope_servers.conf");
                if (pis != null)
                {
                    FileOutputStream fos = new FileOutputStream(f_name);
                    byte b[] = new byte[1024];
                    for (int len = pis.read(b); len > 0; len = pis.read(b))
                        fos.write(b, 0, len);
                    fos.close();
                    pis.close();
                }
            }             
        }

        catch (FileNotFoundException e)
        {
            System.out.println(e);
        }
        catch (IOException e)
        {
            System.out.println(e);
        }
    }

    protected void GetPropertiesValue()
    {
        if (js_prop == null)
            return;

        //jScope configurations file directory can be defined
        //with decrease priority order:
        // 1) by system property jScope.config_directory;
        //    in this case jScope must be started with
        //    -DjScope.config_directory=<directory> option.
        // 2) in jScope.properties using jScope.directory property
        //If the previous properties are not defined jScope create
        //configuration folder in <home directory>/jScope/configurations, if
        //for same abnormal reason the directory creation failed
        //<home directory> is used as configuration directory

        curr_directory = System.getProperty("jScope.config_directory");

        if (curr_directory == null || curr_directory.trim().length() == 0)
        {
            curr_directory = js_prop.getProperty("jScope.directory");

            if (curr_directory == null || curr_directory.trim().length() == 0)
            {
                //Store default jScope configuration file in local
                //directory <home directory>/jScope/configurations.
                //Default configuration are stored in jScope jar file.
                //If configuration directory already exist the configurations
                //copy is not performed.
                try
                {
                    curr_directory = System.getProperty("user.home") +
                        File.separator + "jScope" + File.separator +
                        "configurations" + File.separator;
                    File jScopeUserDir = new File(curr_directory);
                    if (!jScopeUserDir.exists())
                    {
                        byte b[] = new byte[1024];

                        jScopeUserDir.mkdirs();

                        String configList[] =
                            {
                            "FTU_plasma_current.jscp",
                            "fusion.jscp",
                            "JET_plasma_current.jscp",
                            "RFX_plasma_current.jscp",
                            "TS_plasma_current.jscp",
                            "TWU_plasma_current.jscp"};

                        for (int i = 0; i < configList.length; i++)
                        {
                            InputStream fis = getClass().getClassLoader().
                                getResourceAsStream("configurations/" +
                                configList[i]);
                            FileOutputStream fos = new FileOutputStream(
                                curr_directory + configList[i]);
                            for (int len = fis.read(b); len > 0;
                                 len = fis.read(b))
                                fos.write(b, 0, len);
                            fos.close();
                            fis.close();
                        }
                    }
                }
                catch (Exception exc)
                {
                    curr_directory = System.getProperty("user.home");
                }
            }
        }

        default_server_idx = -1;
        String prop = js_prop.getProperty("jScope.default_server");
        if (prop != null)
        {
            try
            {
                default_server_idx = Integer.parseInt(prop) - 1;
            }
            catch (NumberFormatException e)
            {}
        }

        String cache_directory = js_prop.getProperty("jScope.cache_directory");
        String cache_size = js_prop.getProperty("jScope.cache_size");
        String f_name = js_prop.getProperty("jScope.save_selected_points");
        String proxy_host = js_prop.getProperty("jScope.http_proxy_host");
        String proxy_port = js_prop.getProperty("jScope.http_proxy_port");

        prop = js_prop.getProperty("jScope.vertical_offset");
        if (prop != null) {
            int val = 0;
            try {
                val = Integer.parseInt(prop);
            } catch (NumberFormatException e) {}
            Waveform.SetVerticalOffset(val);
        }

        prop = js_prop.getProperty("jScope.horizontal_offset");
        if (prop != null) {
            int val = 0;
            try {
                val = Integer.parseInt(prop);
            } catch (NumberFormatException e) {}
            Waveform.SetHorizontalOffset(val);
        }

        Properties p = System.getProperties();
        if (cache_directory != null)
            p.put("Signal.cache_directory", cache_directory);
        if (cache_size != null)
            p.put("Signal.cache_size", cache_size);
        if (f_name != null)
            p.put("jScope.save_selected_points", f_name);
        if (curr_directory != null)
            p.put("jScope.curr_directory", curr_directory);

        if (proxy_port != null && proxy_host != null)
        {
            p.setProperty("http.proxyHost", proxy_host);
            p.setProperty("http.proxyPort", proxy_port);
        }

        String timeConversion = js_prop.getProperty("jScope.time_format");
        if(timeConversion != null)
        {
            if(timeConversion.toUpperCase().equals("VMS"))
                timeMode = VMS_TIME;
            else if(timeConversion.toUpperCase().equals("EPICS"))
                timeMode = EPICS_TIME;
            //Add here new time formats
        }
        String refreshPeriodStr = js_prop.getProperty("jScope.refresh_period");
        if(refreshPeriodStr != null)
        {
            try {
                refreshPeriod = Integer.parseInt(refreshPeriodStr);
                //Not shorted than 0.5 s
                if(refreshPeriod < 500) refreshPeriod = 500;
            } catch(Exception exc)
            {
                refreshPeriod = -1;
            }
        }
    }

    public static long getRefreshPeriod() {return refreshPeriod;}

    private void InitDataServer()
    {
        String ip_addr = null;
        String dp_class = null;
        DataServerItem srv_item = null;

        Properties props = System.getProperties();
        ip_addr = props.getProperty("data.address");
        dp_class = props.getProperty("data.class");
        server_diag = new ServerDialog(this, "Server list");

        if (ip_addr != null && dp_class != null) //|| is_local == null || (is_local != null && is_local.equals("no")))
        {
            srv_item = new DataServerItem(ip_addr, ip_addr, null, dp_class, null, null, null, false);
            //Add server to the server list and if presente browse class and
            //url browse signal set it into srv_item
            server_diag.addServerIp(srv_item);
        }

        if (srv_item == null || !SetDataServer(srv_item))
        {
            srv_item = wave_panel.DataServerFromClient(srv_item);
            if (srv_item == null || !SetDataServer(srv_item))
            {
                if (server_ip_list != null && default_server_idx >= 0 &&
                    default_server_idx < server_ip_list.length)
                {
                    srv_item = server_ip_list[default_server_idx];
                    SetDataServer(srv_item);
                }
                else
                    setDataServerLabel();
            }
        }
    }

    public void setDataServerLabel()
    {
        net_text.setText("Data Server:" + wave_panel.GetServerLabel());
    }

    public void SetStatusLabel(String msg)
    {
        info_text.setText(" Status: " + msg);
    }

    public void RepaintAllWaves()
    {
        int wave_mode = wave_panel.GetMode();
        jScopeMultiWave w;

        wave_panel.SetMode(Waveform.MODE_WAIT);

        for (int i = 0, k = 0; i < 4; i++)
        {
            for (int j = 0; j < wave_panel.getComponentsInColumn(i); j++, k++)
            {
                w = (jScopeMultiWave) wave_panel.GetWavePanel(k);
                if (w.wi != null)
                {
                    SetStatusLabel("Repaint signal column " + (i + 1) + " row " +
                                   (j + 1));
                    setColor(w.wi);
                    w.Update(w.wi);
                }
            }
        }
        wave_panel.RepaintAllWaves();
        wave_panel.SetMode(wave_mode);
    }

    public boolean briefError()
    {
        return brief_error_i.getState();
    }

    public boolean IsShotDefinedXX()
    {
        String s = shot_t.getText();
        
        if ( s != null && s.trim().length() > 0)
            return true;
        s = def_values.shot_str;
        if (s != null && s.trim().length() > 0)
        {
            shot_t.setText(s);
            return true;
        }
        return false;
    }

    public boolean equalsString(String s1, String s2)
    {
        boolean res = false;

        if (s1 != null)
        {
            if (s2 == null && s1.length() == 0)
                res = true;
            else
            if (s1.equals(s2))
                res = true;
        }
        else
        {
            if (s2 != null && s2.length() != 0)
                res = false;
            else
                res = true;
        }
        return res;
    }

    public void setPublicVariables(String public_variables)
    {
            def_values.setPublicVariables(public_variables);
            if( ! def_values.getIsEvaluated() )
            //def_values.setIsEvaluated(false);
                //Force update in all waveform
                wave_panel.SetModifiedState(true);
    }


    public void ArrowsIncDecShot()
    {
        int idx;
        String sh =  shot_t.getText();

           if ( (idx = sh.lastIndexOf("+")) > 1 ||
               (idx = sh.lastIndexOf("-")) > 1)
           {
               sh = sh.substring(0, idx);
               sh = sh.trim();
           }

           if (incShotValue != 0)
               shot_t.setText(sh + (incShotValue > 0 ? " + " : " - ") +
                              Math.abs(incShotValue));
           else
               shot_t.setText(sh);


    }

    public void SetMainShot()
    {
        wave_panel.SetMainShot(shot_t.getText());
    }

    public void UpdateAllWaves()
    {
        
        String s = shot_t.getText();
        String s1 = def_values.shot_str;
        
        // Set main shot text field with 
        // global setting shot if defined.  
        if ( ( s == null || s.trim().length() == 0 ) && 
              ( s1 != null && s1.trim().length() !=  0 ) ) 
            shot_t.setText(s1);
        
        executing_update = true;
        apply_b.setText("Abort");
        setPublicVariables(pub_var_diag.getPublicVar());
        SetMainShot();
        wave_panel.StartUpdate();
    }

    private void ToFile(PrintWriter out) throws IOException
    {
        Rectangle r = getBounds();
        setChange(false);
        SetWindowTitle("");
        out.println("Scope.geometry: " + r.width + "x" + r.height + "+" + r.x +
                    "+" + r.y);
        out.println("Scope.update.disable: " + update_i.getState());
        out.println("Scope.update.disable_when_icon: " +
                    update_when_icon_i.getState());

        font_dialog.toFile(out, "Scope.font");

        pub_var_diag.toFile(out, "Scope.public_variable_");

        color_dialog.toFile(out, "Scope.color_");

        wave_panel.ToFile(out, "Scope.");
    }

    public void setChange(boolean change)
    {
        if (modified == change)
            return;
        modified = change;
        this.SetWindowTitle("");
    }

    public boolean IsChange()
    {
        return modified;
    }

    private void creaHistoryFile(File f)
    {
        int idx = 0, maxIdx = 0;
        int maxHistory = 2;

        String config_file_history = js_prop.getProperty("jScope.config_file_history_length");
        try {
            maxHistory = Integer.parseInt(config_file_history);
        } catch (Exception exc){};


        File pf = f.getParentFile();
        String list[] = pf.list(new FileFilter(f.getName()));

        for (String file : list) {
            StringTokenizer st = new StringTokenizer(file, ";");
            try {
                String s = st.nextToken();
                s = st.nextToken();
                if (s != null)
                {
                    idx = Integer.parseInt(s);
                    if (idx > maxIdx)
                        maxIdx = idx;
                }
            } catch (Exception exc){};
        }

        maxIdx++;

        if( maxIdx  > maxHistory)
        {
            File fd = new File(f.getAbsolutePath() + ";" + (maxIdx - maxHistory));
            fd.delete();
        }

        File fr = new File(f.getAbsolutePath() + ";" + maxIdx);

        f.renameTo(fr);

    }

    public void SaveConfiguration(String conf_file)
    {
        PrintWriter out;
        File ftmp, fok;

        if (conf_file == null || conf_file.length() == 0)
            return;
        int pPos = conf_file.lastIndexOf('.');
        int sPos = conf_file.lastIndexOf(File.separatorChar);
        if ( pPos == -1 || pPos <  sPos  )
                conf_file = conf_file + ".jscp";

        last_directory = new String(conf_file);
        save_i.setEnabled(true);
        use_last_i.setEnabled(true);

        fok = new File(conf_file);
        ftmp = new File(conf_file + "_tmp");

        try
        {
            out = new PrintWriter(new FileWriter(ftmp));
            ToFile(out);
            out.close();
            if (fok.exists())
                 creaHistoryFile(fok);
            ftmp.renameTo(fok);
        }
        catch (Exception e)
        {
			JOptionPane.showMessageDialog(this,
										  e,
                                          "alert",
                                          JOptionPane.ERROR_MESSAGE);

        }
        ftmp.delete();
    }

    private int saveWarning()
    {
        Object[] options =
            {
            "Save",
            "Don't Save",
            "Cancel"};
        int val = JOptionPane.showOptionDialog(null,
            "Save change to the configuration file before closing ?",
            "Warning",
            JOptionPane.YES_NO_CANCEL_OPTION,
            JOptionPane.QUESTION_MESSAGE,
            null,
            options,
            options[0]);

        return val;

    }

    public void closeScope()
    {
        if (IsChange())
        {
            switch (saveWarning())
            {
                case JOptionPane.YES_OPTION:
                    if (config_file == null)
                        SaveAs();
                    else
                        SaveConfiguration(config_file);
                    break;
                case JOptionPane.NO_OPTION:

//		        exitScope();
                    break;
            }
        }
//	else
//	{
//        exitScope();
//	}
        exitScope();
    }

    private void exitScope()
    {
        try
        {
            wave_panel.RemoveAllEvents(this);
        }
        catch (IOException e)
        {}
        dispose();
        num_scope--;
        //System.gc();
    }

    private void SaveAs()
    {
        if (curr_directory != null && curr_directory.trim().length() != 0)
            file_diag.setCurrentDirectory(new File(curr_directory));

        int returnVal = JFileChooser.CANCEL_OPTION;
        boolean done = false;

        while (!done)
        {
            returnVal = file_diag.showSaveDialog(jScopeFacade.this);
            if (returnVal == JFileChooser.APPROVE_OPTION)
            {
                File file = file_diag.getSelectedFile();
                String txtsig_file = file.getAbsolutePath();

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
            File file = file_diag.getSelectedFile();
            String d = file.getAbsolutePath();
            String f = file.getName();
            if (f != null && f.trim().length() != 0 &&
                d != null && d.trim().length() != 0)
            {
                curr_directory = d;
                config_file = curr_directory;
            }
            else
                config_file = null;
            if (config_file != null)
            {
                SaveConfiguration(config_file);
            }
        }
        //         }
        //      });
        //      tim.setRepeats(false);
        //      tim.start();
    }

    private void LoadConfigurationFrom()
    {
        if (IsChange())
        {
            switch (saveWarning())
            {
                case JOptionPane.YES_OPTION:
                    if (config_file == null)
                    {
                        SaveAs();
                        return;
                    }
                    else
                        SaveConfiguration(config_file);
                    break;
                case JOptionPane.CANCEL_OPTION:
                    return;

            }
        }
        setChange(false);

        if (curr_directory != null && curr_directory.trim().length() != 0)
            file_diag.setCurrentDirectory(new File(curr_directory));

  
        javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener()
        {
            public void actionPerformed(ActionEvent ae)
            {

                int returnVal = file_diag.showOpenDialog(jScopeFacade.this);
                if (returnVal == JFileChooser.APPROVE_OPTION)
                {
                    File file = file_diag.getSelectedFile();
                    String d = file.getAbsolutePath();
                    String f = file.getName();
                    if (f != null && f.trim().length() != 0 && d != null &&
                        d.trim().length() != 0)
                    {
                        curr_directory = d;
                        config_file = curr_directory;
                        LoadConfiguration();
                    }
                }
            }
        });
        t.setRepeats(false);
        t.start();
    }

    public boolean SetDataServer(DataServerItem new_srv_item)
    {
        try
        {
            wave_panel.SetDataServer(new_srv_item, this);

            wave_panel.SetCacheState(new_srv_item.enable_cache);
            setDataServerLabel();

            return true;
        }
        catch (Exception e)
        {
            JOptionPane.showMessageDialog(null, e.getMessage(), "alert SetDataServer",
                                          JOptionPane.ERROR_MESSAGE);

            setDataServerLabel();

        }
        return false;
    }

    public void UpdateFont()
    {
        wave_panel.SetFont(font_dialog.GetFont());
    }

    public void UpdateColors()
    {
        wave_panel.SetColors(color_dialog.GetColors(),
                             color_dialog.GetColorsName());
        this.setup_dialog.SetColorList();
    }

    public void UpdateDefaultValues()
    {

        boolean is_changed = false;

        try
        {

            if ( (is_changed = setup_default.IsChanged(def_values)))
            {
                this.setChange(true);
                wave_panel.RemoveAllEvents(this);
                setup_default.SaveDefaultConfiguration(def_values);
                InvalidateDefaults();
                wave_panel.AddAllEvents(this);
                UpdateAllWaves();
            }
            else
                setup_default.SaveDefaultConfiguration(def_values);

            wave_panel.SetParams(wave_panel.GetMode(),
                                 setup_default.getGridMode(),
                                 setup_default.getLegendMode(),
                                 setup_default.getXLines(),
                                 setup_default.getYLines(),
                                 setup_default.getReversed());

            color_dialog.SetReversed(setup_default.getReversed());

            UpdateColors();

            if (!is_changed)
                RepaintAllWaves();

        }
        catch (IOException e)
        {
            JOptionPane.showMessageDialog(null, e.getMessage(), "alert UpdateDefaultValues",
                                          JOptionPane.ERROR_MESSAGE);
        }
    }

    protected void PrintAllWaves(PrintRequestAttributeSet attrs)
    {
        try
        {
            this.SetStatusLabel("Executing print");
            wave_panel.PrintAllWaves(prnJob, attrs);
            SetStatusLabel("End print operation");
        }
        catch (PrinterException er)
        {
			System.out.println(er);
            JOptionPane.showMessageDialog(null, "Error on print operation",
                                          "alert PrintAllWaves", JOptionPane.ERROR_MESSAGE);
        }
        catch (PrintException er)
        {
			System.out.println(er);
            JOptionPane.showMessageDialog(null, "Error on print operation",
                                          "alert PrintAllWaves", JOptionPane.ERROR_MESSAGE);
        }
    }

    public void processWaveContainerEvent(WaveContainerEvent e)
    {
        String s = null;
        int event_id = e.getID();

        switch (event_id)
        {
            case WaveContainerEvent.END_UPDATE:
            case WaveContainerEvent.KILL_UPDATE:
                apply_b.setText("Apply");
                executing_update = false;
                if (event_id == WaveContainerEvent.KILL_UPDATE)
                {
/*
                            JOptionPane.showMessageDialog(this, e.info,
                                "alert processWaveContainerEvent",
                                JOptionPane.ERROR_MESSAGE);
*/
                    System.out.println(" processWaveContainerEvent " +  e.info);
                    SetStatusLabel(" Aborted ");
                }
                else
                    SetStatusLabel(e.info);
                SetWindowTitle("");
                break;
            case WaveContainerEvent.START_UPDATE:
                SetStatusLabel(e.info);
                break;
            case WaveContainerEvent.WAVEFORM_EVENT:

                WaveformEvent we = (WaveformEvent) e.we;
                jScopeMultiWave w = (jScopeMultiWave) we.getSource();
                MdsWaveInterface wi = (MdsWaveInterface) w.wi;
                int we_id = we.getID();

                switch (we_id)
                {
                    case WaveformEvent.EVENT_UPDATE:
                        if (EventUpdateEnabled())
                        {
                            setPublicVariables(pub_var_diag.getPublicVar());
                            SetMainShot();
                            /*
                            wave_panel.Refresh(w, we.status_info);
                            */
                            w.RefreshOnEvent();
                            //??????*******
                        }
                        break;
                    case WaveformEvent.MEASURE_UPDATE:
                    case WaveformEvent.POINT_UPDATE:
                    case WaveformEvent.POINT_IMAGE_UPDATE:
                        s = we.toString();
                        if (wi.shots != null)
                        {
                            point_pos.setText(s +
                                              " Expr : " +
                                              w.getSignalName(we.signal_idx) +
                                              " Shot = " +
                                              wi.shots[we.signal_idx]);
                        }
                        else
                        {
                            point_pos.setText(s +
                                              " Expr : " +
                                              w.getSignalName(we.signal_idx));
                        }
                        break;
                    case WaveformEvent.STATUS_INFO:
                        SetStatusLabel(we.status_info);
                        break;
                    case WaveformEvent.CACHE_DATA:
                        if (we.status_info != null)
                            progress_bar.setString(we.status_info);
                        else
                            progress_bar.setString("");
                        progress_bar.setValue(0);
                        break;
                }
                break;
        }
    }
    public void processConnectionEvent(ConnectionEvent e)
    {
        progress_bar.setString("");

        if (e.getID() == ConnectionEvent.LOST_CONNECTION)
        {

            JOptionPane.showMessageDialog(this,
                                          e.info,
                                          "alert processConnectionEvent",
                                          JOptionPane.ERROR_MESSAGE);

            SetDataServer(new DataServerItem("Not Connected", null, null,
                                             "NotConnectedDataProvider", null, null, null, false));
            return;
        }

        if (e.current_size == 0 && e.total_size == 0)
        {
            if (e.info != null)
                progress_bar.setString(e.info);
            else
                progress_bar.setString("");
            progress_bar.setValue(0);
        }
        else
        {
            int v = (int) ( (float) e.current_size / e.total_size * 100.);
            progress_bar.setValue(v);
            progress_bar.setString( (e.info != null ? e.info : "") + v + "%");
        }
    }

    private boolean EventUpdateEnabled()
    {
        if (update_i.getState())
        {
            SetStatusLabel("Disable event update");
            return false;
        }
        if (getExtendedState() == JFrame.ICONIFIED &&
            update_when_icon_i.getState())
        {
            this.SetStatusLabel("Event update is disabled when iconified");
            return false;
        }
        return true;
    }

    public void processUpdateEvent(UpdateEvent e)
    {
        if (EventUpdateEnabled())
        {
            String print_event = wave_panel.GetPrintEvent();
            String event = wave_panel.GetEvent();

            if (e.name.equals(event))
            {
               SwingUtilities.invokeLater(new Runnable() {public void run() {UpdateAllWaves();}});
            }
            //wave_panel.StartUpdate();

            if (e.name.equals(print_event))
                wave_panel.StartPrint(prnJob, attrs);
        }
    }

    private DataServerItem getServerItem(String server)
    {
        for (int i = 0; i < server_ip_list.length; i++)
            if (server_ip_list[i].equals(server))
                return server_ip_list[i];
        return null;
    }

    public void actionPerformed(ActionEvent e)
    {

        Object ob = e.getSource();
        String action_cmd = null;

        if (ob != open_i)
            wave_panel.RemoveSelection();

        if (ob instanceof AbstractButton)
            action_cmd = ( (AbstractButton) ob).getModel().getActionCommand();

        if (action_cmd != null)
        {
            StringTokenizer act = new StringTokenizer(action_cmd);
            String action = act.nextToken();

            if (action.equals("SET_SERVER"))
            {
                String value = action_cmd.substring(action.length() + 1);
                if (!wave_panel.GetServerLabel().equals(value))
                {
                    SetDataServer(getServerItem(value));
                }
            }
        }

        if (ob == signal_expr)
        {
            String sig = signal_expr.getText().trim();

            if (sig != null && sig.length() != 0)
            {
                SetMainShot();
                wave_panel.AddSignal(sig, false);
                setChange(true);
            }
        }

        if (ob == apply_b || ob == shot_t)
        {
            incShotValue = 0;

            if (executing_update)
            {
                if (ob == apply_b)
                    wave_panel.AbortUpdate();
            }
            else
            {
                if (ob == shot_t)
                {
                    SetMainShot();
                }

                String sig = signal_expr.getText().trim();
                if (sig != null && sig.length() != 0)
                {
                    wave_panel.AddSignal(sig, true);
                    setChange(true);
                }
                UpdateAllWaves();

            }
        }

        if (ob == color_i)
        {
            javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener()
            {
                public void actionPerformed(ActionEvent ae)
                {
                    jScopeFacade.this.color_dialog.ShowColorDialog(jScopeFacade.this.
                        wave_panel);
                }
            });
            t.setRepeats(false);
            t.start();
        }

        if (ob == font_i)
        {
            font_dialog.setLocationRelativeTo(this);
            javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener()
            {
                public void actionPerformed(ActionEvent ae)
                {
                    font_dialog.setVisible(true);
                }
            });
            t.setRepeats(false);
            t.start();
        }

        if (ob == default_i)
        {

            javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener()
            {
                public void actionPerformed(ActionEvent ae)
                {
                    setup_default.Show(jScopeFacade.this, jScopeFacade.this.def_values);
                }
            });
            t.setRepeats(false);
            t.start();
        }

        if (ob == win_i)
        {

            if (win_diag == null)
                win_diag = new WindowDialog(this, "Window");

            javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener()
            {
                public void actionPerformed(ActionEvent ae)
                {
                    boolean returnFlag = win_diag.ShowWindowDialog();
                    if (returnFlag)
                    {
                        wave_panel.ResetDrawPanel(win_diag.out_row);
//	                wave_panel.update();
                        UpdateColors();
                        UpdateFont();
                        setChange(true);
                    }
                }
            });
            t.setRepeats(false);
            t.start();
        }

        if (ob == use_last_i)
        {
            if (last_directory != null && last_directory.trim().length() != 0)
            {
                curr_directory = last_directory;
                config_file = curr_directory;
                setChange(false);
                LoadConfiguration();
            }
        }

        if (ob == use_i)
        {

            LoadConfigurationFrom();
        }

        if (ob == save_i)
        {
            SaveConfiguration(config_file);
        }

        if (ob == save_as_i)
        {
            SaveAs();
        }

        if (ob == save_all_as_text_i)
        {
            wave_panel.SaveAsText(null, true);
        }

        if (ob == exit_i)
        {
            if (num_scope > 1)
            {
                Object[] options =
                    {
                    "Close this",
                    "Close all",
                    "Cancel"};
                int opt = JOptionPane.showOptionDialog(null,
                    "Close all open scopes?",
                    "Warning",
                    JOptionPane.YES_NO_CANCEL_OPTION,
                    JOptionPane.QUESTION_MESSAGE,
                    null,
                    options,
                    options[0]);
                switch (opt)
                {
                    case JOptionPane.YES_OPTION:
                        closeScope();
                        break;
                    case JOptionPane.NO_OPTION:
                        System.exit(0);
                        break;
                    case JOptionPane.CANCEL_OPTION:
                        break;
                }
            }
            else
            {
                closeScope();
                if (num_scope == 0)
                    System.exit(0);
                //System.gc();
            }
        }

        if (ob == close_i && num_scope != 1)
        {
            closeScope();
        }

        if (ob == open_i)
        {
            num_scope++;
            Rectangle r = getBounds();
            //jScope new_scope = new jScope(r.x+5, r.y+40);
            jScopeFacade new_scope = buildNewScope(r.x + 5, r.y + 40);
            new_scope.wave_panel.SetCopySource(wave_panel.GetCopySource());
            new_scope.startScope(null);
        }

        if (ob == all_i)
        {
            wave_panel.AutoscaleAll();
        }

        if (ob == allY_i)
        {
            wave_panel.AutoscaleAllY();
        }

        if (ob == copy_i)
        {
            wave_panel.SetMode(Waveform.MODE_COPY);
            copy.getModel().setSelected(true);
        }

        if (ob == zoom_i)
        {
            wave_panel.SetMode(Waveform.MODE_ZOOM);
            zoom.getModel().setSelected(true);
        }

        if (ob == point_i)
        {
            wave_panel.SetMode(Waveform.MODE_POINT);
            point.getModel().setSelected(true);
        }

        if (ob == pan_i)
        {
            wave_panel.SetMode(Waveform.MODE_PAN);
            pan.getModel().setSelected(true);
        }

        if (ob == server_list_i)
        {
            javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener()
            {
                public void actionPerformed(ActionEvent ae)
                {
                    server_diag.Show();
                    server_ip_list = server_diag.getServerIpList();
                }
            });
            t.setRepeats(false);
            t.start();
        }

        if (ob == pub_variables_i)
        {
            javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener()
            {
                public void actionPerformed(ActionEvent ae)
                {
                    pub_var_diag.Show();
                }
            });
            t.setRepeats(false);
            t.start();
        }

    }

    private void setColor(WaveInterface wi)
    {
//	if(wi == null || wi.colors_idx == null) return;
//	for(int i = 0; i < wi.colors_idx.length; i++)
//	    wi.colors[i] = color_dialog.GetColorAt(wi.colors_idx[i]);
    }

     public void itemStateChanged(ItemEvent e)
    {

        Object ob = e.getSource();

        if (ob == brief_error_i)
        {
            WaveInterface.brief_error = brief_error_i.getState();
//	        wave_panel.SetBriefError(brief_error_i.getState());
        }

         if (e.getStateChange() != ItemEvent.SELECTED)
            return;

        if (ob == copy)
        {
            wave_panel.SetMode(Waveform.MODE_COPY);
        }

        if (ob == zoom)
        {
            wave_panel.SetMode(Waveform.MODE_ZOOM);
        }

        if (ob == point)
        {
            wave_panel.SetMode(Waveform.MODE_POINT);
        }

        if (ob == pan)
        {
            wave_panel.SetMode(Waveform.MODE_PAN);
        }

    }

    public void SetWindowTitle(String info)
    {

        String f_name = config_file;

        if (f_name == null)
            f_name = "Untitled";

        if (wave_panel.GetTitle() != null)
            setTitle(" - " + wave_panel.GetEvaluatedTitle() + " - " + f_name +
                     (IsChange() ? " (changed)" : "") + " " + info);
        else
            setTitle("- Scope - " + f_name + (IsChange() ? " (changed)" : "") +
                     " " + info);
    }

    public void FromFile(Properties pr) throws IOException
    {
        String prop = "";

        try
        {
            if ( (prop = pr.getProperty("Scope.update.disable")) != null)
            {
                boolean b = new Boolean(prop).booleanValue();
                update_i.setState(b);
            }

            if ( (prop = pr.getProperty("Scope.update.disable_when_icon")) != null)
            {
                boolean b = new Boolean(prop).booleanValue();
                update_when_icon_i.setState(b);
            }

            if ( (prop = pr.getProperty("Scope.geometry")) != null)
            {
                StringTokenizer st = new StringTokenizer(prop);
                width = Integer.parseInt(st.nextToken("x"));
                height = Integer.parseInt(st.nextToken("x+"));
                xpos = Integer.parseInt(st.nextToken("+"));
                ypos = Integer.parseInt(st.nextToken("+"));
                Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
                if (height > screenSize.height)
                    height = screenSize.height;
                if (width > screenSize.width)
                    width = screenSize.width;
                if (ypos + height > screenSize.height)
                    ypos = screenSize.height - height;
                if (xpos + width > screenSize.width)
                    xpos = screenSize.width - width;
            }

        }
        catch (Exception exc)
        {
            throw (new IOException(prop));
        }
    }

    public void LoadFromFile(Properties pr) throws IOException
    {
        try
        {
            FromFile(pr);
            font_dialog.fromFile(pr, "Scope.font");
            color_dialog.FromFile(pr, "Scope.color_");
            pub_var_diag.fromFile(pr, "Scope.public_variable_");
            wave_panel.FromFile(pr, "Scope", color_dialog.getColorMapIndex(), colorMapDialog);
        }
        catch (Exception e)
        {
            throw (new IOException("Configuration file syntax error : " +
                                   e.getMessage()));
        }
    }

    private void Reset()
    {
        config_file = null;
        incShotValue = 0;
        SetWindowTitle("");
        wave_panel.Reset();
    }

    public void LoadConfiguration()
    {
        if (config_file == null)
            return;

        incShotValue = 0;

        try
        {
            jScopeProperties pr = new jScopeProperties();
            pr.load(new FileInputStream(config_file));
            LoadConfiguration(pr);
        }
        catch (IOException e)
        {
            Reset();
            JOptionPane.showMessageDialog(this, e.getMessage(), "alert LoadConfiguration",
                                          JOptionPane.ERROR_MESSAGE);
        }
        save_i.setEnabled(true);
    }

    public void LoadConfiguration(Properties pr)
    {

        wave_panel.EraseAllWave();
        try
        {
            LoadFromFile(pr);
            setBounds(xpos, ypos, width, height);
            UpdateColors();
            UpdateFont();
            wave_panel.update();
            validate();
            DataServerItem dsi = wave_panel.GetServerItem();
            dsi = server_diag.addServerIp(dsi);
/*
remove 28/06/2005
            wave_panel.SetServerItem(dsi);
*/
            
                
            if( ! SetDataServer(dsi) )
                SetDataServer(new DataServerItem("Not Connected", null, null,
                                             "NotConnectedDataProvider", null, null, null, false));
          //SetFastNetworkState(wave_panel.GetFastNetworkState());
            UpdateAllWaves();
        }
        catch (Exception e)
        {
            Reset();
            JOptionPane.showMessageDialog(this, e.getMessage(),
                                          "alert LoadConfiguration", JOptionPane.ERROR_MESSAGE);
        }
        //SetWindowTitle("");
        //System.gc();
    }

    protected jScopeFacade buildNewScope(int x, int y)
    {
        return new jScopeFacade(x, y, propertiesFilePath);
    }

    public void startScope(String file)
    {
        if (file != null)
        {
            config_file = new String(file);
            LoadConfiguration();
        }
        SetWindowTitle("");
        setVisible(true);
    }

    /**********************
        jScope Main
     ***********************/

    static boolean IsNewJVMVersion()
    {
        String ver = System.getProperty("java.version");
        return (! (ver.indexOf("1.0") != -1 || ver.indexOf("1.1") != -1));
    }

    public static void main(String args[])
    {
        startApplication(args);
    }

    public static void startApplication(String args[])
    {
        String file = null;
        String propertiesFile = null;
 
        Properties props = System.getProperties();
        String debug = props.getProperty("debug");
        if (debug != null && debug.equals("true"))
        {
            is_debug = true;
            Waveform.is_debug = true;
        }

        if (args.length != 0)
        {
            for( int i = 0; i < args.length; i++)
            {
                if( args[i].equals("-fp"))
                {
                    if( i+1 < args.length )
                        propertiesFile = args[i+1];
                    i++;
                } else { 
                    file = new String(args[i]);
                }
            }
        }    
        if (IsNewJVMVersion())
            win = new jScopeFacade(100, 100, propertiesFile);
        else
        {
            System.out.println(
                "jScope application required JDK version 1.2 or later");
            System.exit(1);
        }

        win.pack();
        win.setSize(750, 550);

 
                
        
        win.num_scope++;
        win.startScope(file);
    }

    public void windowClosing(WindowEvent e)
    {
        if (num_scope == 1)
            System.exit(0);
        else
            exitScope();
    }

    public void windowOpened(WindowEvent e)
    {
        if (this.aboutScreen != null)
            hideAbout();
    }

    public void windowClosed(WindowEvent e)
    {
    }

    public void windowIconified(WindowEvent e)
    {
    }

    public void windowDeiconified(WindowEvent e)
    {
    }

    public void windowActivated(WindowEvent e)
    {
    }

    public void windowDeactivated(WindowEvent e)
    {
    }

/*    public void handleQuit()
    {
        System.exit(0);
    }
*/
    protected void jScopeUpdateUI()
    {
        jScopeFacade.jScopeSetUI(this);
        jScopeFacade.jScopeSetUI(font_dialog);
        jScopeFacade.jScopeSetUI(setup_default);
        jScopeFacade.jScopeSetUI(color_dialog);
        jScopeFacade.jScopeSetUI(pub_var_diag);
        jScopeFacade.jScopeSetUI(server_diag);
        jScopeFacade.jScopeSetUI(file_diag);
    }

    protected static void jScopeSetUI(Component c)
    {
        if (c != null)
            SwingUtilities.updateComponentTreeUI(c);
    }

    /**
     * A utility function that layers on top of the LookAndFeel's
     * isSupportedLookAndFeel() method. Returns true if the LookAndFeel
     * is supported. Returns false if the LookAndFeel is not supported
     * and/or if there is any kind of error checking if the LookAndFeel
     * is supported.
     * The L&F menu will use this method to detemine whether the various
     * L&F options should be active or inactive.
     *
     */
    protected static boolean isAvailableLookAndFeel(String classname)
    {
        try
        { // Try to create a L&F given a String
            Class lnfClass = Class.forName(classname);
            LookAndFeel newLAF = (LookAndFeel) (lnfClass.newInstance());
            return newLAF.isSupportedLookAndFeel();
        }
        catch (Exception e)
        { // If ANYTHING weird happens, return false
            return false;
        }
    }

    /**
     * Switch the between the Windows, Motif, Mac, and the Java Look and Feel
     */
    class ToggleUIListener
        implements ItemListener
    {
        public void itemStateChanged(ItemEvent e)
        {
            Component root = jScopeFacade.this;
            root.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
            JRadioButtonMenuItem rb = (JRadioButtonMenuItem) e.getSource();
            try
            {
                if (rb.isSelected() &&
                    rb.getText().equals("Windows Style Look and Feel"))
                {
                    //        currentUI = "Windows";
                    UIManager.setLookAndFeel(
                        "com.sun.java.swing.plaf.windows.WindowsLookAndFeel");
                    SwingUtilities.updateComponentTreeUI(jScopeFacade.this);
                }
                else if (rb.isSelected() &&
                         rb.getText().equals("Macintosh Look and Feel"))
                {
                    //                currentUI = "Macintosh";
                    UIManager.setLookAndFeel(
                        "com.sun.java.swing.plaf.mac.MacLookAndFeel");
                    SwingUtilities.updateComponentTreeUI(jScopeFacade.this);
                }
                else if (rb.isSelected() &&
                         rb.getText().equals("Motif Look and Feel"))
                {
                    //                        currentUI = "Motif";
                    UIManager.setLookAndFeel(
                        "com.sun.java.swing.plaf.motif.MotifLookAndFeel");
                    SwingUtilities.updateComponentTreeUI(jScopeFacade.this);
                }
                else if (rb.isSelected() &&
                         rb.getText().equals("Java Look and Feel"))
                {
                    //                                currentUI = "Metal";
                    UIManager.setLookAndFeel(
                        "javax.swing.plaf.metal.MetalLookAndFeel");
                    SwingUtilities.updateComponentTreeUI(jScopeFacade.this);
                }
                jScopeUpdateUI();
                /*
                                 jScope.jScopeSetUI(font_dialog);
                                 jScope.jScopeSetUI(setup_default);
                                 jScope.jScopeSetUI(color_dialog);
                                 jScope.jScopeSetUI(pub_var_diag);
                                 jScope.jScopeSetUI(server_diag);
                                 jScope.jScopeSetUI(file_diag);
                 */
            }
            catch (UnsupportedLookAndFeelException exc)
            {
                // Error - unsupported L&F
                rb.setEnabled(false);
                System.err.println("Unsupported LookAndFeel: " + rb.getText());

                // Set L&F to JLF
                try
                {
                    //       currentUI = "Metal";
                    metalMenuItem.setSelected(true);
                    UIManager.setLookAndFeel(UIManager.
                        getCrossPlatformLookAndFeelClassName());
                    SwingUtilities.updateComponentTreeUI(jScopeFacade.this);
                }
                catch (Exception exc2)
                {
                    exc2.printStackTrace();
                    System.err.println("Could not load LookAndFeel: " + exc2);
                    exc2.printStackTrace();
                }
            }
            catch (Exception exc)
            {
                rb.setEnabled(false);
                exc.printStackTrace();
                System.err.println("Could not load LookAndFeel: " + rb.getText());
                exc.printStackTrace();
            }

            root.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
        }
    }
}

class WindowDialog
    extends JDialog
    implements ActionListener
{

    JTextField titleText, eventText, printEventText;
    JSlider row_1, row_2, row_3, row_4;
    JButton ok, apply, cancel;
    jScopeFacade parent;
    JLabel label;
    int out_row[] = new int[4];
    int in_row[];
    boolean changed = false;

    WindowDialog(JFrame dw, String title)
    {

        super(dw, title, true);

        parent = (jScopeFacade) dw;

        GridBagConstraints c = new GridBagConstraints();
        GridBagLayout gridbag = new GridBagLayout();
        getContentPane().setLayout(gridbag);

        c.insets = new Insets(4, 4, 4, 4);
        c.fill = GridBagConstraints.BOTH;

        c.gridwidth = GridBagConstraints.REMAINDER;
        label = new JLabel("Rows in Column");
        gridbag.setConstraints(label, c);
        getContentPane().add(label);

        JPanel p = new JPanel();
        p.setLayout(new FlowLayout(FlowLayout.LEFT));
        c.gridwidth = GridBagConstraints.BOTH;

        row_1 = new JSlider(JSlider.VERTICAL, 1, 16, 1);
        //row_1.setMajorTickSpacing(4);
        row_1.setMinorTickSpacing(1);
        row_1.setPaintTicks(true);
        row_1.setPaintLabels(true);
        Hashtable<Integer, JLabel> labelTable = new Hashtable<>();
        labelTable.put(new Integer(1), new JLabel("1"));
        labelTable.put(new Integer(4), new JLabel("4"));
        labelTable.put(new Integer(8), new JLabel("8"));
        labelTable.put(new Integer(12), new JLabel("12"));
        labelTable.put(new Integer(16), new JLabel("16"));
        row_1.setLabelTable(labelTable);

        row_1.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 20));
        row_2 = new JSlider(JSlider.VERTICAL, 0, 16, 0);
        row_2.setMajorTickSpacing(4);
        row_2.setMinorTickSpacing(1);
        row_2.setPaintTicks(true);
        row_2.setPaintLabels(true);
        row_2.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 20));
        row_3 = new JSlider(JSlider.VERTICAL, 0, 16, 0);
        row_3.setMajorTickSpacing(4);
        row_3.setMinorTickSpacing(1);
        row_3.setPaintTicks(true);
        row_3.setPaintLabels(true);
        row_3.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 20));
        row_4 = new JSlider(JSlider.VERTICAL, 0, 16, 0);
        row_4.setMajorTickSpacing(4);
        row_4.setMinorTickSpacing(1);
        row_4.setPaintTicks(true);
        row_4.setPaintLabels(true);
        row_4.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 20));

        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(p, c);
        getContentPane().add(p);
        p.add(row_1);
        p.add(row_2);
        p.add(row_3);
        p.add(row_4);

        c.gridwidth = GridBagConstraints.BOTH;
        label = new JLabel("Title");
        gridbag.setConstraints(label, c);
        getContentPane().add(label);

        c.gridwidth = GridBagConstraints.REMAINDER;
        titleText = new JTextField(40);
        gridbag.setConstraints(titleText, c);
        getContentPane().add(titleText);

        c.gridwidth = GridBagConstraints.BOTH;
        label = new JLabel("Update event");
        gridbag.setConstraints(label, c);
        getContentPane().add(label);

        c.gridwidth = GridBagConstraints.REMAINDER;
        eventText = new JTextField(40);
        gridbag.setConstraints(eventText, c);
        getContentPane().add(eventText);

        c.gridwidth = GridBagConstraints.BOTH;
        label = new JLabel("Print event");
        gridbag.setConstraints(label, c);
        getContentPane().add(label);

        c.gridwidth = GridBagConstraints.REMAINDER;
        printEventText = new JTextField(40);
        gridbag.setConstraints(printEventText, c);
        getContentPane().add(printEventText);

        JPanel p1 = new JPanel();
        p1.setLayout(new FlowLayout(FlowLayout.CENTER));

        ok = new JButton("Ok");
        ok.addActionListener(this);
        p1.add(ok);

        cancel = new JButton("Cancel");
        cancel.addActionListener(this);
        p1.add(cancel);

        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(p1, c);
        getContentPane().add(p1);
        pack();
    }

    public boolean ShowWindowDialog()
    {
        changed = false;
        if (parent.wave_panel.GetTitle() != null)
            titleText.setText(parent.wave_panel.GetTitle());
        if (parent.wave_panel.GetEvent() != null)
            eventText.setText(parent.wave_panel.GetEvent());
        if (parent.wave_panel.GetPrintEvent() != null)
            printEventText.setText(parent.wave_panel.GetPrintEvent());

        int in_row[] = parent.wave_panel.getComponentsInColumns();

        row_1.setValue(in_row[0]);
        row_2.setValue(in_row[1]);
        row_3.setValue(in_row[2]);
        row_4.setValue(in_row[3]);

        this.in_row = new int[in_row.length];
        for (int i = 0; i < in_row.length; i++)
            this.in_row[i] = in_row[i];

        setLocationRelativeTo(parent);
        setVisible(true);

        return changed;
    }

    public void actionPerformed(ActionEvent e)
    {

        Object ob = e.getSource();

        try
        {

            if (ob == ok || ob == apply)
            {
                parent.wave_panel.SetTitle(new String(titleText.getText()));

                String event = new String(eventText.getText().trim());
                parent.wave_panel.SetEvent(parent, event);

                event = new String(printEventText.getText().trim());
                parent.wave_panel.SetPrintEvent(parent, event);

                parent.SetWindowTitle("");

                out_row[0] = row_1.getValue();
                out_row[1] = row_2.getValue();
                out_row[2] = row_3.getValue();
                out_row[3] = row_4.getValue();

                for (int i = 0; i < 4; i++)
                    if (out_row[i] != in_row[i])
                    {
                        changed = true;
                        break;
                    }

                in_row = null;

                if (ob == ok)
                    setVisible(false);

            }

            if (ob == cancel)
            {
                setVisible(false);
            }
        }
        catch (IOException ev)
        {
            JOptionPane.showMessageDialog(null, ev.getMessage(), "alert actionPerformed",
                                          JOptionPane.ERROR_MESSAGE);
        }
    }
}

class ServerDialog
    extends JDialog
    implements ActionListener
{
    private Hashtable<String, String> data_server_class = new Hashtable<>();
    static private JList<DataServerItem> server_list;
    private DefaultListModel<DataServerItem> list_model = new DefaultListModel<>();
    private JButton add_b, remove_b, exit_b, connect_b, modify_b;
    JLabel server_label, user_label;
    JTextField server_l, server_a, server_u;
    JCheckBox automatic;
    JComboBox<String> data_provider_list;

    JCheckBox tunneling;
    JTextField tunnel_port;

    jScopeFacade dw;
    private static String know_provider[] =
        {
        "MdsDataProvider",
        "MdsDataProviderUdt",
        "JetMdsDataProvider",
        "TwuDataProvider",
        "JetDataProvider",
        "FtuDataProvider",
        "TSDataProvider",
        "AsdexDataProvider",
        "ASCIIDataProvider",
        "T2DataProvider",
        "LocalDataProvider",
        "MdsAsynchDataProvider",
        "MDSplus.MdsStreamingDataProvider"};

    ServerDialog(JFrame _dw, String title)
    {
        super(_dw, title, true);
        dw = (jScopeFacade) _dw;
//	    setResizable(false);

        GridBagLayout gridbag = new GridBagLayout();
        GridBagConstraints c = new GridBagConstraints();
        Insets insets = new Insets(4, 4, 4, 4);

        getContentPane().setLayout(gridbag);
        c.insets = insets;

        c.anchor = GridBagConstraints.WEST;
        c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.BOTH;
        c.gridheight = 10;
        server_list = new JList<>(list_model);
        JScrollPane scrollServerList = new JScrollPane(server_list);
        server_list.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        server_list.addListSelectionListener(new ListSelectionListener()
        {
            public void valueChanged(ListSelectionEvent e)
            {
                int idx = server_list.getSelectedIndex();
                if (idx != -1)
                {
                    remove_b.setEnabled(true);
                    modify_b.setEnabled(true);
                    server_l.setText(dw.server_ip_list[idx].name);
                    server_a.setText(dw.server_ip_list[idx].argument);
                    server_u.setText(dw.server_ip_list[idx].user);
                    data_provider_list.setSelectedItem(dw.server_ip_list[idx].class_name);
                    if (dw.server_ip_list[idx].tunnel_port != null)
                    {
                        if (dw.server_ip_list[idx].tunnel_port.trim().length() ==
                            0)
                            dw.server_ip_list[idx].tunnel_port = null;
                        else
                        {
                            tunneling.setSelected(true);
                            tunnel_port.setText(dw.server_ip_list[idx].
                                                tunnel_port);
                            tunnel_port.setEditable(true);
                        }
                    }
                    if (dw.server_ip_list[idx].tunnel_port == null)
                    {
                        tunnel_port.setText("");
                        tunneling.setSelected(false);
                        tunnel_port.setEditable(false);
                    }
                }
                else
                {
                    remove_b.setEnabled(false);
                    modify_b.setEnabled(false);
                }
            }

        }
        );
        //server_list.addKeyListener(this);
        gridbag.setConstraints(scrollServerList, c);
        getContentPane().add(scrollServerList);

        c.anchor = GridBagConstraints.WEST;
        c.fill = GridBagConstraints.NONE;
        c.gridheight = 1;
        c.gridwidth = 1;
        server_label = new JLabel("Server label ");
        gridbag.setConstraints(server_label, c);
        getContentPane().add(server_label);

        c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.BOTH;
        server_l = new JTextField(20);
        gridbag.setConstraints(server_l, c);
        getContentPane().add(server_l);

        c.gridwidth = 1;
        server_label = new JLabel("Server argument ");
        gridbag.setConstraints(server_label, c);
        getContentPane().add(server_label);

        c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.BOTH;
        server_a = new JTextField(20);
        gridbag.setConstraints(server_a, c);
        getContentPane().add(server_a);

        c.gridwidth = 1;
        tunneling = new JCheckBox("Tunneling local Port:");
        tunneling.addItemListener(new ItemListener()
        {
            public void itemStateChanged(ItemEvent e)
            {
                if (tunneling.isSelected())
                {
                    tunnel_port.setEditable(true);
                }
                else
                {
                    tunnel_port.setEditable(false);
                }
            }
        });
        gridbag.setConstraints(tunneling, c);
        getContentPane().add(tunneling);

        c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.BOTH;
        tunnel_port = new JTextField(6);
        tunnel_port.setEditable(false);
        gridbag.setConstraints(tunnel_port, c);
        getContentPane().add(tunnel_port);

        c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.BOTH;
        automatic = new JCheckBox("Get user name from host");
        automatic.addItemListener(new ItemListener()
        {
            public void itemStateChanged(ItemEvent e)
            {
                if (automatic.isSelected())
                {
                    server_u.setText(System.getProperty("user.name"));
                    server_u.setEditable(false);
                }
                else
                {
                    server_u.setText("");
                    server_u.setEditable(true);
                }
            }
        });
        gridbag.setConstraints(automatic, c);
        getContentPane().add(automatic);

        c.gridwidth = 1;
        server_label = new JLabel("User name ");
        gridbag.setConstraints(server_label, c);
        getContentPane().add(server_label);

        c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.BOTH;
        server_u = new JTextField(20);
        gridbag.setConstraints(server_u, c);
        getContentPane().add(server_u);

        c.anchor = GridBagConstraints.WEST;
        c.gridwidth = 1;
        JLabel lab = new JLabel("Server Class : ");
        gridbag.setConstraints(lab, c);
        getContentPane().add(lab);

        c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.BOTH;
        data_provider_list = new JComboBox<>();
        gridbag.setConstraints(data_provider_list, c);
        getContentPane().add(data_provider_list);
        data_provider_list.addItemListener(new ItemListener() {
            public void itemStateChanged(ItemEvent e)
            {
                try
                {
                    if( e.getStateChange() == ItemEvent.SELECTED )
                    {
                        String srv = (String)data_provider_list.getSelectedItem();
                        if (srv != null)
                        {
                            Class cl = Class.forName("jScope."+srv);
                            DataProvider dp = ( (DataProvider) cl.newInstance()); 
                            boolean state = dp.SupportsTunneling();
                            tunneling.setEnabled(state);
                            tunnel_port.setEnabled(state);
                        }
                    }                    
                }
                catch (Exception exc)
                {}

            }

        });

        JPanel p = new JPanel(new FlowLayout(FlowLayout.CENTER));
        add_b = new JButton("Add");
        add_b.addActionListener(this);
        p.add(add_b);

        modify_b = new JButton("Modify");
        modify_b.addActionListener(this);
        modify_b.setEnabled(false);
        p.add(modify_b);

        remove_b = new JButton("Remove");
        remove_b.addActionListener(this);
        remove_b.setEnabled(false);
        p.add(remove_b);

        connect_b = new JButton("Connect");
        connect_b.addActionListener(this);
        p.add(connect_b);

        exit_b = new JButton("Close");
        exit_b.addActionListener(this);
        p.add(exit_b);

        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(p, c);
        getContentPane().add(p);

        addKnowProvider();

        if (dw.server_ip_list == null)
            GetPropertiesValue();
        else
            addServerIpList(dw.server_ip_list);

    }

    private void GetPropertiesValue()
    {
        Properties js_prop = dw.js_prop;
        DataServerItem dsi;
        int i = 1;

        if (js_prop == null)
            return;
        while (true)
        {
            dsi = new DataServerItem();
            dsi.name = js_prop.getProperty("jScope.data_server_" + i + ".name");
            if (dsi.name == null)
                break;
            dsi.argument = js_prop.getProperty("jScope.data_server_" + i + ".argument");
            dsi.user = js_prop.getProperty("jScope.data_server_" + i + ".user");
            dsi.class_name = js_prop.getProperty("jScope.data_server_" + i + ".class");
            dsi.browse_class = js_prop.getProperty("jScope.data_server_" + i + ".browse_class");
            dsi.browse_url = js_prop.getProperty("jScope.data_server_" + i + ".browse_url");
            dsi.tunnel_port = js_prop.getProperty("jScope.data_server_" + i + ".tunnel_port");
            try {
                dsi.fast_network_access = new Boolean(js_prop.getProperty(
                    "jScope.data_server_" + i + ".fast_network_access")).booleanValue();
            }
            catch (Exception exc)
            {
                dsi.fast_network_access = false;
            }
            addServerIp(dsi);
            i++;
        }
    }

    public void Show()
    {
        pack();
        resetAll();
        DataServerItem found_dsi = findServer(dw.wave_panel.GetServerItem());
        if (found_dsi != null)
            server_list.setSelectedValue(found_dsi, true);
        setLocationRelativeTo(dw);
        setVisible(true);
    }

    private void resetAll()
    {
        server_list.clearSelection();
        server_l.setText("");
        server_a.setText("");
        server_u.setText("");
        tunnel_port.setText("");
        data_provider_list.setSelectedIndex(0);
    }

    private DataServerItem findServer(DataServerItem dsi)
    {
        DataServerItem found_dsi = null;
        Enumeration<DataServerItem> e = list_model.elements();
        while (e.hasMoreElements())
        {
            found_dsi = e.nextElement();
            if (found_dsi.equals(dsi))
            {
                return found_dsi;
            }
        }
        return null;
    }

    public DataServerItem addServerIp(DataServerItem dsi)
    {
        JMenuItem new_ip;
        DataServerItem found_dsi = null;
        /*
        23-05-2005
        found = ( (found_dsi = findServer(dsi)) != null);
        */
        found_dsi = findServer(dsi);
        if(found_dsi != null)
             return found_dsi;

        if (dsi.class_name != null &&
            !data_server_class.containsValue(dsi.class_name))
        {
            data_server_class.put(dsi.class_name, dsi.class_name);
            data_provider_list.addItem(dsi.class_name);
        }

 //     if (!found && dsi.class_name == null)
        if (dsi.class_name == null)
        {
            JOptionPane.showMessageDialog(null,
                                          "Undefine data server class for " +
                                          dsi.name, "alert addServerIp",
                                          JOptionPane.ERROR_MESSAGE);
        }

//        if (!found)
        {
            list_model.addElement(dsi);
            new_ip = new JMenuItem(dsi.name);
            dw.servers_m.add(new_ip);
            new_ip.setActionCommand("SET_SERVER " + dsi.name);
            new_ip.addActionListener(dw);
            dw.server_ip_list = getServerIpList();
        }

        /*
        23-05-2005
        Ovverride configuration file server definitions
        with property server definition with the same name

        else
        {

            if (found_dsi != null)
            {
                dsi.user = found_dsi.user;
                dsi.class_name = found_dsi.class_name;
                dsi.browse_class = found_dsi.browse_class;
                dsi.browse_url = found_dsi.browse_url;
                dsi.tunnel_port = found_dsi.tunnel_port;
            }
        }
        */
        return dsi;
    }

    private void addKnowProvider()
    {
        for (int i = 0; i < know_provider.length; i++)
        {
            data_server_class.put(know_provider[i], know_provider[i]);
            data_provider_list.addItem(know_provider[i]);
        }
    }

    public void addServerIpList(DataServerItem[] dsi_list)
    {
        if (dsi_list == null)
            return;
        for (int i = 0; i < dsi_list.length; i++)
            addServerIp(dsi_list[i]);
    }

    public DataServerItem[] getServerIpList()
    {
        Enumeration<DataServerItem> e = list_model.elements();
        DataServerItem out[] = new DataServerItem[list_model.size()];
        for (int i = 0; e.hasMoreElements(); i++)
            out[i] = e.nextElement();
        return out;
    }

    public void actionPerformed(ActionEvent event)
    {
        Object ob = event.getSource();

        if (ob == exit_b)
            setVisible(false);

        if (ob == add_b)
        {
            String srv = server_l.getText().trim();

            if (srv != null && srv.length() != 0)
            {

                addServerIp(new DataServerItem(srv, server_a.getText().trim(),
                                               server_u.getText().trim(),
                                               (String)data_provider_list.getSelectedItem(),
                                               null, null, tunnel_port.getText(), false));
            }
        }

        if (ob == remove_b)
        {
            int idx = server_list.getSelectedIndex();
            if (idx >= 0)
            {
                list_model.removeElementAt(idx);
                dw.servers_m.remove(idx);
            }
        }

        if (ob == connect_b)
        {
            int idx = server_list.getSelectedIndex();
            if (idx >= 0)
                dw.SetDataServer( dw.server_ip_list[idx] );
        }

        if (ob == modify_b)
        {
            int idx = server_list.getSelectedIndex();
            if (idx >= 0)
            {
                String srv = server_l.getText().trim();
                if (srv != null && srv.length() != 0)
                {
                    if(!dw.server_ip_list[idx].name.equals(srv))
                    {
                        int itemsCount = dw.servers_m.getItemCount();
                        JMenuItem mi;
                        for(int i = 0; i < itemsCount; i++)
                        {
                            mi = dw.servers_m.getItem(i);
                            if(mi.getText().equals(dw.server_ip_list[idx].name))
                            {
                                mi.setText(srv);
                                mi.setActionCommand("SET_SERVER " + srv);

                            }
                        }
                        dw.server_ip_list[idx].name = srv;
                    }
                    dw.server_ip_list[idx].argument = server_a.getText().trim();
                    dw.server_ip_list[idx].user = server_u.getText().trim();
                    dw.server_ip_list[idx].class_name = (String)data_provider_list.getSelectedItem();
                    dw.server_ip_list[idx].tunnel_port = tunnel_port.getText();
                    server_list.repaint();
                    //It is need to update the current data server if it is
                    //the modified server
     //               if (dw.wave_panel.GetServerItem().equals(dw.server_ip_list[idx]))
      //              {
      //                  dw.SetDataServer(dw.server_ip_list[idx]);
      //              }
                }
            }
        }
    }






}

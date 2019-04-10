import java.awt.*;
import java.io.*;
import java.net.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;

import java.awt.event.*;
import javax.swing.event.*;
import javax.print.*;
import javax.print.attribute.*;
import java.awt.print.Printable;
import java.awt.print.PageFormat;
import java.awt.print.PrinterException;
import java.text.*;
import javax.swing.filechooser.*;


/*
Le tabelle di configurazione rfxConfigHash e rfxConfigOnHash sono aggiornate in:
     - init()
     - modifica singola form config
     - uscita dal PAS

 Il check della configurazione e' eseguito in:
    - uscita dal PAS
    - Caricamento di un impulso

 Quando viene caricata una configurazione salvata si esegue il confronto tra la configurazione salvata
 e l'experiment model attuale.
La configurazione viene salvata solamente alla partenza e quando viene modificato un device di config

*/
public class ParameterSetting
    extends JFrame implements Printable
{
    static final String DECOUPLING_BASE_DIR = "/usr/local/rfx/data_acquisition/real_time/decoupling/";
    //static final int NUM_DEVICES = 22;
    static final int NUM_DEVICES = 23;
    //static final int NUM_SETUP = 15;
    static final int NUM_SETUP = 16;
    boolean isRt = false;
    boolean readOnly = false;
    static String refShotLabelText = "Ref. Shot: ";
    JLabel refShotLabel;
    JButton timesB, poloidalControlB, axiSetupB, pcSetupB, pmSetupB, ipSetupB,
	toroidalControlB, chopperSetupB, ffSetupB, inverterSetupB, tfSetupB,
	bfControlB,
	mhdControlB, viSetupB, mopB, ansaldoConfigB, unitsConfigB,
	poloidalConfigB,
	toroidalConfigB, mhdConfigB, viConfigB;
    JButton buttons[] = new JButton[NUM_DEVICES];
    JButton applyToModelB;
    Database rfx;
    NidData timesRoot, poloidalControlRoot, axiSetupRoot, pcSetupRoot,
	pmSetupRoot, ipSetupRoot,
	toroidalControlRoot, chopperSetupRoot, ffSetupRoot, inverterSetupRoot,
	tfSetupRoot, bfControlRoot,
	mhdControlRoot, viSetupRoot, mopRoot, ansaldoConfigRoot,
	unitsConfigRoot,
	poloidalConfigRoot, toroidalConfigRoot, mhdConfigRoot, viConfigRoot,
	pvSetupRoot, //Usato solo per la configurazione PV
	pelletSetupRoot, diagTimesSetupRoot;
    String[] titles =
	{
	"TIMES SETUP", "POLOIDAL CONTROL", "AXISYMMETRIC SETUP", "PC SETUP",
	"PM SETUP",
	"TOROIDAL CONTROL", "CHOPPER SETUP", "FEEDFORWARD SETUP",
	"INVERTER SETUP", "TF SETUP", "B&F CONTROL",
	"MHD CONTROL", "VI SETUP", "MOP", "UNITS SETUP", "IP SETUP", "UNITS CONFIG",
	"POLOIDAL CONFIG", "TOROIDAL CONFIG", "MHD CONFIG", "VI CONFIG"};

    static final int CHECKED = 0, CHECKING = 1, UNCHECKED = 2;
    int states[] = new int[]
	{
	CHECKED, CHECKED, CHECKED, CHECKED, CHECKED, CHECKED, CHECKED, CHECKED,
	CHECKED, CHECKED,
	CHECKED, CHECKED, CHECKED, CHECKED, CHECKED, CHECKED, CHECKED, CHECKED,
	CHECKED, CHECKED, CHECKED, CHECKED, CHECKED};
    int currPrintDeviceIdx, currPrintLoadPulse;
    JTabbedPane tabbedP;

    NidData nids[] = new NidData[NUM_DEVICES];
    //NidData mhdBcNid;
    DeviceSetup devices[] = new DeviceSetup[NUM_DEVICES];
    JMenuItem applyToModelItem;
    JMenuItem revertModelItem;
    JMenuItem compareItem;
    JDialog changedD;
    JComboBox currFFC;

    String rtIp;
    Socket rtSock;
    DataOutputStream rtDos;
    int[] modifiedNids;
    int currLoadShot;
    Cursor prevCursor = getCursor();

    String decouplingNames[];
    int decouplingKeys[];

    FileWriter logFile = null;


    PrintService printService = PrintServiceLookup.lookupDefaultPrintService();

    /*   Hashtable rfxConfigHash = new Hashtable();
       Hashtable rfxConfigOnHash = new Hashtable();
       Hashtable currConfigHash = new Hashtable();
       Hashtable currConfigOnHash = new Hashtable();
     */

    Hashtable mapSetupHash = new Hashtable();
    Hashtable modelSetupHash = new Hashtable();
    Hashtable modelSetupOnHash = new Hashtable();
    Hashtable currSetupHash = new Hashtable();
    Hashtable currSetupOnHash = new Hashtable();

    JTextField maxPMATF, maxPCATParallelF, maxPCATSeriesF, maxTFATF, maxTCCHF,
	maxTCACF, maxPMVoltageF,
//	maxFillVoltageF, maxPuffVoltageF,
	maxTempRoomF, maxTempSaddleF,
	maxTempMagnetizingF, maxPOhmF, maxPrTimeF, maxTempTorF, maxI2TF,
	maxCurrSellaVF,maxCurrSellaVI0F, maxCurrSellaPF, maxFillPuffVoltageF;
    int maxPMAT, maxPCATParallel, maxPCATSeries, maxTFAT, maxTCCH, maxTCAC,
	maxPMVoltage,
//	maxFillVoltage, maxPuffVoltage,
	maxTempRoom, maxTempSaddle,
	maxTempMagnetizing, maxPOhm, maxPrTime, maxTempTor, maxI2T,
	maxCurrSellaV,maxCurrSellaVI0, maxCurrSellaP, maxFillPuffVoltage;
    JTextArea messageArea;
/*
	JLabel residualI2tPMLabel;
*/
    WarningDialog checkedWd, configWd, limitsWd, versionWd;

    boolean doingShot = false;
    boolean isOnline;
    JComboBox modeC;
    int shot = 100;


    int[]
	pm_mask = new int[]
	{
	25, 26, 1},
	pc_mask = new int[]
	{
	29, 30, 14},
	pv_mask = new int[]
	{
	27, 28},
	pp_mask = new int[]
	{
	2, 3, 4, 5},
	pr_mask = new int[]
	{
	45, 46},
	ptso_mask = new int[]
	{
	19, 20, 21, 22},
	ptcb_mask = new int[]
	{
	15, 16, 17, 18},
	ptct_mask = new int[]
	{
	6, 7, 8, 9, 10, 11, 12, 13},
	gas_mask = new int[]
	{
	47, 48},
	tf_mask = new int[]
	{
	23, 24},
	is_mask = new int[]
	{
	38, 39},
	chopper_mask = new int[]
	{
	32, 33},
	inverter_mask = new int[]
	{
	34, 35, 36, 37, 40, 41, 42, 43, 44};

    JFileChooser chooser = new JFileChooser();

    SelectSetup saveSelected = null, loadSelected = null, applyModelSelected = null;
    DecouplingDialog decouplingD;

    ParameterSetting()
    {
	this(true, false, null);
    }

    ParameterSetting(String rtIp)
    {
	this(false, false, rtIp);
    }

    ParameterSetting(String rtIp, String online)
    {
	this(false, online.equals("online"), rtIp);
    }

    ParameterSetting(boolean isRt, boolean isOnline, String rtIp)
    {
	addWindowListener(new WindowAdapter()
	{
	    public void windowClosing(WindowEvent e)
	    {
	        System.exit(0);
	    }
	});
	this.isRt = isRt;
	if (isRt)
	    readOnly = true;
	this.isOnline = isOnline;
	if (isRt)
	{
	    setTitle("RFX Parameters -- RT    ");
	    handleRt();
	}
	else
	{
	    setTitle("RFX Parameters");
	    this.rtIp = rtIp;
	    if (isOnline) handleNotRt();
	}
	if (isRt || isOnline)
	{
	    handleScheduler();
	    handleAlarms();
	}

	if(!isRt)
	    prepareDecouplingInfo();

	JMenuBar menuBar = new JMenuBar();
	JMenu fileMenu = new JMenu("File");
	JMenuItem printItem = new JMenuItem("Print Setup");
	printItem.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("Print Setup");
	        printSetup();
	    }
	});
	fileMenu.add(printItem);
	if (!isRt)
	{
	    JMenuItem loadItem = new JMenuItem("Load pulse...");
	    loadItem.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            loadPulse();
	        }
	    });
	    fileMenu.add(loadItem);
	    JMenuItem saveItem = new JMenuItem("Save Configuration...");
	    saveItem.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            saveSelectedSetup();
	        }
	    });
	    fileMenu.add(saveItem);
	    loadItem = new JMenuItem("Load Configuration...");
	    loadItem.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            loadSetup();
	        }
	    });
	    fileMenu.add(loadItem);

	    applyToModelItem = new JMenuItem("Apply To Model");
	    applyToModelItem.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            if (JOptionPane.showConfirmDialog(ParameterSetting.this,
	                "Transfer current configuration to the experiment model?",
	                "Confirmation request", JOptionPane.YES_NO_OPTION) ==
	                JOptionPane.YES_OPTION)
	            {
	                log("Apply to Model");
	                applyToModel();
	            }
	            else
	                log("Apply to Model aborted");
	        }
	    });
	    fileMenu.add(applyToModelItem);
	    applyToModelItem.setEnabled(isOnline);

	    revertModelItem = new JMenuItem("Revert To Previous Model");
	    revertModelItem.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            if (JOptionPane.showConfirmDialog(ParameterSetting.this,
	                "Revert to the last experiment model?",
	                "Confirmation request", JOptionPane.YES_NO_OPTION) ==
	                JOptionPane.YES_OPTION)
	            {
	                log("Revert to last experiment model");
	                revertModel();
	            }
	            else
	                log("Revert to last experiment model aborted");
	        }
	    });
	    revertModelItem.setEnabled(false);
	    fileMenu.add(revertModelItem);

	    compareItem = new JMenuItem("Compare to Shot ...");
	    compareItem.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            setCursor(new Cursor(Cursor.WAIT_CURSOR));
	            SwingUtilities.invokeLater(new Runnable() {
	                public void run(){compareShots();}});
	        }
	    });
	    fileMenu.add(compareItem);

	    JMenuItem decouplingItem = new JMenuItem("Set MHD Decoupling...");
	    decouplingItem.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            setDecoupling();
	        }
	    });
	    fileMenu.add(decouplingItem);
	    JMenuItem decouplingInfoItem = new JMenuItem("Get MHD Decoupling Info...");
	    decouplingInfoItem.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            log("Show decoupling info");
	            showDecouplingInfo();
	        }
	    });
	    fileMenu.add(decouplingInfoItem);

	}
	menuBar.add(fileMenu);

	JMenuItem exitItem = new JMenuItem("Exit");
	exitItem.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        try {
	            logFile.close();
	        }catch(Exception exc){}
	        System.exit(0);
	    }
	});
	fileMenu.add(exitItem);
	setJMenuBar(menuBar);

	FrameRepository.setFrame(this);
	JPanel setupJp = new JPanel();
	setupJp.setLayout(new BorderLayout());

	JPanel jp = new JPanel();

	if (!isRt)
	{
	    jp.add(new JLabel("Working shot: "));
	    if (isOnline)
	    {
	        modeC = new JComboBox(new String[]
	                              {"100"});
	        shot = 100;
	    }
	    else
	    {
	        modeC = new JComboBox(new String[]
	                              {"101", "102", "103", "104", "105", "106",
	                              "107", "108", "109"});
	        shot = 101;
	    }
	    modeC.setSelectedIndex(0);
	    modeC.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            int idx = modeC.getSelectedIndex();
	            int baseShot = ( (ParameterSetting.this.isOnline) ? 100 :
	                            101);
	            try
	            {
	                rfx.close(0);
	                rfx  = new Database("rfx", -1);
	                rfx.open();
	                rfx.create(baseShot+idx);
	                rfx.close(0);
	                rfx = new Database("rfx", baseShot + idx);
	                rfx.open();
	                shot = baseShot + idx;
	            }
	            catch (Exception exc)
	            {
	                System.err.println("Error opening working RFX pulse: " +
	                                   exc);
	                System.exit(0);
	            }
	        }
	    });
	    jp.add(modeC);
	    if(isOnline)
	       jp.add(refShotLabel = new JLabel(""));
	}

	buttons[0] = timesB = new JButton("Times Setup");
	timesB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("Times Setup open");

	        int nid = timesRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[0] == null)
	        {
	            devices[0] = device = new RFXTimesSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(0);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(0, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[0] == UNCHECKED)
	        {
	            states[0] = CHECKING;
	            if(modifiedNids != null && modifiedNids.length > 0)
	                device.setHighlight(true, modifiedNids);
	        }
	    }
	});
	jp.add(timesB);


	buttons[14] = viSetupB = new JButton("Diag Times Setup");
	viSetupB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("Pellet Setup open");
	        int nid = diagTimesSetupRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[12] == null)
	        {
	            devices[14] = device = new RFXDiagTimesV1Setup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(14);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(14, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[14] == UNCHECKED)
	        {
	            states[14] = CHECKING;
	            if(modifiedNids != null && modifiedNids.length > 0)
	                device.setHighlight(true, modifiedNids);
	        }
	    }
	});
	jp.add(buttons[14]);
/*
	JButton forTestB = new JButton("Test Button");
	forTestB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
		String limitsMsg = checkLimits();
		System.out.println("Limint : " + limitsMsg);
	    }
	});
	jp.add(forTestB);
*/
	if (!isRt && isOnline)
	{
	    applyToModelB = new JButton("Apply To Model");
	    applyToModelB.setForeground(Color.red);
	    applyToModelB.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            if (JOptionPane.showConfirmDialog(ParameterSetting.this,
	                "Transfer current configuration to the experiment model?",
	                "Confirmation request", JOptionPane.YES_NO_OPTION) ==
	                JOptionPane.YES_OPTION)
	                applyToModel();
	        }
	    });
	    jp.add(applyToModelB);
	}

/*
	jp.add(residualI2tPMLabel = new JLabel());
*/
	setupJp.add(jp, "North");
	jp = new JPanel();
	jp.setLayout(new GridLayout(1, 4));
	JPanel jp1 = new JPanel();
	jp1.setBorder(new TitledBorder("Poloidal"));
	jp1.setLayout(new GridLayout(6, 1));
	buttons[1] = poloidalControlB = new JButton("EDA1");
	poloidalControlB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("EDA1 Setup open");
	        int nid = poloidalControlRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[1] == null)
	        {
	            devices[1] = device = new RFXPolControlSetup();
	            device.configure(rfx, nid);
	            //if (ParameterSetting.this.readOnly)
	            device.setReadOnly(true);
	            PrintButton printB = new PrintButton(1);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(1, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[1] == UNCHECKED)
	        {
	            states[1] = CHECKING;
	            if(modifiedNids != null && modifiedNids.length > 0)
	                device.setHighlight(true, modifiedNids);
	        }
	    }
	});
	jp1.add(poloidalControlB);
	buttons[2] = axiSetupB = new JButton("Axi Setup");
	axiSetupB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("Axi Setup open");
	        int nid = axiSetupRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[2] == null)
	        {
	            devices[2] = device = new RFXAxiControlSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(2);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(2, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[2] == UNCHECKED)
	        {
	            states[2] = CHECKING;
	            if(modifiedNids != null && modifiedNids.length > 0)
	                device.setHighlight(true, modifiedNids);
	        }
	    }
	});
	jp1.add(axiSetupB);
	buttons[3] = pcSetupB = new JButton("PC Setup");
	pcSetupB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("PC Setup open");
	        int nid = pcSetupRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[3] == null)
	        {
	            //devices[3] = device = new RFXPCSetupSetup();
	            devices[3] = device = new RFXPC4SetupSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	           device.pack();
	            //device.setLocation(getMousePosition());
	            PrintButton printB = new PrintButton(3);
	            device.addButton(printB);
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(3, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[3] == UNCHECKED)
	        {
	            states[3] = CHECKING;
	            if(modifiedNids != null && modifiedNids.length > 0)
	                device.setHighlight(true, modifiedNids);
	        }
	    }
	});
	jp1.add(pcSetupB);
	buttons[4] = pmSetupB = new JButton("PM Setup");
	jp1.add(pmSetupB);
	pmSetupB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("PM Setup open");
	        int nid = pmSetupRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[4] == null)
	        {
	            devices[4] = device = new RFXPMSetupSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(4);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(4, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[4] == UNCHECKED)
	        {
	            states[4] = CHECKING;
	            if(modifiedNids != null && modifiedNids.length > 0)
	                device.setHighlight(true, modifiedNids);
	        }
	    }
	});
 ////////New IP Control 2010
	buttons[15] = ipSetupB = new JButton("IP Setup");
	jp1.add(ipSetupB);
	ipSetupB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("IP Setup open");
	        int nid = ipSetupRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[4] == null)
	        {
	            devices[15] = device = new IPControlSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(15);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(15, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[15] == UNCHECKED)
	        {
	            states[15] = CHECKING;
	            if(modifiedNids != null && modifiedNids.length > 0)
	                device.setHighlight(true, modifiedNids);
	        }
	    }
	});



////////////////////////
	jp.add(jp1);

	jp1 = new JPanel();
	jp1.setBorder(new TitledBorder("Toroidal"));
	jp1.setLayout(new GridLayout(6, 1));
	buttons[5] = toroidalControlB = new JButton("EDA3");
	toroidalControlB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("EDA3 Setup open");
	        int nid = toroidalControlRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[5] == null)
	        {
	            devices[5] = device = new RFXTorControlSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(5);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(5, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[5] == UNCHECKED)
	        {
	            states[5] = CHECKING;
	            if(modifiedNids != null && modifiedNids.length > 0)
	                device.setHighlight(true, modifiedNids);
	        }
	    }
	});
	jp1.add(toroidalControlB);
	buttons[6] = chopperSetupB = new JButton("Chopper Setup");
	chopperSetupB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("Chopper Setup open");
	        int nid = chopperSetupRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[6] == null)
	        {
	            devices[6] = device = new RFXChopperSetupSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(6);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(6, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[6] == UNCHECKED)
	        {
	            states[6] = CHECKING;
	            if(modifiedNids != null && modifiedNids.length > 0)
	                device.setHighlight(true, modifiedNids);
	        }
	    }
	});
	jp1.add(chopperSetupB);
	buttons[7] = ffSetupB = new JButton("FeedForward Setup");
	ffSetupB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("Feedforward Setup open");
	        int nid = ffSetupRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[7] == null)
	        {
	            devices[7] = device = new RFXFeedforwardSetupSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(7);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(7, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[7] == UNCHECKED)
	        {
	            states[7] = CHECKING;
	            if(modifiedNids != null && modifiedNids.length > 0)
	                device.setHighlight(true, modifiedNids);
	        }
	    }
	});
	jp1.add(ffSetupB);
	buttons[8] = inverterSetupB = new JButton("Inverter Setup");
	inverterSetupB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("Inverter Setup open");
	        int nid = inverterSetupRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[8] == null)
	        {
	            devices[8] = device = new RFXInverterSetupSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(8);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(8, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[8] == UNCHECKED)
	        {
	            states[8] = CHECKING;
	            if(modifiedNids != null && modifiedNids.length > 0)
	                device.setHighlight(true, modifiedNids);
	        }
	    }
	});
	jp1.add(inverterSetupB);
	buttons[9] = tfSetupB = new JButton("TF Setup");
	tfSetupB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("TF Setup open");
	        int nid = tfSetupRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[9] == null)
	        {
	            devices[9] = device = new RFXTFSetupSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(9);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(9, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[9] == UNCHECKED)
	        {
	            states[9] = CHECKING;
	            if(modifiedNids != null && modifiedNids.length > 0)
	                device.setHighlight(true, modifiedNids);
	        }
	    }
	});
	jp1.add(tfSetupB);
	buttons[10] = bfControlB = new JButton("B&F Control");
	bfControlB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("B&F Setup open");
	        int nid = bfControlRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[10] == null)
	        {
	            devices[10] = device = new RFXAxiToroidalControlSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(10);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(10, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	         if (states[10] == UNCHECKED)
	        {
	            states[10] = CHECKING;
	            if(modifiedNids != null && modifiedNids.length > 0)
	                device.setHighlight(true, modifiedNids);
	        }
	    }
	});
	jp1.add(bfControlB);
	jp.add(jp1);

	jp1 = new JPanel();
	jp1.setBorder(new TitledBorder("MHD"));
	jp1.setLayout(new GridLayout(6, 1));
	buttons[11] = mhdControlB = new JButton("MHD Control");
	mhdControlB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("MHD Control Setup open");
	        int nid = mhdControlRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[11] == null)
	        {
//                    devices[11] = device = new RFXMHDSetup();
	            devices[11] = device = new MARTE_MHD_CTRLSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(11);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(11, updated);
/*                            //Copy the same configuration to MHD BC
	                    //devices[11].apply( mhdControlRoot.getInt());

	                    //GABRIELE OTTOBRE 2008
	                    //Faccio Apply solo se isChanged true
	                    if(justApplied)
	                        devices[11].apply(mhdBcNid.getInt());
	                    else
	                    {
	                        //Il Reset e' necessario nel caso siano state cambiati dei campi senza apply
	                        devices[11].reset();
	                        devices[11].apply(mhdBcNid.getInt());
	                    }

	                    /////////////////////////////////////////



	                    //Copy PAR303_VAL (measure radius), PAR304_VAL (MoNo sine excluded)
	                    //and PAR305_VAL (N limits for sideband correction) into MHD_BR
	                    copyData("\\MHD_AC::CONTROL.PARAMETERS:PAR303_VAL", "\\MHD_BR::CONTROL.PARAMETERS:PAR303_VAL");
	                    copyData("\\MHD_AC::CONTROL.PARAMETERS:PAR304_VAL", "\\MHD_BR::CONTROL.PARAMETERS:PAR304_VAL");
	                    copyData("\\MHD_AC::CONTROL.PARAMETERS:PAR305_VAL", "\\MHD_BR::CONTROL.PARAMETERS:PAR305_VAL");
 */
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[11] == UNCHECKED)
	        {
	            states[11] = CHECKING;
	            if(modifiedNids != null && modifiedNids.length > 0)
	                device.setHighlight(true, modifiedNids);
	        }
	    }
	});
	jp1.add(mhdControlB);
	JPanel jp2 = new JPanel();
	jp2.add(new JLabel("MHD FF:"));
	jp2.add(currFFC = new JComboBox(new String[] {"DISABLED", "ENABLED"}));
	jp1.add(jp2);
	if(isRt)
	    currFFC.setEnabled(false);
	else
	{
	    currFFC.addActionListener(new ActionListener() {
	        public void actionPerformed(ActionEvent e)
	        {
	            setCurrFFState();
	        }
	    });
	}
	jp.add(jp1);

	jp1 = new JPanel();
	jp1.setBorder(new TitledBorder("Vessel"));
	jp1.setLayout(new GridLayout(6, 1));
	buttons[12] = viSetupB = new JButton("VI Setup");
	viSetupB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("VI Setup open");
	       int nid = viSetupRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[12] == null)
	        {
//					devices[12] = device = new RFXVISetupSetup();
					devices[12] = device = new RFXVICONTROLSetup();

	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(12);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(12, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[12] == UNCHECKED)
	        {
	            states[12] = CHECKING;
	            if(modifiedNids != null && modifiedNids.length > 0)
	                device.setHighlight(true, modifiedNids);
	        }
	    }
	});
	jp1.add(viSetupB);
	buttons[13] = viSetupB = new JButton("Pellet Setup");
	viSetupB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("Pellet Setup open");
	        int nid = pelletSetupRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[12] == null)
	        {
	            devices[13] = device = new PELLETSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(13);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(13, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[13] == UNCHECKED)
	        {
	            states[13] = CHECKING;
	            if(modifiedNids != null && modifiedNids.length > 0)
	                device.setHighlight(true, modifiedNids);
	        }
	    }
	});
	jp1.add(viSetupB);
	jp.add(jp1);
	setupJp.add(jp, "Center");

	if (!isRt)
	{
	    getContentPane().add(setupJp, "Center");
	    chooser.setCurrentDirectory(new File(System.getProperty("user.home")));
	    chooser.setFileFilter(new RfxFileFilter());
	    return;
	}

//Other Stuff only for RT
	JPanel configJp = new JPanel();
	configJp.setLayout(new GridLayout(1, 2));
	jp = new JPanel();
	jp.setLayout(new GridLayout(4, 1));

	buttons[16] = mopB = new JButton("MOP");
	mopB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("MOP Setup open");
	        int nid = mopRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[13] == null)
	        {
	            devices[16] = device = new RFXMOPSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(16);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(16, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[16] == UNCHECKED)
	            states[16] = CHECKING;
	    }
	});
	jp.add(mopB);

	buttons[17] = ansaldoConfigB = new JButton("Ansaldo Config");
	ansaldoConfigB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("Ansaldo Setup open");
	        int nid = ansaldoConfigRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[14] == null)
	        {
	            devices[17] = device = new RFXANSALDOSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(17);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(17, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[17] == UNCHECKED)
	            states[17] = CHECKING;
	    }
	});
	jp.add(ansaldoConfigB);

	buttons[18] = unitsConfigB = new JButton("Units Config");
	unitsConfigB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("Units Setup open");
	        int nid = unitsConfigRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[15] == null)
	        {
	            devices[18] = device = new RFXABUnitsSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(18);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(18, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[18] == UNCHECKED)
	            states[18] = CHECKING;
	    }
	});
	jp.add(unitsConfigB);
	configJp.add(jp);

	jp = new JPanel();
	jp.setLayout(new GridLayout(4, 1));

	buttons[19] = poloidalConfigB = new JButton("Poloidal Config");
	poloidalConfigB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("Poloidal Config Setup open");
	       int nid = poloidalConfigRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[16] == null)
	        {
	            devices[19] = device = new RFXPoloidalSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(19);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(19, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[19] == UNCHECKED)
	            states[19] = CHECKING;
	    }
	});
	jp.add(poloidalConfigB);

	buttons[20] = toroidalConfigB = new JButton("Toroidal Config");
	toroidalConfigB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("Toroidal config Setup open");
	        int nid = toroidalConfigRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[17] == null)
	        {
	            devices[20] = device = new RFXToroidalSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(20);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(20, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[20] == UNCHECKED)
	            states[20] = CHECKING;
	    }
	});
	jp.add(toroidalConfigB);

	buttons[21] = mhdConfigB = new JButton("MHD Config");
	mhdConfigB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("MHD Config Setup open");
	        int nid = mhdConfigRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[18] == null)
	        {
	            devices[21] = device = new RFXPRConfigSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(21);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(21, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[21] == UNCHECKED)
	            states[21] = CHECKING;
	    }
	});
	jp.add(mhdConfigB);

	//buttons[22] = viConfigB = new JButton("Vi Config"); Taliercio 10 - 01 - 2011
	buttons[22] = viConfigB = new JButton("V Config");
	viConfigB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        log("VI Config Setup open");
	        int nid = viConfigRoot.getInt();
	        DeviceSetup device = DeviceSetup.getDevice(nid);
	        if (device == null)
	        //if (devices[19] == null)
	        {
					/*
	            devices[22] = device = new RFXVIConfigSetup(); Taliercio 10 - 1 - 2011
					*/
	            devices[22] = device = new RFXVConfigSetup();
	            device.configure(rfx, nid);
	            if (ParameterSetting.this.readOnly)
	                device.setReadOnly(true);
	            PrintButton printB = new PrintButton(22);
	            device.addButton(printB);
	            if(ParameterSetting.this.isRt) device.setCancelText("Acknowledge");
	            device.pack();
	            //device.setLocation(getMousePosition());
	            device.setVisible(true);
	            device.addDeviceCloseListener(new DeviceCloseListener()
	            {
	                public void deviceClosed(boolean updated, boolean justApplied)
	                {
	                    handleDeviceClosed(22, updated);
	                }
	            });
	        }
	        else
	            device.setVisible(true);
	        if (states[22] == UNCHECKED)
	            states[22] = CHECKING;
	    }
	});
	jp.add(viConfigB);
	configJp.add(jp);

	JPanel limitsJp = new JPanel();
	limitsJp.setLayout(new BorderLayout());
	JPanel limitsListJp = new JPanel();

	limitsListJp.setLayout(new GridLayout(18, 1));
	jp = new JPanel();
	jp.add(new JLabel("Corrente Max. PMAT per unita' (A): "));
	jp.add(maxPMATF = new JTextField("" + maxPMAT, 10));
	limitsListJp.add(jp);
	jp = new JPanel();
	jp.add(new JLabel("Tensione Max. PCAT in serie: "));
	jp.add(maxPCATSeriesF = new JTextField("" + maxPCATSeries, 10));
	limitsListJp.add(jp);
	jp = new JPanel();
	jp.add(new JLabel("Tensione Max. PCAT in Parallelo: "));
	jp.add(maxPCATParallelF = new JTextField("" + maxPCATParallel, 10));
	limitsListJp.add(jp);
	jp = new JPanel();
	jp.add(new JLabel("Corrente Max. TFAT per unita' (A): "));
	jp.add(maxTFATF = new JTextField("" + maxTFAT, 10));
	limitsListJp.add(jp);
	jp = new JPanel();
	jp.add(new JLabel("Tensione Max. Chopper toroidale TCCH (V): "));
	jp.add(maxTCCHF = new JTextField("" + maxTCCH, 10));
	limitsListJp.add(jp);
	jp = new JPanel();
	jp.add(new JLabel("Corrente Max. Inverter toroidale TCAC (A): "));
	jp.add(maxTCACF = new JTextField("" + maxTCAC, 10));
	limitsListJp.add(jp);
	jp = new JPanel();
	jp.add(new JLabel("Tensione di picco avvolgimento magnetizzante: "));
	jp.add(maxPMVoltageF = new JTextField("" + maxPMVoltage, 10));
	limitsListJp.add(jp);
	jp = new JPanel();
	jp.add(new JLabel("Temperatura Max. avvolgimento magnetizzante (C): "));
	jp.add(maxTempMagnetizingF = new JTextField("" + maxTempMagnetizing, 10));
	limitsListJp.add(jp);
	jp = new JPanel();
	jp.add(new JLabel("Limite del tempo diaccensione di PR (ms): "));
	jp.add(maxPrTimeF = new JTextField("" + maxPrTime, 10));
	limitsListJp.add(jp);


	jp = new JPanel();
	jp.add(new JLabel("Limite corrente bobine a sella a vuoto (A): "));
	jp.add(maxCurrSellaVF = new JTextField("" + maxCurrSellaV, 10));
	limitsListJp.add(jp);

	jp = new JPanel();
	jp.add(new JLabel("Limite corrente bobine a sella a vuoto, altri avvolgimenti I=0 (A): "));
	jp.add(maxCurrSellaVI0F = new JTextField("" + maxCurrSellaVI0, 10));
	limitsListJp.add(jp);

	jp = new JPanel();
	jp.add(new JLabel("Limite corrente bobine a sella con plasma (A): "));
	jp.add(maxCurrSellaPF = new JTextField("" + maxCurrSellaP, 10));
	limitsListJp.add(jp);


	jp = new JPanel();
	jp.add(new JLabel("Temperatura Max. Bobine a sella (C): "));
	jp.add(maxTempSaddleF = new JTextField("" + maxTempSaddle, 10));
	limitsListJp.add(jp);
	jp = new JPanel();
	jp.add(new JLabel("Temperatura Max. Camera (C): "));
	jp.add(maxTempRoomF = new JTextField("" + maxTempRoom, 10));
	limitsListJp.add(jp);
	jp = new JPanel();
	jp.add(new JLabel("POhm. Max (MW): "));
	jp.add(maxPOhmF = new JTextField("" + maxPOhm, 10));
	limitsListJp.add(jp);
	jp = new JPanel();
	jp.add(new JLabel("Temperatura Max. bobine toroidali (C): "));
	jp.add(maxTempTorF = new JTextField("" + maxTempTor, 10));
	limitsListJp.add(jp);
	jp = new JPanel();
	jp.add(new JLabel("Limite I2T avvolgimento Toroidale (MA2s): "));
	jp.add(maxI2TF = new JTextField("" + maxI2T, 10));
	limitsListJp.add(jp);

	jp = new JPanel();
	jp.add(new JLabel("Tensione Max. Valvole Filling e Puffing (V): "));
	jp.add(maxFillPuffVoltageF = new JTextField("" + maxFillPuffVoltage, 10));
	limitsListJp.add(jp);

/*
	jp = new JPanel();
	jp.add(new JLabel("Tensione Max. Valvole Filling (V): "));
	jp.add(maxFillVoltageF = new JTextField("" + maxFillVoltage, 10));
	limitsListJp.add(jp);
	jp = new JPanel();
	jp.add(new JLabel("Tensione Max. Valvole Puffing (V): "));
	jp.add(maxPuffVoltageF = new JTextField("" + maxPuffVoltage, 10));
	limitsListJp.add(jp);
*/
	getLimits();

	limitsJp.add(limitsListJp, "Center");
	jp = new JPanel();
	JButton enterB = new JButton("Enter");
	enterB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        saveLimits();
	    }
	});
	jp.add(enterB);
	limitsJp.add(jp, "South");

	JPanel messageJp = new JPanel();
	messageJp.setLayout(new BorderLayout());
	jp = new JPanel();
	JLabel messagesTitle = new JLabel(
	    "Messaggi per il Responsabile di Turno");
	messagesTitle.setFont(new Font("Serif", Font.BOLD, 30));
	jp.add(messagesTitle);
	messageJp.add(jp, "North");
	messageArea = new JTextArea(10, 40);
	getRtMessages();
	messageJp.add(messageArea, "Center");
	jp = new JPanel();
	JButton saveB = new JButton("Save");
	saveB.addActionListener(new ActionListener()
	{
	    public void actionPerformed(ActionEvent e)
	    {
	        saveRtMessages();
	    }
	});
	jp.add(saveB);
	messageJp.add(jp, "South");

	tabbedP = new JTabbedPane();
	tabbedP.addChangeListener(new ChangeListener()
	{
	    public void stateChanged(ChangeEvent e)
	    {
	        if (tabbedP.getSelectedIndex() == 2) //If showing linits
	            getLimits();
	    }
	});
	tabbedP.add(setupJp, "Setup");
	tabbedP.add(configJp, "Config");
	tabbedP.add(limitsJp, "Limits");
	tabbedP.add(messageJp, "Messages");
	getContentPane().add(tabbedP, "Center");

    }

    void getLimits()
    {
	try
	{
	    BufferedReader br = new BufferedReader(new FileReader("rt_limits"));
	    maxPMAT = Integer.parseInt(br.readLine());
	    maxPCATParallel = Integer.parseInt(br.readLine());
	    maxPCATSeries = Integer.parseInt(br.readLine());
	    maxTFAT = Integer.parseInt(br.readLine());
	    maxTCCH = Integer.parseInt(br.readLine());
	    maxTCAC = Integer.parseInt(br.readLine());
	    maxPMVoltage = Integer.parseInt(br.readLine());
//            maxFillVoltage = Integer.parseInt(br.readLine());
//            maxPuffVoltage = Integer.parseInt(br.readLine());
	    maxTempRoom = Integer.parseInt(br.readLine());
	    maxTempSaddle = Integer.parseInt(br.readLine());
	    maxTempMagnetizing = Integer.parseInt(br.readLine());
	    maxPOhm = Integer.parseInt(br.readLine());
	    maxPrTime = Integer.parseInt(br.readLine());
	    maxTempTor = Integer.parseInt(br.readLine());
	    maxI2T = Integer.parseInt(br.readLine());

	    maxCurrSellaV = Integer.parseInt(br.readLine());
	    maxCurrSellaVI0 = Integer.parseInt(br.readLine());
	    maxCurrSellaP = Integer.parseInt(br.readLine());
	    maxFillPuffVoltage = Integer.parseInt(br.readLine());

	    br.close();
	}
	catch (Exception exc)
	{
	    System.out.println(exc);
	    maxPMAT = 12500;
	    maxPCATParallel = 1500;
	    maxPCATSeries = 3000;
	    maxTFAT = 6000;
	    maxTCCH = 2000;
	    maxTCAC = 2500;
	    maxPMVoltage = 35000;
//            maxFillVoltage = 120;
//            maxPuffVoltage = 120;
	    maxTempRoom = 100;
	    maxTempSaddle = 60;
	    maxTempMagnetizing = 35;
	    maxPOhm = 80;
	    maxPrTime = 650;
	    maxTempTor = 50;
	    maxI2T = 300;
	    maxCurrSellaV = 200;
	    maxCurrSellaVI0 = 400;
	    maxCurrSellaP = 400;
	    maxFillPuffVoltage = 120;

	}
	maxPMATF.setText("" + maxPMAT);
	maxPCATParallelF.setText("" + maxPCATParallel);
	maxPCATSeriesF.setText("" + maxPCATSeries);
	maxTFATF.setText("" + maxTFAT);
	maxTCCHF.setText("" + maxTCCH);
	maxTCACF.setText("" + maxTCAC);
	maxPMVoltageF.setText("" + maxPMVoltage);
	//maxFillVoltageF.setText("" + maxFillVoltage);
	//maxPuffVoltageF.setText("" + maxPuffVoltage);
	maxTempRoomF.setText("" + maxTempRoom);
	maxTempSaddleF.setText("" + maxTempSaddle);
	maxTempMagnetizingF.setText("" + maxTempMagnetizing);
	maxPOhmF.setText("" + maxPOhm);
	maxPrTimeF.setText("" + maxPrTime);
	maxTempTorF.setText("" + maxTempTor);
	maxI2TF.setText("" + maxI2T);

	maxCurrSellaVF.setText("" + maxCurrSellaV);
	maxCurrSellaVI0F.setText("" + maxCurrSellaVI0);
	maxCurrSellaPF.setText("" + maxCurrSellaP);
	maxFillPuffVoltageF.setText("" + maxFillPuffVoltage);

    }

    void saveLimits()
    {
	try
	{
	    maxPMAT = Integer.parseInt(maxPMATF.getText());
	    maxPCATParallel = Integer.parseInt(maxPCATParallelF.getText());
	    maxPCATSeries = Integer.parseInt(maxPCATSeriesF.getText());
	    maxTFAT = Integer.parseInt(maxTFATF.getText());
	    maxTCCH = Integer.parseInt(maxTCCHF.getText());
	    maxTCAC = Integer.parseInt(maxTCACF.getText());
	    maxPMVoltage = Integer.parseInt(maxPMVoltageF.getText());
	    //maxFillVoltage = Integer.parseInt(maxFillVoltageF.getText());
	    //maxPuffVoltage = Integer.parseInt(maxPuffVoltageF.getText());
	    maxTempRoom = Integer.parseInt(maxTempRoomF.getText());
	    maxTempSaddle = Integer.parseInt(maxTempSaddleF.getText());
	    maxTempMagnetizing = Integer.parseInt(maxTempMagnetizingF.getText());
	    maxPOhm = Integer.parseInt(maxPOhmF.getText());
	    maxPrTime = Integer.parseInt(maxPrTimeF.getText());
	    maxTempTor = Integer.parseInt(maxTempTorF.getText());
	    maxI2T = Integer.parseInt(maxI2TF.getText());

	    maxCurrSellaV = Integer.parseInt(maxCurrSellaVF.getText());
	    maxCurrSellaVI0 = Integer.parseInt(maxCurrSellaVI0F.getText());
	    maxCurrSellaP = Integer.parseInt(maxCurrSellaPF.getText());
	    maxFillPuffVoltage = Integer.parseInt(maxFillPuffVoltageF.getText());

	}
	catch (Exception exc)
	{
	    JOptionPane.showMessageDialog(this, "Valori errati nei limiti",
	                                  "Errore", JOptionPane.ERROR_MESSAGE);
	    return;
	}
	try
	{
	    BufferedWriter bw = new BufferedWriter(new FileWriter("rt_limits"));
	    bw.write("" + maxPMAT);
	    bw.newLine();
	    bw.write("" + maxPCATParallel);
	    bw.newLine();
	    bw.write("" + maxPCATSeries);
	    bw.newLine();
	    bw.write("" + maxTFAT);
	    bw.newLine();
	    bw.write("" + maxTCCH);
	    bw.newLine();
	    bw.write("" + maxTCAC);
	    bw.newLine();
	    bw.write("" + maxPMVoltage);
	    bw.newLine();
//            bw.write("" + maxFillVoltage);
//            bw.newLine();
//            bw.write("" + maxPuffVoltage);
//            bw.newLine();
	    bw.write("" + maxTempRoom);
	    bw.newLine();
	    bw.write("" + maxTempSaddle);
	    bw.newLine();
	    bw.write("" + maxTempMagnetizing);
	    bw.newLine();
	    bw.write("" + maxPOhm);
	    bw.newLine();
	    bw.write("" + maxPrTime);
	    bw.newLine();
	    bw.write("" + maxTempTor);
	    bw.newLine();
	    bw.write("" + maxI2T);
	    bw.newLine();

	    bw.write("" + maxCurrSellaV);
	    bw.newLine();
	    bw.write("" + maxCurrSellaVI0);
	    bw.newLine();
	    bw.write("" + maxCurrSellaP);
	    bw.newLine();
	    bw.write("" + maxFillPuffVoltage);
	    bw.newLine();

	    bw.close();
	}
	catch (Exception exc)
	{
	    System.err.println("Error saving limits: " + exc);
	}
    }

    void getRtMessages()
    {
	try
	{
	    BufferedReader br = new BufferedReader(new FileReader("rt_messages"));
	    String currLine = br.readLine();
	    String messages = "";
	    while (currLine != null)
	    {
	        messages += (currLine + "\n");
	        currLine = br.readLine();
	    }
	    messageArea.setText(messages);
	    br.close();
	}
	catch (Exception exc)
	{
	    System.err.println("Cannot get rt messages: " + exc);
	}
    }

    void saveRtMessages()
    {
	try
	{
	    BufferedWriter bw = new BufferedWriter(new FileWriter("rt_messages"));
	    String messages = messageArea.getText();
	    StringTokenizer st = new StringTokenizer(messages, "\n");
	    while (st.hasMoreTokens())
	    {
	        bw.write(st.nextToken());
	        bw.newLine();
	    }
	    bw.close();
	}
	catch (Exception exc)
	{
	    System.err.println("Error saving rt messages: " + exc);
	}
    }

    void updateDeviceNids()
    {
	try
	{
	    nids[0] = timesRoot = rfx.resolve(new PathData("\\RFX_TIMES"), 0);
//            nids[1] = poloidalControlRoot = rfx.resolve(new PathData(
//                "\\EDA1::CONTROL"), 0);
	    nids[1] = poloidalControlRoot = rfx.resolve(new PathData(
	        "\\EDA1::MARTE"), 0);
	    nids[2] = axiSetupRoot = rfx.resolve(new PathData("\\AXI_CONTROL"),
	                                         0);
	    nids[3] = pcSetupRoot = rfx.resolve(new PathData("\\PC_SETUP"), 0);
	    nids[4] = pmSetupRoot = rfx.resolve(new PathData("\\PM_SETUP"), 0);
	    nids[5] = toroidalControlRoot = rfx.resolve(new PathData(
	        "\\EDA3::CONTROL"), 0);
	    nids[6] = chopperSetupRoot = rfx.resolve(new PathData(
	        "\\CHOPPER_SETUP"), 0);
	    nids[7] = ffSetupRoot = rfx.resolve(new PathData(
	        "\\FEEDFORWARD_SETUP"), 0);
	    nids[8] = inverterSetupRoot = rfx.resolve(new PathData(
	        "\\INVERTER_SETUP"), 0);
	    nids[9] = tfSetupRoot = rfx.resolve(new PathData("\\TF_SETUP"), 0);
	    nids[10] = bfControlRoot = rfx.resolve(new PathData(
	        "\\AXI_TOROIDAL_CONTROL"), 0);
 //           nids[11] = mhdControlRoot = rfx.resolve(new PathData("\\MHD_AC::CONTROL"), 0);
	    nids[11] = mhdControlRoot = rfx.resolve(new PathData("\\MHD_AC::MARTE"), 0);
	    //mhdBcNid = rfx.resolve(new PathData("\\MHD_BC::CONTROL"), 0);
	    nids[12] = viSetupRoot = rfx.resolve(new PathData("\\VI_SETUP"), 0);
	    nids[13] = pelletSetupRoot = rfx.resolve(new PathData("\\PELLET_SETUP"), 0);
	    nids[14] = diagTimesSetupRoot = rfx.resolve(new PathData("\\DIAG_TIMES_SETUP"), 0);
	    nids[15] = ipSetupRoot = rfx.resolve(new PathData("\\IP_CONTROL"), 0);
	    nids[16] = mopRoot = rfx.resolve(new PathData("\\MOP"), 0);

	    nids[17] = ansaldoConfigRoot = rfx.resolve(new PathData("\\ANSALDO"),
	        0);
	    nids[18] = unitsConfigRoot = rfx.resolve(new PathData("\\ABUNITS"),
	        0);
	    nids[19] = poloidalConfigRoot = rfx.resolve(new PathData(
	        "\\P_CONFIG"), 0);
	    nids[20] = toroidalConfigRoot = rfx.resolve(new PathData(
	        "\\T_CONFIG"), 0);
	    nids[21] = mhdConfigRoot = rfx.resolve(new PathData("\\PR_CONFIG"),
	        0);
	    nids[22] = viConfigRoot = rfx.resolve(new PathData("\\VI_CONFIG"),
	                                      0);
	    pvSetupRoot = rfx.resolve(new PathData("\\PV_SETUP"),
	                                       0);

	}
	catch (Exception exc)
	{
			exc.printStackTrace();
	    System.err.println("Error opening device");
	    System.exit(0);
	}
    }

    void init()
    {
	try
	{
	    if (isRt)
	    {
	        rfx = new Database("RFX", -1);
	        rfx.open();
	    }
	    else
	    {
	        if(isOnline)
	        {
	            rfx = new Database("RFX", -1);
	            rfx.open();
	            rfx.create(100);
	            rfx.close(0);
	            rfx = new Database("RFX", 100);
	            rfx.open();
	            try {
	                NidData nidD = rfx.resolve(new PathData("\\INFO:REF_SHOT"), 0);
	                int refShot = rfx.getData(nidD, 0).getInt();
	                refShotLabel.setText(refShotLabelText+ refShot);
	            }catch(Exception exc){}
	        }
	        else
	        {
	            rfx = new Database("RFX", -1);
	            rfx.open();
	            rfx.create(101);
	            rfx.close(0);
	            rfx = new Database("RFX", 101);
	            rfx.open();
	        }
	    }
	    rfx.open();
	}
	catch (Exception exc)
	{
	    System.err.println("Cannot open RFX");
	    System.exit(0);
	}
	updateDeviceNids();
	saveSetup(currSetupHash, currSetupOnHash);
	getCurrFFState();

	if (isRt)
	    setTitle("RFX Parameters -- RT --    shot: " + getShot());
	else
	    setTitle("RFX Parameters  shot: " + shot);
    }


    void saveSetup(Hashtable setupHash, Hashtable setupOnHash)
    {
	for (int i = 0; i < NUM_SETUP; i++)
	    saveSetup(i, setupHash, setupOnHash);
    }

    void saveSetupAndConfig(Hashtable setupHash, Hashtable setupOnHash)
    {
	for (int i = 0; i < NUM_DEVICES; i++)
	    saveSetup(i, setupHash, setupOnHash);
    }

    void loadSetup()
    {
	chooser.rescanCurrentDirectory();
	chooser.setApproveButtonText("Load Configuration");
	int returnVal = chooser.showOpenDialog(this);
	if (returnVal == JFileChooser.APPROVE_OPTION)
	{
	    String filePath = chooser.getSelectedFile().getPath();
	    if (!filePath.toLowerCase().endsWith(".rfx"))
	        filePath += ".rfx";
	    readSetupFromFile(filePath);
	    loadSelectedSetup();
	    loadSelected.setEnabledDevicesForSavingConfiguration();
	}
    }

    static class RfxFileFilter
	extends javax.swing.filechooser.FileFilter
    {
	public boolean accept(File f)
	{
	    if (f.isDirectory())return true;
	    return f.getName().endsWith(".rfx");
	}

	public String getDescription()
	{
	    return "Saved configurations for RFX";
	}
    }

//Class SelectSetup defines the check boxes for selective load and save
    class SelectSetup
	extends JDialog
    {
	JCheckBox
	    poloidalCB,
	    axiCB,
	    pcCB,
	    pmCB,
	    toroidalCB,
	    chopperCB,
	    feedForwardCB,
	    inverterCB,
	    tfCB,
	    bfCB,
	    mhdCB,
	    viCB,
	    pelletCB,
	    diagTimesCB,
	    ipCB,
	    timesPmCB,
	    timesPcCB,
	    timesPvCB,
	    timesPpCB,
	    timesPrCB,
	    timesPtsoCB,
	    timesPtcbCB,
	    timesPtctCB,
	    timesGasCB,
	    timesTfCB,
	    timesIsCB,
	    timesChopperCB,
	    timesInverterCB;

	JCheckBox currFFCB;

	JCheckBox[] checkBoxes = new JCheckBox[NUM_SETUP-1];
	JCheckBox mhdDecoupingCheckBox;
	JCheckBox[] timeCheckBoxes = new JCheckBox[13];

	SelectSetup(ActionListener actionListener)
	{
	    super(ParameterSetting.this, "Select components");
	    JPanel jp1 = new JPanel();
	    jp1.setLayout(new GridLayout(1, 2));
	    JPanel jp = new JPanel();
	    jp.setLayout(new GridLayout(NUM_SETUP, 1));
	    jp.add(checkBoxes[0] = poloidalCB = new JCheckBox("EDA1", true));
	    jp.add(checkBoxes[1] = axiCB = new JCheckBox("Axisymmetric contr.", true));
	    jp.add(checkBoxes[2] = pcCB = new JCheckBox("PC", true));
	    jp.add(checkBoxes[3] = pmCB = new JCheckBox("PM", true));
	    jp.add(checkBoxes[4] = toroidalCB = new JCheckBox("EDA3", true));
	    jp.add(checkBoxes[5] = chopperCB = new JCheckBox("Chopper", true));
	    jp.add(checkBoxes[6] = feedForwardCB = new JCheckBox("Feed Forward", true));
	    jp.add(checkBoxes[7] = inverterCB = new JCheckBox("Inverter", true));
	    jp.add(checkBoxes[8] = tfCB = new JCheckBox("TF", true));
	    jp.add(checkBoxes[9] = bfCB = new JCheckBox("B & F", true));
	    jp.add(checkBoxes[10] = mhdCB = new JCheckBox("MHD", true));
	    jp.add(checkBoxes[11] = viCB = new JCheckBox("VI", true));
	    jp.add(checkBoxes[12] = pelletCB = new JCheckBox("Pellet", true));
	    jp.add(checkBoxes[13] = diagTimesCB = new JCheckBox("Diag. times", true));
	    jp.add(checkBoxes[14] = ipCB = new JCheckBox("IP Control", true));
	    jp.add(currFFCB = new JCheckBox("MHD FF", true));
	    jp1.add(jp);

	    jp = new JPanel();
	    jp.setLayout(new GridLayout(13, 1));
	    jp.add(timeCheckBoxes[0] = timesPmCB = new JCheckBox("Times: PM", true));
	    jp.add(timeCheckBoxes[1] = timesPcCB = new JCheckBox("Times: PC", true));
	    jp.add(timeCheckBoxes[2] = timesPvCB = new JCheckBox("Times: PV", true));
	    jp.add(timeCheckBoxes[3] = timesPpCB = new JCheckBox("Times: PP", true));
	    jp.add(timeCheckBoxes[4] = timesPrCB = new JCheckBox("Times: PR", true));
	    jp.add(timeCheckBoxes[5] = timesPtsoCB = new JCheckBox(
	        "Times: PTSO", true));
	    jp.add(timeCheckBoxes[6] = timesPtcbCB = new JCheckBox(
	        "Times: PTCB", true));
	    jp.add(timeCheckBoxes[7] = timesPtctCB = new JCheckBox(
	        "Times: PTCT", true));
	    jp.add(timeCheckBoxes[8] = timesGasCB = new JCheckBox("Times: Gas", true));
	    jp.add(timeCheckBoxes[9] = timesTfCB = new JCheckBox("Times: TF", true));
	    jp.add(timeCheckBoxes[10] = timesIsCB = new JCheckBox("Times: IS", true));
	    jp.add(timeCheckBoxes[11] = timesChopperCB = new JCheckBox(
	        "Times: Chopper", true));
	    jp.add(timeCheckBoxes[12] = timesInverterCB = new JCheckBox(
	        "Times: Inverter", true));

	    jp1.add(jp);

	    getContentPane().add(jp1, "Center");
	    jp = new JPanel();
	    JButton deselectAllB = new JButton("Deselect All");
	    deselectAllB.addActionListener(new ActionListener() {
	        public void actionPerformed(ActionEvent e)
	        {
	            //for(int i = 0; i < 14; i++)
	            for(int i = 0; i < 15; i++)
	                checkBoxes[i].setSelected(false);
	            for(int i = 0; i < 13; i++)
	                timeCheckBoxes[i].setSelected(false);
	            currFFCB.setSelected(false);
	        }
	    });
	    jp.add(deselectAllB);
	    JButton saveB = new JButton("Save");
	    saveB.addActionListener(actionListener);
	    jp.add(saveB);
	    JButton cancelB = new JButton("Cancel");
	    cancelB.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            setVisible(false);
	        }
	    });
	    jp.add(cancelB);
	    getContentPane().add(jp, "South");
	    pack();
	}

	boolean[] getSelectedDevices()
	{
	    return new boolean[]
	        {
	        poloidalCB.isSelected(),
	        axiCB.isSelected(),
	        pcCB.isSelected(),
	        pmCB.isSelected(),
	        toroidalCB.isSelected(),
	        chopperCB.isSelected(),
	        feedForwardCB.isSelected(),
	        inverterCB.isSelected(),
	        tfCB.isSelected(),
	        bfCB.isSelected(),
	        mhdCB.isSelected(),
	        viCB.isSelected(),
	        pelletCB.isSelected(),
	        diagTimesCB.isSelected(),
	        ipCB.isSelected()
	    };
	}

	boolean[] getSelectedTimes()
	{
	    return new boolean[]
	        {
	        timesPmCB.isSelected(),
	        timesPcCB.isSelected(),
	        timesPvCB.isSelected(),
	        timesPpCB.isSelected(),
	        timesPrCB.isSelected(),
	        timesPtsoCB.isSelected(),
	        timesPtcbCB.isSelected(),
	        timesPtctCB.isSelected(),
	        timesGasCB.isSelected(),
	        timesTfCB.isSelected(),
	        timesIsCB.isSelected(),
	        timesChopperCB.isSelected(),
	        timesInverterCB.isSelected()};
	}

	boolean currFFSelected()
	{
	    return currFFCB.isSelected();
	}
	void setEnabledDevicesForSavingConfiguration()
	{
	    for (int i = 0; i < NUM_SETUP-1; i++)
	    {
	        checkBoxes[i].setSelected(true);
	        checkBoxes[i].setEnabled(true);
	    }
	    for (int i = 0; i < 13; i++)
	    {
	        timeCheckBoxes[i].setSelected(true);
	        timeCheckBoxes[i].setEnabled(true);
	    }
	    currFFCB.setSelected(true);
	    checkBoxes[0].setSelected(false);
	    checkBoxes[0].setEnabled(false);
	    timeCheckBoxes[5].setSelected(true);
	    timeCheckBoxes[5].setEnabled(false);
	}

	void setEnabledDevices(Hashtable setupHash)
	{
	    for (int i = 0; i < NUM_SETUP-1; i++)
	    {
	        checkBoxes[i].setSelected(false);
	        checkBoxes[i].setEnabled(false);
	    }


/*            Enumeration mapNames = mapSetupHash.keys();
	    System.out.println("\n\n\nMAP CONTENT");
	     while (mapNames.hasMoreElements())
	        System.out.println((String)mapNames.nextElement());


*/

	    Enumeration pathNames = setupHash.keys();
	    while (pathNames.hasMoreElements())
	    {
	        String currPathName = (String)pathNames.nextElement();
 //System.out.println("Setup Hash: " + currPathName);


//                Integer currInt = (Integer) mapSetupHash.get(pathNames.
//                    nextElement());
	        Integer currInt = (Integer) mapSetupHash.get(currPathName);
 if(currInt == null)
     System.out.println("MISSING IDX for "+currPathName);
	        if (currInt != null)
	        {
	            int idx = currInt.intValue();
	            if (idx > 0)
	            {
	                checkBoxes[idx - 1].setEnabled(true);
	                checkBoxes[idx - 1].setSelected(true);
	            }
	        }
	    }
	    currFFCB.setSelected(true);
	}

	void setEnabledTimes(Hashtable setupHash)
	{
	    for (int i = 0; i < 13; i++)
	    {
	        timeCheckBoxes[i].setSelected(false);
	        timeCheckBoxes[i].setEnabled(false);
	    }
	    int nids[] = new int[setupHash.size()];
	    Enumeration pathNames = setupHash.keys();
	    int idx = 0;
	    while (pathNames.hasMoreElements())
	    {
	        String currPath = (String) pathNames.nextElement();
	        try
	        {
	            NidData currNid = rfx.resolve(new PathData(currPath), 0);
	            nids[idx] = currNid.getInt();
	            idx++;
	        }
	        catch (Exception exc)
	        {
	            System.err.println("Internal error in setEnabledTimes for " +currPath +": " +
	                               exc);
	        }
	    }
	    enable(nids, pm_mask, 0);
	    enable(nids, pc_mask, 1);
	    enable(nids, pv_mask, 2);
	    enable(nids, pp_mask, 3);
	    enable(nids, pr_mask, 4);
	    enable(nids, ptso_mask, 5);
	    //MAY 2009 Force PTSO NOT TO BE ENABLED
	    timeCheckBoxes[5].setEnabled(false);

	    enable(nids, ptcb_mask, 6);
	    enable(nids, ptct_mask, 7);
	    enable(nids, gas_mask, 8);
	    enable(nids, tf_mask, 9);
	    enable(nids, is_mask, 10);
	    enable(nids, chopper_mask, 11);
	    enable(nids, inverter_mask, 12);

	}

	void enable(int[] nids, int[] mask, int idx)
	{
	    int baseNid = ParameterSetting.this.nids[0].getInt();
	    for (int i = 0; i < mask.length; i++)
	    {
	        for (int j = 0; j < nids.length; j++)
	        {
	            if (nids[j] - baseNid == mask[i])
	            {
	                timeCheckBoxes[idx].setEnabled(true);
	                timeCheckBoxes[idx].setSelected(true);
	            }
	        }
	    }
	}
    } //End class SelectSetup

    void saveSelectedSetup()
    {
	if (saveSelected == null)
	{
	    saveSelected = new SelectSetup(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            log("Save Configuration");
	            boolean[] selectedDevices = saveSelected.getSelectedDevices();
	            boolean[] selectedTimes = saveSelected.getSelectedTimes();
	            saveSetup(selectedDevices, selectedTimes);
	            saveSelected.setVisible(false);
	        }
	    });
	    saveSelected.setEnabledDevicesForSavingConfiguration();
	}
	saveSelected.setVisible(true);
    }

    void loadSelectedSetup()
    {
	if (loadSelected == null)
	{
	    loadSelected = new SelectSetup(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            log("Load Pulse");
	            boolean[] selectedDevices = loadSelected.getSelectedDevices();
	            boolean[] selectedTimes = loadSelected.getSelectedTimes();
	            applySetup(currSetupHash, currSetupOnHash, selectedDevices,
	                       selectedTimes);

	            if(loadSelected.currFFSelected())
	            {
	                loadCurrFF(currLoadShot);
	            }

	            checkVersions();
	           loadSelected.setVisible(false);

	            if (isOnline) {
	                String decouplingName = getDecouplingName(currLoadShot);
	                if(decouplingName == null) decouplingName = "Unknown";
	                if (JOptionPane.showConfirmDialog(ParameterSetting.this,
	                        "Caricare MHD Decoupling da shot " + currLoadShot +
	                        " ("+decouplingName + ")?",
	                        "Decoupling", JOptionPane.YES_NO_OPTION)
	                    == JOptionPane.YES_OPTION) {
	                    copyDecoupling(currLoadShot, shot);
	                }

	                /*if (JOptionPane.showConfirmDialog(ParameterSetting.this,
	                                                  "Stampare la scheda di caricamento impulso?",
	                                                  "Caricamento impulso",
	                                                  JOptionPane.YES_NO_OPTION) ==
	                    JOptionPane.YES_OPTION) {
	                    try {
	                        printLoadPulse(currLoadShot);
	                    } catch (Exception exc) {
	                        System.err.println("Error printing form: " + exc);
	                    }
	                }*/
	                //Report saved shot
	                refShotLabel.setText(refShotLabelText + currLoadShot);
//                        i2tEvaluateResidualPrePulse();
	             }
	        }
	    });
	}
	loadSelected.setEnabledDevices(currSetupHash);
	//loadSelected.setEnabledTimes(currSetupHash); //May 2009 Use currSetupOnHash for includiong PTSO
	loadSelected.setEnabledTimes(currSetupOnHash);
	loadSelected.setVisible(true);
    }

    void saveSetup(boolean select[], boolean timeSelect[])
    {
	chooser.rescanCurrentDirectory();
	chooser.setApproveButtonText("Save");
	int returnVal = chooser.showSaveDialog(this);
	if (returnVal == JFileChooser.APPROVE_OPTION)
	{
	    currSetupHash = new Hashtable();
	    currSetupOnHash = new Hashtable();
	    for (int i = 1; i < NUM_SETUP; i++)
	        if (select[i - 1]) saveSetup(i, currSetupHash, currSetupOnHash);

	    //Timing components
	    if (timeSelect[0]) saveSetup(0, pm_mask, currSetupHash,
	                                 currSetupOnHash);
	    if (timeSelect[1]) saveSetup(0, pc_mask, currSetupHash,
	                                 currSetupOnHash);
	    if (timeSelect[2]) saveSetup(0, pv_mask, currSetupHash,
	                                 currSetupOnHash);
	    if (timeSelect[3]) saveSetup(0, pp_mask, currSetupHash,
	                                 currSetupOnHash);
	    if (timeSelect[4]) saveSetup(0, pr_mask, currSetupHash,
	                                 currSetupOnHash);
	    if (timeSelect[5]) saveSetup(0, ptso_mask, currSetupHash,
	                                 currSetupOnHash);

	    if (timeSelect[6]) saveSetup(0, ptcb_mask, currSetupHash,
	                                 currSetupOnHash);
	    if (timeSelect[7]) saveSetup(0, ptct_mask, currSetupHash,
	                                 currSetupOnHash);
	    if (timeSelect[8]) saveSetup(0, gas_mask, currSetupHash,
	                                 currSetupOnHash);
	    if (timeSelect[9]) saveSetup(0, tf_mask, currSetupHash,
	                                 currSetupOnHash);
	    if (timeSelect[10]) saveSetup(0, is_mask, currSetupHash,
	                                  currSetupOnHash);
	    if (timeSelect[11]) saveSetup(0, chopper_mask, currSetupHash,
	                                  currSetupOnHash);
	    if (timeSelect[12]) saveSetup(0, inverter_mask, currSetupHash,
	                                  currSetupOnHash);

	    String filePath = chooser.getSelectedFile().getPath();
	    if (!filePath.toLowerCase().endsWith(".rfx"))
	        filePath += ".rfx";
	    writeSetupToFile(filePath);
	}
    }

    void getSetupForModel(Hashtable setupHash, Hashtable setupOnHash,
	                  boolean select[], boolean timeSelect[])
    {
	for (int i = 1; i < NUM_SETUP; i++)
	    if (select[i - 1]) saveSetup(i, setupHash, setupOnHash);

	//Timing components
	if (timeSelect[0]) saveSetup(0, pm_mask, setupHash, setupOnHash);
	if (timeSelect[1]) saveSetup(0, pc_mask, setupHash, setupOnHash);
	if (timeSelect[2]) saveSetup(0, pv_mask, setupHash, setupOnHash);
	if (timeSelect[3]) saveSetup(0, pp_mask, setupHash, setupOnHash);
	if (timeSelect[4]) saveSetup(0, pr_mask, setupHash, setupOnHash);
	if (timeSelect[5]) saveSetup(0, ptso_mask, setupHash, setupOnHash);
	if (timeSelect[6]) saveSetup(0, ptcb_mask, setupHash, setupOnHash);
	if (timeSelect[7]) saveSetup(0, ptct_mask, setupHash, setupOnHash);
	if (timeSelect[8]) saveSetup(0, gas_mask, setupHash, setupOnHash);
	if (timeSelect[9]) saveSetup(0, tf_mask, setupHash, setupOnHash);
	if (timeSelect[10]) saveSetup(0, is_mask, setupHash, setupOnHash);
	if (timeSelect[11]) saveSetup(0, chopper_mask, setupHash, setupOnHash);
	if (timeSelect[12]) saveSetup(0, inverter_mask, setupHash, setupOnHash);


    }

    void writeSetupToFile(String fileName)
    {
	try
	{
	    ObjectOutputStream oos = new ObjectOutputStream(new
	        FileOutputStream(fileName));
	    oos.writeObject(currSetupHash);
	    oos.writeObject(currSetupOnHash);
	    oos.close();
	}
	catch (Exception exc)
	{
	    JOptionPane.showMessageDialog(this,
	                                  "Cannot save configuration: " + exc,
	                                  "Write error",
	                                  JOptionPane.WARNING_MESSAGE);
	}
    }

    void readSetupFromFile(String fileName)
    {
	try
	{
	    ObjectInputStream ois = new ObjectInputStream(new FileInputStream(
	        fileName));
	    currSetupHash = (Hashtable) ois.readObject();
	    currSetupOnHash = (Hashtable) ois.readObject();
	    ois.close();
	}
	catch (Exception exc)
	{
	    JOptionPane.showMessageDialog(this,
	                                  "Cannot load configuration: " + exc,
	                                  "Read error",
	                                  JOptionPane.WARNING_MESSAGE);
	}
    }

    void handleDeviceClosed(int idx, boolean isChanged)
    {
	log("Device " + idx + " Closed");
	if (isRt)
	{
	    if (idx < NUM_SETUP) //Setup devices
	    {
	        if (states[idx] == CHECKING)
	        {
	            states[idx] = CHECKED;
	            buttons[idx].setForeground(Color.black);
	        }

	        if(modifiedNids != null && modifiedNids.length > 0)
	            devices[idx].setHighlight(false, modifiedNids);
	    }
	}
     }

    void setUncheckedRt(int idx)
    {
	if (rtDos != null)
	{
	    try
	    {
	        rtDos.writeInt(idx);
	    }
	    catch (Exception exc)
	    {
	        rtDos = null;
	        handleNotRt();
	        //Trye to resend message
	        try {
	            rtDos.writeInt(idx);
	        }catch(Exception exc1){}
	    }
	}
    }

    void handleNotRt()
    {
	try
	{
	    rtSock = new Socket(rtIp, 4000);
	    rtDos = new DataOutputStream(rtSock.getOutputStream());
	}
	catch (Exception exc)
	{
	    rtDos = null;
	    java.util.Timer timer = new java.util.Timer();
	    timer.schedule(new TimerTask()
	    {
	        public void run()
	        {
	            handleNotRt();
	        }
	    }, 5000);
	}
	System.out.println("handleNotRt Finito");
    }

    void handleRt()
    {
	(new RtHandler()).start();
    }

    boolean allChecked()
    {
	for (int i = 0; i < states.length; i++)
	    if (states[i] != CHECKED)
	        return false;
	return true;
    }

    class RtHandler
	extends Thread
    {
	int currIdx;
	public void run()
	{
	    try
	    {
	        ServerSocket serverSock = new ServerSocket(4000);
	        while (true)
	        {
	            Socket sock = serverSock.accept();
	            DataInputStream dis = new DataInputStream(sock.
	                getInputStream());
	            try
	            {
	                while (true)
	                {
	                    //The index of the changed forms are passed
	                    currIdx = dis.readInt();
	                    if (currIdx >= 0)
	                    {
	                        System.out.println("RECEIVED UNCHECK " +
	                            currIdx);

	                        states[currIdx] = UNCHECKED;
	                        SwingUtilities.invokeAndWait(new Runnable()
	                        {
	                            public void run()
	                            {
	                                System.out.println("ADESSO METTO ROSSO");
	                                buttons[currIdx].setForeground(Color.
	                                    red);
	                            }
	                        });
	                    }
	                    else if(currIdx == -1)//Going to receive the list of modified nids
	                    {
	                        int numModifiedNids = dis.readInt();
	                        modifiedNids = new int[numModifiedNids];
	                        for(int i = 0; i < numModifiedNids; i++)
	                            modifiedNids[i] = dis.readInt();
	                        getCurrFFState();
	                    }
	                    else //currIdx == -2. Notified and of applyToModel
	                       getCurrFFState();
	                }
	            }
	            catch (Exception exc)
	            {
	                System.out.println("Client exited");
	            }
	        }
	    }
	    catch (Exception exc)
	    {
	        System.err.println("Error accepting socket connections");
	    }
	}
    } // End class RtHandler

    void handleAlarms()
    {
	(new AlarmHandler()).start();
    }

    class AlarmHandler
	extends Thread
    {
	DataInputStream dis;
	DataOutputStream dos;
	public void run()
	{
	    try
	    {
	        ServerSocket serverSock;
	        if (isRt)
	            serverSock = new ServerSocket(4003);
	        else
	            serverSock = new ServerSocket(4004);
	        while (true)
	        {
	            Socket sock = serverSock.accept();
	            dis = new DataInputStream(sock.
	                                      getInputStream());
	            try
	            {
	                while (true)
	                {
	                    String message = dis.readUTF();
	                    JOptionPane.showMessageDialog(ParameterSetting.this, message, "Warning",
	                        JOptionPane.WARNING_MESSAGE);
	                }
	            }
	            catch (Exception exc)
	            {
	                System.out.println("Client exited");
	            }
	        }
	    }
	    catch (Exception exc)
	    {
	        System.err.println("Error accepting socket connections");
	    }
	}
    }




    static final int ENTER_PAS = 1, LEAVE_PAS = 2, ENTER_PRE = 3, LEAVE_SECONDARY = 4;
    void handleScheduler()
    {
	(new SchedulerHandler()).start();
    }

    class SchedulerHandler
	extends Thread
    {
	DataInputStream dis;
	DataOutputStream dos;
	public void run()
	{
	    try
	    {
	        ServerSocket serverSock;
	        if (isRt)
	            serverSock = new ServerSocket(4001);
	        else
	            serverSock = new ServerSocket(4002);
	        while (true)
	        {
	            Socket sock = serverSock.accept();
	            dis = new DataInputStream(sock.
	                                      getInputStream());
	            dos = new DataOutputStream(sock.
	                                       getOutputStream());
	            try
	            {
	                while (true)
	                {
	                    //The index of the changed forms are passed
	                    int phaseIdx = dis.readInt();
	                    switch (phaseIdx)
	                    {
	                        case LEAVE_PAS:
	                        {
	                            if (isRt)
	                            {
	                                if (!allChecked())
	                                {
	                                    checkedWd = new WarningDialog(
	                                        ParameterSetting.this,
	                                        "Una o piu' form di impostazione non sono state verificate");
	                                    checkedWd.addActionListener(new
	                                        ActionListener()
	                                    {
	                                        public void actionPerformed(
	                                            ActionEvent
	                                            e)
	                                        {
	                                            if (allChecked())
	                                            {
	                                                checkedWd.dispose();
	                                                //proceedeConfig();
	                                                proceedeLimits();
	                                            }
	                                        }
	                                    });
	                                    checkedWd.pack();
	                                    checkedWd.setVisible(true);
	                                }
	                                else
	                                {
	                                    //proceedeConfig();
	                                    proceedeLimits();
	                                }
	                            }
	                            else
	                            {
	                                applyToModelItem.setEnabled(false);
	                                if (isOnline)
	                                    applyToModelB.setEnabled(false);
	                                revertModelItem.setEnabled(false);
	                            }
	                            break;
	                        }
	                        case ENTER_PRE:
	                            //doingShot = true;
	                        case ENTER_PAS:
	                            if (!isRt)
	                            {
	                                applyToModelItem.setEnabled(true);
	                                if (isOnline)
	                                    applyToModelB.setEnabled(true);
	                                revertModelItem.setEnabled(true);
	                                setTitle("RFX Parameters     shot: " +
	                                         getShot());
	                            }
	                            else
	                                setTitle(
	                                    "RFX Parameters -- RT --  shot: " +
	                                    getShot());

	                            break;
	                        case LEAVE_SECONDARY:
	                            doingShot = false;
 //                                   i2tEvaluateResidualPostPulse();
	                            if (!isRt)
	                                setTitle("RFX Parameters     shot: " +
	                                         getShot());
	                            else
	                                setTitle(
	                                    "RFX Parameters -- RT --  shot: " +
	                                    getShot());
	                    }
	                }
	            }
	            catch (Exception exc)
	            {
	                System.out.println("Client exited");
	            }
	        }
	    }
	    catch (Exception exc)
	    {
	        System.err.println("Error accepting socket connections");
	    }
	}

	void proceedeLimits()
	{
	    String limitsMsg = checkLimits();
	    if (limitsMsg != null)
	    {
	        limitsWd = new WarningDialog(ParameterSetting.this, limitsMsg);
	        limitsWd.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                String newMsg = checkLimits();
	                if (newMsg != null)
	                    limitsWd.setText(newMsg);
	                else
	                {
	                    limitsWd.dispose();
	                    proceedeConfirm();
	                }
	            }
	        });
	        limitsWd.pack();
	        limitsWd.setVisible(true);
	    }
	    else
	        proceedeVersions();
 //               proceedeConfirm();
	}
	void proceedeVersions()
	{
	    String versionMsg = checkVersionsForPas();
	    if (versionMsg != null)
	    {
	        versionWd = new WarningDialog(ParameterSetting.this, versionMsg);
	        versionWd.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                String newMsg = checkVersionsForPas();
	                if (newMsg != null)
	                    versionWd.setText(newMsg);
	                else
	                {
	                    versionWd.dispose();
	                    proceedeConfirm();
	                }
	            }
	        });
	        versionWd.pack();
	        versionWd.setVisible(true);
	    }
	    else
	        proceedeConfirm();
	}

		private String executePulseCheck()
		{

			try {
				Data msgData = rfx.evaluateData(Data.fromExpr("ParameterSettingCheck()"), 0);
				String s = msgData.toString();
				s = s.substring(1, s.length() - 1);
				StringTokenizer st =  new StringTokenizer(s, "#");
				String out="";
				while( st.hasMoreTokens() )
				{
					out += st.nextToken() + "\n";
				}
				return out;
			}catch(Exception exc)
			{
				System.err.println("Error evalutaing ParameterSettingCheck function : " + exc);
				return "";
			}
		}

	void proceedeConfirm()
	{

			String msg = executePulseCheck();

			System.out.println("Messaggio = " + msg);

			if (msg != null && msg.length() > 0)
				msg = "Transitare dal PAS (Corrente Magnetizzante: " +
	        getMagnetizingCurrent() + " A)?\n\n\tATTENZIONE\n\n" + msg;
			else
				msg = "Transitare dal PAS (Corrente Magnetizzante: " +
	        getMagnetizingCurrent() + " A)?\n\n";

	    JOptionPane.showConfirmDialog(
	        ParameterSetting.this,
				msg,
	        "Acknowledgement request",
	        JOptionPane.YES_OPTION);


/* Taliercio 6 - 2 - 2009
	    JOptionPane.showConfirmDialog(
	        ParameterSetting.this,
	        "Transitare dal PAS (Corrente Magnetizzante: " +
	        getMagnetizingCurrent() + " A)?",
	        "Acknowledgement request",
	        JOptionPane.YES_OPTION);
*/
	    try
	    {
	        dos.writeInt(1);
	        dos.flush();
	    }
	    catch (Exception exc)
	    {
	        System.err.println("Error sending scheduler answer: " + exc);
	    }
	}
    } // End SchedulerHandler

    void setReadOnly(boolean readOnly)
    {
	System.out.println("SET READ ONLY ");
	this.readOnly = readOnly;
	for (int i = 0; i < nids.length; i++)
	{
	    DeviceSetup device = DeviceSetup.getDevice(nids[i].getInt());
	    if (device != null)
	    {
	        System.out.println("SET READ ONLY " + readOnly);

	        device.setReadOnly(readOnly);
	    }
	}
    }

    void printSetup()
    {
	PrintRequestAttributeSet dialogAttributes = new
	    HashPrintRequestAttributeSet(),
	    printAttributes = new HashPrintRequestAttributeSet();
	printService = ServiceUI.printDialog(null, 50, 50,
	                                     PrintServiceLookup.
	                                     lookupPrintServices(null,
	    printAttributes), printService,
	                                     null,
	                                     dialogAttributes);

    }

    public int print(Graphics g, PageFormat pf, int pageIndex) throws
	PrinterException
    {
	//if(currPrintDeviceIdx == 7 || currPrintDeviceIdx == 14)
	//  pf.setOrientation(PageFormat.LANDSCAPE);
	double height = pf.getImageableHeight();
	double width = pf.getImageableWidth();
	Graphics2D g2 = (Graphics2D) g;

	if (pageIndex == 0)
	{
	    g2.translate(pf.getImageableX(), pf.getImageableY());

	    Font prevFont = g2.getFont();
	    Font titleFont = new Font("Serif", Font.BOLD, 30);
	    g2.setFont(titleFont);
	    FontMetrics titleFontMetrics = g2.getFontMetrics();

	    int titleWidth, titleHeight;
System.out.println("Printing title...");
	    if(currPrintDeviceIdx >= 0)
	    {
	        titleWidth = titleFontMetrics.stringWidth(titles[
	                                                      currPrintDeviceIdx]);
	        titleHeight = titleFontMetrics.getHeight();

	        g2.drawString(titles[currPrintDeviceIdx],
	                      (int) width / 2 - titleWidth / 2, titleHeight);
	    }
	    else //Scheda ripetizione impulso
	    {
	        titleWidth = titleFontMetrics.stringWidth("Ripetizione Impulso");
	        titleHeight = titleFontMetrics.getHeight();

	        g2.drawString("Ripetizione Impulso",
	                      (int) width / 2 - titleWidth / 2, titleHeight);

	    }
	    Font infoFont = new Font("Serif", Font.BOLD, 20);
	    g2.setFont(infoFont);
	    FontMetrics infoFontMetrics = g2.getFontMetrics();

System.out.println("Printing Date..." + currPrintDeviceIdx);
	    int infoHeight = infoFontMetrics.getHeight();
	    g2.drawString("Data: " +
	                  new SimpleDateFormat("dd/MM/yyy").format(new Date()) +
	                  "          Impulso: " + getShot(),
	                  0, 2 * titleHeight + 10);

/*            if (currPrintDeviceIdx < 13)
	    {
	        g2.drawString("RT: ",
	                      0, 2 * titleHeight + 20 + infoHeight);
	        int rtWidth = infoFontMetrics.stringWidth("RT: ");
	        g2.drawLine(rtWidth, 2 * titleHeight + 20 + infoHeight,
	                    rtWidth + 150, 2 * titleHeight + 20 + infoHeight);
	    }
*/

	    if(currPrintDeviceIdx >= 0)
	    {

	        g2.setFont(prevFont);

	        g2.translate(0, 2 * titleHeight + 30 + infoHeight);

	        g2.scale( ( (double) width) / devices[currPrintDeviceIdx].getWidth(),
	                 ( (double) (height - (2 * titleHeight + 30 + infoHeight)) /
	                  devices[currPrintDeviceIdx].getHeight()));
System.out.println("Printing Device...");

	        devices[currPrintDeviceIdx].printAll(g2);
	    }
	    else //Scheda ripetizione impulso
	    {
	        g2.drawString("Configurazione caricata dall'impulso " + currPrintLoadPulse,
	                      0, 3 * titleHeight + 10);

	    }
System.out.println("Print Done");


	    return Printable.PAGE_EXISTS;
	}
	else
	    return Printable.NO_SUCH_PAGE;
    }

    void print(int idx) throws PrinterException, PrintException
    {
	currPrintDeviceIdx = idx;
	DocFlavor flavor = DocFlavor.SERVICE_FORMATTED.PRINTABLE;
	Doc doc = new SimpleDoc(this, flavor, null);
	DocPrintJob prnJob = printService.createPrintJob();
	prnJob.print(doc, null);
    }

    void printLoadPulse(int shot) throws PrinterException, PrintException
    {
	int prevPrintDeviceIdx = currPrintDeviceIdx;
	currPrintDeviceIdx = -1;
	currPrintLoadPulse = shot;
	DocFlavor flavor = DocFlavor.SERVICE_FORMATTED.PRINTABLE;
	Doc doc = new SimpleDoc(this, flavor, null);
	DocPrintJob prnJob = printService.createPrintJob();
	prnJob.print(doc, null);
	currPrintDeviceIdx = prevPrintDeviceIdx;
    }

    class PrintButton
	extends JButton implements ActionListener
    {
	int idx;
	PrintButton(int idx)
	{
	    super("Print");
	    this.idx = idx;
	    addActionListener(this);
	}

	public void actionPerformed(ActionEvent e)
	{
	    try
	    {
	        log("Print device "+idx);
	        ParameterSetting.this.print(idx);
	    }
	    catch (Exception exc)
	    {
	        System.err.println("Cannot print device: " + exc);
	    }
	}
    }

    void applySetup(Hashtable setupHash, Hashtable setupOnHash)
    {
	Enumeration pathNames = setupHash.keys();
	String currPath = "", currDecompiled = "", savedDecompiled = "";
	while (pathNames.hasMoreElements())
	{
	    try
	    {
	        currPath = (String) pathNames.nextElement();

System.out.println("APPLY SETUP: "+currPath);
	        NidData currNid = rfx.resolve(new PathData(currPath), 0);
	        currDecompiled = (String) setupHash.get(currPath);

	        try
	        {
	            savedDecompiled = (rfx.getData(currNid, 0)).toString();
	        }
	        catch (Exception exc)
	        {
	            savedDecompiled = "";
	        }

	        if (!currDecompiled.equals(savedDecompiled))
	        {
	            Data currData = Data.fromExpr(currDecompiled);
	            rfx.putData(currNid, currData, 0);
	        }
	    }
	    catch (Exception exc)
	    {
	        System.err.println("Error applying configuration: " + exc +
	                           currPath + "  " + currDecompiled + "  " +
	                           savedDecompiled);
	    }
	}
	pathNames = setupOnHash.keys();
	while (pathNames.hasMoreElements())
	{
	    try
	    {
	        currPath = (String) pathNames.nextElement();
	        NidData currNid = rfx.resolve(new PathData(currPath), 0);
	        Boolean currBool = (Boolean) setupOnHash.get(currPath);
	        rfx.setOn(currNid, currBool.booleanValue(), 0);
	    }
	    catch (Exception exc)
	    {
	        System.err.println("Error applying configuration: " + exc +
	                           currPath + "  " + currDecompiled + "  " +
	                           savedDecompiled);
	    }
	}

    }

    void applySetup(Hashtable setupHash, Hashtable setupOnHash,
	            boolean[] deviceSelect, boolean[] timeSelect)
    {
	Enumeration pathNames = setupHash.keys();
	String currPath = "", currDecompiled = "", savedDecompiled = "";
	while (pathNames.hasMoreElements())
	{
	    try
	    {
	        currPath = (String) pathNames.nextElement();
	        Integer idxObj = (Integer) mapSetupHash.get(currPath);
	        if (idxObj == null)
	            continue;
	        int idx = idxObj.intValue();
	        if (idx == 0 || idx >= NUM_SETUP || !deviceSelect[idx - 1])
	            continue;

	        NidData currNid = rfx.resolve(new PathData(currPath), 0);
	        currDecompiled = (String) setupHash.get(currPath);

	        try
	        {
	            savedDecompiled = (rfx.getData(currNid, 0)).toString();
	        }
	        catch (Exception exc)
	        {
	            savedDecompiled = "";
	        }

	        if (!currDecompiled.equals(savedDecompiled))
	        {
	            Data currData = Data.fromExpr(currDecompiled);
	            rfx.putData(currNid, currData, 0);
	        }
	    }
	    catch (Exception exc)
	    {
	        System.err.println("Error applying configuration: " + exc +
	                           currPath + "  " + currDecompiled + "  " +
	                           savedDecompiled);
	    }
	}
	pathNames = setupOnHash.keys();
	while (pathNames.hasMoreElements())
	{
	    try
	    {
	        currPath = (String) pathNames.nextElement();
	        Integer idxObj = (Integer) mapSetupHash.get(currPath);
	        if (idxObj == null)
	            continue;
	        int idx = idxObj.intValue();
	        if (idx == 0 || idx >= NUM_SETUP || !deviceSelect[idx - 1])
	            continue;

	        NidData currNid = rfx.resolve(new PathData(currPath), 0);
	        Boolean currBool = (Boolean) setupOnHash.get(currPath);
	        rfx.setOn(currNid, currBool.booleanValue(), 0);
	    }
	    catch (Exception exc)
	    {
	        System.err.println("Error applying configuration: " + exc +
	                           currPath + "  " + currDecompiled + "  " +
	                           savedDecompiled);
	    }
	}

	//Timing components
	if (timeSelect[0]) applySetup(0, pm_mask, setupHash, setupOnHash);
	if (timeSelect[1]) applySetup(0, pc_mask, setupHash, setupOnHash);
	if (timeSelect[2]) applySetup(0, pv_mask, setupHash, setupOnHash);
	if (timeSelect[3]) applySetup(0, pp_mask, setupHash, setupOnHash);
	if (timeSelect[4]) applySetup(0, pr_mask, setupHash, setupOnHash);
//        if (timeSelect[5]) applySetup(0, ptso_mask, setupHash, setupOnHash);
	if (timeSelect[5]) applyOnSetup(0, ptso_mask, setupOnHash);
	if (timeSelect[6]) applySetup(0, ptcb_mask, setupHash,
	                              setupOnHash);
	if (timeSelect[7]) applySetup(0, ptct_mask, setupHash,
	                              setupOnHash);
	if (timeSelect[8]) applySetup(0, gas_mask, setupHash,
	                             setupOnHash);
	if (timeSelect[9]) applySetup(0, tf_mask, setupHash, setupOnHash);
	if (timeSelect[10]) applySetup(0, is_mask, setupHash,
	                               setupOnHash);
	if (timeSelect[11]) applySetup(0, chopper_mask, setupHash,
	                               setupOnHash);
	if (timeSelect[12]) applySetup(0, inverter_mask, setupHash,
	                               setupOnHash);

    }

    void applySetup(int idx, int nidOffsets[], Hashtable setupHash,
	            Hashtable setupOnHash)
    {
	try
	{
	    for (int nidIdx = 0; nidIdx < nidOffsets.length; nidIdx++)
	    {
	        NidData currNid = new NidData(nids[idx].getInt() +
	                                      nidOffsets[nidIdx]);
	        String fullPath = rfx.getInfo(currNid, 0).getFullPath();
	        String currDecompiled = (String) setupHash.get(fullPath);
	        if (currDecompiled != null)
	        {
	            Data currData = Data.fromExpr(currDecompiled);
	            rfx.putData(currNid, currData, 0);
	        }
	        Boolean isOn = (Boolean) setupOnHash.get(fullPath);
	        if (isOn != null)
	        {
	            rfx.setOn(currNid, isOn.booleanValue(), 0);
	        }
	    }
	}
	catch (Exception exc1)
	{
	    System.err.println("Error applying setup for nid array: " + exc1);
	}
    }
    //Apply only state: used for PTSO
    void applyOnSetup(int idx, int nidOffsets[], Hashtable setupOnHash)
    {
	try
	{
	    for (int nidIdx = 0; nidIdx < nidOffsets.length; nidIdx++)
	    {
	        NidData currNid = new NidData(nids[idx].getInt() +
	                                      nidOffsets[nidIdx]);
	        String fullPath = rfx.getInfo(currNid, 0).getFullPath();
	        Boolean isOn = (Boolean) setupOnHash.get(fullPath);
	        if (isOn != null)
	        {
	            rfx.setOn(currNid, isOn.booleanValue(), 0);
	        }
	    }
	}
	catch (Exception exc1)
	{
	    System.err.println("Error applying setup for nid array: " + exc1);
	}
    }

    void saveSetup(int idx, Hashtable configHash, Hashtable configOnHash)
    {
	try
	{
	    NidData baseNid = rfx.getDefault(0);
	    rfx.setDefault(nids[idx], 0);
	    NidData[] deviceNids = rfx.getWild(NodeInfo.USAGE_NUMERIC, 0);
	    rfx.setDefault(baseNid, 0);
	    if (deviceNids != null)
	    {
	        for (int i = 0; i < deviceNids.length; i++)
	        {
	            String currDec;
	            try
	            {
	                String fullPath = rfx.getInfo(deviceNids[i], 0).getFullPath();
System.out.println("SAVE SETUP: " + fullPath);
	               if(fullPath.endsWith(".TRIANGLE:REGULATION"))
	                   System.out.println("CUEA");


	                if(!fullPath.endsWith("PARAMS:PAR_312:DATA") &&  //Escludo le matrici di disaccopiamento!!!!!!!!!!!11
	                    (fullPath.indexOf("SIGNALS:") == -1)) //E i segnali in RfxControl
	                {
	                    mapSetupHash.put(fullPath, new Integer(idx));
	                    currDec = (rfx.getData(deviceNids[i], 0)).toString();
	                    configHash.put(fullPath, currDec);

	                }
	            }
	            catch (Exception exc)
	            {}
	            configOnHash.put(rfx.getInfo(deviceNids[i], 0).
	                             getFullPath(),
	                             new Boolean(rfx.isOn(deviceNids[i], 0)));
	        }
	    }
	    rfx.setDefault(nids[idx], 0);
	    deviceNids = rfx.getWild(NodeInfo.USAGE_TEXT, 0);
	    rfx.setDefault(baseNid, 0);
	    if (deviceNids != null)
	    {
	        for (int i = 0; i < deviceNids.length; i++)
	        {
	            String currDec;
	            try
	            {
	                String fullPath = rfx.getInfo(deviceNids[i], 0).
	                    getFullPath();
	              if(fullPath.endsWith(".TRIANGLE:REGULATION"))
	                    System.out.println("CUEA");
	               if (!fullPath.endsWith(":PAR236_VAL") && //Escludo le matrici di disaccopiamento!!!!!!!!!!!11
	                      (fullPath.indexOf("SIGNALS:") == -1)) //E i segnali in RfxControl
	                {
	                    mapSetupHash.put(fullPath, new Integer(idx));
	                    currDec = (rfx.getData(deviceNids[i], 0)).toString();
	                    configHash.put(fullPath, currDec);
	                }
	            }
	            catch (Exception exc)
	            {}
	            configOnHash.put(rfx.getInfo(deviceNids[i], 0).
	                             getFullPath(),
	                             new Boolean(rfx.isOn(deviceNids[i], 0)));
	        }
	    }


	    rfx.setDefault(nids[idx], 0);
	    deviceNids = rfx.getWild(NodeInfo.USAGE_STRUCTURE, 0);
	    rfx.setDefault(baseNid, 0);
	    if (deviceNids != null)
	    {
	        for (int i = 0; i < deviceNids.length; i++)
	        {
	            String fullPath = rfx.getInfo(deviceNids[i], 0).
	                             getFullPath();
	            mapSetupHash.put(fullPath, new Integer(idx));
	            configOnHash.put(fullPath,
	                             new Boolean(rfx.isOn(deviceNids[i], 0)));
	            //Dummy value
	            configHash.put(fullPath, "");
	        }
	    }

	    rfx.setDefault(nids[idx], 0);
	    deviceNids = rfx.getWild(NodeInfo.USAGE_SIGNAL, 0);
	    rfx.setDefault(baseNid, 0);
	    if (deviceNids != null)
	    {
	        for (int i = 0; i < deviceNids.length; i++)
	        {
	            String currDec;
	            try
	            {
	                String fullPath = rfx.getInfo(deviceNids[i], 0).
	                    getFullPath();
	                 if(!fullPath.endsWith(":PAR236_VAL") &&  //Escludo le matrici di disaccopiamento!!!!!!!!!!!11
	                        (fullPath.indexOf("SIGNALS:") == -1)) //E i segnali in RfxControl
	                 {
	                     mapSetupHash.put(fullPath, new Integer(idx));
	                     currDec = (rfx.getData(deviceNids[i], 0)).toString();
	                     configHash.put(fullPath, currDec);
	                 }
	            }
	            catch (Exception exc)
	            {}
	            configOnHash.put(rfx.getInfo(deviceNids[i], 0).
	                             getFullPath(),
	                             new Boolean(rfx.isOn(deviceNids[i], 0)));
	        }
	    }
	}
	catch (Exception exc1)
	{
	    System.err.println("Error getting device nids: " + exc1);
	}
    }

    void saveSetup(int idx, int nidOffsets[], Hashtable configHash,
	           Hashtable configOnHash)
    {
	try
	{
	    for (int nidIdx = 0; nidIdx < nidOffsets.length; nidIdx++)
	    {
	        NidData currNid = new NidData(nids[idx].getInt() +
	                                      nidOffsets[nidIdx]);
	        String currDec;
	        String fullPath = rfx.getInfo(currNid, 0).getFullPath();
	        try
	        {
	            currDec = (rfx.getData(currNid, 0)).toString();
	            configHash.put(fullPath, currDec);
	        }
	        catch (Exception exc)
	        {}
	        configOnHash.put(fullPath,
	                         new Boolean(rfx.isOn(currNid, 0)));
	    }
	}
	catch (Exception exc1)
	{
	    System.err.println("Error getting device nids: " + exc1);
	}
    }
     NidData checkDeviceConfig(NidData deviceRoot, Hashtable configHash,
	                      Hashtable configOnHash)
    {
	try
	{
	    NidData baseNid = rfx.getDefault(0);
	    rfx.setDefault(deviceRoot, 0);
	    NidData[] deviceNids = rfx.getWild(NodeInfo.USAGE_NUMERIC, 0);
	    rfx.setDefault(baseNid, 0);
	    if (deviceNids != null)
	    {
	        for (int i = 0; i < deviceNids.length; i++)
	        {
	            String currDec, savedDec;
	            try
	            {
	                currDec = (rfx.getData(deviceNids[i], 0)).toString();
	            }
	            catch (Exception exc)
	            {
	                currDec = null;
	            }
	            savedDec = (String) configHash.get(rfx.getInfo(deviceNids[i],
	                0).getFullPath());
	            if ( (savedDec == null && currDec != null) ||
	                (savedDec != null && currDec == null) ||
	                ( (savedDec != null && currDec != null) &&
	                 !savedDec.equals(currDec)))
	                return deviceNids[i];

	            boolean on = rfx.isOn(deviceNids[i], 0);
	            boolean savedOn = ( (Boolean) configOnHash.get(rfx.getInfo(
	                deviceNids[i], 0).getFullPath())).booleanValue();
	            if (on != savedOn)
	                return deviceNids[i];
	        }
	    }
	    rfx.setDefault(deviceRoot, 0);
	    deviceNids = rfx.getWild(NodeInfo.USAGE_TEXT, 0);
	    rfx.setDefault(baseNid, 0);
	    if (deviceNids != null)
	    {
	        for (int i = 0; i < deviceNids.length; i++)
	        {

	            String currDec, savedDec;
	            try
	            {
	                currDec = (rfx.getData(deviceNids[i], 0)).toString();
	            }
	            catch (Exception exc)
	            {
	                currDec = null;
	            }
	            savedDec = (String) configHash.get(rfx.getInfo(deviceNids[i],
	                0).getFullPath());
	            if ( (savedDec == null && currDec != null) ||
	                (savedDec != null && currDec == null) ||
	                ( (savedDec != null && currDec != null) &&
	                 !savedDec.equals(currDec)))
	                return deviceNids[i];
	            boolean on = rfx.isOn(deviceNids[i], 0);
	            boolean savedOn = ( (Boolean) configOnHash.get(rfx.getInfo(
	                deviceNids[i], 0).getFullPath())).booleanValue();
	            if (on != savedOn)
	                return deviceNids[i];
	        }
	    }
	    return null;
	}
	catch (Exception exc)
	{
	    System.err.println("Error comparing configurations: " + exc);
	    return null;
	}
    }

    String checkLimits()
    {
	try
	{
	    float[] pmWave = (rfx.evaluateData(new PathData("\\PM_SETUP:WAVE"),
	                                       0)).getFloatArray();
	    float maxCurr = 0;
	    for (int i = 0; i < pmWave.length; i++)
	    {
	        if (maxCurr < pmWave[i])
	            maxCurr = pmWave[i];
	    }
	    if (maxCurr > maxPMAT)
	    {
	        return "Corrente Magnetizzante sopra i limiti";
	    }
	    int numPMUnits = countPMUnits();
	    float rTransfer = (rfx.evaluateData(new PathData(
	        "\\P_CONFIG:LOAD_RESIST"), 0)).getFloat();
	    if (maxCurr * numPMUnits * rTransfer > maxPMVoltage)
	        return
	            "Tensione di picco avvolgimento magnetizzante sopra i limiti";
	}
	catch (Exception exc)
	{
	    System.err.println("Cannot read max PMAT: " + exc);
	}
	try
	{
	    float[] pmWave = (rfx.evaluateData(new PathData("\\TF_SETUP:WAVE"),
	                                       0)).getFloatArray();
	    float maxCurr = 0;
	    for (int i = 0; i < pmWave.length; i++)
	    {
	        if (maxCurr < pmWave[i])
	            maxCurr = pmWave[i];
	    }
	    if (maxCurr > maxTFAT)
	    {
	        return "Corrente TFAT sopra i limiti";
	    }
	}
	catch (Exception exc)
	{
	    System.err.println("Cannot read max TFAT: " + exc);
	}
	try
	{
	    float[] pmWave = (rfx.evaluateData(new PathData(
	        "\\CHOPPER_SETUP:WAVE"), 0)).getFloatArray();
	    float maxCurr = 0;
	    for (int i = 0; i < pmWave.length; i++)
	    {
	        if (maxCurr < pmWave[i])
	            maxCurr = pmWave[i];
	    }
	    if (maxCurr > maxTCCH)
	    {
	        return "Tensione Chopper sopra i limiti";
	    }
	}
	catch (Exception exc)
	{
	    System.err.println("Cannot read max Chopper voltage: " + exc);
	}
	try
	{
	    for (int waveIdx = 1; waveIdx <= 12; waveIdx++)
	    {
	        float[] pmWave = (rfx.evaluateData(new PathData(
	            "\\INVERTER_SETUP.CHANNEL_" + waveIdx + ":WAVE"), 0)).
	            getFloatArray();
	        float maxCurr = 0;
	        for (int i = 0; i < pmWave.length; i++)
	        {
	            if (maxCurr > pmWave[i])
	                maxCurr = pmWave[i];
	        }
	        if (Math.abs(maxCurr) > Math.abs(maxTCAC))
	        {
	            return "Corrente Inverter toroidale sopra i limiti";
	        }
	    }
	}
	catch (Exception exc)
	{
	    System.err.println("Cannot read max Inverter current: " + exc);
	}
	try
	{
	    String pcConfig = (rfx.evaluateData(new PathData(
	        "\\PC_SETUP:CONFIG"), 0)).
	        getString();
	    float maxVolt = 0;
//          float[] pcWave = (rfx.evaluateData(new PathData("\\PC_SETUP:WAVE"), 0)).getFloatArray();
	    float[] pcWave = (rfx.evaluateData(new PathData("\\RFX::PC_SETUP.WAVE_1:WAVE"), 0)).getFloatArray();

	    for (int i = 0; i < pcWave.length; i++)
	    {
	        if (maxVolt < pcWave[i])
	            maxVolt = pcWave[i];
	    }
	    if (pcConfig.trim().toUpperCase().equals("PARALLEL"))
	    {
	        if (Math.abs(maxVolt) > Math.abs(maxPCATParallel))
	        {
	            return
	                "Tensione PCAT in configurazione parallela sopra i limiti";
	        }
	    }
	    else
	    {
	        if (Math.abs(maxVolt) > Math.abs(maxPCATSeries))
	        {
	            return
	                "Tensione PCAT in configurazione serie sopra i limiti";
	        }
	    }
	}
	catch (Exception exc)
	{
	    System.err.println("Cannot read max PCAT voltage: " + exc);
	}

	try
	{
	    float[] currWave = (rfx.evaluateData(new PathData(
	        "\\VI_SETUP:FILL_WAVE"), 0)).getFloatArray();
	    float maxVolt = 0;
	    for (int i = 0; i < currWave.length; i++)
	    {
	        if (maxVolt < currWave[i])
	            maxVolt = currWave[i];
	    }
//            if (maxVolt > maxFillVoltage)
	    if (maxVolt > maxFillPuffVoltage)
	    {
	        return "Tensione Valvole Filling (He/impurities injection) sopra i limiti";
	    }
	}
	catch (Exception exc)
	{
	    System.err.println("Cannot read max Filling (He/impurities injection) voltage: " + exc);
	}
	try
	{
	    float[] currWave = (rfx.evaluateData(new PathData(
	        "\\VI_SETUP:PUFF_WAVE"), 0)).getFloatArray();
	    float maxVolt = 0;
	    for (int i = 0; i < currWave.length; i++)
	    {
	        if (maxVolt < currWave[i])
	            maxVolt = currWave[i];
	    }
//            if (maxVolt > maxPuffVoltage)
	    if (maxVolt > maxFillPuffVoltage)
	    {
	        return "Tensione Valvole Puffing (H2 injection) sopra i limiti";
	    }
	}
	catch (Exception exc)
	{
	    System.err.println("Cannot read max Puffing (H2 injection) voltage: " + exc);
	}

	try
	{
	    float startPR = (rfx.evaluateData(new PathData(
	        "\\RFX::T_START_PR"), 0)).getFloat();
	    float stopPR = (rfx.evaluateData(new PathData(
	        "\\RFX::T_STOP_PR"), 0)).getFloat();

	    if ( (stopPR - startPR )  > maxPrTime / 1000. )
	    {
	        return "Durata di accensione di PR oltre i limiti";
	    }
	}
	catch (Exception exc)
	{
	    System.err.println("Cannot read PR timing : " + exc);
	}

	try
	{
	    int controlType = (rfx.evaluateData(new PathData(
	        "\\RFX::IP_CONTROL:TYPE"), 0)).getInt();
	    if( controlType > 1)
	    {
		float phomMax = (rfx.evaluateData(new PathData(
			"\\RFX::IP_CONTROL.RFP:POHMMAX"), 0)).getFloat();

		if ( phomMax  > maxPOhm * 1e6 )
		{
			return "Valore di Pohm Max definita in IP_CONTROL oltre i limiti";
		}
	    }
	}
	catch (Exception exc)
	{
	    System.err.println("Cannot read Pohm Max : " + exc);
	}


	try
	{
	    float i2tTF = (rfx.evaluateData(Data.fromExpr("computeTF_i2t()"), 0)).getFloat();

	    if ( i2tTF  > maxI2T * 1e6 )
	    {
		return "Valore di I2T su avvolgimento toroidale oltre i limiti";
	    }
	}
	catch (Exception exc)
	{
	    System.err.println("Cannot compute I2T for toroidal coils : " + exc);
	}

	return null;
    }

    void loadPulse()
    {
	int currShot = 0;
	int prevPMUnits;
	float prevRTransfer;
	String prevPCConnection;
	String prevPVConnection;
	String shotStr = JOptionPane.showInputDialog(this, "Shot number: ",
	    "Enter shot", JOptionPane.INFORMATION_MESSAGE);
	if (shotStr == null || shotStr.trim().equals(""))return;
	try
	{
	    currLoadShot = currShot = Integer.parseInt(shotStr);
	    log("Load Pulse " + shotStr);
	    LoadPulse loadP = new LoadPulse();
	    currSetupHash = new Hashtable();
	    currSetupOnHash = new Hashtable();
	    //loadP.getSetup("rfx", currShot, currSetupHash, currSetupOnHash);
	    loadP.getSetupWithAbsPath("rfx", currShot, -1, currSetupHash, currSetupOnHash);
	    prevPMUnits = loadP.getPMUnits();
	    prevRTransfer = loadP.getRTransfer();
	    prevPCConnection = loadP.getPCConnection();
	    prevPVConnection = loadP.getPVConnection();
	    loadSelectedSetup();
	    //Checks to be performed after loading pulse
	    int currPMUnits = countPMUnits();
	    if (currPMUnits != prevPMUnits)
	        JOptionPane.showMessageDialog(ParameterSetting.this,
	            "The number of enabled PM units in loaded shot " + currShot +
	                                      " is " + prevPMUnits +
	            " which is different from the previous number (" +
	                                      currPMUnits +
	            ")  of enabled PM units in the working shot",
	                                      "Configuration discrepance",
	                                      JOptionPane.WARNING_MESSAGE);
	    float rTransfer = getRTransfer();
	    if (rTransfer != prevRTransfer)
	        JOptionPane.showMessageDialog(ParameterSetting.this,
	            "Transfer Resistance in loaded shot " + currShot +
	                                      " is " + prevRTransfer +
	            " which is different from the previous value (" + rTransfer +
	                                      ")  in the working shot",
	                                      "Configuration discrepance",
	                                      JOptionPane.WARNING_MESSAGE);
	    String pcConnection = getPCConnection();
	    if (!pcConnection.equals(prevPCConnection))
	        JOptionPane.showMessageDialog(ParameterSetting.this,
	                                      "PCAT connection in loaded shot " +
	                                      currShot +
	                                      " is " + prevPCConnection +
	            " which is different from the previous value (" +
	                                      pcConnection +
	                                      ")  in the working shot",
	                                      "Configuration discrepance",
	                                      JOptionPane.WARNING_MESSAGE);
	    String pvConnection = getPVConnection();
	    if (!pvConnection.equals(prevPVConnection))
	        JOptionPane.showMessageDialog(ParameterSetting.this,
	                                      "PVAT connection in loaded shot " +
	                                      currShot +
	                                      " is " + prevPVConnection +
	            " which is different from the previous value (" +
	                                      pvConnection +
	                                      ")  in the working shot",
	                                      "Configuration discrepance",
	                                      JOptionPane.WARNING_MESSAGE);

	}

	catch (Exception exc)
	{
	    JOptionPane.showMessageDialog(ParameterSetting.this,
	                                  "Error loading pulse " + currShot +
	                                  ": " + exc,
	                                  "Error loading pulse",
	                                  JOptionPane.WARNING_MESSAGE);
	}
	}

	boolean  checkVersions()
	{
/*            if(!checkVersionVme("\\MHD_AC::CONTROL:VERSION", "\\VERSIONS:VME_MHD_AC", "MHD_AC", true))
	        return false;
	    if(!checkVersionVme("\\MHD_BC::CONTROL:VERSION", "\\VERSIONS:VME_MHD_BC", "MHD_BC", true))
	        return false;
	    if(!checkVersionVme("\\MHD_BR::CONTROL:VERSION", "\\VERSIONS:VME_MHD_BR", "MHD_BR", true))
	        return false;
	    if(!checkVersionVme("\\EDA1::CONTROL:VERSION", "\\VERSIONS:VME_EDA1", "EDA1", true))
	        return false;
	    if(!checkVersionVme("\\EDA3::CONTROL:VERSION", "\\VERSIONS:VME_EDA3", "EDA3", true))
	        return false;
	    if(!checkVersionVme("\\DEQU_RAW::CONTROL:VERSION", "\\VERSIONS:VME_DEQU", "DEQU", true))
	        return false;
	    if(!checkVersionVme("\\DFLU_RAW::CONTROL:VERSION", "\\VERSIONS:VME_DFLU", "DFLU", true))
	        return false;
*/            return true;
	}


	String checkVersionsForPas()
	{
/*            if(!checkVersionVme("\\MHD_AC::CONTROL:VERSION", "\\VERSIONS:VME_MHD_AC", "MHD_AC", false))
	        return "Incompatible major version number for MHD_AC";
	    if(!checkVersionVme("\\MHD_BC::CONTROL:VERSION", "\\VERSIONS:VME_MHD_BC", "MHD_BC", false))
	        return "Incompatible major version number for MHD_BC";
	    if(!checkVersionVme("\\MHD_BR::CONTROL:VERSION", "\\VERSIONS:VME_MHD_BR", "MHD_BR", false))
	         return "Incompatible major version number for MHD_BR";
	   if(!checkVersionVme("\\EDA1::CONTROL:VERSION", "\\VERSIONS:VME_EDA1", "EDA1", false))
	         return "Incompatible major version number for EDA1";
	   if(!checkVersionVme("\\EDA3::CONTROL:VERSION", "\\VERSIONS:VME_EDA3", "EDA3", false))
	        return "Incompatible major version number for EDA3";
	    if(!checkVersionVme("\\DEQU_RAW::CONTROL:VERSION", "\\VERSIONS:VME_DEQU", "DEQU", false))
	        return "Incompatible major version number for DEQU";
	    if(!checkVersionVme("\\DFLU_RAW::CONTROL:VERSION", "\\VERSIONS:VME_DFLU", "DFLU", false))
	        return "Incompatible major version number for DFLU";
*/            return null;
	}


	boolean checkVersionVme(String currPath, String configPath, String name, boolean displayWarning)
	{

	    try {
	        Data data;
	        String currVersion, version;
	        data = rfx.evaluateData(new PathData(currPath), 0);
	        currVersion = data.getString();
	        data = rfx.evaluateData(new PathData(configPath), 0);
	        version = data.getString();
	        StringTokenizer st1 = new StringTokenizer(currVersion, ".");
	        StringTokenizer st2 = new StringTokenizer(version, ".");
	        String major1 = st1.nextToken();
	        String major2 = st2.nextToken();
	        String minor1 = st1.nextToken();
	        String minor2 = st2.nextToken();
	        if(!major1.equals(major2))
	        {
	            if(displayWarning)
	                JOptionPane.showMessageDialog(ParameterSetting.this,
	                                              "Major version of "+currVersion+" is not compatible with major version of "+version+
	                                               " for "+ name +": the loaded configuration cannot be used!!",
	                                              "Error comparing versions",
	                                              JOptionPane.ERROR_MESSAGE);
	            return false;

	        }
	        if(!minor1.equals(minor2) && displayWarning)
	        {
	            JOptionPane.showMessageDialog(ParameterSetting.this,
	                                          "Minor version of "+currVersion+" is not compatible with minor version of "+version+
	                                           " for "+ name +": the loaded configuration might be not fully compatible",
	                                          "Error comparing versionse",
	                                          JOptionPane.WARNING_MESSAGE);

	        }
	        return true;
	    }catch(Exception exc)
	    {
	      /*  JOptionPane.showMessageDialog(ParameterSetting.this,
	                "Error reading version numbers of" + name,
	                "Error comparing versions",
	                JOptionPane.WARNING_MESSAGE);
	        //return false;*/
	        return true;
	   }
	}


	    class DecouplingDialog
	    extends JDialog
	{
	JComboBox decouplingC;
	DecouplingDialog()
	{
	    super(ParameterSetting.this, "Select MHD decoupling");
	    JPanel jp = new JPanel();
	    String []matrixNames = getMatrixFiles();
	    String[] allMatrixNames = new String[matrixNames.length + 1];
	    for(int i = 0; i < matrixNames.length; i++)
	        allMatrixNames[i] = matrixNames[i].substring(0, matrixNames[i].length() - 4);
	    allMatrixNames[matrixNames.length] = "From Shot...";
	    decouplingC = new JComboBox(allMatrixNames);
	    decouplingC.setSelectedIndex(0);
	    jp.add(new JLabel("Decoupling: "));
	    jp.add(decouplingC);
	    getContentPane().add(jp, "Center");
	    jp = new JPanel();
	    JButton okB = new JButton("Ok");
	    okB.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            log("Set Decoupling");
	            setDecoupling((String)decouplingC.getSelectedItem());
	            setVisible(false);
	        }
	    });
	    jp.add(okB);
	    JButton cancelB = new JButton("Cancel");
	    cancelB.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            log("Set decoupling aborted");
	            setVisible(false);
	        }
	    });
	    jp.add(cancelB);
	    getContentPane().add(jp, "South");
	    pack();
	}
    } //End class DecouplingDialog

    void setDecoupling()
    {
	if (decouplingD == null)
	    decouplingD = new DecouplingDialog();
	decouplingD.setVisible(true);
    }

    void setDecoupling(String decouplingName)
    {
	Convert conv;
	if (decouplingName.equals("diagonal"))
	{
	    conv = new Convert(
 //               "\\mhd_ac::control.parameters:par236_val", "diagonal", shot);
	        "\\\\MHD_AC::MARTE.PARAMS:PAR_312:DATA", "diagonal", shot);
	    conv.convertMatrix();
/*            conv = new Convert(
//                "\\mhd_bc::control.parameters:par236_val", "diagonal", -1);
	          "\\mhd_bc::control.parameters:par236_val", "diagonal", shot);
	    conv.convertMatrix();
*/        }
	else if(decouplingName.equals("From Shot..."))
	{
	    String shotStr = JOptionPane.showInputDialog(ParameterSetting.this, "Shot number: ");
	    try {
	        int shotNum = Integer.parseInt(shotStr);
	        copyDecoupling(shotNum, shot);
	    }catch(Exception exc)
	    {
	        JOptionPane.showMessageDialog(ParameterSetting.this, "Error loading Decoupling Matrix", "Error",
	                JOptionPane.WARNING_MESSAGE);
	    }


	}
	else
	{
	    conv = new Convert(
//                "\\mhd_ac::control.parameters:par236_val", DECOUPLING_BASE_DIR + decouplingName + ".dat", shot);
	        "\\MHD_AC::MARTE.PARAMS:PAR_312:DATA", DECOUPLING_BASE_DIR + decouplingName + ".dat", shot);
	    try {
	        rfx.close(0);
	        conv.convertMatrix();
	        rfx = new Database("RFX", shot);
	        rfx.open();
	      }catch(Exception exc) {System.out.println("CANNOT OPEN RFX AFTER SETTING DECOUPLING");}
 /*           conv = new Convert(
//                "\\mhd_bc::control.parameters:par236_val", DECOUPLING_BASE_DIR + decouplingName + ".dat", -1);
	        "\\mhd_bc::control.parameters:par236_val", DECOUPLING_BASE_DIR + decouplingName + ".dat", shot);
	    conv.convertMatrix();
 */       }
	decouplingD.setVisible(false);
     }

    int getShot()
    {
	try
	{
	    int currShot = rfx.getCurrentShot("rfx");
	    if(doingShot)
	        return currShot;
	    else
	        return currShot + 1;
	}
	catch (Exception exc)
	{
	    System.err.println("Error getting current shot: " + exc);
	    return -1;
	}
    }

    class WarningDialog
	extends JDialog
    {
	JButton retryB;
	JLabel label;
	WarningDialog(JFrame frame, String message)
	{
	    super(frame, "Warning");
	    getContentPane().add(label = new JLabel(message), "Center");
	    JPanel jp = new JPanel();
	    retryB = new JButton("Retry");
	    jp.add(retryB);
	    getContentPane().add(jp, "South");
	    setLocation(new Point(frame.getWidth() / 2, frame.getHeight() / 2));
	    setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
	}

	void addActionListener(ActionListener al)
	{
	    retryB.addActionListener(al);
	}

	void setText(String text)
	{
	    label.setText(text);
	}
    } //End class WarningDialog


    boolean[] compareSetup(Hashtable currSetupHash, Hashtable currSetupOnHash,
	                   Hashtable modelSetupHash, Hashtable modelSetupOnHash,
	                   Hashtable modifiedSetupHash,
	                   Hashtable modifiedSetupOnHash)
    {
	boolean changed[] = new boolean[]
	    {
	    false, false, false, false, false, false, false, false, false, false, false, false, false,
	    false, false, false, false, false, false, false};
	Enumeration pathNames = currSetupHash.keys();
	while (pathNames.hasMoreElements())
	{
	    String currPath = (String) pathNames.nextElement();


	    String currDecompiled = (String) currSetupHash.get(currPath);
	    String modelDecompiled = (String) modelSetupHash.get(currPath);
	    if (currDecompiled == null) currDecompiled = "";
	    if (modelDecompiled == null) modelDecompiled = "";
	    if (!currDecompiled.equals(modelDecompiled))
	    {
	        try
	        {
	            int idx = ( (Integer) (mapSetupHash.get(currPath))).
	                intValue();
	            changed[idx] = true;
	            modifiedSetupHash.put(currPath, currDecompiled);

/*//////////////////////////////////GAB 2011 TACON DI URGENZA
			    if(currPath.startsWith("\\RFX::TOP.RFX.MHD.MHD_AC"))
			    {
				String newCurrPath = "\\RFX::TOP.RFX.MHD.MHD_BC"+currPath.substring(24);
				System.out.println("ZONTATO " + newCurrPath);
	                    	modifiedSetupHash.put(newCurrPath, currDecompiled);
			    }
/////////////////////////////////////////////////*/



	        }
	        catch (Exception exc)
	        {
	            System.err.println(
	                "Warning : mapping not found in compareSetup for " +
	                currPath);
	            //System.exit(0);
	        }
	    }
	    try
	    {
	        boolean currOn = ( (Boolean) currSetupOnHash.get(currPath)).
	            booleanValue();
	        boolean modelOn = ( (Boolean) modelSetupOnHash.get(currPath)).
	            booleanValue();
	        if (currOn != modelOn)
	        {
	            int idx = ( (Integer) (mapSetupHash.get(currPath))).
	                intValue();
	            changed[idx] = true;
	            modifiedSetupOnHash.put(currPath, new Boolean(currOn));
	        }
	    }
	    catch (Exception exc)
	    {
	        System.err.println(
	            "Warning : mapping not found in compareSetup for " +
	            currPath);
	        //System.exit(0);
	    }
	}
	return changed;
    }


    void reportDiffToModifiedNids(Hashtable modifiedSetupHash, Hashtable modifiedSetupOnHash)
    {
	Vector nidsV = getModifiedNidsV(modifiedSetupHash,
	                                modifiedSetupOnHash);
	modifiedNids = new int[nidsV.size()];
	for (int i = 0; i < modifiedNids.length; i++)
	    modifiedNids[i] = ( (Integer) nidsV.elementAt(i)).
	        intValue();
    }



    void revertModel()
    {
	try
	{
	    rfx.close(0);
	    rfx = new Database("RFX", -1);
	    rfx.open();
	}
	catch (Exception exc)
	{
	    System.err.println("Cannot open model");
	    return;
	}
	applySetup(modelSetupHash, modelSetupOnHash);
	try
	{
	    rfx.close(0);
	    rfx = new Database("RFX", shot);
	    rfx.open();
	}
	catch (Exception exc)
	{
	    System.err.println("Cannot open working shot");
	    return;
	}
    }

    void applyToModel()
    {
	Hashtable applyHash = new Hashtable();
	Hashtable applyOnHash = new Hashtable();
	boolean allSelectedDevices[] =
	    {
	    false, true, true, true, true, true, true, true, true, true, true, true, true, true, true};
	boolean allSelectedTimes[] =
	    {
	    true, true, true, true, true, true, true, true, true, true, true, true, true, true, true};
	getSetupForModel(applyHash, applyOnHash, allSelectedDevices,
	                 allSelectedTimes);

	applyToModel(applyHash, applyOnHash);
     }


    void applyToModel(Hashtable applyHash, Hashtable applyOnHash)
    {
	try
	{
	    rfx.close(0);
	    rfx = new Database("RFX", -1);
	    rfx.open();
	}
	catch (Exception exc)
	{
	    System.err.println("Cannot open model");
	    return;
	}
	saveSetup(modelSetupHash, modelSetupOnHash);
	revertModelItem.setEnabled(true);

	Hashtable modifiedSetupHash = new Hashtable();
	Hashtable modifiedSetupOnHash = new Hashtable();
	boolean[] changed = compareSetup(applyHash, applyOnHash, modelSetupHash,
	                                 modelSetupOnHash,
	                                 modifiedSetupHash, modifiedSetupOnHash);
	if(isOnline)
	{
	    for (int i = 0; i < NUM_SETUP; i++)
	        if (changed[i]) setUncheckedRt(i);
	}
	notifyChangedRt(modifiedSetupHash, modifiedSetupOnHash);
	applySetup(modifiedSetupHash, modifiedSetupOnHash);
	applyTimes();
	applyCurrFFToModel(currFFC.getSelectedIndex() == 1);
	notifyApplySetupFinishedRt();


	//Write Ref. Shot
	try {
	    String refShotStr = refShotLabel.getText().substring(refShotLabelText.length());
	    NidData refShotNid = rfx.resolve(new PathData("\\INFO:REF_SHOT"), 0);
	    int refShot = Integer.parseInt(refShotStr);
	    rfx.putData(refShotNid, new IntData(refShot), 0);
	}catch(Exception exc){System.out.println("Cannot write Ref. Shot");}


	try
	{
	    rfx.close(0);
	    rfx = new Database("RFX", shot);
	    rfx.open();
	}
	catch (Exception exc)
	{
	    System.err.println("Cannot open working shot");
	    return;
	}
	copyDecoupling(100, -1);
	//copyMhdBr(100, -1);
    }

    void compareShots()
    {
	String shotsStr = JOptionPane.showInputDialog(this, "Shot", "");
	try
	{
	    StringTokenizer st = new StringTokenizer(shotsStr, " ,");
	    int shot1 = Integer.parseInt(st.nextToken());
	    log("Compare to shot " + shot1);
	    compareShots(shot1);
	}
	catch (Exception exc)
	{
	    JOptionPane.showMessageDialog(this, "Incorrect shot numbers", "",
	                                  JOptionPane.WARNING_MESSAGE);
	}
    }

    void compareShots(int shot1)
    {
	Hashtable setup1Hash = new Hashtable(), setup1OnHash = new Hashtable(),
	    setup2Hash = new Hashtable(), setup2OnHash = new Hashtable(),
	    diffSetupHash = new Hashtable(), diffSetupOnHash = new Hashtable();

	updateDeviceNids();
	saveSetupAndConfig(setup1Hash, setup1OnHash);
	try
	{
	    rfx.close(0);
	    rfx = new Database("RFX", shot1);
	    rfx.open();
	}
	catch (Exception exc)
	{
	    setCursor(prevCursor);
	    JOptionPane.showMessageDialog(this, "Cannot open pulse " + shot1,
	                                  "Error opening tree",
	                                  JOptionPane.WARNING_MESSAGE);
	    return;
	}
	updateDeviceNids();
	saveSetupAndConfig(setup2Hash, setup2OnHash);
	try
	{
	    rfx.close(0);
	    rfx = new Database("RFX", shot);
	    rfx.open();
	}
	catch (Exception exc)
	{
	    setCursor(prevCursor);
	    System.err.println("Cannot open working shot");
	    return;
	}
	updateDeviceNids();
	boolean changed[] = compareSetup(setup1Hash, setup1OnHash,
	                                 setup2Hash, setup2OnHash,
	                                 diffSetupHash, diffSetupOnHash);
	int numChanged = 0;
	for (int i = 0; i < changed.length; i++)
	    if (changed[i]) numChanged++;

	if (numChanged > 0)
	{

	    reportDiffToModifiedNids(diffSetupHash, diffSetupOnHash);
	    changedD = new JDialog(this, "Changed Devices");
	    JPanel jp = new JPanel();
	    jp.setLayout(new GridLayout(numChanged, 1));
	    for (int i = 0; i < changed.length; i++)
	    {
	        if (changed[i])
	        {
	            JPanel jp1 = new JPanel();
	            jp1.add(new JLabel(titles[i]));
	            jp1.add(new DiffButton(shot, i));
	            jp1.add(new DiffButton(shot1, i, false));
	            jp.add(jp1);
	        }
	    }
	    changedD.getContentPane().add(jp, "Center");
	    jp = new JPanel();
	    JButton cancelB = new JButton("Close");
	    cancelB.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            changedD.dispose();
	        }
	    });
	    jp.add(cancelB);
	    changedD.getContentPane().add(jp, "South");
	    changedD.pack();
	    changedD.setVisible(true);
	}
	else //No changes
	{
	    JOptionPane.showMessageDialog(this, "No differences found", "",
	                                  JOptionPane.INFORMATION_MESSAGE);
	}
	setCursor(prevCursor);
    }

    DeviceSetup createDevice(int idx)
    {
	switch (idx)
	{
	    case 0:
	        return new RFXTimesSetup();
	    case 1:
	        return new RFXPolControlSetup();
	    case 2:
	        return new RFXAxiControlSetup();
	    case 3:
	        //return new RFXPCSetupSetup();
	        return new RFXPC4SetupSetup();
	    case 4:
	        return new RFXPMSetupSetup();
	    case 5:
	        return new RFXTorControlSetup();
	    case 6:
	        return new RFXChopperSetupSetup();
	    case 7:
	        return new RFXFeedforwardSetupSetup();
	    case 8:
	        return new RFXInverterSetupSetup();
	    case 9:
	        return new RFXTFSetupSetup();
	    case 10:
	        return new RFXAxiToroidalControlSetup();
	    case 11:
	        return new RFXMHDSetup();
	    case 12:
//                return new RFXVISetupSetup();
	        return new RFXVICONTROLSetup();
	    case 13:
	        return new PELLETSetup();
	    case 14:
	        return new RFXDiagTimesV1Setup();
	    case 15:
	        return new IPControlSetup();
	    case 16:
	        return new RFXMOPSetup();
	    case 17:
	        return new RFXANSALDOSetup();
	    case 18:
	        return new RFXABUnitsSetup();
	    case 19:
	        return new RFXPoloidalSetup();
	    case 20:
	        return new RFXToroidalSetup();
	    case 21:
	        return new RFXPRConfigSetup();
	    case 22:
	        //return new RFXVIConfigSetup(); Taliercio 10 - 01 - 2011
	        return new RFXVConfigSetup();
	}
	return null;
    }

    class DiffButton
	extends JButton
    {
	int actShot;
	int idx;
	boolean highlightDiff = false;
	DiffButton(int actShot, int idx)
	{
	    this(actShot, idx, true);
	}
	DiffButton(int actShot, int idx, boolean highlightDiff)
	{
	    super("" + actShot);
	    this.highlightDiff = highlightDiff;
	    this.actShot = actShot;
	    this.idx = idx;
	    addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            try
	            {
	                rfx.close(0);
	                rfx = new Database("RFX", DiffButton.this.actShot);
	                rfx.open();
	            }
	            catch (Exception exc)
	            {
	                JOptionPane.showMessageDialog(ParameterSetting.this,
	                    "Cannot open pulse " + DiffButton.this.actShot,
	                    "Error opening tree",
	                    JOptionPane.WARNING_MESSAGE);
	                return;
	            }
	            updateDeviceNids();
	            try
	            {
	                log("Show difference in device " + DiffButton.this.idx);
	                DeviceSetup currDevice = (DeviceSetup) createDevice(
	                    DiffButton.this.idx);
	                currDevice.configure(rfx,
	                                     nids[DiffButton.this.idx].getInt());
	                currDevice.setReadOnly(true);
	                currDevice.setTitle(currDevice.getTitle() + "  Shot: " +
	                                    DiffButton.this.actShot);

	                if(DiffButton.this.highlightDiff)
	                    currDevice.setHighlight(true, modifiedNids);
	                currDevice.setVisible(true);
	            }
	            catch (Exception exc)
	            {
	                System.err.println("Error creating device: " + exc);
	            }
	            try
	            {
	                rfx.close(0);
	                rfx = new Database("RFX", shot);
	                rfx.open();
	            }
	            catch (Exception exc)
	            {
	                JOptionPane.showMessageDialog(ParameterSetting.this,
	                    "Cannot open pulse " + shot,
	                    "Error opening tree",
	                    JOptionPane.WARNING_MESSAGE);
	                return;
	            }
	            updateDeviceNids();
	        }
	    });
	}
    } //End inner class DiffButton

    void applyTimes()
    {
	int nid = timesRoot.getInt();

	DeviceSetup device = new RFXTimesSetup();
	device.configure(rfx, nid);
	device.check();
	if (ParameterSetting.this.readOnly)
	    device.setReadOnly(true);
	PrintButton printB = new PrintButton(0);
	device.addButton(printB);
	device.resetNidHash();
    }



    int countPMUnits()
    {
	NidData unitsNid = new NidData(nids[4].getInt() + 5);
	try {
	    Data unitsData = rfx.evaluateData(unitsNid, 0);
	    String units = unitsData.toString();
	    StringTokenizer st = new StringTokenizer(units, " ,\"");
	    return st.countTokens();
	}catch(Exception exc)
	{
	    System.err.println("Error getting num enabled PM: " + exc);
	    return 0;
	}
    }
    String getPCConnection()
    {
	NidData configNid = new NidData(nids[3].getInt() + 2);
	try {
	    Data configData = rfx.evaluateData(configNid, 0);
	    return configData.getString();
	}catch(Exception exc)
	{
	    System.err.println("Error getting PC connection: " + exc);
	    return "";
	}
    }
    String getPVConnection()
    {
	NidData configNid = new NidData(pvSetupRoot.getInt() + 2);
	try {
	    Data configData = rfx.evaluateData(configNid, 0);
	    return configData.getString();
	}catch(Exception exc)
	{
	    System.err.println("Error getting PV connection: " + exc);
	    return "";
	}
    }
    float getRTransfer()
    {
	NidData unitsNid = new NidData(nids[19].getInt() + 20);
	try {
	    Data configData = rfx.evaluateData(unitsNid, 0);

	    System.out.println(configData.getFloat());

	    return configData.getFloat();
	}catch(Exception exc)
	{
	    System.err.println("Error getting R transfer: " + exc);
	    return 0;
	}
    }

    float getMagnetizingCurrent()
    {
	NidData waveNid = new NidData(nids[4].getInt() + 8);
	try {
	    Data waveData = rfx.evaluateData(waveNid, 0);
	    float[] wave = waveData.getFloatArray();
	    float maxCurr = wave[0];
	    for(int i = 0; i < wave.length; i++)
	        if(maxCurr < wave[i])
	            maxCurr = wave[i];
	    return maxCurr * countPMUnits();
	}catch(Exception exc)
	{
	    System.err.println("Error getting Magnetizing current: " + exc);
	    return 0;
	}
    }

    String[] getMatrixFiles()
    {
	try
	{
	    File currDir = new File("/usr/local/rfx/data_acquisition/real_time/decoupling");
	    return  currDir.list(new FilenameFilter() {
	        public boolean accept(File dir, String name)
	        {
	            return name.endsWith(".dat");
	        }
	    });
	}catch(Exception exc){return new String[0];}
    }


    Vector getModifiedNidsV(Hashtable modifiedSetupHash, Hashtable modifiedSetupOnHash)
    {
	Vector nidsV = new Vector();
	Enumeration changedPaths = modifiedSetupHash.keys();
	String currName = "";
	while(changedPaths.hasMoreElements())
	{
	    try {
	        currName = (String)changedPaths.nextElement();
	        NidData currNidData =  rfx.resolve(new PathData(currName), 0);
	        nidsV.addElement(new Integer(currNidData.getInt()));
	    }catch(Exception exc)
	    {
	        System.err.println("Cannot resolve " + currName + ": " + exc);
	    }
	}
	changedPaths = modifiedSetupOnHash.keys();
	while(changedPaths.hasMoreElements())
	{
	    try {
	        currName = (String)changedPaths.nextElement();
	        NidData currNidData =  rfx.resolve(new PathData(currName), 0);
	        nidsV.addElement(new Integer(currNidData.getInt()));
	    }catch(Exception exc)
	    {
	        System.err.println("Cannot resolve " + currName + ": " + exc);
	    }
	}
	return nidsV;
    }

    void notifyApplySetupFinishedRt()
    {
	if (rtDos != null)
	{
	    try
	    {
	        rtDos.writeInt(-2);
	        rtDos.flush();
	    }
	    catch (Exception exc)
	    {
	        rtDos = null;
	        handleNotRt();
	    }
	}
    }




    void notifyChangedRt(Hashtable modifiedSetupHash, Hashtable modifiedSetupOnHash)
    {
	Vector nidsV = getModifiedNidsV(modifiedSetupHash, modifiedSetupOnHash);
	//Protocol: -1, followed by the number of chenged nids and the nids
	if (rtDos != null && nidsV.size() > 0)
	{
	    try
	    {
	        rtDos.writeInt(-1);
	        rtDos.writeInt(nidsV.size());
	        for(int i = 0; i < nidsV.size(); i++)
	            rtDos.writeInt(((Integer)nidsV.elementAt(i)).intValue());
	        rtDos.flush();
	    }
	    catch (Exception exc)
	    {
	        rtDos = null;
	        handleNotRt();
	    }
	}

    }

/*
    void copyMhdBr(int fromShot, int toShot)
    {
	Data data303, data304, data305;
	NidData nid303, nid304, nid305;
	Data decouplingData;
	if(fromShot == toShot)
	    return;

	try {
	    rfx.close(0);
	    rfx = new Database("rfx", fromShot);
	    rfx.open();
	}
	catch(Exception exc)
	{
	    JOptionPane.showMessageDialog(this, "Cannot open shot " + fromShot, "Error opening shot",
	            JOptionPane.WARNING_MESSAGE);
	    return;
	}
	try {
	    nid303 = rfx.resolve(new PathData("\\MHD_AC::CONTROL.PARAMETERS:PAR303_VAL"), 0);
	    data303 = rfx.getData(nid303, 0);
	    nid304 = rfx.resolve(new PathData("\\MHD_AC::CONTROL.PARAMETERS:PAR304_VAL"), 0);
	    data304 = rfx.getData(nid304, 0);
	    nid305 = rfx.resolve(new PathData("\\MHD_AC::CONTROL.PARAMETERS:PAR305_VAL"), 0);
	    data305 = rfx.getData(nid305, 0);
	}
	catch(Exception exc)
	{
	    JOptionPane.showMessageDialog(this, "Cannot read MHD_BR for " + fromShot, "Error reading data",
	            JOptionPane.WARNING_MESSAGE);
	    return;
	}
	try {
	    rfx.close(0);
	    rfx = new Database("rfx", toShot);
	    rfx.open();
	}
	catch(Exception exc)
	{
	    JOptionPane.showMessageDialog(this, "Cannot open shot " + toShot, "Error opening shot",
	            JOptionPane.WARNING_MESSAGE);
	    return;
	}
	try {
	    nid303 = rfx.resolve(new PathData("\\MHD_BR::CONTROL.PARAMETERS:PAR303_VAL"), 0);
	    rfx.putData(nid303, data303, 0);
	    nid304 = rfx.resolve(new PathData("\\MHD_BR::CONTROL.PARAMETERS:PAR304_VAL"), 0);
	    rfx.putData(nid304, data304, 0);
	    nid305 = rfx.resolve(new PathData("\\MHD_BR::CONTROL.PARAMETERS:PAR305_VAL"), 0);
	    rfx.putData(nid305, data305, 0);
	 }
	catch(Exception exc)
	{
	    JOptionPane.showMessageDialog(this, "Cannot write MHD_BR for " + toShot, "Error reading data",
	            JOptionPane.WARNING_MESSAGE);
	    return;
	}
	if(toShot != shot)
	{
	    try {
	        rfx.close(0);
	        rfx = new Database("rfx", shot);
	        rfx.open();
	    }
	    catch(Exception exc)
	    {
	        JOptionPane.showMessageDialog(this, "Cannot open shot " + shot, "Error opening shot",
	                JOptionPane.WARNING_MESSAGE);
	        return;
	    }
	}

    }
*/
    void copyDecoupling(int fromShot, int toShot)
    {
	Data decouplingData;
	if(fromShot == toShot)
	    return;

	try {
	    rfx.close(0);
	    rfx = new Database("rfx", fromShot);
	    rfx.open();
	}
	catch(Exception exc)
	{
	    JOptionPane.showMessageDialog(this, "Cannot open shot " + fromShot, "Error opening shot",
	            JOptionPane.WARNING_MESSAGE);
	    return;
	}
	NidData decNid;
/*        try {
	     decNid = rfx.resolve(new PathData(
	            "\\MHD_AC::CONTROL.PARAMETERS:PAR236_VAL"), 0);
	}catch(Exception exc) {decNid = null; }
	if(decNid == null)
	{
 */           try {
	     decNid = rfx.resolve(new PathData(
	            "\\MHD_AC::MARTE.PARAMS:PAR_312:DATA"), 0);
	    }
	    catch(Exception exc)
	    {
	        JOptionPane.showMessageDialog(this, "Cannot find Decoupling for " + fromShot, "Error reading data",
	            JOptionPane.WARNING_MESSAGE);
	       try {
	            rfx.close(0);
	            rfx = new Database("rfx", toShot);
	            rfx.open();
	        }
	        catch(Exception exc1)
	        {
	            JOptionPane.showMessageDialog(this, "Cannot open shot " + toShot, "Error opening shot",
	                    JOptionPane.WARNING_MESSAGE);
	            return;
	        }
	        return;

	    }
//       }
       try {
	   decouplingData = rfx.getData(decNid, 0);
       } catch (Exception exc)
       {
	    JOptionPane.showMessageDialog(this, "Cannot read Decoupling for " + fromShot, "Error reading data",
	        JOptionPane.WARNING_MESSAGE);
	    return;
       }
       try {
	    rfx.close(0);
	    rfx = new Database("rfx", toShot);
	    rfx.open();
	}
	catch(Exception exc)
	{
	    JOptionPane.showMessageDialog(this, "Cannot open shot " + toShot, "Error opening shot",
	            JOptionPane.WARNING_MESSAGE);
	    return;
	}
	try {
	    NidData decNid1 = rfx.resolve(new PathData(
//                    "\\MHD_AC::CONTROL.PARAMETERS:PAR236_VAL"), 0);
	            "\\MHD_AC::MARTE.PARAMS:PAR_312:DATA"), 0);
//            NidData decNid2 = rfx.resolve(new PathData(
//                    "\\MHD_BC::CONTROL.PARAMETERS:PAR236_VAL"), 0);
	    rfx.putData(decNid1, decouplingData, 0);
//            rfx.putData(decNid2, decouplingData, 0);
	}
	catch(Exception exc)
	{
	    JOptionPane.showMessageDialog(this, "Cannot read Decoupling for " + toShot, "Error reading data",
	            JOptionPane.WARNING_MESSAGE);
	    return;
	}
	if(toShot != shot)
	{
	    try {
	        rfx.close(0);
	        rfx = new Database("rfx", shot);
	        rfx.open();
	    }
	    catch(Exception exc)
	    {
	        JOptionPane.showMessageDialog(this, "Cannot open shot " + shot, "Error opening shot",
	                JOptionPane.WARNING_MESSAGE);
	        return;
	    }
	}

    }


    void prepareDecouplingInfo() {
	String []fileNames;
	try {
	    File currDir = new File("/usr/local/rfx/data_acquisition/real_time/decoupling");
	    fileNames = currDir.list(new FilenameFilter() {
	        public boolean accept(File dir, String name) {
	            return name.endsWith(".key");
	        }
	    });
	} catch (Exception exc) {
	    return;
	}
	if(fileNames == null)
	    fileNames = new String[0];
	decouplingKeys = new int[fileNames.length];
	decouplingNames = new String[fileNames.length];
	for(int i = 0; i < fileNames.length; i++)
	{
	    try {
	        BufferedReader br = new BufferedReader(new FileReader(DECOUPLING_BASE_DIR+fileNames[i]));
	        String keyStr = br.readLine();
	        decouplingKeys[i] = Integer.parseInt(keyStr);
	        decouplingNames[i] = fileNames[i].substring(0, fileNames[i].length() - 4);
	    }catch(Exception exc)
	    {
	        System.err.println(exc);
	    }
	}
    }

    String getDecouplingName(int inShot)
    {
	String outName = null;
	try {
//            if(inShot != shot)
	    {
//                rfx.close(0);
	        rfx = new Database("rfx", inShot);
	        rfx.open();
	    }
	    NidData decNid;
	    try {decNid = rfx.resolve(new PathData(
	            "\\MHD_AC::CONTROL.PARAMETERS:PAR236_VAL"), 0);
	    }catch(Exception exc){decNid = null;}
	    if(decNid == null)
	        decNid = rfx.resolve(new PathData(
	            "\\MHD_AC::MARTE.PARAMS:PAR_312:DATA"), 0);
	    Data decouplingData = rfx.getData(decNid, 0);
	    Data evaluatedDecouplingData = rfx.evaluateData(decouplingData, 0);
	    float[] decouplingValues = ((ArrayData) evaluatedDecouplingData).getFloatArray();
	    int key = Convert.getKey(decouplingValues);
	    for (int i = 0; i < decouplingKeys.length; i++) {
	        if (decouplingKeys[i] == key) {
	            outName = decouplingNames[i];
	            break;
	        }
	    }
//            if (inShot != shot) {
	        rfx.close(0);
	        rfx = new Database("rfx", shot);
	        rfx.open();
//            }
	} catch (Exception exc) {
	    System.err.println(exc);
	    outName = null;
	}
	return outName;
    }


    void showDecouplingInfo()
    {
	String shotStr = JOptionPane.showInputDialog(ParameterSetting.this, "Shot: ");
	try {
	    int inShot = Integer.parseInt(shotStr);
	    String decouplingName = getDecouplingName(inShot);
	    if(decouplingName != null)
	        JOptionPane.showMessageDialog(ParameterSetting.this, "Decoupling for shot " + inShot + ": "
	                +decouplingName, "Decoupling", JOptionPane.INFORMATION_MESSAGE);
	    else
	        JOptionPane.showMessageDialog(ParameterSetting.this, "Unknown decoupling matrix", "Decoupling", JOptionPane.INFORMATION_MESSAGE);
	}
	catch(Exception exc)
	{
	    JOptionPane.showMessageDialog(ParameterSetting.this, "Unknown decoupling matrix", "Decoupling", JOptionPane.INFORMATION_MESSAGE);
	}
    }

    void log(String message)
    {
	if(isRt || !isOnline) return;
	if(logFile == null)
	{
	    try
	    {
	        logFile = new FileWriter("ParameterSetting.log");
	    }
	    catch (Exception exc)
	    {
	        System.out.println("Cannot open Log file: " + exc);
	        return;
	    }
	}
	try {
	    logFile.write("" + new Date() + "\t" + message + "\n");
	}catch(Exception exc){System.err.println("Error writing to log file: " + exc);}
    }


    void copyData(String pathStr1, String pathStr2)
    {
	try {
	    PathData path1 = new PathData(pathStr1);
	    PathData path2 = new PathData(pathStr2);
	    NidData nid1 = rfx.resolve(path1, 0);
	    NidData nid2 = rfx.resolve(path2, 0);
	    Data data = rfx.getData(nid1, 0);
	    rfx.putData(nid2, data, 0);
	}catch(Exception exc) {System.err.println("Error copying" + pathStr1 + " into " + pathStr2 + ": " + exc);}
    }
    private void getCurrFFState()
    {
	boolean on1, on2;
	try {
	    NidData nid1D = rfx.resolve(new PathData("\\MHD_AC::CURR_FF"), 0);
	    NidData nid2D = rfx.resolve(new PathData("\\MHD_BC::CURR_FF"), 0);
	    on1 = rfx.isOn(nid1D, 0);
	    on2 = rfx.isOn(nid2D, 0);
	}catch(Exception exc) {System.out.println("Cannot get state of currFF"); return;}
	if(on1 != on2)
	    JOptionPane.showMessageDialog(this, "Different FF setting for AC and BC");
	currFFC.setSelectedIndex(on1?1:0);
    }

    private void setCurrFFState()
    {
	int idx = currFFC.getSelectedIndex();
	try {
	    NidData nid1D = rfx.resolve(new PathData("\\MHD_AC::CURR_FF"), 0);
	    NidData nid2D = rfx.resolve(new PathData("\\MHD_BC::CURR_FF"), 0);
	    rfx.setOn(nid1D, idx != 0, 0);
	    rfx.setOn(nid2D, idx != 0, 0);
	}catch(Exception exc) {System.out.println("Cannot set state of currFF"); return;}
    }
    void applyCurrFFToModel(boolean ffOn)
    {
	try {
	    rfx.close(0);
	    rfx = new Database("rfx", -1);
	    rfx.open();
	    NidData ffNid1 = rfx.resolve(new PathData("\\MHD_AC::CURR_FF"), 0);
	    NidData ffNid2 = rfx.resolve(new PathData("\\MHD_BC::CURR_FF"), 0);
	    rfx.setOn(ffNid1, ffOn, 0);
	    rfx.setOn(ffNid2, ffOn, 0);
	    rfx.close(0);
	    rfx = new Database("rfx", shot);
	    rfx.open();
	}catch(Exception exc) {System.out.println("Cannot set state of currFF in model"); return;}
    }

    void loadCurrFF(int ffShot)
    {
	try {
	    rfx.close(0);
	    rfx = new Database("rfx", ffShot);
	    rfx.open();
	    NidData ffNid1 = rfx.resolve(new PathData("\\MHD_AC::CURR_FF"), 0);
	    NidData ffNid2 = rfx.resolve(new PathData("\\MHD_BC::CURR_FF"), 0);
	    boolean currFFOn = rfx.isOn(ffNid1,0);
	    if(currFFOn)
	    {
	         Data ffCurr[] = new Data[192];
	        int idx = 0;
	        for(int i = 1; i <= 4; i++)
	        {
	            for(int j = 1; j <= 9; j++)
	            {
	                NidData nid = rfx.resolve(new PathData("\\MHD_AC::CURR_FF:I0"+j+i), 0);
	                ffCurr[idx++] = rfx.getData(nid, 0);
	            }
	            for(int j = 10; j <= 48; j++)
	             {
	                NidData nid = rfx.resolve(new PathData("\\MHD_AC::CURR_FF:I"+j+i), 0);
	                ffCurr[idx++] = rfx.getData(nid, 0);
	            }
	        }
	        rfx.close(0);
	        rfx = new Database("MHD_FF", 100);
	        rfx.open();
	        idx = 0;
	        for(int i = 1; i <= 4; i++)
	        {
	            for(int j = 1; j <= 9; j++)
	            {
	                NidData nid = rfx.resolve(new PathData("\\CURR_FF:I0"+j+i), 0);
	                rfx.putData(nid, ffCurr[idx++], 0);
	            }
	            for(int j = 10; j <= 48; j++)
	             {
	                NidData nid = rfx.resolve(new PathData("\\CURR_FF:I"+j+i), 0);
	                rfx.putData(nid, ffCurr[idx++], 0);
	            }
	        }
	    }
	    rfx.close(0);
	    Data cleanExpr = Data.fromExpr("tcl(\"clean mhd_ff/shot=100/override\")");
	    rfx.evaluateData(cleanExpr, 0);
	    rfx = new Database("rfx", shot);
	    rfx.open();
	    ffNid1 = rfx.resolve(new PathData("\\MHD_AC::CURR_FF"), 0);
	    ffNid2 = rfx.resolve(new PathData("\\MHD_BC::CURR_FF"), 0);
	    rfx.setOn(ffNid1, currFFOn, 0);
	    rfx.setOn(ffNid2, currFFOn, 0);
	    getCurrFFState();
	}catch(Exception exc)
	{
	    JOptionPane.showMessageDialog(ParameterSetting.this, "Error loadingh MHD FF configuration: "+ exc,
	            "Error", JOptionPane.ERROR_MESSAGE);
	}
    }

	/*
    void i2tEvaluateResidualPrePulse()
    {
	try {
		if( rfx == null )
		{
			Data msgData = rfx.evaluateData( Data.fromExpr("I2t_PM('PRE_PULSE')"), 0 );
			float out = msgData.getFloat();
			residualI2tPMLabel.setText( "Residual I2t PM = "+out+" A2s" );
			if( out < 0 )
				JOptionPane.showMessageDialog(ParameterSetting.this, "ATTENZIONE : con questa impostazione di PM superato il valore di i2t (23e9) giornaliero ammesso",
	            "Error", JOptionPane.ERROR_MESSAGE);
		}
	}catch(Exception exc)
	{
	    JOptionPane.showMessageDialog(ParameterSetting.this, "Error Evalating Residual I2T Magnetizing",
	            "Error", JOptionPane.ERROR_MESSAGE);

	}
    }

    void i2tEvaluateResidualPostPulse()

    {
	try {
		if( rfx != null )
		{
			Data msgData = rfx.evaluateData( Data.fromExpr("I2t_PM('POST_PULSE')"), 0 );
			float out = msgData.getFloat();
			residualI2tPMLabel.setText( "Residual I2t PM = "+out+" A2s" );
			if( out < 0 )
				JOptionPane.showMessageDialog(ParameterSetting.this, "ATTENZIONE : Superato il valore di i2t (23e9) giornaliero ammesso sessione da SOSPENDERE",
	            "Error", JOptionPane.ERROR_MESSAGE);
		}
	}catch (Exception exc)
	{
	    JOptionPane.showMessageDialog(ParameterSetting.this, "Error Evalating Residual I2T Magnetizing",
	            "Error", JOptionPane.ERROR_MESSAGE);

	}
    }
	*/

    public static void main(String args[])
    {
	ParameterSetting parameterS;
	if(args.length > 1)
	    parameterS = new ParameterSetting(args[0], args[1]);
	else if (args.length > 0)
	    parameterS = new ParameterSetting(args[0]);
	else
	    parameterS = new ParameterSetting();

	parameterS.init();
	parameterS.pack();
	parameterS.setVisible(true);
    }

}

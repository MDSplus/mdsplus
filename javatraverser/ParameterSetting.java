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


*/
public class ParameterSetting
    extends JFrame implements Printable
{
    static final int NUM_DEVICES = 20;
    boolean isRt = false;
    boolean readOnly = false;
    JButton timesB, poloidalControlB, axiSetupB, pcSetupB, pmSetupB,
        toroidalControlB, chopperSetupB, ffSetupB, inverterSetupB, tfSetupB,
        bfControlB,
        mhdControlB, viSetupB, mopB, ansaldoConfigB, unitsConfigB,
        poloidalConfigB,
        toroidalConfigB, mhdConfigB, viConfigB;
    JButton buttons[] = new JButton[NUM_DEVICES];
    Database rfx;
    NidData timesRoot, poloidalControlRoot, axiSetupRoot, pcSetupRoot,
        pmSetupRoot,
        toroidalControlRoot, chopperSetupRoot, ffSetupRoot, inverterSetupRoot,
        tfSetupRoot, bfControlRoot,
        mhdControlRoot, viSetupRoot, mopRoot, ansaldoConfigRoot,
        unitsConfigRoot,
        poloidalConfigRoot, toroidalConfigRoot, mhdConfigRoot, viConfigRoot;
    String[] titles =
        {
        "TIMES SETUP", "POLOIDAL CONTROL", "AXISYMMETRIC SETUP", "PC SETUP",
        "PM SETUP",
        "TOROIDAL CONTROL", "CHOPPER SETUP", "FEEDFORWARD SETUP",
        "INVERTER SETUP", "TF SETUP", "B&F CONTROL",
        "MHD CONTROL", "VI SETUP", "MOP", "UNITS SETUP", "UNITS CONFIG",
        "POLOIDAL CONFIG", "TOROIDAL CONFIG", "MHD CONFIG", "VI CONFIG"};

    static final int CHECKED = 0, CHECKING = 1, UNCHECKED = 2;
    int states[] = new int[]
        {
        CHECKED, CHECKED, CHECKED, CHECKED, CHECKED, CHECKED, CHECKED, CHECKED,
        CHECKED, CHECKED,
        CHECKED, CHECKED, CHECKED, CHECKED, CHECKED, CHECKED, CHECKED, CHECKED,
        CHECKED, CHECKED};
    int currPrintDeviceIdx;
    JTabbedPane tabbedP;

    NidData nids[] = new NidData[NUM_DEVICES];
    DeviceSetup devices[] = new DeviceSetup[NUM_DEVICES];

    String rtIp;
    Socket rtSock;
    DataOutputStream rtDos;

    PrintService printService = PrintServiceLookup.lookupDefaultPrintService();

    Hashtable rfxConfigHash = new Hashtable();
    Hashtable rfxConfigOnHash = new Hashtable();
    Hashtable currConfigHash = new Hashtable();
    Hashtable currConfigOnHash = new Hashtable();
    Hashtable currSetupHash = new Hashtable();
    Hashtable currSetupOnHash = new Hashtable();

    JTextField maxPMATF, maxPVATF, maxPVAT8F, maxTFATF, maxTCCHF, maxTCACF;
    int maxPMAT, maxPVAT, maxPVAT8, maxTFAT, maxTCCH, maxTCAC;
    JTextArea messageArea;

    WarningDialog checkedWd, configWd, limitsWd;

    JFileChooser chooser = new JFileChooser();

    ParameterSetting()
    {
        this(true, null);
    }

    ParameterSetting(String rtIp)
    {
        this(false, rtIp);
    }

    ParameterSetting(boolean isRt, String rtIp)
    {
        this.isRt = isRt;
        if (isRt)
            readOnly = true;
        if (isRt)
        {
            setTitle("RFX Parameters -- RT    ");
            handleRt();
        }
        else
        {
            setTitle("RFX Parameters");
            this.rtIp = rtIp;
            handleNotRt();
        }
        handleScheduler();
        JMenuBar menuBar = new JMenuBar();
        JMenu fileMenu = new JMenu("File");
        JMenuItem printItem = new JMenuItem("Print Setup");
        printItem.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                printSetup();
            }
        });
        fileMenu.add(printItem);
        if (isRt)
        {
            JMenuItem loadItem = new JMenuItem("Load pulse");
            loadItem.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    loadPulse();
                }
            });
            fileMenu.add(loadItem);
        }
        else
        {
            JMenuItem saveItem = new JMenuItem("Save Configuration");
            saveItem.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    saveSetup();
                }
            });
            fileMenu.add(saveItem);
            JMenuItem loadItem = new JMenuItem("Load Configuration");
            loadItem.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    loadSetup();
                }
            });
            fileMenu.add(loadItem);


        }
        menuBar.add(fileMenu);

        JMenuItem exitItem = new JMenuItem("Exit");
        exitItem.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                System.exit(0);
            }
        });
        fileMenu.add(exitItem);
        setJMenuBar(menuBar);

        FrameRepository.setFrame(this);
        JPanel setupJp = new JPanel();
        setupJp.setLayout(new BorderLayout());

        JPanel jp = new JPanel();

        buttons[0] = timesB = new JButton("Times Setup");
        timesB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = timesRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[0] = device = new RFXTimesSetup();
                    device.configure(rfx, nid);
                    if (ParameterSetting.this.readOnly)
                        device.setReadOnly(true);
                    PrintButton printB = new PrintButton(0);
                    device.addButton(printB);
                    device.pack();
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(0, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[0] == UNCHECKED)
                    states[0] = CHECKING;
            }
        });
        jp.add(timesB);
        setupJp.add(jp, "North");
        jp = new JPanel();
        jp.setLayout(new GridLayout(1, 4));
        JPanel jp1 = new JPanel();
        jp1.setBorder(new TitledBorder("Poloidal"));
        jp1.setLayout(new GridLayout(6, 1));
        buttons[1] = poloidalControlB = new JButton("Poloidal Control");
        poloidalControlB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = poloidalControlRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[1] = device = new RFXPolControlSetup();
                    device.configure(rfx, nid);
                    if (ParameterSetting.this.readOnly)
                        device.setReadOnly(true);
                    PrintButton printB = new PrintButton(1);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(1, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[1] == UNCHECKED)
                    states[1] = CHECKING;
            }
        });
        jp1.add(poloidalControlB);
        buttons[2] = axiSetupB = new JButton("Axi Setup");
        axiSetupB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = axiSetupRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[2] = device = new RFXAxiControlSetup();
                    device.configure(rfx, nid);
                    if (ParameterSetting.this.readOnly)
                        device.setReadOnly(true);
                    PrintButton printB = new PrintButton(2);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(2, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[2] == UNCHECKED)
                    states[2] = CHECKING;
            }
        });
        jp1.add(axiSetupB);
        buttons[3] = pcSetupB = new JButton("PC Setup");
        pcSetupB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = pcSetupRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[3] = device = new RFXPCSetupSetup();
                    device.configure(rfx, nid);
                    if (ParameterSetting.this.readOnly)
                        device.setReadOnly(true);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    PrintButton printB = new PrintButton(3);
                    device.addButton(printB);
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(3, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[3] == UNCHECKED)
                    states[3] = CHECKING;
            }
        });
        jp1.add(pcSetupB);
        buttons[4] = pmSetupB = new JButton("PM Setup");
        jp1.add(pmSetupB);
        pmSetupB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = pmSetupRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[4] = device = new RFXPMSetupSetup();
                    device.configure(rfx, nid);
                    if (ParameterSetting.this.readOnly)
                        device.setReadOnly(true);
                    PrintButton printB = new PrintButton(4);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(4, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[4] == UNCHECKED)
                    states[4] = CHECKING;
            }
        });
        jp.add(jp1);

        jp1 = new JPanel();
        jp1.setBorder(new TitledBorder("Toroidal"));
        jp1.setLayout(new GridLayout(6, 1));
        buttons[5] = toroidalControlB = new JButton("Toroidal Control");
        toroidalControlB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = toroidalControlRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[5] = device = new RFXTorControlSetup();
                    device.configure(rfx, nid);
                    if (ParameterSetting.this.readOnly)
                        device.setReadOnly(true);
                    PrintButton printB = new PrintButton(5);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(5, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[5] == UNCHECKED)
                    states[5] = CHECKING;
            }
        });
        jp1.add(toroidalControlB);
        buttons[6] = chopperSetupB = new JButton("Chopper Setup");
        chopperSetupB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = chopperSetupRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[6] = device = new RFXChopperSetupSetup();
                    device.configure(rfx, nid);
                    if (ParameterSetting.this.readOnly)
                        device.setReadOnly(true);
                    PrintButton printB = new PrintButton(6);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(6, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[6] == UNCHECKED)
                    states[6] = CHECKING;
            }
        });
        jp1.add(chopperSetupB);
        buttons[7] = ffSetupB = new JButton("FeedForward Setup");
        ffSetupB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = ffSetupRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[7] = device = new RFXFeedforwardSetupSetup();
                    device.configure(rfx, nid);
                    if (ParameterSetting.this.readOnly)
                        device.setReadOnly(true);
                    PrintButton printB = new PrintButton(7);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(7, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[7] == UNCHECKED)
                    states[7] = CHECKING;
            }
        });
        jp1.add(ffSetupB);
        buttons[8] = inverterSetupB = new JButton("Inverter Setup");
        inverterSetupB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = inverterSetupRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[8] = device = new RFXInverterSetupSetup();
                    device.configure(rfx, nid);
                    if (ParameterSetting.this.readOnly)
                        device.setReadOnly(true);
                    PrintButton printB = new PrintButton(8);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(8, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[8] == UNCHECKED)
                    states[8] = CHECKING;
            }
        });
        jp1.add(inverterSetupB);
        buttons[9] = tfSetupB = new JButton("TF Setup");
        tfSetupB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = tfSetupRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[9] = device = new RFXTFSetupSetup();
                    device.configure(rfx, nid);
                    if (ParameterSetting.this.readOnly)
                        device.setReadOnly(true);
                    PrintButton printB = new PrintButton(9);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(9, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[9] == UNCHECKED)
                    states[9] = CHECKING;
            }
        });
        jp1.add(tfSetupB);
        buttons[10] = bfControlB = new JButton("B&F Control");
        bfControlB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = bfControlRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[10] = device = new RFXAxiToroidalControlSetup();
                    device.configure(rfx, nid);
                    if (ParameterSetting.this.readOnly)
                        device.setReadOnly(true);
                    PrintButton printB = new PrintButton(10);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(10, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[10] == UNCHECKED)
                    states[10] = CHECKING;
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
                int nid = mhdControlRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[11] = device = new RFXMHDSetup();
                    device.configure(rfx, nid);
                    if (ParameterSetting.this.readOnly)
                        device.setReadOnly(true);
                    PrintButton printB = new PrintButton(11);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(11, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[11] == UNCHECKED)
                    states[11] = CHECKING;
            }
        });
        jp1.add(mhdControlB);
        jp.add(jp1);

        jp1 = new JPanel();
        jp1.setBorder(new TitledBorder("Vessel"));
        jp1.setLayout(new GridLayout(6, 1));
        buttons[12] = viSetupB = new JButton("VI Setup");
        viSetupB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = viSetupRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[12] = device = new RFXVISetupSetup();
                    device.configure(rfx, nid);
                    if (ParameterSetting.this.readOnly)
                        device.setReadOnly(true);
                    PrintButton printB = new PrintButton(12);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(12, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[12] == UNCHECKED)
                    states[12] = CHECKING;
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

        buttons[13] = mopB = new JButton("MOP");
        mopB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = mopRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[13] = device = new RFXMOPSetup();
                    device.configure(rfx, nid);
                    PrintButton printB = new PrintButton(13);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(13, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[13] == UNCHECKED)
                    states[13] = CHECKING;
            }
        });
        jp.add(mopB);

        buttons[14] = ansaldoConfigB = new JButton("Ansaldo Config");
        ansaldoConfigB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = ansaldoConfigRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[14] = device = new RFXANSALDOSetup();
                    device.configure(rfx, nid);
                    PrintButton printB = new PrintButton(14);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(14, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[14] == UNCHECKED)
                    states[14] = CHECKING;
            }
        });
        jp.add(ansaldoConfigB);

        buttons[15] = unitsConfigB = new JButton("Units Config");
        unitsConfigB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = unitsConfigRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[15] = device = new RFXABUnitsSetup();
                    device.configure(rfx, nid);
                    PrintButton printB = new PrintButton(15);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(15, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[15] == UNCHECKED)
                    states[15] = CHECKING;
            }
        });
        jp.add(unitsConfigB);
        configJp.add(jp);

        jp = new JPanel();
        jp.setLayout(new GridLayout(4, 1));

        buttons[16] = poloidalConfigB = new JButton("Poloidal Config");
        poloidalConfigB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = poloidalConfigRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[16] = device = new RFXPoloidalSetup();
                    device.configure(rfx, nid);
                    PrintButton printB = new PrintButton(16);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(16, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[16] == UNCHECKED)
                    states[16] = CHECKING;
            }
        });
        jp.add(poloidalConfigB);

        buttons[17] = toroidalConfigB = new JButton("Toroidal Config");
        toroidalConfigB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = toroidalConfigRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[17] = device = new RFXToroidalSetup();
                    device.configure(rfx, nid);
                    PrintButton printB = new PrintButton(17);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(17, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[17] == UNCHECKED)
                    states[17] = CHECKING;
            }
        });
        jp.add(toroidalConfigB);

        buttons[18] = mhdConfigB = new JButton("MHD Config");
        mhdConfigB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = mhdConfigRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[18] = device = new RFXPRConfigSetup();
                    device.configure(rfx, nid);
                    PrintButton printB = new PrintButton(18);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(18, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[18] == UNCHECKED)
                    states[18] = CHECKING;
            }
        });
        jp.add(mhdConfigB);

        buttons[19] = viConfigB = new JButton("Vi Config");
        viConfigB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                int nid = viConfigRoot.getInt();
                DeviceSetup device = DeviceSetup.getDevice(nid);
                if (device == null)
                {
                    devices[19] = device = new RFXVIConfigSetup();
                    device.configure(rfx, nid);
                    PrintButton printB = new PrintButton(19);
                    device.addButton(printB);
                    device.pack();
                    //device.setLocation(getMousePosition());
                    device.setVisible(true);
                    device.addDeviceCloseListener(new DeviceCloseListener()
                    {
                        public void deviceClosed(boolean isChanged)
                        {
                            handleDeviceClosed(19, isChanged);
                        }
                    });
                }
                else
                    device.setVisible(true);
                if (states[19] == UNCHECKED)
                    states[19] = CHECKING;
            }
        });
        jp.add(viConfigB);
        configJp.add(jp);

        JPanel limitsJp = new JPanel();
        limitsJp.setLayout(new BorderLayout());
        JPanel limitsListJp = new JPanel();

        limitsListJp.setLayout(new GridLayout(6, 1));
        jp = new JPanel();
        jp.add(new JLabel("Corrente Max. PMAT per unita' (A): "));
        jp.add(maxPMATF = new JTextField("" + maxPMAT, 10));
        limitsListJp.add(jp);
        jp = new JPanel();
        jp.add(new JLabel("Corrente Max. PVAT bobine F1-F7 (A): "));
        jp.add(maxPVATF = new JTextField("" + maxPVAT, 10));
        limitsListJp.add(jp);
        jp = new JPanel();
        jp.add(new JLabel("Corrente Max. PVAT bobina F8 (A): "));
        jp.add(maxPVAT8F = new JTextField("" + maxPVAT8, 10));
        limitsListJp.add(jp);
        jp = new JPanel();
        jp.add(new JLabel("Corrente Max. TFAT per unita' (A): "));
        jp.add(maxTFATF = new JTextField("" + maxTFAT, 10));
        limitsListJp.add(jp);
        jp = new JPanel();
        jp.add(new JLabel("Tensione Max. Chopper toroidale (V): "));
        jp.add(maxTCCHF = new JTextField("" + maxTCCH, 10));
        limitsListJp.add(jp);
        jp = new JPanel();
        jp.add(new JLabel("Corrente Max. Inverter toroidale (A): "));
        jp.add(maxTCACF = new JTextField("" + maxTCAC, 10));
        limitsListJp.add(jp);
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
            maxPVAT = Integer.parseInt(br.readLine());
            maxPVAT8 = Integer.parseInt(br.readLine());
            maxTFAT = Integer.parseInt(br.readLine());
            maxTCCH = Integer.parseInt(br.readLine());
            maxTCAC = Integer.parseInt(br.readLine());
            br.close();
        }
        catch (Exception exc)
        {
            System.out.println(exc);
            maxPMAT = 12500;
            maxPVAT = 2000;
            maxPVAT8 = 2500;
            maxTFAT = 6000;
            maxTCCH = 2000;
            maxTCAC = 2500;
        }
        maxPMATF.setText("" + maxPMAT);
        maxPVATF.setText("" + maxPVAT);
        maxPVAT8F.setText("" + maxPVAT8);
        maxTFATF.setText("" + maxTFAT);
        maxTCCHF.setText("" + maxTCCH);
        maxTCACF.setText("" + maxTCAC);
    }

    void saveLimits()
    {
        try
        {
            maxPMAT = Integer.parseInt(maxPMATF.getText());
            maxPVAT = Integer.parseInt(maxPVATF.getText());
            maxPVAT8 = Integer.parseInt(maxPVAT8F.getText());
            maxTFAT = Integer.parseInt(maxTFATF.getText());
            maxTCCH = Integer.parseInt(maxTCCHF.getText());
            maxTCAC = Integer.parseInt(maxTCACF.getText());
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
            bw.write("" + maxPVAT);
            bw.newLine();
            bw.write("" + maxPVAT8);
            bw.newLine();
            bw.write("" + maxTFAT);
            bw.newLine();
            bw.write("" + maxTCCH);
            bw.newLine();
            bw.write("" + maxTCAC);
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

    void init()
    {
        try
        {
            rfx = new Database("RFX", -1);
            rfx.open();
        }
        catch (Exception exc)
        {
            System.err.println("Cannot open RFX");
            System.exit(0);
        }
        try
        {
            nids[0] = timesRoot = rfx.resolve(new PathData("\\RFX_TIMES"), 0);
            nids[1] = poloidalControlRoot = rfx.resolve(new PathData(
                "\\EDA1::CONTROL"), 0);
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
            nids[11] = mhdControlRoot = rfx.resolve(new PathData(
                "\\MHD_AC::CONTROL"), 0);
            nids[12] = viSetupRoot = rfx.resolve(new PathData("\\VI_SETUP"), 0);
            nids[13] = mopRoot = rfx.resolve(new PathData("\\MOP"), 0);
            nids[14] = ansaldoConfigRoot = rfx.resolve(new PathData("\\ANSALDO"),
                0);
            nids[15] = unitsConfigRoot = rfx.resolve(new PathData("\\ABUNITS"),
                0);
            nids[16] = poloidalConfigRoot = rfx.resolve(new PathData(
                "\\P_CONFIG"), 0);
            nids[17] = toroidalConfigRoot = rfx.resolve(new PathData(
                "\\T_CONFIG"), 0);
            nids[18] = mhdConfigRoot = rfx.resolve(new PathData("\\PR_CONFIG"),
                0);
            nids[19] = viConfigRoot = rfx.resolve(new PathData("\\VI_CONFIG"),
                                                  0);

            saveConfig();
       }
        catch (Exception exc)
        {
            System.err.println("Error opening device");
            System.exit(0);
        }

        if (isRt)
            setTitle("RFX Parameters -- RT --    shot: " + getShot());
        else
            setTitle("RFX Parameters  shot: " + getShot());
    }

    void saveConfig()
    {
        for(int i = 13; i < 20; i++)
            saveConfig(i, rfxConfigHash, rfxConfigOnHash);
    }

    void loadSetup()
    {

        chooser.rescanCurrentDirectory();
        int returnVal = chooser.showOpenDialog(this);
        if (returnVal == JFileChooser.APPROVE_OPTION)
        {
            readSetupFromFile(chooser.getSelectedFile().getName());
            applySetup();
            String errMsg = checkConfig(currConfigHash, currConfigOnHash);
            if(errMsg != null)
            {
                JOptionPane.showMessageDialog(this, errMsg, "Configuration error",
                                              JOptionPane.WARNING_MESSAGE);

            }
        }
    }

    static class RfxFileFilter extends javax.swing.filechooser.FileFilter
    {
        public boolean accept(File f)
        {
            if(f.isDirectory()) return true;
            return f.getName().endsWith(".rfx");
        }
        public String getDescription()
        {
            return "Saved configurations for RFX";
        }
    }


    void saveSetup()
    {
        chooser.rescanCurrentDirectory();
        int returnVal = chooser.showSaveDialog(this);
        if (returnVal == JFileChooser.APPROVE_OPTION)
        {
            for (int i = 0; i < 13; i++)
                saveConfig(i, currSetupHash, currSetupOnHash);
            for (int i = 13; i < 20; i++)
                saveConfig(i, currConfigHash, currConfigOnHash);
            writeSetupToFile(chooser.getSelectedFile().getName());
        }
    }

    void writeSetupToFile(String fileName)
    {
        try {
            ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(fileName));
            oos.writeObject(currSetupHash);
            oos.writeObject(currSetupOnHash);
            oos.writeObject(currConfigHash);
            oos.writeObject(currConfigOnHash);
            oos.close();
        }catch(Exception exc)
        {
            JOptionPane.showMessageDialog(this, "Cannot save configuration: " + exc, "Write error",
                                           JOptionPane.WARNING_MESSAGE);
        }
    }

    void readSetupFromFile(String fileName)
    {
        try {
            ObjectInputStream ois = new ObjectInputStream(new FileInputStream(fileName));
            currSetupHash = (Hashtable)ois.readObject();
            currSetupOnHash = (Hashtable)ois.readObject();
            currConfigHash = (Hashtable)ois.readObject();
            currConfigOnHash = (Hashtable)ois.readObject();
            ois.close();
        }catch(Exception exc)
        {
            JOptionPane.showMessageDialog(this, "Cannot load configuration: " + exc, "Read error",
                                           JOptionPane.WARNING_MESSAGE);
        }
    }



    void handleDeviceClosed(int idx, boolean isChanged)
    {
        if (isRt)
        {
            if (idx < 13) //Setup devices
            {
                if (states[idx] == CHECKING)
                {
                    states[idx] = CHECKED;
                    buttons[idx].setForeground(Color.black);
                }
            }
            else // Config devices
            {
                saveConfig(idx, rfxConfigHash, rfxConfigOnHash);
            }
        }
        else
        {
            if (isChanged)
            {
                setUncheckedRt(idx);
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
                            System.out.println("RECEIVED UNCHECK " + currIdx);

                            states[currIdx] = UNCHECKED;
                            SwingUtilities.invokeAndWait(new Runnable()
                            {
                                public void run()
                                {
                                    System.out.println("ADESSO METTO ROSSO");
                                    buttons[currIdx].setForeground(Color.red);
                                }
                            });
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

    static final int LEAVE_PAS = 1, ENTER_INIT = 2;
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
                            if (phaseIdx == LEAVE_PAS)
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
                                            public void actionPerformed(ActionEvent
                                                e)
                                            {
                                                if (allChecked())
                                                {
                                                    checkedWd.dispose();
                                                    proceedeConfig();
                                                    saveConfig();
                                                }
                                            }
                                        });
                                        checkedWd.pack();
                                        checkedWd.setVisible(true);
                                    }
                                    else
                                    {
                                        proceedeConfig();
                                        saveConfig();
                                    }
                                }
                                else
                                    setReadOnly(true);
                            }
                            if (phaseIdx == ENTER_INIT)
                            {
                                if (!isRt)
                                {
                                    setReadOnly(false);
                                    setTitle("RFX Parameters     shot: " +
                                             getShot());
                                }
                                else
                                {
                                    setTitle("RFX Parameters -- RT --  shot: " +
                                             getShot());
                                }
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

        void proceedeConfig()
        {
            String configMsg = checkConfig(rfxConfigHash, rfxConfigOnHash);
            if (configMsg != null)
            {
                configWd = new WarningDialog(ParameterSetting.this, configMsg);
                configWd.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent e)
                    {
                        String newMsg = checkConfig(rfxConfigHash, rfxConfigOnHash);
                        if (newMsg != null)
                        {
                            configWd.setText(newMsg);
                         }
                        else
                        {
                            configWd.dispose();
                            proceedeLimits();
                        }
                    }
                });
                configWd.pack();
                configWd.setVisible(true);
            }
            else
                proceedeLimits();
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
                proceedeConfirm();
        }

        void proceedeConfirm()
        {

            JOptionPane.showConfirmDialog(
                ParameterSetting.this,
                "Transitare dal PAS?",
                "Acknowledgement request",
                JOptionPane.YES_OPTION);
                    try {
                        dos.writeInt(1);
                        dos.flush();
                    }catch(Exception exc){System .err.println("Error sending scheduler answer: " +  exc);}
        }
    }// End SchedulerHandler


    void setReadOnly(boolean readOnly)
    {
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
            int titleWidth = titleFontMetrics.stringWidth(titles[
                currPrintDeviceIdx]);
            int titleHeight = titleFontMetrics.getHeight();

            g2.drawString(titles[currPrintDeviceIdx],
                          (int) width / 2 - titleWidth / 2, titleHeight);

            Font infoFont = new Font("Serif", Font.BOLD, 20);
            g2.setFont(infoFont);
            FontMetrics infoFontMetrics = g2.getFontMetrics();

            int infoHeight = infoFontMetrics.getHeight();
            g2.drawString("Data: " +
                          new SimpleDateFormat("dd/MM/yyy").format(new Date()) +
                          "          Impulso: " + getShot(),
                          0, 2 * titleHeight + 10);
            if(currPrintDeviceIdx < 13)
            {
                g2.drawString("RT: ",
                              0, 2 * titleHeight + 20 + infoHeight);
                int rtWidth = infoFontMetrics.stringWidth("RT: ");
                g2.drawLine(rtWidth, 2 * titleHeight + 20 + infoHeight,
                            rtWidth + 150, 2 * titleHeight + 20 + infoHeight);
            }
            g2.setFont(prevFont);

            g2.translate(0, 2 * titleHeight + 30 + infoHeight);

            g2.scale( ( (double) width) / devices[currPrintDeviceIdx].getWidth(),
                     ( (double) (height - (2 * titleHeight + 30 + infoHeight)) /
                      devices[currPrintDeviceIdx].getHeight()));
            devices[currPrintDeviceIdx].printAll(g2);
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
                ParameterSetting.this.print(idx);
            }
            catch (Exception exc)
            {
                System.err.println("Cannot print device: " + exc);
            }
        }
    }


    void applySetup()
    {
        Enumeration pathNames = currSetupHash.keys();
        String currPath = "", currDecompiled = "", savedDecompiled = "";
        while(pathNames.hasMoreElements())
        {
            try {
                currPath = (String)pathNames.nextElement();
                NidData currNid = rfx.resolve(new PathData(currPath), 0);
                currDecompiled = (String)currSetupHash.get(currPath);
                try {
                    savedDecompiled  = (rfx.getData(currNid, 0)).toString();
                }catch(Exception exc){savedDecompiled = "";}

                if(!currDecompiled.equals(savedDecompiled))
                {
                    Data currData = Data.fromExpr(currDecompiled);
                    System.out.println(currDecompiled);
                    System.out.println(savedDecompiled+"\n");
                    rfx.putData(currNid, currData, 0);
                }

                Boolean currBool = (Boolean)currSetupOnHash.get(currPath);
                rfx.setOn(currNid, currBool.booleanValue(), 0);
            }catch(Exception exc)
            {
                System.err.println("Error applying configuration: " + exc + currPath + "  " + currDecompiled + "  " + savedDecompiled);
            }
        }
    }




    void saveConfig(int idx, Hashtable configHash, Hashtable configOnHash)
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
                        currDec = (rfx.getData(deviceNids[i], 0)).toString();
                        configHash.put(rfx.getInfo(deviceNids[i], 0).
                                       getFullPath(), currDec);
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
                        currDec = (rfx.getData(deviceNids[i], 0)).toString();
                        configHash.put(rfx.getInfo(deviceNids[i], 0).
                                       getFullPath(), currDec);
                    }
                    catch (Exception exc)
                    {}
                    configOnHash.put(rfx.getInfo(deviceNids[i], 0).
                                     getFullPath(),
                                     new Boolean(rfx.isOn(deviceNids[i], 0)));
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
                        currDec = (rfx.getData(deviceNids[i], 0)).toString();
                        configHash.put(rfx.getInfo(deviceNids[i], 0).
                                       getFullPath(), currDec);
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

    NidData checkDeviceConfig(NidData deviceRoot, Hashtable configHash, Hashtable configOnHash)
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

    String checkConfig(Hashtable configHash, Hashtable configOnHash)
    {
        NidData errNid;
        String errPath = "";
        if ((errNid = checkDeviceConfig(mopRoot, configHash, configOnHash))!= null)
        {
            try {
                errPath = rfx.getInfo(errNid, 0).getFullPath();
            }catch(Exception exc){errPath = "";}
            return  "Discrepanza nella configurazione MOP: "+ errPath;
        }
        if ((errNid = checkDeviceConfig(ansaldoConfigRoot, configHash, configOnHash))!= null)
        {
            try {
               errPath = rfx.getInfo(errNid, 0).getFullPath();
           }catch(Exception exc){errPath = "";}
           return "Discrepanza nella configurazione Ansaldo: "+ errPath;
        }
        if ((errNid = checkDeviceConfig(unitsConfigRoot, configHash, configOnHash))!= null)
        {
            try
            {
                errPath = rfx.getInfo(errNid, 0).getFullPath();
            }
            catch (Exception exc)
            {
                errPath = "";
            }

            return "Discrepanza nella configurazione unita' A e B: "+ errPath;
        }
        if ((errNid = checkDeviceConfig(poloidalConfigRoot, configHash, configOnHash))!= null)
        {
            try {
                errPath = rfx.getInfo(errNid, 0).getFullPath();
            }catch(Exception exc){errPath = "";}
            return "Discrepanza nella configurazione Poloidale: "+ errPath;
        }
        if ((errNid = checkDeviceConfig(toroidalConfigRoot, configHash, configOnHash))!= null)
        {
            try {
                errPath = rfx.getInfo(errNid, 0).getFullPath();
            }catch(Exception exc){errPath = "";}
            return "Discrepanza nella configurazione Toroidale: "+ errPath;
        }
        if ((errNid = checkDeviceConfig(mhdConfigRoot, configHash, configOnHash))!= null)
        {
            try {
                errPath = rfx.getInfo(errNid, 0).getFullPath();
            }catch(Exception exc){errPath = "";}
            return "Discrepanza nella configurazione Sonde a Sella: "+errPath;
        }
        if ((errNid = checkDeviceConfig(viConfigRoot, configHash, configOnHash))!= null)
        {
            try {
                errPath = rfx.getInfo(errNid, 0).getFullPath();
            }catch(Exception exc){errPath = "";}
            return "Discrepanza nella configurazione del Vuoto: "+ errPath;
        }
        return null;
    }

    String  checkLimits()
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
                return  "Corrente TFAT sopra i limiti";
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
                    if (maxCurr < pmWave[i])
                        maxCurr = pmWave[i];
                }
                if (maxCurr > maxTCAC)
                {
                    return "Corrente Inverter toroidale sopra i limiti";
                }
            }
        }
        catch (Exception exc)
        {
            System.err.println("Cannot read max Inverter current: " + exc);
        }
        return null;
    }

    void loadPulse()
    {
        String shotStr = JOptionPane.showInputDialog(this, "Shot number: ",
            "Enter shot", JOptionPane.INFORMATION_MESSAGE);
        try
        {
            int shot = Integer.parseInt(shotStr);
            rfx.close(0);
            LoadPulse loadP = new LoadPulse();
            loadP.load("rfx", shot, -1);
            rfx.open();
	    Convert conv = new Convert("\\mhd_ac::control.parameters:par236_val", "normalised_gain_0.01.txt");
	    conv.convertMatrix();
 	    conv = new Convert("\\mhd_bc::control.parameters:par236_val", "normalised_gain_0.01.txt");
	    conv.convertMatrix();
        String configMsg = checkConfig(rfxConfigHash, rfxConfigOnHash);
	    if(configMsg != null)
	    	JOptionPane.showMessageDialog(this, configMsg, "Configuration error", JOptionPane.WARNING_MESSAGE);
        }
        catch (Exception exc)
        {
            JOptionPane.showMessageDialog(this,
                                          "Error loading pulse " + shotStr);
            System.err.println(exc);
        }
    }

    int getShot()
    {
        try
        {
            int shot = rfx.getCurrentShot("rfx");
            return shot + 1;
        }
        catch (Exception exc)
        {
            System.err.println("Error getting current shot: " + exc);
            return -1;
        }
    }

    class WarningDialog extends JDialog
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
            setLocation(new Point(frame.getWidth()/2, frame.getHeight()/2));
        }
        void addActionListener(ActionListener al)
        {
            retryB.addActionListener(al);
        }
        void setText(String text)
        {
            label.setText(text);
        }
    }//End class WarningDialog




    public static void main(String args[])
    {
        ParameterSetting parameterS;
        if (args.length > 0)
            parameterS = new ParameterSetting(args[0]);
        else
            parameterS = new ParameterSetting();

        parameterS.init();
        parameterS.pack();
        parameterS.setVisible(true);
    }

}

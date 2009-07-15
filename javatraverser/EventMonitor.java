import java.util.*;
import java.net.*;
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.table.*;
import javax.swing.event.*;


public class EventMonitor extends JPanel
{
    static final int DESCRIPTOR_SIZE = 20; 
    long []freeOfs;
    long []freeDataOfs;
    int []freeSize;
    long []unusedOfs;
    int numEvents;
    String eventNames[];
    int numRegistered[];
    int numPending[];
    long refreshPeriod;
    Socket sock;
    DataInputStream dis;
    DataOutputStream dos;
    int memSize;
    TableModel tm;
    JTable table;
    boolean useMdsip;
    MdsConnection mdsip;
    
    class MemDisplay extends JComponent
    {
        public Dimension getPreferredSize() {return new Dimension(250, 30); }
        int getPix(long dim, int memSize)
        {
            int pix = (int)(getWidth() * ((double)dim)/memSize);
            if(pix < 1) pix = 1;
            return pix;
        }
        public void paintComponent(Graphics g)
        {
            Graphics2D g2 = (Graphics2D)g;
            int width = getWidth();
            int height = getHeight();
            g2.setColor(Color.RED);
            g2.fillRect(0, 0, width, height);
            if(freeOfs == null) return;
            //Draw Free Descriptors (Yellow)
            g2.setColor(Color.YELLOW);
            for(int i = 0; i < freeOfs.length; i++)
            {
                g2.fillRect(getPix(freeOfs[i], memSize), 0, getPix(DESCRIPTOR_SIZE, memSize), height);
            }
            //Draw Unised Descriptors (Orange)
            g2.setColor(Color.ORANGE);
            for(int i = 0; i < unusedOfs.length; i++)
            {
                g2.fillRect(getPix(unusedOfs[i], memSize), 0, getPix(DESCRIPTOR_SIZE, memSize), height);
            }
                
            //Draw Free Space (Green)
            g2.setColor(Color.GREEN);
            for(int i = 0; i < freeOfs.length; i++)
            {
                g2.fillRect(getPix(freeDataOfs[i], memSize), 0, getPix(freeSize[i], memSize), height);
            }
            g2.setColor(Color.BLACK);
            g2.drawRect(0,0,width-1, height-1);
            
        }
    }
    
    
    public EventMonitor(String ip, int port, int refreshPeriod, boolean useMdsip) throws Exception
    {
        this.useMdsip = useMdsip;
        if(useMdsip)
        {
            mdsip = new MdsConnection(ip+":"+port);
            mdsip.ConnectToMds(false);
        }
        else
        {
            sock = new Socket(ip, port);
        }

        this.refreshPeriod = refreshPeriod;
        JPanel jp = new JPanel();
        jp.add(new MemDisplay());
        setLayout(new BorderLayout());
        add(jp, "North");
        tm = new DefaultTableModel()  {
            public int getColumnCount() {return 3;}
            public int getRowCount() 
            {
                return numEvents;
            }
            public String getColumnName(int col)
            {
                switch(col) {
                    case 0: return "Event Name";
                    case 1: return "Num. Registered";
                    case 2: return "Num. Pending";
                    default: return "";
                }
            }
            public Object getValueAt(int row, int col)
            {
                if (row >= numEvents) return "";
                switch(col) {
                    case 0: return eventNames[row];
                    case 1: return ""+numRegistered[row];
                    case 2: return ""+numPending[row];
                }
                return "";
            }
            public boolean isCellEditable(int row, int col)
            {
                return false;
            }
        };
        table = new JTable();
        table.setModel(tm);
        add(new JScrollPane(table), "Center");
        jp = new JPanel();
        JButton cleanB = new JButton("Clean");
        cleanB.setForeground(Color.RED);
        cleanB.addActionListener(new ActionListener() 
        {
            public void actionPerformed(ActionEvent e)
            {
                clean();
            }
        });
        jp.add(cleanB);
        add(jp, "South");
    }
    
    
    void initialize() throws IOException
    {
        if(!useMdsip)
        {
            dis = new DataInputStream(sock.getInputStream());
            dos = new DataOutputStream(sock.getOutputStream());
        }
        (new Thread(new Runnable() {
            public void run()
            {
                while(true)
                {
                    update();
                    try {
                        Thread.currentThread().sleep(refreshPeriod);
                    }catch(InterruptedException exc){}
                }
            }
        })).start();
    }
    void update()
    {
        int msgSize;
        try {
            if(useMdsip)
                updateMds();
            else
                updateConnector();
                       //Memory State
            if(!useMdsip)
                msgSize = dis.readInt(); //Used only in EventConnector protocol
            memSize = dis.readInt();
            int numFreeBufs = dis.readInt();
            int numUnusedBufs = dis.readInt();
            freeOfs = new long[numFreeBufs];
            freeDataOfs = new long[numFreeBufs];
            freeSize = new int[numFreeBufs];
            for(int i = 0; i < numFreeBufs; i++)
            {
                freeOfs[i] = dis.readLong();
                freeDataOfs[i] = dis.readLong();
                freeSize[i] = dis.readInt();

                System.out.println(freeOfs[i]);
                System.out.println(freeDataOfs[i]);
                System.out.println(freeSize[i]);
                System.out.println("");

            }
            System.out.println("");
            unusedOfs = new long[numUnusedBufs];
            for(int i = 0; i < numUnusedBufs; i++)
            {
                unusedOfs[i] = dis.readLong();
                System.out.println(unusedOfs[i]);
                System.out.println("");
            }
            
            //Event State
            if(!useMdsip)
                msgSize = dis.readInt(); //Used only in EventConnector protocol
            numEvents = dis.readInt();
            eventNames = new String[numEvents];
            numRegistered = new int[numEvents];
            numPending = new int[numEvents];
            for(int i = 0; i < numEvents; i++)
            {
                int len = dis.readInt();
                byte nameB[] = new byte[len];
                dis.read(nameB);
                eventNames[i] = new String(nameB);
                numRegistered[i] = dis.readInt();
                numPending[i] = dis.readInt();
            }
            
        }catch(Exception exc){System.out.println("Cannot read state: " + exc);}
        table.tableChanged(new TableModelEvent(tm));
        repaint();
    }
    
    void updateMds() throws Exception
    {
        Descriptor memStateD = mdsip.MdsValue("RtEventsShr->MdsEventGetMemState:DSC()", new Vector());
        Descriptor stateD = mdsip.MdsValue("RtEventsShr->MdsEventGetState:DSC()", new Vector());
        byte memState[] = memStateD.dataToByteArray();
        byte state[] = stateD.dataToByteArray();
        byte[] totState = new byte[memState.length + state.length];
        System.arraycopy(memState, 0, totState, 0, memState.length);
        System.arraycopy(state, 0, totState, memState.length, state.length);
        dis = new DataInputStream(new ByteArrayInputStream(totState));
    }
    
    void updateConnector() throws Exception
    {
        dos.writeInt(1);
        dos.writeByte(1);
        dos.flush();
    }
     
   void clean() 
   {
       try {
           if(useMdsip)
           {
               mdsip.MdsValue("RtEventsShr->MdsEventClean()", new Vector());
           }
           else
           {
                dos.writeInt(1);
                dos.writeByte(2);
                dos.flush();
           }
       }catch(Exception exc)
       {
           JOptionPane.showMessageDialog(null,"Cannot send Clean command", "Error", JOptionPane.ERROR_MESSAGE);
       }
   }
    
    
    
    public static void main(String args[])
    {
        Properties prop = new Properties();
        try {
            prop.load(new FileInputStream("EventMonitor.properties"));
        }catch(Exception exc)
        {
            System.out.println("Cannot load property file");
            System.exit(0);
        }
        boolean useMdsip;
        try {
            useMdsip = prop.getProperty("EventMonitor.use_mdsip").toUpperCase().equals("YES");
        }catch(Exception exc)
        {
            useMdsip = false;
        }
        int refreshPeriod;
        try {
            refreshPeriod = Integer.parseInt(prop.getProperty("EventMonitor.refresh_period"));
        }catch(Exception exc)
        {
            refreshPeriod = 500;
        }
        JFrame frame = new JFrame("Event Monitor");
        JTabbedPane tp = new JTabbedPane();
        int idx = 1;
        while(true)
        {
            String ip = prop.getProperty("EventMonitor.host_"+idx);
            if(ip == null) break;
            try {
                int port = Integer.parseInt(prop.getProperty("EventMonitor.port_"+idx));
                EventMonitor em = new EventMonitor(ip, port, refreshPeriod, useMdsip);
                em.initialize();
                tp.add(ip, em);
            }catch(Exception exc){}
            idx++;
        }
        frame.getContentPane().add(tp, "Center");
 /*       JPanel jp = new JPanel();
        JButton b = new JButton("Quit");
        b.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) 
            {
                System.exit(0);
            }
        });
        jp.add(b);
        frame.getContentPane().add(jp, "South");
  * */
        frame.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e)
            {
                System.exit(0);
            }
        });
        frame.pack();
        frame.setVisible(true);
  }
}

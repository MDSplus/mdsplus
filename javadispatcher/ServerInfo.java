/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author taliercio
 */
import javax.swing.JButton;
import java.awt.event.*;
import java.io.*;

 public class ServerInfo
 {
        private boolean active;
        private String  className;
        private String  address;
        private String  subTree;
        private boolean isJava;
        private int     watchdogPort;
        private String  startScript;
        private String  stopScript;
        private MdsMonitorEvent  monitorEvent;
        private int     pos;

        public class TableButton extends JButton
        {
            private ServerInfo serverInfo;
            public TableButton(String label, ServerInfo serverInfo)
            {
                super(label);
            this.serverInfo = serverInfo;
            }

            public ServerInfo getInfo()
            {
                return serverInfo;
            }
            public String toString()
            {
                return "Class "+ serverInfo.className +" Address " + serverInfo.address;
            }
        }

        public ServerInfo(String className, String address, String subTree,
                          boolean isJava, int watchdogPort,
                          String startScript, String stopScript)
        {
            this.active = false;
            this.className = className;
            this.address = address;
            this.subTree = subTree;
            this.isJava = isJava;
            this.watchdogPort = watchdogPort;
            this.startScript = startScript;
            this.stopScript = stopScript;

        }

        public String getClassName()
        {
            return className;
        }

        public String getStartScript()
        {
            return startScript;
        }

        public String getStopScript()
        {
            return stopScript;
        }

        public String getAddress()
        {
            return address;
        }

        public String getSubTree()
        {
            return subTree;
        }

        public int getWarchdogPort()
        {
            return watchdogPort;
        }

        public boolean isJava()
        {
            return isJava;
        }

        public boolean isActive()
        {
            return isJava;
        }

        public void setActive(boolean active)
        {
            this.active = active;
        }

        public MdsMonitorEvent getAction()
        {
            return monitorEvent;
        }

        public void setAction(MdsMonitorEvent monitorEvent)
        {
            this.monitorEvent = monitorEvent;
        }

        public int getPos(){return pos;}
        public void setPos(int pos){this.pos=pos;}

        public Object[] getAsArray()
        {
            TableButton b;

            Object out[] = new Object[6];
            out[0] = new Boolean(active);
            out[1] = className;
            out[2] = address;
            out[3] = monitorEvent;

/*-----------------------------------------------------------------------------
            b = new TableButton ("Start", this);
            b.addMouseListener( new MouseAdapter()
            {
                public void mouseClicked(MouseEvent e)
                {
                    ServerInfo si = ((TableButton) e.getSource()).getInfo();
                    System.out.println("ACION Start " + si +" "+ si.getStartScript() );
                    startServer(si);
                }
            });
            out[4] = b;

            b = new TableButton ("Stop", this);
            b.addMouseListener( new MouseAdapter()
            {
                public void mouseClicked(MouseEvent e)
                {
                    ServerInfo si = ((TableButton) e.getSource()).getInfo();
                    System.out.println("ACION Stop " + si +" "+ si.getStopScript() );
                    stopServer(si);
                }
            });
            out[5] = b;
------------------------------------------------------------------------------*/
            return out;
        }

        public void startServer()
        {
            startServer(this);
        }

        public void startServer(ServerInfo si)
        {
            Runtime r = Runtime.getRuntime();
            try
            {
                if( si.getStartScript() != null)
                {
                    Process p = r.exec(si.getStartScript());
                    //new ServerLogger(si.className, si.address, p);
                }
            }
            catch(IOException exc)
            {
                System.out.println("Start " + exc);
            }
        }

        public void stopServer()
        {
            stopServer(this);
        }

        public void stopServer(ServerInfo si)
        {
            Runtime r = Runtime.getRuntime();
            try
            {
                if( si.getStopScript() != null)
                    r.exec(si.getStopScript());
            }
            catch(IOException exc)
            {
                System.out.println("Stop " + exc);
            }
        }

        public String toString()
        {
            return "Class :"+ className +"\n"+
                    "Address : "+ address +"\n"+
                    "Sub Tree : "+ subTree +"\n"+
                    "Java Server : "+ isJava +"\n"+
                    "Watchdog port  : "+ watchdogPort +"\n"+
                    "Start Script : "+ startScript +"\n"+
                    "Stop  Script : "+ stopScript +"\n";

        }

    }// End ServerInfo class

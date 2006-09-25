import java.io.*;
import java.net.*;
import javax.swing.*;
import java.awt.event.*;

class TestParameterSetting
{
    int rtPort = 4001;
    int slPort = 4002;
    DataInputStream rtDis;
    DataOutputStream rtDos, slDos;
    static final int ENTER_PAS = 1, LEAVE_PAS = 2, ENTER_PRE = 3, LEAVE_SECONDARY = 4;

    TestParameterSetting(String rtIp, String slIp)
    {
        try {
            Socket rtSock = new Socket(rtIp, rtPort);
            Socket slSock = new Socket(slIp, slPort);
            rtDis = new DataInputStream(rtSock.getInputStream());
            rtDos = new DataOutputStream(rtSock.getOutputStream());
            slDos = new DataOutputStream(slSock.getOutputStream());
        }catch(Exception exc)
        {
            System.err.println("Cannot connect to sl or rt: " + exc);
            System.exit(0);
        }
        JFrame frame = new JFrame("Test ParameterSetting");
        JPanel jp = new JPanel();
        JButton leavePasB = new JButton("Leave Pas");
        leavePasB.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
                try {
                    slDos.writeInt(LEAVE_PAS);
                    rtDos.writeInt(LEAVE_PAS);
                    rtDis.readInt();
                }catch(Exception exc)
                {
                    System.err.println("Cannot communicate with sl or rt: " + exc);
                }
            }
        });
        jp.add(leavePasB);
        JButton enterInitB = new JButton("Enter Pre");
        enterInitB.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
                try {
                    slDos.writeInt(ENTER_PRE);
                    rtDos.writeInt(ENTER_PRE);
                }catch(Exception exc)
                {
                    System.err.println("Cannot communicate with sl or rt: " + exc);
                }
            }
        });
        jp.add(enterInitB);
        frame.getContentPane().add(jp, "Center");
        jp = new JPanel();
        JButton exitB = new JButton("Exit");
        exitB.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
                System.exit(0);
            }
        });
        jp.add(exitB);
        frame.getContentPane().add(jp, "South");
        frame.pack();
        frame.setVisible(true);
    }


    public static void main(String args[])
    {
        String rtIp = "localhost";
        String slIp = "localhost";
        if (args.length >= 2)
        {
            rtIp = args[0];
            slIp = args[1];
        }
        TestParameterSetting tps = new TestParameterSetting(rtIp, slIp);
    }
}

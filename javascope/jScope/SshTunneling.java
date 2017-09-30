package jScope;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.net.Socket;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JTextField;

import com.mindbright.jca.security.SecureRandom;
import com.mindbright.ssh2.SSH2Channel;
import com.mindbright.ssh2.SSH2Connection;
import com.mindbright.ssh2.SSH2ConnectionEventAdapter;
import com.mindbright.ssh2.SSH2Listener;
import com.mindbright.ssh2.SSH2SimpleClient;
import com.mindbright.ssh2.SSH2Transport;
import com.mindbright.util.RandomSeed;
import com.mindbright.util.SecureRandomAndPad;

public class SshTunneling
    extends Thread
{
    String server;
    String remotePort;
    String localPort;

    SSH2SimpleClient client;
    SSH2Listener sshListener;
    SSH2Transport transport;

    DataProvider da;

    JDialog inquiry_dialog;
    JTextField user_text;
    JPasswordField passwd_text;
    int login_status;
    String username;
    String passwd;
    String error_string = null;
    JFrame f;

    public static SecureRandomAndPad createSecureRandom()
    {
        /* NOTE, this is how it should be done if you want good
         * randomness, however good randomness takes time so we settle with
         * just some low-entropy garbage here.
         *
         * RandomSeed seed = new RandomSeed("/dev/random", "/dev/urandom");
         * byte[] s = seed.getBytesBlocking(20);
         * return new SecureRandomAndPad(new SecureRandom(s));
         *
         */
        byte[] seed = RandomSeed.getSystemStateHash();
        return new SecureRandomAndPad(new SecureRandom(seed));
    }

    boolean CheckPasswd(String server, String username, String passwd)
    {
        try
        {
            Socket serverSocket = new Socket(server, 22);
            transport = new SSH2Transport(serverSocket, createSecureRandom());
            client = new SSH2SimpleClient(transport, username, passwd);
            return true;
        }
        catch (Exception exc)
        {
            error_string = exc.getMessage();
            return false;
        }
    }

    private int credentialsDialog(JFrame f, String user)
    {

        login_status = DataProvider.LOGIN_OK;
        inquiry_dialog = new JDialog(f, "SSH login on node : " + server, true);

        inquiry_dialog.getContentPane().setLayout(new BorderLayout());
        JPanel p = new JPanel();
        p.add(new JLabel("Username: "));
        user_text = new JTextField(15);
        p.add(user_text);
        if (user != null)
            user_text.setText(user);
        inquiry_dialog.getContentPane().add(p, "North");
        p = new JPanel();
        p.add(new JLabel("Password: "));
        passwd_text = new JPasswordField(15);
        passwd_text.setEchoChar('*');
        p.add(passwd_text);
        inquiry_dialog.getContentPane().add(p, "Center");
        p = new JPanel();
        JButton ok_b = new JButton("Ok");
        ok_b.setDefaultCapable(true);
        ok_b.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                username = user_text.getText();
                passwd = new String(passwd_text.getPassword());
                if (!CheckPasswd(server, username, passwd))
                {
                    JOptionPane.showMessageDialog(inquiry_dialog,
                                                  "Login ERROR : " +
                                                  ( (error_string != null) ?
                        error_string : "no further information"),
                                                  "alert",
                                                  JOptionPane.ERROR_MESSAGE);
                    login_status = DataProvider.LOGIN_ERROR;
                }
                else
                {
                    inquiry_dialog.setVisible(false);
                    login_status = DataProvider.LOGIN_OK;
                }
            }
        });
        p.add(ok_b);
        JButton clear_b = new JButton("Clear");
        clear_b.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                user_text.setText("");
                passwd_text.setText("");
            }
        });
        p.add(clear_b);
        JButton cancel_b = new JButton("Cancel");
        cancel_b.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                login_status = DataProvider.LOGIN_CANCEL;
                inquiry_dialog.setVisible(false);
            }
        });
        p.add(cancel_b);
        inquiry_dialog.getContentPane().add(p, "South");
        inquiry_dialog.pack();
        if (f != null)
        {
            Rectangle r = f.getBounds();
            inquiry_dialog.setLocation(r.x + r.width / 2 -
                                       inquiry_dialog.getBounds().width / 2,
                                       r.y + r.height / 2 -
                                       inquiry_dialog.getBounds().height / 2);
        }
        else
        {
            Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
            inquiry_dialog.setLocation(screenSize.width / 2 -
                                       inquiry_dialog.getSize().width / 2,
                                       screenSize.height / 2 -
                                       inquiry_dialog.getSize().height / 2);
        }
        inquiry_dialog.setVisible(true);
        return login_status;
    }

    SshTunneling(JFrame f, DataProvider da, String ip, String remotePort,
                 String user, String localPort) throws IOException
    {
        this.da = da;
        this.server = ip;
        this.remotePort = remotePort;
        this.localPort = localPort;
        this.f = f;

        int status = credentialsDialog(f, user);
        if (status != DataProvider.LOGIN_OK)
            throw (new IOException("Login not successful"));
    }

    public void Dispose()
    {
        /*
         * Disconnect the transport layer gracefully
         */
        transport.normalDisconnect("User disconnects");
        sshListener.stop();
    }

    protected void finalize()
    {
    }

    public void run()
    {
        try
        {
            SSH2Connection con = client.getConnection();
            /*
                  System.out.println("127.0.0.1:" +
                          Integer.parseInt(localPort)+
                          " "+server+":"+
                          Integer.parseInt(remotePort));
             */
            sshListener = con.newLocalForward("127.0.0.1",
                                              Integer.parseInt(localPort),
                                              server,
                                              Integer.parseInt(remotePort));

            con.setEventHandler(new SSH2ConnectionEventAdapter()
            {
                public void remoteSessionConnect(SSH2Connection connection,
                                                 String remoteAddr,
                                                 int remotePort,
                                                 SSH2Channel channel)
                {
                    System.out.println("OK " + remoteAddr + " " + remotePort);
                }

            });
        }
        catch (Exception e)
        {
            JOptionPane.showMessageDialog(f,
                "Exception starting ssh port forward process! " + e,
                                          "alert", JOptionPane.ERROR_MESSAGE);
        }
    }
}

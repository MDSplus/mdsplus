/*
 * ServerShowDialog.java
 *
 * Created on July 22, 2008, 11:18 AM
 */



/**
 *
 * @author  taliercio
 */

import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.table.*;


public class ServersInfoPanel extends JPanel {


    private String    address;
    private int       info_port;
    private Hashtable<String, ServerInfo> serversInfo = null;

    /** Creates new form ServerShowDialog */
    public ServersInfoPanel() {
//        super(parent, modal);
//        this.setPreferredSize(new Dimension(700,500));
	initComponents();
	this.jTable1.setComponentPopupMenu(this.serverActionPopup);
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */

    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

	serverActionPopup = new javax.swing.JPopupMenu();
	startServer = new javax.swing.JMenuItem();
	stopServer = new javax.swing.JMenuItem();
	jPanel1 = new javax.swing.JPanel();
	update = new javax.swing.JButton();
	startAllServer = new javax.swing.JButton();
	killAllServer = new javax.swing.JButton();
	jPanel2 = new javax.swing.JPanel();
	jScrollPane1 = new javax.swing.JScrollPane();
	jTable1 = new javax.swing.JTable();

	startServer.setText("Start");
	startServer.addActionListener(new java.awt.event.ActionListener() {
	    public void actionPerformed(java.awt.event.ActionEvent evt) {
	        startServerActionPerformed(evt);
	    }
	});
	serverActionPopup.add(startServer);

	stopServer.setText("Stop");
	stopServer.addActionListener(new java.awt.event.ActionListener() {
	    public void actionPerformed(java.awt.event.ActionEvent evt) {
	        stopServerActionPerformed(evt);
	    }
	});
	serverActionPopup.add(stopServer);

	setLayout(new java.awt.BorderLayout());

	update.setText("Servers State Update");
	update.addActionListener(new java.awt.event.ActionListener() {
	    public void actionPerformed(java.awt.event.ActionEvent evt) {
	        updateActionPerformed(evt);
	    }
	});
	jPanel1.add(update);

	startAllServer.setText("Start All Servers");
	startAllServer.addActionListener(new java.awt.event.ActionListener() {
	    public void actionPerformed(java.awt.event.ActionEvent evt) {
	        startAllServerActionPerformed(evt);
	    }
	});
	//jPanel1.add(startAllServer);

	killAllServer.setText("Kill All Servers");
	killAllServer.addActionListener(new java.awt.event.ActionListener() {
	    public void actionPerformed(java.awt.event.ActionEvent evt) {
	        killAllServerActionPerformed(evt);
	    }
	});
	//jPanel1.add(killAllServer);

	add(jPanel1, java.awt.BorderLayout.PAGE_END);

	jPanel2.setLayout(new java.awt.BorderLayout());

	jTable1.setModel(new javax.swing.table.DefaultTableModel(
	    new Object [][] {

	    },
	    new String [] {
	        "State", "Class Name", "Address", "Action"
	    }
	) {
	    boolean[] canEdit = new boolean [] {
	        false, false, false, false
	    };

	    public boolean isCellEditable(int rowIndex, int columnIndex) {
	        return canEdit [columnIndex];
	    }
	});
	jTable1.addMouseListener(new JTableButtonMouseListener(jTable1) );
	jScrollPane1.setViewportView(jTable1);
	jTable1.getColumnModel().getColumn(0).setMinWidth(50);
	jTable1.getColumnModel().getColumn(0).setPreferredWidth(50);
	jTable1.getColumnModel().getColumn(0).setMaxWidth(50);
	jTable1.getColumnModel().getColumn(0).setCellRenderer(new StateRenderer());

	jTable1.getColumnModel().getColumn(1).setMinWidth(180);
	jTable1.getColumnModel().getColumn(1).setPreferredWidth(180);
	jTable1.getColumnModel().getColumn(1).setMaxWidth(180);

		jTable1.getColumnModel().getColumn(2).setMinWidth(200);
	jTable1.getColumnModel().getColumn(2).setPreferredWidth(200);
	jTable1.getColumnModel().getColumn(2).setMaxWidth(200);


	jTable1.getColumnModel().getColumn(3).setMinWidth(200);
	jTable1.getColumnModel().getColumn(3).setPreferredWidth(200);
	jTable1.getColumnModel().getColumn(3).setCellRenderer(new ActionRenderer());

	jPanel2.add(jScrollPane1, java.awt.BorderLayout.CENTER);

	add(jPanel2, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents

private void updateActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_updateActionPerformed
// TODO add your handling code here:
      try
      {
	this.updateServersState();
      }
      catch(Exception exc)
      {
	exc.printStackTrace();
	System.out.println("Error : "+exc);
      }
}//GEN-LAST:event_updateActionPerformed

private void killAllServerActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_killAllServerActionPerformed
// TODO add your handling code here:
    Iterator<ServerInfo> i = serversInfo.values().iterator();
    while( i.hasNext() )
       i.next().stopServer();
}//GEN-LAST:event_killAllServerActionPerformed

private void startAllServerActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_startAllServerActionPerformed
// TODO add your handling code here:
    Iterator<ServerInfo> i = serversInfo.values().iterator();//GEN-LAST:event_startAllServerActionPerformed
    while( i.hasNext() )
	 i.next().startServer();
}

private void startServerActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_startServerActionPerformed
// TODO add your handling code here:
    TableModel tm = jTable1.getModel();
    int sel[] = jTable1.getSelectedRows();

    for(int i = 0; i < sel.length; i++)
    {
	String s = (String)tm.getValueAt(sel[i], 2);
	serversInfo.get(s).startServer();
    }

}//GEN-LAST:event_startServerActionPerformed

private void stopServerActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_stopServerActionPerformed
// TODO add your handling code here:
    TableModel tm = jTable1.getModel();
    int sel[] = jTable1.getSelectedRows();

    for(int i = 0; i < sel.length; i++)
    {
	String s = (String)tm.getValueAt(sel[i], 2);
	serversInfo.get(s).stopServer();
    }

}//GEN-LAST:event_stopServerActionPerformed



    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JTable jTable1;
    private javax.swing.JButton killAllServer;
    private javax.swing.JPopupMenu serverActionPopup;
    private javax.swing.JButton startAllServer;
    private javax.swing.JMenuItem startServer;
    private javax.swing.JMenuItem stopServer;
    private javax.swing.JButton update;
    // End of variables declaration//GEN-END:variables

   public void updateServerState(String serverAddress, boolean state)
   {
      try
      {
	 DefaultTableModel md = ( DefaultTableModel ) this.jTable1.getModel();

	ServerInfo si = serversInfo.get(serverAddress);
	if(si != null)
	    md.setValueAt(new Boolean(state), si.getPos(), 0);
      }
      catch(Exception exc)
      {
	exc.printStackTrace();
	System.out.println("Error : "+exc);
      }
  }

  public void setServersInfo(Hashtable<String, ServerInfo> serversInfo)
  {
      this.serversInfo = serversInfo;
  }

  public boolean checkInfoServer(String address, int info_port)
  {
      return this.address != null && address.equals(this.address) && info_port == this.info_port;
  }


  public void updateServersState()  throws Exception
  {

	Socket s = new Socket(address, info_port);
	s.setSoTimeout(5000);
	DataOutputStream dos = new DataOutputStream(s.getOutputStream());
	DataInputStream dis = new DataInputStream(s.getInputStream());

	dos.writeUTF("servers");
	int numServer = dis.readInt();

	for(int i = 0; i < numServer; i++)
	{
	//System.out.println(dis.readUTF()+" "+dis.readUTF()+" "+dis.readBoolean()+" "+dis.readInt());
	//addString(dis.readUTF(), dis.readUTF(), dis.readBoolean());

	    String serverClass = dis.readUTF();
	    String addressSrv = dis.readUTF();
	    boolean active = dis.readBoolean();
//            int aDoing = dis.readInt();

//            System.out.println("Class :"+ serverClass +"Server :"+ addressSrv  );

	    if( serversInfo != null && addressSrv != null && addressSrv.length() != 0 )
	    {
	        //System.out.println(serversInfo.get(addressSrv));
	        DefaultTableModel md = ( DefaultTableModel ) this.jTable1.getModel();

	        ServerInfo si = serversInfo.get(addressSrv);
	        if( si == null)
	        {
	            System.out.println("ERROR : Server Address "+ addressSrv +" for server class "+serverClass+" not found");
	            continue;
	        }

	        si.setActive(active);

	        int numRow = md.getRowCount();
	        int j;

	        for(j = 0; j < numRow; j++)
	        {
	            Object o = md.getValueAt(j, 2);
	            if(o != null && o.equals(addressSrv))
	            {
	                md.setValueAt(new Boolean(active), j, 0);
	                break;
	            }
	        }
	    }
	}
	s.close();
  }


  public void loadServerState(String address, int info_port) throws Exception
  {
	this.address = address;
	this.info_port = info_port;

	Socket s = new Socket(address, info_port);
	s.setSoTimeout(5000);
	DataOutputStream dos = new DataOutputStream(s.getOutputStream());
	DataInputStream dis = new DataInputStream(s.getInputStream());

	dos.writeUTF("servers");
	int numServer = dis.readInt();

	for(int i = 0; i < numServer; i++)
	{
	//System.out.println(dis.readUTF()+" "+dis.readUTF()+" "+dis.readBoolean()+" "+dis.readInt());
	//addString(dis.readUTF(), dis.readUTF(), dis.readBoolean());

	    String serverClass = dis.readUTF();
	    String addressSrv = dis.readUTF();
	    boolean active = dis.readBoolean();
//            int aDoing = dis.readInt();

//            System.out.println("Class :"+ serverClass +"Server :"+ addressSrv  );

	    if( serversInfo != null && addressSrv != null && addressSrv.length() != 0 )
	    {
	        //System.out.println(serversInfo.get(addressSrv));
	        DefaultTableModel md = ( DefaultTableModel ) this.jTable1.getModel();

	        ServerInfo si = serversInfo.get(addressSrv);
	        if( si == null)
	        {
	            System.out.println("ERROR : Server Address "+ addressSrv +" for server class "+serverClass+" not found");
	            continue;
	        }

	        si.setActive(active);

	        int numRow = md.getRowCount();
	        int j;

	        for(j = 0; j < numRow; j++)
	        {
	            Object o = md.getValueAt(j, 2);
	            if(o != null && o.equals(addressSrv))
	            {
	                md.setValueAt(new Boolean(active), j, 0);
	                break;
	            }
	        }

	        if(j == numRow)
	        {
	            si.setPos(j);
	            md.addRow( si.getAsArray() );
	        }

	    }

	}
	s.close();
  }

  public void updateServersInfoAction(MdsMonitorEvent event)
  {
	DefaultTableModel md = ( DefaultTableModel ) this.jTable1.getModel();

	int numRow = md.getRowCount();
	ServerInfo si = serversInfo.get(event.server_address);
	if(si != null)
	{
	    System.out.println("Update server Action "+ event + " pos " + si.getPos());
	    md.setValueAt(event, si.getPos(), 3);
	}
  }

  public class ActionRenderer extends JLabel implements TableCellRenderer
  {
      public Component getTableCellRendererComponent(JTable table, Object value,
	                                             boolean isSelected, boolean hasFocus,
	                                             int row, int column)
      {
	  if(value instanceof MdsMonitorEvent)
	  {
	      MdsMonitorEvent e = (MdsMonitorEvent)value;
	      switch(e.mode)
	      {
	          case MdsMonitorEvent.MonitorDoing :
	            setForeground(Color.BLUE);
	          break;
	          case MdsMonitorEvent.MonitorDone :
	            if( (e.status & 1) != 0 )
	                setForeground(Color.GREEN);
	            else
	                setForeground(Color.RED);
	          break;
	      }
	      this.setText(e.node_path);
	  }
	  else
	      this.setText("");
	  return this;
      }
   }

  public class StateRenderer extends JPanel implements TableCellRenderer
  {
      public Component getTableCellRendererComponent(JTable table, Object value,
	                                             boolean isSelected, boolean hasFocus,
	                                             int row, int column)
      {
	  if(value instanceof Boolean)
	  {
	      Boolean b = (Boolean)value;
	      if(b.booleanValue())
	          setBackground(Color.GREEN);
	      else
	          setBackground(Color.RED);
	  }
	  return this;
      }
   }


  public class ButtonRenderer implements TableCellRenderer
  {

      public Component getTableCellRendererComponent(JTable table, Object value,
	                                             boolean isSelected, boolean hasFocus,
	                                             int row, int column)
      {
	  /*
	  if (isSelected)
	  {
	      setForeground(table.getSelectionForeground());
	      setBackground(table.getSelectionBackground());
	  } else{
	      setForeground(table.getForeground());
	      setBackground(UIManager.getColor("Button.background"));
	  }
	 */
      //    command = value.toString();
	  //setText( (value == null) ? "" : value.toString() );
	//  return this;
	    return (Component)value;
      }

   }


class JTableButtonMouseListener implements MouseListener {
  private JTable table;

  private void forwardEventToButton(MouseEvent e) {
    TableColumnModel columnModel = table.getColumnModel();
    int column = columnModel.getColumnIndexAtX(e.getX());
    int row    = e.getY() / table.getRowHeight();
    Object value;
    JButton button;
    MouseEvent buttonEvent;

    if(row >= table.getRowCount() || row < 0 ||
       column >= table.getColumnCount() || column < 0)
      return;

    value = table.getValueAt(row, column);

    if(!(value instanceof JButton))
      return;

    button = (JButton)value;
    buttonEvent = SwingUtilities.convertMouseEvent(table, e, button);
    button.dispatchEvent(buttonEvent);

    // This is necessary so that when a button is pressed and released
    // it gets rendered properly.  Otherwise, the button may still appear
    // pressed down when it has been released.
    table.repaint();
  }

  public JTableButtonMouseListener(JTable table) {
    this.table = table;
  }

  public void mouseClicked(MouseEvent e) {
    forwardEventToButton(e);
  }

  public void mouseEntered(MouseEvent e) {
    forwardEventToButton(e);
  }

  public void mouseExited(MouseEvent e) {
    forwardEventToButton(e);
  }

  public void mousePressed(MouseEvent e) {
    forwardEventToButton(e);
  }

  public void mouseReleased(MouseEvent e) {
    forwardEventToButton(e);
  }
}

}

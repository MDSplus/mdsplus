

import java.awt.*;
import javax.swing.*;
import javax.swing.border.*;
/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2003</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class RFXPoloidalSetup extends DeviceSetup {
  BorderLayout borderLayout1 = new BorderLayout();
  DeviceButtons deviceButtons1 = new DeviceButtons();
  JPanel jPanel1 = new JPanel();
  JPanel jPanel2 = new JPanel();
  GridLayout gridLayout1 = new GridLayout();
  JPanel jPanel3 = new JPanel();
  JPanel jPanel4 = new JPanel();
  JPanel jPanel5 = new JPanel();
  DeviceField deviceField1 = new DeviceField();
  DeviceField deviceField2 = new DeviceField();
  DeviceField deviceField3 = new DeviceField();
  DeviceField deviceField4 = new DeviceField();
  DeviceField deviceField5 = new DeviceField();
  GridLayout gridLayout2 = new GridLayout();
  JPanel jPanel6 = new JPanel();
  JPanel jPanel7 = new JPanel();
  JPanel jPanel8 = new JPanel();
  TitledBorder titledBorder1;
  TitledBorder titledBorder2;
  TitledBorder titledBorder3;
  TitledBorder titledBorder4;
  BorderLayout borderLayout2 = new BorderLayout();
  JPanel jPanel9 = new JPanel();
  DeviceField deviceField6 = new DeviceField();
  JPanel jPanel10 = new JPanel();
  DeviceField deviceField7 = new DeviceField();
  DeviceField deviceField8 = new DeviceField();
  BorderLayout borderLayout3 = new BorderLayout();
  DeviceField deviceField9 = new DeviceField();
  JPanel jPanel11 = new JPanel();
  JPanel jPanel12 = new JPanel();
  DeviceField deviceField10 = new DeviceField();
  DeviceField deviceField11 = new DeviceField();
  BorderLayout borderLayout4 = new BorderLayout();
  DeviceField deviceField12 = new DeviceField();
  JPanel jPanel13 = new JPanel();
  JPanel jPanel14 = new JPanel();
  DeviceField deviceField13 = new DeviceField();
  DeviceField deviceField14 = new DeviceField();
  public RFXPoloidalSetup() {
    try {
      jbInit();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  private void jbInit() throws Exception {
    titledBorder1 = new TitledBorder("");
    titledBorder2 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white,new Color(148, 145, 140)),"PV");
    titledBorder3 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white,new Color(142, 142, 142)),"PC");
    titledBorder4 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white,new Color(142, 142, 142)),"PM");
    this.setWidth(576);
    this.setHeight(576);
    this.setDeviceType("RFXPoloidal");
    this.setDeviceProvider("localhost");
    this.setDeviceTitle("RFX Poloidal Configuration");
    this.getContentPane().setLayout(borderLayout1);
    jPanel1.setLayout(gridLayout1);
    gridLayout1.setColumns(1);
    gridLayout1.setRows(3);
    deviceField1.setOffsetNid(1);
    deviceField1.setTextOnly(true);
    deviceField1.setLabelString("Comment:");
    deviceField1.setNumCols(30);
    deviceField1.setIdentifier("");
    deviceField3.setOffsetNid(2);
    deviceField3.setLabelString("Load Ind. (mH):");
    deviceField3.setNumCols(6);
    deviceField3.setIdentifier("");
    deviceField2.setOffsetNid(3);
    deviceField2.setLabelString("Load Res. (Ohm):");
    deviceField2.setNumCols(6);
    deviceField2.setIdentifier("");
    deviceField5.setOffsetNid(4);
    deviceField5.setLabelString("Plasma Ind. (mH):");
    deviceField5.setNumCols(8);
    deviceField5.setIdentifier("");
    deviceField4.setOffsetNid(20);
    deviceField4.setLabelString("Transfer R (Ohm):");
    deviceField4.setNumCols(6);
    deviceField4.setIdentifier("");
    jPanel2.setLayout(gridLayout2);
    gridLayout2.setColumns(1);
    gridLayout2.setRows(3);
    jPanel6.setBorder(titledBorder4);
    jPanel6.setLayout(borderLayout2);
    jPanel8.setBorder(titledBorder3);
    jPanel8.setLayout(borderLayout3);
    jPanel7.setBorder(titledBorder2);
    jPanel7.setLayout(borderLayout4);
    deviceField6.setOffsetNid(5);
    deviceField6.setTextOnly(true);
    deviceField6.setLabelString("PM Units:");
    deviceField6.setNumCols(35);
    deviceField6.setIdentifier("");
    deviceField6.setEditable(false);
    deviceField8.setOffsetNid(6);
    deviceField8.setTextOnly(true);
    deviceField8.setLabelString("PM Connection:");
    deviceField8.setIdentifier("");
    deviceField8.setEditable(false);
    deviceField7.setOffsetNid(9);
    deviceField7.setLabelString("PM Window: ");
    deviceField7.setNumCols(4);
    deviceField7.setIdentifier("");
    deviceField7.setEditable(false);
    deviceField9.setEditable(false);
    deviceField9.setIdentifier("");
    deviceField9.setNumCols(35);
    deviceField9.setLabelString("PC Units:");
    deviceField9.setTextOnly(true);
    deviceField9.setOffsetNid(10);
    deviceField10.setOffsetNid(14);
    deviceField10.setLabelString("PC Window: ");
    deviceField10.setNumCols(4);
    deviceField10.setIdentifier("");
    deviceField10.setEditable(false);
    deviceField11.setOffsetNid(11);
    deviceField11.setTextOnly(true);
    deviceField11.setLabelString("PC Connection:");
    deviceField11.setIdentifier("");
    deviceField11.setEditable(false);
    deviceField12.setOffsetNid(15);
    deviceField12.setTextOnly(true);
    deviceField12.setLabelString("PV Units:");
    deviceField12.setNumCols(35);
    deviceField12.setIdentifier("");
    deviceField12.setEditable(false);
    deviceField13.setOffsetNid(19);
    deviceField13.setLabelString("PV Window: ");
    deviceField13.setNumCols(4);
    deviceField13.setIdentifier("");
    deviceField13.setEditable(false);
    deviceField14.setOffsetNid(16);
    deviceField14.setTextOnly(true);
    deviceField14.setLabelString("PV Connection:");
    deviceField14.setIdentifier("");
    deviceField14.setEditable(false);
    this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
    this.getContentPane().add(jPanel1, BorderLayout.NORTH);
    jPanel1.add(jPanel3, null);
    jPanel3.add(deviceField1, null);
    jPanel1.add(jPanel5, null);
    jPanel5.add(deviceField3, null);
    jPanel5.add(deviceField2, null);
    jPanel1.add(jPanel4, null);
    jPanel4.add(deviceField5, null);
    jPanel4.add(deviceField4, null);
    this.getContentPane().add(jPanel2, BorderLayout.CENTER);
    jPanel2.add(jPanel6, null);
    jPanel6.add(jPanel9, BorderLayout.NORTH);
    jPanel9.add(deviceField6, null);
    jPanel6.add(jPanel10,  BorderLayout.CENTER);
    jPanel10.add(deviceField8, null);
    jPanel10.add(deviceField7, null);
    jPanel2.add(jPanel8, null);
    jPanel8.add(jPanel11,  BorderLayout.NORTH);
    jPanel11.add(deviceField9, null);
    jPanel8.add(jPanel12, BorderLayout.CENTER);
    jPanel12.add(deviceField11, null);
    jPanel12.add(deviceField10, null);
    jPanel2.add(jPanel7, null);
    jPanel7.add(jPanel13, BorderLayout.NORTH);
    jPanel13.add(deviceField12, null);
    jPanel7.add(jPanel14,  BorderLayout.CENTER);
    jPanel14.add(deviceField14, null);
    jPanel14.add(deviceField13, null);
  }

}
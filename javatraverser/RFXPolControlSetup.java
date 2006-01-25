

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

public class RFXPolControlSetup extends DeviceSetup {
  DeviceButtons deviceButtons1 = new DeviceButtons();
  JPanel jPanel9 = new JPanel();
  BorderLayout borderLayout1 = new BorderLayout();
  JPanel jPanel33 = new JPanel();
  BorderLayout borderLayout2 = new BorderLayout();
  JTabbedPane jTabbedPane1 = new JTabbedPane();
  JScrollPane jScrollPane2 = new JScrollPane();
  GridLayout gridLayout3 = new GridLayout();
  JPanel jPanel1 = new JPanel();
  DeviceTable deviceTable6 = new DeviceTable();
  DeviceTable deviceTable3 = new DeviceTable();
  DeviceTable deviceTable5 = new DeviceTable();
  JPanel jPanel40 = new JPanel();
  GridLayout gridLayout8 = new GridLayout();
  JPanel jPanel10 = new JPanel();
  DeviceField deviceField1 = new DeviceField();
  BorderLayout borderLayout3 = new BorderLayout();
  Border border1;
  TitledBorder titledBorder1;
  TitledBorder titledBorder2;
  Border border2;
  TitledBorder titledBorder3;
  Border border3;
  TitledBorder titledBorder4;
  Border border4;
  TitledBorder titledBorder5;
  Border border5;
  TitledBorder titledBorder6;
  Border border6;
  TitledBorder titledBorder7;
  public RFXPolControlSetup() {
    try {
      jbInit();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  private void jbInit() throws Exception {
    border1 = new EtchedBorder(EtchedBorder.RAISED,Color.white,new Color(148, 145, 140));
    titledBorder1 = new TitledBorder(BorderFactory.createLineBorder(Color.black,1),"Output 4");
    titledBorder2 = new TitledBorder("");
    border2 = BorderFactory.createLineBorder(Color.black,2);
    titledBorder3 = new TitledBorder(border2,"Output 3");
    border3 = BorderFactory.createLineBorder(Color.black,1);
    titledBorder4 = new TitledBorder(border3,"Output 1");
    border4 = BorderFactory.createLineBorder(Color.black,1);
    titledBorder5 = new TitledBorder(border4,"Output 2");
    border5 = BorderFactory.createLineBorder(Color.black,2);
    titledBorder6 = new TitledBorder(border5,"Output 3");
    border6 = BorderFactory.createLineBorder(Color.black,1);
    titledBorder7 = new TitledBorder(border6,"Output 4");
    this.setWidth(626);
    this.setHeight(626);
    this.setDeviceType("RFXControl");
    this.setDeviceProvider("");
    this.setDeviceTitle("RFX Control Setup");
    this.getContentPane().setLayout(borderLayout1);
    jPanel9.setLayout(borderLayout3);
    jPanel9.setDebugGraphicsOptions(0);
    jPanel33.setLayout(borderLayout2);
    gridLayout3.setRows(4);
    jPanel1.setLayout(gridLayout3);
    deviceTable6.setOffsetNid(1091);
    deviceTable6.setLabelString("Axi Feedforward");
    deviceTable6.setNumCols(5);
    deviceTable6.setNumRows(1);
    deviceTable6.setIdentifier("");
    deviceTable6.setColumnNames(new String[] {"T_ip", "Bv_Gain", "Max_PVAT_Curr", "GainDecoupl", "GainCompRes"});
        deviceTable6.setRowNames(null);
    deviceTable6.setPreferredHeight(20);
    deviceTable6.setUseExpressions(true);
    deviceTable3.setOffsetNid(1171);
    deviceTable3.setLabelString("Control Selection");
    deviceTable3.setNumCols(5);
    deviceTable3.setNumRows(1);
    deviceTable3.setIdentifier("");
    deviceTable3.setColumnNames(new String[] {"AxiFeedforward", "AxiControl", "VoltageControl", "EquiFlux", "Control5", "Control6", "Control7", "Control8"});
    deviceTable3.setRowNames(null);
    deviceTable3.setBinary(true);
    deviceTable3.setPreferredHeight(20);
    deviceTable5.setOffsetNid(1093);
    deviceTable5.setLabelString("Axi Feedback");
    deviceTable5.setNumCols(5);
    deviceTable5.setNumRows(1);
    deviceTable5.setIdentifier("");
    deviceTable5.setColumnNames(new String[] {"Kp", "Ki", "Kd", "Max_PVAT_Curr", "Par5"});
        deviceTable5.setRowNames(null);
    deviceTable5.setPreferredHeight(20);
        jPanel40.setLayout(gridLayout8);
    gridLayout8.setColumns(1);
    gridLayout8.setHgap(0);
    gridLayout8.setRows(1);
    gridLayout8.setVgap(-2);
    deviceField1.setOffsetNid(1);
    deviceField1.setTextOnly(true);
    deviceField1.setLabelString("Comment: ");
    deviceField1.setNumCols(20);
    deviceField1.setIdentifier("");
    this.getContentPane().add(jPanel9,  BorderLayout.NORTH);
    jPanel9.add(jPanel40, BorderLayout.CENTER);
    jPanel40.add(jPanel10, null);
    jPanel10.add(deviceField1, null);
    this.getContentPane().add(jPanel33,  BorderLayout.CENTER);
    jPanel33.add(jTabbedPane1,  BorderLayout.CENTER);
        jTabbedPane1.add(jScrollPane2, "Simulink Parameters");
        jScrollPane2.getViewport().add(jPanel1, null);
    jPanel1.add(deviceTable3, null);
    jPanel1.add(deviceTable6, null);
    jPanel1.add(deviceTable5, null);
    this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
  }

}



import java.awt.*;
import javax.swing.*;
/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2003</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class MCUSetup extends DeviceSetup {
  BorderLayout borderLayout1 = new BorderLayout();
  DeviceButtons deviceButtons1 = new DeviceButtons();
  JPanel jPanel1 = new JPanel();
  GridLayout gridLayout1 = new GridLayout();
  JPanel jPanel2 = new JPanel();
  JPanel jPanel3 = new JPanel();
  JPanel jPanel4 = new JPanel();
  JPanel jPanel5 = new JPanel();
  DeviceField deviceField1 = new DeviceField();
  FlowLayout flowLayout1 = new FlowLayout();
  DeviceField deviceField2 = new DeviceField();
  DeviceDispatch deviceDispatch1 = new DeviceDispatch();
  FlowLayout flowLayout2 = new FlowLayout();
  DeviceTable deviceTable1 = new DeviceTable();
  BorderLayout borderLayout2 = new BorderLayout();
  DeviceTable deviceTable2 = new DeviceTable();
  BorderLayout borderLayout3 = new BorderLayout();
  public MCUSetup() {
    try {
      jbInit();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  private void jbInit() throws Exception {
    this.setWidth(576);
    this.setHeight(576);
    this.setDeviceType("MCU");
    this.setDeviceProvider("150.178.3.33");
    this.setDeviceTitle("MCU Tomography");
    this.getContentPane().setLayout(borderLayout1);
    deviceButtons1.setMethods(new String[] {"store"});
    jPanel1.setLayout(gridLayout1);
    gridLayout1.setRows(4);
    jPanel2.setLayout(flowLayout1);
    flowLayout1.setAlignment(FlowLayout.LEFT);
    deviceField1.setOffsetNid(1);
    deviceField1.setLabelString("Comment: ");
    deviceField1.setNumCols(35);
    deviceField1.setIdentifier("");
    deviceField2.setOffsetNid(2);
    deviceField2.setLabelString("Ip Address: ");
    deviceField2.setNumCols(20);
    deviceField2.setIdentifier("");
    jPanel3.setLayout(flowLayout2);
    flowLayout2.setAlignment(FlowLayout.LEFT);
    deviceTable1.setOffsetNid(4);
    deviceTable1.setNumCols(7);
    deviceTable1.setNumRows(1);
    deviceTable1.setIdentifier("");
    deviceTable1.setColumnNames(new String[] {"VDC", "VDE", "VDI", "HOR", "VUE", "VUI", "VUC"});
    deviceTable1.setRowNames(new String[] {"Man_Position"});
    deviceTable1.setEditable(false);
    jPanel4.setLayout(borderLayout2);
    jPanel5.setLayout(borderLayout3);
    deviceTable2.setOffsetNid(3);
    deviceTable2.setNumCols(7);
    deviceTable2.setNumRows(1);
    deviceTable2.setIdentifier("");
    deviceTable2.setColumnNames(new String[] {"VDC", "VDE", "VDI", "HOR", "VUE", "VUI", "VUC"});
    deviceTable2.setRowNames(new String[] {"Filter_Position"});
    this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
    this.getContentPane().add(jPanel1,  BorderLayout.CENTER);
    jPanel1.add(jPanel2, null);
    jPanel2.add(deviceField1, null);
    jPanel1.add(jPanel3, null);
    jPanel3.add(deviceField2, null);
    jPanel3.add(deviceDispatch1, null);
    jPanel1.add(jPanel4, null);
    jPanel4.add(deviceTable1, BorderLayout.CENTER);
    jPanel1.add(jPanel5, null);
    jPanel5.add(deviceTable2, BorderLayout.CENTER);
  }

}


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

public class RFXPCSetupSetup extends DeviceSetup {
  BorderLayout borderLayout1 = new BorderLayout();
  DeviceButtons deviceButtons1 = new DeviceButtons();
  DeviceWave deviceWave1 = new DeviceWave();
  JPanel jPanel1 = new JPanel();
  GridLayout gridLayout1 = new GridLayout();
  JPanel jPanel2 = new JPanel();
  JPanel jPanel3 = new JPanel();
  JPanel jPanel4 = new JPanel();
  DeviceField deviceField1 = new DeviceField();
  DeviceField deviceField2 = new DeviceField();
  DeviceField deviceField3 = new DeviceField();
  DeviceField deviceField4 = new DeviceField();
  DeviceField deviceField5 = new DeviceField();
  public RFXPCSetupSetup() {
    try {
      jbInit();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  private void jbInit() throws Exception {
    this.setWidth(515);
    this.setHeight(515);
    this.setDeviceType("RFXPCSetup");
    this.setDeviceProvider("localhost");
    this.setDeviceTitle("RFX PC Setup");
    this.getContentPane().setLayout(borderLayout1);
    deviceWave1.setOffsetNid(6);
    deviceWave1.setIdentifier("");
    deviceWave1.setUpdateExpression("");
    jPanel1.setLayout(gridLayout1);
    gridLayout1.setColumns(1);
    gridLayout1.setRows(3);
    deviceField1.setOffsetNid(1);
    deviceField1.setTextOnly(true);
    deviceField1.setLabelString("Comment:");
    deviceField1.setNumCols(25);
    deviceField1.setIdentifier("");
    deviceField2.setOffsetNid(4);
    deviceField2.setLabelString("Window");
    deviceField2.setNumCols(4);
    deviceField2.setIdentifier("");
    deviceField2.setEditable(false);
    deviceField2.setDisplayEvaluated(true);
    deviceField3.setOffsetNid(5);
    deviceField3.setTextOnly(true);
    deviceField3.setLabelString("Enabled Units:");
    deviceField3.setNumCols(20);
    deviceField3.setIdentifier("");
    deviceField3.setEditable(false);
    deviceField3.setDisplayEvaluated(true);
    deviceField5.setOffsetNid(2);
    deviceField5.setTextOnly(true);
    deviceField5.setLabelString("Connection:");
    deviceField5.setNumCols(8);
    deviceField5.setIdentifier("");
    deviceField5.setEditable(false);
    deviceField5.setDisplayEvaluated(true);
    deviceField4.setOffsetNid(3);
    deviceField4.setTextOnly(true);
    deviceField4.setLabelString("Control: ");
    deviceField4.setNumCols(8);
    deviceField4.setIdentifier("");
    deviceField4.setEditable(false);
    deviceField4.setDisplayEvaluated(true);
    this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
    this.getContentPane().add(deviceWave1, BorderLayout.CENTER);
    this.getContentPane().add(jPanel1, BorderLayout.NORTH);
    jPanel1.add(jPanel2, null);
    jPanel2.add(deviceField1, null);
    jPanel1.add(jPanel4, null);
    jPanel4.add(deviceField3, null);
    jPanel4.add(deviceField2, null);
    jPanel1.add(jPanel3, null);
    jPanel3.add(deviceField5, null);
    jPanel3.add(deviceField4, null);
  }

}
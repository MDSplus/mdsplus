

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

public class RFXAxiControlSetup extends DeviceSetup
{
    BorderLayout borderLayout1 = new BorderLayout();
    DeviceButtons deviceButtons1 = new DeviceButtons();
    JTabbedPane jTabbedPane1 = new JTabbedPane();
    DeviceWave deviceWave1 = new DeviceWave();
    DeviceWave deviceWave2 = new DeviceWave();
    DeviceWave deviceWave3 = new DeviceWave();
    DeviceWave deviceWave4 = new DeviceWave();
    DeviceWave deviceWave5 = new DeviceWave();
    DeviceWave deviceWave6 = new DeviceWave();
    DeviceWave deviceWave7 = new DeviceWave();
    DeviceWave deviceWave8 = new DeviceWave();
    DeviceWave deviceWave9 = new DeviceWave();
    DeviceWave deviceWave10 = new DeviceWave();
    DeviceWave deviceWave11 = new DeviceWave();
    JPanel jPanel1 = new JPanel();
    JPanel jPanel2 = new JPanel();
    DeviceField deviceField1 = new DeviceField();
    GridLayout gridLayout1 = new GridLayout();
    JPanel jPanel3 = new JPanel();
    DeviceField deviceField2 = new DeviceField();
    public RFXAxiControlSetup()
    {
        try
        {
          jbInit();
        }
        catch(Exception e)
        {
          e.printStackTrace();
        }
    }
    private void jbInit() throws Exception
    {
      this.setWidth(510);
      this.setHeight(510);
      this.setDeviceType("RFXAxiControl");
      this.setDeviceProvider("localhost");
      this.setDeviceTitle("Axisymmetric Control Setup");
      this.getContentPane().setLayout(borderLayout1);
      deviceWave1.setOffsetNid(3);
      deviceWave1.setIdentifier("");
      deviceWave1.setUpdateExpression("");
      deviceWave2.setOffsetNid(9);
      deviceWave2.setIdentifier("");
      deviceWave2.setUpdateExpression("");
      deviceWave3.setOffsetNid(15);
      deviceWave3.setIdentifier("");
      deviceWave3.setUpdateExpression("");
      deviceWave4.setOffsetNid(21);
      deviceWave4.setIdentifier("");
      deviceWave4.setUpdateExpression("");
      deviceWave5.setOffsetNid(27);
      deviceWave5.setIdentifier("");
      deviceWave5.setUpdateExpression("");
      deviceWave6.setOffsetNid(33);
      deviceWave6.setIdentifier("");
      deviceWave6.setUpdateExpression("");
      deviceWave7.setOffsetNid(39);
      deviceWave7.setIdentifier("");
      deviceWave7.setUpdateExpression("");
      deviceWave8.setOffsetNid(45);
      deviceWave8.setIdentifier("");
      deviceWave8.setUpdateExpression("");
      deviceWave9.setOffsetNid(51);
      deviceWave9.setIdentifier("");
      deviceWave9.setUpdateExpression("");
      deviceWave10.setOffsetNid(57);
      deviceWave10.setIdentifier("");
      deviceWave10.setUpdateExpression("");
      deviceWave11.setOffsetNid(63);
      deviceWave11.setIdentifier("");
      deviceWave11.setUpdateExpression("");
      deviceField1.setOffsetNid(1);
        deviceField1.setTextOnly(true);
        deviceField1.setLabelString("Comment: ");
        deviceField1.setNumCols(30);
        deviceField1.setIdentifier("");
        jPanel1.setLayout(gridLayout1);
        gridLayout1.setColumns(1);
        gridLayout1.setRows(2);
        deviceField2.setOffsetNid(68);
        deviceField2.setLabelString("Bv_Gain");
        deviceField2.setIdentifier("");
        this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
      this.getContentPane().add(jTabbedPane1, BorderLayout.CENTER);
      jTabbedPane1.add(deviceWave1,   "BV_add");
      jTabbedPane1.add(deviceWave2,   "Delta_Ip");
      jTabbedPane1.add(deviceWave3,   "Ref_delta_H");
      jTabbedPane1.add(deviceWave4,   "I_FS1_add");
      jTabbedPane1.add(deviceWave5,   "I_FS2_add");
      jTabbedPane1.add(deviceWave6,   "I_FS3_add");
      jTabbedPane1.add(deviceWave7,   "I_FS4_add");
      jTabbedPane1.add(deviceWave8,   "I_FS5_add");
      jTabbedPane1.add(deviceWave9,   "I_FS6_add");
      jTabbedPane1.add(deviceWave10,   "I_FS7_add");
      jTabbedPane1.add(deviceWave11,   "I_FS8_add");
        this.getContentPane().add(jPanel1, BorderLayout.NORTH);
        jPanel1.add(jPanel2, null);
        jPanel2.add(deviceField1, null);
        jPanel1.add(jPanel3, null);
        jPanel3.add(deviceField2, null);
    }

}
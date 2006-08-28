/**
 * <p>Title: </p>
 *
 * <p>Description: </p>
 *
 * <p>Copyright: Copyright (c) 2003</p>
 *
 * <p>Company: </p>
 *
 * @author not attributable
 * @version 1.0
 */

import java.awt.BorderLayout;
import java.awt.*;
import javax.swing.JPanel;

public class DEMOADCSetup
    extends DeviceSetup
{
    public DEMOADCSetup()
    {
        try
        {
            jbInit();
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
    }

    private void jbInit() throws Exception
    {
        this.setWidth(471);
        this.setHeight(471);
        this.setDeviceType("DEMOADC");
        this.setDeviceProvider("localhost");
        this.setDeviceTitle("Demo Adc Setup");
        this.getContentPane().setLayout(borderLayout1);
        deviceField2.setOffsetNid(2);
        deviceField2.setIdentifier("");
        deviceField2.setNumCols(25);
        deviceField2.setLabelString("Comment: ");
        deviceField2.setTextOnly(true);
        deviceChoice2.setOffsetNid(4);
        deviceChoice2.setIdentifier("");
        deviceChoice2.setUpdateIdentifier("");
        deviceChoice2.setChoiceItems(new String[]
                                     {"INTERNAL", "EXTERNAL"});
        deviceChoice2.setChoiceIntValues(null);
        deviceChoice2.setChoiceFloatValues(null);
        deviceChoice2.setLabelString("Clock Mode");
        deviceChoice1.setOffsetNid(7);
        deviceChoice1.setIdentifier("");
        deviceChoice1.setUpdateIdentifier("");
        deviceChoice1.setChoiceItems(new String[]
                                     {"1000", "5000", "1E4", "1E5"});
        deviceChoice1.setChoiceIntValues(null);
        deviceChoice1.setLabelString("Frequency: ");
        deviceField3.setOffsetNid(6);
        deviceField3.setIdentifier("");
        deviceField3.setNumCols(25);
        deviceField3.setLabelString("Clock source: ");
        deviceField4.setOffsetNid(5);
        deviceField4.setIdentifier("");
        deviceField4.setNumCols(25);
        deviceField4.setLabelString("Trig. Source: ");
        jPanel2.setLayout(gridLayout2);
        gridLayout2.setRows(2);
        deviceChannel1.setOffsetNid(9);
        deviceChannel1.setUpdateIdentifier("");
        deviceChannel1.setShowVal("");
        deviceChannel1.setColumns(1);
        deviceChannel1.setLabelString("Chan. 1");
        deviceChannel1.setBorderVisible(true);
        deviceChannel2.setOffsetNid(13);
        deviceChannel2.setUpdateIdentifier("");
        deviceChannel2.setShowVal("");
        deviceChannel2.setColumns(1);
        deviceChannel2.setLabelString("Chan 2");
        deviceChannel2.setBorderVisible(true);
        deviceField6.setOffsetNid(10);
        deviceField6.setIdentifier("");
        deviceField6.setLabelString("Start Idx: ");
        deviceField5.setOffsetNid(11);
        deviceField5.setIdentifier("");
        deviceField5.setLabelString("End Idx:");
        deviceField8.setOffsetNid(14);
        deviceField8.setIdentifier("");
        deviceField8.setLabelString("Start Idx: ");
        deviceField7.setOffsetNid(15);
        deviceField7.setIdentifier("");
        deviceField7.setLabelString("End Idx: ");
        this.getContentPane().add(deviceButtons1, java.awt.BorderLayout.SOUTH);
        gridLayout1.setRows(5);
        jPanel1.setLayout(gridLayout1);
        this.getContentPane().add(jPanel1, java.awt.BorderLayout.NORTH);
        deviceField1.setOffsetNid(1);
        deviceField1.setIdentifier("");
        deviceField1.setLabelString("CAMAC Name ");
        deviceField1.setTextOnly(true);
        jPanel1.add(jPanel3);
        jPanel3.add(deviceField1);
        jPanel3.add(deviceDispatch1);
        jPanel1.add(jPanel7);
        jPanel7.add(deviceField2);
        jPanel1.add(jPanel6);
        jPanel6.add(deviceChoice2);
        jPanel6.add(deviceChoice1);
        jPanel1.add(jPanel5);
        jPanel5.add(deviceField3);
        jPanel1.add(jPanel4);
        jPanel4.add(deviceField4);
        this.getContentPane().add(jPanel2, java.awt.BorderLayout.CENTER);
        jPanel2.add(deviceChannel1);
        jPanel2.add(deviceChannel2);
        deviceChannel1.add(jPanel8, java.awt.BorderLayout.CENTER);
        jPanel8.add(deviceField6);
        jPanel8.add(deviceField5);
        deviceChannel2.add(jPanel9, java.awt.BorderLayout.CENTER);
        jPanel9.add(deviceField8);
        jPanel9.add(deviceField7);
    }

    BorderLayout borderLayout1 = new BorderLayout();
    DeviceButtons deviceButtons1 = new DeviceButtons();
    JPanel jPanel1 = new JPanel();
    JPanel jPanel2 = new JPanel();
    GridLayout gridLayout1 = new GridLayout();
    JPanel jPanel3 = new JPanel();
    JPanel jPanel4 = new JPanel();
    JPanel jPanel5 = new JPanel();
    JPanel jPanel6 = new JPanel();
    JPanel jPanel7 = new JPanel();
    DeviceDispatch deviceDispatch1 = new DeviceDispatch();
    DeviceField deviceField1 = new DeviceField();
    DeviceField deviceField2 = new DeviceField();
    DeviceChoice deviceChoice1 = new DeviceChoice();
    DeviceChoice deviceChoice2 = new DeviceChoice();
    DeviceField deviceField3 = new DeviceField();
    DeviceField deviceField4 = new DeviceField();
    GridLayout gridLayout2 = new GridLayout();
    DeviceChannel deviceChannel1 = new DeviceChannel();
    DeviceChannel deviceChannel2 = new DeviceChannel();
    JPanel jPanel8 = new JPanel();
    JPanel jPanel9 = new JPanel();
    DeviceField deviceField5 = new DeviceField();
    DeviceField deviceField6 = new DeviceField();
    DeviceField deviceField7 = new DeviceField();
    DeviceField deviceField8 = new DeviceField();
}

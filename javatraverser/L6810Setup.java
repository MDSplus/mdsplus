

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

public class L6810Setup extends DeviceSetup
{
    BorderLayout borderLayout1 = new BorderLayout();
    DeviceButtons deviceButtons1 = new DeviceButtons();
    TitledBorder titledBorder1;
    DeviceField deviceField5 = new DeviceField();
    DeviceField deviceField3 = new DeviceField();
    JPanel jPanel8 = new JPanel();
    DeviceChoice deviceChoice2 = new DeviceChoice();
    JPanel jPanel4 = new JPanel();
    DeviceDispatch deviceDispatch1 = new DeviceDispatch();
    JPanel jPanel7 = new JPanel();
    DeviceChoice deviceChoice1 = new DeviceChoice();
    DeviceField deviceField4 = new DeviceField();
    DeviceChoice deviceChoice4 = new DeviceChoice();
    JPanel jPanel6 = new JPanel();
    DeviceField deviceField1 = new DeviceField();
    GridLayout gridLayout2 = new GridLayout();
    JPanel jPanel11 = new JPanel();
    JPanel jPanel9 = new JPanel();
    DeviceChoice deviceChoice3 = new DeviceChoice();
    DeviceField deviceField2 = new DeviceField();
    JTabbedPane jTabbedPane1 = new JTabbedPane();
    JPanel jPanel1 = new JPanel();
    GridLayout gridLayout1 = new GridLayout();
    DeviceChoice deviceChoice7 = new DeviceChoice();
    DeviceChoice deviceChoice8 = new DeviceChoice();
    JPanel jPanel5 = new JPanel();
    GridLayout gridLayout3 = new GridLayout();
    JPanel jPanel10 = new JPanel();
    DeviceField deviceField8 = new DeviceField();
    DeviceField deviceField9 = new DeviceField();
    JPanel jPanel12 = new JPanel();
    DeviceChoice deviceChoice9 = new DeviceChoice();
    DeviceChoice deviceChoice10 = new DeviceChoice();
    JPanel jPanel13 = new JPanel();
    GridLayout gridLayout4 = new GridLayout();
    JPanel jPanel14 = new JPanel();
    DeviceField deviceField10 = new DeviceField();
    DeviceField deviceField11 = new DeviceField();
    JPanel jPanel15 = new JPanel();
    DeviceChoice deviceChoice11 = new DeviceChoice();
    DeviceChoice deviceChoice12 = new DeviceChoice();
    JPanel jPanel16 = new JPanel();
    GridLayout gridLayout5 = new GridLayout();
    JPanel jPanel17 = new JPanel();
    DeviceField deviceField12 = new DeviceField();
    DeviceField deviceField13 = new DeviceField();
    JPanel jPanel18 = new JPanel();
  JPanel jPanel3 = new JPanel();
  DeviceField deviceField7 = new DeviceField();
  DeviceField deviceField6 = new DeviceField();
  DeviceChoice deviceChoice6 = new DeviceChoice();
  DeviceChoice deviceChoice5 = new DeviceChoice();
  JPanel jPanel2 = new JPanel();
    public L6810Setup()
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
      titledBorder1 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white,new Color(148, 145, 140)),"Trigger Information");
        this.setWidth(550);
      this.setHeight(450);
      this.setDeviceType("L6810");
      this.setDeviceProvider("localhost");
      this.setDeviceTitle("LeCroy L6810 Transient Recorder");
      this.getContentPane().setLayout(borderLayout1);
        deviceField5.setOffsetNid(4);
        deviceField5.setLabelString("CLock source: ");
        deviceField5.setNumCols(20);
        deviceField5.setIdentifier("");
        deviceField3.setOffsetNid(1);
        deviceField3.setTextOnly(true);
        deviceField3.setLabelString("CAMAC Name: ");
        deviceField3.setNumCols(15);
        deviceField3.setIdentifier("");
        deviceChoice2.setChoiceIntValues(new int[] {(int)1,(int)2,(int)3,(int)4});
        deviceChoice2.setChoiceFloatValues(null);
        deviceChoice2.setConvert(true);
        deviceChoice2.setOffsetNid(6);
        deviceChoice2.setLabelString("Tot Memory");
        deviceChoice2.setChoiceItems(new String[] {"512K", "1M", "1.5M", "2M"});
        deviceChoice2.setUpdateIdentifier("");
        deviceChoice2.setIdentifier("");
        jPanel4.setLayout(gridLayout2);
        deviceChoice1.setChoiceIntValues(new int[] {(int)1,(int)2,(int)4});
        deviceChoice1.setChoiceFloatValues(null);
        deviceChoice1.setOffsetNid(9);
        deviceChoice1.setLabelString("Channels: ");
        deviceChoice1.setChoiceItems(new String[] {"1", "2", "4"});
        deviceChoice1.setUpdateIdentifier("");
        deviceChoice1.setIdentifier("");
        deviceField4.setOffsetNid(5);
        deviceField4.setLabelString("Trigger source: ");
        deviceField4.setNumCols(30);
        deviceField4.setIdentifier("");
        deviceChoice4.setChoiceIntValues(new int[] {(int)0,(int)1,(int)2,(int)3,(int)4,(int)5,(int)6,(int)7,(int)8,(int)9,(int)10,(int)11,(int)12,(int)13,(int)14,(int)15,(int)16,(int)17});
        deviceChoice4.setChoiceFloatValues(null);
        deviceChoice4.setConvert(true);
        deviceChoice4.setOffsetNid(3);
        deviceChoice4.setLabelString("Clock Frequency:");
        deviceChoice4.setChoiceItems(new String[] {"External", "20", "50", "100", "200", "500", "1E3", "2E3", "5E3", "10E3", "20E3", "50E3", "100E3", "200E3", "500E3", "1E6", "2E6", "5E6"});
        deviceChoice4.setUpdateIdentifier("");
        deviceChoice4.setIdentifier("");
        deviceField1.setOffsetNid(2);
        deviceField1.setTextOnly(true);
        deviceField1.setLabelString("Comment: ");
        deviceField1.setNumCols(25);
        deviceField1.setIdentifier("");
        gridLayout2.setColumns(1);
        gridLayout2.setRows(5);
        deviceChoice3.setChoiceIntValues(new int[] {(int)1024,(int)2048,(int)4096,(int)8192,(int)16384,(int)32768,(int)65536,(int)313072,(int)262144,(int)524288,(int)1048576,(int)2097152,(int)4194304,(int)8288608});
        deviceChoice3.setChoiceFloatValues(null);
        deviceChoice3.setConvert(true);
        deviceChoice3.setOffsetNid(8);
        deviceChoice3.setLabelString("Samples per segment: ");
        deviceChoice3.setChoiceItems(new String[] {"1K", "2K", "4K", "8K", "16K", "32L", "64K", "128K", "256K", "512K", "1M", "2M", "4M", "8M"});
        deviceChoice3.setUpdateIdentifier("");
        deviceChoice3.setIdentifier("");
        deviceField2.setOffsetNid(7);
        deviceField2.setLabelString("Num Segments: ");
        deviceField2.setNumCols(4);
        deviceField2.setIdentifier("");
        jPanel1.setLayout(gridLayout1);
        gridLayout1.setColumns(1);
        gridLayout1.setRows(2);
        deviceChoice7.setIdentifier("");
        deviceChoice7.setUpdateIdentifier("");
        deviceChoice7.setChoiceItems(new String[] {"409.6mV", "1.024V", "2.048V", "4.096V", "10.24V", "25.6V", "51.2V", "102.4V"});
        deviceChoice7.setLabelString("Range: ");
        deviceChoice7.setOffsetNid(19);
        deviceChoice7.setConvert(true);
        deviceChoice7.setChoiceFloatValues(null);
        deviceChoice7.setChoiceIntValues(new int[] {(int)0,(int)1,(int)2,(int)3,(int)4,(int)5,(int)6,(int)7});
        deviceChoice8.setIdentifier("");
        deviceChoice8.setUpdateIdentifier("");
        deviceChoice8.setChoiceItems(new String[] {"Non inverting input, DC coupled", "Non inverting input, AC coupled", "Inverting input, DC coupled", "Inverting input, AC coupled",
        "Double ended input, DC coupled", "Double ended input, AC coupled", "Internally grounded, DC coupled", "Internally grounded, AC coupled"
        });
        deviceChoice8.setLabelString("Coupling: ");
        deviceChoice8.setOffsetNid(20);
        deviceChoice8.setConvert(true);
        deviceChoice8.setChoiceFloatValues(null);
        deviceChoice8.setChoiceIntValues(new int[] {(int)0,(int)1,(int)2,(int)3,(int)4,(int)5,(int)6,(int)7});
        gridLayout3.setColumns(1);
        gridLayout3.setRows(2);
        jPanel10.setLayout(gridLayout3);
        deviceField8.setOffsetNid(17);
        deviceField8.setLabelString("Start idx: ");
        deviceField8.setNumCols(15);
        deviceField8.setIdentifier("");
        deviceField9.setOffsetNid(18);
        deviceField9.setLabelString("End idx: ");
        deviceField9.setNumCols(15);
        deviceField9.setIdentifier("");
        deviceChoice9.setIdentifier("");
        deviceChoice9.setUpdateIdentifier("");
        deviceChoice9.setChoiceItems(new String[] {"409.6mV", "1.024V", "2.048V", "4.096V", "10.24V", "25.6V", "51.2V", "102.4V"});
        deviceChoice9.setLabelString("Range: ");
        deviceChoice9.setOffsetNid(25);
        deviceChoice9.setConvert(true);
        deviceChoice9.setChoiceFloatValues(null);
        deviceChoice9.setChoiceIntValues(new int[] {(int)0,(int)1,(int)2,(int)3,(int)4,(int)5,(int)6,(int)7});
        deviceChoice10.setIdentifier("");
        deviceChoice10.setUpdateIdentifier("");
        deviceChoice10.setChoiceItems(new String[] {"Non inverting input, DC coupled", "Non inverting input, AC coupled", "Inverting input, DC coupled", "Inverting input, AC coupled",
        "Double ended input, DC coupled", "Double ended input, AC coupled", "Internally grounded, DC coupled", "Internally grounded, AC coupled"
        });
        deviceChoice10.setLabelString("Coupling: ");
        deviceChoice10.setOffsetNid(26);
        deviceChoice10.setConvert(true);
        deviceChoice10.setChoiceFloatValues(null);
        deviceChoice10.setChoiceIntValues(new int[] {(int)0,(int)1,(int)2,(int)3,(int)4,(int)5,(int)6,(int)7});
        gridLayout4.setColumns(1);
        gridLayout4.setRows(2);
        jPanel14.setLayout(gridLayout4);
        deviceField10.setOffsetNid(23);
        deviceField10.setLabelString("Start idx: ");
        deviceField10.setNumCols(15);
        deviceField10.setIdentifier("");
        deviceField11.setOffsetNid(24);
        deviceField11.setLabelString("End idx: ");
        deviceField11.setNumCols(15);
        deviceField11.setIdentifier("");
        deviceChoice11.setIdentifier("");
        deviceChoice11.setUpdateIdentifier("");
        deviceChoice11.setChoiceItems(new String[] {"409.6mV", "1.024V", "2.048V", "4.096V", "10.24V", "25.6V", "51.2V", "102.4V"});
        deviceChoice11.setLabelString("Range: ");
        deviceChoice11.setOffsetNid(31);
        deviceChoice11.setConvert(true);
        deviceChoice11.setChoiceFloatValues(null);
        deviceChoice11.setChoiceIntValues(new int[] {(int)0,(int)1,(int)2,(int)3,(int)4,(int)5,(int)6,(int)7});
        deviceChoice12.setIdentifier("");
        deviceChoice12.setUpdateIdentifier("");
        deviceChoice12.setChoiceItems(new String[] {"Non inverting input, DC coupled", "Non inverting input, AC coupled", "Inverting input, DC coupled", "Inverting input, AC coupled",
        "Double ended input, DC coupled", "Double ended input, AC coupled", "Internally grounded, DC coupled", "Internally grounded, AC coupled"
        });
        deviceChoice12.setLabelString("Coupling: ");
        deviceChoice12.setOffsetNid(32);
        deviceChoice12.setConvert(true);
        deviceChoice12.setChoiceFloatValues(null);
        deviceChoice12.setChoiceIntValues(new int[] {(int)0,(int)1,(int)2,(int)3,(int)4,(int)5,(int)6,(int)7});
        gridLayout5.setColumns(1);
        gridLayout5.setRows(2);
        jPanel17.setLayout(gridLayout5);
        deviceField12.setOffsetNid(29);
        deviceField12.setLabelString("Start idx: ");
        deviceField12.setNumCols(15);
        deviceField12.setIdentifier("");
        deviceField13.setOffsetNid(30);
        deviceField13.setLabelString("End idx: ");
        deviceField13.setNumCols(15);
        deviceField13.setIdentifier("");
        deviceButtons1.setCheckExpressions(null);
        deviceButtons1.setCheckMessages(null);
        deviceButtons1.setMethods(new String[] {"INIT", "TRIGGER", "STORE"});
        deviceField7.setOffsetNid(11);
    deviceField7.setLabelString("Start idx: ");
    deviceField7.setNumCols(15);
    deviceField7.setIdentifier("");
    deviceField6.setOffsetNid(12);
    deviceField6.setLabelString("End idx: ");
    deviceField6.setNumCols(15);
    deviceField6.setIdentifier("");
    deviceChoice6.setChoiceIntValues(new int[] {(int)0,(int)1,(int)2,(int)3,(int)4,(int)5,(int)6,(int)7});
    deviceChoice6.setChoiceFloatValues(null);
    deviceChoice6.setConvert(true);
    deviceChoice6.setOffsetNid(13);
    deviceChoice6.setLabelString("Range: ");
    deviceChoice6.setChoiceItems(new String[] {"409.6mV", "1.024V", "2.048V", "4.096V", "10.24V", "25.6V", "51.2V", "102.4V"});
    deviceChoice6.setUpdateIdentifier("");
    deviceChoice6.setIdentifier("");
    deviceChoice5.setChoiceIntValues(new int[] {(int)0,(int)1,(int)2,(int)3,(int)4,(int)5,(int)6,(int)7});
    deviceChoice5.setChoiceFloatValues(null);
    deviceChoice5.setConvert(true);
    deviceChoice5.setOffsetNid(14);
    deviceChoice5.setLabelString("Coupling: ");
    deviceChoice5.setChoiceItems(new String[] {"Non inverting input, DC coupled", "Non inverting input, AC coupled", "Inverting input, DC coupled", "Inverting input, AC coupled",
        "Double ended input, DC coupled", "Double ended input, AC coupled", "Internally grounded, DC coupled", "Internally grounded, AC coupled"
        });
    deviceChoice5.setUpdateIdentifier("");
    deviceChoice5.setIdentifier("");
    this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
        this.getContentPane().add(jPanel4, BorderLayout.NORTH);
        jPanel6.add(deviceField1, null);
        jPanel6.add(deviceDispatch1, null);
        jPanel4.add(jPanel7, null);
        jPanel4.add(jPanel6, null);
        jPanel7.add(deviceField3, null);
        jPanel7.add(deviceChoice2, null);
        jPanel7.add(deviceChoice1, null);
        jPanel4.add(jPanel8, null);
        jPanel8.add(deviceField2, null);
        jPanel8.add(deviceChoice3, null);
        jPanel4.add(jPanel9, null);
        jPanel9.add(deviceField4, null);
        jPanel4.add(jPanel11, null);
        jPanel11.add(deviceChoice4, null);
        jPanel11.add(deviceField5, null);
        this.getContentPane().add(jTabbedPane1, BorderLayout.CENTER);
        jTabbedPane1.add(jPanel1,   "Chan 1");
    jPanel1.add(jPanel3, null);
    jPanel3.add(deviceField7, null);
    jPanel3.add(deviceField6, null);
    jPanel1.add(jPanel2, null);
    jPanel2.add(deviceChoice6, null);
    jPanel2.add(deviceChoice5, null);
        jTabbedPane1.add(jPanel10,   "Chan 2");
        jPanel12.add(deviceChoice7, null);
        jPanel12.add(deviceChoice8, null);
        jPanel10.add(jPanel5, null);
        jPanel10.add(jPanel12, null);
        jPanel5.add(deviceField8, null);
        jPanel5.add(deviceField9, null);
        jTabbedPane1.add(jPanel14,   "Chan 3");
        jPanel15.add(deviceChoice9, null);
        jPanel15.add(deviceChoice10, null);
        jPanel14.add(jPanel13, null);
        jPanel14.add(jPanel15, null);
        jPanel13.add(deviceField10, null);
        jPanel13.add(deviceField11, null);
        jTabbedPane1.add(jPanel17,   "Chan 4");
        jPanel18.add(deviceChoice11, null);
        jPanel18.add(deviceChoice12, null);
        jPanel17.add(jPanel16, null);
        jPanel17.add(jPanel18, null);
        jPanel16.add(deviceField12, null);
        jPanel16.add(deviceField13, null);
    }

}
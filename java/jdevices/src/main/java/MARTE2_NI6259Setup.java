/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/**
 *
 * @author manduchi
 */
public class MARTE2_NI6259Setup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;

	/**
	 * Creates new form MARTE2_NI6259Setup
	 */
	public MARTE2_NI6259Setup()
	{
		initComponents();
	}

	/**
	 * This method is called from within the constructor to initialize the form.
	 * WARNING: Do NOT modify this code. The content of this method is always
	 * regenerated by the Form Editor.
	 */
	@SuppressWarnings("unchecked")
	// <editor-fold defaultstate="collapsed" desc="Generated
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        deviceButtons1 = new DeviceButtons();
        jPanel1 = new javax.swing.JPanel();
        jPanel2 = new javax.swing.JPanel();
        deviceField2 = new DeviceField();
        deviceField3 = new DeviceField();
        deviceField4 = new DeviceField();
        deviceChoice1 = new DeviceChoice();
        jPanel3 = new javax.swing.JPanel();
        deviceChoice2 = new DeviceChoice();
        deviceChoice3 = new DeviceChoice();
        deviceChoice4 = new DeviceChoice();
        jPanel12 = new javax.swing.JPanel();
        deviceField5 = new DeviceField();
        deviceField6 = new DeviceField();
        deviceField1 = new DeviceField();
        jTabbedPane1 = new javax.swing.JTabbedPane();
        jPanel4 = new javax.swing.JPanel();
        deviceChoice5 = new DeviceChoice();
        deviceChoice6 = new DeviceChoice();
        deviceChoice7 = new DeviceChoice();
        deviceField7 = new DeviceField();
        jPanel5 = new javax.swing.JPanel();
        deviceChoice8 = new DeviceChoice();
        deviceChoice9 = new DeviceChoice();
        deviceChoice10 = new DeviceChoice();
        deviceField8 = new DeviceField();
        jPanel6 = new javax.swing.JPanel();
        deviceChoice11 = new DeviceChoice();
        deviceChoice12 = new DeviceChoice();
        deviceChoice13 = new DeviceChoice();
        deviceField9 = new DeviceField();
        jPanel8 = new javax.swing.JPanel();
        deviceChoice17 = new DeviceChoice();
        deviceChoice18 = new DeviceChoice();
        deviceChoice19 = new DeviceChoice();
        deviceField10 = new DeviceField();
        jPanel7 = new javax.swing.JPanel();
        deviceChoice14 = new DeviceChoice();
        deviceChoice15 = new DeviceChoice();
        deviceChoice16 = new DeviceChoice();
        deviceField11 = new DeviceField();
        jPanel9 = new javax.swing.JPanel();
        deviceChoice20 = new DeviceChoice();
        deviceChoice21 = new DeviceChoice();
        deviceChoice22 = new DeviceChoice();
        deviceField12 = new DeviceField();
        jPanel10 = new javax.swing.JPanel();
        deviceChoice23 = new DeviceChoice();
        deviceChoice24 = new DeviceChoice();
        deviceChoice25 = new DeviceChoice();
        deviceField13 = new DeviceField();
        jPanel11 = new javax.swing.JPanel();
        deviceChoice26 = new DeviceChoice();
        deviceChoice27 = new DeviceChoice();
        deviceChoice28 = new DeviceChoice();
        deviceField14 = new DeviceField();

        setDeviceProvider("localhost:8100");
        setDeviceTitle("MARTe2 NI6259");
        setDeviceType("MARTE2_NI6259");
        setHeight(300);
        setWidth(1000);
        getContentPane().add(deviceButtons1, java.awt.BorderLayout.PAGE_END);

        jPanel1.setLayout(new java.awt.GridLayout(3, 0));

        deviceField2.setIdentifier("");
        deviceField2.setLabelString("Board Id: ");
        deviceField2.setNumCols(4);
        deviceField2.setOffsetNid(13);
        jPanel2.add(deviceField2);

        deviceField3.setIdentifier("");
        deviceField3.setLabelString("(8*)Sampling Freq:");
        deviceField3.setOffsetNid(7);
        jPanel2.add(deviceField3);

        deviceField4.setIdentifier("");
        deviceField4.setLabelString("Delay Div.");
        deviceField4.setNumCols(4);
        deviceField4.setOffsetNid(16);
        jPanel2.add(deviceField4);

        deviceChoice1.setChoiceItems(new String[] {"SI_TC", "PFI0", "PFI1", "PFI2", "PFI3", "PFI4", "PFI5", "PFI6", "PFI7", "PFI8", "PFI9", "PFI10", "PFI11", "PFI12", "PFI13", "PFI14", "PFI15", "RTSI0", "RTSI1", "RTSI2", "RTSI3", "RTSI4", "RTSI5", "RTSI6", "RTSI7", "RTSI8", "RTSI9", "RTSI10", "RTSI11", "RTSI12", "RTSI13", "RTSI14", "RTSI15", "RTSI16", "RTSI17", "PULSE", "GPCRT0_OUT", "STAR_TRIGGER", "GPCTR1_OUT", "SCXI_TRIG1", "ANALOG_TRIGGER", "LOW"});
        deviceChoice1.setIdentifier("");
        deviceChoice1.setLabelString("Clock Sample Source: ");
        deviceChoice1.setOffsetNid(19);
        deviceChoice1.setUpdateIdentifier("");
        jPanel2.add(deviceChoice1);

        jPanel1.add(jPanel2);

        deviceChoice2.setChoiceItems(new String[] {"ACTIVE_HIGH_OR_RISING_EDGE", "ACTIVE_LOW_OR_FALLING_EDGE"});
        deviceChoice2.setIdentifier("");
        deviceChoice2.setLabelString("Clock Sample Polarity:");
        deviceChoice2.setOffsetNid(22);
        deviceChoice2.setUpdateIdentifier("");
        jPanel3.add(deviceChoice2);

        deviceChoice3.setChoiceItems(new String[] {"SI2TC", "PFI0", "PFI1", "PFI2", "PFI3", "PFI4", "PFI5", "PFI6", "PFI7", "PFI8", "PFI9", "PFI10", "PFI11", "PFI12", "PFI13", "PFI14", "PFI15", "RTSI0", "RTSI1", "RTSI2", "RTSI3", "RTSI4", "RTSI5", "RTSI6", "RTSI7", "RTSI8", "RTSI9", "RTSI10", "RTSI11", "RTSI12", "RTSI13", "RTSI14", "RTSI15", "RTSI16", "RTSI17", "PULSE", "GPCRT0_OUT", "STAR_TRIGGER", "ANALOG_TRIGGER", "LOW"});
        deviceChoice3.setIdentifier("");
        deviceChoice3.setLabelString("Clock Convert Source:");
        deviceChoice3.setOffsetNid(25);
        deviceChoice3.setUpdateIdentifier("");
        jPanel3.add(deviceChoice3);

        deviceChoice4.setChoiceItems(new String[] {"RISING_EDGE", "FALLING_EDGE"});
        deviceChoice4.setIdentifier("");
        deviceChoice4.setLabelString("Clock Convert Polarity: ");
        deviceChoice4.setOffsetNid(28);
        deviceChoice4.setUpdateIdentifier("");
        jPanel3.add(deviceChoice4);

        jPanel1.add(jPanel3);

        deviceField5.setIdentifier("");
        deviceField5.setLabelString("Samples: ");
        deviceField5.setOffsetNid(58);
        jPanel12.add(deviceField5);

        deviceField6.setIdentifier("");
        deviceField6.setLabelString("Storage CPUs");
        deviceField6.setNumCols(4);
        deviceField6.setOffsetNid(38);
        jPanel12.add(deviceField6);

        deviceField1.setIdentifier("");
        deviceField1.setLabelString("Acq. CPUs");
        deviceField1.setNumCols(4);
        deviceField1.setOffsetNid(31);
        jPanel12.add(deviceField1);

        jPanel1.add(jPanel12);

        getContentPane().add(jPanel1, java.awt.BorderLayout.NORTH);

        deviceChoice5.setChoiceFloatValues(new float[] {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f});
        deviceChoice5.setChoiceItems(new String[] {"0.1", "0.2", "0.5", "1", "2", "5", "10"});
        deviceChoice5.setIdentifier("");
        deviceChoice5.setLabelString("Input Range: ");
        deviceChoice5.setOffsetNid(70);
        deviceChoice5.setUpdateIdentifier("");
        jPanel4.add(deviceChoice5);

        deviceChoice6.setChoiceItems(new String[] {"Bipolar", "Unipolar"});
        deviceChoice6.setIdentifier("");
        deviceChoice6.setLabelString("Input Polarity: ");
        deviceChoice6.setOffsetNid(76);
        deviceChoice6.setUpdateIdentifier("");
        jPanel4.add(deviceChoice6);

        deviceChoice7.setChoiceItems(new String[] {"Differential", "RSE", "NRSE"});
        deviceChoice7.setIdentifier("");
        deviceChoice7.setLabelString("Input Mode: ");
        deviceChoice7.setOffsetNid(79);
        deviceChoice7.setUpdateIdentifier("");
        jPanel4.add(deviceChoice7);

        deviceField7.setIdentifier("");
        deviceField7.setLabelString("Segment len (0 to disable writing in tree):");
        deviceField7.setOffsetNid(64);
        jPanel4.add(deviceField7);

        jTabbedPane1.addTab("Ch 1", jPanel4);

        deviceChoice8.setChoiceFloatValues(new float[] {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f});
        deviceChoice8.setChoiceItems(new String[] {"0.1", "0.2", "0.5", "1", "2", "5", "10"});
        deviceChoice8.setIdentifier("");
        deviceChoice8.setLabelString("Input Range: ");
        deviceChoice8.setOffsetNid(92);
        deviceChoice8.setUpdateIdentifier("");
        jPanel5.add(deviceChoice8);

        deviceChoice9.setChoiceItems(new String[] {"Bipolar", "Unipolar"});
        deviceChoice9.setIdentifier("");
        deviceChoice9.setLabelString("Input Polarity: ");
        deviceChoice9.setOffsetNid(101);
        deviceChoice9.setUpdateIdentifier("");
        jPanel5.add(deviceChoice9);

        deviceChoice10.setChoiceItems(new String[] {"Differential", "RSE", "NRSE"});
        deviceChoice10.setIdentifier("");
        deviceChoice10.setLabelString("Input Mode: ");
        deviceChoice10.setOffsetNid(98);
        deviceChoice10.setUpdateIdentifier("");
        jPanel5.add(deviceChoice10);

        deviceField8.setIdentifier("");
        deviceField8.setLabelString("Segment len (0 to disable writing in tree):");
        deviceField8.setOffsetNid(86);
        jPanel5.add(deviceField8);

        jTabbedPane1.addTab("Ch 2", jPanel5);

        deviceChoice11.setChoiceFloatValues(new float[] {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f});
        deviceChoice11.setChoiceItems(new String[] {"0.1", "0.2", "0.5", "1", "2", "5", "10"});
        deviceChoice11.setIdentifier("");
        deviceChoice11.setLabelString("Input Range: ");
        deviceChoice11.setOffsetNid(114);
        deviceChoice11.setUpdateIdentifier("");
        jPanel6.add(deviceChoice11);

        deviceChoice12.setChoiceItems(new String[] {"Bipolar", "Unipolar"});
        deviceChoice12.setIdentifier("");
        deviceChoice12.setLabelString("Input Polarity: ");
        deviceChoice12.setOffsetNid(120);
        deviceChoice12.setUpdateIdentifier("");
        jPanel6.add(deviceChoice12);

        deviceChoice13.setChoiceItems(new String[] {"Differential", "RSE", "NRSE"});
        deviceChoice13.setIdentifier("");
        deviceChoice13.setLabelString("Input Mode: ");
        deviceChoice13.setOffsetNid(123);
        deviceChoice13.setUpdateIdentifier("");
        jPanel6.add(deviceChoice13);

        deviceField9.setIdentifier("");
        deviceField9.setLabelString("Segment len (0 to disable writing in tree):");
        deviceField9.setOffsetNid(108);
        jPanel6.add(deviceField9);

        jTabbedPane1.addTab("Ch 3", jPanel6);

        deviceChoice17.setChoiceFloatValues(new float[] {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f});
        deviceChoice17.setChoiceItems(new String[] {"0.1", "0.2", "0.5", "1", "2", "5", "10"});
        deviceChoice17.setIdentifier("");
        deviceChoice17.setLabelString("Input Range: ");
        deviceChoice17.setOffsetNid(136);
        deviceChoice17.setUpdateIdentifier("");
        jPanel8.add(deviceChoice17);

        deviceChoice18.setChoiceItems(new String[] {"Bipolar", "Unipolar"});
        deviceChoice18.setIdentifier("");
        deviceChoice18.setLabelString("Input Polarity: ");
        deviceChoice18.setOffsetNid(142);
        deviceChoice18.setUpdateIdentifier("");
        jPanel8.add(deviceChoice18);

        deviceChoice19.setChoiceItems(new String[] {"Differential", "RSE", "NRSE"});
        deviceChoice19.setIdentifier("");
        deviceChoice19.setLabelString("Input Mode: ");
        deviceChoice19.setOffsetNid(145);
        deviceChoice19.setUpdateIdentifier("");
        jPanel8.add(deviceChoice19);

        deviceField10.setIdentifier("");
        deviceField10.setLabelString("Segment len (0 to disable writing in tree):");
        deviceField10.setOffsetNid(130);
        jPanel8.add(deviceField10);

        jTabbedPane1.addTab("Ch 4", jPanel8);

        deviceChoice14.setChoiceFloatValues(new float[] {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f});
        deviceChoice14.setChoiceItems(new String[] {"0.1", "0.2", "0.5", "1", "2", "5", "10"});
        deviceChoice14.setIdentifier("");
        deviceChoice14.setLabelString("Input Range: ");
        deviceChoice14.setOffsetNid(158);
        deviceChoice14.setUpdateIdentifier("");
        jPanel7.add(deviceChoice14);

        deviceChoice15.setChoiceItems(new String[] {"Bipolar", "Unipolar"});
        deviceChoice15.setIdentifier("");
        deviceChoice15.setLabelString("Input Polarity: ");
        deviceChoice15.setOffsetNid(164);
        deviceChoice15.setUpdateIdentifier("");
        jPanel7.add(deviceChoice15);

        deviceChoice16.setChoiceItems(new String[] {"Differential", "RSE", "NRSE"});
        deviceChoice16.setIdentifier("");
        deviceChoice16.setLabelString("Input Mode: ");
        deviceChoice16.setOffsetNid(167);
        deviceChoice16.setUpdateIdentifier("");
        jPanel7.add(deviceChoice16);

        deviceField11.setIdentifier("");
        deviceField11.setLabelString("Segment len (0 to disable writing in tree):");
        deviceField11.setOffsetNid(152);
        jPanel7.add(deviceField11);

        jTabbedPane1.addTab("Ch 5", jPanel7);

        deviceChoice20.setChoiceFloatValues(new float[] {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f});
        deviceChoice20.setChoiceItems(new String[] {"0.1", "0.2", "0.5", "1", "2", "5", "10"});
        deviceChoice20.setIdentifier("");
        deviceChoice20.setLabelString("Input Range: ");
        deviceChoice20.setOffsetNid(180);
        deviceChoice20.setUpdateIdentifier("");
        jPanel9.add(deviceChoice20);

        deviceChoice21.setChoiceItems(new String[] {"Bipolar", "Unipolar"});
        deviceChoice21.setIdentifier("");
        deviceChoice21.setLabelString("Input Polarity: ");
        deviceChoice21.setOffsetNid(186);
        deviceChoice21.setUpdateIdentifier("");
        jPanel9.add(deviceChoice21);

        deviceChoice22.setChoiceItems(new String[] {"Differential", "RSE", "NRSE"});
        deviceChoice22.setIdentifier("");
        deviceChoice22.setLabelString("Input Mode: ");
        deviceChoice22.setOffsetNid(189);
        deviceChoice22.setUpdateIdentifier("");
        jPanel9.add(deviceChoice22);

        deviceField12.setIdentifier("");
        deviceField12.setLabelString("Segment len (0 to disable writing in tree):");
        deviceField12.setOffsetNid(174);
        jPanel9.add(deviceField12);

        jTabbedPane1.addTab("Ch 6", jPanel9);

        deviceChoice23.setChoiceFloatValues(new float[] {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f});
        deviceChoice23.setChoiceItems(new String[] {"0.1", "0.2", "0.5", "1", "2", "5", "10"});
        deviceChoice23.setIdentifier("");
        deviceChoice23.setLabelString("Input Range: ");
        deviceChoice23.setOffsetNid(202);
        deviceChoice23.setUpdateIdentifier("");
        jPanel10.add(deviceChoice23);

        deviceChoice24.setChoiceItems(new String[] {"Bipolar", "Unipolar"});
        deviceChoice24.setIdentifier("");
        deviceChoice24.setLabelString("Input Polarity: ");
        deviceChoice24.setOffsetNid(208);
        deviceChoice24.setUpdateIdentifier("");
        jPanel10.add(deviceChoice24);

        deviceChoice25.setChoiceItems(new String[] {"Differential", "RSE", "NRSE"});
        deviceChoice25.setIdentifier("");
        deviceChoice25.setLabelString("Input Mode: ");
        deviceChoice25.setOffsetNid(211);
        deviceChoice25.setUpdateIdentifier("");
        jPanel10.add(deviceChoice25);

        deviceField13.setIdentifier("");
        deviceField13.setLabelString("Segment len (0 to disable writing in tree):");
        deviceField13.setOffsetNid(196);
        jPanel10.add(deviceField13);

        jTabbedPane1.addTab("Ch 7", jPanel10);

        deviceChoice26.setChoiceFloatValues(new float[] {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f});
        deviceChoice26.setChoiceItems(new String[] {"0.1", "0.2", "0.5", "1", "2", "5", "10"});
        deviceChoice26.setIdentifier("");
        deviceChoice26.setLabelString("Input Range: ");
        deviceChoice26.setOffsetNid(224);
        deviceChoice26.setUpdateIdentifier("");
        jPanel11.add(deviceChoice26);

        deviceChoice27.setChoiceItems(new String[] {"Bipolar", "Unipolar"});
        deviceChoice27.setIdentifier("");
        deviceChoice27.setLabelString("Input Polarity: ");
        deviceChoice27.setOffsetNid(230);
        deviceChoice27.setUpdateIdentifier("");
        jPanel11.add(deviceChoice27);

        deviceChoice28.setChoiceItems(new String[] {"Differential", "RSE", "NRSE"});
        deviceChoice28.setIdentifier("");
        deviceChoice28.setLabelString("Input Mode: ");
        deviceChoice28.setOffsetNid(233);
        deviceChoice28.setUpdateIdentifier("");
        jPanel11.add(deviceChoice28);

        deviceField14.setIdentifier("");
        deviceField14.setLabelString("Segment len (0 to disable writing in tree):");
        deviceField14.setOffsetNid(218);
        jPanel11.add(deviceField14);

        jTabbedPane1.addTab("Ch 8", jPanel11);

        getContentPane().add(jTabbedPane1, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private DeviceButtons deviceButtons1;
    private DeviceChoice deviceChoice1;
    private DeviceChoice deviceChoice10;
    private DeviceChoice deviceChoice11;
    private DeviceChoice deviceChoice12;
    private DeviceChoice deviceChoice13;
    private DeviceChoice deviceChoice14;
    private DeviceChoice deviceChoice15;
    private DeviceChoice deviceChoice16;
    private DeviceChoice deviceChoice17;
    private DeviceChoice deviceChoice18;
    private DeviceChoice deviceChoice19;
    private DeviceChoice deviceChoice2;
    private DeviceChoice deviceChoice20;
    private DeviceChoice deviceChoice21;
    private DeviceChoice deviceChoice22;
    private DeviceChoice deviceChoice23;
    private DeviceChoice deviceChoice24;
    private DeviceChoice deviceChoice25;
    private DeviceChoice deviceChoice26;
    private DeviceChoice deviceChoice27;
    private DeviceChoice deviceChoice28;
    private DeviceChoice deviceChoice3;
    private DeviceChoice deviceChoice4;
    private DeviceChoice deviceChoice5;
    private DeviceChoice deviceChoice6;
    private DeviceChoice deviceChoice7;
    private DeviceChoice deviceChoice8;
    private DeviceChoice deviceChoice9;
    private DeviceField deviceField1;
    private DeviceField deviceField10;
    private DeviceField deviceField11;
    private DeviceField deviceField12;
    private DeviceField deviceField13;
    private DeviceField deviceField14;
    private DeviceField deviceField2;
    private DeviceField deviceField3;
    private DeviceField deviceField4;
    private DeviceField deviceField5;
    private DeviceField deviceField6;
    private DeviceField deviceField7;
    private DeviceField deviceField8;
    private DeviceField deviceField9;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel10;
    private javax.swing.JPanel jPanel11;
    private javax.swing.JPanel jPanel12;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JPanel jPanel4;
    private javax.swing.JPanel jPanel5;
    private javax.swing.JPanel jPanel6;
    private javax.swing.JPanel jPanel7;
    private javax.swing.JPanel jPanel8;
    private javax.swing.JPanel jPanel9;
    private javax.swing.JTabbedPane jTabbedPane1;
    // End of variables declaration//GEN-END:variables
}

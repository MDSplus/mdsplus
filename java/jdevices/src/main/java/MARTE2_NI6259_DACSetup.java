/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/**
 *
 * @author manduchi
 */
public class MARTE2_NI6259_DACSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;

	/**
	 * Creates new form MARTE2_NI6259_DACSetup
	 */
	public MARTE2_NI6259_DACSetup()
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

        jPanel2 = new javax.swing.JPanel();
        jPanel14 = new javax.swing.JPanel();
        deviceField1 = new DeviceField();
        jPanel15 = new javax.swing.JPanel();
        deviceField2 = new DeviceField();
        deviceChoice2 = new DeviceChoice();
        deviceChoice1 = new DeviceChoice();
        jTabbedPane1 = new javax.swing.JTabbedPane();
        jPanel1 = new javax.swing.JPanel();
        jPanel3 = new javax.swing.JPanel();
        deviceChoice3 = new DeviceChoice();
        jPanel4 = new javax.swing.JPanel();
        deviceField3 = new DeviceField();
        jPanel5 = new javax.swing.JPanel();
        jPanel6 = new javax.swing.JPanel();
        deviceChoice4 = new DeviceChoice();
        jPanel7 = new javax.swing.JPanel();
        deviceField4 = new DeviceField();
        jPanel8 = new javax.swing.JPanel();
        jPanel9 = new javax.swing.JPanel();
        deviceChoice5 = new DeviceChoice();
        jPanel10 = new javax.swing.JPanel();
        deviceField5 = new DeviceField();
        jPanel11 = new javax.swing.JPanel();
        jPanel12 = new javax.swing.JPanel();
        deviceChoice6 = new DeviceChoice();
        jPanel13 = new javax.swing.JPanel();
        deviceField6 = new DeviceField();
        deviceButtons1 = new DeviceButtons();

        setDeviceProvider("localhost:8100");
        setDeviceTitle("MARTe2 NI 6250 DAC");
        setDeviceType("MARTE2_NI6259_DAC");
        setHeight(300);
        setWidth(700);

        jPanel2.setLayout(new java.awt.GridLayout(2, 1));

        deviceField1.setIdentifier("");
        deviceField1.setLabelString("Board Id:");
        deviceField1.setNumCols(4);
        deviceField1.setOffsetNid(10);
        jPanel14.add(deviceField1);

        jPanel2.add(jPanel14);

        deviceField2.setIdentifier("");
        deviceField2.setLabelString("Clock update divisor: ");
        deviceField2.setNumCols(3);
        deviceField2.setOffsetNid(16);
        jPanel15.add(deviceField2);

        deviceChoice2.setChoiceItems(new String[] {"RISING_EDGE", "FALLING_EDGE"});
        deviceChoice2.setIdentifier("");
        deviceChoice2.setLabelString("Clock update polarity: ");
        deviceChoice2.setOffsetNid(16);
        deviceChoice2.setUpdateIdentifier("");
        jPanel15.add(deviceChoice2);

        deviceChoice1.setChoiceItems(new String[] {"UI_TC", "PFI0", "PFI1", "PFI2", "PFI3", "PFI4", "PFI5", "PFI6", "PFI7", "PFI8", "PFI9", "PFI10", "PFI11", "PFI12", "PFI13", "PFI14", "PFI15", "RTSI0", "RTSI1", "RTSI2", "RTSI3", "RTSI4", "RTSI5", "RTSI6", "RTSI7", "GPCRT0_OUT", "GPCRT1_OUT", "START_TRIGGER", "ANALOG_TRIGGER", "LOW"});
        deviceChoice1.setIdentifier("");
        deviceChoice1.setLabelString("Clock update source: ");
        deviceChoice1.setOffsetNid(13);
        deviceChoice1.setUpdateIdentifier("");
        jPanel15.add(deviceChoice1);

        jPanel2.add(jPanel15);

        getContentPane().add(jPanel2, java.awt.BorderLayout.PAGE_START);

        jPanel1.setLayout(new java.awt.GridLayout(2, 1));

        deviceChoice3.setChoiceItems(new String[] {"Unipolar", "Bipolar"});
        deviceChoice3.setIdentifier("");
        deviceChoice3.setLabelString("Polarity:");
        deviceChoice3.setOffsetNid(33);
        deviceChoice3.setUpdateIdentifier("");
        jPanel3.add(deviceChoice3);

        jPanel1.add(jPanel3);

        deviceField3.setIdentifier("");
        deviceField3.setLabelString("Value: ");
        deviceField3.setNumCols(40);
        deviceField3.setOffsetNid(25);
        jPanel4.add(deviceField3);

        jPanel1.add(jPanel4);

        jTabbedPane1.addTab("Chan 1", jPanel1);

        jPanel5.setLayout(new java.awt.GridLayout(2, 1));

        deviceChoice4.setChoiceItems(new String[] {"Unipolar", "Bipolar"});
        deviceChoice4.setIdentifier("");
        deviceChoice4.setLabelString("Polarity:");
        deviceChoice4.setOffsetNid(50);
        deviceChoice4.setUpdateIdentifier("");
        jPanel6.add(deviceChoice4);

        jPanel5.add(jPanel6);

        deviceField4.setIdentifier("");
        deviceField4.setLabelString("Value: ");
        deviceField4.setNumCols(40);
        deviceField4.setOffsetNid(42);
        jPanel7.add(deviceField4);

        jPanel5.add(jPanel7);

        jTabbedPane1.addTab("Chan 2", jPanel5);

        jPanel8.setLayout(new java.awt.GridLayout(2, 1));

        deviceChoice5.setChoiceItems(new String[] {"Unipolar", "Bipolar"});
        deviceChoice5.setIdentifier("");
        deviceChoice5.setLabelString("Polarity:");
        deviceChoice5.setOffsetNid(64);
        deviceChoice5.setUpdateIdentifier("");
        jPanel9.add(deviceChoice5);

        jPanel8.add(jPanel9);

        deviceField5.setIdentifier("");
        deviceField5.setLabelString("Value: ");
        deviceField5.setNumCols(40);
        deviceField5.setOffsetNid(56);
        jPanel10.add(deviceField5);

        jPanel8.add(jPanel10);

        jTabbedPane1.addTab("Chan 3", jPanel8);

        jPanel11.setLayout(new java.awt.GridLayout(2, 1));

        deviceChoice6.setChoiceItems(new String[] {"Unipolar", "Bipolar"});
        deviceChoice6.setIdentifier("");
        deviceChoice6.setLabelString("Polarity:");
        deviceChoice6.setOffsetNid(78);
        deviceChoice6.setUpdateIdentifier("");
        jPanel12.add(deviceChoice6);

        jPanel11.add(jPanel12);

        deviceField6.setIdentifier("");
        deviceField6.setLabelString("Value: ");
        deviceField6.setNumCols(40);
        deviceField6.setOffsetNid(70);
        jPanel13.add(deviceField6);

        jPanel11.add(jPanel13);

        jTabbedPane1.addTab("Chan 4", jPanel11);

        getContentPane().add(jTabbedPane1, java.awt.BorderLayout.CENTER);
        getContentPane().add(deviceButtons1, java.awt.BorderLayout.PAGE_END);

        getAccessibleContext().setAccessibleName("");
    }// </editor-fold>//GEN-END:initComponents

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private DeviceButtons deviceButtons1;
    private DeviceChoice deviceChoice1;
    private DeviceChoice deviceChoice2;
    private DeviceChoice deviceChoice3;
    private DeviceChoice deviceChoice4;
    private DeviceChoice deviceChoice5;
    private DeviceChoice deviceChoice6;
    private DeviceField deviceField1;
    private DeviceField deviceField2;
    private DeviceField deviceField3;
    private DeviceField deviceField4;
    private DeviceField deviceField5;
    private DeviceField deviceField6;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel10;
    private javax.swing.JPanel jPanel11;
    private javax.swing.JPanel jPanel12;
    private javax.swing.JPanel jPanel13;
    private javax.swing.JPanel jPanel14;
    private javax.swing.JPanel jPanel15;
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

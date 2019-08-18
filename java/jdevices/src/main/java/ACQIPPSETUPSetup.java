/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Cesare
 */
public class ACQIPPSETUPSetup extends DeviceSetup {

    /**
     * Creates new form ACQIPPSETUPSetup
     */
    public ACQIPPSETUPSetup() {
	initComponents();
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

	deviceButtons1 = new DeviceButtons();
	jPanel1 = new javax.swing.JPanel();
	jPanel7 = new javax.swing.JPanel();
	jPanel2 = new javax.swing.JPanel();
	deviceField1 = new DeviceField();
	deviceDispatch1 = new DeviceDispatch();
	jPanel3 = new javax.swing.JPanel();
	deviceField2 = new DeviceField();
	deviceField3 = new DeviceField();
	deviceField4 = new DeviceField();
	deviceChoice1 = new DeviceChoice();
	jPanel5 = new javax.swing.JPanel();
	deviceField8 = new DeviceField();
	deviceField9 = new DeviceField();
	deviceField10 = new DeviceField();
	deviceChoice3 = new DeviceChoice();
	jPanel4 = new javax.swing.JPanel();
	jPanel6 = new javax.swing.JPanel();
	deviceField5 = new DeviceField();
	deviceField6 = new DeviceField();
	deviceField7 = new DeviceField();
	deviceChoice2 = new DeviceChoice();
	deviceField11 = new DeviceField();

	setDeviceProvider("localhost");
	setDeviceTitle("Probes & TC  Acquisition Setup");
	setDeviceType("ACQIPPSETUP");
	setHeight(400);
	setWidth(850);
	getContentPane().add(deviceButtons1, java.awt.BorderLayout.PAGE_END);

	jPanel1.setLayout(new java.awt.GridLayout(2, 0));

	jPanel7.setLayout(new java.awt.GridLayout(3, 0));

	jPanel2.setBorder(new javax.swing.border.MatteBorder(null));
	jPanel2.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT));

	deviceField1.setIdentifier("");
	deviceField1.setLabelString("Comment:");
	deviceField1.setNumCols(50);
	deviceField1.setOffsetNid(1);
	deviceField1.setTextOnly(true);
	jPanel2.add(deviceField1);
	jPanel2.add(deviceDispatch1);

	jPanel7.add(jPanel2);

	jPanel3.setBorder(javax.swing.BorderFactory.createTitledBorder("Probe Acquisition"));
	jPanel3.setName("Probe Acquisition"); // NOI18N

	deviceField2.setIdentifier("");
	deviceField2.setLabelString("Start Time (s):");
	deviceField2.setOffsetNid(3);
	jPanel3.add(deviceField2);

	deviceField3.setIdentifier("");
	deviceField3.setLabelString("End Time (s):");
	deviceField3.setOffsetNid(4);
	jPanel3.add(deviceField3);

	deviceField4.setIdentifier("");
	deviceField4.setLabelString("Frequency (Hz):");
	deviceField4.setOffsetNid(5);
	jPanel3.add(deviceField4);

	deviceChoice1.setChoiceItems(new String[] {"EXTERNAL", "INTERNAL"});
	deviceChoice1.setIdentifier("");
	deviceChoice1.setLabelString("Trig. Mode:");
	deviceChoice1.setOffsetNid(6);
	deviceChoice1.setUpdateIdentifier("");
	jPanel3.add(deviceChoice1);

	jPanel7.add(jPanel3);

	jPanel5.setBorder(javax.swing.BorderFactory.createTitledBorder("Thermocouples Acquisition"));

	deviceField8.setIdentifier("");
	deviceField8.setLabelString("Start Time (s):");
	deviceField8.setOffsetNid(16);
	jPanel5.add(deviceField8);

	deviceField9.setIdentifier("");
	deviceField9.setLabelString("End Time (s):");
	deviceField9.setOffsetNid(17);
	jPanel5.add(deviceField9);

	deviceField10.setIdentifier("");
	deviceField10.setLabelString("Frequency (Hz):");
	deviceField10.setOffsetNid(18);
	jPanel5.add(deviceField10);

	deviceChoice3.setChoiceItems(new String[] {"EXTERNAL", "INTERNAL"});
	deviceChoice3.setIdentifier("");
	deviceChoice3.setLabelString("Trig. Mode:");
	deviceChoice3.setOffsetNid(19);
	deviceChoice3.setUpdateIdentifier("");
	jPanel5.add(deviceChoice3);

	jPanel7.add(jPanel5);

	jPanel1.add(jPanel7);

	jPanel4.setBorder(javax.swing.BorderFactory.createTitledBorder("Sweep Voltage Configuration"));
	jPanel4.setLayout(new java.awt.GridLayout(3, 0));

	jPanel6.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.CENTER, 0, 0));

	deviceField5.setIdentifier("");
	deviceField5.setLabelString("Min Value (V):");
	deviceField5.setOffsetNid(11);
	jPanel6.add(deviceField5);

	deviceField6.setIdentifier("");
	deviceField6.setLabelString("Max Value (V):");
	deviceField6.setOffsetNid(12);
	jPanel6.add(deviceField6);

	deviceField7.setIdentifier("");
	deviceField7.setLabelString("Frequency (Hz):");
	deviceField7.setOffsetNid(13);
	jPanel6.add(deviceField7);

	deviceChoice2.setChoiceItems(new String[] {"EXTERNAL", "INTERNAL"});
	deviceChoice2.setIdentifier("");
	deviceChoice2.setLabelString("Trig. Mode:");
	deviceChoice2.setOffsetNid(14);
	deviceChoice2.setUpdateIdentifier("");
	jPanel6.add(deviceChoice2);

	jPanel4.add(jPanel6);

	deviceField11.setIdentifier("");
	deviceField11.setLabelString("Expression:");
	deviceField11.setNumCols(60);
	deviceField11.setOffsetNid(22);
	jPanel4.add(deviceField11);

	jPanel1.add(jPanel4);

	getContentPane().add(jPanel1, java.awt.BorderLayout.CENTER);

	getAccessibleContext().setAccessibleName("");
    }// </editor-fold>//GEN-END:initComponents
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private DeviceButtons deviceButtons1;
    private DeviceChoice deviceChoice1;
    private DeviceChoice deviceChoice2;
    private DeviceChoice deviceChoice3;
    private DeviceDispatch deviceDispatch1;
    private DeviceField deviceField1;
    private DeviceField deviceField10;
    private DeviceField deviceField11;
    private DeviceField deviceField2;
    private DeviceField deviceField3;
    private DeviceField deviceField4;
    private DeviceField deviceField5;
    private DeviceField deviceField6;
    private DeviceField deviceField7;
    private DeviceField deviceField8;
    private DeviceField deviceField9;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JPanel jPanel4;
    private javax.swing.JPanel jPanel5;
    private javax.swing.JPanel jPanel6;
    private javax.swing.JPanel jPanel7;
    // End of variables declaration//GEN-END:variables
}
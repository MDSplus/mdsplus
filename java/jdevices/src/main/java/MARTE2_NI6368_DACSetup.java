/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author mdsplus
 */
public class MARTE2_NI6368_DACSetup extends DeviceSetup {

    /**
     * Creates new form MARTE2_NI6368_DACSetup
     */
    public MARTE2_NI6368_DACSetup() {
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
        deviceField1 = new DeviceField();
        jTabbedPane1 = new javax.swing.JTabbedPane();
        jPanel2 = new javax.swing.JPanel();
        jPanel3 = new javax.swing.JPanel();
        deviceChoice1 = new DeviceChoice();
        jPanel4 = new javax.swing.JPanel();
        deviceField4 = new DeviceField();
        jPanel5 = new javax.swing.JPanel();
        jPanel6 = new javax.swing.JPanel();
        deviceChoice2 = new DeviceChoice();
        jPanel7 = new javax.swing.JPanel();
        deviceField5 = new DeviceField();
        jPanel8 = new javax.swing.JPanel();
        jPanel9 = new javax.swing.JPanel();
        deviceChoice3 = new DeviceChoice();
        jPanel10 = new javax.swing.JPanel();
        deviceField6 = new DeviceField();
        jPanel11 = new javax.swing.JPanel();
        jPanel12 = new javax.swing.JPanel();
        deviceChoice4 = new DeviceChoice();
        jPanel13 = new javax.swing.JPanel();
        deviceField7 = new DeviceField();

        setDeviceProvider("localhost:8100");
        setDeviceTitle("NI6368 DAC");
        setDeviceType("MARTE2_NI6368_DAC");
        setHeight(200);
        setWidth(500);
        getContentPane().add(deviceButtons1, java.awt.BorderLayout.PAGE_END);

        deviceField1.setIdentifier("");
        deviceField1.setLabelString("Board Id:");
        deviceField1.setNumCols(2);
        deviceField1.setOffsetNid(10);
        jPanel1.add(deviceField1);

        getContentPane().add(jPanel1, java.awt.BorderLayout.PAGE_START);

        jPanel2.setLayout(new java.awt.GridLayout(2, 1));

        deviceChoice1.setChoiceItems(new String[] {"10", "5", "APFI0", "APFI1"});
        deviceChoice1.setIdentifier("");
        deviceChoice1.setLabelString("Range: ");
        deviceChoice1.setOffsetNid(45);
        deviceChoice1.setUpdateIdentifier("");
        jPanel3.add(deviceChoice1);

        jPanel2.add(jPanel3);

        deviceField4.setIdentifier("");
        deviceField4.setLabelString("Value: ");
        deviceField4.setNumCols(30);
        deviceField4.setOffsetNid(37);
        jPanel4.add(deviceField4);

        jPanel2.add(jPanel4);

        jTabbedPane1.addTab("DAC0", jPanel2);

        jPanel5.setLayout(new java.awt.GridLayout(2, 1));

        deviceChoice2.setChoiceItems(new String[] {"10", "5", "APFI0", "APFI1"});
        deviceChoice2.setIdentifier("");
        deviceChoice2.setLabelString("Range: ");
        deviceChoice2.setOffsetNid(62);
        deviceChoice2.setUpdateIdentifier("");
        jPanel6.add(deviceChoice2);

        jPanel5.add(jPanel6);

        deviceField5.setIdentifier("");
        deviceField5.setLabelString("Value: ");
        deviceField5.setNumCols(30);
        deviceField5.setOffsetNid(54);
        jPanel7.add(deviceField5);

        jPanel5.add(jPanel7);

        jTabbedPane1.addTab("DAC1", jPanel5);

        jPanel8.setLayout(new java.awt.GridLayout(2, 1));

        deviceChoice3.setChoiceItems(new String[] {"10", "5", "APFI0", "APFI1"});
        deviceChoice3.setIdentifier("");
        deviceChoice3.setLabelString("Range: ");
        deviceChoice3.setOffsetNid(76);
        deviceChoice3.setUpdateIdentifier("");
        jPanel9.add(deviceChoice3);

        jPanel8.add(jPanel9);

        deviceField6.setIdentifier("");
        deviceField6.setLabelString("Value: ");
        deviceField6.setNumCols(30);
        deviceField6.setOffsetNid(68);
        jPanel10.add(deviceField6);

        jPanel8.add(jPanel10);

        jTabbedPane1.addTab("DAC2", jPanel8);

        jPanel11.setLayout(new java.awt.GridLayout(2, 1));

        deviceChoice4.setChoiceItems(new String[] {"10", "5", "APFI0", "APFI1"});
        deviceChoice4.setIdentifier("");
        deviceChoice4.setLabelString("Range: ");
        deviceChoice4.setOffsetNid(90);
        deviceChoice4.setUpdateIdentifier("");
        jPanel12.add(deviceChoice4);

        jPanel11.add(jPanel12);

        deviceField7.setIdentifier("");
        deviceField7.setLabelString("Value: ");
        deviceField7.setNumCols(30);
        deviceField7.setOffsetNid(82);
        jPanel13.add(deviceField7);

        jPanel11.add(jPanel13);

        jTabbedPane1.addTab("DAC3", jPanel11);

        getContentPane().add(jTabbedPane1, java.awt.BorderLayout.CENTER);

        getAccessibleContext().setAccessibleName("");
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private DeviceButtons deviceButtons1;
    private DeviceChoice deviceChoice1;
    private DeviceChoice deviceChoice2;
    private DeviceChoice deviceChoice3;
    private DeviceChoice deviceChoice4;
    private DeviceField deviceField1;
    private DeviceField deviceField4;
    private DeviceField deviceField5;
    private DeviceField deviceField6;
    private DeviceField deviceField7;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel10;
    private javax.swing.JPanel jPanel11;
    private javax.swing.JPanel jPanel12;
    private javax.swing.JPanel jPanel13;
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

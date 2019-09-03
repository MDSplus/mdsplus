/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author manduchi
 */
public class MARTE2_STREAMSetup extends DeviceSetup {

    /**
     * Creates new form MARTE2_STREAMSetup
     */
    public MARTE2_STREAMSetup() {
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

        jPanel2 = new javax.swing.JPanel();
        jPanel11 = new javax.swing.JPanel();
        deviceField1 = new DeviceField();
        deviceChoice9 = new DeviceChoice();
        deviceField10 = new DeviceField();
        jPanel12 = new javax.swing.JPanel();
        deviceField11 = new DeviceField();
        deviceField12 = new DeviceField();
        deviceField13 = new DeviceField();
        deviceButtons1 = new DeviceButtons();
        jScrollPane1 = new javax.swing.JScrollPane();
        jPanel1 = new javax.swing.JPanel();
        jPanel3 = new javax.swing.JPanel();
        deviceField2 = new DeviceField();
        jPanel4 = new javax.swing.JPanel();
        deviceField3 = new DeviceField();
        jPanel5 = new javax.swing.JPanel();
        deviceField4 = new DeviceField();
        jPanel6 = new javax.swing.JPanel();
        deviceField5 = new DeviceField();
        jPanel7 = new javax.swing.JPanel();
        deviceField6 = new DeviceField();
        jPanel8 = new javax.swing.JPanel();
        deviceField7 = new DeviceField();
        jPanel9 = new javax.swing.JPanel();
        deviceField8 = new DeviceField();
        jPanel10 = new javax.swing.JPanel();
        deviceField9 = new DeviceField();

        setDeviceProvider("scdevail.rfx.local:8123");
        setDeviceTitle("MARTe2 Stream");
        setDeviceType("MARTE2_STREAM");
        setHeight(500);
        setWidth(700);

        jPanel2.setLayout(new java.awt.GridLayout(2, 1));

        deviceField1.setIdentifier("");
        deviceField1.setLabelString("Timebase");
        deviceField1.setNumCols(25);
        deviceField1.setOffsetNid(3);
        jPanel11.add(deviceField1);

        deviceChoice9.setChoiceIntValues(new int[] {1, 0});
        deviceChoice9.setChoiceItems(new String[] {"Time Streaming", "Oscilloscope"});
        deviceChoice9.setConvert(true);
        deviceChoice9.setIdentifier("");
        deviceChoice9.setLabelString("Mode: ");
        deviceChoice9.setOffsetNid(16);
        deviceChoice9.setUpdateIdentifier("");
        jPanel11.add(deviceChoice9);

        deviceField10.setIdentifier("");
        deviceField10.setLabelString("Event Division: ");
        deviceField10.setNumCols(4);
        deviceField10.setOffsetNid(7);
        jPanel11.add(deviceField10);

        jPanel2.add(jPanel11);

        deviceField11.setIdentifier("");
        deviceField11.setLabelString("CPU Mask: ");
        deviceField11.setOffsetNid(19);
        jPanel12.add(deviceField11);

        deviceField12.setIdentifier("");
        deviceField12.setLabelString("Stack size: ");
        deviceField12.setOffsetNid(22);
        jPanel12.add(deviceField12);

        deviceField13.setIdentifier("");
        deviceField13.setLabelString("Num. buffers: ");
        deviceField13.setOffsetNid(25);
        jPanel12.add(deviceField13);

        jPanel2.add(jPanel12);

        getContentPane().add(jPanel2, java.awt.BorderLayout.NORTH);
        jPanel2.getAccessibleContext().setAccessibleName("");

        getContentPane().add(deviceButtons1, java.awt.BorderLayout.PAGE_END);

        jPanel1.setLayout(new java.awt.GridLayout(8, 1));

        jPanel3.setBorder(javax.swing.BorderFactory.createTitledBorder("Signal 1(Time)"));

        deviceField2.setIdentifier("");
        deviceField2.setLabelString("Value: ");
        deviceField2.setNumCols(35);
        deviceField2.setOffsetNid(31);
        jPanel3.add(deviceField2);

        jPanel1.add(jPanel3);

        jPanel4.setBorder(javax.swing.BorderFactory.createTitledBorder("Signal 2"));

        deviceField3.setIdentifier("");
        deviceField3.setLabelString("Value: ");
        deviceField3.setNumCols(35);
        deviceField3.setOffsetNid(37);
        jPanel4.add(deviceField3);

        jPanel1.add(jPanel4);

        jPanel5.setBorder(javax.swing.BorderFactory.createTitledBorder("Signal 3"));

        deviceField4.setIdentifier("");
        deviceField4.setLabelString("Value: ");
        deviceField4.setNumCols(35);
        deviceField4.setOffsetNid(43);
        jPanel5.add(deviceField4);

        jPanel1.add(jPanel5);

        jPanel6.setBorder(javax.swing.BorderFactory.createTitledBorder("Signal 4"));

        deviceField5.setIdentifier("");
        deviceField5.setLabelString("Value: ");
        deviceField5.setNumCols(35);
        deviceField5.setOffsetNid(49);
        jPanel6.add(deviceField5);

        jPanel1.add(jPanel6);

        jPanel7.setBorder(javax.swing.BorderFactory.createTitledBorder("Signal 5"));

        deviceField6.setIdentifier("");
        deviceField6.setLabelString("Value: ");
        deviceField6.setNumCols(35);
        deviceField6.setOffsetNid(55);
        jPanel7.add(deviceField6);

        jPanel1.add(jPanel7);

        jPanel8.setBorder(javax.swing.BorderFactory.createTitledBorder("Signal 6"));

        deviceField7.setIdentifier("");
        deviceField7.setLabelString("Value: ");
        deviceField7.setNumCols(35);
        deviceField7.setOffsetNid(61);
        jPanel8.add(deviceField7);

        jPanel1.add(jPanel8);

        jPanel9.setBorder(javax.swing.BorderFactory.createTitledBorder("Signal 7"));

        deviceField8.setIdentifier("");
        deviceField8.setLabelString("Value: ");
        deviceField8.setNumCols(35);
        deviceField8.setOffsetNid(67);
        jPanel9.add(deviceField8);

        jPanel1.add(jPanel9);

        jPanel10.setBorder(javax.swing.BorderFactory.createTitledBorder("Signal 8"));

        deviceField9.setIdentifier("");
        deviceField9.setLabelString("Value: ");
        deviceField9.setNumCols(35);
        deviceField9.setOffsetNid(73);
        jPanel10.add(deviceField9);

        jPanel1.add(jPanel10);

        jScrollPane1.setViewportView(jPanel1);

        getContentPane().add(jScrollPane1, java.awt.BorderLayout.CENTER);

        getAccessibleContext().setAccessibleName("");
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private DeviceButtons deviceButtons1;
    private DeviceChoice deviceChoice9;
    private DeviceField deviceField1;
    private DeviceField deviceField10;
    private DeviceField deviceField11;
    private DeviceField deviceField12;
    private DeviceField deviceField13;
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
    private javax.swing.JScrollPane jScrollPane1;
    // End of variables declaration//GEN-END:variables
}

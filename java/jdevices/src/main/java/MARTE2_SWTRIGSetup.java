/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author mdsplus
 */
public class MARTE2_SWTRIGSetup extends DeviceSetup {

    /**
     * Creates new form MARTE2_SFTRIGSetup
     */
    public MARTE2_SWTRIGSetup() {
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
        jPanel1 = new javax.swing.JPanel();
        deviceField1 = new DeviceField();
        deviceField2 = new DeviceField();
        deviceField3 = new DeviceField();
        jPanel3 = new javax.swing.JPanel();
        deviceField4 = new DeviceField();
        deviceField5 = new DeviceField();
        jScrollPane1 = new javax.swing.JScrollPane();
        jPanel4 = new javax.swing.JPanel();
        jPanel5 = new javax.swing.JPanel();
        deviceField6 = new DeviceField();
        deviceField7 = new DeviceField();
        deviceField8 = new DeviceField();
        deviceField30 = new DeviceField();
        jPanel6 = new javax.swing.JPanel();
        deviceField9 = new DeviceField();
        deviceField10 = new DeviceField();
        deviceField11 = new DeviceField();
        deviceField31 = new DeviceField();
        jPanel7 = new javax.swing.JPanel();
        deviceField12 = new DeviceField();
        deviceField13 = new DeviceField();
        deviceField14 = new DeviceField();
        deviceField32 = new DeviceField();
        jPanel8 = new javax.swing.JPanel();
        deviceField15 = new DeviceField();
        deviceField16 = new DeviceField();
        deviceField17 = new DeviceField();
        deviceField33 = new DeviceField();
        jPanel9 = new javax.swing.JPanel();
        deviceField18 = new DeviceField();
        deviceField19 = new DeviceField();
        deviceField20 = new DeviceField();
        deviceField34 = new DeviceField();
        jPanel10 = new javax.swing.JPanel();
        deviceField21 = new DeviceField();
        deviceField22 = new DeviceField();
        deviceField23 = new DeviceField();
        deviceField35 = new DeviceField();
        jPanel11 = new javax.swing.JPanel();
        deviceField24 = new DeviceField();
        deviceField25 = new DeviceField();
        deviceField26 = new DeviceField();
        deviceField36 = new DeviceField();
        jPanel12 = new javax.swing.JPanel();
        deviceField27 = new DeviceField();
        deviceField28 = new DeviceField();
        deviceField29 = new DeviceField();
        deviceField37 = new DeviceField();
        deviceButtons1 = new DeviceButtons();

        setDeviceProvider("spilds:8100");
        setDeviceTitle("MARTe2 Software Clock/Trigger");
        setDeviceType("MARTE2_SWTRIG");
        setHeight(300);
        setUpdateEvent("");
        setWidth(800);

        jPanel2.setLayout(new java.awt.GridLayout(2, 1));

        deviceField1.setIdentifier("");
        deviceField1.setLabelString("Start Event:");
        deviceField1.setOffsetNid(7);
        deviceField1.setTextOnly(true);
        jPanel1.add(deviceField1);

        deviceField2.setIdentifier("");
        deviceField2.setLabelString("Frequency");
        deviceField2.setOffsetNid(10);
        jPanel1.add(deviceField2);

        deviceField3.setIdentifier("");
        deviceField3.setLabelString("StartTime: ");
        deviceField3.setOffsetNid(13);
        jPanel1.add(deviceField3);

        jPanel2.add(jPanel1);

        deviceField4.setIdentifier("");
        deviceField4.setLabelString("CpuMask");
        deviceField4.setNumCols(6);
        deviceField4.setOffsetNid(16);
        jPanel3.add(deviceField4);

        deviceField5.setIdentifier("");
        deviceField5.setLabelString("StackSize: ");
        deviceField5.setNumCols(6);
        deviceField5.setOffsetNid(19);
        jPanel3.add(deviceField5);

        jPanel2.add(jPanel3);

        getContentPane().add(jPanel2, java.awt.BorderLayout.NORTH);

        jPanel4.setLayout(new java.awt.GridLayout(8, 1));

        jPanel5.setBorder(javax.swing.BorderFactory.createTitledBorder("Trigger 1"));

        deviceField6.setIdentifier("");
        deviceField6.setLabelString("Trigger Event:");
        deviceField6.setOffsetNid(56);
        deviceField6.setTextOnly(true);
        jPanel5.add(deviceField6);

        deviceField7.setIdentifier("");
        deviceField7.setLabelString("Wait cycles: ");
        deviceField7.setNumCols(6);
        deviceField7.setOffsetNid(59);
        jPanel5.add(deviceField7);

        deviceField8.setIdentifier("");
        deviceField8.setLabelString("Trigger cycles:");
        deviceField8.setNumCols(6);
        deviceField8.setOffsetNid(62);
        jPanel5.add(deviceField8);

        deviceField30.setIdentifier("");
        deviceField30.setLabelString("Segment len.:");
        deviceField30.setNumCols(4);
        deviceField30.setOffsetNid(50);
        jPanel5.add(deviceField30);

        jPanel4.add(jPanel5);

        jPanel6.setBorder(javax.swing.BorderFactory.createTitledBorder("Trigger 2"));

        deviceField9.setIdentifier("");
        deviceField9.setLabelString("Trigger Event:");
        deviceField9.setOffsetNid(74);
        deviceField9.setTextOnly(true);
        jPanel6.add(deviceField9);

        deviceField10.setIdentifier("");
        deviceField10.setLabelString("Wait cycles: ");
        deviceField10.setNumCols(6);
        deviceField10.setOffsetNid(77);
        jPanel6.add(deviceField10);

        deviceField11.setIdentifier("");
        deviceField11.setLabelString("Trigger cycles:");
        deviceField11.setNumCols(6);
        deviceField11.setOffsetNid(80);
        jPanel6.add(deviceField11);

        deviceField31.setIdentifier("");
        deviceField31.setLabelString("Segment len.:");
        deviceField31.setNumCols(4);
        deviceField31.setOffsetNid(68);
        jPanel6.add(deviceField31);

        jPanel4.add(jPanel6);

        jPanel7.setBorder(javax.swing.BorderFactory.createTitledBorder("Trigger 3"));

        deviceField12.setIdentifier("");
        deviceField12.setLabelString("Trigger Event:");
        deviceField12.setOffsetNid(92);
        deviceField12.setTextOnly(true);
        jPanel7.add(deviceField12);

        deviceField13.setIdentifier("");
        deviceField13.setLabelString("Wait cycles: ");
        deviceField13.setNumCols(6);
        deviceField13.setOffsetNid(95);
        jPanel7.add(deviceField13);

        deviceField14.setIdentifier("");
        deviceField14.setLabelString("Trigger cycles:");
        deviceField14.setNumCols(6);
        deviceField14.setOffsetNid(98);
        jPanel7.add(deviceField14);

        deviceField32.setIdentifier("");
        deviceField32.setLabelString("Segment len.:");
        deviceField32.setNumCols(4);
        deviceField32.setOffsetNid(86);
        jPanel7.add(deviceField32);

        jPanel4.add(jPanel7);

        jPanel8.setBorder(javax.swing.BorderFactory.createTitledBorder("Trigger 4"));

        deviceField15.setIdentifier("");
        deviceField15.setLabelString("Trigger Event:");
        deviceField15.setOffsetNid(110);
        deviceField15.setTextOnly(true);
        jPanel8.add(deviceField15);

        deviceField16.setIdentifier("");
        deviceField16.setLabelString("Wait cycles: ");
        deviceField16.setNumCols(6);
        deviceField16.setOffsetNid(113);
        jPanel8.add(deviceField16);

        deviceField17.setIdentifier("");
        deviceField17.setLabelString("Trigger cycles:");
        deviceField17.setNumCols(6);
        deviceField17.setOffsetNid(116);
        jPanel8.add(deviceField17);

        deviceField33.setIdentifier("");
        deviceField33.setLabelString("Segment len.:");
        deviceField33.setNumCols(4);
        deviceField33.setOffsetNid(104);
        jPanel8.add(deviceField33);

        jPanel4.add(jPanel8);

        jPanel9.setBorder(javax.swing.BorderFactory.createTitledBorder("Trigger 5"));

        deviceField18.setIdentifier("");
        deviceField18.setLabelString("Trigger Event:");
        deviceField18.setOffsetNid(128);
        deviceField18.setTextOnly(true);
        jPanel9.add(deviceField18);

        deviceField19.setIdentifier("");
        deviceField19.setLabelString("Wait cycles: ");
        deviceField19.setNumCols(6);
        deviceField19.setOffsetNid(131);
        jPanel9.add(deviceField19);

        deviceField20.setIdentifier("");
        deviceField20.setLabelString("Trigger cycles:");
        deviceField20.setNumCols(6);
        deviceField20.setOffsetNid(134);
        jPanel9.add(deviceField20);

        deviceField34.setIdentifier("");
        deviceField34.setLabelString("Segment len.:");
        deviceField34.setNumCols(4);
        deviceField34.setOffsetNid(122);
        jPanel9.add(deviceField34);

        jPanel4.add(jPanel9);

        jPanel10.setBorder(javax.swing.BorderFactory.createTitledBorder("Trigger 6"));

        deviceField21.setIdentifier("");
        deviceField21.setLabelString("Trigger Event:");
        deviceField21.setOffsetNid(146);
        deviceField21.setTextOnly(true);
        jPanel10.add(deviceField21);

        deviceField22.setIdentifier("");
        deviceField22.setLabelString("Wait cycles: ");
        deviceField22.setNumCols(6);
        deviceField22.setOffsetNid(149);
        jPanel10.add(deviceField22);

        deviceField23.setIdentifier("");
        deviceField23.setLabelString("Trigger cycles:");
        deviceField23.setNumCols(6);
        deviceField23.setOffsetNid(152);
        jPanel10.add(deviceField23);

        deviceField35.setIdentifier("");
        deviceField35.setLabelString("Segment len.:");
        deviceField35.setNumCols(4);
        deviceField35.setOffsetNid(140);
        jPanel10.add(deviceField35);

        jPanel4.add(jPanel10);

        jPanel11.setBorder(javax.swing.BorderFactory.createTitledBorder("Trigger 7"));

        deviceField24.setIdentifier("");
        deviceField24.setLabelString("Trigger Event:");
        deviceField24.setOffsetNid(164);
        deviceField24.setTextOnly(true);
        jPanel11.add(deviceField24);

        deviceField25.setIdentifier("");
        deviceField25.setLabelString("Wait cycles: ");
        deviceField25.setNumCols(6);
        deviceField25.setOffsetNid(167);
        jPanel11.add(deviceField25);

        deviceField26.setIdentifier("");
        deviceField26.setLabelString("Trigger cycles:");
        deviceField26.setNumCols(6);
        deviceField26.setOffsetNid(170);
        jPanel11.add(deviceField26);

        deviceField36.setIdentifier("");
        deviceField36.setLabelString("Segment len.:");
        deviceField36.setNumCols(4);
        deviceField36.setOffsetNid(158);
        jPanel11.add(deviceField36);

        jPanel4.add(jPanel11);

        jPanel12.setBorder(javax.swing.BorderFactory.createTitledBorder("Trigger 8"));

        deviceField27.setIdentifier("");
        deviceField27.setLabelString("Trigger Event:");
        deviceField27.setOffsetNid(182);
        deviceField27.setTextOnly(true);
        jPanel12.add(deviceField27);

        deviceField28.setIdentifier("");
        deviceField28.setLabelString("Wait cycles: ");
        deviceField28.setNumCols(6);
        deviceField28.setOffsetNid(185);
        jPanel12.add(deviceField28);

        deviceField29.setIdentifier("");
        deviceField29.setLabelString("Trigger cycles:");
        deviceField29.setNumCols(6);
        deviceField29.setOffsetNid(188);
        jPanel12.add(deviceField29);

        deviceField37.setIdentifier("");
        deviceField37.setLabelString("Segment len.:");
        deviceField37.setNumCols(4);
        deviceField37.setOffsetNid(176);
        jPanel12.add(deviceField37);

        jPanel4.add(jPanel12);

        jScrollPane1.setViewportView(jPanel4);

        getContentPane().add(jScrollPane1, java.awt.BorderLayout.CENTER);
        getContentPane().add(deviceButtons1, java.awt.BorderLayout.PAGE_END);
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private DeviceButtons deviceButtons1;
    private DeviceField deviceField1;
    private DeviceField deviceField10;
    private DeviceField deviceField11;
    private DeviceField deviceField12;
    private DeviceField deviceField13;
    private DeviceField deviceField14;
    private DeviceField deviceField15;
    private DeviceField deviceField16;
    private DeviceField deviceField17;
    private DeviceField deviceField18;
    private DeviceField deviceField19;
    private DeviceField deviceField2;
    private DeviceField deviceField20;
    private DeviceField deviceField21;
    private DeviceField deviceField22;
    private DeviceField deviceField23;
    private DeviceField deviceField24;
    private DeviceField deviceField25;
    private DeviceField deviceField26;
    private DeviceField deviceField27;
    private DeviceField deviceField28;
    private DeviceField deviceField29;
    private DeviceField deviceField3;
    private DeviceField deviceField30;
    private DeviceField deviceField31;
    private DeviceField deviceField32;
    private DeviceField deviceField33;
    private DeviceField deviceField34;
    private DeviceField deviceField35;
    private DeviceField deviceField36;
    private DeviceField deviceField37;
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

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author mdsplus
 */
public class MARTE2_SIMULINK_RABBITSetup extends DeviceSetup {

    /**
     * Creates new form MARTE2_SIMULINK_RABBITSetup
     */
    public MARTE2_SIMULINK_RABBITSetup() {
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
        jPanel2 = new javax.swing.JPanel();
        deviceField1 = new DeviceField();
        jTabbedPane1 = new javax.swing.JTabbedPane();
        deviceInputs1 = new DeviceInputs();
        deviceOutputs1 = new DeviceOutputs();

        setDeviceProvider("spilds:8100");
        setDeviceTitle("Rabbit Setup");
        setDeviceType("MARTE2_SIMULINK_RABBIT");
        setHeight(400);
        setUpdateEvent("");
        setWidth(500);
        getContentPane().add(deviceButtons1, java.awt.BorderLayout.PAGE_END);

        deviceField1.setIdentifier("");
        deviceField1.setLabelString("Timebase: ");
        deviceField1.setOffsetNid(3);
        jPanel2.add(deviceField1);

        getContentPane().add(jPanel2, java.awt.BorderLayout.PAGE_START);

        deviceInputs1.setOffsetNid(32);
        jTabbedPane1.addTab("Inputs", deviceInputs1);

        deviceOutputs1.setOffsetNid(353);
        jTabbedPane1.addTab("Outputs", deviceOutputs1);

        getContentPane().add(jTabbedPane1, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private DeviceButtons deviceButtons1;
    private DeviceField deviceField1;
    private DeviceInputs deviceInputs1;
    private DeviceOutputs deviceOutputs1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JTabbedPane jTabbedPane1;
    // End of variables declaration//GEN-END:variables
}

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author mdsplus
 */
public class MARTE2_SIMULINK_DTT_VSSetup extends DeviceSetup {

    /**
     * Creates new form MARTE2_SIMULINK_DTT_VSSetup
     */
    public MARTE2_SIMULINK_DTT_VSSetup() {
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
        deviceField3 = new DeviceField();
        deviceField1 = new DeviceField();
        jTabbedPane1 = new javax.swing.JTabbedPane();
        deviceInputs1 = new DeviceInputs();
        deviceOutputs1 = new DeviceOutputs();

        setDeviceProvider("localhost:8100");
        setDeviceTitle("DTT VS Control");
        setDeviceType("MARTE2_SIMULINK_DTT_VS");
        setHeight(400);
        setWidth(800);
        getContentPane().add(deviceButtons1, java.awt.BorderLayout.PAGE_END);

        deviceField3.setIdentifier("");
        deviceField3.setLabelString("Write CPU Mask: ");
        deviceField3.setNumCols(6);
        deviceField3.setOffsetNid(44);
        jPanel1.add(deviceField3);

        deviceField1.setIdentifier("");
        deviceField1.setLabelString("Port");
        deviceField1.setNumCols(6);
        deviceField1.setOffsetNid(62);
        jPanel1.add(deviceField1);

        getContentPane().add(jPanel1, java.awt.BorderLayout.PAGE_START);

        deviceInputs1.setOffsetNid(17);
        jTabbedPane1.addTab("Inputs", deviceInputs1);
        jTabbedPane1.addTab("Outputs", deviceOutputs1);

        getContentPane().add(jTabbedPane1, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private DeviceButtons deviceButtons1;
    private DeviceField deviceField1;
    private DeviceField deviceField3;
    private DeviceInputs deviceInputs1;
    private DeviceOutputs deviceOutputs1;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JTabbedPane jTabbedPane1;
    // End of variables declaration//GEN-END:variables
}

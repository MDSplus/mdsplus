/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/**
 *
 * @author manduchi
 */
public class MARTE2_EXPRSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;

	/**
	 * Creates new form MARTE2_EXPRSetup
	 */
	public MARTE2_EXPRSetup()
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
        jTabbedPane1 = new javax.swing.JTabbedPane();
        jScrollPane1 = new javax.swing.JScrollPane();
        deviceInputs1 = new DeviceInputs();
        jScrollPane2 = new javax.swing.JScrollPane();
        deviceOutputs1 = new DeviceOutputs();
        jPanel50 = new javax.swing.JPanel();
        deviceField42 = new DeviceField();

        setDeviceProvider("localhost:8100");
        setDeviceType("MARTE2_EXPR");
        setHeight(400);
        setWidth(900);
        getContentPane().add(deviceButtons1, java.awt.BorderLayout.PAGE_END);

        jPanel1.setLayout(new java.awt.BorderLayout());

        deviceInputs1.setOffsetNid(29);
        jScrollPane1.setViewportView(deviceInputs1);

        jTabbedPane1.addTab("Inputs", jScrollPane1);

        deviceOutputs1.setOffsetNid(94);
        jScrollPane2.setViewportView(deviceOutputs1);

        jTabbedPane1.addTab("Outputs", jScrollPane2);

        jPanel1.add(jTabbedPane1, java.awt.BorderLayout.CENTER);

        getContentPane().add(jPanel1, java.awt.BorderLayout.CENTER);

        deviceField42.setIdentifier("");
        deviceField42.setLabelString("Storage CPUs: ");
        deviceField42.setNumCols(4);
        deviceField42.setOffsetNid(92);
        jPanel50.add(deviceField42);

        getContentPane().add(jPanel50, java.awt.BorderLayout.PAGE_START);

        getAccessibleContext().setAccessibleName("");
    }// </editor-fold>//GEN-END:initComponents

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private DeviceButtons deviceButtons1;
    private DeviceField deviceField42;
    private DeviceInputs deviceInputs1;
    private DeviceOutputs deviceOutputs1;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel50;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JTabbedPane jTabbedPane1;
    // End of variables declaration//GEN-END:variables
}

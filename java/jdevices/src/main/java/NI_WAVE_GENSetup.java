/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/**
 *
 * @author taliercio
 */
public class NI_WAVE_GENSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;

	/**
	 * Creates new form NI_WAVE_GENSetup
	 */
	public NI_WAVE_GENSetup()
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
	// Code">//GEN-BEGIN:initComponents
	private void initComponents()
	{
		jPanel1 = new javax.swing.JPanel();
		jPanel2 = new javax.swing.JPanel();
		deviceField1 = new DeviceField();
		deviceChoice1 = new DeviceChoice();
		deviceField9 = new DeviceField();
		deviceDispatch1 = new DeviceDispatch();
		jPanel4 = new javax.swing.JPanel();
		deviceChoice3 = new DeviceChoice();
		deviceField4 = new DeviceField();
		deviceButtons1 = new DeviceButtons();
		jPanel5 = new javax.swing.JPanel();
		jPanel6 = new javax.swing.JPanel();
		deviceChannel1 = new DeviceChannel();
		deviceChoice4 = new DeviceChoice();
		deviceField5 = new DeviceField();
		deviceField6 = new DeviceField();
		deviceField7 = new DeviceField();
		deviceField8 = new DeviceField();
		deviceField22 = new DeviceField();
		jPanel7 = new javax.swing.JPanel();
		deviceChannel2 = new DeviceChannel();
		deviceChoice5 = new DeviceChoice();
		deviceField10 = new DeviceField();
		deviceField11 = new DeviceField();
		deviceField12 = new DeviceField();
		deviceField13 = new DeviceField();
		deviceField23 = new DeviceField();
		jPanel8 = new javax.swing.JPanel();
		deviceChannel3 = new DeviceChannel();
		deviceChoice6 = new DeviceChoice();
		deviceField14 = new DeviceField();
		deviceField15 = new DeviceField();
		deviceField16 = new DeviceField();
		deviceField17 = new DeviceField();
		deviceField24 = new DeviceField();
		jPanel9 = new javax.swing.JPanel();
		deviceChannel4 = new DeviceChannel();
		deviceChoice7 = new DeviceChoice();
		deviceField18 = new DeviceField();
		deviceField19 = new DeviceField();
		deviceField20 = new DeviceField();
		deviceField21 = new DeviceField();
		deviceField25 = new DeviceField();
		setDeviceProvider("sdepl.nbtf");
		setDeviceTitle("NI PXI-6259 / 6368 Analog Output Waveform Generation");
		setDeviceType("NI_WAVE_GEN");
		setHeight(400);
		setWidth(1100);
		jPanel1.setLayout(new java.awt.GridLayout(2, 0));
		deviceField1.setIdentifier("");
		deviceField1.setLabelString("Comment:");
		deviceField1.setNumCols(25);
		deviceField1.setOffsetNid(1);
		jPanel2.add(deviceField1);
		deviceChoice1.setChoiceItems(new String[]
		{ "NI6259", "NI6368" });
		deviceChoice1.setIdentifier("");
		deviceChoice1.setLabelString("Board Type:");
		deviceChoice1.setOffsetNid(3);
		deviceChoice1.setUpdateIdentifier("");
		jPanel2.add(deviceChoice1);
		deviceField9.setIdentifier("");
		deviceField9.setLabelString("Board Id:");
		deviceField9.setOffsetNid(2);
		jPanel2.add(deviceField9);
		jPanel2.add(deviceDispatch1);
		jPanel1.add(jPanel2);
		deviceChoice3.setChoiceItems(new String[]
		{ "INTERNAL", "EXTERNAL" });
		deviceChoice3.setIdentifier("");
		deviceChoice3.setLabelString("Trig. Mode:");
		deviceChoice3.setOffsetNid(5);
		deviceChoice3.setUpdateIdentifier("");
		jPanel4.add(deviceChoice3);
		deviceField4.setIdentifier("");
		deviceField4.setLabelString("Trig. Source:");
		deviceField4.setNumCols(40);
		deviceField4.setOffsetNid(6);
		jPanel4.add(deviceField4);
		jPanel1.add(jPanel4);
		getContentPane().add(jPanel1, java.awt.BorderLayout.PAGE_START);
		deviceButtons1.setCheckExpressions(new String[] {});
		deviceButtons1.setCheckMessages(new String[] {});
		deviceButtons1.setMethods(new String[]
		{ "init", "start_gen", "stop_gen", "pause_gen", "resume_gen", "update_gen"});
		getContentPane().add(deviceButtons1, java.awt.BorderLayout.PAGE_END);
		jPanel5.setLayout(new java.awt.GridLayout(4, 0));
		jPanel6.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
		deviceChannel1.setInSameLine(true);
		deviceChannel1.setLabelString("AO1");
		deviceChannel1.setLines(0);
		deviceChannel1.setOffsetNid(8);
		deviceChannel1.setShowVal("");
		deviceChannel1.setUpdateIdentifier("");
		deviceChoice4.setChoiceItems(new String[]
		{ "AS_IS", "SIN", "COS", "TRIANGULAR", "SQUARE", "SAWTOOTH" });
		deviceChoice4.setIdentifier("");
		deviceChoice4.setLabelString("Wave Type:");
		deviceChoice4.setOffsetNid(13);
		deviceChoice4.setUpdateIdentifier("");
		deviceChannel1.getContainer().add(deviceChoice4);
		deviceField5.setIdentifier("");
		deviceField5.setLabelString("X:");
		deviceField5.setOffsetNid(11);
		deviceChannel1.getContainer().add(deviceField5);
		deviceField6.setIdentifier("");
		deviceField6.setLabelString("Y:");
		deviceField6.setOffsetNid(12);
		deviceChannel1.getContainer().add(deviceField6);
		deviceField7.setIdentifier("");
		deviceField7.setLabelString("Min Y:");
		deviceField7.setOffsetNid(10);
		deviceChannel1.getContainer().add(deviceField7);
		deviceField8.setIdentifier("");
		deviceField8.setLabelString("Max Y:");
		deviceField8.setOffsetNid(9);
		deviceChannel1.getContainer().add(deviceField8);
		deviceField22.setIdentifier("");
		deviceField22.setLabelString("Freq.:");
		deviceField22.setOffsetNid(14);
		deviceChannel1.getContainer().add(deviceField22);
		jPanel6.add(deviceChannel1);
		jPanel5.add(jPanel6);
		jPanel7.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
		deviceChannel2.setInSameLine(true);
		deviceChannel2.setLabelString("AO2");
		deviceChannel2.setLines(0);
		deviceChannel2.setOffsetNid(15);
		deviceChannel2.setShowVal("");
		deviceChannel2.setUpdateIdentifier("");
		deviceChoice5.setChoiceItems(new String[]
		{ "AS_IS", "SIN", "COS", "TRIANGULAR", "SQUARE", "SAWTOOTH" });
		deviceChoice5.setIdentifier("");
		deviceChoice5.setLabelString("Wave Type:");
		deviceChoice5.setOffsetNid(20);
		deviceChoice5.setUpdateIdentifier("");
		deviceChannel2.getContainer().add(deviceChoice5);
		deviceField10.setIdentifier("");
		deviceField10.setLabelString("X:");
		deviceField10.setOffsetNid(18);
		deviceChannel2.getContainer().add(deviceField10);
		deviceField11.setIdentifier("");
		deviceField11.setLabelString("Y:");
		deviceField11.setOffsetNid(19);
		deviceChannel2.getContainer().add(deviceField11);
		deviceField12.setIdentifier("");
		deviceField12.setLabelString("Min Y:");
		deviceField12.setOffsetNid(17);
		deviceChannel2.getContainer().add(deviceField12);
		deviceField13.setIdentifier("");
		deviceField13.setLabelString("Max Y:");
		deviceField13.setOffsetNid(16);
		deviceChannel2.getContainer().add(deviceField13);
		deviceField23.setIdentifier("");
		deviceField23.setLabelString("Freq.:");
		deviceField23.setOffsetNid(21);
		deviceChannel2.getContainer().add(deviceField23);
		jPanel7.add(deviceChannel2);
		jPanel5.add(jPanel7);
		jPanel8.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
		deviceChannel3.setInSameLine(true);
		deviceChannel3.setLabelString("AO3");
		deviceChannel3.setLines(0);
		deviceChannel3.setOffsetNid(22);
		deviceChannel3.setShowVal("");
		deviceChannel3.setUpdateIdentifier("");
		deviceChoice6.setChoiceItems(new String[]
		{ "AS_IS", "SIN", "COS", "TRIANGULAR", "SQUARE", "SAWTOOTH" });
		deviceChoice6.setIdentifier("");
		deviceChoice6.setLabelString("Wave Type:");
		deviceChoice6.setOffsetNid(27);
		deviceChoice6.setUpdateIdentifier("");
		deviceChannel3.getContainer().add(deviceChoice6);
		deviceField14.setIdentifier("");
		deviceField14.setLabelString("X:");
		deviceField14.setOffsetNid(25);
		deviceChannel3.getContainer().add(deviceField14);
		deviceField15.setIdentifier("");
		deviceField15.setLabelString("Y:");
		deviceField15.setOffsetNid(26);
		deviceChannel3.getContainer().add(deviceField15);
		deviceField16.setIdentifier("");
		deviceField16.setLabelString("Min Y:");
		deviceField16.setOffsetNid(24);
		deviceChannel3.getContainer().add(deviceField16);
		deviceField17.setIdentifier("");
		deviceField17.setLabelString("Max Y:");
		deviceField17.setOffsetNid(23);
		deviceChannel3.getContainer().add(deviceField17);
		deviceField24.setIdentifier("");
		deviceField24.setLabelString("Freq.:");
		deviceField24.setOffsetNid(28);
		deviceChannel3.getContainer().add(deviceField24);
		jPanel8.add(deviceChannel3);
		jPanel5.add(jPanel8);
		jPanel9.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
		deviceChannel4.setInSameLine(true);
		deviceChannel4.setLabelString("AO4");
		deviceChannel4.setLines(0);
		deviceChannel4.setOffsetNid(29);
		deviceChannel4.setShowVal("");
		deviceChannel4.setUpdateIdentifier("");
		deviceChoice7.setChoiceItems(new String[]
		{ "AS_IS", "SIN", "COS", "TRIANGULAR", "SQUARE", "SAWTOOTH" });
		deviceChoice7.setIdentifier("");
		deviceChoice7.setLabelString("Wave Type:");
		deviceChoice7.setOffsetNid(34);
		deviceChoice7.setUpdateIdentifier("");
		deviceChannel4.getContainer().add(deviceChoice7);
		deviceField18.setIdentifier("");
		deviceField18.setLabelString("X:");
		deviceField18.setOffsetNid(32);
		deviceChannel4.getContainer().add(deviceField18);
		deviceField19.setIdentifier("");
		deviceField19.setLabelString("Y:");
		deviceField19.setOffsetNid(33);
		deviceChannel4.getContainer().add(deviceField19);
		deviceField20.setIdentifier("");
		deviceField20.setLabelString("Min Y:");
		deviceField20.setOffsetNid(31);
		deviceChannel4.getContainer().add(deviceField20);
		deviceField21.setIdentifier("");
		deviceField21.setLabelString("Max Y:");
		deviceField21.setOffsetNid(30);
		deviceChannel4.getContainer().add(deviceField21);
		deviceField25.setIdentifier("");
		deviceField25.setLabelString("Freq.:");
		deviceField25.setOffsetNid(35);
		deviceChannel4.getContainer().add(deviceField25);
		jPanel9.add(deviceChannel4);
		jPanel5.add(jPanel9);
		getContentPane().add(jPanel5, java.awt.BorderLayout.CENTER);
	}// </editor-fold>//GEN-END:initComponents

	// Variables declaration - do not modify//GEN-BEGIN:variables
	private DeviceButtons deviceButtons1;
	private DeviceChannel deviceChannel1;
	private DeviceChannel deviceChannel2;
	private DeviceChannel deviceChannel3;
	private DeviceChannel deviceChannel4;
	private DeviceChoice deviceChoice1;
	private DeviceChoice deviceChoice3;
	private DeviceChoice deviceChoice4;
	private DeviceChoice deviceChoice5;
	private DeviceChoice deviceChoice6;
	private DeviceChoice deviceChoice7;
	private DeviceDispatch deviceDispatch1;
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
	private DeviceField deviceField20;
	private DeviceField deviceField21;
	private DeviceField deviceField22;
	private DeviceField deviceField23;
	private DeviceField deviceField24;
	private DeviceField deviceField25;
	private DeviceField deviceField4;
	private DeviceField deviceField5;
	private DeviceField deviceField6;
	private DeviceField deviceField7;
	private DeviceField deviceField8;
	private DeviceField deviceField9;
	private javax.swing.JPanel jPanel1;
	private javax.swing.JPanel jPanel2;
	private javax.swing.JPanel jPanel4;
	private javax.swing.JPanel jPanel5;
	private javax.swing.JPanel jPanel6;
	private javax.swing.JPanel jPanel7;
	private javax.swing.JPanel jPanel8;
	private javax.swing.JPanel jPanel9;
	// End of variables declaration//GEN-END:variables
}

import java.awt.*;
import javax.swing.*;

/**
 * <p>
 * Title:
 * </p>
 * <p>
 * Description:
 * </p>
 * <p>
 * Copyright: Copyright (c) 2003
 * </p>
 * <p>
 * Company:
 * </p>
 *
 * @author not attributable
 * @version 1.0
 */
public class RFXToroidalSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceField deviceField1 = new DeviceField();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JTabbedPane jTabbedPane1 = new JTabbedPane();
	JPanel jPanel1 = new JPanel();
	JPanel jPanel4 = new JPanel();
	JPanel jPanel5 = new JPanel();
	BorderLayout borderLayout2 = new BorderLayout();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel6 = new JPanel();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	JScrollPane jScrollPane1 = new JScrollPane();
	JScrollPane jScrollPane2 = new JScrollPane();
	DeviceTable deviceTable1 = new DeviceTable();
	GridLayout gridLayout2 = new GridLayout();
	DeviceTable deviceTable2 = new DeviceTable();
	JPanel jPanel10 = new JPanel();
	JPanel jPanel11 = new JPanel();
	JPanel jPanel12 = new JPanel();
	BorderLayout borderLayout5 = new BorderLayout();
	DeviceTable deviceTable5 = new DeviceTable();
	JPanel jPanel13 = new JPanel();
	DeviceField deviceField3 = new DeviceField();
	JScrollPane jScrollPane4 = new JScrollPane();
	DeviceTable deviceTable6 = new DeviceTable();
	BorderLayout borderLayout6 = new BorderLayout();
	DeviceTable deviceTable7 = new DeviceTable();
	DeviceField deviceField4 = new DeviceField();
	JPanel jPanel14 = new JPanel();
	JScrollPane jScrollPane5 = new JScrollPane();
	DeviceTable deviceTable8 = new DeviceTable();
	GridLayout gridLayout3 = new GridLayout();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	JPanel jPanel2 = new JPanel();
	DeviceField deviceField2 = new DeviceField();
	DeviceChoice deviceChoice5 = new DeviceChoice();
	DeviceChoice deviceChoice7 = new DeviceChoice();

	public RFXToroidalSetup()
	{
		try
		{
			jbInit();
		}
		catch (final Exception e)
		{
			e.printStackTrace();
		}
	}

	private void jbInit() throws Exception
	{
		this.setWidth(670);
		this.setHeight(652);
		this.setDeviceType("RFXToroidal");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("RFX Toroidal Parameter Setting");
		this.getContentPane().setLayout(borderLayout1);
		deviceField1.setOffsetNid(1);
		deviceField1.setLabelString("Comment: ");
		deviceField1.setNumCols(35);
		deviceField1.setIdentifier("");
		jPanel1.setLayout(borderLayout2);
		jPanel5.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(1);
		deviceChoice2.setChoiceIntValues(null);
		deviceChoice2.setChoiceFloatValues(null);
		deviceChoice2.setOffsetNid(3);
		deviceChoice2.setLabelString("Configuration :");
		deviceChoice2.setChoiceItems(new String[]
		{ "ADVANCED", "CLASSIC", "PASSIVE CB", "TOKAMAK" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		deviceTable1.setOffsetNid(8);
		deviceTable1.setLabelString("");
		deviceTable1.setNumCols(6);
		deviceTable1.setNumRows(11);
		deviceTable1.setIdentifier("");
		deviceTable1.setColumnNames(new String[]
		{ "Sect.1", "Sect.2", "Sect.3", "Sect.4", "Sect.5", "Sect.6" });
		deviceTable1.setRowNames(new String[]
		{ "ls_ch_init_state", "ch2_lockt_ls", "irelease_ls", "hs_ch_init_state", "ch1_lockt_hs", "irelease_hs",
				"ith_inv_on", "s0_reg_iac", "s0_reg_vac", "s0_reg_iac_lim", "s0_reg_vac_lim" });
		deviceTable1.setDisplayRowNumber(true);
		deviceTable1.setPreferredColumnWidth(70);
		deviceTable1.setPreferredHeight(100);
		jPanel4.setLayout(gridLayout2);
		gridLayout2.setColumns(1);
		gridLayout2.setRows(2);
		deviceTable2.setPreferredHeight(100);
		deviceTable2.setPreferredColumnWidth(70);
		deviceTable2.setDisplayRowNumber(true);
		deviceTable2.setRowNames(new String[]
		{ "ls_ch_init_state", "ch2_lockt_ls", "irelease_ls", "hs_ch_init_state", "ch1_lockt_hs", "irelease_hs",
				"ith_inv_on", "s0_reg_iac", "s0_reg_vac", "s0_reg_iac_lim", "s0_reg_vac_lim" });
		deviceTable2.setColumnNames(new String[]
		{ "Sect.7", "Sect.8", "Sect.9", "Sect.10", "Sect.11", "Sect.12" });
		deviceTable2.setIdentifier("");
		deviceTable2.setNumRows(11);
		deviceTable2.setNumCols(6);
		deviceTable2.setLabelString("");
		deviceTable2.setOffsetNid(9);
		jPanel10.setDebugGraphicsOptions(0);
		jPanel10.setLayout(gridLayout3);
		jPanel11.setLayout(borderLayout5);
		deviceTable5.setPreferredHeight(17);
		deviceTable5.setPreferredColumnWidth(60);
		deviceTable5.setDisplayRowNumber(false);
		deviceTable5.setEditable(false);
		deviceTable5.setColumnNames(new String[]
		{ "Sect.1", "Sect.2", "Sect.3", "Sect.4", "Sect.5", "Sect.6" });
		deviceTable5.setIdentifier("");
		deviceTable5.setNumRows(1);
		deviceTable5.setNumCols(6);
		deviceTable5.setLabelString("Bank Selection CPU1");
		deviceTable5.setOffsetNid(12);
		deviceField3.setOffsetNid(10);
		deviceField3.setTextOnly(true);
		deviceField3.setLabelString("Selector CPU1: ");
		deviceField3.setIdentifier("");
		deviceField3.setEditable(false);
		deviceTable6.setPreferredHeight(100);
		deviceTable6.setPreferredColumnWidth(40);
		deviceTable6.setDisplayRowNumber(true);
		deviceTable6.setRowNames(new String[]
		{ "vth_up1_slow", "vth_up2_slow", "vth_dw_slow", "ls_ch_init_state", "ch2_cnt1_la", "ch2_cnt2_ls", "timeout_ls",
				"tmin_on_ls", "tmin_off_ls", "ch2_lockt_ls", "irelease_ls", "vth_up1_fast", "vth_up2_fast",
				"vtw_dw_fast", "hs_ch_init_state", "ch2_cnt1_hs", "ch2_cnt2_hs", "timeout_hs", "tmin_on_hs",
				"tmin_off_hs", "ch1_lockt_hs", "irelease_hs", "ch1_to_hs", "kp_reg_iac", "ki_reg_iac", "up_reg_iac",
				"lo_reg_iac", "tc_init_h", "fpga_clk", "tmin_on_h", "tmin_off_h", "tdead", "tfbon_h", "tfboff_h",
				"ith_inv_on", "ith_inv_end_force", "imin_inv_off", "vdc_max", "vmaxload", "ih_max", "vdc_overvolt",
				"vdc_undevolt", "vdc_t", "spare44", "spare45", "iac_lim", "vac_lim", "spare48", "kp_reg_vac",
				"ki_reg_vac", "s0_reg_vac", "up_reg_vac", "lo_reg_vac", "kp_rec_iac_lim", "ki_reg_iac_lim",
				"s0_reg_iac_lim", "up_reg_iac_lim", "lo_reg_uiac_lim", "kp_reg_vac_lim", "ki_reg_vac_lim",
				"s0_reg_vac_lim", "up_reg_vac_lim", "lo_reg_vac_lim", "s0_reg_iac", "spare1", "spare2", "spare3",
				"spare4", "spare5", "spare6", "spare7", "spare8", "spare9", "spare10", "spare11" });
		deviceTable6.setEditable(false);
		deviceTable6.setColumnNames(new String[]
		{ "Sect.1", "Sect.2", "Sect.3", "Sect.4", "Sect.5", "Sect.6" });
		deviceTable6.setIdentifier("");
		deviceTable6.setNumRows(75);
		deviceTable6.setNumCols(6);
		deviceTable6.setLabelString("");
		deviceTable6.setOffsetNid(14);
		jPanel12.setLayout(borderLayout6);
		deviceTable7.setOffsetNid(13);
		deviceTable7.setLabelString("Bank Selection CPU2");
		deviceTable7.setNumCols(6);
		deviceTable7.setNumRows(1);
		deviceTable7.setIdentifier("");
		deviceTable7.setColumnNames(new String[]
		{ "Sect.7", "Sect.8", "Sect.9", "Sect.10", "Sect.11", "Sect.12" });
		deviceTable7.setRowNames(null);
		deviceTable7.setEditable(false);
		deviceTable7.setDisplayRowNumber(false);
		deviceTable7.setPreferredColumnWidth(60);
		deviceTable7.setPreferredHeight(17);
		deviceField4.setEditable(false);
		deviceField4.setIdentifier("");
		deviceField4.setLabelString("Selector CPU2: ");
		deviceField4.setTextOnly(true);
		deviceField4.setOffsetNid(11);
		deviceTable8.setOffsetNid(15);
		deviceTable8.setLabelString("");
		deviceTable8.setNumCols(6);
		deviceTable8.setNumRows(75);
		deviceTable8.setIdentifier("");
		deviceTable8.setColumnNames(new String[]
		{ "Sect.7", "Sect.8", "Sect.9", "Sect.10", "Sect.11", "Sect.12" });
		deviceTable8.setEditable(false);
		deviceTable8.setRowNames(new String[]
		{ "vth_up1_slow", "vth_up2_slow", "vth_dw_slow", "ls_ch_init_state", "ch2_cnt1_la", "ch2_cnt2_ls", "timeout_ls",
				"tmin_on_ls", "tmin_off_ls", "ch2_lockt_ls", "irelease_ls", "vth_up1_fast", "vth_up2_fast",
				"vtw_dw_fast", "hs_ch_init_state", "ch2_cnt1_hs", "ch2_cnt2_hs", "timeout_hs", "tmin_on_hs",
				"tmin_off_hs", "ch1_lockt_hs", "irelease_hs", "ch1_to_hs", "kp_reg_iac", "ki_reg_iac", "up_reg_iac",
				"lo_reg_iac", "tc_init_h", "fpga_clk", "tmin_on_h", "tmin_off_h", "tdead", "tfbon_h", "tfboff_h",
				"ith_inv_on", "ith_inv_end_force", "imin_inv_off", "vdc_max", "vmaxload", "ih_max", "vdc_overvolt",
				"vdc_undevolt", "vdc_t", "spare44", "spare45", "iac_lim", "vac_lim", "spare48", "kp_reg_vac",
				"ki_reg_vac", "s0_reg_vac", "up_reg_vac", "lo_reg_vac", "kp_rec_iac_lim", "ki_reg_iac_lim",
				"s0_reg_iac_lim", "up_reg_iac_lim", "lo_reg_uiac_lim", "kp_reg_vac_lim", "ki_reg_vac_lim",
				"s0_reg_vac_lim", "up_reg_vac_lim", "lo_reg_vac_lim", "s0_reg_iac", "spare1", "spare2", "spare3",
				"spare4", "spare5", "spare6", "spare7", "spare8", "spare9", "spare10", "spare11" });
		deviceTable8.setDisplayRowNumber(true);
		deviceTable8.setPreferredColumnWidth(40);
		deviceTable8.setPreferredHeight(100);
		jPanel13.setBorder(null);
		jPanel11.setBorder(null);
		jPanel11.setDebugGraphicsOptions(0);
		jPanel12.setBorder(null);
		gridLayout3.setColumns(1);
		gridLayout3.setRows(2);
		deviceField5.setOffsetNid(16);
		deviceField5.setTextOnly(true);
		deviceField5.setLabelString("TF Units: ");
		deviceField5.setNumCols(30);
		deviceField5.setIdentifier("");
		deviceField5.setEditable(false);
		deviceField6.setOffsetNid(20);
		deviceField6.setLabelString("TF Window: ");
		deviceField6.setNumCols(4);
		deviceField6.setIdentifier("");
		deviceField6.setEditable(false);
		deviceField2.setOffsetNid(17);
		deviceField2.setTextOnly(true);
		deviceField2.setLabelString("TF Connection: ");
		deviceField2.setNumCols(8);
		deviceField2.setIdentifier("");
		deviceField2.setEditable(false);
		deviceChoice5.setIdentifier("");
		deviceChoice5.setUpdateIdentifier("");
		deviceChoice5.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice5.setLabelString("Control :");
		deviceChoice5.setOffsetNid(2);
		deviceChoice5.setChoiceFloatValues(null);
		deviceChoice5.setChoiceIntValues(null);
		deviceChoice7.setIdentifier("");
		deviceChoice7.setUpdateIdentifier("");
		deviceChoice7.setChoiceItems(new String[]
		{ "8mF", "16mF" });
		deviceChoice7.setLabelString("Selection Bank :");
		deviceChoice7.setOffsetNid(4);
		deviceChoice7.setChoiceFloatValues(null);
		deviceChoice7.setChoiceIntValues(null);
		this.getContentPane().add(deviceField1, BorderLayout.NORTH);
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jTabbedPane1, BorderLayout.CENTER);
		jTabbedPane1.add(jPanel1, "TC Send");
		jPanel1.add(jPanel5, BorderLayout.NORTH);
		jPanel5.add(jPanel6, null);
		jPanel6.add(deviceChoice2, null);
		jPanel6.add(deviceChoice5, null);
		jPanel6.add(deviceChoice7, null);
		jPanel1.add(jPanel4, BorderLayout.CENTER);
		jPanel4.add(jScrollPane2, null);
		jScrollPane2.getViewport().add(deviceTable1, null);
		jPanel4.add(jScrollPane1, null);
		jScrollPane1.getViewport().add(deviceTable2, null);
		jTabbedPane1.add(jPanel10, "TC Receive");
		jPanel10.add(jPanel11, null);
		jPanel11.add(jPanel13, BorderLayout.NORTH);
		jPanel13.add(deviceField3, null);
		jPanel13.add(deviceTable5, null);
		jPanel11.add(jScrollPane4, BorderLayout.CENTER);
		jScrollPane4.getViewport().add(deviceTable6, null);
		jPanel10.add(jPanel12, null);
		jPanel12.add(jPanel14, BorderLayout.NORTH);
		jPanel14.add(deviceField4, null);
		jPanel14.add(deviceTable7, null);
		jPanel12.add(jScrollPane5, BorderLayout.CENTER);
		jTabbedPane1.add(jPanel2, "TF");
		jPanel2.add(deviceField5, null);
		jPanel2.add(deviceField2, null);
		jPanel2.add(deviceField6, null);
		jScrollPane5.getViewport().add(deviceTable8, null);
	}
}

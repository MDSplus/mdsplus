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
public class RFXABUnitsSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	JPanel jPanel1 = new JPanel();
	DeviceField deviceField4 = new DeviceField();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JTabbedPane jTabbedPane1 = new JTabbedPane();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel2 = new JPanel();
	JPanel jPanel3 = new JPanel();
	JPanel jPanel4 = new JPanel();
	JPanel jPanel5 = new JPanel();
	BorderLayout borderLayout2 = new BorderLayout();
	BorderLayout borderLayout3 = new BorderLayout();
	DeviceTable deviceTable2 = new DeviceTable();
	DeviceTable deviceTable1 = new DeviceTable();
	DeviceField deviceField1 = new DeviceField();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField5 = new DeviceField();

	public RFXABUnitsSetup()
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
		this.setWidth(529);
		this.setHeight(529);
		this.setDeviceType("RFXABUnits");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("A - B Units Configuration");
		this.getContentPane().setLayout(borderLayout1);
		deviceField4.setOffsetNid(1);
		deviceField4.setTextOnly(true);
		deviceField4.setLabelString("Comment");
		deviceField4.setNumCols(35);
		deviceField4.setIdentifier("");
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(3);
		jPanel4.setLayout(borderLayout2);
		jPanel5.setLayout(borderLayout3);
		deviceTable2.setRowNames(new String[]
		{ "CL05_S", "CL5_S", "VL_S", "KP_I_S", "KI_I_S", "KD_I_S", "KP_V_S", "KI_V_S", "KP_RIP_S", "KI_RIP_S",
				"KP_FF_S", "OMEGA_P_S", "CL05_T", "CL5_T", "VL_T", "KP_I_T", "KI_I_T", "KD_I_T", "KP_V_T", "KI_V_T",
				"KP_RIP_T", "KI_RIP_T", "KP_FF_T", "OMEGA_P_T" });
		deviceTable2.setEditable(false);
		deviceTable2.setDisplayRowNumber(true);
		deviceTable2.setColumnNames(new String[]
		{ "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "A10", "A11", "A12" });
		deviceTable2.setIdentifier("");
		deviceTable2.setNumRows(24);
		deviceTable2.setNumCols(12);
		deviceTable2.setLabelString("Parameters");
		deviceTable2.setOffsetNid(6);
		deviceTable1.setOffsetNid(7);
		deviceTable1.setLabelString("Parameters");
		deviceTable1.setNumCols(8);
		deviceTable1.setNumRows(24);
		deviceTable1.setIdentifier("");
		deviceTable1.setColumnNames(new String[]
		{ "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8" });
		deviceTable1.setRowNames(new String[]
		{ "CL05_S", "CL5_S", "VL_S", "KP_I_S", "KI_I_S", "KD_I_S", "KP_V_S", "KI_V_S", "KP_RIP_S", "KI_RIP_S",
				"KP_FF_S", "OMEGA_P_S", "CL05_T", "CL5_T", "VL_T", "KP_I_T", "KI_I_T", "KD_I_T", "KP_V_T", "KI_V_T",
				"KP_RIP_T", "KI_RIP_T", "KP_FF_T", "OMEGA_P_T" });
		deviceTable1.setEditable(false);
		deviceTable1.setDisplayRowNumber(true);
		deviceField2.setOffsetNid(2);
		deviceField2.setTextOnly(true);
		deviceField2.setLabelString("PM Units: ");
		deviceField2.setNumCols(15);
		deviceField2.setIdentifier("");
		deviceField2.setEditable(false);
		deviceField2.setDisplayEvaluated(true);
		deviceField1.setOffsetNid(3);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("PC Units: ");
		deviceField1.setNumCols(15);
		deviceField1.setIdentifier("");
		deviceField1.setEditable(false);
		deviceField1.setDisplayEvaluated(true);
		deviceField5.setOffsetNid(4);
		deviceField5.setTextOnly(true);
		deviceField5.setLabelString("PV Units: ");
		deviceField5.setNumCols(15);
		deviceField5.setIdentifier("");
		deviceField5.setEditable(false);
		deviceField5.setDisplayEvaluated(true);
		deviceField3.setOffsetNid(5);
		deviceField3.setTextOnly(true);
		deviceField3.setLabelString("TF Units: ");
		deviceField3.setNumCols(15);
		deviceField3.setIdentifier("");
		deviceField3.setEditable(false);
		deviceField3.setDisplayEvaluated(true);
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(deviceField4, null);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceField2, null);
		jPanel2.add(deviceField1, null);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceField5, null);
		jPanel3.add(deviceField3, null);
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jTabbedPane1, BorderLayout.CENTER);
		jTabbedPane1.add(jPanel4, "A Units");
		jPanel4.add(deviceTable2, BorderLayout.CENTER);
		jTabbedPane1.add(jPanel5, "B Units");
		jPanel5.add(deviceTable1, BorderLayout.CENTER);
	}
}

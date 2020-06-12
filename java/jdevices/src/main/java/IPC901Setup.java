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
public class IPC901Setup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel1 = new JPanel();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel2 = new JPanel();
	JPanel jPanel5 = new JPanel();
	JPanel jPanel6 = new JPanel();
	JPanel jPanel7 = new JPanel();
	JPanel jPanel8 = new JPanel();
	JPanel jPanel9 = new JPanel();
	JPanel jPanel10 = new JPanel();
	JPanel jPanel11 = new JPanel();
	DeviceField deviceField1 = new DeviceField();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceChoice deviceChoice3 = new DeviceChoice();
	DeviceChoice deviceChoice4 = new DeviceChoice();
	DeviceChoice deviceChoice5 = new DeviceChoice();
	DeviceChoice deviceChoice6 = new DeviceChoice();
	DeviceChoice deviceChoice7 = new DeviceChoice();
	DeviceChoice deviceChoice9 = new DeviceChoice();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();
	DeviceField deviceField9 = new DeviceField();
	DeviceField deviceField10 = new DeviceField();

	public IPC901Setup()
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
		this.setWidth(619);
		this.setHeight(400);
		this.setDeviceType("IPC901");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("IPROM IPC 901");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setRows(8);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("CAMAC Name:");
		deviceField1.setIdentifier("");
		deviceField2.setOffsetNid(2);
		deviceField2.setTextOnly(true);
		deviceField2.setLabelString("Comment:");
		deviceField2.setNumCols(30);
		deviceField2.setIdentifier("");
		deviceField4.setOffsetNid(3);
		deviceField4.setLabelString("Wave Length 1: ");
		deviceField4.setNumCols(6);
		deviceField4.setIdentifier("");
		deviceField3.setOffsetNid(4);
		deviceField3.setLabelString("Wave length 2:");
		deviceField3.setNumCols(6);
		deviceField3.setIdentifier("");
		deviceChoice1.setChoiceIntValues(null);
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setOffsetNid(5);
		deviceChoice1.setLabelString("Trigger Mode: ");
		deviceChoice1.setChoiceItems(new String[]
		{ "INTERNAL", "EXTERNAL" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceField5.setOffsetNid(7);
		deviceField5.setLabelString("End Time: ");
		deviceField5.setNumCols(4);
		deviceField5.setIdentifier("");
		deviceField6.setOffsetNid(6);
		deviceField6.setLabelString("Ext. Trigger: ");
		deviceField6.setNumCols(20);
		deviceField6.setIdentifier("");
		deviceChoice3.setChoiceIntValues(null);
		deviceChoice3.setChoiceFloatValues(null);
		deviceChoice3.setOffsetNid(8);
		deviceChoice3.setLabelString("Acquisition mode: ");
		deviceChoice3.setChoiceItems(new String[]
		{ "MEASURE", "CALIBRATION" });
		deviceChoice3.setUpdateIdentifier("");
		deviceChoice3.setIdentifier("");
		deviceChoice2.setChoiceIntValues(null);
		deviceChoice2.setChoiceFloatValues(null);
		deviceChoice2.setOffsetNid(9);
		deviceChoice2.setLabelString("Store Flag: ");
		deviceChoice2.setChoiceItems(new String[]
		{ "YES", "NO" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		deviceChoice5.setChoiceIntValues(new int[]
		{ 1, 2, 4, 8, 16, 32 });
		deviceChoice5.setChoiceFloatValues(null);
		deviceChoice5.setOffsetNid(10);
		deviceChoice5.setLabelString("DAC Gain: ");
		deviceChoice5.setChoiceItems(new String[]
		{ "1", "2", "4", "8", "16", "32" });
		deviceChoice5.setUpdateIdentifier("");
		deviceChoice5.setIdentifier("");
		deviceChoice4.setChoiceIntValues(null);
		deviceChoice4.setChoiceFloatValues(null);
		deviceChoice4.setOffsetNid(11);
		deviceChoice4.setLabelString("DAC oputput");
		deviceChoice4.setChoiceItems(new String[]
		{ "PHASE_1", "PHASE_2" });
		deviceChoice4.setUpdateIdentifier("");
		deviceChoice4.setIdentifier("");
		deviceChoice7.setChoiceIntValues(null);
		deviceChoice7.setChoiceFloatValues(new float[]
		{ (float) 0.0, (float) 2500.0, (float) 5000.0, (float) 10000.0, (float) 25000.0, (float) 50000.0 });
		deviceChoice7.setOffsetNid(12);
		deviceChoice7.setLabelString("FIR Cut Off:");
		deviceChoice7.setChoiceItems(new String[]
		{ "0", "2.5E3", "5E3", "10E3", "25E3", "50E3" });
		deviceChoice7.setUpdateIdentifier("");
		deviceChoice7.setIdentifier("");
		deviceChoice6.setChoiceIntValues(new int[]
		{ 8, 16, 32 });
		deviceChoice6.setChoiceFloatValues(null);
		deviceChoice6.setOffsetNid(13);
		deviceChoice6.setLabelString("FIR Window: ");
		deviceChoice6.setChoiceItems(new String[]
		{ "8", "16", "32" });
		deviceChoice6.setUpdateIdentifier("");
		deviceChoice6.setIdentifier("");
		deviceChoice9.setChoiceIntValues(new int[]
		{ 1, 2, 4, 8, 16, 32 });
		deviceChoice9.setChoiceFloatValues(null);
		deviceChoice9.setOffsetNid(14);
		deviceChoice9.setLabelString("Hard Decim.");
		deviceChoice9.setChoiceItems(new String[]
		{ "1", "2", "4", "8", "16", "32" });
		deviceChoice9.setUpdateIdentifier("");
		deviceChoice9.setIdentifier("");
		deviceField10.setOffsetNid(16);
		deviceField10.setLabelString("OVF lev. 1:");
		deviceField10.setNumCols(6);
		deviceField10.setIdentifier("");
		deviceField9.setOffsetNid(17);
		deviceField9.setLabelString("OVF lev. 2:");
		deviceField9.setNumCols(6);
		deviceField9.setIdentifier("");
		deviceField8.setOffsetNid(18);
		deviceField8.setLabelString("UFL lev. 1:");
		deviceField8.setNumCols(6);
		deviceField8.setIdentifier("");
		deviceField7.setOffsetNid(19);
		deviceField7.setLabelString("UFL lev. 2:");
		deviceField7.setNumCols(6);
		deviceField7.setIdentifier("");
		deviceButtons1.setCheckExpressions(null);
		deviceButtons1.setCheckMessages(null);
		deviceButtons1.setMethods(new String[]
		{ "INIT", "TRIGGER", "STORE", "RESET" });
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.CENTER);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceField1, null);
		jPanel2.add(deviceDispatch1, null);
		jPanel1.add(jPanel11, null);
		jPanel11.add(deviceField2, null);
		jPanel1.add(jPanel10, null);
		jPanel10.add(deviceField5, null);
		jPanel10.add(deviceField4, null);
		jPanel10.add(deviceField3, null);
		jPanel1.add(jPanel9, null);
		jPanel9.add(deviceChoice1, null);
		jPanel9.add(deviceField6, null);
		jPanel1.add(jPanel8, null);
		jPanel8.add(deviceChoice3, null);
		jPanel8.add(deviceChoice2, null);
		jPanel1.add(jPanel7, null);
		jPanel7.add(deviceChoice5, null);
		jPanel7.add(deviceChoice4, null);
		jPanel1.add(jPanel6, null);
		jPanel6.add(deviceChoice9, null);
		jPanel6.add(deviceChoice7, null);
		jPanel6.add(deviceChoice6, null);
		jPanel1.add(jPanel5, null);
		jPanel5.add(deviceField10, null);
		jPanel5.add(deviceField9, null);
		jPanel5.add(deviceField8, null);
		jPanel5.add(deviceField7, null);
	}
}

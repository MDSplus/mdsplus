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
public class RFXPulseSetup extends DeviceSetup
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
	JPanel jPanel3 = new JPanel();
	JPanel jPanel4 = new JPanel();
	JPanel jPanel5 = new JPanel();
	JPanel jPanel6 = new JPanel();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	DeviceField deviceField1 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceField deviceField4 = new DeviceField();
	DeviceChoice deviceChoice3 = new DeviceChoice();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();

	public RFXPulseSetup()
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
		this.setWidth(550);
		this.setHeight(350);
		this.setDeviceType("RFXPulse");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("Pulse Generator for MPB timing");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(5);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Comment: ");
		deviceField1.setNumCols(20);
		deviceField1.setIdentifier("");
		deviceField2.setOffsetNid(2);
		deviceField2.setLabelString("Decoder: ");
		deviceField2.setNumCols(15);
		deviceField2.setIdentifier("");
		deviceChoice1.setChoiceIntValues(new int[]
		{ 1, 2, 3, 4, 5 });
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setOffsetNid(3);
		deviceChoice1.setLabelString("Channel: ");
		deviceChoice1.setChoiceItems(new String[]
		{ "1", "2", "3", "4", "5" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceChoice2.setChoiceIntValues(null);
		deviceChoice2.setChoiceFloatValues(null);
		deviceChoice2.setOffsetNid(4);
		deviceChoice2.setLabelString("Trigger mode: ");
		deviceChoice2.setChoiceItems(new String[]
		{ "EVENT", "TRIGGER RISING", "TRIGGER FALLING", "SOFTWARE" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		deviceField3.setOffsetNid(5);
		deviceField3.setTextOnly(true);
		deviceField3.setLabelString("Event");
		deviceField3.setIdentifier("");
		deviceChoice3.setChoiceIntValues(null);
		deviceChoice3.setChoiceFloatValues(null);
		deviceChoice3.setOffsetNid(14);
		deviceChoice3.setLabelString("Output mode: ");
		deviceChoice3.setChoiceItems(new String[]
		{ "HIGH PULSE", "LOW PULSE", "SINGLE TOGGLE: INITIAL HIGH", "SINGLE TOGGLE: INITIAL LOW",
				"DOUBLE TOGGLE: INITIAL HIGH", "DOUBLE TOGGLE: INITIAL LOW" });
		deviceChoice3.setUpdateIdentifier("");
		deviceChoice3.setIdentifier("");
		deviceField4.setOffsetNid(6);
		deviceField4.setLabelString("Ext. trigger: ");
		deviceField4.setNumCols(10);
		deviceField4.setIdentifier("");
		deviceField6.setOffsetNid(7);
		deviceField6.setLabelString("Delay(s): ");
		deviceField6.setIdentifier("");
		deviceField5.setOffsetNid(8);
		deviceField5.setLabelString("Duration(s): ");
		deviceField5.setIdentifier("");
		deviceButtons1.setCheckExpressions(null);
		deviceButtons1.setCheckMessages(null);
		deviceButtons1.setMethods(new String[]
		{ "INIT" });
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.CENTER);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceField1, null);
		jPanel2.add(deviceDispatch1, null);
		jPanel1.add(jPanel6, null);
		jPanel6.add(deviceField2, null);
		jPanel6.add(deviceChoice1, null);
		jPanel1.add(jPanel5, null);
		jPanel5.add(deviceChoice2, null);
		jPanel5.add(deviceField3, null);
		jPanel1.add(jPanel4, null);
		jPanel4.add(deviceChoice3, null);
		jPanel4.add(deviceField4, null);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceField6, null);
		jPanel3.add(deviceField5, null);
	}
}

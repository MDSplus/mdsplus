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
public class RFXPMSetupSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel1 = new JPanel();
	DeviceWave deviceWave1 = new DeviceWave();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel2 = new JPanel();
	JPanel jPanel3 = new JPanel();
	JPanel jPanel4 = new JPanel();
	DeviceField deviceField1 = new DeviceField();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceField deviceField7 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();

	// DeviceField deviceField5 = new DeviceField();
	public RFXPMSetupSetup()
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
		this.setWidth(591);
		this.setHeight(500);
		this.setDeviceType("RFXPMSetup");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("RFX PM Setup");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(3);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Comment:");
		deviceField1.setNumCols(30);
		deviceField1.setIdentifier("");
		deviceField2.setOffsetNid(4);
		deviceField2.setLabelString("Window");
		deviceField2.setNumCols(4);
		deviceField2.setIdentifier("");
		deviceField2.setEditable(false);
		deviceField2.setDisplayEvaluated(true);
		deviceField3.setOffsetNid(5);
		deviceField3.setTextOnly(true);
		deviceField3.setLabelString("Enabled Units");
		deviceField3.setNumCols(25);
		deviceField3.setIdentifier("");
		deviceField3.setEditable(false);
		deviceField3.setDisplayEvaluated(true);
		deviceField6.setOffsetNid(2);
		deviceField6.setTextOnly(true);
		deviceField6.setLabelString("Connection:");
		deviceField6.setNumCols(8);
		deviceField6.setIdentifier("");
		deviceField6.setEditable(false);
		deviceField6.setDisplayEvaluated(true);
		deviceField4.setOffsetNid(6);
		deviceField4.setLabelString("Tot. Current Limit:");
		deviceField4.setNumCols(6);
		deviceField4.setIdentifier("imax");
		deviceWave1.setOffsetNid(8);
		deviceWave1.setIdentifier("wave");
		deviceWave1.setUpdateIdentifier("");
		deviceWave1.setUpdateExpression("");
		deviceButtons1.setCheckExpressions(new String[]
		{ "(maxval(_wave) *  \\POLOIDAL:PM_ENABLED) <= _imax" });
		deviceButtons1.setCheckMessages(new String[]
		{ "Total magnetizing current above limit" });
		/*
		 * deviceChoice1.setChoiceIntValues(null);
		 * deviceChoice1.setChoiceFloatValues(null); deviceChoice1.setOffsetNid(3);
		 * deviceChoice1.setLabelString("Control"); deviceChoice1.setChoiceItems(new
		 * String[] {"CURRENT", "VOLTAGE", "OPEN LOOP"});
		 * deviceChoice1.setUpdateIdentifier(""); deviceChoice1.setIdentifier("");
		 */
		deviceField7.setOffsetNid(3);
		deviceField7.setTextOnly(true);
		deviceField7.setLabelString("Control:");
		deviceField7.setNumCols(8);
		deviceField7.setIdentifier("");
		deviceField7.setEditable(false);
		deviceField7.setDisplayEvaluated(true);
		/*
		 * deviceField5.setOffsetNid(13); deviceField5.setLabelString("PTCB Voltage");
		 * deviceField5.setNumCols(4); deviceField5.setIdentifier("");
		 */
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceField1, null);
		// jPanel2.add(deviceField5, null);
		jPanel1.add(jPanel4, null);
		jPanel4.add(deviceField3, null);
		jPanel4.add(deviceField2, null);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceField6, null);
//    jPanel3.add(deviceChoice1, null);
		jPanel3.add(deviceField7, null);
		jPanel3.add(deviceField4, null);
		this.getContentPane().add(deviceWave1, BorderLayout.CENTER);
	}
}

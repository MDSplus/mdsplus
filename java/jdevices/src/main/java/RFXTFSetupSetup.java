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
public class RFXTFSetupSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	JPanel jPanel1 = new JPanel();
	GridLayout gridLayout1 = new GridLayout();
	DeviceField deviceField1 = new DeviceField();
	JPanel jPanel2 = new JPanel();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	JPanel jPanel3 = new JPanel();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	DeviceWave deviceWave1 = new DeviceWave();
	DeviceField deviceField4 = new DeviceField();
	DeviceField deviceField5 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();

	public RFXTFSetupSetup()
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
		this.setWidth(501);
		this.setHeight(501);
		this.setDeviceType("RFXTFSetup");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("TF Setup");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setRows(3);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Comment: ");
		deviceField1.setNumCols(25);
		deviceField1.setIdentifier("");
		deviceField3.setOffsetNid(5);
		deviceField3.setTextOnly(true);
		deviceField3.setLabelString("Enabled Units: ");
		deviceField3.setNumCols(15);
		deviceField3.setIdentifier("");
		deviceField3.setEditable(false);
		deviceField3.setDisplayEvaluated(true);
		deviceField2.setOffsetNid(3);
		deviceField2.setLabelString("Window: ");
		deviceField2.setNumCols(4);
		deviceField2.setIdentifier("");
		deviceField2.setEditable(false);
		deviceField2.setDisplayEvaluated(true);
		deviceWave1.setOffsetNid(6);
		deviceWave1.setIdentifier("");
		deviceWave1.setUpdateExpression("");
		deviceField4.setOffsetNid(4);
		deviceField4.setTextOnly(true);
		deviceField4.setLabelString("Connection: ");
		deviceField4.setIdentifier("");
		deviceField4.setEditable(false);
		deviceField4.setDisplayEvaluated(true);
		/*
		 * deviceChoice1.setChoiceIntValues(null);
		 * deviceChoice1.setChoiceFloatValues(null); deviceChoice1.setOffsetNid(2);
		 * deviceChoice1.setLabelString("Control:"); deviceChoice1.setChoiceItems(new
		 * String[] {"CURRENT", "VOLTAGE", "OPEN LOOP"});
		 * deviceChoice1.setUpdateIdentifier(""); deviceChoice1.setIdentifier("");
		 */
		deviceField5.setOffsetNid(2);
		deviceField5.setTextOnly(true);
		deviceField5.setLabelString("Control: ");
		deviceField5.setIdentifier("");
		deviceField5.setEditable(false);
		deviceField5.setDisplayEvaluated(true);
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(deviceField1, null);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceField3, null);
		jPanel2.add(deviceField2, null);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceField4, null);
//  jPanel3.add(deviceChoice1, null);
		jPanel3.add(deviceField5, null);
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(deviceWave1, BorderLayout.CENTER);
	}
}

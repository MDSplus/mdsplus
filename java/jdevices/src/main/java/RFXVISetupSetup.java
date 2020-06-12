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
public class RFXVISetupSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel1 = new JPanel();
	JTabbedPane jTabbedPane1 = new JTabbedPane();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel2 = new JPanel();
	JPanel jPanel3 = new JPanel();
	JPanel jPanel4 = new JPanel();
	DeviceField deviceField1 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	JPanel jPanel5 = new JPanel();
	JPanel jPanel6 = new JPanel();
	DeviceWave deviceWave1 = new DeviceWave();
	BorderLayout borderLayout2 = new BorderLayout();
	BorderLayout borderLayout3 = new BorderLayout();
	DeviceWave deviceWave2 = new DeviceWave();

	public RFXVISetupSetup()
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
		this.setWidth(547);
		this.setHeight(547);
		this.setDeviceType("RFXVISetup");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("Gas Puffing & Filling Setup");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(3);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Comment: ");
		deviceField1.setNumCols(30);
		deviceField1.setIdentifier("");
		deviceChoice1.setChoiceIntValues(new int[]
		{ 1, 2, 3, 4, 5, 6, 7 });
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setConvert(true);
		deviceChoice1.setOffsetNid(2);
		deviceChoice1.setLabelString("FIlling Type: ");
		deviceChoice1.setChoiceItems(new String[]
		{ "Open Loop Continuous Flux", "Closed Loop Continuous Flux", "Pulsed Flux", "Fixed Charge",
				"Open Loop Continuous Flux + Pulsed Flux", "Closed Loop Continuous Flux + Pulsed Flux",
				"Fixed Charge + Pulsed Flux" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceField3.setOffsetNid(3);
		deviceField3.setLabelString("FIlling Press (mbar): ");
		deviceField3.setNumCols(10);
		deviceField3.setIdentifier("");
		deviceField2.setOffsetNid(4);
		deviceField2.setLabelString("Impurity Lev. (Volt): ");
		deviceField2.setIdentifier("");
		deviceWave1.setOffsetNid(5);
		deviceWave1.setMaxYVisible(false);
		deviceWave1.setIdentifier("");
		deviceWave1.setUpdateExpression("");
		jPanel5.setLayout(borderLayout2);
		jPanel6.setLayout(borderLayout3);
		deviceWave2.setOffsetNid(10);
		deviceWave2.setMaxYVisible(false);
		deviceWave2.setIdentifier("");
		deviceWave2.setUpdateExpression("");
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceField1, null);
		jPanel1.add(jPanel4, null);
		jPanel4.add(deviceChoice1, null);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceField3, null);
		jPanel3.add(deviceField2, null);
		this.getContentPane().add(jTabbedPane1, BorderLayout.CENTER);
		// jTabbedPane1.add(jPanel6, "PZ Puffing");
		jTabbedPane1.add(jPanel6, "H2 injection");
		// jTabbedPane1.add(jPanel5, "PZ Filling");
		jTabbedPane1.add(jPanel5, "He/impurities injection");
		jPanel5.add(deviceWave1, BorderLayout.CENTER);
		jPanel6.add(deviceWave2, BorderLayout.CENTER);
	}
}

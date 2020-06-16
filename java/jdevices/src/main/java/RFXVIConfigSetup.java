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
public class RFXVIConfigSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel1 = new JPanel();
	JPanel jPanel2 = new JPanel();
	DeviceField deviceField1 = new DeviceField();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel3 = new JPanel();
	JPanel jPanel4 = new JPanel();
	GridLayout gridLayout2 = new GridLayout();
	GridLayout gridLayout3 = new GridLayout();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();
	DeviceField deviceField9 = new DeviceField();
	DeviceField deviceField14 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceChoice deviceChoice3 = new DeviceChoice();
	DeviceField deviceField10 = new DeviceField();

	public RFXVIConfigSetup()
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
		this.setWidth(428);
		this.setHeight(428);
		this.setDeviceType("RFXVIConfig");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("RFX Gas Puffing&Filling Config");
		this.getContentPane().setLayout(borderLayout1);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Comment: ");
		deviceField1.setNumCols(25);
		deviceField1.setIdentifier("");
		jPanel2.setLayout(gridLayout1);
		gridLayout1.setColumns(2);
		gridLayout1.setRows(1);
		jPanel3.setLayout(gridLayout2);
		jPanel4.setLayout(gridLayout3);
		gridLayout2.setColumns(1);
		gridLayout2.setRows(7);
		gridLayout3.setColumns(1);
		gridLayout3.setRows(7);
		deviceField2.setOffsetNid(3);
		deviceField2.setLabelString("N. Turbo Pumps On: ");
		deviceField2.setNumCols(6);
		deviceField2.setIdentifier("");
		deviceField2.setEditable(false);
		deviceField7.setOffsetNid(4);
		deviceField7.setLabelString("N. Group Fill Valv.:");
		deviceField7.setNumCols(6);
		deviceField7.setIdentifier("");
		deviceField6.setOffsetNid(8);
		deviceField6.setLabelString("VIK1 Press.(mbar): ");
		deviceField6.setNumCols(6);
		deviceField6.setIdentifier("");
		deviceField6.setEditable(false);
		deviceField5.setOffsetNid(10);
		deviceField5.setLabelString("VIK2 Press.(mbar): ");
		deviceField5.setNumCols(6);
		deviceField5.setIdentifier("");
		deviceField5.setEditable(false);
		deviceField4.setOffsetNid(12);
		deviceField4.setLabelString("Bottle Press.(mbar): ");
		deviceField4.setNumCols(6);
		deviceField4.setIdentifier("");
		deviceField4.setEditable(false);
		deviceField3.setOffsetNid(13);
		deviceField3.setLabelString("VVMC01 INI(mbar): ");
		deviceField3.setNumCols(6);
		deviceField3.setIdentifier("");
		deviceField3.setEditable(false);
		deviceField8.setOffsetNid(14);
		deviceField8.setLabelString("VVMC01 PPC(mbar): ");
		deviceField8.setNumCols(6);
		deviceField8.setIdentifier("");
		deviceField8.setEditable(false);
		deviceField9.setOffsetNid(2);
		deviceField9.setLabelString("N. Crio Pumps On: ");
		deviceField9.setNumCols(6);
		deviceField9.setIdentifier("");
		deviceField9.setEditable(false);
		deviceField14.setOffsetNid(5);
		deviceField14.setLabelString("N. Group Puff Valv.: ");
		deviceField14.setNumCols(6);
		deviceField14.setIdentifier("");
		deviceChoice1.setChoiceIntValues(null);
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setOffsetNid(7);
		deviceChoice1.setLabelString("VIK1 gas: ");
		deviceChoice1.setChoiceItems(new String[]
		{ "H2", "He", "Not Used" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceChoice2.setChoiceIntValues(null);
		deviceChoice2.setChoiceFloatValues(null);
		deviceChoice2.setOffsetNid(9);
		deviceChoice2.setLabelString("VIK2 Gas: ");
		deviceChoice2.setChoiceItems(new String[]
		{ "H2", "He", "Not Used" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		deviceChoice3.setChoiceIntValues(null);
		deviceChoice3.setChoiceFloatValues(null);
		deviceChoice3.setOffsetNid(11);
		deviceChoice3.setLabelString("Bottle Gas: ");
		deviceChoice3.setChoiceItems(new String[]
		{ "H2", "He", "Ne", "Not Used" });
		deviceChoice3.setUpdateIdentifier("");
		deviceChoice3.setIdentifier("");
		deviceField10.setOffsetNid(6);
		deviceField10.setLabelString("Vacuum lev. (mbar): ");
		deviceField10.setNumCols(6);
		deviceField10.setIdentifier("");
		deviceField10.setEditable(false);
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(deviceField1, null);
		this.getContentPane().add(jPanel2, BorderLayout.CENTER);
		jPanel2.add(jPanel3, null);
		jPanel3.add(deviceField2, null);
		jPanel3.add(deviceField7, null);
		jPanel3.add(deviceField6, null);
		jPanel3.add(deviceField5, null);
		jPanel3.add(deviceField4, null);
		jPanel3.add(deviceField3, null);
		jPanel3.add(deviceField8, null);
		jPanel2.add(jPanel4, null);
		jPanel4.add(deviceField9, null);
		jPanel4.add(deviceField14, null);
		jPanel4.add(deviceField10, null);
		jPanel4.add(deviceChoice1, null);
		jPanel4.add(deviceChoice2, null);
		jPanel4.add(deviceChoice3, null);
	}
}

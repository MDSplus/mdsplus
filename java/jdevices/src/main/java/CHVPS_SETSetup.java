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
public class CHVPS_SETSetup extends DeviceSetup
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
	DeviceField deviceField1 = new DeviceField();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	FlowLayout flowLayout1 = new FlowLayout();
	JPanel jPanel3 = new JPanel();
	DeviceField deviceField2 = new DeviceField();
	FlowLayout flowLayout2 = new FlowLayout();
	DeviceField deviceField3 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	JPanel jPanel4 = new JPanel();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceChoice deviceChoice3 = new DeviceChoice();
	DeviceField deviceField4 = new DeviceField();
	FlowLayout flowLayout3 = new FlowLayout();
	DeviceField deviceField5 = new DeviceField();
	JPanel jPanel5 = new JPanel();
	DeviceField deviceField6 = new DeviceField();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();
	DeviceField deviceField9 = new DeviceField();
	FlowLayout flowLayout4 = new FlowLayout();

	public CHVPS_SETSetup()
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
		this.setWidth(800);
		this.setHeight(240);
		this.setDeviceType("CHVPS_SET");
		this.setDeviceProvider("150.178.3.33");
		this.setDeviceTitle("CHVPS Channel set configuration");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setRows(4);
		deviceField1.setOffsetNid(1);
		deviceField1.setLabelString("Comment :");
		deviceField1.setNumCols(30);
		deviceField1.setIdentifier("");
		jPanel2.setLayout(flowLayout1);
		flowLayout1.setAlignment(FlowLayout.LEFT);
		deviceField2.setOffsetNid(2);
		deviceField2.setLabelString("CHVPS Target : ");
		deviceField2.setNumCols(20);
		deviceField2.setIdentifier("");
		jPanel3.setLayout(flowLayout2);
		flowLayout2.setAlignment(FlowLayout.LEFT);
		deviceField3.setOffsetNid(3);
		deviceField3.setLabelString("Set num.: ");
		deviceField3.setIdentifier("");
		deviceChoice1.setChoiceIntValues(null);
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setOffsetNid(4);
		deviceChoice1.setLabelString("Reset Mode: ");
		deviceChoice1.setChoiceItems(new String[]
		{ "Disable", "Enable" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceChoice2.setChoiceIntValues(null);
		deviceChoice2.setChoiceFloatValues(null);
		deviceChoice2.setOffsetNid(5);
		deviceChoice2.setLabelString("Gate Mode: ");
		deviceChoice2.setChoiceItems(new String[]
		{ "None", "External" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		deviceChoice3.setChoiceIntValues(null);
		deviceChoice3.setChoiceFloatValues(null);
		deviceChoice3.setOffsetNid(6);
		deviceChoice3.setLabelString("Overload Mode:");
		deviceChoice3.setChoiceItems(new String[]
		{ "Disable", "Enable" });
		deviceChoice3.setUpdateIdentifier("");
		deviceChoice3.setIdentifier("");
		deviceField4.setOffsetNid(7);
		deviceField4.setLabelString("Trip Time: ");
		deviceField4.setIdentifier("");
		jPanel4.setLayout(flowLayout3);
		flowLayout3.setAlignment(FlowLayout.LEFT);
		deviceField5.setOffsetNid(10);
		deviceField5.setLabelString("Vmax: ");
		deviceField5.setIdentifier("");
		deviceField6.setOffsetNid(8);
		deviceField6.setLabelString("Voltage: ");
		deviceField6.setIdentifier("");
		deviceField7.setOffsetNid(9);
		deviceField7.setLabelString("Current");
		deviceField7.setIdentifier("");
		deviceField8.setOffsetNid(11);
		deviceField8.setLabelString("Raise Rate: ");
		deviceField8.setIdentifier("");
		deviceField9.setOffsetNid(12);
		deviceField9.setLabelString("Fall Rate: ");
		deviceField9.setIdentifier("");
		jPanel5.setLayout(flowLayout4);
		flowLayout4.setAlignment(FlowLayout.LEFT);
		deviceButtons1.setMethods(new String[]
		{ "init" });
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.CENTER);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceField1, null);
		jPanel2.add(deviceDispatch1, null);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceField2, null);
		jPanel3.add(deviceField3, null);
		jPanel3.add(deviceChoice1, null);
		jPanel1.add(jPanel4, null);
		jPanel4.add(deviceChoice2, null);
		jPanel4.add(deviceChoice3, null);
		jPanel4.add(deviceField4, null);
		jPanel4.add(deviceField5, null);
		jPanel1.add(jPanel5, null);
		jPanel5.add(deviceField6, null);
		jPanel5.add(deviceField7, null);
		jPanel5.add(deviceField8, null);
		jPanel5.add(deviceField9, null);
	}
}

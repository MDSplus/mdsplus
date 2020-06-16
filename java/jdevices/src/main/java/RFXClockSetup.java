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
public class RFXClockSetup extends DeviceSetup
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
	DeviceField deviceField1 = new DeviceField();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	DeviceField deviceField3 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();

	public RFXClockSetup()
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
		this.setWidth(520);
		this.setHeight(250);
		this.setDeviceType("RFXClock");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("Clock generator for MPB timing");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(3);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Comment");
		deviceField1.setNumCols(25);
		deviceField1.setIdentifier("");
		deviceField3.setOffsetNid(2);
		deviceField3.setLabelString("Decoder: ");
		deviceField3.setNumCols(15);
		deviceField3.setIdentifier("");
		deviceChoice1.setChoiceIntValues(new int[]
		{ 0, 1, 2, 3, 4, 5 });
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setOffsetNid(3);
		deviceChoice1.setLabelString("Channel: ");
		deviceChoice1.setChoiceItems(new String[]
		{ "0", "1", "2", "3", "4", "5" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceField4.setOffsetNid(4);
		deviceField4.setLabelString("Frequency (Hz): ");
		deviceField4.setIdentifier("");
		deviceField2.setOffsetNid(5);
		deviceField2.setLabelString("Duty cycle: ");
		deviceField2.setNumCols(5);
		deviceField2.setIdentifier("");
		deviceButtons1.setCheckExpressions(null);
		deviceButtons1.setCheckMessages(null);
		deviceButtons1.setMethods(new String[]
		{ "INIT" });
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.CENTER);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceField1, null);
		jPanel2.add(deviceDispatch1, null);
		jPanel1.add(jPanel4, null);
		jPanel4.add(deviceField3, null);
		jPanel4.add(deviceChoice1, null);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceField4, null);
		jPanel3.add(deviceField2, null);
	}
}

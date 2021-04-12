import java.awt.*;
import javax.swing.*;
import javax.swing.border.*;

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
public class NI6071ESetup extends DeviceSetup
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
	DeviceField deviceField2 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceField deviceField3 = new DeviceField();
	JPanel jPanel5 = new JPanel();
	BorderLayout borderLayout2 = new BorderLayout();
	JPanel jPanel6 = new JPanel();
	JPanel jPanel7 = new JPanel();
	Border border1;
	TitledBorder titledBorder1;
	Border border2;
	TitledBorder titledBorder2;
	GridLayout gridLayout2 = new GridLayout();
	JPanel jPanel8 = new JPanel();
	JPanel jPanel9 = new JPanel();
	DeviceField deviceField4 = new DeviceField();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	GridLayout gridLayout3 = new GridLayout();
	JPanel jPanel10 = new JPanel();
	JPanel jPanel11 = new JPanel();
	JPanel jPanel12 = new JPanel();
	JPanel jPanel13 = new JPanel();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceField deviceField9 = new DeviceField();
	DeviceField deviceField10 = new DeviceField();
	DeviceField deviceField11 = new DeviceField();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();

	public NI6071ESetup()
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
		border1 = BorderFactory.createLineBorder(Color.black, 2);
		titledBorder1 = new TitledBorder(border1, "Immediate Scan");
		border2 = BorderFactory.createLineBorder(Color.black, 2);
		titledBorder2 = new TitledBorder(border2, "Triggered Scan");
		this.setWidth(526);
		this.setHeight(526);
		this.setDeviceType("NI6071E");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("National Instruments ADC 6071E");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(3);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Comment:");
		deviceField1.setNumCols(30);
		deviceField1.setIdentifier("");
		deviceField2.setOffsetNid(2);
		deviceField2.setLabelString("Device ID: ");
		deviceField2.setNumCols(8);
		deviceField2.setIdentifier("");
		deviceField3.setOffsetNid(10);
		deviceField3.setLabelString("Chan. range");
		deviceField3.setIdentifier("");
		deviceChoice1.setChoiceIntValues(null);
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setOffsetNid(11);
		deviceChoice1.setLabelString("Chan. polarity");
		deviceChoice1.setChoiceItems(new String[]
		{ "BIPOLAR", "UNIPOLAR" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		jPanel5.setLayout(borderLayout2);
		jPanel6.setBorder(titledBorder1);
		jPanel6.setLayout(gridLayout2);
		jPanel7.setBorder(titledBorder2);
		jPanel7.setLayout(gridLayout3);
		gridLayout2.setRows(2);
		deviceField5.setOffsetNid(13);
		deviceField5.setLabelString("Scan rate: ");
		deviceField5.setNumCols(6);
		deviceField5.setIdentifier("");
		deviceField4.setOffsetNid(14);
		deviceField4.setLabelString("Num. of scans: ");
		deviceField4.setNumCols(4);
		deviceField4.setIdentifier("");
		deviceField6.setOffsetNid(15);
		deviceField6.setLabelString("Channels: ");
		deviceField6.setNumCols(35);
		deviceField6.setIdentifier("");
		gridLayout3.setColumns(1);
		gridLayout3.setRows(4);
		deviceField7.setOffsetNid(3);
		deviceField7.setLabelString("Trig. Source: ");
		deviceField7.setNumCols(25);
		deviceField7.setIdentifier("");
		deviceChoice2.setChoiceIntValues(null);
		deviceChoice2.setChoiceFloatValues(null);
		deviceChoice2.setOffsetNid(4);
		deviceChoice2.setLabelString("Trig. slope: ");
		deviceChoice2.setChoiceItems(new String[]
		{ "RISING", "FALLING" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		deviceField8.setOffsetNid(5);
		deviceField8.setLabelString("Trig. level: ");
		deviceField8.setNumCols(6);
		deviceField8.setIdentifier("");
		deviceField10.setOffsetNid(6);
		deviceField10.setLabelString("Scan rate: ");
		deviceField10.setNumCols(6);
		deviceField10.setIdentifier("");
		deviceField9.setOffsetNid(7);
		deviceField9.setLabelString("Num. of scans:");
		deviceField9.setNumCols(4);
		deviceField9.setIdentifier("");
		deviceField11.setOffsetNid(8);
		deviceField11.setLabelString("Channels: ");
		deviceField11.setNumCols(35);
		deviceField11.setIdentifier("");
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceField1, null);
		jPanel1.add(jPanel4, null);
		jPanel4.add(deviceField2, null);
		jPanel4.add(deviceDispatch1, null);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceField3, null);
		jPanel3.add(deviceChoice1, null);
		this.getContentPane().add(jPanel5, BorderLayout.CENTER);
		jPanel5.add(jPanel6, BorderLayout.NORTH);
		jPanel6.add(jPanel9, null);
		jPanel9.add(deviceField5, null);
		jPanel9.add(deviceField4, null);
		jPanel6.add(jPanel8, null);
		jPanel8.add(deviceField6, null);
		jPanel5.add(jPanel7, BorderLayout.CENTER);
		jPanel7.add(jPanel13, null);
		jPanel13.add(deviceField7, null);
		jPanel7.add(jPanel12, null);
		jPanel12.add(deviceChoice2, null);
		jPanel12.add(deviceField8, null);
		jPanel7.add(jPanel11, null);
		jPanel11.add(deviceField10, null);
		jPanel11.add(deviceField9, null);
		jPanel7.add(jPanel10, null);
		jPanel10.add(deviceField11, null);
	}
}

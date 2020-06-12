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
public class PXD114Setup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	DeviceButtons deviceButtons1 = new DeviceButtons();
	BorderLayout borderLayout1 = new BorderLayout();
	JPanel jPanel1 = new JPanel();
	JPanel jPanel2 = new JPanel();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel3 = new JPanel();
	JPanel jPanel4 = new JPanel();
	JPanel jPanel5 = new JPanel();
	JPanel jPanel6 = new JPanel();
	JPanel jPanel7 = new JPanel();
	DeviceField deviceField1 = new DeviceField();
	DeviceField deviceField2 = new DeviceField();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceChoice deviceChoice3 = new DeviceChoice();
	DeviceChoice deviceChoice4 = new DeviceChoice();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceField deviceField7 = new DeviceField();
	GridLayout gridLayout2 = new GridLayout();
	JPanel jPanel8 = new JPanel();
	Border border1;
	TitledBorder titledBorder1;
	DeviceField deviceField8 = new DeviceField();
	DeviceChoice deviceChoice5 = new DeviceChoice();
	DeviceChoice deviceChoice6 = new DeviceChoice();
	DeviceChoice deviceChoice7 = new DeviceChoice();
	FlowLayout flowLayout1 = new FlowLayout();
	DeviceChoice deviceChoice8 = new DeviceChoice();
	FlowLayout flowLayout2 = new FlowLayout();
	DeviceField deviceField9 = new DeviceField();
	JPanel jPanel9 = new JPanel();
	DeviceChoice deviceChoice9 = new DeviceChoice();
	DeviceChoice deviceChoice10 = new DeviceChoice();
	DeviceChoice deviceChoice11 = new DeviceChoice();
	FlowLayout flowLayout3 = new FlowLayout();
	DeviceField deviceField10 = new DeviceField();
	JPanel jPanel10 = new JPanel();
	DeviceChoice deviceChoice12 = new DeviceChoice();
	DeviceChoice deviceChoice13 = new DeviceChoice();
	DeviceChoice deviceChoice14 = new DeviceChoice();
	FlowLayout flowLayout4 = new FlowLayout();
	DeviceField deviceField11 = new DeviceField();
	JPanel jPanel11 = new JPanel();
	DeviceChoice deviceChoice15 = new DeviceChoice();
	DeviceChoice deviceChoice16 = new DeviceChoice();
	Border border2;
	TitledBorder titledBorder2;
	Border border3;
	TitledBorder titledBorder3;
	Border border4;
	TitledBorder titledBorder4;
	Border border5;
	TitledBorder titledBorder5;

	public PXD114Setup()
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
		border1 = BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140));
		titledBorder1 = new TitledBorder(BorderFactory.createLineBorder(Color.black, 1), "Channel 4");
		border2 = BorderFactory.createLineBorder(Color.black, 1);
		titledBorder2 = new TitledBorder(border2, "Channel 1");
		border3 = BorderFactory.createLineBorder(Color.black, 1);
		titledBorder3 = new TitledBorder(border3, "Channel 2");
		border4 = BorderFactory.createLineBorder(Color.black, 1);
		titledBorder4 = new TitledBorder(border4, "Channel 3");
		border5 = BorderFactory.createLineBorder(Color.black, 1);
		titledBorder5 = new TitledBorder(border5, "Channel 4");
		this.setWidth(603);
		this.setHeight(603);
		this.setDeviceType("PXD114");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("LeCroy PXD114 Transient Recorder");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(5);
		deviceField1.setOffsetNid(2);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Comment: ");
		deviceField1.setNumCols(35);
		deviceField1.setIdentifier("");
		deviceField2.setOffsetNid(1);
		deviceField2.setTextOnly(true);
		deviceField2.setLabelString("Board Name: ");
		deviceField2.setNumCols(20);
		deviceField2.setIdentifier("");
		deviceChoice3.setChoiceIntValues(null);
		deviceChoice3.setChoiceFloatValues(null);
		deviceChoice3.setOffsetNid(3);
		deviceChoice3.setLabelString("Trig. Source");
		deviceChoice3.setChoiceItems(new String[]
		{ "C1", "C2", "C3", "C4", "EXTERNAL", "EXTERNAL5" });
		deviceChoice3.setUpdateIdentifier("");
		deviceChoice3.setIdentifier("");
		deviceChoice2.setChoiceIntValues(null);
		deviceChoice2.setChoiceFloatValues(null);
		deviceChoice2.setOffsetNid(6);
		deviceChoice2.setLabelString("Trig. Coupling: ");
		deviceChoice2.setChoiceItems(new String[]
		{ "DC", "AC", "GND" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		deviceChoice1.setChoiceIntValues(new int[]
		{ 50, 1000000 });
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setConvert(true);
		deviceChoice1.setOffsetNid(7);
		deviceChoice1.setLabelString("Trig. Imp. :");
		deviceChoice1.setChoiceItems(new String[]
		{ "50 Ohm", "1MOhm" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceChoice4.setChoiceIntValues(null);
		deviceChoice4.setChoiceFloatValues(null);
		deviceChoice4.setOffsetNid(5);
		deviceChoice4.setLabelString("Trig.  Edge:");
		deviceChoice4.setChoiceItems(new String[]
		{ "POSITIVE", "NEGATIVE" });
		deviceChoice4.setUpdateIdentifier("");
		deviceChoice4.setIdentifier("");
		deviceField3.setOffsetNid(4);
		deviceField3.setLabelString("Trig. Level (V):");
		deviceField3.setNumCols(6);
		deviceField3.setIdentifier("");
		deviceField4.setOffsetNid(8);
		deviceField4.setLabelString("Trig. Time (s): ");
		deviceField4.setIdentifier("");
		deviceField7.setOffsetNid(9);
		deviceField7.setLabelString("Num. Segments:");
		deviceField7.setNumCols(5);
		deviceField7.setIdentifier("");
		deviceField6.setOffsetNid(10);
		deviceField6.setLabelString("Segment time (s)");
		deviceField6.setNumCols(6);
		deviceField6.setIdentifier("");
		deviceField5.setOffsetNid(11);
		deviceField5.setLabelString("Segment start (s):");
		deviceField5.setNumCols(6);
		deviceField5.setIdentifier("");
		jPanel2.setLayout(gridLayout2);
		gridLayout2.setColumns(1);
		gridLayout2.setRows(4);
		jPanel8.setBorder(titledBorder2);
		jPanel8.setLayout(flowLayout1);
		deviceChoice7.setChoiceIntValues(new int[]
		{ 50, 1000000 });
		deviceChoice7.setChoiceFloatValues(null);
		deviceChoice7.setConvert(true);
		deviceChoice7.setOffsetNid(15);
		deviceChoice7.setLabelString("Impedance: ");
		deviceChoice7.setChoiceItems(new String[]
		{ "50 Ohm", "1 MOhm" });
		deviceChoice7.setUpdateIdentifier("");
		deviceChoice7.setIdentifier("");
		deviceChoice6.setChoiceIntValues(null);
		deviceChoice6.setChoiceFloatValues(new float[]
		{ (float) 0.04, (float) 0.08, (float) 0.16, (float) 0.4, (float) 0.8, (float) 1.6, (float) 4.0, (float) 8.0 });
		deviceChoice6.setOffsetNid(16);
		deviceChoice6.setLabelString("Range (V):");
		deviceChoice6.setChoiceItems(new String[]
		{ "0.04", "0.08", "0.16", "0.4", "0.8", "1.6", "4", "8" });
		deviceChoice6.setUpdateIdentifier("");
		deviceChoice6.setIdentifier("");
		deviceChoice5.setChoiceIntValues(null);
		deviceChoice5.setChoiceFloatValues(null);
		deviceChoice5.setOffsetNid(18);
		deviceChoice5.setLabelString("Coupling:");
		deviceChoice5.setChoiceItems(new String[]
		{ "DC", "AC", "GND" });
		deviceChoice5.setUpdateIdentifier("");
		deviceChoice5.setIdentifier("");
		deviceField8.setOffsetNid(17);
		deviceField8.setLabelString("Offset(V):");
		deviceField8.setNumCols(8);
		deviceField8.setIdentifier("");
		deviceChoice8.setIdentifier("");
		deviceChoice8.setUpdateIdentifier("");
		deviceChoice8.setChoiceItems(new String[]
		{ "0.04", "0.08", "0.16", "0.4", "0.8", "1.6", "4", "8" });
		deviceChoice8.setLabelString("Range (V):");
		deviceChoice8.setOffsetNid(34);
		deviceChoice8.setChoiceFloatValues(new float[]
		{ (float) 0.04, (float) 0.08, (float) 0.16, (float) 0.4, (float) 0.8, (float) 1.6, (float) 4.0, (float) 8.0 });
		deviceChoice8.setChoiceIntValues(null);
		deviceField9.setIdentifier("");
		deviceField9.setNumCols(8);
		deviceField9.setLabelString("Offset(V):");
		deviceField9.setOffsetNid(35);
		jPanel9.setLayout(flowLayout2);
		jPanel9.setBorder(titledBorder5);
		deviceChoice9.setChoiceIntValues(null);
		deviceChoice9.setChoiceFloatValues(null);
		deviceChoice9.setOffsetNid(36);
		deviceChoice9.setLabelString("Coupling:");
		deviceChoice9.setChoiceItems(new String[]
		{ "DC", "AC", "GND" });
		deviceChoice9.setUpdateIdentifier("");
		deviceChoice9.setIdentifier("");
		deviceChoice10.setChoiceIntValues(new int[]
		{ 50, 1000000 });
		deviceChoice10.setChoiceFloatValues(null);
		deviceChoice10.setConvert(true);
		deviceChoice10.setOffsetNid(33);
		deviceChoice10.setLabelString("Impedance: ");
		deviceChoice10.setChoiceItems(new String[]
		{ "50 Ohm", "1 MOhm" });
		deviceChoice10.setUpdateIdentifier("");
		deviceChoice10.setIdentifier("");
		deviceChoice11.setIdentifier("");
		deviceChoice11.setUpdateIdentifier("");
		deviceChoice11.setChoiceItems(new String[]
		{ "0.04", "0.08", "0.16", "0.4", "0.8", "1.6", "4", "8" });
		deviceChoice11.setLabelString("Range (V):");
		deviceChoice11.setOffsetNid(28);
		deviceChoice11.setChoiceFloatValues(new float[]
		{ (float) 0.04, (float) 0.08, (float) 0.16, (float) 0.4, (float) 0.8, (float) 1.6, (float) 4.0, (float) 8.0 });
		deviceChoice11.setChoiceIntValues(null);
		deviceField10.setIdentifier("");
		deviceField10.setNumCols(8);
		deviceField10.setLabelString("Offset(V):");
		deviceField10.setOffsetNid(29);
		jPanel10.setLayout(flowLayout3);
		jPanel10.setBorder(titledBorder4);
		deviceChoice12.setChoiceIntValues(null);
		deviceChoice12.setChoiceFloatValues(null);
		deviceChoice12.setOffsetNid(30);
		deviceChoice12.setLabelString("Coupling:");
		deviceChoice12.setChoiceItems(new String[]
		{ "DC", "AC", "GND" });
		deviceChoice12.setUpdateIdentifier("");
		deviceChoice12.setIdentifier("");
		deviceChoice13.setChoiceIntValues(new int[]
		{ 50, 1000000 });
		deviceChoice13.setChoiceFloatValues(null);
		deviceChoice13.setConvert(true);
		deviceChoice13.setOffsetNid(27);
		deviceChoice13.setLabelString("Impedance: ");
		deviceChoice13.setChoiceItems(new String[]
		{ "50 Ohm", "1 MOhm" });
		deviceChoice13.setUpdateIdentifier("");
		deviceChoice13.setIdentifier("");
		deviceChoice14.setIdentifier("");
		deviceChoice14.setUpdateIdentifier("");
		deviceChoice14.setChoiceItems(new String[]
		{ "0.04", "0.08", "0.16", "0.4", "0.8", "1.6", "4", "8" });
		deviceChoice14.setLabelString("Range (V):");
		deviceChoice14.setOffsetNid(22);
		deviceChoice14.setChoiceFloatValues(new float[]
		{ (float) 0.04, (float) 0.08, (float) 0.16, (float) 0.4, (float) 0.8, (float) 1.6, (float) 4.0, (float) 8.0 });
		deviceChoice14.setChoiceIntValues(null);
		deviceField11.setIdentifier("");
		deviceField11.setNumCols(8);
		deviceField11.setLabelString("Offset(V):");
		deviceField11.setOffsetNid(23);
		jPanel11.setLayout(flowLayout4);
		jPanel11.setBorder(titledBorder3);
		deviceChoice15.setChoiceIntValues(null);
		deviceChoice15.setChoiceFloatValues(null);
		deviceChoice15.setOffsetNid(24);
		deviceChoice15.setLabelString("Coupling:");
		deviceChoice15.setChoiceItems(new String[]
		{ "DC", "AC", "GND" });
		deviceChoice15.setUpdateIdentifier("");
		deviceChoice15.setIdentifier("");
		deviceChoice16.setChoiceIntValues(new int[]
		{ 50, 1000000 });
		deviceChoice16.setChoiceFloatValues(null);
		deviceChoice16.setConvert(true);
		deviceChoice16.setOffsetNid(21);
		deviceChoice16.setLabelString("Impedance: ");
		deviceChoice16.setChoiceItems(new String[]
		{ "50 Ohm", "1 MOhm" });
		deviceChoice16.setUpdateIdentifier("");
		deviceChoice16.setIdentifier("");
		deviceButtons1.setCheckExpressions(null);
		deviceButtons1.setCheckMessages(null);
		deviceButtons1.setMethods(new String[]
		{ "INIT", "STORE" });
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceField1, null);
		jPanel1.add(jPanel7, null);
		jPanel7.add(deviceField2, null);
		jPanel7.add(deviceDispatch1, null);
		jPanel1.add(jPanel6, null);
		jPanel6.add(deviceChoice3, null);
		jPanel6.add(deviceChoice2, null);
		jPanel6.add(deviceChoice1, null);
		jPanel1.add(jPanel5, null);
		jPanel5.add(deviceChoice4, null);
		jPanel5.add(deviceField3, null);
		jPanel5.add(deviceField4, null);
		jPanel1.add(jPanel4, null);
		jPanel4.add(deviceField7, null);
		jPanel4.add(deviceField6, null);
		jPanel4.add(deviceField5, null);
		this.getContentPane().add(jPanel2, BorderLayout.CENTER);
		jPanel2.add(jPanel8, null);
		jPanel8.add(deviceChoice7, null);
		jPanel8.add(deviceChoice6, null);
		jPanel8.add(deviceChoice5, null);
		jPanel8.add(deviceField8, null);
		jPanel2.add(jPanel11, null);
		jPanel11.add(deviceChoice16, null);
		jPanel11.add(deviceChoice14, null);
		jPanel11.add(deviceChoice15, null);
		jPanel11.add(deviceField11, null);
		jPanel2.add(jPanel10, null);
		jPanel10.add(deviceChoice13, null);
		jPanel10.add(deviceChoice11, null);
		jPanel10.add(deviceChoice12, null);
		jPanel10.add(deviceField10, null);
		jPanel2.add(jPanel9, null);
		jPanel9.add(deviceChoice10, null);
		jPanel9.add(deviceChoice8, null);
		jPanel9.add(deviceChoice9, null);
		jPanel9.add(deviceField9, null);
	}
}

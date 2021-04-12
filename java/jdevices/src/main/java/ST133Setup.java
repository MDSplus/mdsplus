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
public class ST133Setup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel1 = new JPanel();
	JPanel jPanel2 = new JPanel();
	GridLayout gridLayout1 = new GridLayout();
	FlowLayout flowLayout1 = new FlowLayout();
	FlowLayout flowLayout2 = new FlowLayout();
	JPanel jPanel3 = new JPanel();
	FlowLayout flowLayout3 = new FlowLayout();
	JPanel jPanel4 = new JPanel();
	FlowLayout flowLayout4 = new FlowLayout();
	JPanel jPanel5 = new JPanel();
	DeviceField deviceField1 = new DeviceField();
	DeviceField deviceField2 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	DeviceField deviceField5 = new DeviceField();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	DeviceField deviceField6 = new DeviceField();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();
	DeviceTable deviceTable1 = new DeviceTable();

	public ST133Setup()
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
		this.setWidth(700);
		this.setHeight(450);
		this.setDeviceType("ST133");
		this.setDeviceProvider("150.178.3.33");
		this.setDeviceTitle("ST133 CCD Controller");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(4);
		jPanel2.setLayout(flowLayout1);
		flowLayout1.setAlignment(FlowLayout.LEFT);
		flowLayout1.setHgap(5);
		flowLayout1.setVgap(0);
		flowLayout2.setVgap(0);
		flowLayout2.setHgap(5);
		flowLayout2.setAlignment(FlowLayout.LEFT);
		jPanel3.setLayout(flowLayout2);
		flowLayout3.setVgap(0);
		flowLayout3.setHgap(5);
		flowLayout3.setAlignment(FlowLayout.LEFT);
		jPanel4.setLayout(flowLayout3);
		flowLayout4.setVgap(0);
		flowLayout4.setHgap(5);
		flowLayout4.setAlignment(FlowLayout.LEFT);
		jPanel5.setLayout(flowLayout4);
		deviceField1.setOffsetNid(1);
		deviceField1.setLabelString("Comment: ");
		deviceField1.setNumCols(30);
		deviceField1.setIdentifier("");
		deviceField2.setOffsetNid(2);
		deviceField2.setTextOnly(true);
		deviceField2.setLabelString("Ip Address: ");
		deviceField2.setIdentifier("");
		deviceChoice1.setChoiceIntValues(null);
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setOffsetNid(3);
		deviceChoice1.setLabelString("Clock Mode: ");
		deviceChoice1.setChoiceItems(new String[]
		{ "INTERNAL", "EXTERNAL" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceField3.setOffsetNid(4);
		deviceField3.setLabelString("Clock. Source: ");
		deviceField3.setNumCols(30);
		deviceField3.setIdentifier("");
		deviceField4.setOffsetNid(14);
		deviceField4.setLabelString("Exp. Time: ");
		deviceField4.setNumCols(15);
		deviceField4.setIdentifier("");
		deviceField5.setOffsetNid(18);
		deviceField5.setLabelString("Num. Frames:");
		deviceField5.setIdentifier("");
		deviceButtons1.setMethods(new String[]
		{ "init", "store" });
		deviceField6.setOffsetNid(16);
		deviceField6.setLabelString("Cleans: ");
		deviceField6.setIdentifier("");
		deviceField7.setOffsetNid(17);
		deviceField7.setLabelString("Skip Cleans: ");
		deviceField7.setIdentifier("");
		deviceField8.setOffsetNid(19);
		deviceField8.setLabelString("Num. ROI");
		deviceField8.setIdentifier("");
		deviceTable1.setOffsetNid(20);
		deviceTable1.setNumCols(6);
		deviceTable1.setNumRows(10);
		deviceTable1.setIdentifier("");
		deviceTable1.setColumnNames(new String[]
		{ "StartX", "EndX", "GroupX", "StartY", "EndY", "GroupY" });
		deviceTable1.setRowNames(null);
		deviceTable1.setDisplayRowNumber(true);
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceField1, null);
		jPanel2.add(deviceField2, null);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceChoice1, null);
		jPanel3.add(deviceField3, null);
		jPanel1.add(jPanel4, null);
		jPanel4.add(deviceField5, null);
		jPanel4.add(deviceField4, null);
		jPanel4.add(deviceDispatch1, null);
		jPanel1.add(jPanel5, null);
		jPanel5.add(deviceField6, null);
		jPanel5.add(deviceField7, null);
		jPanel5.add(deviceField8, null);
		this.getContentPane().add(deviceTable1, BorderLayout.CENTER);
	}
}

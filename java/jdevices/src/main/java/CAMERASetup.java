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
public class CAMERASetup extends DeviceSetup
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
	GridLayout gridLayout2 = new GridLayout();
	JPanel jPanel4 = new JPanel();
	JPanel jPanel5 = new JPanel();
	JPanel jPanel6 = new JPanel();
	JPanel jPanel7 = new JPanel();
	FlowLayout flowLayout1 = new FlowLayout();
	FlowLayout flowLayout2 = new FlowLayout();
	FlowLayout flowLayout3 = new FlowLayout();
	DeviceField deviceField1 = new DeviceField();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceField deviceField7 = new DeviceField();
	FlowLayout flowLayout4 = new FlowLayout();
	Border border1;
	TitledBorder titledBorder1;
	GridLayout gridLayout3 = new GridLayout();
	FlowLayout flowLayout6 = new FlowLayout();
	JPanel jPanel10 = new JPanel();
	FlowLayout flowLayout7 = new FlowLayout();
	JPanel jPanel11 = new JPanel();
	FlowLayout flowLayout8 = new FlowLayout();
	JPanel jPanel12 = new JPanel();
	FlowLayout flowLayout9 = new FlowLayout();
	JPanel jPanel13 = new JPanel();
	FlowLayout flowLayout10 = new FlowLayout();
	JPanel jPanel14 = new JPanel();
	FlowLayout flowLayout11 = new FlowLayout();
	JPanel jPanel15 = new JPanel();
	FlowLayout flowLayout12 = new FlowLayout();
	JPanel jPanel16 = new JPanel();
	FlowLayout flowLayout13 = new FlowLayout();
	JPanel jPanel17 = new JPanel();
	FlowLayout flowLayout14 = new FlowLayout();
	JPanel jPanel18 = new JPanel();
	FlowLayout flowLayout15 = new FlowLayout();
	JPanel jPanel19 = new JPanel();
	DeviceField deviceField8 = new DeviceField();
	DeviceField deviceField9 = new DeviceField();
	DeviceField deviceField10 = new DeviceField();
	DeviceField deviceField11 = new DeviceField();
	DeviceField deviceField12 = new DeviceField();
	DeviceField deviceField13 = new DeviceField();
	DeviceField deviceField14 = new DeviceField();
	DeviceField deviceField15 = new DeviceField();
	DeviceField deviceField16 = new DeviceField();
	DeviceField deviceField17 = new DeviceField();

	public CAMERASetup()
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
		titledBorder1 = new TitledBorder(border1, "Camera Info");
		this.setWidth(700);
		this.setHeight(380);
		this.setDeviceType("CAMERA");
		this.setDeviceProvider("150.178.3.33");
		this.setDeviceTitle("CAMERA");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setRows(2);
		jPanel2.setLayout(gridLayout2);
		gridLayout2.setRows(4);
		jPanel4.setLayout(flowLayout1);
		flowLayout1.setAlignment(FlowLayout.LEFT);
		flowLayout1.setHgap(0);
		flowLayout1.setVgap(0);
		jPanel5.setLayout(flowLayout2);
		flowLayout2.setAlignment(FlowLayout.LEFT);
		flowLayout2.setHgap(0);
		flowLayout2.setVgap(0);
		jPanel6.setLayout(flowLayout3);
		flowLayout3.setAlignment(FlowLayout.LEFT);
		flowLayout3.setHgap(0);
		flowLayout3.setVgap(0);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Comment: ");
		deviceField1.setNumCols(35);
		deviceField1.setIdentifier("");
		deviceField2.setOffsetNid(2);
		deviceField2.setLabelString("Name: ");
		deviceField2.setIdentifier("");
		deviceField3.setOffsetNid(3);
		deviceField3.setLabelString("Ip Address: ");
		deviceField3.setNumCols(15);
		deviceField3.setIdentifier("");
		deviceField4.setOffsetNid(4);
		deviceField4.setLabelString("Port: ");
		deviceField4.setIdentifier("");
		deviceChoice1.setChoiceIntValues(null);
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setOffsetNid(5);
		deviceChoice1.setLabelString("Trig Mode:");
		deviceChoice1.setChoiceItems(new String[]
		{ "INTERNAL", "EXTERNAL" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceField5.setOffsetNid(6);
		deviceField5.setLabelString("Trig. Source: ");
		deviceField5.setNumCols(30);
		deviceField5.setIdentifier("");
		jPanel7.setLayout(flowLayout4);
		flowLayout4.setAlignment(FlowLayout.LEFT);
		flowLayout4.setHgap(0);
		flowLayout4.setVgap(0);
		deviceField7.setOffsetNid(7);
		deviceField7.setLabelString("Num. Frame");
		deviceField7.setNumCols(15);
		deviceField7.setIdentifier("");
		deviceField6.setOffsetNid(18);
		deviceField6.setLabelString("Frame rate (f.p.s.)");
		deviceField6.setNumCols(15);
		deviceField6.setIdentifier("");
		jPanel3.setBorder(titledBorder1);
		jPanel3.setLayout(gridLayout3);
		deviceButtons1.setMethods(new String[]
		{ "Init", "Arm", "Store" });
		gridLayout3.setColumns(2);
		gridLayout3.setRows(5);
		flowLayout6.setVgap(0);
		flowLayout6.setHgap(0);
		flowLayout6.setAlignment(FlowLayout.LEFT);
		jPanel10.setLayout(flowLayout6);
		flowLayout7.setVgap(0);
		flowLayout7.setHgap(0);
		flowLayout7.setAlignment(FlowLayout.LEFT);
		jPanel11.setLayout(flowLayout7);
		flowLayout8.setVgap(0);
		flowLayout8.setHgap(0);
		flowLayout8.setAlignment(FlowLayout.LEFT);
		jPanel12.setLayout(flowLayout8);
		flowLayout9.setVgap(0);
		flowLayout9.setHgap(0);
		flowLayout9.setAlignment(FlowLayout.LEFT);
		jPanel13.setLayout(flowLayout9);
		flowLayout10.setVgap(0);
		flowLayout10.setHgap(0);
		flowLayout10.setAlignment(FlowLayout.LEFT);
		jPanel14.setLayout(flowLayout10);
		flowLayout11.setVgap(0);
		flowLayout11.setHgap(0);
		flowLayout11.setAlignment(FlowLayout.LEFT);
		jPanel15.setLayout(flowLayout11);
		flowLayout12.setVgap(0);
		flowLayout12.setHgap(0);
		flowLayout12.setAlignment(FlowLayout.LEFT);
		jPanel16.setLayout(flowLayout12);
		flowLayout13.setVgap(0);
		flowLayout13.setHgap(0);
		flowLayout13.setAlignment(FlowLayout.LEFT);
		jPanel17.setLayout(flowLayout13);
		flowLayout14.setVgap(0);
		flowLayout14.setHgap(0);
		flowLayout14.setAlignment(FlowLayout.LEFT);
		jPanel18.setLayout(flowLayout14);
		flowLayout15.setVgap(0);
		flowLayout15.setHgap(0);
		flowLayout15.setAlignment(FlowLayout.LEFT);
		jPanel19.setLayout(flowLayout15);
		deviceField8.setOffsetNid(8);
		deviceField8.setTextOnly(true);
		deviceField8.setLabelString("Model: ");
		deviceField8.setNumCols(20);
		deviceField8.setIdentifier("");
		deviceField9.setOffsetNid(9);
		deviceField9.setTextOnly(true);
		deviceField9.setLabelString("Lens Type: ");
		deviceField9.setNumCols(20);
		deviceField9.setIdentifier("");
		deviceField10.setOffsetNid(10);
		deviceField10.setTextOnly(false);
		deviceField10.setLabelString("Aperture: ");
		deviceField10.setNumCols(20);
		deviceField10.setIdentifier("");
		deviceField11.setOffsetNid(11);
		deviceField11.setLabelString("F Distance: ");
		deviceField11.setNumCols(20);
		deviceField11.setIdentifier("");
		deviceField12.setOffsetNid(12);
		deviceField12.setTextOnly(true);
		deviceField12.setLabelString("Filter: ");
		deviceField12.setNumCols(20);
		deviceField12.setIdentifier("");
		deviceField13.setOffsetNid(13);
		deviceField13.setTextOnly(true);
		deviceField13.setLabelString("Shutter: ");
		deviceField13.setNumCols(20);
		deviceField13.setIdentifier("");
		deviceField14.setOffsetNid(14);
		deviceField14.setTextOnly(true);
		deviceField14.setLabelString("Tor. Pos. :");
		deviceField14.setNumCols(20);
		deviceField14.setIdentifier("");
		deviceField15.setOffsetNid(15);
		deviceField15.setTextOnly(true);
		deviceField15.setLabelString("Pol. Pos. :");
		deviceField15.setNumCols(20);
		deviceField15.setIdentifier("");
		deviceField16.setOffsetNid(16);
		deviceField16.setTextOnly(true);
		deviceField16.setLabelString("Target Zone :");
		deviceField16.setNumCols(20);
		deviceField16.setIdentifier("");
		deviceField17.setOffsetNid(17);
		deviceField17.setTextOnly(true);
		deviceField17.setLabelString("Pixel Frame: ");
		deviceField17.setNumCols(20);
		deviceField17.setIdentifier("");
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.CENTER);
		jPanel1.add(jPanel2, null);
		jPanel2.add(jPanel4, null);
		jPanel4.add(deviceField1, null);
		jPanel4.add(deviceDispatch1, null);
		jPanel2.add(jPanel5, null);
		jPanel5.add(deviceField2, null);
		jPanel5.add(deviceField3, null);
		jPanel5.add(deviceField4, null);
		jPanel2.add(jPanel6, null);
		jPanel6.add(deviceChoice1, null);
		jPanel6.add(deviceField5, null);
		jPanel2.add(jPanel7, null);
		jPanel7.add(deviceField7, null);
		jPanel7.add(deviceField6, null);
		jPanel1.add(jPanel3, null);
		jPanel3.add(jPanel19, null);
		jPanel19.add(deviceField8, null);
		jPanel3.add(jPanel18, null);
		jPanel18.add(deviceField9, null);
		jPanel3.add(jPanel17, null);
		jPanel17.add(deviceField10, null);
		jPanel3.add(jPanel16, null);
		jPanel16.add(deviceField11, null);
		jPanel3.add(jPanel15, null);
		jPanel15.add(deviceField12, null);
		jPanel3.add(jPanel14, null);
		jPanel14.add(deviceField13, null);
		jPanel3.add(jPanel13, null);
		jPanel13.add(deviceField14, null);
		jPanel3.add(jPanel12, null);
		jPanel12.add(deviceField15, null);
		jPanel3.add(jPanel11, null);
		jPanel11.add(deviceField16, null);
		jPanel3.add(jPanel10, null);
		jPanel10.add(deviceField17, null);
	}
}

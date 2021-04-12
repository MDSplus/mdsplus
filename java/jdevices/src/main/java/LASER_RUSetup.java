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
public class LASER_RUSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	JPanel jPanel1 = new JPanel();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel2 = new JPanel();
	FlowLayout flowLayout1 = new FlowLayout();
	DeviceField deviceField1 = new DeviceField();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	FlowLayout flowLayout2 = new FlowLayout();
	JPanel jPanel3 = new JPanel();
	DeviceField deviceField3 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceField deviceField4 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	JPanel jPanel4 = new JPanel();
	JPanel jPanel5 = new JPanel();
	JPanel jPanel6 = new JPanel();
	GridLayout gridLayout2 = new GridLayout();
	GridLayout gridLayout3 = new GridLayout();
	GridLayout gridLayout4 = new GridLayout();
	Border border1;
	TitledBorder titledBorder1;
	Border border2;
	TitledBorder titledBorder2;
	JPanel jPanel7 = new JPanel();
	JPanel jPanel8 = new JPanel();
	DeviceField deviceField2 = new DeviceField();
	FlowLayout flowLayout3 = new FlowLayout();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();
	FlowLayout flowLayout4 = new FlowLayout();
	DeviceField deviceField9 = new DeviceField();
	DeviceField deviceField10 = new DeviceField();
	DeviceField deviceField11 = new DeviceField();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel9 = new JPanel();
	JPanel jPanel10 = new JPanel();
	FlowLayout flowLayout5 = new FlowLayout();
	FlowLayout flowLayout6 = new FlowLayout();
	DeviceField deviceField12 = new DeviceField();
	DeviceField deviceField13 = new DeviceField();
	DeviceField deviceField14 = new DeviceField();
	DeviceField deviceField15 = new DeviceField();
	DeviceField deviceField16 = new DeviceField();
	DeviceField deviceField17 = new DeviceField();
	DeviceField deviceField18 = new DeviceField();
	DeviceChoice deviceChoice2 = new DeviceChoice();

	public LASER_RUSetup()
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
		titledBorder1 = new TitledBorder(border1, "Voltages [V]");
		border2 = BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140));
		titledBorder2 = new TitledBorder(border2, "Delay Times [s]");
		this.setWidth(770);
		this.setHeight(300);
		this.setDeviceType("LASER_RU");
		this.setDeviceProvider("150.178.3.33");
		this.setDeviceTitle("Ruby Laser");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setRows(2);
		gridLayout1.setVgap(0);
		jPanel2.setLayout(flowLayout1);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Comment:");
		deviceField1.setNumCols(20);
		deviceField1.setIdentifier("");
		flowLayout1.setAlignment(FlowLayout.LEFT);
		flowLayout1.setVgap(0);
		flowLayout2.setAlignment(FlowLayout.LEFT);
		flowLayout2.setHgap(0);
		flowLayout2.setVgap(0);
		jPanel3.setLayout(flowLayout2);
		deviceField3.setOffsetNid(4);
		deviceField3.setTextOnly(true);
		deviceField3.setLabelString("RS232 Port:");
		deviceField3.setNumCols(8);
		deviceField3.setIdentifier("");
		deviceChoice1.setChoiceIntValues(null);
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setOffsetNid(5);
		deviceChoice1.setLabelString("Trig. Mode:");
		deviceChoice1.setChoiceItems(new String[]
		{ "EXTERNAL", "INTERNAL", "AUTOMATIC", "MANUAL" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceField4.setOffsetNid(6);
		deviceField4.setLabelString("Num. Shots :");
		deviceField4.setNumCols(5);
		deviceField4.setIdentifier("");
		deviceField6.setOffsetNid(7);
		deviceField6.setLabelString("Energy:");
		deviceField6.setNumCols(8);
		deviceField6.setIdentifier("");
		jPanel6.setLayout(gridLayout2);
		jPanel4.setLayout(gridLayout3);
		gridLayout3.setRows(2);
		gridLayout2.setRows(2);
		jPanel5.setLayout(gridLayout4);
		gridLayout4.setRows(2);
		jPanel6.setBorder(titledBorder1);
		jPanel5.setBorder(titledBorder2);
		jPanel5.setDebugGraphicsOptions(0);
		deviceField2.setOffsetNid(8);
		deviceField2.setLabelString("Osc. :");
		deviceField2.setIdentifier("");
		jPanel8.setLayout(flowLayout3);
		flowLayout3.setAlignment(FlowLayout.CENTER);
		flowLayout3.setHgap(0);
		flowLayout3.setVgap(0);
		deviceField5.setOffsetNid(9);
		deviceField5.setLabelString("Amp. 1:");
		deviceField5.setIdentifier("");
		deviceField7.setOffsetNid(10);
		deviceField7.setLabelString("Amp. 2:");
		deviceField7.setIdentifier("");
		deviceField8.setOffsetNid(11);
		deviceField8.setLabelString("Amp. 3:");
		deviceField8.setIdentifier("");
		jPanel7.setLayout(flowLayout4);
		flowLayout4.setAlignment(FlowLayout.CENTER);
		flowLayout4.setHgap(0);
		flowLayout4.setVgap(0);
		deviceField9.setOffsetNid(12);
		deviceField9.setLabelString("Pockel 1:");
		deviceField9.setIdentifier("");
		deviceField10.setOffsetNid(13);
		deviceField10.setLabelString("Pockel 2:");
		deviceField10.setIdentifier("");
		deviceField11.setOffsetNid(14);
		deviceField11.setLabelString("Pockel 3:");
		deviceField11.setIdentifier("");
		jPanel10.setLayout(flowLayout5);
		jPanel9.setLayout(flowLayout6);
		deviceField12.setOffsetNid(20);
		deviceField12.setLabelString("Delay fire:");
		deviceField12.setIdentifier("");
		deviceField13.setOffsetNid(15);
		deviceField13.setLabelString("Delay amp.:");
		deviceField13.setIdentifier("");
		flowLayout5.setHgap(0);
		flowLayout5.setVgap(0);
		flowLayout6.setHgap(0);
		flowLayout6.setVgap(0);
		deviceField14.setOffsetNid(16);
		deviceField14.setLabelString("Pulse 1:");
		deviceField14.setIdentifier("");
		deviceField15.setOffsetNid(17);
		deviceField15.setLabelString("Pulse 2:");
		deviceField15.setIdentifier("");
		deviceField16.setOffsetNid(18);
		deviceField16.setLabelString("Pulse 3:");
		deviceField16.setIdentifier("");
		deviceField17.setOffsetNid(19);
		deviceField17.setLabelString("Pulse 4:");
		deviceField17.setIdentifier("");
		deviceButtons1.setMethods(new String[]
		{ "init", "trigger", "dump", "reset", "check" });
		deviceDispatch1.setToolTipText("");
		deviceDispatch1.setUpdateIdentifier("");
		deviceField18.setOffsetNid(3);
		deviceField18.setLabelString("IP address: ");
		deviceField18.setIdentifier("");
		deviceChoice2.setChoiceIntValues(null);
		deviceChoice2.setChoiceFloatValues(null);
		deviceChoice2.setConvert(false);
		deviceChoice2.setOffsetNid(2);
		deviceChoice2.setLabelString("Mode: ");
		deviceChoice2.setChoiceItems(new String[]
		{ "LOCAL", "REMOTE" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceField1, null);
		jPanel2.add(deviceChoice2, null);
		jPanel2.add(deviceField18, null);
		jPanel2.add(deviceDispatch1, null);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceField3, null);
		jPanel3.add(deviceField4, null);
		jPanel3.add(deviceField6, null);
		jPanel3.add(deviceChoice1, null);
		this.getContentPane().add(jPanel4, BorderLayout.CENTER);
		jPanel4.add(jPanel6, null);
		jPanel6.add(jPanel8, null);
		jPanel8.add(deviceField2, null);
		jPanel8.add(deviceField5, null);
		jPanel8.add(deviceField7, null);
		jPanel8.add(deviceField8, null);
		jPanel6.add(jPanel7, null);
		jPanel7.add(deviceField9, null);
		jPanel7.add(deviceField10, null);
		jPanel7.add(deviceField11, null);
		jPanel4.add(jPanel5, null);
		jPanel5.add(jPanel10, null);
		jPanel10.add(deviceField12, null);
		jPanel10.add(deviceField13, null);
		jPanel5.add(jPanel9, null);
		jPanel9.add(deviceField14, null);
		jPanel9.add(deviceField15, null);
		jPanel9.add(deviceField16, null);
		jPanel9.add(deviceField17, null);
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
	}
}

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
public class DEQUSetupSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	JTabbedPane jTabbedPane1 = new JTabbedPane();
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel1 = new JPanel();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel2 = new JPanel();
	JPanel jPanel3 = new JPanel();
	JPanel jPanel4 = new JPanel();
	JPanel jPanel5 = new JPanel();
	JPanel jPanel6 = new JPanel();
	JPanel jPanel7 = new JPanel();
	FlowLayout flowLayout1 = new FlowLayout();
	FlowLayout flowLayout2 = new FlowLayout();
	DeviceField deviceField1 = new DeviceField();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();
	DeviceField deviceField9 = new DeviceField();
	JPanel jPanel8 = new JPanel();
	FlowLayout flowLayout3 = new FlowLayout();
	DeviceField deviceField10 = new DeviceField();
	DeviceField deviceField11 = new DeviceField();
	DeviceField deviceField12 = new DeviceField();
	FlowLayout flowLayout4 = new FlowLayout();
	JPanel jPanel9 = new JPanel();
	JPanel jPanel10 = new JPanel();
	DeviceField deviceField13 = new DeviceField();
	DeviceField deviceField14 = new DeviceField();
	GridLayout gridLayout2 = new GridLayout();
	FlowLayout flowLayout5 = new FlowLayout();
	DeviceField deviceField15 = new DeviceField();
	DeviceField deviceField16 = new DeviceField();
	DeviceField deviceField17 = new DeviceField();
	JPanel jPanel11 = new JPanel();
	GridLayout gridLayout3 = new GridLayout();
	DeviceField deviceField18 = new DeviceField();
	DeviceField deviceField19 = new DeviceField();
	DeviceField deviceField110 = new DeviceField();
	JPanel jPanel12 = new JPanel();
	JPanel jPanel13 = new JPanel();
	JPanel jPanel14 = new JPanel();
	GridLayout gridLayout5 = new GridLayout();
	JPanel jPanel15 = new JPanel();
	JPanel jPanel16 = new JPanel();
	FlowLayout flowLayout6 = new FlowLayout();

	public DEQUSetupSetup()
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
		this.setHeight(250);
		this.setDeviceType("DEQUSetup");
		this.setDeviceProvider("150.178.3.33");
		this.setDeviceTitle("DEQU Acquisition Configuration");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setHgap(0);
		gridLayout1.setRows(3);
		gridLayout1.setVgap(0);
		jPanel5.setLayout(flowLayout1);
		jPanel7.setLayout(flowLayout2);
		flowLayout1.setAlignment(FlowLayout.CENTER);
		flowLayout1.setHgap(5);
		flowLayout2.setAlignment(FlowLayout.CENTER);
		flowLayout2.setHgap(0);
		flowLayout2.setVgap(0);
		deviceField1.setOffsetNid(2);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Event: ");
		deviceField1.setNumCols(15);
		deviceField1.setIdentifier("");
		deviceField1.setShowState(false);
		deviceField1.setEditable(false);
		deviceField2.setOffsetNid(3);
		deviceField2.setLabelString("Delay: ");
		deviceField2.setNumCols(15);
		deviceField2.setIdentifier("");
		deviceField3.setOffsetNid(4);
		deviceField3.setLabelString("Duration: ");
		deviceField3.setNumCols(15);
		deviceField3.setIdentifier("");
		deviceField4.setOffsetNid(5);
		deviceField4.setLabelString("Frequency 1: ");
		deviceField4.setNumCols(15);
		deviceField4.setIdentifier("");
		deviceField5.setOffsetNid(6);
		deviceField5.setLabelString("Frequency 2: ");
		deviceField5.setNumCols(15);
		deviceField5.setIdentifier("");
		deviceField6.setOffsetNid(7);
		deviceField6.setLabelString("Start Acq. :");
		deviceField6.setNumCols(15);
		deviceField6.setIdentifier("");
		deviceField7.setOffsetNid(8);
		deviceField7.setLabelString("Stop Acq: ");
		deviceField7.setNumCols(15);
		deviceField7.setIdentifier("");
		deviceField8.setNumCols(15);
		deviceField8.setIdentifier("");
		deviceField8.setOffsetNid(14);
		deviceField8.setLabelString("Frequency 2: ");
		deviceField9.setNumCols(15);
		deviceField9.setIdentifier("");
		deviceField9.setOffsetNid(16);
		deviceField9.setLabelString("Stop Acq: ");
		flowLayout3.setAlignment(FlowLayout.CENTER);
		flowLayout3.setHgap(0);
		flowLayout3.setVgap(5);
		deviceField10.setOffsetNid(12);
		deviceField10.setLabelString("Duration: ");
		deviceField10.setNumCols(15);
		deviceField10.setIdentifier("");
		deviceField11.setOffsetNid(10);
		deviceField11.setTextOnly(true);
		deviceField11.setLabelString("Event: ");
		deviceField11.setNumCols(15);
		deviceField11.setIdentifier("");
		deviceField11.setShowState(false);
		deviceField11.setEditable(false);
		deviceField12.setOffsetNid(15);
		deviceField12.setLabelString("Start Acq. :");
		deviceField12.setNumCols(15);
		deviceField12.setIdentifier("");
		flowLayout4.setAlignment(FlowLayout.CENTER);
		flowLayout4.setHgap(0);
		flowLayout4.setVgap(0);
		jPanel9.setLayout(flowLayout3);
		jPanel10.setLayout(flowLayout4);
		deviceField13.setOffsetNid(13);
		deviceField13.setLabelString("Frequency 1: ");
		deviceField13.setNumCols(15);
		deviceField13.setIdentifier("");
		deviceField14.setOffsetNid(11);
		deviceField14.setLabelString("Delay: ");
		deviceField14.setNumCols(15);
		deviceField14.setIdentifier("");
		jPanel2.setLayout(gridLayout2);
		gridLayout2.setRows(3);
		flowLayout5.setVgap(0);
		flowLayout5.setHgap(0);
		flowLayout5.setAlignment(FlowLayout.CENTER);
		deviceField15.setNumCols(15);
		deviceField15.setIdentifier("");
		deviceField15.setOffsetNid(23);
		deviceField15.setLabelString("Delay: ");
		deviceField16.setNumCols(15);
		deviceField16.setIdentifier("");
		deviceField16.setOffsetNid(24);
		deviceField16.setLabelString("Duration: ");
		deviceField17.setEditable(false);
		deviceField17.setShowState(false);
		deviceField17.setIdentifier("");
		deviceField17.setNumCols(15);
		deviceField17.setLabelString("Event: ");
		deviceField17.setOffsetNid(22);
		deviceField17.setTextOnly(true);
		jPanel11.setLayout(flowLayout5);
		jPanel4.setLayout(gridLayout3);
		deviceField18.setNumCols(15);
		deviceField18.setIdentifier("");
		deviceField18.setOffsetNid(19);
		deviceField18.setLabelString("Delay: ");
		deviceField19.setNumCols(15);
		deviceField19.setIdentifier("");
		deviceField19.setOffsetNid(20);
		deviceField19.setLabelString("Duration: ");
		deviceField110.setEditable(false);
		deviceField110.setShowState(false);
		deviceField110.setIdentifier("");
		deviceField110.setNumCols(15);
		deviceField110.setLabelString("Event: ");
		deviceField110.setOffsetNid(18);
		deviceField110.setTextOnly(true);
		jPanel12.setLayout(flowLayout6);
		gridLayout3.setRows(3);
		jPanel3.setLayout(gridLayout5);
		gridLayout5.setRows(3);
		flowLayout6.setAlignment(FlowLayout.LEFT);
		this.getContentPane().add(jTabbedPane1, BorderLayout.CENTER);
		jTabbedPane1.add(jPanel1, "TR10 ACQ");
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		jTabbedPane1.add(jPanel2, "TRCH ACQ");
		jPanel8.add(deviceField13, null);
		jPanel8.add(deviceField8, null);
		jPanel2.add(jPanel9, null);
		jPanel9.add(deviceField11, null);
		jPanel9.add(deviceField14, null);
		jPanel9.add(deviceField10, null);
		jPanel10.add(deviceField12, null);
		jPanel10.add(deviceField9, null);
		jPanel2.add(jPanel8, null);
		jPanel2.add(jPanel10, null);
		jTabbedPane1.add(jPanel4, "TRIGGER");
		jPanel4.add(jPanel13, null);
		jPanel4.add(jPanel11, null);
		jPanel11.add(deviceField17, null);
		jPanel11.add(deviceField15, null);
		jPanel11.add(deviceField16, null);
		jPanel4.add(jPanel14, null);
		jTabbedPane1.add(jPanel3, "AUTO ZERO");
		jPanel3.add(jPanel15, null);
		jPanel3.add(jPanel12, null);
		jPanel12.add(deviceField110, null);
		jPanel12.add(deviceField18, null);
		jPanel12.add(deviceField19, null);
		jPanel3.add(jPanel16, null);
		jPanel1.add(jPanel5, null);
		jPanel5.add(deviceField1, null);
		jPanel5.add(deviceField2, null);
		jPanel5.add(deviceField3, null);
		jPanel1.add(jPanel6, null);
		jPanel6.add(deviceField4, null);
		jPanel6.add(deviceField5, null);
		jPanel1.add(jPanel7, null);
		jPanel7.add(deviceField6, null);
		jPanel7.add(deviceField7, null);
	}
}

import javax.swing.*;

import java.awt.*;

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
public class DFLUSetupSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	JTabbedPane jTabbedPane1 = new JTabbedPane();
	JPanel jPanel1 = new JPanel();
	JPanel jPanel2 = new JPanel();
	BorderLayout borderLayout2 = new BorderLayout();
	JTabbedPane jTabbedPane2 = new JTabbedPane();
	JPanel jPanel3 = new JPanel();
	JPanel jPanel4 = new JPanel();
	JPanel jPanel5 = new JPanel();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel6 = new JPanel();
	JPanel jPanel7 = new JPanel();
	JPanel jPanel8 = new JPanel();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	DeviceField deviceField1 = new DeviceField();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceField deviceField7 = new DeviceField();
	GridLayout gridLayout2 = new GridLayout();
	DeviceField deviceField8 = new DeviceField();
	JPanel jPanel9 = new JPanel();
	JPanel jPanel10 = new JPanel();
	JPanel jPanel11 = new JPanel();
	DeviceField deviceField9 = new DeviceField();
	DeviceField deviceField10 = new DeviceField();
	DeviceField deviceField11 = new DeviceField();
	GridLayout gridLayout3 = new GridLayout();
	JPanel jPanel12 = new JPanel();
	JPanel jPanel13 = new JPanel();
	JPanel jPanel14 = new JPanel();
	DeviceField deviceField12 = new DeviceField();
	DeviceField deviceField13 = new DeviceField();
	DeviceField deviceField14 = new DeviceField();
	BorderLayout borderLayout3 = new BorderLayout();
	DeviceField deviceField15 = new DeviceField();
	DeviceField deviceField16 = new DeviceField();
	JPanel jPanel15 = new JPanel();
	JPanel jPanel16 = new JPanel();
	DeviceField deviceField17 = new DeviceField();
	JPanel jPanel17 = new JPanel();
	JPanel jPanel18 = new JPanel();
	GridLayout gridLayout4 = new GridLayout();
	DeviceField deviceField18 = new DeviceField();
	DeviceField deviceField19 = new DeviceField();
	GridLayout gridLayout5 = new GridLayout();
	DeviceField deviceField20 = new DeviceField();
	JPanel jPanel19 = new JPanel();
	JPanel jPanel110 = new JPanel();
	JPanel jPanel111 = new JPanel();
	DeviceField deviceField110 = new DeviceField();
	GridLayout gridLayout6 = new GridLayout();
	JPanel jPanel20 = new JPanel();
	DeviceField deviceField21 = new DeviceField();
	DeviceField deviceField111 = new DeviceField();
	JTabbedPane jTabbedPane3 = new JTabbedPane();
	DeviceField deviceField112 = new DeviceField();
	DeviceField deviceField22 = new DeviceField();
	JPanel jPanel21 = new JPanel();
	JPanel jPanel22 = new JPanel();
	JPanel jPanel112 = new JPanel();
	DeviceField deviceField23 = new DeviceField();
	JPanel jPanel23 = new JPanel();
	DeviceField deviceField24 = new DeviceField();

	public DFLUSetupSetup()
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
		this.setWidth(900);
		this.setHeight(250);
		this.setDeviceType("DFLUSetup");
		this.setDeviceProvider("150.178.3.33");
		this.setDeviceTitle("DFLU Acquisition Configuration");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(borderLayout2);
		jPanel3.setLayout(gridLayout1);
		gridLayout1.setRows(3);
		deviceField1.setOffsetNid(3);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Event: ");
		deviceField1.setNumCols(20);
		deviceField1.setIdentifier("");
		deviceField1.setEditable(false);
		deviceField2.setOffsetNid(4);
		deviceField2.setLabelString("Delay: ");
		deviceField2.setNumCols(20);
		deviceField2.setIdentifier("");
		deviceField3.setOffsetNid(5);
		deviceField3.setLabelString("Duration: ");
		deviceField3.setNumCols(20);
		deviceField3.setIdentifier("");
		deviceField4.setOffsetNid(6);
		deviceField4.setLabelString("Frequency 1: ");
		deviceField4.setNumCols(20);
		deviceField4.setIdentifier("");
		deviceField5.setOffsetNid(7);
		deviceField5.setLabelString("Frequency 2:");
		deviceField5.setNumCols(20);
		deviceField5.setIdentifier("");
		deviceField6.setOffsetNid(8);
		deviceField6.setLabelString("Start Acq. :");
		deviceField6.setNumCols(20);
		deviceField6.setIdentifier("");
		deviceField7.setOffsetNid(9);
		deviceField7.setLabelString("Stop Acq. :");
		deviceField7.setNumCols(20);
		deviceField7.setIdentifier("");
		jPanel4.setLayout(gridLayout2);
		gridLayout2.setRows(3);
		gridLayout2.setVgap(0);
		deviceField9.setOffsetNid(11);
		deviceField9.setTextOnly(true);
		deviceField9.setLabelString("Event: ");
		deviceField9.setNumCols(20);
		deviceField9.setIdentifier("");
		deviceField9.setEditable(false);
		deviceField10.setOffsetNid(12);
		deviceField10.setLabelString("Delay: ");
		deviceField10.setNumCols(20);
		deviceField10.setIdentifier("");
		deviceField11.setOffsetNid(13);
		deviceField11.setLabelString("Duration: ");
		deviceField11.setNumCols(20);
		deviceField11.setIdentifier("");
		jPanel5.setLayout(gridLayout3);
		gridLayout3.setRows(3);
		deviceField14.setOffsetNid(17);
		deviceField14.setLabelString("Duration:");
		deviceField14.setNumCols(20);
		deviceField14.setIdentifier("");
		deviceField12.setOffsetNid(15);
		deviceField12.setTextOnly(true);
		deviceField12.setLabelString("Event: ");
		deviceField12.setNumCols(20);
		deviceField12.setIdentifier("");
		deviceField12.setEditable(false);
		deviceField13.setOffsetNid(16);
		deviceField13.setLabelString("Delay: ");
		deviceField13.setNumCols(20);
		deviceField13.setIdentifier("");
		jPanel2.setLayout(borderLayout3);
		deviceField15.setOffsetNid(34);
		deviceField15.setLabelString("Duration:");
		deviceField15.setNumCols(20);
		deviceField16.setOffsetNid(24);
		deviceField16.setLabelString("Frequency 2:");
		deviceField16.setNumCols(20);
		deviceField16.setIdentifier("");
		jPanel15.setLayout(gridLayout5);
		deviceField17.setOffsetNid(23);
		deviceField17.setLabelString("Frequency 1: ");
		deviceField17.setNumCols(20);
		deviceField17.setIdentifier("");
		jPanel17.setLayout(gridLayout4);
		gridLayout4.setRows(3);
		deviceField18.setOffsetNid(33);
		deviceField18.setLabelString("Delay: ");
		deviceField18.setNumCols(20);
		deviceField18.setIdentifier("");
		deviceField19.setOffsetNid(26);
		deviceField19.setLabelString("Stop Acq. :");
		deviceField19.setNumCols(20);
		deviceField19.setIdentifier("");
		gridLayout5.setRows(3);
		gridLayout5.setVgap(0);
		deviceField20.setOffsetNid(25);
		deviceField20.setLabelString("Start Acq. :");
		deviceField20.setNumCols(20);
		deviceField20.setIdentifier("");
		jPanel19.setLayout(gridLayout6);
		deviceField110.setOffsetNid(29);
		deviceField110.setLabelString("Delay: ");
		deviceField110.setNumCols(20);
		deviceField110.setIdentifier("");
		gridLayout6.setRows(3);
		deviceField21.setOffsetNid(22);
		deviceField21.setLabelString("Duration: ");
		deviceField21.setNumCols(20);
		deviceField21.setIdentifier("");
		deviceField111.setOffsetNid(30);
		deviceField111.setLabelString("Duration: ");
		deviceField111.setNumCols(20);
		deviceField111.setIdentifier("");
		deviceField112.setOffsetNid(32);
		deviceField112.setTextOnly(true);
		deviceField112.setLabelString("Event: ");
		deviceField112.setNumCols(20);
		deviceField112.setIdentifier("");
		deviceField112.setEditable(false);
		deviceField22.setOffsetNid(28);
		deviceField22.setTextOnly(true);
		deviceField22.setLabelString("Event: ");
		deviceField22.setNumCols(20);
		deviceField22.setIdentifier("");
		deviceField22.setEditable(false);
		deviceField23.setOffsetNid(20);
		deviceField23.setTextOnly(true);
		deviceField23.setLabelString("Event: ");
		deviceField23.setNumCols(20);
		deviceField23.setIdentifier("");
		deviceField23.setEditable(false);
		deviceField24.setOffsetNid(21);
		deviceField24.setLabelString("Delay: ");
		deviceField24.setNumCols(20);
		deviceField24.setIdentifier("");
		this.getContentPane().add(jTabbedPane1, BorderLayout.CENTER);
		jTabbedPane1.add(jPanel1, "POLOIDAL");
		jPanel1.add(jTabbedPane2, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel3, "CLOCK");
		jPanel3.add(jPanel6, null);
		jPanel6.add(deviceField1, null);
		jPanel6.add(deviceField2, null);
		jPanel6.add(deviceField3, null);
		jPanel3.add(jPanel7, null);
		jPanel7.add(deviceField4, null);
		jPanel7.add(deviceField5, null);
		jPanel3.add(jPanel8, null);
		jPanel8.add(deviceField6, null);
		jPanel8.add(deviceField7, null);
		jTabbedPane2.add(jPanel4, "TRIGGER");
		jPanel4.add(jPanel9, null);
		jPanel4.add(jPanel10, null);
		jPanel10.add(deviceField9, null);
		jPanel10.add(deviceField10, null);
		jPanel10.add(deviceField11, null);
		jPanel4.add(jPanel11, null);
		jTabbedPane2.add(jPanel5, "AUTO ZERO");
		jPanel5.add(jPanel12, null);
		jPanel5.add(jPanel13, null);
		jPanel13.add(deviceField12, null);
		jPanel13.add(deviceField13, null);
		jPanel13.add(deviceField14, null);
		jPanel5.add(jPanel14, null);
		jTabbedPane1.add(jPanel2, "TOROIDAL");
		jPanel2.add(jTabbedPane3, BorderLayout.CENTER);
		jPanel22.add(deviceField23, null);
		jPanel22.add(deviceField24, null);
		jPanel22.add(deviceField21, null);
		jPanel17.add(jPanel22, null);
		jPanel17.add(jPanel21, null);
		jPanel21.add(deviceField17, null);
		jPanel21.add(deviceField16, null);
		jPanel17.add(jPanel20, null);
		jPanel20.add(deviceField20, null);
		jPanel20.add(deviceField19, null);
		jTabbedPane3.add(jPanel17, "CLOCK");
		jTabbedPane3.add(jPanel15, "TRIGGER");
		jPanel15.add(jPanel23, null);
		jPanel15.add(jPanel18, null);
		jPanel18.add(deviceField22, null);
		jPanel18.add(deviceField110, null);
		jPanel18.add(deviceField111, null);
		jPanel15.add(jPanel110, null);
		jTabbedPane3.add(jPanel19, "AUTO ZERO");
		jPanel19.add(jPanel111, null);
		jPanel19.add(jPanel16, null);
		jPanel16.add(deviceField112, null);
		jPanel16.add(deviceField18, null);
		jPanel16.add(deviceField15, null);
		jPanel19.add(jPanel112, null);
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
	}
}

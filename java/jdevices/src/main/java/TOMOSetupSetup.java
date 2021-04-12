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
public class TOMOSetupSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	JPanel jPanel1 = new JPanel();
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JTabbedPane jTabbedPane1 = new JTabbedPane();
	JPanel jPanel2 = new JPanel();
	JPanel jPanel3 = new JPanel();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel6 = new JPanel();
	JPanel jPanel7 = new JPanel();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField1 = new DeviceField();
	FlowLayout flowLayout1 = new FlowLayout();
	DeviceField deviceField4 = new DeviceField();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	GridLayout gridLayout2 = new GridLayout();
	FlowLayout flowLayout2 = new FlowLayout();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();
	JPanel jPanel8 = new JPanel();
	DeviceField deviceField9 = new DeviceField();
	FlowLayout flowLayout3 = new FlowLayout();
	DeviceField deviceField10 = new DeviceField();
	DeviceField deviceField11 = new DeviceField();
	JPanel jPanel9 = new JPanel();
	DeviceField deviceField12 = new DeviceField();
	FlowLayout flowLayout4 = new FlowLayout();
	DeviceField deviceField13 = new DeviceField();
	DeviceField deviceField14 = new DeviceField();
	JPanel jPanel10 = new JPanel();
	DeviceField deviceField15 = new DeviceField();
	JPanel jPanel11 = new JPanel();
	DeviceField deviceField16 = new DeviceField();
	DeviceField deviceField17 = new DeviceField();
	DeviceField deviceField18 = new DeviceField();
	JPanel jPanel12 = new JPanel();
	DeviceField deviceField19 = new DeviceField();
	DeviceField deviceField110 = new DeviceField();
	FlowLayout flowLayout5 = new FlowLayout();
	DeviceField deviceField111 = new DeviceField();
	DeviceField deviceField20 = new DeviceField();
	JPanel jPanel4 = new JPanel();
	FlowLayout flowLayout6 = new FlowLayout();
	JPanel jPanel13 = new JPanel();
	DeviceField deviceField112 = new DeviceField();
	DeviceField deviceField21 = new DeviceField();
	DeviceField deviceField22 = new DeviceField();
	FlowLayout flowLayout7 = new FlowLayout();
	GridLayout gridLayout3 = new GridLayout();
	JPanel jPanel14 = new JPanel();
	JPanel jPanel15 = new JPanel();
	DeviceField deviceField113 = new DeviceField();
	DeviceField deviceField114 = new DeviceField();
	DeviceField deviceField115 = new DeviceField();
	JPanel jPanel16 = new JPanel();
	DeviceField deviceField116 = new DeviceField();
	DeviceField deviceField117 = new DeviceField();
	FlowLayout flowLayout8 = new FlowLayout();
	DeviceField deviceField118 = new DeviceField();
	DeviceField deviceField23 = new DeviceField();
	JPanel jPanel5 = new JPanel();
	FlowLayout flowLayout9 = new FlowLayout();
	JPanel jPanel17 = new JPanel();
	DeviceField deviceField119 = new DeviceField();
	DeviceField deviceField24 = new DeviceField();
	DeviceField deviceField25 = new DeviceField();
	FlowLayout flowLayout10 = new FlowLayout();
	GridLayout gridLayout4 = new GridLayout();
	JPanel jPanel18 = new JPanel();
	JPanel jPanel19 = new JPanel();
	DeviceField deviceField1110 = new DeviceField();

	public TOMOSetupSetup()
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
		jPanel1.setLayout(borderLayout1);
		this.setWidth(730);
		this.setHeight(250);
		this.setDeviceType("TOMOSetup");
		this.setDeviceProvider("150.178.3.33");
		this.setDeviceTitle("\"Tomography timing configuration");
		jPanel2.setLayout(gridLayout1);
		gridLayout1.setRows(2);
		deviceField3.setOffsetNid(49);
		deviceField3.setLabelString("Delay ");
		deviceField3.setNumCols(15);
		deviceField3.setIdentifier("");
		deviceField2.setOffsetNid(48);
		deviceField2.setTextOnly(true);
		deviceField2.setLabelString("Trigger :   Event");
		deviceField2.setNumCols(15);
		deviceField2.setIdentifier("");
		deviceField2.setEditable(false);
		deviceField1.setOffsetNid(50);
		deviceField1.setLabelString("Duration");
		deviceField1.setNumCols(5);
		deviceField1.setIdentifier("");
		jPanel7.setLayout(flowLayout1);
		deviceField4.setOffsetNid(45);
		deviceField4.setLabelString("Stop : ");
		deviceField4.setIdentifier("");
		deviceField5.setOffsetNid(46);
		deviceField5.setLabelString("Frequerncy : ");
		deviceField5.setIdentifier("");
		deviceField6.setOffsetNid(44);
		deviceField6.setLabelString("Start : ");
		deviceField6.setIdentifier("");
		jPanel3.setLayout(gridLayout2);
		gridLayout2.setRows(4);
		deviceField7.setOffsetNid(5);
		deviceField7.setLabelString("Delay ");
		deviceField7.setNumCols(15);
		deviceField7.setIdentifier("");
		deviceField8.setOffsetNid(6);
		deviceField8.setLabelString("Duration");
		deviceField8.setNumCols(5);
		deviceField8.setIdentifier("");
		jPanel8.setLayout(flowLayout2);
		deviceField9.setOffsetNid(4);
		deviceField9.setTextOnly(true);
		deviceField9.setLabelString("Trigger :   Event");
		deviceField9.setNumCols(15);
		deviceField9.setIdentifier("");
		deviceField9.setEditable(false);
		deviceField10.setOffsetNid(13);
		deviceField10.setLabelString("Delay ");
		deviceField10.setNumCols(15);
		deviceField10.setIdentifier("");
		deviceField11.setOffsetNid(14);
		deviceField11.setLabelString("Duration");
		deviceField11.setNumCols(5);
		deviceField11.setIdentifier("");
		jPanel9.setLayout(flowLayout3);
		deviceField12.setOffsetNid(12);
		deviceField12.setTextOnly(true);
		deviceField12.setLabelString("Reset :   Event");
		deviceField12.setNumCols(15);
		deviceField12.setIdentifier("");
		deviceField12.setEditable(false);
		deviceField13.setOffsetNid(9);
		deviceField13.setLabelString("Delay ");
		deviceField13.setNumCols(15);
		deviceField13.setIdentifier("");
		deviceField14.setOffsetNid(10);
		deviceField14.setLabelString("Duration");
		deviceField14.setNumCols(5);
		deviceField14.setIdentifier("");
		jPanel10.setLayout(flowLayout4);
		deviceField15.setOffsetNid(8);
		deviceField15.setTextOnly(true);
		deviceField15.setLabelString("Zero  :   Event");
		deviceField15.setNumCols(15);
		deviceField15.setIdentifier("");
		deviceField15.setEditable(false);
		deviceField16.setOffsetNid(2);
		deviceField16.setLabelString("Acquisition duration : ");
		deviceField16.setIdentifier("");
		deviceField17.setNumCols(5);
		deviceField17.setIdentifier("");
		deviceField17.setOffsetNid(24);
		deviceField17.setLabelString("Duration");
		deviceField18.setNumCols(15);
		deviceField18.setIdentifier("");
		deviceField18.setOffsetNid(27);
		deviceField18.setLabelString("Delay ");
		jPanel12.setLayout(flowLayout5);
		deviceField19.setOffsetNid(28);
		deviceField19.setLabelString("Duration");
		deviceField19.setNumCols(5);
		deviceField19.setIdentifier("");
		deviceField110.setOffsetNid(26);
		deviceField110.setTextOnly(true);
		deviceField110.setLabelString("Reset :   Event");
		deviceField110.setNumCols(15);
		deviceField110.setIdentifier("");
		deviceField110.setEditable(false);
		deviceField111.setOffsetNid(22);
		deviceField111.setTextOnly(true);
		deviceField111.setLabelString("Zero  :   Event");
		deviceField111.setNumCols(15);
		deviceField111.setIdentifier("");
		deviceField111.setEditable(false);
		deviceField20.setOffsetNid(18);
		deviceField20.setTextOnly(true);
		deviceField20.setLabelString("Trigger :   Event");
		deviceField20.setNumCols(15);
		deviceField20.setIdentifier("");
		deviceField20.setEditable(false);
		jPanel4.setLayout(gridLayout3);
		jPanel13.setLayout(flowLayout7);
		deviceField112.setOffsetNid(23);
		deviceField112.setLabelString("Delay ");
		deviceField112.setNumCols(15);
		deviceField112.setIdentifier("");
		deviceField21.setOffsetNid(19);
		deviceField21.setLabelString("Delay ");
		deviceField21.setNumCols(15);
		deviceField21.setIdentifier("");
		deviceField22.setOffsetNid(20);
		deviceField22.setLabelString("Duration");
		deviceField22.setNumCols(5);
		deviceField22.setIdentifier("");
		gridLayout3.setRows(4);
		jPanel15.setLayout(flowLayout6);
		deviceField113.setOffsetNid(16);
		deviceField113.setLabelString("Acquisition duration : ");
		deviceField113.setIdentifier("");
		deviceField114.setNumCols(5);
		deviceField114.setIdentifier("");
		deviceField114.setOffsetNid(38);
		deviceField114.setLabelString("Duration");
		deviceField115.setNumCols(15);
		deviceField115.setIdentifier("");
		deviceField115.setOffsetNid(41);
		deviceField115.setLabelString("Delay ");
		jPanel16.setLayout(flowLayout8);
		deviceField116.setOffsetNid(42);
		deviceField116.setLabelString("Duration");
		deviceField116.setNumCols(5);
		deviceField116.setIdentifier("");
		deviceField117.setOffsetNid(40);
		deviceField117.setTextOnly(true);
		deviceField117.setLabelString("Reset :   Event");
		deviceField117.setNumCols(15);
		deviceField117.setIdentifier("");
		deviceField117.setEditable(false);
		deviceField118.setOffsetNid(36);
		deviceField118.setTextOnly(true);
		deviceField118.setLabelString("Zero  :   Event");
		deviceField118.setNumCols(15);
		deviceField118.setIdentifier("");
		deviceField118.setEditable(false);
		deviceField23.setOffsetNid(32);
		deviceField23.setTextOnly(true);
		deviceField23.setLabelString("Trigger :   Event");
		deviceField23.setNumCols(15);
		deviceField23.setIdentifier("");
		deviceField23.setEditable(false);
		jPanel5.setLayout(gridLayout4);
		jPanel17.setLayout(flowLayout10);
		deviceField119.setOffsetNid(37);
		deviceField119.setLabelString("Delay ");
		deviceField119.setNumCols(15);
		deviceField119.setIdentifier("");
		deviceField24.setOffsetNid(33);
		deviceField24.setLabelString("Delay ");
		deviceField24.setNumCols(15);
		deviceField24.setIdentifier("");
		deviceField25.setOffsetNid(34);
		deviceField25.setLabelString("Duration");
		deviceField25.setNumCols(5);
		deviceField25.setIdentifier("");
		gridLayout4.setRows(4);
		jPanel19.setLayout(flowLayout9);
		deviceField1110.setOffsetNid(30);
		deviceField1110.setLabelString("Acquisition duration : ");
		deviceField1110.setIdentifier("");
		this.getContentPane().add(jPanel1, BorderLayout.CENTER);
		jPanel1.add(deviceButtons1, BorderLayout.SOUTH);
		jPanel1.add(jTabbedPane1, BorderLayout.CENTER);
		jTabbedPane1.add(jPanel2, "DSXM");
		jPanel2.add(jPanel7, null);
		jPanel7.add(deviceField2, null);
		jPanel7.add(deviceField3, null);
		jPanel7.add(deviceField1, null);
		jPanel2.add(jPanel6, null);
		jPanel6.add(deviceField5, null);
		jPanel6.add(deviceField6, null);
		jPanel6.add(deviceField4, null);
		jTabbedPane1.add(jPanel3, "DBOT");
		jPanel3.add(jPanel11, null);
		jPanel11.add(deviceField16, null);
		jPanel3.add(jPanel8, null);
		jPanel8.add(deviceField9, null);
		jPanel8.add(deviceField7, null);
		jPanel8.add(deviceField8, null);
		jPanel3.add(jPanel10, null);
		jPanel10.add(deviceField15, null);
		jPanel10.add(deviceField13, null);
		jPanel10.add(deviceField14, null);
		jPanel3.add(jPanel9, null);
		jPanel9.add(deviceField12, null);
		jPanel9.add(deviceField10, null);
		jPanel9.add(deviceField11, null);
		jTabbedPane1.add(jPanel4, "DSXT");
		jPanel4.add(jPanel14, null);
		jPanel14.add(deviceField113, null);
		jPanel4.add(jPanel12, null);
		jPanel12.add(deviceField20, null);
		jPanel12.add(deviceField21, null);
		jPanel12.add(deviceField22, null);
		jPanel4.add(jPanel13, null);
		jPanel13.add(deviceField111, null);
		jPanel13.add(deviceField112, null);
		jPanel13.add(deviceField17, null);
		jPanel4.add(jPanel15, null);
		jPanel15.add(deviceField110, null);
		jPanel15.add(deviceField18, null);
		jPanel15.add(deviceField19, null);
		jTabbedPane1.add(jPanel5, "DSXC");
		jPanel5.add(jPanel18, null);
		jPanel18.add(deviceField1110, null);
		jPanel5.add(jPanel16, null);
		jPanel16.add(deviceField23, null);
		jPanel16.add(deviceField24, null);
		jPanel16.add(deviceField25, null);
		jPanel5.add(jPanel17, null);
		jPanel17.add(deviceField118, null);
		jPanel17.add(deviceField119, null);
		jPanel17.add(deviceField114, null);
		jPanel5.add(jPanel19, null);
		jPanel19.add(deviceField117, null);
		jPanel19.add(deviceField115, null);
		jPanel19.add(deviceField116, null);
	}
}

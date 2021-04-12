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
public class WE900Setup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	JPanel jPanel1 = new JPanel();
	JPanel jPanel3 = new JPanel();
	DeviceField deviceField1 = new DeviceField();
	FlowLayout flowLayout2 = new FlowLayout();
	JPanel jPanel4 = new JPanel();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	FlowLayout flowLayout3 = new FlowLayout();
	DeviceField deviceField2 = new DeviceField();
	FlowLayout flowLayout4 = new FlowLayout();
	JPanel jPanel5 = new JPanel();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceChoice deviceChoice3 = new DeviceChoice();
	FlowLayout flowLayout5 = new FlowLayout();
	JPanel jPanel6 = new JPanel();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	JScrollPane jScrollPane1 = new JScrollPane();
	DeviceChoice deviceChoice4 = new DeviceChoice();
	FlowLayout flowLayout6 = new FlowLayout();
	JPanel jPanel7 = new JPanel();
	FlowLayout flowLayout1 = new FlowLayout();
	DeviceField deviceField3 = new DeviceField();
	JPanel jPanel2 = new JPanel();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel8 = new JPanel();
	GridLayout gridLayout1 = new GridLayout();
	DeviceField deviceField9 = new DeviceField();
	DeviceChoice deviceChoice7 = new DeviceChoice();
	JPanel jPanel11 = new JPanel();
	JPanel jPanel9 = new JPanel();
	DeviceChannel deviceChannel3 = new DeviceChannel();
	FlowLayout flowLayout7 = new FlowLayout();
	FlowLayout flowLayout8 = new FlowLayout();
	DeviceField deviceField10 = new DeviceField();
	DeviceChoice deviceChoice8 = new DeviceChoice();
	JPanel jPanel12 = new JPanel();
	JPanel jPanel10 = new JPanel();
	DeviceChannel deviceChannel4 = new DeviceChannel();
	FlowLayout flowLayout9 = new FlowLayout();
	DeviceField deviceField11 = new DeviceField();
	DeviceChoice deviceChoice9 = new DeviceChoice();
	JPanel jPanel13 = new JPanel();
	JPanel jPanel14 = new JPanel();
	DeviceChannel deviceChannel5 = new DeviceChannel();
	FlowLayout flowLayout10 = new FlowLayout();
	DeviceField deviceField12 = new DeviceField();
	DeviceChoice deviceChoice10 = new DeviceChoice();
	JPanel jPanel15 = new JPanel();
	JPanel jPanel16 = new JPanel();
	DeviceChannel deviceChannel6 = new DeviceChannel();
	FlowLayout flowLayout11 = new FlowLayout();
	DeviceField deviceField13 = new DeviceField();
	DeviceChoice deviceChoice11 = new DeviceChoice();
	JPanel jPanel17 = new JPanel();
	JPanel jPanel18 = new JPanel();
	DeviceChannel deviceChannel7 = new DeviceChannel();
	FlowLayout flowLayout12 = new FlowLayout();
	DeviceField deviceField14 = new DeviceField();
	DeviceChoice deviceChoice12 = new DeviceChoice();
	JPanel jPanel19 = new JPanel();
	JPanel jPanel110 = new JPanel();
	DeviceChannel deviceChannel8 = new DeviceChannel();
	FlowLayout flowLayout13 = new FlowLayout();
	DeviceField deviceField15 = new DeviceField();
	DeviceChoice deviceChoice13 = new DeviceChoice();
	JPanel jPanel111 = new JPanel();
	JPanel jPanel112 = new JPanel();
	DeviceChannel deviceChannel9 = new DeviceChannel();
	FlowLayout flowLayout14 = new FlowLayout();
	DeviceField deviceField16 = new DeviceField();
	DeviceChoice deviceChoice14 = new DeviceChoice();
	JPanel jPanel113 = new JPanel();
	JPanel jPanel114 = new JPanel();
	DeviceChannel deviceChannel10 = new DeviceChannel();
	DeviceField deviceField4 = new DeviceField();
	JPanel jPanel115 = new JPanel();
	JPanel jPanel116 = new JPanel();
	DeviceField deviceField17 = new DeviceField();
	DeviceChannel deviceChannel11 = new DeviceChannel();
	DeviceChoice deviceChoice15 = new DeviceChoice();
	FlowLayout flowLayout15 = new FlowLayout();

	public WE900Setup()
	{
		try
		{
			jbInit();
			setSize(new Dimension(530, 420));
		}
		catch (final Exception e)
		{
			e.printStackTrace();
		}
	}

	private void jbInit() throws Exception
	{
		this.setWidth(529);
		this.setHeight(529);
		this.setDeviceType("WE900");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("YOKOGAWA Rack 9 Slots");
		this.getContentPane().setLayout(null);
		jPanel1.setLayout(null);
		deviceField1.setOffsetNid(1);
		deviceField1.setLabelString("Comment: ");
		deviceField1.setNumCols(25);
		jPanel3.setLayout(flowLayout2);
		flowLayout2.setAlignment(FlowLayout.LEFT);
		flowLayout2.setHgap(0);
		flowLayout2.setVgap(0);
		deviceChoice1.setOffsetNid(6);
		deviceChoice1.setLabelString("Bus Trigger 2:");
		deviceChoice1.setChoiceItems(new String[]
		{ "SOFTWARE", "EXT I/O", "TRIGIN" });
		jPanel4.setLayout(flowLayout3);
		flowLayout3.setAlignment(FlowLayout.LEFT);
		flowLayout3.setHgap(5);
		flowLayout3.setVgap(0);
		deviceField2.setOffsetNid(7);
		deviceField2.setLabelString("Source: ");
		deviceField2.setNumCols(20);
		flowLayout4.setAlignment(FlowLayout.LEFT);
		flowLayout4.setHgap(5);
		flowLayout4.setVgap(0);
		jPanel5.setLayout(flowLayout4);
		deviceChoice2.setOffsetNid(4);
		deviceChoice2.setLabelString("Bus Trigger 1:");
		deviceChoice2.setShowState(false);
		deviceChoice2.setChoiceItems(new String[]
		{ "SOFTWARE", "EXT I/O", "TRIGIN" });
		deviceField5.setOffsetNid(5);
		deviceField5.setLabelString("Source: ");
		deviceField5.setNumCols(20);
		deviceField6.setNumCols(20);
		deviceField6.setOffsetNid(10);
		deviceField6.setLabelString("Source: ");
		deviceChoice3.setChoiceItems(new String[]
		{ "NONE", "EXT I/O", "TRIGIN" });
		deviceChoice3.setOffsetNid(9);
		deviceChoice3.setLabelString("Cmn. Clock:");
		flowLayout5.setAlignment(FlowLayout.LEFT);
		flowLayout5.setHgap(5);
		flowLayout5.setVgap(0);
		jPanel6.setLayout(flowLayout5);
		jPanel1.setPreferredSize(new Dimension(300, 500));
		jPanel1.setBounds(new Rectangle(1, 2, 529, 396));
		jScrollPane1.setDebugGraphicsOptions(0);
		jScrollPane1.setBounds(new Rectangle(15, 191, 488, 138));
		deviceChoice4.setOffsetNid(8);
		deviceChoice4.setLabelString("Trigger Slope:");
		deviceChoice4.setChoiceItems(new String[]
		{ "POSITIVE", "NEGATIVE" });
		flowLayout6.setAlignment(FlowLayout.LEFT);
		flowLayout6.setVgap(0);
		jPanel7.setLayout(flowLayout6);
		flowLayout1.setAlignment(FlowLayout.LEFT);
		flowLayout1.setHgap(0);
		flowLayout1.setVgap(0);
		deviceField3.setNumCols(15);
		deviceField3.setOffsetNid(2);
		deviceField3.setLabelString("Controller IP: ");
		jPanel2.setLayout(flowLayout1);
		jPanel3.setBounds(new Rectangle(2, 1, 524, 31));
		jPanel5.setBounds(new Rectangle(1, 94, 564, 31));
		jPanel6.setBounds(new Rectangle(2, 61, 578, 31));
		jPanel4.setBounds(new Rectangle(3, 127, 565, 31));
		jPanel7.setBounds(new Rectangle(4, 158, 216, 31));
		jPanel2.setBounds(new Rectangle(7, 34, 572, 31));
		deviceButtons1.setMethods(new String[]
		{ "init", "arm", "turnOff" });
		deviceButtons1.setBounds(new Rectangle(24, 347, 474, 35));
		jPanel8.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(9);
		deviceField9.setLabelString("Modules Linked");
		deviceField9.setNumCols(10);
		deviceField9.setOffsetNid(32);
		deviceChoice7.setIdentifier("");
		deviceChoice7.setChoiceItems(new String[]
		{ "WE7275", "WE7116" });
		deviceChoice7.setShowState(false);
		deviceChoice7.setLabelString("Module Type");
		deviceChoice7.setOffsetNid(31);
		deviceChannel3.setInSameLine(true);
		deviceChannel3.setLabelString("Slot 07");
		deviceChannel3.setOffsetNid(30);
		jPanel9.setLayout(flowLayout7);
		flowLayout7.setHgap(0);
		flowLayout7.setVgap(0);
		flowLayout8.setVgap(0);
		flowLayout8.setHgap(0);
		deviceField10.setOffsetNid(35);
		deviceField10.setLabelString("Modules Linked");
		deviceField10.setIdentifier("");
		deviceChoice8.setOffsetNid(34);
		deviceChoice8.setLabelString("Module Type");
		deviceChoice8.setShowState(false);
		deviceChoice8.setChoiceItems(new String[]
		{ "WE7275", "WE7116" });
		deviceChoice8.setIdentifier("");
		jPanel10.setLayout(flowLayout8);
		deviceChannel4.setInSameLine(true);
		deviceChannel4.setLabelString("Slot 08");
		deviceChannel4.setOffsetNid(33);
		flowLayout9.setVgap(0);
		flowLayout9.setHgap(0);
		deviceField11.setOffsetNid(29);
		deviceField11.setLabelString("Modules Linked");
		deviceField11.setNumCols(10);
		deviceChoice9.setOffsetNid(28);
		deviceChoice9.setLabelString("Module Type");
		deviceChoice9.setShowState(false);
		deviceChoice9.setChoiceItems(new String[]
		{ "WE7275", "WE7116" });
		deviceChoice9.setIdentifier("");
		jPanel14.setLayout(flowLayout9);
		deviceChannel5.setInSameLine(true);
		deviceChannel5.setLabelString("Slot 06");
		deviceChannel5.setOffsetNid(27);
		deviceChannel5.setLines(1);
		flowLayout10.setVgap(0);
		flowLayout10.setHgap(0);
		deviceField12.setOffsetNid(14);
		deviceField12.setLabelString("Modules Linked");
		deviceChoice10.setOffsetNid(13);
		deviceChoice10.setLabelString("Module Type");
		deviceChoice10.setShowState(false);
		deviceChoice10.setChoiceItems(new String[]
		{ "WE7275", "WE7116" });
		deviceChoice10.setIdentifier("");
		jPanel16.setLayout(flowLayout10);
		deviceChannel6.setInSameLine(true);
		deviceChannel6.setLabelString("Slot 01");
		deviceChannel6.setOffsetNid(12);
		deviceChannel6.setLines(1);
		flowLayout11.setVgap(0);
		flowLayout11.setHgap(0);
		deviceField13.setOffsetNid(26);
		deviceField13.setLabelString("Modules Linked");
		deviceField13.setIdentifier("");
		deviceChoice11.setOffsetNid(25);
		deviceChoice11.setLabelString("Module Type");
		deviceChoice11.setShowState(false);
		deviceChoice11.setChoiceItems(new String[]
		{ "WE7275", "WE7116" });
		deviceChoice11.setIdentifier("");
		jPanel18.setLayout(flowLayout11);
		deviceChannel7.setInSameLine(true);
		deviceChannel7.setLabelString("Slot 05");
		deviceChannel7.setOffsetNid(24);
		deviceChannel7.setLines(1);
		flowLayout12.setVgap(0);
		flowLayout12.setHgap(0);
		deviceField14.setOffsetNid(23);
		deviceField14.setLabelString("Modules Linked");
		deviceField14.setIdentifier("");
		deviceChoice12.setOffsetNid(22);
		deviceChoice12.setLabelString("Module Type");
		deviceChoice12.setShowState(false);
		deviceChoice12.setChoiceItems(new String[]
		{ "WE7275", "WE7116" });
		deviceChoice12.setIdentifier("");
		jPanel110.setLayout(flowLayout12);
		deviceChannel8.setInSameLine(true);
		deviceChannel8.setLabelString("Slot 04");
		deviceChannel8.setOffsetNid(21);
		deviceChannel8.setLines(1);
		flowLayout13.setVgap(0);
		flowLayout13.setHgap(0);
		deviceField15.setOffsetNid(20);
		deviceField15.setLabelString("Modules Linked");
		deviceField15.setIdentifier("");
		deviceChoice13.setOffsetNid(19);
		deviceChoice13.setLabelString("Module Type");
		deviceChoice13.setShowState(false);
		deviceChoice13.setChoiceItems(new String[]
		{ "WE7275", "WE7116" });
		deviceChoice13.setIdentifier("");
		jPanel112.setLayout(flowLayout13);
		deviceChannel9.setInSameLine(true);
		deviceChannel9.setLabelString("Slot 03");
		deviceChannel9.setOffsetNid(18);
		deviceChannel9.setLines(1);
		flowLayout14.setVgap(0);
		flowLayout14.setHgap(0);
		deviceField16.setOffsetNid(17);
		deviceField16.setLabelString("Modules Linked");
		deviceField16.setNumCols(10);
		deviceChoice14.setOffsetNid(16);
		deviceChoice14.setLabelString("Module Type");
		deviceChoice14.setShowState(false);
		deviceChoice14.setChoiceItems(new String[]
		{ "WE7275", "WE7116" });
		deviceChoice14.setIdentifier("");
		jPanel114.setLayout(flowLayout14);
		deviceChannel10.setInSameLine(true);
		deviceChannel10.setLabelString("Slot 02");
		deviceChannel10.setOffsetNid(15);
		deviceChannel10.setLines(1);
		deviceField4.setOffsetNid(3);
		deviceField4.setLabelString("Station IP:");
		deviceField4.setNumCols(15);
		jPanel115.setLayout(flowLayout15);
		deviceField17.setOffsetNid(38);
		deviceField17.setLabelString("Modules Linked");
		deviceField17.setIdentifier("");
		deviceChannel11.setInSameLine(true);
		deviceChannel11.setLabelString("Slot 09");
		deviceChannel11.setOffsetNid(36);
		deviceChoice15.setOffsetNid(37);
		deviceChoice15.setLabelString("Module Type");
		deviceChoice15.setShowState(false);
		deviceChoice15.setChoiceItems(new String[]
		{ "WE7275", "WE7116" });
		deviceChoice15.setIdentifier("");
		flowLayout15.setVgap(0);
		flowLayout15.setHgap(0);
		jPanel3.add(deviceField1, null);
		jPanel3.add(deviceDispatch1, null);
		jPanel1.add(deviceButtons1, null);
		jPanel1.add(jScrollPane1, null);
		jScrollPane1.getViewport().add(jPanel8, null);
		jPanel11.add(deviceChoice7, null);
		jPanel11.add(deviceField9, null);
		jPanel8.add(jPanel16, null);
		jPanel16.add(deviceChannel6, null);
		deviceChannel6.add(jPanel15, BorderLayout.EAST);
		jPanel15.add(deviceChoice10, null);
		jPanel15.add(deviceField12, null);
		jPanel116.add(deviceChoice15, null);
		jPanel116.add(deviceField17, null);
		jPanel8.add(jPanel114, null);
		jPanel114.add(deviceChannel10, null);
		deviceChannel10.add(jPanel113, BorderLayout.EAST);
		jPanel113.add(deviceChoice14, null);
		jPanel113.add(deviceField16, null);
		jPanel8.add(jPanel112, null);
		jPanel112.add(deviceChannel9, null);
		deviceChannel9.add(jPanel111, BorderLayout.EAST);
		jPanel111.add(deviceChoice13, null);
		jPanel111.add(deviceField15, null);
		jPanel8.add(jPanel110, null);
		jPanel110.add(deviceChannel8, null);
		deviceChannel8.add(jPanel19, BorderLayout.EAST);
		jPanel19.add(deviceChoice12, null);
		jPanel19.add(deviceField14, null);
		jPanel8.add(jPanel18, null);
		jPanel18.add(deviceChannel7, null);
		deviceChannel7.add(jPanel17, BorderLayout.EAST);
		jPanel17.add(deviceChoice11, null);
		jPanel17.add(deviceField13, null);
		jPanel8.add(jPanel14, null);
		jPanel14.add(deviceChannel5, null);
		deviceChannel5.add(jPanel13, BorderLayout.EAST);
		jPanel8.add(jPanel9, null);
		jPanel9.add(deviceChannel3, null);
		deviceChannel3.add(jPanel11, BorderLayout.EAST);
		jPanel13.add(deviceChoice9, null);
		jPanel13.add(deviceField11, null);
		jPanel8.add(jPanel10, null);
		jPanel10.add(deviceChannel4, null);
		deviceChannel4.add(jPanel12, BorderLayout.EAST);
		jPanel12.add(deviceChoice8, null);
		jPanel12.add(deviceField10, null);
		jPanel8.add(jPanel115, null);
		jPanel115.add(deviceChannel11, null);
		deviceChannel11.add(jPanel116, BorderLayout.EAST);
		jPanel1.add(jPanel7, null);
		jPanel7.add(deviceChoice4, null);
		jPanel6.add(deviceChoice3, null);
		jPanel6.add(deviceField6, null);
		jPanel1.add(jPanel4, null);
		jPanel1.add(jPanel5, null);
		jPanel5.add(deviceChoice2, null);
		jPanel5.add(deviceField5, null);
		jPanel4.add(deviceChoice1, null);
		jPanel4.add(deviceField2, null);
		jPanel1.add(jPanel6, null);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceField3, null);
		jPanel2.add(deviceField4, null);
		jPanel1.add(jPanel3, null);
		this.getContentPane().add(jPanel1, null);
	}
}

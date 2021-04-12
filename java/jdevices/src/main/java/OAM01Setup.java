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
public class OAM01Setup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	JPanel jPanel1 = new JPanel();
	BorderLayout borderLayout2 = new BorderLayout();
	JScrollPane jScrollPane1 = new JScrollPane();
	JScrollPane jScrollPane2 = new JScrollPane();
	JPanel jPanel2 = new JPanel();
	JPanel jPanel3 = new JPanel();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel4 = new JPanel();
	FlowLayout flowLayout1 = new FlowLayout();
	JLabel jLabel1 = new JLabel();
	DeviceChannel deviceChannel1 = new DeviceChannel();
	FlowLayout flowLayout2 = new FlowLayout();
	JPanel jPanel5 = new JPanel();
	FlowLayout flowLayout3 = new FlowLayout();
	FlowLayout flowLayout4 = new FlowLayout();
	JLabel jLabel2 = new JLabel();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceChoice deviceChoice3 = new DeviceChoice();
	DeviceField deviceField1 = new DeviceField();
	JLabel jLabel3 = new JLabel();
	JLabel jLabel4 = new JLabel();
	JLabel jLabel5 = new JLabel();
	JLabel jLabel6 = new JLabel();
	FlowLayout flowLayout5 = new FlowLayout();
	DeviceChoice deviceChoice4 = new DeviceChoice();
	JPanel jPanel6 = new JPanel();
	DeviceChannel deviceChannel2 = new DeviceChannel();
	JLabel jLabel7 = new JLabel();
	FlowLayout flowLayout6 = new FlowLayout();
	DeviceChoice deviceChoice5 = new DeviceChoice();
	FlowLayout flowLayout7 = new FlowLayout();
	DeviceField deviceField2 = new DeviceField();
	JPanel jPanel7 = new JPanel();
	DeviceChoice deviceChoice6 = new DeviceChoice();
	FlowLayout flowLayout8 = new FlowLayout();
	DeviceChoice deviceChoice7 = new DeviceChoice();
	JPanel jPanel8 = new JPanel();
	DeviceChannel deviceChannel3 = new DeviceChannel();
	JLabel jLabel8 = new JLabel();
	FlowLayout flowLayout9 = new FlowLayout();
	DeviceChoice deviceChoice8 = new DeviceChoice();
	FlowLayout flowLayout10 = new FlowLayout();
	DeviceField deviceField3 = new DeviceField();
	JPanel jPanel9 = new JPanel();
	DeviceChoice deviceChoice9 = new DeviceChoice();
	FlowLayout flowLayout11 = new FlowLayout();
	DeviceChoice deviceChoice10 = new DeviceChoice();
	JPanel jPanel10 = new JPanel();
	DeviceChannel deviceChannel4 = new DeviceChannel();
	JLabel jLabel9 = new JLabel();
	FlowLayout flowLayout12 = new FlowLayout();
	DeviceChoice deviceChoice11 = new DeviceChoice();
	FlowLayout flowLayout13 = new FlowLayout();
	DeviceField deviceField4 = new DeviceField();
	JPanel jPanel11 = new JPanel();
	DeviceChoice deviceChoice12 = new DeviceChoice();
	FlowLayout flowLayout14 = new FlowLayout();
	DeviceChoice deviceChoice13 = new DeviceChoice();
	JPanel jPanel12 = new JPanel();
	DeviceChannel deviceChannel5 = new DeviceChannel();
	JLabel jLabel10 = new JLabel();
	FlowLayout flowLayout15 = new FlowLayout();
	DeviceChoice deviceChoice14 = new DeviceChoice();
	FlowLayout flowLayout16 = new FlowLayout();
	DeviceField deviceField5 = new DeviceField();
	JPanel jPanel13 = new JPanel();
	DeviceChoice deviceChoice15 = new DeviceChoice();
	FlowLayout flowLayout17 = new FlowLayout();
	DeviceChoice deviceChoice16 = new DeviceChoice();
	JPanel jPanel14 = new JPanel();
	DeviceChannel deviceChannel6 = new DeviceChannel();
	JLabel jLabel11 = new JLabel();
	FlowLayout flowLayout18 = new FlowLayout();
	DeviceChoice deviceChoice17 = new DeviceChoice();
	FlowLayout flowLayout19 = new FlowLayout();
	DeviceField deviceField6 = new DeviceField();
	JPanel jPanel15 = new JPanel();
	DeviceChoice deviceChoice18 = new DeviceChoice();
	FlowLayout flowLayout110 = new FlowLayout();
	DeviceChoice deviceChoice19 = new DeviceChoice();
	JPanel jPanel16 = new JPanel();
	DeviceChannel deviceChannel7 = new DeviceChannel();
	JLabel jLabel12 = new JLabel();
	FlowLayout flowLayout111 = new FlowLayout();
	DeviceChoice deviceChoice110 = new DeviceChoice();
	FlowLayout flowLayout112 = new FlowLayout();
	DeviceField deviceField7 = new DeviceField();
	JPanel jPanel17 = new JPanel();
	DeviceChoice deviceChoice111 = new DeviceChoice();
	FlowLayout flowLayout113 = new FlowLayout();
	DeviceChoice deviceChoice112 = new DeviceChoice();
	JPanel jPanel18 = new JPanel();
	DeviceChannel deviceChannel8 = new DeviceChannel();
	JLabel jLabel13 = new JLabel();
	FlowLayout flowLayout114 = new FlowLayout();
	DeviceChoice deviceChoice113 = new DeviceChoice();
	FlowLayout flowLayout115 = new FlowLayout();
	DeviceField deviceField8 = new DeviceField();
	JPanel jPanel19 = new JPanel();
	DeviceChoice deviceChoice114 = new DeviceChoice();
	JPanel jPanel20 = new JPanel();
	FlowLayout flowLayout20 = new FlowLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JScrollPane jScrollPane3 = new JScrollPane();
	JPanel jPanel21 = new JPanel();
	GridLayout gridLayout2 = new GridLayout();
	JPanel jPanel22 = new JPanel();
	JPanel jPanel23 = new JPanel();
	FlowLayout flowLayout21 = new FlowLayout();
	JLabel jLabel15 = new JLabel();
	JPanel jPanel25 = new JPanel();
	FlowLayout flowLayout23 = new FlowLayout();
	DeviceField deviceField10 = new DeviceField();
	JLabel jLabel16 = new JLabel();
	JPanel jPanel26 = new JPanel();
	FlowLayout flowLayout24 = new FlowLayout();
	DeviceChoice deviceChoice20 = new DeviceChoice();
	JLabel jLabel17 = new JLabel();
	JPanel jPanel27 = new JPanel();
	FlowLayout flowLayout25 = new FlowLayout();
	DeviceField deviceField11 = new DeviceField();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	FlowLayout flowLayout26 = new FlowLayout();
	JPanel jPanel24 = new JPanel();

	public OAM01Setup()
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
		this.setWidth(607);
		this.setHeight(607);
		this.setDeviceType("OAM01");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("OAM01 MODULE SETUP");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(borderLayout2);
		jPanel3.setLayout(gridLayout1);
		gridLayout1.setColumns(0);
		gridLayout1.setRows(8);
		gridLayout1.setVgap(-15);
		jPanel4.setLayout(flowLayout1);
		jLabel1.setText("1A");
		deviceChannel1.setLabelString("");
		deviceChannel1.setOffsetNid(4);
		deviceChannel1.setInSameLine(true);
		deviceChannel1.setUpdateIdentifier("");
		deviceChannel1.setShowVal("");
		deviceChannel1.setLayout(flowLayout2);
		jPanel5.setLayout(flowLayout3);
		jPanel2.setLayout(flowLayout4);
		jLabel2.setPreferredSize(new Dimension(50, 16));
		jLabel2.setHorizontalAlignment(SwingConstants.CENTER);
		jLabel2.setText("STATE");
		deviceChoice1.setChoiceIntValues(null);
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setOffsetNid(7);
		deviceChoice1.setLabelString("");
		deviceChoice1.setChoiceItems(new String[]
		{ "INPUT", "ZERO", "REF", "BAT" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceChoice3.setChoiceIntValues(null);
		deviceChoice3.setChoiceFloatValues(new float[]
		{ (float) 250.0, (float) 100.0, (float) 50.0, (float) 25.0, (float) 10.0, (float) 5.0, (float) 2.5, (float) 1.0,
				(float) 0.5 });
		deviceChoice3.setOffsetNid(5);
		deviceChoice3.setLabelString("");
		deviceChoice3.setChoiceItems(new String[]
		{ "250", "100", "50", "25", "10", "5", "2.5", "1", "0.5" });
		deviceChoice3.setUpdateIdentifier("");
		deviceChoice3.setIdentifier("");
		deviceChoice2.setChoiceIntValues(null);
		deviceChoice2.setChoiceFloatValues(null);
		deviceChoice2.setOffsetNid(6);
		deviceChoice2.setLabelString("");
		deviceChoice2.setChoiceItems(new String[]
		{ "AC", "DC" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		jLabel3.setPreferredSize(new Dimension(60, 16));
		jLabel3.setHorizontalAlignment(SwingConstants.CENTER);
		jLabel3.setText("RANGE");
		jLabel4.setPreferredSize(new Dimension(70, 16));
		jLabel4.setHorizontalAlignment(SwingConstants.CENTER);
		jLabel4.setText("COUPLING");
		deviceField1.setOffsetNid(9);
		deviceField1.setTextOnly(false);
		deviceField1.setNumCols(25);
		deviceField1.setIdentifier("");
		jLabel5.setPreferredSize(new Dimension(60, 16));
		jLabel5.setHorizontalAlignment(SwingConstants.CENTER);
		jLabel5.setText("SOURCE");
		jLabel6.setPreferredSize(new Dimension(320, 16));
		jLabel6.setHorizontalAlignment(SwingConstants.CENTER);
		jLabel6.setText("OUTPUT");
		jScrollPane1.setBorder(BorderFactory.createEmptyBorder());
		jScrollPane2.setBorder(BorderFactory.createEmptyBorder());
		deviceChoice4.setIdentifier("");
		deviceChoice4.setUpdateIdentifier("");
		deviceChoice4.setChoiceItems(new String[]
		{ "AC", "DC" });
		deviceChoice4.setLabelString("");
		deviceChoice4.setOffsetNid(48);
		deviceChoice4.setChoiceFloatValues(null);
		deviceChoice4.setChoiceIntValues(null);
		jPanel6.setLayout(flowLayout5);
		deviceChannel2.setLabelString("");
		deviceChannel2.setOffsetNid(46);
		deviceChannel2.setInSameLine(true);
		deviceChannel2.setUpdateIdentifier("");
		deviceChannel2.setShowVal("");
		deviceChannel2.setLayout(flowLayout6);
		jLabel7.setText("4B");
		deviceChoice5.setChoiceIntValues(null);
		deviceChoice5.setChoiceFloatValues(null);
		deviceChoice5.setOffsetNid(49);
		deviceChoice5.setLabelString("");
		deviceChoice5.setChoiceItems(new String[]
		{ "INPUT", "ZERO", "REF", "BAT" });
		deviceChoice5.setUpdateIdentifier("");
		deviceChoice5.setIdentifier("");
		deviceField2.setOffsetNid(51);
		deviceField2.setTextOnly(false);
		deviceField2.setNumCols(25);
		deviceField2.setIdentifier("");
		jPanel7.setLayout(flowLayout7);
		deviceChoice6.setChoiceIntValues(null);
		deviceChoice6.setChoiceFloatValues(new float[]
		{ (float) 250.0, (float) 100.0, (float) 50.0, (float) 25.0, (float) 10.0, (float) 5.0, (float) 2.5, (float) 1.0,
				(float) 0.5 });
		deviceChoice6.setOffsetNid(47);
		deviceChoice6.setLabelString("");
		deviceChoice6.setChoiceItems(new String[]
		{ "250", "100", "50", "25", "10", "5", "2.5", "1", "0.5" });
		deviceChoice6.setUpdateIdentifier("");
		deviceChoice6.setIdentifier("");
		deviceChoice7.setIdentifier("");
		deviceChoice7.setUpdateIdentifier("");
		deviceChoice7.setChoiceItems(new String[]
		{ "AC", "DC" });
		deviceChoice7.setLabelString("");
		deviceChoice7.setOffsetNid(42);
		deviceChoice7.setChoiceFloatValues(null);
		deviceChoice7.setChoiceIntValues(null);
		jPanel8.setLayout(flowLayout8);
		deviceChannel3.setLabelString("");
		deviceChannel3.setOffsetNid(40);
		deviceChannel3.setInSameLine(true);
		deviceChannel3.setUpdateIdentifier("");
		deviceChannel3.setShowVal("");
		deviceChannel3.setLayout(flowLayout9);
		jLabel8.setText("4A");
		deviceChoice8.setChoiceIntValues(null);
		deviceChoice8.setChoiceFloatValues(null);
		deviceChoice8.setOffsetNid(43);
		deviceChoice8.setLabelString("");
		deviceChoice8.setChoiceItems(new String[]
		{ "INPUT", "ZERO", "REF", "BAT" });
		deviceChoice8.setUpdateIdentifier("");
		deviceChoice8.setIdentifier("");
		deviceField3.setOffsetNid(45);
		deviceField3.setTextOnly(false);
		deviceField3.setNumCols(25);
		deviceField3.setIdentifier("");
		jPanel9.setLayout(flowLayout10);
		deviceChoice9.setChoiceIntValues(null);
		deviceChoice9.setChoiceFloatValues(new float[]
		{ (float) 250.0, (float) 100.0, (float) 50.0, (float) 25.0, (float) 10.0, (float) 5.0, (float) 2.5, (float) 1.0,
				(float) 0.5 });
		deviceChoice9.setOffsetNid(41);
		deviceChoice9.setLabelString("");
		deviceChoice9.setChoiceItems(new String[]
		{ "250", "100", "50", "25", "10", "5", "2.5", "1", "0.5" });
		deviceChoice9.setUpdateIdentifier("");
		deviceChoice9.setIdentifier("");
		deviceChoice10.setIdentifier("");
		deviceChoice10.setUpdateIdentifier("");
		deviceChoice10.setChoiceItems(new String[]
		{ "AC", "DC" });
		deviceChoice10.setLabelString("");
		deviceChoice10.setOffsetNid(36);
		deviceChoice10.setChoiceFloatValues(null);
		deviceChoice10.setChoiceIntValues(null);
		jPanel10.setLayout(flowLayout11);
		deviceChannel4.setLabelString("");
		deviceChannel4.setOffsetNid(34);
		deviceChannel4.setInSameLine(true);
		deviceChannel4.setUpdateIdentifier("");
		deviceChannel4.setShowVal("");
		deviceChannel4.setLayout(flowLayout12);
		jLabel9.setText("3B");
		deviceChoice11.setChoiceIntValues(null);
		deviceChoice11.setChoiceFloatValues(null);
		deviceChoice11.setOffsetNid(37);
		deviceChoice11.setLabelString("");
		deviceChoice11.setChoiceItems(new String[]
		{ "INPUT", "ZERO", "REF", "BAT" });
		deviceChoice11.setUpdateIdentifier("");
		deviceChoice11.setIdentifier("");
		deviceField4.setOffsetNid(39);
		deviceField4.setTextOnly(false);
		deviceField4.setNumCols(25);
		deviceField4.setIdentifier("");
		jPanel11.setLayout(flowLayout13);
		deviceChoice12.setChoiceIntValues(null);
		deviceChoice12.setChoiceFloatValues(new float[]
		{ (float) 250.0, (float) 100.0, (float) 50.0, (float) 25.0, (float) 10.0, (float) 5.0, (float) 2.5, (float) 1.0,
				(float) 0.5 });
		deviceChoice12.setOffsetNid(35);
		deviceChoice12.setLabelString("");
		deviceChoice12.setChoiceItems(new String[]
		{ "250", "100", "50", "25", "10", "5", "2.5", "1", "0.5" });
		deviceChoice12.setUpdateIdentifier("");
		deviceChoice12.setIdentifier("");
		deviceChoice13.setIdentifier("");
		deviceChoice13.setUpdateIdentifier("");
		deviceChoice13.setChoiceItems(new String[]
		{ "AC", "DC" });
		deviceChoice13.setLabelString("");
		deviceChoice13.setOffsetNid(30);
		deviceChoice13.setChoiceFloatValues(null);
		deviceChoice13.setChoiceIntValues(null);
		jPanel12.setLayout(flowLayout14);
		deviceChannel5.setLabelString("");
		deviceChannel5.setOffsetNid(28);
		deviceChannel5.setInSameLine(true);
		deviceChannel5.setUpdateIdentifier("");
		deviceChannel5.setShowVal("");
		deviceChannel5.setLayout(flowLayout15);
		jLabel10.setText("3A");
		deviceChoice14.setChoiceIntValues(null);
		deviceChoice14.setChoiceFloatValues(null);
		deviceChoice14.setOffsetNid(31);
		deviceChoice14.setLabelString("");
		deviceChoice14.setChoiceItems(new String[]
		{ "INPUT", "ZERO", "REF", "BAT" });
		deviceChoice14.setUpdateIdentifier("");
		deviceChoice14.setIdentifier("");
		deviceField5.setOffsetNid(33);
		deviceField5.setTextOnly(false);
		deviceField5.setNumCols(25);
		deviceField5.setIdentifier("");
		jPanel13.setLayout(flowLayout16);
		deviceChoice15.setChoiceIntValues(null);
		deviceChoice15.setChoiceFloatValues(new float[]
		{ (float) 250.0, (float) 100.0, (float) 50.0, (float) 25.0, (float) 10.0, (float) 5.0, (float) 2.5, (float) 1.0,
				(float) 0.5 });
		deviceChoice15.setOffsetNid(29);
		deviceChoice15.setLabelString("");
		deviceChoice15.setChoiceItems(new String[]
		{ "250", "100", "50", "25", "10", "5", "2.5", "1", "0.5" });
		deviceChoice15.setUpdateIdentifier("");
		deviceChoice15.setIdentifier("");
		deviceChoice16.setIdentifier("");
		deviceChoice16.setUpdateIdentifier("");
		deviceChoice16.setChoiceItems(new String[]
		{ "AC", "DC" });
		deviceChoice16.setLabelString("");
		deviceChoice16.setOffsetNid(24);
		deviceChoice16.setChoiceFloatValues(null);
		deviceChoice16.setChoiceIntValues(null);
		jPanel14.setLayout(flowLayout17);
		deviceChannel6.setLabelString("");
		deviceChannel6.setOffsetNid(22);
		deviceChannel6.setInSameLine(true);
		deviceChannel6.setUpdateIdentifier("");
		deviceChannel6.setShowVal("");
		deviceChannel6.setLayout(flowLayout18);
		jLabel11.setText("2B");
		deviceChoice17.setChoiceIntValues(null);
		deviceChoice17.setChoiceFloatValues(null);
		deviceChoice17.setOffsetNid(25);
		deviceChoice17.setLabelString("");
		deviceChoice17.setChoiceItems(new String[]
		{ "INPUT", "ZERO", "REF", "BAT" });
		deviceChoice17.setUpdateIdentifier("");
		deviceChoice17.setIdentifier("");
		deviceField6.setOffsetNid(27);
		deviceField6.setTextOnly(false);
		deviceField6.setNumCols(25);
		deviceField6.setIdentifier("");
		jPanel15.setLayout(flowLayout19);
		deviceChoice18.setChoiceIntValues(null);
		deviceChoice18.setChoiceFloatValues(new float[]
		{ (float) 250.0, (float) 100.0, (float) 50.0, (float) 25.0, (float) 10.0, (float) 5.0, (float) 2.5, (float) 1.0,
				(float) 0.5 });
		deviceChoice18.setOffsetNid(23);
		deviceChoice18.setLabelString("");
		deviceChoice18.setChoiceItems(new String[]
		{ "250", "100", "50", "25", "10", "5", "2.5", "1", "0.5" });
		deviceChoice18.setUpdateIdentifier("");
		deviceChoice18.setIdentifier("");
		deviceChoice19.setIdentifier("");
		deviceChoice19.setUpdateIdentifier("");
		deviceChoice19.setChoiceItems(new String[]
		{ "AC", "DC" });
		deviceChoice19.setLabelString("");
		deviceChoice19.setOffsetNid(18);
		deviceChoice19.setChoiceFloatValues(null);
		deviceChoice19.setChoiceIntValues(null);
		jPanel16.setLayout(flowLayout110);
		deviceChannel7.setLabelString("");
		deviceChannel7.setOffsetNid(16);
		deviceChannel7.setInSameLine(true);
		deviceChannel7.setUpdateIdentifier("");
		deviceChannel7.setShowVal("");
		deviceChannel7.setLayout(flowLayout111);
		jLabel12.setText("2A");
		deviceChoice110.setChoiceIntValues(null);
		deviceChoice110.setChoiceFloatValues(null);
		deviceChoice110.setOffsetNid(19);
		deviceChoice110.setLabelString("");
		deviceChoice110.setChoiceItems(new String[]
		{ "INPUT", "ZERO", "REF", "BAT" });
		deviceChoice110.setUpdateIdentifier("");
		deviceChoice110.setIdentifier("");
		deviceField7.setOffsetNid(21);
		deviceField7.setTextOnly(false);
		deviceField7.setNumCols(25);
		deviceField7.setIdentifier("");
		jPanel17.setLayout(flowLayout112);
		deviceChoice111.setChoiceIntValues(null);
		deviceChoice111.setChoiceFloatValues(new float[]
		{ (float) 250.0, (float) 100.0, (float) 50.0, (float) 25.0, (float) 10.0, (float) 5.0, (float) 2.5, (float) 1.0,
				(float) 0.5 });
		deviceChoice111.setOffsetNid(17);
		deviceChoice111.setLabelString("");
		deviceChoice111.setChoiceItems(new String[]
		{ "250", "100", "50", "25", "10", "5", "2.5", "1", "0.5" });
		deviceChoice111.setUpdateIdentifier("");
		deviceChoice111.setIdentifier("");
		deviceChoice112.setIdentifier("");
		deviceChoice112.setUpdateIdentifier("");
		deviceChoice112.setChoiceItems(new String[]
		{ "AC", "DC" });
		deviceChoice112.setLabelString("");
		deviceChoice112.setOffsetNid(12);
		deviceChoice112.setChoiceFloatValues(null);
		deviceChoice112.setChoiceIntValues(null);
		jPanel18.setLayout(flowLayout113);
		deviceChannel8.setLabelString("");
		deviceChannel8.setOffsetNid(10);
		deviceChannel8.setInSameLine(true);
		deviceChannel8.setUpdateIdentifier("");
		deviceChannel8.setShowVal("");
		deviceChannel8.setLayout(flowLayout114);
		jLabel13.setText("1B");
		deviceChoice113.setChoiceIntValues(null);
		deviceChoice113.setChoiceFloatValues(null);
		deviceChoice113.setOffsetNid(13);
		deviceChoice113.setLabelString("");
		deviceChoice113.setChoiceItems(new String[]
		{ "INPUT", "ZERO", "REF", "BAT" });
		deviceChoice113.setUpdateIdentifier("");
		deviceChoice113.setIdentifier("");
		deviceField8.setOffsetNid(15);
		deviceField8.setTextOnly(false);
		deviceField8.setNumCols(25);
		deviceField8.setIdentifier("");
		jPanel19.setLayout(flowLayout115);
		deviceChoice114.setChoiceIntValues(null);
		deviceChoice114.setChoiceFloatValues(new float[]
		{ (float) 250.0, (float) 100.0, (float) 50.0, (float) 25.0, (float) 10.0, (float) 5.0, (float) 2.5, (float) 1.0,
				(float) 0.5 });
		deviceChoice114.setOffsetNid(11);
		deviceChoice114.setLabelString("");
		deviceChoice114.setChoiceItems(new String[]
		{ "250", "100", "50", "25", "10", "5", "2.5", "1", "0.5" });
		deviceChoice114.setUpdateIdentifier("");
		deviceChoice114.setIdentifier("");
		jPanel20.setLayout(flowLayout20);
		deviceButtons1.setCheckExpressions(null);
		deviceButtons1.setCheckMessages(null);
		deviceButtons1.setMethods(new String[]
		{ "INIT", "OFF", "CALIB", "RESET", "INFO" });
		jPanel21.setLayout(gridLayout2);
		gridLayout2.setRows(2);
		jPanel22.setLayout(flowLayout21);
		jLabel15.setHorizontalAlignment(SwingConstants.LEADING);
		jLabel15.setText("GPIB ADDR");
		jPanel25.setLayout(flowLayout23);
		jLabel16.setText("MODULE TYPE");
		jPanel26.setLayout(flowLayout24);
		deviceChoice20.setChoiceIntValues(null);
		deviceChoice20.setChoiceFloatValues(null);
		deviceChoice20.setOffsetNid(2);
		deviceChoice20.setLabelString("");
		deviceChoice20.setChoiceItems(new String[]
		{ "OAM01A", "OAM01B" });
		deviceChoice20.setUpdateIdentifier("");
		deviceChoice20.setIdentifier("");
		deviceField10.setOffsetNid(1);
		deviceField10.setNumCols(3);
		deviceField10.setIdentifier("");
		jLabel17.setPreferredSize(new Dimension(62, 16));
		jLabel17.setText("COMMENT");
		jPanel27.setLayout(flowLayout25);
		deviceField11.setOffsetNid(3);
		deviceField11.setTextOnly(true);
		deviceField11.setNumCols(30);
		deviceField11.setIdentifier("");
		jPanel23.setLayout(flowLayout26);
		jPanel1.setBorder(BorderFactory.createEtchedBorder(Color.white, new Color(142, 142, 142)));
		jScrollPane3.setBorder(BorderFactory.createEtchedBorder(Color.white, new Color(142, 142, 142)));
		jPanel24.setPreferredSize(new Dimension(240, 10));
		flowLayout25.setAlignment(FlowLayout.CENTER);
		flowLayout23.setAlignment(FlowLayout.CENTER);
		this.getContentPane().add(jPanel1, BorderLayout.CENTER);
		jPanel1.add(jScrollPane1, BorderLayout.NORTH);
		jScrollPane1.getViewport().add(jPanel2, null);
		jPanel2.add(jLabel2, null);
		jPanel2.add(jLabel3, null);
		jPanel2.add(jLabel4, null);
		jPanel2.add(jLabel5, null);
		jPanel2.add(jLabel6, null);
		jPanel1.add(jScrollPane2, BorderLayout.CENTER);
		this.getContentPane().add(jPanel20, BorderLayout.SOUTH);
		jPanel20.add(deviceButtons1, null);
		this.getContentPane().add(jScrollPane3, BorderLayout.NORTH);
		jScrollPane3.getViewport().add(jPanel21, null);
		jPanel21.add(jPanel22, null);
		jPanel22.add(jLabel15, null);
		jPanel22.add(jPanel25, null);
		jPanel25.add(deviceField10, null);
		jPanel22.add(jLabel16, null);
		jPanel22.add(jPanel26, null);
		jPanel26.add(deviceChoice20, null);
		jPanel22.add(jPanel24, null);
		jPanel21.add(jPanel23, null);
		jPanel23.add(jLabel17, null);
		jPanel23.add(jPanel27, null);
		jPanel27.add(deviceField11, null);
		jPanel23.add(deviceDispatch1, null);
		jScrollPane2.getViewport().add(jPanel3, null);
		jPanel3.add(jPanel4, null);
		jPanel4.add(jLabel1, null);
		jPanel4.add(deviceChannel1, null);
		deviceChannel1.add(jPanel5, null);
		jPanel5.add(deviceChoice3, null);
		jPanel5.add(deviceChoice2, null);
		jPanel5.add(deviceChoice1, null);
		jPanel5.add(deviceField1, null);
		jPanel3.add(jPanel18, null);
		jPanel18.add(jLabel13, null);
		jPanel18.add(deviceChannel8, null);
		deviceChannel8.add(jPanel19, null);
		jPanel19.add(deviceChoice114, null);
		jPanel19.add(deviceChoice112, null);
		jPanel19.add(deviceChoice113, null);
		jPanel19.add(deviceField8, null);
		jPanel3.add(jPanel16, null);
		jPanel16.add(jLabel12, null);
		jPanel16.add(deviceChannel7, null);
		deviceChannel7.add(jPanel17, null);
		jPanel17.add(deviceChoice111, null);
		jPanel17.add(deviceChoice19, null);
		jPanel17.add(deviceChoice110, null);
		jPanel17.add(deviceField7, null);
		jPanel3.add(jPanel14, null);
		jPanel14.add(jLabel11, null);
		jPanel14.add(deviceChannel6, null);
		deviceChannel6.add(jPanel15, null);
		jPanel15.add(deviceChoice18, null);
		jPanel15.add(deviceChoice16, null);
		jPanel15.add(deviceChoice17, null);
		jPanel15.add(deviceField6, null);
		jPanel3.add(jPanel12, null);
		jPanel12.add(jLabel10, null);
		jPanel12.add(deviceChannel5, null);
		deviceChannel5.add(jPanel13, null);
		jPanel13.add(deviceChoice15, null);
		jPanel13.add(deviceChoice13, null);
		jPanel13.add(deviceChoice14, null);
		jPanel13.add(deviceField5, null);
		jPanel3.add(jPanel10, null);
		jPanel10.add(jLabel9, null);
		jPanel10.add(deviceChannel4, null);
		deviceChannel4.add(jPanel11, null);
		jPanel11.add(deviceChoice12, null);
		jPanel11.add(deviceChoice10, null);
		jPanel11.add(deviceChoice11, null);
		jPanel11.add(deviceField4, null);
		jPanel3.add(jPanel8, null);
		jPanel8.add(jLabel8, null);
		jPanel8.add(deviceChannel3, null);
		deviceChannel3.add(jPanel9, null);
		jPanel9.add(deviceChoice9, null);
		jPanel9.add(deviceChoice7, null);
		jPanel9.add(deviceChoice8, null);
		jPanel9.add(deviceField3, null);
		jPanel3.add(jPanel6, null);
		jPanel6.add(jLabel7, null);
		jPanel6.add(deviceChannel2, null);
		deviceChannel2.add(jPanel7, null);
		jPanel7.add(deviceChoice6, null);
		jPanel7.add(deviceChoice4, null);
		jPanel7.add(deviceChoice5, null);
		jPanel7.add(deviceField2, null);
	}
}

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
public class ISIS_GAINSetup extends DeviceSetup
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
	DeviceField deviceField1 = new DeviceField();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	FlowLayout flowLayout1 = new FlowLayout();
	JPanel jPanel3 = new JPanel();
	DeviceField deviceField2 = new DeviceField();
	FlowLayout flowLayout2 = new FlowLayout();
	JScrollPane jScrollPane1 = new JScrollPane();
	JPanel jPanel4 = new JPanel();
	GridLayout gridLayout2 = new GridLayout();
	DeviceChannel deviceChannel1 = new DeviceChannel();
	FlowLayout flowLayout3 = new FlowLayout();
	JPanel jPanel5 = new JPanel();
	FlowLayout flowLayout4 = new FlowLayout();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceField deviceField3 = new DeviceField();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceField deviceField5 = new DeviceField();
	FlowLayout flowLayout5 = new FlowLayout();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();
	DeviceChoice deviceChoice3 = new DeviceChoice();
	DeviceChannel deviceChannel2 = new DeviceChannel();
	FlowLayout flowLayout6 = new FlowLayout();
	JPanel jPanel6 = new JPanel();
	DeviceChoice deviceChoice4 = new DeviceChoice();
	FlowLayout flowLayout7 = new FlowLayout();
	DeviceField deviceField11 = new DeviceField();
	DeviceField deviceField12 = new DeviceField();
	DeviceChoice deviceChoice5 = new DeviceChoice();
	DeviceChannel deviceChannel3 = new DeviceChannel();
	FlowLayout flowLayout8 = new FlowLayout();
	JPanel jPanel7 = new JPanel();
	DeviceChoice deviceChoice6 = new DeviceChoice();
	FlowLayout flowLayout9 = new FlowLayout();
	DeviceField deviceField15 = new DeviceField();
	DeviceField deviceField16 = new DeviceField();
	DeviceChoice deviceChoice7 = new DeviceChoice();
	DeviceChannel deviceChannel4 = new DeviceChannel();
	FlowLayout flowLayout10 = new FlowLayout();
	JPanel jPanel8 = new JPanel();
	DeviceChoice deviceChoice8 = new DeviceChoice();
	FlowLayout flowLayout11 = new FlowLayout();
	DeviceField deviceField19 = new DeviceField();
	DeviceField deviceField110 = new DeviceField();
	DeviceChoice deviceChoice9 = new DeviceChoice();
	DeviceChannel deviceChannel5 = new DeviceChannel();
	FlowLayout flowLayout12 = new FlowLayout();
	JPanel jPanel9 = new JPanel();
	DeviceChoice deviceChoice10 = new DeviceChoice();
	FlowLayout flowLayout13 = new FlowLayout();
	DeviceField deviceField113 = new DeviceField();
	DeviceField deviceField114 = new DeviceField();
	DeviceChoice deviceChoice11 = new DeviceChoice();
	DeviceChannel deviceChannel6 = new DeviceChannel();
	FlowLayout flowLayout14 = new FlowLayout();
	JPanel jPanel10 = new JPanel();
	DeviceChoice deviceChoice12 = new DeviceChoice();
	FlowLayout flowLayout15 = new FlowLayout();
	DeviceField deviceField117 = new DeviceField();
	DeviceField deviceField118 = new DeviceField();
	DeviceChoice deviceChoice13 = new DeviceChoice();
	DeviceChannel deviceChannel7 = new DeviceChannel();
	FlowLayout flowLayout16 = new FlowLayout();
	JPanel jPanel11 = new JPanel();
	DeviceChoice deviceChoice14 = new DeviceChoice();
	FlowLayout flowLayout17 = new FlowLayout();
	DeviceField deviceField1111 = new DeviceField();
	DeviceField deviceField1112 = new DeviceField();
	DeviceChoice deviceChoice15 = new DeviceChoice();
	DeviceChannel deviceChannel8 = new DeviceChannel();
	FlowLayout flowLayout18 = new FlowLayout();
	JPanel jPanel12 = new JPanel();
	DeviceChoice deviceChoice16 = new DeviceChoice();
	FlowLayout flowLayout19 = new FlowLayout();
	DeviceField deviceField1115 = new DeviceField();
	DeviceField deviceField1116 = new DeviceField();
	DeviceChoice deviceChoice17 = new DeviceChoice();
	DeviceChannel deviceChannel9 = new DeviceChannel();
	FlowLayout flowLayout110 = new FlowLayout();
	JPanel jPanel13 = new JPanel();
	DeviceChoice deviceChoice18 = new DeviceChoice();
	FlowLayout flowLayout111 = new FlowLayout();
	DeviceField deviceField1119 = new DeviceField();
	DeviceField deviceField11110 = new DeviceField();
	DeviceChoice deviceChoice19 = new DeviceChoice();
	DeviceChannel deviceChannel10 = new DeviceChannel();
	FlowLayout flowLayout112 = new FlowLayout();
	JPanel jPanel14 = new JPanel();
	DeviceChoice deviceChoice110 = new DeviceChoice();
	FlowLayout flowLayout113 = new FlowLayout();
	DeviceField deviceField11113 = new DeviceField();
	DeviceField deviceField11114 = new DeviceField();
	DeviceChoice deviceChoice111 = new DeviceChoice();
	DeviceChannel deviceChannel11 = new DeviceChannel();
	FlowLayout flowLayout114 = new FlowLayout();
	JPanel jPanel15 = new JPanel();
	DeviceChoice deviceChoice112 = new DeviceChoice();
	FlowLayout flowLayout115 = new FlowLayout();
	DeviceField deviceField11117 = new DeviceField();
	DeviceField deviceField11118 = new DeviceField();
	DeviceChoice deviceChoice113 = new DeviceChoice();
	DeviceChannel deviceChannel12 = new DeviceChannel();
	FlowLayout flowLayout116 = new FlowLayout();
	JPanel jPanel16 = new JPanel();
	DeviceChoice deviceChoice114 = new DeviceChoice();
	FlowLayout flowLayout117 = new FlowLayout();
	DeviceField deviceField111111 = new DeviceField();
	DeviceField deviceField111112 = new DeviceField();
	DeviceChoice deviceChoice115 = new DeviceChoice();
	DeviceChannel deviceChannel13 = new DeviceChannel();
	FlowLayout flowLayout118 = new FlowLayout();
	JPanel jPanel17 = new JPanel();
	DeviceChoice deviceChoice116 = new DeviceChoice();
	FlowLayout flowLayout119 = new FlowLayout();
	DeviceField deviceField111115 = new DeviceField();
	DeviceField deviceField111116 = new DeviceField();
	DeviceChoice deviceChoice117 = new DeviceChoice();
	DeviceChannel deviceChannel14 = new DeviceChannel();
	FlowLayout flowLayout1110 = new FlowLayout();
	JPanel jPanel18 = new JPanel();
	DeviceChoice deviceChoice118 = new DeviceChoice();
	FlowLayout flowLayout1111 = new FlowLayout();
	DeviceField deviceField111119 = new DeviceField();
	DeviceField deviceField1111110 = new DeviceField();
	DeviceChoice deviceChoice119 = new DeviceChoice();
	DeviceChannel deviceChannel15 = new DeviceChannel();
	FlowLayout flowLayout1112 = new FlowLayout();
	JPanel jPanel19 = new JPanel();
	DeviceChoice deviceChoice1110 = new DeviceChoice();
	FlowLayout flowLayout1113 = new FlowLayout();
	DeviceField deviceField1111113 = new DeviceField();
	DeviceField deviceField1111114 = new DeviceField();
	DeviceChoice deviceChoice1111 = new DeviceChoice();
	DeviceChannel deviceChannel16 = new DeviceChannel();
	FlowLayout flowLayout1114 = new FlowLayout();
	JPanel jPanel110 = new JPanel();
	DeviceChoice deviceChoice1112 = new DeviceChoice();
	FlowLayout flowLayout1115 = new FlowLayout();
	DeviceField deviceField1111117 = new DeviceField();
	DeviceField deviceField1111118 = new DeviceField();
	DeviceChoice deviceChoice1113 = new DeviceChoice();
	DeviceChannel deviceChannel17 = new DeviceChannel();
	FlowLayout flowLayout1116 = new FlowLayout();
	JPanel jPanel111 = new JPanel();
	DeviceChoice deviceChoice1114 = new DeviceChoice();
	FlowLayout flowLayout1117 = new FlowLayout();
	DeviceField deviceField11111111 = new DeviceField();
	DeviceField deviceField11111112 = new DeviceField();
	DeviceChoice deviceChoice1115 = new DeviceChoice();
	DeviceChannel deviceChannel18 = new DeviceChannel();
	FlowLayout flowLayout1118 = new FlowLayout();
	JPanel jPanel112 = new JPanel();
	DeviceChoice deviceChoice1116 = new DeviceChoice();
	DeviceLabel deviceLabel1 = new DeviceLabel();
	DeviceLabel deviceLabel2 = new DeviceLabel();
	DeviceLabel deviceLabel3 = new DeviceLabel();
	DeviceLabel deviceLabel4 = new DeviceLabel();
	DeviceLabel deviceLabel5 = new DeviceLabel();
	DeviceLabel deviceLabel6 = new DeviceLabel();
	DeviceLabel deviceLabel7 = new DeviceLabel();
	DeviceLabel deviceLabel8 = new DeviceLabel();
	DeviceLabel deviceLabel9 = new DeviceLabel();
	DeviceLabel deviceLabel10 = new DeviceLabel();
	DeviceLabel deviceLabel11 = new DeviceLabel();
	DeviceLabel deviceLabel12 = new DeviceLabel();
	DeviceLabel deviceLabel13 = new DeviceLabel();
	DeviceLabel deviceLabel14 = new DeviceLabel();
	DeviceLabel deviceLabel15 = new DeviceLabel();
	DeviceLabel deviceLabel16 = new DeviceLabel();
	DeviceLabel deviceLabel17 = new DeviceLabel();
	DeviceLabel deviceLabel18 = new DeviceLabel();
	DeviceLabel deviceLabel19 = new DeviceLabel();
	DeviceLabel deviceLabel20 = new DeviceLabel();
	DeviceLabel deviceLabel21 = new DeviceLabel();
	DeviceLabel deviceLabel22 = new DeviceLabel();
	DeviceLabel deviceLabel23 = new DeviceLabel();
	DeviceLabel deviceLabel24 = new DeviceLabel();
	DeviceLabel deviceLabel25 = new DeviceLabel();
	DeviceLabel deviceLabel26 = new DeviceLabel();
	DeviceLabel deviceLabel27 = new DeviceLabel();
	DeviceLabel deviceLabel28 = new DeviceLabel();
	DeviceLabel deviceLabel29 = new DeviceLabel();
	DeviceLabel deviceLabel30 = new DeviceLabel();
	DeviceLabel deviceLabel31 = new DeviceLabel();
	DeviceLabel deviceLabel32 = new DeviceLabel();
	DeviceLabel deviceLabel33 = new DeviceLabel();
	DeviceLabel deviceLabel34 = new DeviceLabel();
	DeviceLabel deviceLabel35 = new DeviceLabel();
	DeviceLabel deviceLabel36 = new DeviceLabel();
	JPanel jPanel20 = new JPanel();
	JLabel jLabel7 = new JLabel();
	JPanel jPanel21 = new JPanel();
	JLabel jLabel4 = new JLabel();
	JLabel jLabel2 = new JLabel();
	JLabel jLabel6 = new JLabel();
	JLabel jLabel1 = new JLabel();
	JLabel jLabel3 = new JLabel();
	JLabel jLabel5 = new JLabel();
	FlowLayout flowLayout21 = new FlowLayout();
	BorderLayout borderLayout2 = new BorderLayout();

	public ISIS_GAINSetup()
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
		this.setHeight(600);
		this.setDeviceType("ISIS_GAIN");
		this.setDeviceProvider("150.178.3.33");
		this.setDeviceTitle("ISIS Front End Gains");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setRows(3);
		deviceField1.setOffsetNid(1);
		deviceField1.setLabelString("Comment: ");
		deviceField1.setNumCols(30);
		deviceField1.setIdentifier("");
		jPanel2.setLayout(flowLayout1);
		flowLayout1.setAlignment(FlowLayout.LEFT);
		deviceField2.setOffsetNid(2);
		deviceField2.setLabelString("IP address: ");
		deviceField2.setNumCols(20);
		deviceField2.setIdentifier("");
		jPanel3.setLayout(flowLayout2);
		flowLayout2.setAlignment(FlowLayout.LEFT);
		jPanel4.setLayout(gridLayout2);
		gridLayout2.setRows(18);
		gridLayout2.setVgap(0);
		deviceChannel1.setLayout(flowLayout3);
		deviceChannel1.setLabelString("01");
		deviceChannel1.setOffsetNid(4);
		deviceChannel1.setInSameLine(true);
		deviceChannel1.setUpdateIdentifier("");
		deviceChannel1.setShowVal("");
		jPanel5.setLayout(flowLayout4);
		deviceChoice2.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice2.setChoiceFloatValues(null);
		deviceChoice2.setOffsetNid(7);
		deviceChoice2.setLabelString(" ");
		deviceChoice2.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		deviceChoice1.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setOffsetNid(12);
		deviceChoice1.setLabelString(" ");
		deviceChoice1.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceField5.setOffsetNid(6);
		deviceField5.setIdentifier("");
		deviceField3.setOffsetNid(11);
		deviceField3.setIdentifier("");
		deviceField7.setIdentifier("");
		deviceField7.setOffsetNid(193);
		deviceField8.setIdentifier("");
		deviceField8.setOffsetNid(198);
		deviceChoice3.setIdentifier("");
		deviceChoice3.setUpdateIdentifier("");
		deviceChoice3.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice3.setLabelString("");
		deviceChoice3.setOffsetNid(194);
		deviceChoice3.setChoiceFloatValues(null);
		deviceChoice3.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel2.setShowVal("");
		deviceChannel2.setUpdateIdentifier("");
		deviceChannel2.setInSameLine(true);
		deviceChannel2.setOffsetNid(191);
		deviceChannel2.setLabelString("18");
		deviceChannel2.setLayout(flowLayout5);
		jPanel6.setLayout(flowLayout6);
		deviceChoice4.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice4.setChoiceFloatValues(null);
		deviceChoice4.setOffsetNid(199);
		deviceChoice4.setLabelString(" ");
		deviceChoice4.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice4.setUpdateIdentifier("");
		deviceChoice4.setIdentifier("");
		deviceField11.setIdentifier("");
		deviceField11.setOffsetNid(182);
		deviceField12.setIdentifier("");
		deviceField12.setOffsetNid(187);
		deviceChoice5.setIdentifier("");
		deviceChoice5.setUpdateIdentifier("");
		deviceChoice5.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice5.setLabelString(" ");
		deviceChoice5.setOffsetNid(183);
		deviceChoice5.setChoiceFloatValues(null);
		deviceChoice5.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel3.setShowVal("");
		deviceChannel3.setUpdateIdentifier("");
		deviceChannel3.setInSameLine(true);
		deviceChannel3.setOffsetNid(180);
		deviceChannel3.setLabelString("17");
		deviceChannel3.setLayout(flowLayout7);
		jPanel7.setLayout(flowLayout8);
		deviceChoice6.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice6.setChoiceFloatValues(null);
		deviceChoice6.setOffsetNid(188);
		deviceChoice6.setLabelString(" ");
		deviceChoice6.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice6.setUpdateIdentifier("");
		deviceChoice6.setIdentifier("");
		deviceField15.setIdentifier("");
		deviceField15.setOffsetNid(171);
		deviceField16.setIdentifier("");
		deviceField16.setOffsetNid(176);
		deviceChoice7.setIdentifier("");
		deviceChoice7.setUpdateIdentifier("");
		deviceChoice7.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice7.setLabelString(" ");
		deviceChoice7.setOffsetNid(172);
		deviceChoice7.setChoiceFloatValues(null);
		deviceChoice7.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel4.setShowVal("");
		deviceChannel4.setUpdateIdentifier("");
		deviceChannel4.setInSameLine(true);
		deviceChannel4.setOffsetNid(169);
		deviceChannel4.setLabelString("16");
		deviceChannel4.setLayout(flowLayout9);
		jPanel8.setLayout(flowLayout10);
		deviceChoice8.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice8.setChoiceFloatValues(null);
		deviceChoice8.setOffsetNid(177);
		deviceChoice8.setLabelString(" ");
		deviceChoice8.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice8.setUpdateIdentifier("");
		deviceChoice8.setIdentifier("");
		deviceField19.setIdentifier("");
		deviceField19.setOffsetNid(160);
		deviceField110.setIdentifier("");
		deviceField110.setOffsetNid(165);
		deviceChoice9.setIdentifier("");
		deviceChoice9.setUpdateIdentifier("");
		deviceChoice9.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice9.setLabelString(" ");
		deviceChoice9.setOffsetNid(161);
		deviceChoice9.setChoiceFloatValues(null);
		deviceChoice9.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel5.setShowVal("");
		deviceChannel5.setUpdateIdentifier("");
		deviceChannel5.setInSameLine(true);
		deviceChannel5.setOffsetNid(158);
		deviceChannel5.setLabelString("15");
		deviceChannel5.setLayout(flowLayout11);
		jPanel9.setLayout(flowLayout12);
		deviceChoice10.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice10.setChoiceFloatValues(null);
		deviceChoice10.setOffsetNid(166);
		deviceChoice10.setLabelString(" ");
		deviceChoice10.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice10.setUpdateIdentifier("");
		deviceChoice10.setIdentifier("");
		deviceField113.setIdentifier("");
		deviceField113.setOffsetNid(149);
		deviceField114.setIdentifier("");
		deviceField114.setOffsetNid(154);
		deviceChoice11.setIdentifier("");
		deviceChoice11.setUpdateIdentifier("");
		deviceChoice11.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice11.setLabelString(" ");
		deviceChoice11.setOffsetNid(150);
		deviceChoice11.setChoiceFloatValues(null);
		deviceChoice11.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel6.setShowVal("");
		deviceChannel6.setUpdateIdentifier("");
		deviceChannel6.setInSameLine(true);
		deviceChannel6.setOffsetNid(147);
		deviceChannel6.setLabelString("14");
		deviceChannel6.setLayout(flowLayout13);
		jPanel10.setLayout(flowLayout14);
		deviceChoice12.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice12.setChoiceFloatValues(null);
		deviceChoice12.setOffsetNid(155);
		deviceChoice12.setLabelString(" ");
		deviceChoice12.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice12.setUpdateIdentifier("");
		deviceChoice12.setIdentifier("");
		deviceField117.setIdentifier("");
		deviceField117.setOffsetNid(138);
		deviceField118.setIdentifier("");
		deviceField118.setOffsetNid(143);
		deviceChoice13.setIdentifier("");
		deviceChoice13.setUpdateIdentifier("");
		deviceChoice13.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice13.setLabelString(" ");
		deviceChoice13.setOffsetNid(139);
		deviceChoice13.setChoiceFloatValues(null);
		deviceChoice13.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel7.setShowVal("");
		deviceChannel7.setUpdateIdentifier("");
		deviceChannel7.setInSameLine(true);
		deviceChannel7.setOffsetNid(136);
		deviceChannel7.setLabelString("13");
		deviceChannel7.setLayout(flowLayout15);
		jPanel11.setLayout(flowLayout16);
		deviceChoice14.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice14.setChoiceFloatValues(null);
		deviceChoice14.setOffsetNid(144);
		deviceChoice14.setLabelString("");
		deviceChoice14.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice14.setUpdateIdentifier("");
		deviceChoice14.setIdentifier("");
		deviceField1111.setIdentifier("");
		deviceField1111.setOffsetNid(127);
		deviceField1112.setIdentifier("");
		deviceField1112.setOffsetNid(132);
		deviceChoice15.setIdentifier("");
		deviceChoice15.setUpdateIdentifier("");
		deviceChoice15.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice15.setLabelString(" ");
		deviceChoice15.setOffsetNid(128);
		deviceChoice15.setChoiceFloatValues(null);
		deviceChoice15.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel8.setShowVal("");
		deviceChannel8.setUpdateIdentifier("");
		deviceChannel8.setInSameLine(true);
		deviceChannel8.setOffsetNid(125);
		deviceChannel8.setLabelString("12");
		deviceChannel8.setLayout(flowLayout17);
		jPanel12.setLayout(flowLayout18);
		deviceChoice16.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice16.setChoiceFloatValues(null);
		deviceChoice16.setOffsetNid(133);
		deviceChoice16.setLabelString(" ");
		deviceChoice16.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice16.setUpdateIdentifier("");
		deviceChoice16.setIdentifier("");
		deviceField1115.setIdentifier("");
		deviceField1115.setOffsetNid(116);
		deviceField1116.setIdentifier("");
		deviceField1116.setOffsetNid(121);
		deviceChoice17.setIdentifier("");
		deviceChoice17.setUpdateIdentifier("");
		deviceChoice17.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice17.setLabelString(" ");
		deviceChoice17.setOffsetNid(117);
		deviceChoice17.setChoiceFloatValues(null);
		deviceChoice17.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel9.setShowVal("");
		deviceChannel9.setUpdateIdentifier("");
		deviceChannel9.setInSameLine(true);
		deviceChannel9.setOffsetNid(114);
		deviceChannel9.setLabelString("11");
		deviceChannel9.setLayout(flowLayout19);
		jPanel13.setLayout(flowLayout110);
		deviceChoice18.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice18.setChoiceFloatValues(null);
		deviceChoice18.setOffsetNid(122);
		deviceChoice18.setLabelString(" ");
		deviceChoice18.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice18.setUpdateIdentifier("");
		deviceChoice18.setIdentifier("");
		deviceField1119.setIdentifier("");
		deviceField1119.setOffsetNid(105);
		deviceField11110.setIdentifier("");
		deviceField11110.setOffsetNid(110);
		deviceChoice19.setIdentifier("");
		deviceChoice19.setUpdateIdentifier("");
		deviceChoice19.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice19.setLabelString(" ");
		deviceChoice19.setOffsetNid(106);
		deviceChoice19.setChoiceFloatValues(null);
		deviceChoice19.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel10.setShowVal("");
		deviceChannel10.setUpdateIdentifier("");
		deviceChannel10.setInSameLine(true);
		deviceChannel10.setOffsetNid(103);
		deviceChannel10.setLabelString("10");
		deviceChannel10.setLayout(flowLayout111);
		jPanel14.setLayout(flowLayout112);
		deviceChoice110.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice110.setChoiceFloatValues(null);
		deviceChoice110.setOffsetNid(111);
		deviceChoice110.setLabelString(" ");
		deviceChoice110.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice110.setUpdateIdentifier("");
		deviceChoice110.setIdentifier("");
		deviceField11113.setIdentifier("");
		deviceField11113.setOffsetNid(94);
		deviceField11114.setIdentifier("");
		deviceField11114.setOffsetNid(99);
		deviceChoice111.setIdentifier("");
		deviceChoice111.setUpdateIdentifier("");
		deviceChoice111.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice111.setLabelString("");
		deviceChoice111.setOffsetNid(95);
		deviceChoice111.setChoiceFloatValues(null);
		deviceChoice111.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel11.setShowVal("");
		deviceChannel11.setUpdateIdentifier("");
		deviceChannel11.setInSameLine(true);
		deviceChannel11.setOffsetNid(92);
		deviceChannel11.setLabelString("09");
		deviceChannel11.setLayout(flowLayout113);
		jPanel15.setLayout(flowLayout114);
		deviceChoice112.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice112.setChoiceFloatValues(null);
		deviceChoice112.setOffsetNid(100);
		deviceChoice112.setLabelString(" ");
		deviceChoice112.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice112.setUpdateIdentifier("");
		deviceChoice112.setIdentifier("");
		deviceField11117.setIdentifier("");
		deviceField11117.setOffsetNid(83);
		deviceField11118.setIdentifier("");
		deviceField11118.setOffsetNid(88);
		deviceChoice113.setIdentifier("");
		deviceChoice113.setUpdateIdentifier("");
		deviceChoice113.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice113.setLabelString(" ");
		deviceChoice113.setOffsetNid(84);
		deviceChoice113.setChoiceFloatValues(null);
		deviceChoice113.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel12.setShowVal("");
		deviceChannel12.setUpdateIdentifier("");
		deviceChannel12.setInSameLine(true);
		deviceChannel12.setOffsetNid(81);
		deviceChannel12.setLabelString("08");
		deviceChannel12.setLayout(flowLayout115);
		jPanel16.setLayout(flowLayout116);
		deviceChoice114.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice114.setChoiceFloatValues(null);
		deviceChoice114.setOffsetNid(89);
		deviceChoice114.setLabelString(" ");
		deviceChoice114.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice114.setUpdateIdentifier("");
		deviceChoice114.setIdentifier("");
		deviceField111111.setIdentifier("");
		deviceField111111.setOffsetNid(72);
		deviceField111112.setIdentifier("");
		deviceField111112.setOffsetNid(77);
		deviceChoice115.setIdentifier("");
		deviceChoice115.setUpdateIdentifier("");
		deviceChoice115.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice115.setLabelString(" ");
		deviceChoice115.setOffsetNid(73);
		deviceChoice115.setChoiceFloatValues(null);
		deviceChoice115.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel13.setShowVal("");
		deviceChannel13.setUpdateIdentifier("");
		deviceChannel13.setInSameLine(true);
		deviceChannel13.setOffsetNid(70);
		deviceChannel13.setLabelString("07");
		deviceChannel13.setLayout(flowLayout117);
		jPanel17.setLayout(flowLayout118);
		deviceChoice116.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice116.setChoiceFloatValues(null);
		deviceChoice116.setOffsetNid(78);
		deviceChoice116.setLabelString(" ");
		deviceChoice116.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice116.setUpdateIdentifier("");
		deviceChoice116.setIdentifier("");
		deviceField111115.setIdentifier("");
		deviceField111115.setOffsetNid(61);
		deviceField111116.setIdentifier("");
		deviceField111116.setOffsetNid(66);
		deviceChoice117.setIdentifier("");
		deviceChoice117.setUpdateIdentifier("");
		deviceChoice117.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice117.setLabelString(" ");
		deviceChoice117.setOffsetNid(62);
		deviceChoice117.setChoiceFloatValues(null);
		deviceChoice117.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel14.setShowVal("");
		deviceChannel14.setUpdateIdentifier("");
		deviceChannel14.setInSameLine(true);
		deviceChannel14.setOffsetNid(59);
		deviceChannel14.setLabelString("06");
		deviceChannel14.setLayout(flowLayout119);
		jPanel18.setLayout(flowLayout1110);
		deviceChoice118.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice118.setChoiceFloatValues(null);
		deviceChoice118.setOffsetNid(67);
		deviceChoice118.setLabelString(" ");
		deviceChoice118.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice118.setUpdateIdentifier("");
		deviceChoice118.setIdentifier("");
		deviceField111119.setIdentifier("");
		deviceField111119.setOffsetNid(50);
		deviceField1111110.setIdentifier("");
		deviceField1111110.setOffsetNid(55);
		deviceChoice119.setIdentifier("");
		deviceChoice119.setUpdateIdentifier("");
		deviceChoice119.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice119.setLabelString(" ");
		deviceChoice119.setOffsetNid(51);
		deviceChoice119.setChoiceFloatValues(null);
		deviceChoice119.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel15.setShowVal("");
		deviceChannel15.setUpdateIdentifier("");
		deviceChannel15.setInSameLine(true);
		deviceChannel15.setOffsetNid(48);
		deviceChannel15.setLabelString("05");
		deviceChannel15.setLayout(flowLayout1111);
		jPanel19.setLayout(flowLayout1112);
		deviceChoice1110.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice1110.setChoiceFloatValues(null);
		deviceChoice1110.setOffsetNid(56);
		deviceChoice1110.setLabelString("");
		deviceChoice1110.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice1110.setUpdateIdentifier("");
		deviceChoice1110.setIdentifier("");
		deviceField1111113.setIdentifier("");
		deviceField1111113.setOffsetNid(39);
		deviceField1111114.setIdentifier("");
		deviceField1111114.setOffsetNid(44);
		deviceChoice1111.setIdentifier("");
		deviceChoice1111.setUpdateIdentifier("");
		deviceChoice1111.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice1111.setLabelString(" ");
		deviceChoice1111.setOffsetNid(40);
		deviceChoice1111.setChoiceFloatValues(null);
		deviceChoice1111.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel16.setShowVal("");
		deviceChannel16.setUpdateIdentifier("");
		deviceChannel16.setInSameLine(true);
		deviceChannel16.setOffsetNid(37);
		deviceChannel16.setLabelString("04");
		deviceChannel16.setLayout(flowLayout1113);
		jPanel110.setLayout(flowLayout1114);
		deviceChoice1112.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice1112.setChoiceFloatValues(null);
		deviceChoice1112.setOffsetNid(45);
		deviceChoice1112.setLabelString(" ");
		deviceChoice1112.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice1112.setUpdateIdentifier("");
		deviceChoice1112.setIdentifier("");
		deviceField1111117.setIdentifier("");
		deviceField1111117.setOffsetNid(28);
		deviceField1111118.setIdentifier("");
		deviceField1111118.setOffsetNid(33);
		deviceChoice1113.setIdentifier("");
		deviceChoice1113.setUpdateIdentifier("");
		deviceChoice1113.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice1113.setLabelString("");
		deviceChoice1113.setOffsetNid(29);
		deviceChoice1113.setChoiceFloatValues(null);
		deviceChoice1113.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel17.setShowVal("");
		deviceChannel17.setUpdateIdentifier("");
		deviceChannel17.setInSameLine(true);
		deviceChannel17.setOffsetNid(26);
		deviceChannel17.setLabelString("03");
		deviceChannel17.setLayout(flowLayout1115);
		jPanel111.setLayout(flowLayout1116);
		deviceChoice1114.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice1114.setChoiceFloatValues(null);
		deviceChoice1114.setOffsetNid(34);
		deviceChoice1114.setLabelString(" ");
		deviceChoice1114.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice1114.setUpdateIdentifier("");
		deviceChoice1114.setIdentifier("");
		deviceField11111111.setIdentifier("");
		deviceField11111111.setOffsetNid(17);
		deviceField11111112.setIdentifier("");
		deviceField11111112.setOffsetNid(22);
		deviceChoice1115.setIdentifier("");
		deviceChoice1115.setUpdateIdentifier("");
		deviceChoice1115.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice1115.setLabelString(" ");
		deviceChoice1115.setOffsetNid(18);
		deviceChoice1115.setChoiceFloatValues(null);
		deviceChoice1115.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChannel18.setShowVal("");
		deviceChannel18.setUpdateIdentifier("");
		deviceChannel18.setInSameLine(true);
		deviceChannel18.setOffsetNid(15);
		deviceChannel18.setLabelString("02");
		deviceChannel18.setLayout(flowLayout1117);
		jPanel112.setLayout(flowLayout1118);
		deviceChoice1116.setChoiceIntValues(new int[]
		{ 1, 2, 5, 10 });
		deviceChoice1116.setChoiceFloatValues(null);
		deviceChoice1116.setOffsetNid(23);
		deviceChoice1116.setLabelString(" ");
		deviceChoice1116.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice1116.setUpdateIdentifier("");
		deviceChoice1116.setIdentifier("");
		jPanel19.setAlignmentY((float) 0.5);
		deviceButtons1.setMethods(new String[]
		{ "init" });
		flowLayout1113.setVgap(0);
		flowLayout3.setVgap(5);
		flowLayout4.setVgap(5);
		flowLayout1117.setVgap(5);
		flowLayout1118.setVgap(5);
		deviceLabel1.setOffsetNid(5);
		deviceLabel1.setLabelString("");
		deviceLabel1.setNumCols(15);
		deviceLabel1.setIdentifier("");
		deviceLabel2.setIdentifier("");
		deviceLabel2.setNumCols(15);
		deviceLabel2.setLabelString("");
		deviceLabel2.setOffsetNid(16);
		deviceLabel3.setIdentifier("");
		deviceLabel3.setNumCols(15);
		deviceLabel3.setLabelString("");
		deviceLabel3.setOffsetNid(27);
		deviceLabel4.setIdentifier("");
		deviceLabel4.setNumCols(15);
		deviceLabel4.setLabelString("");
		deviceLabel4.setOffsetNid(38);
		deviceLabel5.setIdentifier("");
		deviceLabel5.setNumCols(15);
		deviceLabel5.setLabelString("");
		deviceLabel5.setOffsetNid(49);
		deviceLabel6.setIdentifier("");
		deviceLabel6.setNumCols(15);
		deviceLabel6.setLabelString("");
		deviceLabel6.setOffsetNid(60);
		deviceLabel7.setIdentifier("");
		deviceLabel7.setNumCols(15);
		deviceLabel7.setLabelString("");
		deviceLabel7.setOffsetNid(71);
		deviceLabel8.setIdentifier("");
		deviceLabel8.setNumCols(15);
		deviceLabel8.setLabelString("");
		deviceLabel8.setOffsetNid(82);
		deviceLabel9.setIdentifier("");
		deviceLabel9.setNumCols(15);
		deviceLabel9.setLabelString("");
		deviceLabel9.setOffsetNid(93);
		deviceLabel10.setIdentifier("");
		deviceLabel10.setNumCols(15);
		deviceLabel10.setLabelString("");
		deviceLabel10.setOffsetNid(104);
		deviceLabel11.setIdentifier("");
		deviceLabel11.setNumCols(15);
		deviceLabel11.setLabelString("");
		deviceLabel11.setOffsetNid(115);
		deviceLabel12.setIdentifier("");
		deviceLabel12.setNumCols(15);
		deviceLabel12.setLabelString("");
		deviceLabel12.setOffsetNid(126);
		deviceLabel13.setIdentifier("");
		deviceLabel13.setNumCols(15);
		deviceLabel13.setLabelString("");
		deviceLabel13.setOffsetNid(137);
		deviceLabel14.setIdentifier("");
		deviceLabel14.setNumCols(15);
		deviceLabel14.setLabelString("");
		deviceLabel14.setOffsetNid(148);
		deviceLabel15.setIdentifier("");
		deviceLabel15.setNumCols(15);
		deviceLabel15.setLabelString("");
		deviceLabel15.setOffsetNid(159);
		deviceLabel16.setIdentifier("");
		deviceLabel16.setNumCols(15);
		deviceLabel16.setLabelString("");
		deviceLabel16.setOffsetNid(170);
		deviceLabel17.setIdentifier("");
		deviceLabel17.setNumCols(15);
		deviceLabel17.setLabelString("");
		deviceLabel17.setOffsetNid(181);
		deviceLabel18.setIdentifier("");
		deviceLabel18.setNumCols(15);
		deviceLabel18.setLabelString("");
		deviceLabel18.setOffsetNid(192);
		deviceLabel19.setOffsetNid(10);
		deviceLabel19.setNumCols(15);
		deviceLabel20.setOffsetNid(21);
		deviceLabel20.setNumCols(15);
		deviceLabel21.setOffsetNid(32);
		deviceLabel21.setNumCols(15);
		deviceLabel22.setOffsetNid(43);
		deviceLabel22.setNumCols(15);
		deviceLabel23.setOffsetNid(54);
		deviceLabel23.setNumCols(15);
		deviceLabel24.setOffsetNid(65);
		deviceLabel24.setNumCols(15);
		deviceLabel25.setOffsetNid(76);
		deviceLabel25.setNumCols(15);
		deviceLabel26.setOffsetNid(87);
		deviceLabel26.setNumCols(15);
		deviceLabel27.setOffsetNid(98);
		deviceLabel27.setNumCols(15);
		deviceLabel28.setOffsetNid(109);
		deviceLabel28.setNumCols(15);
		deviceLabel29.setOffsetNid(120);
		deviceLabel29.setNumCols(15);
		deviceLabel30.setOffsetNid(131);
		deviceLabel30.setNumCols(15);
		deviceLabel31.setOffsetNid(142);
		deviceLabel31.setNumCols(15);
		deviceLabel32.setOffsetNid(153);
		deviceLabel32.setNumCols(15);
		deviceLabel33.setOffsetNid(164);
		deviceLabel33.setNumCols(15);
		deviceLabel34.setOffsetNid(175);
		deviceLabel34.setNumCols(15);
		deviceLabel35.setOffsetNid(186);
		deviceLabel35.setNumCols(15);
		deviceLabel36.setOffsetNid(197);
		deviceLabel36.setNumCols(15);
		/*
		 * deviceLabel1.setTextOnly(true); deviceLabel2.setTextOnly(true);
		 * deviceLabel3.setTextOnly(true); deviceLabel4.setTextOnly(true);
		 * deviceLabel5.setTextOnly(true); deviceLabel6.setTextOnly(true);
		 * deviceLabel7.setTextOnly(true); deviceLabel8.setTextOnly(true);
		 * deviceLabel9.setTextOnly(true); deviceLabel10.setTextOnly(true);
		 * deviceLabel11.setTextOnly(true); deviceLabel12.setTextOnly(true);
		 * deviceLabel13.setTextOnly(true); deviceLabel14.setTextOnly(true);
		 * deviceLabel15.setTextOnly(true); deviceLabel16.setTextOnly(true);
		 * deviceLabel17.setTextOnly(true); deviceLabel18.setTextOnly(true);
		 * deviceLabel19.setTextOnly(true); deviceLabel20.setTextOnly(true);
		 * deviceLabel21.setTextOnly(true); deviceLabel22.setTextOnly(true);
		 * deviceLabel23.setTextOnly(true); deviceLabel24.setTextOnly(true);
		 * deviceLabel25.setTextOnly(true); deviceLabel26.setTextOnly(true);
		 * deviceLabel27.setTextOnly(true); deviceLabel28.setTextOnly(true);
		 * deviceLabel29.setTextOnly(true); deviceLabel30.setTextOnly(true);
		 * deviceLabel31.setTextOnly(true); deviceLabel32.setTextOnly(true);
		 * deviceLabel33.setTextOnly(true); deviceLabel34.setTextOnly(true);
		 * deviceLabel35.setTextOnly(true); deviceLabel36.setTextOnly(true);
		 */
		jPanel20.setLayout(borderLayout2);
		jLabel7.setText("CHANNEL");
		jLabel4.setText("           NAME");
		jLabel2.setText("                        CALIB.");
		jLabel6.setMaximumSize(new Dimension(29, 15));
		jLabel6.setMinimumSize(new Dimension(29, 15));
		jLabel6.setText("                  GAIN");
		jLabel1.setText("                NAME");
		jLabel3.setText("                          CALIB.");
		jLabel5.setText("                   GAIN");
		jPanel21.setLayout(flowLayout21);
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceField1, null);
		jPanel2.add(deviceDispatch1, null);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceField2, null);
		jPanel1.add(jPanel20, null);
		jPanel21.add(jLabel4, null);
		jPanel21.add(jLabel2, null);
		jPanel21.add(jLabel6, null);
		jPanel21.add(jLabel1, null);
		jPanel21.add(jLabel3, null);
		jPanel21.add(jLabel5, null);
		jPanel20.add(jLabel7, BorderLayout.WEST);
		jPanel20.add(jPanel21, BorderLayout.CENTER);
		this.getContentPane().add(jScrollPane1, BorderLayout.CENTER);
		jScrollPane1.getViewport().add(jPanel4, null);
		jPanel4.add(deviceChannel1, null);
		deviceChannel1.add(jPanel5, null);
		jPanel5.add(deviceLabel1, null);
		jPanel5.add(deviceField5, null);
		jPanel5.add(deviceChoice2, null);
		jPanel5.add(deviceLabel19, null);
		jPanel5.add(deviceField3, null);
		jPanel5.add(deviceChoice1, null);
		jPanel4.add(deviceChannel18, null);
		deviceChannel18.add(jPanel112, null);
		jPanel112.add(deviceLabel2, null);
		jPanel112.add(deviceField11111111, null);
		jPanel112.add(deviceChoice1115, null);
		jPanel112.add(deviceLabel20, null);
		jPanel112.add(deviceField11111112, null);
		jPanel112.add(deviceChoice1116, null);
		jPanel4.add(deviceChannel17, null);
		deviceChannel17.add(jPanel111, null);
		jPanel111.add(deviceLabel3, null);
		jPanel111.add(deviceField1111117, null);
		jPanel111.add(deviceChoice1113, null);
		jPanel111.add(deviceLabel21, null);
		jPanel111.add(deviceField1111118, null);
		jPanel111.add(deviceChoice1114, null);
		jPanel4.add(deviceChannel16, null);
		deviceChannel16.add(jPanel110, null);
		jPanel110.add(deviceLabel4, null);
		jPanel110.add(deviceField1111113, null);
		jPanel110.add(deviceChoice1111, null);
		jPanel110.add(deviceLabel22, null);
		jPanel110.add(deviceField1111114, null);
		jPanel110.add(deviceChoice1112, null);
		jPanel4.add(deviceChannel15, null);
		deviceChannel15.add(jPanel19, null);
		jPanel19.add(deviceLabel5, null);
		jPanel19.add(deviceField111119, null);
		jPanel19.add(deviceChoice119, null);
		jPanel19.add(deviceLabel23, null);
		jPanel19.add(deviceField1111110, null);
		jPanel19.add(deviceChoice1110, null);
		jPanel4.add(deviceChannel14, null);
		deviceChannel14.add(jPanel18, null);
		jPanel18.add(deviceLabel6, null);
		jPanel18.add(deviceField111115, null);
		jPanel18.add(deviceChoice117, null);
		jPanel18.add(deviceLabel24, null);
		jPanel18.add(deviceField111116, null);
		jPanel18.add(deviceChoice118, null);
		jPanel4.add(deviceChannel13, null);
		deviceChannel13.add(jPanel17, null);
		jPanel17.add(deviceLabel7, null);
		jPanel17.add(deviceField111111, null);
		jPanel17.add(deviceChoice115, null);
		jPanel17.add(deviceLabel25, null);
		jPanel17.add(deviceField111112, null);
		jPanel17.add(deviceChoice116, null);
		jPanel4.add(deviceChannel12, null);
		deviceChannel12.add(jPanel16, null);
		jPanel16.add(deviceLabel8, null);
		jPanel16.add(deviceField11117, null);
		jPanel16.add(deviceChoice113, null);
		jPanel16.add(deviceLabel26, null);
		jPanel16.add(deviceField11118, null);
		jPanel16.add(deviceChoice114, null);
		jPanel4.add(deviceChannel11, null);
		deviceChannel11.add(jPanel15, null);
		jPanel15.add(deviceLabel9, null);
		jPanel15.add(deviceField11113, null);
		jPanel15.add(deviceChoice111, null);
		jPanel15.add(deviceLabel27, null);
		jPanel15.add(deviceField11114, null);
		jPanel15.add(deviceChoice112, null);
		jPanel4.add(deviceChannel10, null);
		deviceChannel10.add(jPanel14, null);
		jPanel14.add(deviceLabel10, null);
		jPanel14.add(deviceField1119, null);
		jPanel14.add(deviceChoice19, null);
		jPanel14.add(deviceLabel28, null);
		jPanel14.add(deviceField11110, null);
		jPanel14.add(deviceChoice110, null);
		jPanel4.add(deviceChannel9, null);
		deviceChannel9.add(jPanel13, null);
		jPanel13.add(deviceLabel11, null);
		jPanel13.add(deviceField1115, null);
		jPanel13.add(deviceChoice17, null);
		jPanel13.add(deviceLabel29, null);
		jPanel13.add(deviceField1116, null);
		jPanel13.add(deviceChoice18, null);
		jPanel4.add(deviceChannel8, null);
		deviceChannel8.add(jPanel12, null);
		jPanel12.add(deviceLabel12, null);
		jPanel12.add(deviceField1111, null);
		jPanel12.add(deviceChoice15, null);
		jPanel12.add(deviceLabel30, null);
		jPanel12.add(deviceField1112, null);
		jPanel12.add(deviceChoice16, null);
		jPanel4.add(deviceChannel7, null);
		deviceChannel7.add(jPanel11, null);
		jPanel11.add(deviceLabel13, null);
		jPanel11.add(deviceField117, null);
		jPanel11.add(deviceChoice13, null);
		jPanel11.add(deviceLabel31, null);
		jPanel11.add(deviceField118, null);
		jPanel11.add(deviceChoice14, null);
		jPanel4.add(deviceChannel6, null);
		deviceChannel6.add(jPanel10, null);
		jPanel10.add(deviceLabel14, null);
		jPanel10.add(deviceField113, null);
		jPanel10.add(deviceChoice11, null);
		jPanel10.add(deviceLabel32, null);
		jPanel10.add(deviceField114, null);
		jPanel10.add(deviceChoice12, null);
		jPanel4.add(deviceChannel5, null);
		deviceChannel5.add(jPanel9, null);
		jPanel9.add(deviceLabel15, null);
		jPanel9.add(deviceField19, null);
		jPanel9.add(deviceChoice9, null);
		jPanel9.add(deviceLabel33, null);
		jPanel9.add(deviceField110, null);
		jPanel9.add(deviceChoice10, null);
		jPanel4.add(deviceChannel4, null);
		deviceChannel4.add(jPanel8, null);
		jPanel8.add(deviceLabel16, null);
		jPanel8.add(deviceField15, null);
		jPanel8.add(deviceChoice7, null);
		jPanel8.add(deviceLabel34, null);
		jPanel8.add(deviceField16, null);
		jPanel8.add(deviceChoice8, null);
		jPanel4.add(deviceChannel3, null);
		deviceChannel3.add(jPanel7, null);
		jPanel7.add(deviceLabel17, null);
		jPanel7.add(deviceField11, null);
		jPanel7.add(deviceChoice5, null);
		jPanel7.add(deviceLabel35, null);
		jPanel7.add(deviceField12, null);
		jPanel7.add(deviceChoice6, null);
		jPanel4.add(deviceChannel2, null);
		deviceChannel2.add(jPanel6, null);
		jPanel6.add(deviceLabel18, null);
		jPanel6.add(deviceField7, null);
		jPanel6.add(deviceChoice3, null);
		jPanel6.add(deviceLabel36, null);
		jPanel6.add(deviceField8, null);
		jPanel6.add(deviceChoice4, null);
	}
}

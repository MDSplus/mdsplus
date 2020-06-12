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
public class RFXMHDSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel9 = new JPanel();
	BorderLayout borderLayout1 = new BorderLayout();
	JPanel jPanel33 = new JPanel();
	BorderLayout borderLayout2 = new BorderLayout();
	JTabbedPane jTabbedPane1 = new JTabbedPane();
	JScrollPane jScrollPane4 = new JScrollPane();
	JPanel jPanel6 = new JPanel();
	FlowLayout flowLayout5 = new FlowLayout();
	FlowLayout flowLayout8 = new FlowLayout();
	JPanel jPanel7 = new JPanel();
	JPanel jPanel5 = new JPanel();
	GridLayout gridLayout5 = new GridLayout();
	JPanel jPanel40 = new JPanel();
	GridLayout gridLayout8 = new GridLayout();
	JPanel jPanel10 = new JPanel();
	DeviceField deviceField1 = new DeviceField();
	JPanel jPanel30 = new JPanel();
	JPanel jPanel29 = new JPanel();
	JPanel jPanel28 = new JPanel();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	JPanel jPanel27 = new JPanel();
	JPanel jPanel26 = new JPanel();
	JPanel jPanel25 = new JPanel();
	BorderLayout borderLayout3 = new BorderLayout();
	JPanel jPanel31 = new JPanel();
	GridLayout gridLayout7 = new GridLayout();
	JScrollPane jScrollPane5 = new JScrollPane();
	DeviceTable deviceTable7 = new DeviceTable();
	DeviceField deviceField34 = new DeviceField();
	BorderLayout borderLayout5 = new BorderLayout();
	GridLayout gridLayout10 = new GridLayout();
	JPanel jPanel47 = new JPanel();
	JPanel jPanel42 = new JPanel();
	JPanel jPanel44 = new JPanel();
	DeviceField deviceField35 = new DeviceField();
	JScrollPane jScrollPane14 = new JScrollPane();
	JPanel jPanel415 = new JPanel();
	DeviceTable deviceTable13 = new DeviceTable();
	DeviceField deviceField37 = new DeviceField();
	JPanel jPanel46 = new JPanel();
	DeviceTable deviceTable8 = new DeviceTable();
	DeviceField deviceField36 = new DeviceField();
	BorderLayout borderLayout6 = new BorderLayout();
	GridLayout gridLayout11 = new GridLayout();
	JPanel jPanel45 = new JPanel();
	JScrollPane jScrollPane7 = new JScrollPane();
	JPanel jPanel417 = new JPanel();
	DeviceTable deviceTable15 = new DeviceTable();
	DeviceField deviceField38 = new DeviceField();
	JPanel jPanel418 = new JPanel();
	JPanel jPanel416 = new JPanel();
	GridLayout gridLayout12 = new GridLayout();
	BorderLayout borderLayout7 = new BorderLayout();
	DeviceField deviceField39 = new DeviceField();
	DeviceTable deviceTable9 = new DeviceTable();
	Border border1;
	TitledBorder titledBorder1;
	TitledBorder titledBorder2;
	Border border2;
	TitledBorder titledBorder3;
	Border border3;
	TitledBorder titledBorder4;
	Border border4;
	TitledBorder titledBorder5;
	Border border5;
	TitledBorder titledBorder6;
	Border border6;
	TitledBorder titledBorder7;
	JScrollPane jScrollPane15 = new JScrollPane();
	JPanel jPanel54 = new JPanel();
	GridLayout gridLayout9 = new GridLayout();
	DeviceTable deviceTable23 = new DeviceTable();
	DeviceTable deviceTable24 = new DeviceTable();
	JScrollPane jScrollPane9 = new JScrollPane();
	JPanel jPanel55 = new JPanel();
	DeviceTable deviceTable22 = new DeviceTable();
	BorderLayout borderLayout9 = new BorderLayout();
	JPanel jPanel56 = new JPanel();
	DeviceField deviceField43 = new DeviceField();
	DeviceField deviceField44 = new DeviceField();
	JScrollPane jScrollPane10 = new JScrollPane();
	JScrollPane jScrollPane11 = new JScrollPane();
	BorderLayout borderLayout10 = new BorderLayout();
	DeviceField deviceField45 = new DeviceField();
	JPanel jPanel58 = new JPanel();
	JPanel jPanel57 = new JPanel();
	DeviceTable deviceTable25 = new DeviceTable();
	DeviceField deviceField46 = new DeviceField();
	DeviceTable deviceTable26 = new DeviceTable();
	JPanel jPanel59 = new JPanel();
	DeviceField deviceField48 = new DeviceField();
	JPanel jPanel510 = new JPanel();
	BorderLayout borderLayout11 = new BorderLayout();
	DeviceField deviceField47 = new DeviceField();
	DeviceTable deviceTable27 = new DeviceTable();
	DeviceTable deviceTable28 = new DeviceTable();
	DeviceTable deviceTable210 = new DeviceTable();
	DeviceTable deviceTable18 = new DeviceTable();
	DeviceTable deviceTable19 = new DeviceTable();
	JPanel jPanel43 = new JPanel();
	FlowLayout flowLayout9 = new FlowLayout();
	JPanel jPanel419 = new JPanel();
	FlowLayout flowLayout10 = new FlowLayout();
	JPanel jPanel4110 = new JPanel();
	FlowLayout flowLayout11 = new FlowLayout();
	JPanel jPanel4111 = new JPanel();
	FlowLayout flowLayout12 = new FlowLayout();
	FlowLayout flowLayout7 = new FlowLayout();
	JPanel jPanel23 = new JPanel();
	JPanel jPanel24 = new JPanel();
	FlowLayout flowLayout6 = new FlowLayout();
	DeviceField deviceField63 = new DeviceField();
	DeviceChoice deviceChoice8 = new DeviceChoice();
	DeviceField deviceField16 = new DeviceField();
	DeviceField deviceField25 = new DeviceField();
	DeviceField deviceField24 = new DeviceField();
	DeviceField deviceField23 = new DeviceField();
	DeviceField deviceField22 = new DeviceField();
	DeviceField deviceField21 = new DeviceField();
	DeviceField deviceField30 = new DeviceField();
	DeviceField deviceField20 = new DeviceField();
	DeviceField deviceField62 = new DeviceField();
	DeviceChoice deviceChoice9 = new DeviceChoice();
	DeviceField deviceField40 = new DeviceField();
	DeviceField deviceField218 = new DeviceField();
	DeviceField deviceField219 = new DeviceField();
	DeviceField deviceField2112 = new DeviceField();
	DeviceField deviceField2110 = new DeviceField();
	DeviceField deviceField2113 = new DeviceField();
	DeviceField deviceField31 = new DeviceField();
	DeviceField deviceField2111 = new DeviceField();
	DeviceField deviceField64 = new DeviceField();
	DeviceChoice deviceChoice10 = new DeviceChoice();
	DeviceField deviceField41 = new DeviceField();
	DeviceField deviceField212 = new DeviceField();
	DeviceField deviceField213 = new DeviceField();
	DeviceField deviceField216 = new DeviceField();
	DeviceField deviceField214 = new DeviceField();
	DeviceField deviceField217 = new DeviceField();
	DeviceField deviceField32 = new DeviceField();
	DeviceField deviceField215 = new DeviceField();
	DeviceField deviceField65 = new DeviceField();
	DeviceChoice deviceChoice11 = new DeviceChoice();
	DeviceField deviceField414 = new DeviceField();
	DeviceField deviceField2214 = new DeviceField();
	DeviceField deviceField2215 = new DeviceField();
	DeviceField deviceField21111 = new DeviceField();
	DeviceField deviceField2212 = new DeviceField();
	DeviceField deviceField21110 = new DeviceField();
	DeviceField deviceField313 = new DeviceField();
	DeviceField deviceField2213 = new DeviceField();
	DeviceField deviceField66 = new DeviceField();
	DeviceChoice deviceChoice12 = new DeviceChoice();
	DeviceField deviceField413 = new DeviceField();
	DeviceField deviceField2210 = new DeviceField();
	DeviceField deviceField2211 = new DeviceField();
	DeviceField deviceField67 = new DeviceField();
	DeviceChoice deviceChoice13 = new DeviceChoice();
	DeviceField deviceField412 = new DeviceField();
	DeviceField deviceField226 = new DeviceField();
	DeviceField deviceField227 = new DeviceField();
	DeviceField deviceField2117 = new DeviceField();
	DeviceField deviceField224 = new DeviceField();
	DeviceField deviceField2116 = new DeviceField();
	DeviceField deviceField311 = new DeviceField();
	DeviceField deviceField225 = new DeviceField();
	DeviceField deviceField68 = new DeviceField();
	DeviceChoice deviceChoice14 = new DeviceChoice();
	DeviceField deviceField411 = new DeviceField();
	DeviceField deviceField222 = new DeviceField();
	DeviceField deviceField223 = new DeviceField();
	DeviceField deviceField2115 = new DeviceField();
	DeviceField deviceField220 = new DeviceField();
	DeviceField deviceField2114 = new DeviceField();
	DeviceField deviceField310 = new DeviceField();
	DeviceField deviceField221 = new DeviceField();
	DeviceField deviceField69 = new DeviceField();
	DeviceChoice deviceChoice15 = new DeviceChoice();
	DeviceField deviceField42 = new DeviceField();
	DeviceField deviceField26 = new DeviceField();
	DeviceField deviceField27 = new DeviceField();
	DeviceField deviceField210 = new DeviceField();
	DeviceField deviceField28 = new DeviceField();
	DeviceField deviceField211 = new DeviceField();
	DeviceField deviceField33 = new DeviceField();
	DeviceField deviceField29 = new DeviceField();
	DeviceField deviceField2216 = new DeviceField();
	DeviceField deviceField2119 = new DeviceField();
	DeviceField deviceField228 = new DeviceField();
	DeviceField deviceField2118 = new DeviceField();
	DeviceField deviceField312 = new DeviceField();
	DeviceField deviceField229 = new DeviceField();
	DeviceTable deviceTable11 = new DeviceTable();
	DeviceTable deviceTable16 = new DeviceTable();
	DeviceTable deviceTable110 = new DeviceTable();
	DeviceTable deviceTable111 = new DeviceTable();
	DeviceTable deviceTable112 = new DeviceTable();
	DeviceField deviceField73 = new DeviceField();
	JPanel jPanel70 = new JPanel();
	DeviceTable deviceTable20 = new DeviceTable();
	DeviceField deviceField70 = new DeviceField();
	DeviceField deviceField71 = new DeviceField();
	DeviceChoice deviceChoice5 = new DeviceChoice();
	DeviceTable deviceTable21 = new DeviceTable();
	JPanel jPanel71 = new JPanel();
	DeviceField deviceField72 = new DeviceField();

	public RFXMHDSetup()
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
		border1 = new EtchedBorder(EtchedBorder.RAISED, Color.white, new Color(148, 145, 140));
		titledBorder1 = new TitledBorder(BorderFactory.createLineBorder(Color.black, 1), "Output 4");
		titledBorder2 = new TitledBorder("");
		border2 = BorderFactory.createLineBorder(Color.black, 2);
		titledBorder3 = new TitledBorder(border2, "Output 3");
		border3 = BorderFactory.createLineBorder(Color.black, 1);
		titledBorder4 = new TitledBorder(border3, "Output 1");
		border4 = BorderFactory.createLineBorder(Color.black, 1);
		titledBorder5 = new TitledBorder(border4, "Output 2");
		border5 = BorderFactory.createLineBorder(Color.black, 2);
		titledBorder6 = new TitledBorder(border5, "Output 3");
		border6 = BorderFactory.createLineBorder(Color.black, 1);
		titledBorder7 = new TitledBorder(border6, "Output 4");
		this.setWidth(900);
		this.setHeight(700);
		this.setDeviceType("RFXControl");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("RFX MHD Control Setup");
		this.getContentPane().setLayout(borderLayout1);
		jPanel9.setLayout(borderLayout3);
		jPanel9.setDebugGraphicsOptions(0);
		jPanel33.setLayout(borderLayout2);
		jPanel6.setLayout(flowLayout5);
		jPanel7.setLayout(flowLayout8);
		jPanel5.setLayout(gridLayout5);
		gridLayout5.setRows(8);
		gridLayout5.setVgap(-5);
		jPanel40.setLayout(gridLayout8);
		gridLayout8.setColumns(1);
		gridLayout8.setHgap(0);
		gridLayout8.setRows(3);
		gridLayout8.setVgap(-2);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Comment: ");
		deviceField1.setNumCols(20);
		deviceField1.setIdentifier("");
		deviceChoice2.setChoiceIntValues(new int[]
		{ 3, 4, 9, 20, 10, 11, 12, 23 });
		deviceChoice2.setChoiceFloatValues(null);
		deviceChoice2.setConvert(true);
		deviceChoice2.setOffsetNid(7);
		deviceChoice2.setLabelString("Trig1 control: ");
		deviceChoice2.setChoiceItems(new String[]
		{ "ROT.PERTURBATION", "MODE CONTROL", "MODE CONTROL+ROT.PERT.(FEEDBACK)", "MODE CONTROL+ROT.PERT.(FEEDFORW)",
				"VIRTUAL SHELL", "VS+ROT.PERT.(FEEDBACK)", "VS+ROT.PERT.(FEEDFORW)", "CMC + Modal Decoupling" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		jPanel31.setLayout(gridLayout7);
		gridLayout7.setColumns(2);
		deviceTable7.setOffsetNid(1229);
		deviceTable7.setLabelString("Gain Module:");
		deviceTable7.setRefMode(DeviceTable.REFLEX);
		deviceTable7.setNumCols(48);
		deviceTable7.setNumRows(4);
		deviceTable7.setIdentifier("");
		deviceTable7.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1" });
		deviceTable7.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceField34.setOffsetNid(1227);
		deviceField34.setLabelString("End Time(s): ");
		deviceField34.setNumCols(5);
		deviceField34.setIdentifier("");
		gridLayout10.setColumns(1);
		gridLayout10.setRows(4);
		jPanel47.setLayout(gridLayout10);
		jPanel42.setLayout(borderLayout5);
		deviceField35.setOffsetNid(1225);
		deviceField35.setLabelString("Start Time(s): ");
		deviceField35.setNumCols(5);
		deviceField35.setIdentifier("");
		jPanel415.setLayout(gridLayout11);
		deviceTable13.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable13.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2",
				"16<NL>5<NL>4<NL>3<NL>2<NL>1<NL>" });
		deviceTable13.setIdentifier("");
		deviceTable13.setNumRows(4);
		deviceTable13.setNumCols(48);
		deviceTable13.setLabelString("Derivative Gain:");
		deviceTable13.setRefMode(DeviceTable.REFLEX);
		deviceTable13.setOffsetNid(1247);
		deviceField37.setOffsetNid(1237);
		deviceField37.setLabelString("Start Time(s): ");
		deviceField37.setNumCols(5);
		deviceField37.setIdentifier("");
		deviceTable8.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable8.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1" });
		deviceTable8.setIdentifier("");
		deviceTable8.setNumRows(4);
		deviceTable8.setNumCols(48);
		deviceTable8.setLabelString("Gain Module:");
		deviceTable8.setRefMode(DeviceTable.REFLEX);
		deviceTable8.setOffsetNid(1241);
		deviceField36.setIdentifier("");
		deviceField36.setLabelString("End Time(s): ");
		deviceField36.setNumCols(5);
		deviceField36.setOffsetNid(1239);
		gridLayout11.setRows(4);
		gridLayout11.setColumns(1);
		jPanel45.setLayout(borderLayout6);
		jPanel417.setLayout(borderLayout7);
		deviceTable15.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable15.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1" });
		deviceTable15.setIdentifier("");
		deviceTable15.setNumRows(4);
		deviceTable15.setNumCols(48);
		deviceTable15.setLabelString("Derivative Gain:");
		deviceTable15.setRefMode(DeviceTable.REFLEX);
		deviceTable15.setOffsetNid(1259);
		deviceField38.setIdentifier("");
		deviceField38.setLabelString("End Time(s): ");
		deviceField38.setNumCols(5);
		deviceField38.setOffsetNid(1251);
		jPanel416.setLayout(gridLayout12);
		gridLayout12.setRows(4);
		gridLayout12.setColumns(1);
		deviceField39.setOffsetNid(1249);
		deviceField39.setLabelString("Start Time(s): ");
		deviceField39.setNumCols(5);
		deviceField39.setIdentifier("");
		deviceTable9.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable9.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1" });
		deviceTable9.setIdentifier("");
		deviceTable9.setNumRows(4);
		deviceTable9.setNumCols(48);
		deviceTable9.setLabelString("Gain Module:");
		deviceTable9.setRefMode(DeviceTable.REFLEX);
		deviceTable9.setOffsetNid(1253);
		jPanel54.setLayout(gridLayout9);
		gridLayout9.setColumns(1);
		gridLayout9.setRows(4);
		deviceTable23.setDisplayRowNumber(true);
		deviceTable23.setRowNames(null);
		deviceTable23.setColumnNames(new String[]
		{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
				"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37",
				"38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48" });
		deviceTable23.setIdentifier("");
		deviceTable23.setNumRows(4);
		deviceTable23.setNumCols(48);
		deviceTable23.setLabelString("Kd");
		deviceTable23.setOffsetNid(1085);
		deviceTable24.setDisplayRowNumber(true);
		deviceTable24.setRowNames(null);
		deviceTable24.setColumnNames(new String[]
		{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
				"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37",
				"38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48" });
		deviceTable24.setIdentifier("");
		deviceTable24.setNumRows(4);
		deviceTable24.setNumCols(48);
		deviceTable24.setLabelString("Ki");
		deviceTable24.setOffsetNid(1083);
		jPanel55.setLayout(borderLayout9);
		deviceTable22.setDisplayRowNumber(true);
		deviceTable22.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable22.setBinary(false);
		deviceTable22.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1" });
		deviceTable22.setIdentifier("");
		deviceTable22.setNumRows(4);
		deviceTable22.setNumCols(48);
		deviceTable22.setLabelString("Mode Gains (Module)");
		deviceTable22.setRefMode(DeviceTable.REFLEX);
		deviceTable22.setOffsetNid(1269);
		deviceField44.setOffsetNid(1265);
		deviceField44.setLabelString("Start time: ");
		deviceField44.setNumCols(5);
		deviceField44.setIdentifier("");
		deviceField43.setOffsetNid(1267);
		deviceField43.setLabelString("End time: ");
		deviceField43.setNumCols(5);
		deviceField43.setIdentifier("");
		deviceField45.setIdentifier("");
		deviceField45.setNumCols(5);
		deviceField45.setLabelString("End time: ");
		deviceField45.setOffsetNid(1277);
		jPanel57.setLayout(borderLayout10);
		deviceTable25.setDisplayRowNumber(true);
		deviceTable25.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable25.setBinary(false);
		deviceTable25.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1" });
		deviceTable25.setIdentifier("");
		deviceTable25.setNumRows(4);
		deviceTable25.setNumCols(48);
		deviceTable25.setLabelString("Mode Gains (Module)");
		deviceTable25.setRefMode(DeviceTable.REFLEX);
		deviceTable25.setOffsetNid(1279);
		deviceField46.setOffsetNid(1275);
		deviceField46.setLabelString("Start time: ");
		deviceField46.setNumCols(5);
		deviceField46.setIdentifier("");
		deviceTable26.setDisplayRowNumber(true);
		deviceTable26.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable26.setBinary(false);
		deviceTable26.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1" });
		deviceTable26.setIdentifier("");
		deviceTable26.setNumRows(4);
		deviceTable26.setNumCols(48);
		deviceTable26.setLabelString("Mode Gains (Module)");
		deviceTable26.setRefMode(DeviceTable.REFLEX);
		deviceTable26.setOffsetNid(1289);
		jPanel59.setLayout(borderLayout11);
		deviceField48.setOffsetNid(1285);
		deviceField48.setLabelString("Start time: ");
		deviceField48.setNumCols(5);
		deviceField48.setIdentifier("");
		deviceField47.setIdentifier("");
		deviceField47.setNumCols(5);
		deviceField47.setLabelString("End time: ");
		deviceField47.setOffsetNid(1287);
		deviceTable27.setOffsetNid(1271);
		deviceTable27.setLabelString("Mode Gains (Phase, Rad)");
		deviceTable27.setRefMode(DeviceTable.REFLEX_INVERT);
		deviceTable27.setNumCols(48);
		deviceTable27.setNumRows(4);
		deviceTable27.setIdentifier("");
		deviceTable27.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1" });
		deviceTable27.setBinary(false);
		deviceTable27.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable27.setDisplayRowNumber(true);
		deviceTable28.setOffsetNid(1281);
		deviceTable28.setLabelString("Mode Gains (Phase, Rad)");
		deviceTable28.setRefMode(DeviceTable.REFLEX_INVERT);
		deviceTable28.setNumCols(48);
		deviceTable28.setNumRows(4);
		deviceTable28.setIdentifier("");
		deviceTable28.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1" });
		deviceTable28.setBinary(false);
		deviceTable28.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable28.setDisplayRowNumber(true);
		deviceTable210.setOffsetNid(1291);
		deviceTable210.setLabelString("Mode Gains (Phase, Rad)");
		deviceTable210.setRefMode(DeviceTable.REFLEX_INVERT);
		deviceTable210.setNumCols(48);
		deviceTable210.setNumRows(4);
		deviceTable210.setIdentifier("");
		deviceTable210.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1" });
		deviceTable210.setBinary(false);
		deviceTable210.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable210.setDisplayRowNumber(true);
		deviceTable18.setOffsetNid(1245);
		deviceTable18.setLabelString("Inetgral Gain:");
		deviceTable18.setRefMode(DeviceTable.REFLEX);
		deviceTable18.setNumCols(48);
		deviceTable18.setNumRows(4);
		deviceTable18.setIdentifier("");
		deviceTable18.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2",
				"16<NL>5<NL>4<NL>3<NL>2<NL>1<NL>" });
		deviceTable18.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable19.setOffsetNid(1257);
		deviceTable19.setLabelString("Integral Gain:");
		deviceTable19.setRefMode(DeviceTable.REFLEX);
		deviceTable19.setNumCols(48);
		deviceTable19.setNumRows(4);
		deviceTable19.setIdentifier("");
		deviceTable19.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1" });
		deviceTable19.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		jPanel43.setLayout(flowLayout9);
		jPanel419.setLayout(flowLayout10);
		jPanel4110.setLayout(flowLayout11);
		jPanel4111.setLayout(flowLayout12);
		jPanel23.setLayout(flowLayout7);
		jPanel24.setLayout(flowLayout6);
		deviceField16.setOffsetNid(1295);
		deviceField16.setLabelString("Rise time(s):");
		deviceField16.setNumCols(4);
		deviceField16.setIdentifier("");
		deviceField25.setOffsetNid(1037);
		deviceField25.setLabelString("Rot. Phase(Rad):");
		deviceField25.setNumCols(4);
		deviceField25.setIdentifier("");
		deviceField24.setOffsetNid(1035);
		deviceField24.setLabelString("Rot. Freq. (Hz):");
		deviceField24.setNumCols(4);
		deviceField24.setIdentifier("");
		deviceField23.setOffsetNid(1033);
		deviceField23.setLabelString("End:");
		deviceField23.setNumCols(4);
		deviceField23.setIdentifier("");
		deviceField22.setOffsetNid(1031);
		deviceField22.setLabelString("Start:");
		deviceField22.setNumCols(4);
		deviceField22.setIdentifier("");
		deviceField21.setOffsetNid(1029);
		deviceField21.setLabelString("Amp.:");
		deviceField21.setNumCols(4);
		deviceField21.setIdentifier("");
		deviceField30.setOffsetNid(1027);
		deviceField30.setLabelString("M1:");
		deviceField30.setNumCols(2);
		deviceField30.setIdentifier("");
		deviceField20.setOffsetNid(1025);
		deviceField20.setLabelString("N1: ");
		deviceField20.setNumCols(2);
		deviceField20.setIdentifier("");
		deviceField40.setOffsetNid(1297);
		deviceField40.setLabelString("Rise time(s): ");
		deviceField40.setNumCols(4);
		deviceField40.setIdentifier("");
		deviceField218.setIdentifier("");
		deviceField218.setNumCols(4);
		deviceField218.setLabelString("Rot. Phase(Rad):");
		deviceField218.setOffsetNid(1051);
		deviceField219.setIdentifier("");
		deviceField219.setNumCols(4);
		deviceField219.setLabelString("Rot. Freq. (Hz):");
		deviceField219.setOffsetNid(1049);
		deviceField2112.setOffsetNid(1047);
		deviceField2112.setLabelString("End:");
		deviceField2112.setNumCols(4);
		deviceField2112.setIdentifier("");
		deviceField2110.setOffsetNid(1045);
		deviceField2110.setLabelString("Start:");
		deviceField2110.setNumCols(4);
		deviceField2110.setIdentifier("");
		deviceField2113.setOffsetNid(1043);
		deviceField2113.setLabelString("Amp.:");
		deviceField2113.setNumCols(4);
		deviceField2113.setIdentifier("");
		deviceField31.setOffsetNid(1041);
		deviceField31.setLabelString("M2:");
		deviceField31.setNumCols(2);
		deviceField2111.setOffsetNid(1039);
		deviceField2111.setLabelString("N2: ");
		deviceField2111.setNumCols(2);
		deviceField2111.setIdentifier("");
		deviceField41.setOffsetNid(1299);
		deviceField41.setLabelString("Rise time(s):");
		deviceField41.setNumCols(4);
		deviceField41.setIdentifier("");
		deviceField212.setIdentifier("");
		deviceField212.setNumCols(4);
		deviceField212.setLabelString("Rot. Phase(Rad):");
		deviceField212.setOffsetNid(1065);
		deviceField213.setIdentifier("");
		deviceField213.setNumCols(4);
		deviceField213.setLabelString("Rot. Freq. (Hz):");
		deviceField213.setOffsetNid(1063);
		deviceField216.setOffsetNid(1061);
		deviceField216.setLabelString("End:");
		deviceField216.setNumCols(4);
		deviceField216.setIdentifier("");
		deviceField214.setOffsetNid(1059);
		deviceField214.setLabelString("Start:");
		deviceField214.setNumCols(4);
		deviceField214.setIdentifier("");
		deviceField217.setOffsetNid(1057);
		deviceField217.setLabelString("Amp.:");
		deviceField217.setNumCols(4);
		deviceField217.setIdentifier("");
		deviceField32.setOffsetNid(1055);
		deviceField32.setLabelString("M3:");
		deviceField32.setNumCols(2);
		deviceField215.setOffsetNid(1053);
		deviceField215.setLabelString("N3: ");
		deviceField215.setNumCols(2);
		deviceField215.setIdentifier("");
		deviceField414.setIdentifier("");
		deviceField414.setNumCols(4);
		deviceField414.setLabelString("Rise time(s):");
		deviceField414.setOffsetNid(1301);
		deviceField2214.setOffsetNid(1079);
		deviceField2214.setLabelString("Rot. Phase(Rad):");
		deviceField2214.setNumCols(4);
		deviceField2214.setIdentifier("");
		deviceField2215.setOffsetNid(1077);
		deviceField2215.setLabelString("Rot. Freq. (Hz):");
		deviceField2215.setNumCols(4);
		deviceField2215.setIdentifier("");
		deviceField21111.setIdentifier("");
		deviceField21111.setNumCols(4);
		deviceField21111.setLabelString("End:");
		deviceField21111.setOffsetNid(1075);
		deviceField2212.setIdentifier("");
		deviceField2212.setNumCols(4);
		deviceField2212.setLabelString("Start:");
		deviceField2212.setOffsetNid(1073);
		deviceField21110.setIdentifier("");
		deviceField21110.setNumCols(4);
		deviceField21110.setLabelString("Amp.:");
		deviceField21110.setOffsetNid(1071);
		deviceField313.setNumCols(2);
		deviceField313.setLabelString("M4:");
		deviceField313.setOffsetNid(1069);
		deviceField2213.setIdentifier("");
		deviceField2213.setNumCols(2);
		deviceField2213.setLabelString("N4: ");
		deviceField2213.setOffsetNid(1067);
		deviceField413.setIdentifier("");
		deviceField413.setNumCols(4);
		deviceField413.setLabelString("Rise time(s):");
		deviceField413.setOffsetNid(1363);
		deviceField2210.setOffsetNid(1361);
		deviceField2210.setLabelString("Rot. Phase(Rad):");
		deviceField2210.setNumCols(4);
		deviceField2210.setIdentifier("");
		deviceField2211.setOffsetNid(1077);
		deviceField2211.setLabelString("Rot. Freq. (Hz):");
		deviceField2211.setNumCols(4);
		deviceField2211.setIdentifier("");
		deviceField412.setIdentifier("");
		deviceField412.setNumCols(4);
		deviceField412.setLabelString("Rise time(s):");
		deviceField412.setOffsetNid(1383);
		deviceField226.setOffsetNid(1381);
		deviceField226.setLabelString("Rot. Phase(Rad):");
		deviceField226.setNumCols(4);
		deviceField226.setIdentifier("");
		deviceField227.setOffsetNid(1379);
		deviceField227.setLabelString("Rot. Freq. (Hz):");
		deviceField227.setNumCols(4);
		deviceField227.setIdentifier("");
		deviceField2117.setIdentifier("");
		deviceField2117.setNumCols(4);
		deviceField2117.setLabelString("End:");
		deviceField2117.setOffsetNid(1377);
		deviceField224.setIdentifier("");
		deviceField224.setNumCols(4);
		deviceField224.setLabelString("Start:");
		deviceField224.setOffsetNid(1375);
		deviceField2116.setIdentifier("");
		deviceField2116.setNumCols(4);
		deviceField2116.setLabelString("Amp.:");
		deviceField2116.setOffsetNid(1373);
		deviceField311.setNumCols(2);
		deviceField311.setIdentifier("");
		deviceField311.setLabelString("M6:");
		deviceField311.setOffsetNid(1371);
		deviceField225.setIdentifier("");
		deviceField225.setNumCols(2);
		deviceField225.setLabelString("N6: ");
		deviceField225.setOffsetNid(1369);
		deviceField411.setIdentifier("");
		deviceField411.setNumCols(4);
		deviceField411.setLabelString("Rise time(s):");
		deviceField411.setOffsetNid(1403);
		deviceField222.setOffsetNid(1401);
		deviceField222.setLabelString("Rot. Phase(Rad):");
		deviceField222.setNumCols(4);
		deviceField222.setIdentifier("");
		deviceField223.setOffsetNid(1399);
		deviceField223.setLabelString("Rot. Freq. (Hz):");
		deviceField223.setNumCols(4);
		deviceField223.setIdentifier("");
		deviceField2115.setIdentifier("");
		deviceField2115.setNumCols(4);
		deviceField2115.setLabelString("End:");
		deviceField2115.setOffsetNid(1397);
		deviceField220.setIdentifier("");
		deviceField220.setNumCols(4);
		deviceField220.setLabelString("Start:");
		deviceField220.setOffsetNid(1395);
		deviceField2114.setIdentifier("");
		deviceField2114.setNumCols(4);
		deviceField2114.setLabelString("Amp.:");
		deviceField2114.setOffsetNid(1393);
		deviceField310.setNumCols(2);
		deviceField310.setIdentifier("");
		deviceField310.setLabelString("M7:");
		deviceField310.setOffsetNid(1391);
		deviceField221.setIdentifier("");
		deviceField221.setNumCols(2);
		deviceField221.setLabelString("N7: ");
		deviceField221.setOffsetNid(1389);
		deviceField42.setOffsetNid(1423);
		deviceField42.setLabelString("Rise time(s):");
		deviceField42.setNumCols(4);
		deviceField42.setIdentifier("");
		deviceField26.setIdentifier("");
		deviceField26.setNumCols(4);
		deviceField26.setLabelString("Rot. Phase(Rad):");
		deviceField26.setOffsetNid(1421);
		deviceField27.setIdentifier("");
		deviceField27.setNumCols(4);
		deviceField27.setLabelString("Rot. Freq. (Hz):");
		deviceField27.setOffsetNid(1419);
		deviceField210.setOffsetNid(1417);
		deviceField210.setLabelString("End:");
		deviceField210.setNumCols(4);
		deviceField210.setIdentifier("");
		deviceField28.setOffsetNid(1415);
		deviceField28.setLabelString("Start:");
		deviceField28.setNumCols(4);
		deviceField28.setIdentifier("");
		deviceField211.setOffsetNid(1413);
		deviceField211.setLabelString("Amp.:");
		deviceField211.setNumCols(4);
		deviceField211.setIdentifier("");
		deviceField33.setOffsetNid(1411);
		deviceField33.setLabelString("M8:");
		deviceField33.setNumCols(2);
		deviceField33.setIdentifier("");
		deviceField29.setOffsetNid(1409);
		deviceField29.setLabelString("N8: ");
		deviceField29.setNumCols(2);
		deviceField29.setIdentifier("");
		deviceChoice8.setChoiceIntValues(new int[]
		{ 0, 1 });
		deviceChoice8.setChoiceFloatValues(null);
		deviceChoice8.setConvert(true);
		deviceChoice8.setOffsetNid(1333);
		deviceChoice8.setLabelString("Phase ");
		deviceChoice8.setChoiceItems(new String[]
		{ "Absolute", "Relative Br" });
		deviceChoice8.setUpdateIdentifier("");
		deviceChoice8.setIdentifier("");
		deviceChoice9.setChoiceIntValues(new int[]
		{ 0, 1 });
		deviceChoice9.setChoiceFloatValues(null);
		deviceChoice9.setConvert(true);
		deviceChoice9.setOffsetNid(1335);
		deviceChoice9.setLabelString("Phase");
		deviceChoice9.setChoiceItems(new String[]
		{ "Absolute", "Relative Br" });
		deviceChoice9.setUpdateIdentifier("");
		deviceChoice9.setIdentifier("");
		deviceChoice10.setChoiceIntValues(new int[]
		{ 0, 1 });
		deviceChoice10.setChoiceFloatValues(null);
		deviceChoice10.setConvert(true);
		deviceChoice10.setOffsetNid(1337);
		deviceChoice10.setLabelString("Phase");
		deviceChoice10.setChoiceItems(new String[]
		{ "Absolute", "Relative Br" });
		deviceChoice10.setUpdateIdentifier("");
		deviceChoice10.setIdentifier("");
		deviceChoice11.setChoiceIntValues(new int[]
		{ 0, 1 });
		deviceChoice11.setChoiceFloatValues(null);
		deviceChoice11.setConvert(true);
		deviceChoice11.setOffsetNid(1339);
		deviceChoice11.setLabelString("Phase");
		deviceChoice11.setChoiceItems(new String[]
		{ "Absolute", "Relative Br" });
		deviceChoice11.setUpdateIdentifier("");
		deviceChoice11.setIdentifier("");
		deviceField63.setOffsetNid(1341);
		deviceField63.setLabelString("Threshold:");
		deviceField63.setNumCols(4);
		deviceField63.setIdentifier("");
		deviceField62.setOffsetNid(1343);
		deviceField62.setLabelString("Threshold:");
		deviceField62.setNumCols(4);
		deviceField62.setIdentifier("");
		deviceField64.setOffsetNid(1345);
		deviceField64.setLabelString("Threshold:");
		deviceField64.setNumCols(4);
		deviceField64.setIdentifier("");
		deviceField65.setOffsetNid(1347);
		deviceField65.setLabelString("Threshold:");
		deviceField65.setNumCols(4);
		deviceField65.setIdentifier("");
		deviceField2216.setIdentifier("");
		deviceField2216.setNumCols(4);
		deviceField2216.setLabelString("Rot. Freq. (Hz):");
		deviceField2216.setOffsetNid(1359);
		deviceField2119.setIdentifier("");
		deviceField2119.setNumCols(4);
		deviceField2119.setLabelString("End:");
		deviceField2119.setOffsetNid(1357);
		deviceField228.setIdentifier("");
		deviceField228.setNumCols(4);
		deviceField228.setLabelString("Start:");
		deviceField228.setOffsetNid(1355);
		deviceField2118.setIdentifier("");
		deviceField2118.setNumCols(4);
		deviceField2118.setLabelString("Amp.:");
		deviceField2118.setOffsetNid(1353);
		deviceField312.setNumCols(2);
		deviceField312.setIdentifier("");
		deviceField312.setLabelString("M5:");
		deviceField312.setOffsetNid(1351);
		deviceField229.setIdentifier("");
		deviceField229.setNumCols(2);
		deviceField229.setLabelString("N5: ");
		deviceField229.setOffsetNid(1349);
		deviceChoice12.setChoiceIntValues(new int[]
		{ 0, 1 });
		deviceChoice12.setChoiceFloatValues(null);
		deviceChoice12.setConvert(true);
		deviceChoice12.setOffsetNid(1365);
		deviceChoice12.setLabelString("Phase");
		deviceChoice12.setChoiceItems(new String[]
		{ "Absolute", "Relative Br" });
		deviceChoice12.setUpdateIdentifier("");
		deviceChoice12.setIdentifier("");
		deviceField66.setOffsetNid(1367);
		deviceField66.setLabelString("Threshold:");
		deviceField66.setNumCols(4);
		deviceField66.setIdentifier("");
		deviceChoice13.setChoiceIntValues(new int[]
		{ 0, 1 });
		deviceChoice13.setChoiceFloatValues(null);
		deviceChoice13.setConvert(true);
		deviceChoice13.setOffsetNid(1385);
		deviceChoice13.setLabelString("Phase");
		deviceChoice13.setChoiceItems(new String[]
		{ "Absolute", "Relative Br" });
		deviceChoice13.setUpdateIdentifier("");
		deviceChoice13.setIdentifier("");
		deviceField67.setOffsetNid(1387);
		deviceField67.setLabelString("Threshold:");
		deviceField67.setNumCols(4);
		deviceField67.setIdentifier("");
		deviceChoice14.setChoiceIntValues(new int[]
		{ 0, 1 });
		deviceChoice14.setChoiceFloatValues(null);
		deviceChoice14.setConvert(true);
		deviceChoice14.setOffsetNid(1405);
		deviceChoice14.setLabelString("Phase");
		deviceChoice14.setChoiceItems(new String[]
		{ "Absolute", "Relative Br" });
		deviceChoice14.setUpdateIdentifier("");
		deviceChoice14.setIdentifier("");
		deviceField68.setOffsetNid(1407);
		deviceField68.setLabelString("Threshold:");
		deviceField68.setNumCols(4);
		deviceField68.setIdentifier("");
		deviceChoice15.setChoiceIntValues(new int[]
		{ 0, 1 });
		deviceChoice15.setChoiceFloatValues(null);
		deviceChoice15.setConvert(true);
		deviceChoice15.setOffsetNid(1425);
		deviceChoice15.setLabelString("Phase");
		deviceChoice15.setChoiceItems(new String[]
		{ "Absolute", "Relative Br" });
		deviceChoice15.setUpdateIdentifier("");
		deviceChoice15.setIdentifier("");
		deviceField69.setOffsetNid(1427);
		deviceField69.setLabelString("Treshold:");
		deviceField69.setNumCols(4);
		deviceField69.setIdentifier("");
		deviceTable11.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable11.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1" });
		deviceTable11.setIdentifier("");
		deviceTable11.setNumRows(4);
		deviceTable11.setNumCols(48);
		deviceTable11.setLabelString("Derivative Gain:");
		deviceTable11.setRefMode(DeviceTable.REFLEX);
		deviceTable11.setOffsetNid(1235);
		deviceTable16.setOffsetNid(1233);
		deviceTable16.setLabelString("Integral Gain:");
		deviceTable16.setRefMode(DeviceTable.REFLEX);
		deviceTable16.setNumCols(48);
		deviceTable16.setNumRows(4);
		deviceTable16.setIdentifier("");
		deviceTable16.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1" });
		deviceTable16.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable110.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable110.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1" });
		deviceTable110.setIdentifier("");
		deviceTable110.setNumRows(4);
		deviceTable110.setNumCols(48);
		deviceTable110.setLabelString("Gain Phase (Rad):");
		deviceTable110.setRefMode(DeviceTable.REFLEX_INVERT);
		deviceTable110.setOffsetNid(1231);
		deviceTable111.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable111.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2",
				"16<NL>5<NL>4<NL>3<NL>2<NL>1<NL>" });
		deviceTable111.setIdentifier("");
		deviceTable111.setNumRows(4);
		deviceTable111.setNumCols(48);
		deviceTable111.setLabelString("Gain Phase (Rad):");
		deviceTable111.setRefMode(DeviceTable.REFLEX_INVERT);
		deviceTable111.setOffsetNid(1243);
		deviceTable112.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable112.setColumnNames(new String[]
		{ "0", "-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10", "-11", "-12", "-13", "-14", "-15", "-16",
				"-17", "-18", "-19", "-20", "-21", "-22", "-23", "-24", "23", "22", "21", "20", "19", "18", "17", "16",
				"15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1" });
		deviceTable112.setIdentifier("");
		deviceTable112.setNumRows(4);
		deviceTable112.setNumCols(48);
		deviceTable112.setLabelString("Gain Phase (Rad):");
		deviceTable112.setRefMode(DeviceTable.REFLEX_INVERT);
		deviceTable112.setOffsetNid(1255);
		deviceField73.setOffsetNid(1261);
		deviceField73.setLabelString("Derivative Cutoff freq (Hz):");
		deviceField73.setNumCols(5);
		deviceField73.setIdentifier("");
		deviceChoice5.setChoiceIntValues(new int[]
		{ 1, 0 });
		deviceChoice5.setChoiceFloatValues(null);
		deviceChoice5.setConvert(true);
		deviceChoice5.setOffsetNid(1439);
		deviceChoice5.setLabelString("M1 N0 Sine Component");
		deviceChoice5.setChoiceItems(new String[]
		{ "Excluded", "Included" });
		deviceChoice5.setUpdateIdentifier("");
		deviceChoice5.setIdentifier("");
		deviceField71.setOffsetNid(1437);
		deviceField71.setLabelString("Br Measure Radius");
		deviceField71.setNumCols(5);
		deviceField71.setIdentifier("");
		deviceField70.setOffsetNid(1443);
		deviceField70.setLabelString("Bt Cutoff Freqyency ");
		deviceField70.setNumCols(5);
		deviceField70.setIdentifier("");
		deviceTable20.setOffsetNid(1441);
		deviceTable20.setLabelString("Sideband Correction ranges");
		deviceTable20.setNumCols(2);
		deviceTable20.setNumRows(4);
		deviceTable20.setIdentifier("");
		deviceTable20.setColumnNames(new String[]
		{ "NMin", "NMax" });
		deviceTable20.setRowNames(new String[]
		{ "0", "1", "2", "3" });
		deviceTable20.setDisplayRowNumber(true);
		deviceTable21.setOffsetNid(1081);
		deviceTable21.setLabelString("Kp");
		deviceTable21.setNumCols(48);
		deviceTable21.setNumRows(4);
		deviceTable21.setIdentifier("");
		deviceTable21.setColumnNames(new String[]
		{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
				"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37",
				"38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48" });
		deviceTable21.setRowNames(null);
		deviceTable21.setDisplayRowNumber(true);
		deviceField72.setOffsetNid(1089);
		deviceField72.setLabelString("Derivative Cutoff Frequency (Hz): ");
		deviceField72.setNumCols(5);
		deviceField72.setIdentifier("");
		this.getContentPane().add(jPanel9, BorderLayout.NORTH);
		jPanel9.add(jPanel40, BorderLayout.CENTER);
		jPanel40.add(jPanel10, null);
		jPanel10.add(deviceField1, null);
		jPanel40.add(jPanel25, null);
		jPanel40.add(jPanel26, null);
		jPanel40.add(jPanel27, null);
		jPanel27.add(deviceChoice2, null);
		jPanel40.add(jPanel28, null);
		jPanel40.add(jPanel29, null);
		jPanel40.add(jPanel30, null);
		this.getContentPane().add(jPanel33, BorderLayout.CENTER);
		jPanel33.add(jTabbedPane1, BorderLayout.CENTER);
		jPanel6.add(deviceField20, null);
		jPanel6.add(deviceField30, null);
		jPanel6.add(deviceField21, null);
		jPanel6.add(deviceField22, null);
		jPanel6.add(deviceField23, null);
		jPanel6.add(deviceField24, null);
		jPanel6.add(deviceField25, null);
		jPanel6.add(deviceField16, null);
		jPanel6.add(deviceChoice8, null);
		jPanel6.add(deviceField63, null);
		jPanel5.add(jPanel24, null);
		jPanel5.add(jPanel6, null);
		jPanel24.add(deviceField2111, null);
		jPanel24.add(deviceField31, null);
		jPanel24.add(deviceField2113, null);
		jPanel24.add(deviceField2110, null);
		jPanel24.add(deviceField2112, null);
		jPanel24.add(deviceField219, null);
		jPanel24.add(deviceField218, null);
		jPanel24.add(deviceField40, null);
		jPanel24.add(deviceChoice9, null);
		jPanel24.add(deviceField62, null);
		jPanel5.add(jPanel23, null);
		jPanel23.add(deviceField215, null);
		jPanel23.add(deviceField32, null);
		jPanel23.add(deviceField217, null);
		jPanel23.add(deviceField214, null);
		jPanel23.add(deviceField216, null);
		jPanel23.add(deviceField213, null);
		jPanel23.add(deviceField212, null);
		jPanel23.add(deviceField41, null);
		jPanel23.add(deviceChoice10, null);
		jPanel23.add(deviceField64, null);
		jPanel5.add(jPanel4111, null);
		jPanel4111.add(deviceField2213, null);
		jPanel4111.add(deviceField313, null);
		jPanel4111.add(deviceField21110, null);
		jPanel4111.add(deviceField2212, null);
		jPanel4111.add(deviceField21111, null);
		jPanel4111.add(deviceField2215, null);
		jPanel4111.add(deviceField2214, null);
		jPanel4111.add(deviceField414, null);
		jPanel4111.add(deviceChoice11, null);
		jPanel4111.add(deviceField65, null);
		jPanel5.add(jPanel4110, null);
		jPanel4110.add(deviceField229, null);
		jPanel4110.add(deviceField312, null);
		jPanel4110.add(deviceField2118, null);
		jPanel4110.add(deviceField228, null);
		jPanel4110.add(deviceField2119, null);
		jPanel4110.add(deviceField2216, null);
		jPanel4110.add(deviceField2210, null);
		jPanel4110.add(deviceField413, null);
		jPanel4110.add(deviceChoice12, null);
		jPanel4110.add(deviceField66, null);
		jPanel5.add(jPanel419, null);
		jPanel419.add(deviceField225, null);
		jPanel419.add(deviceField311, null);
		jPanel419.add(deviceField2116, null);
		jPanel419.add(deviceField224, null);
		jPanel419.add(deviceField2117, null);
		jPanel419.add(deviceField227, null);
		jPanel419.add(deviceField226, null);
		jPanel419.add(deviceField412, null);
		jPanel419.add(deviceChoice13, null);
		jPanel419.add(deviceField67, null);
		jPanel5.add(jPanel43, null);
		jPanel43.add(deviceField221, null);
		jPanel43.add(deviceField310, null);
		jPanel43.add(deviceField2114, null);
		jPanel43.add(deviceField220, null);
		jPanel43.add(deviceField2115, null);
		jPanel43.add(deviceField223, null);
		jPanel43.add(deviceField222, null);
		jPanel43.add(deviceField411, null);
		jPanel43.add(deviceChoice14, null);
		jPanel43.add(deviceField68, null);
		jPanel5.add(jPanel7, null);
		jPanel7.add(deviceField29, null);
		jPanel7.add(deviceField33, null);
		jPanel7.add(deviceField211, null);
		jPanel7.add(deviceField28, null);
		jPanel7.add(deviceField210, null);
		jPanel7.add(deviceField27, null);
		jPanel7.add(deviceField26, null);
		jPanel7.add(deviceField42, null);
		jPanel7.add(deviceChoice15, null);
		jPanel7.add(deviceField69, null);
		jTabbedPane1.add(jScrollPane5, "Mode Control 1");
		jScrollPane5.getViewport().add(jPanel42, null);
		jPanel44.add(deviceField73, null);
		jPanel44.add(deviceField35, null);
		jPanel44.add(deviceField34, null);
		jPanel42.add(jPanel47, BorderLayout.CENTER);
		jPanel42.add(jPanel44, BorderLayout.NORTH);
		jPanel47.add(deviceTable7, null);
		jPanel47.add(deviceTable110, null);
		jPanel47.add(deviceTable16, null);
		jPanel47.add(deviceTable11, null);
		jTabbedPane1.add(jScrollPane14, "Mode Control 2");
		jTabbedPane1.add(jScrollPane7, "Mode Control 3");
		jTabbedPane1.add(jScrollPane15, "Virtual Shell: PID Parameters");
		jTabbedPane1.add(jScrollPane9, "Virtual Shell: Input Config. 1");
		jTabbedPane1.add(jScrollPane10, "Virtual Shell: Input Config. 2");
		jScrollPane10.getViewport().add(jPanel57, null);
		jPanel57.add(deviceTable25, BorderLayout.CENTER);
		jPanel57.add(jPanel58, BorderLayout.NORTH);
		jPanel58.add(deviceField46, null);
		jPanel58.add(deviceField45, null);
		jPanel57.add(deviceTable28, BorderLayout.SOUTH);
		jTabbedPane1.add(jScrollPane11, "Virtual Shell: Input Config. 3");
		jTabbedPane1.add(jPanel70, "MHD - BR");
		jScrollPane11.getViewport().add(jPanel59, null);
		jPanel59.add(deviceTable26, BorderLayout.CENTER);
		jPanel59.add(jPanel510, BorderLayout.NORTH);
		jPanel510.add(deviceField48, null);
		jPanel510.add(deviceField47, null);
		jPanel59.add(deviceTable210, BorderLayout.SOUTH);
		jScrollPane9.getViewport().add(jPanel55, null);
		jPanel55.add(deviceTable22, BorderLayout.CENTER);
		jPanel55.add(jPanel56, BorderLayout.NORTH);
		jPanel56.add(deviceField44, null);
		jPanel56.add(deviceField43, null);
		jPanel55.add(deviceTable27, BorderLayout.SOUTH);
		jScrollPane15.getViewport().add(jPanel54, null);
		jPanel54.add(jPanel71, null);
		jPanel71.add(deviceField72, null);
		jPanel54.add(deviceTable21, null);
		jPanel54.add(deviceTable24, null);
		jPanel54.add(deviceTable23, null);
		jScrollPane7.getViewport().add(jPanel417, null);
		jPanel416.add(deviceTable9, null);
		jPanel416.add(deviceTable112, null);
		jPanel416.add(deviceTable19, null);
		jPanel416.add(deviceTable15, null);
		jPanel417.add(jPanel418, BorderLayout.NORTH);
		jPanel417.add(jPanel416, BorderLayout.CENTER);
		jPanel418.add(deviceField39, null);
		jPanel418.add(deviceField38, null);
		jScrollPane14.getViewport().add(jPanel45, null);
		jPanel415.add(deviceTable8, null);
		jPanel415.add(deviceTable111, null);
		jPanel415.add(deviceTable18, null);
		jPanel415.add(deviceTable13, null);
		jPanel45.add(jPanel46, BorderLayout.NORTH);
		jPanel45.add(jPanel415, BorderLayout.CENTER);
		jPanel46.add(deviceField37, null);
		jPanel46.add(deviceField36, null);
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		jPanel9.add(jPanel31, BorderLayout.SOUTH);
		jPanel70.add(deviceChoice5, null);
		jPanel70.add(deviceField71, null);
		jPanel70.add(deviceField70, null);
		jPanel70.add(deviceTable20, null);
		jTabbedPane1.add(jScrollPane4, "Rot. Perturbation");
		jScrollPane4.getViewport().add(jPanel5, null);
	}
}

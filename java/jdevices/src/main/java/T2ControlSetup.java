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
public class T2ControlSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JTabbedPane jTabbedPane1 = new JTabbedPane();
	JPanel jPanel1 = new JPanel();
	BorderLayout borderLayout2 = new BorderLayout();
	JPanel jPanel2 = new JPanel();
	DeviceTable deviceTable1 = new DeviceTable();
	DeviceTable deviceTable2 = new DeviceTable();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel3 = new JPanel();
	GridLayout gridLayout2 = new GridLayout();
	JPanel jPanel4 = new JPanel();
	JPanel jPanel5 = new JPanel();
	JPanel jPanel6 = new JPanel();
	JPanel jPanel7 = new JPanel();
	JPanel jPanel8 = new JPanel();
	JPanel jPanel9 = new JPanel();
	DeviceField deviceField1 = new DeviceField();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceField deviceField9 = new DeviceField();
	DeviceChoice deviceChoice3 = new DeviceChoice();
	DeviceField deviceField10 = new DeviceField();
	DeviceChoice deviceChoice4 = new DeviceChoice();
	DeviceChoice deviceChoice5 = new DeviceChoice();
	JPanel jPanel10 = new JPanel();
	DeviceField deviceField11 = new DeviceField();
	DeviceChoice deviceChoice6 = new DeviceChoice();
	JPanel jPanel11 = new JPanel();
	JPanel jPanel12 = new JPanel();
	JPanel jPanel13 = new JPanel();
	JPanel jPanel14 = new JPanel();
	JPanel jPanel15 = new JPanel();
	JPanel jPanel16 = new JPanel();
	JPanel jPanel17 = new JPanel();
	DeviceChoice deviceChoice7 = new DeviceChoice();
	DeviceChoice deviceChoice8 = new DeviceChoice();
	DeviceField deviceField12 = new DeviceField();
	DeviceField deviceField13 = new DeviceField();
	DeviceField deviceField14 = new DeviceField();
	GridLayout gridLayout3 = new GridLayout();
	JPanel jPanel18 = new JPanel();
	DeviceField deviceField15 = new DeviceField();
	DeviceField deviceField16 = new DeviceField();
	DeviceField deviceField17 = new DeviceField();
	DeviceField deviceField18 = new DeviceField();
	DeviceField deviceField19 = new DeviceField();
	DeviceField deviceField110 = new DeviceField();
	DeviceField deviceField111 = new DeviceField();
	JPanel jPanel19 = new JPanel();
	DeviceField deviceField112 = new DeviceField();
	DeviceField deviceField113 = new DeviceField();
	DeviceField deviceField114 = new DeviceField();
	DeviceField deviceField115 = new DeviceField();
	JPanel jPanel110 = new JPanel();
	DeviceField deviceField116 = new DeviceField();
	DeviceField deviceField117 = new DeviceField();
	DeviceField deviceField118 = new DeviceField();
	DeviceField deviceField119 = new DeviceField();
	JPanel jPanel111 = new JPanel();
	DeviceField deviceField1110 = new DeviceField();
	BorderLayout borderLayout3 = new BorderLayout();
	JScrollPane jScrollPane1 = new JScrollPane();
	JPanel jPanel20 = new JPanel();
	GridLayout gridLayout4 = new GridLayout();
	DeviceField deviceField25 = new DeviceField();
	DeviceField deviceField24 = new DeviceField();
	DeviceField deviceField22 = new DeviceField();
	JPanel jPanel21 = new JPanel();
	DeviceField deviceField20 = new DeviceField();
	DeviceField deviceField23 = new DeviceField();
	DeviceField deviceField21 = new DeviceField();
	DeviceField deviceField26 = new DeviceField();
	DeviceField deviceField27 = new DeviceField();
	DeviceField deviceField28 = new DeviceField();
	DeviceField deviceField29 = new DeviceField();
	DeviceField deviceField210 = new DeviceField();
	DeviceField deviceField211 = new DeviceField();
	JPanel jPanel22 = new JPanel();
	DeviceField deviceField212 = new DeviceField();
	DeviceField deviceField213 = new DeviceField();
	DeviceField deviceField214 = new DeviceField();
	DeviceField deviceField215 = new DeviceField();
	DeviceField deviceField216 = new DeviceField();
	DeviceField deviceField217 = new DeviceField();
	JPanel jPanel23 = new JPanel();
	DeviceField deviceField218 = new DeviceField();
	DeviceField deviceField219 = new DeviceField();
	DeviceField deviceField2110 = new DeviceField();
	DeviceField deviceField2111 = new DeviceField();
	DeviceField deviceField2112 = new DeviceField();
	DeviceField deviceField2113 = new DeviceField();
	JPanel jPanel24 = new JPanel();
	BorderLayout borderLayout4 = new BorderLayout();
	BorderLayout borderLayout5 = new BorderLayout();
	DeviceTable deviceTable5 = new DeviceTable();
	DeviceTable deviceTable3 = new DeviceTable();
	GridLayout gridLayout5 = new GridLayout();
	JPanel jPanel25 = new JPanel();
	DeviceTable deviceTable4 = new DeviceTable();
	BorderLayout borderLayout6 = new BorderLayout();
	DeviceTable deviceTable6 = new DeviceTable();
	GridLayout gridLayout6 = new GridLayout();
	DeviceTable deviceTable7 = new DeviceTable();
	DeviceTable deviceTable8 = new DeviceTable();
	DeviceTable deviceTable9 = new DeviceTable();
	DeviceTable deviceTable10 = new DeviceTable();

	public T2ControlSetup()
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
		this.setHeight(600);
		this.setDeviceType("T2Control");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("T2 Feedback Control");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(borderLayout2);
		jPanel2.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(2);
		deviceTable2.setOffsetNid(4);
		deviceTable2.setLabelString("In Calibration");
		deviceTable2.setNumCols(2);
		deviceTable2.setNumRows(96);
		deviceTable2.setIdentifier("");
		deviceTable2.setColumnNames(new String[]
		{ "Gain", "Offset" });
		deviceTable2.setRowNames(null);
		deviceTable2.setDisplayRowNumber(true);
		deviceTable1.setOffsetNid(5);
		deviceTable1.setLabelString("Out alibration");
		deviceTable1.setNumCols(2);
		deviceTable1.setNumRows(32);
		deviceTable1.setIdentifier("");
		deviceTable1.setColumnNames(new String[]
		{ "Gain", "Offset" });
		deviceTable1.setRowNames(null);
		deviceTable1.setDisplayRowNumber(true);
		jPanel3.setLayout(gridLayout2);
		gridLayout2.setColumns(1);
		gridLayout2.setHgap(0);
		gridLayout2.setRows(7);
		deviceField2.setOffsetNid(1);
		deviceField2.setTextOnly(true);
		deviceField2.setLabelString("Comment: ");
		deviceField2.setNumCols(20);
		deviceField2.setIdentifier("");
		deviceField1.setOffsetNid(2);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("VME IP: ");
		deviceField1.setIdentifier("");
		deviceField5.setOffsetNid(8);
		deviceField5.setLabelString("Trigger time(s):");
		deviceField5.setNumCols(5);
		deviceField5.setIdentifier("");
		deviceField4.setOffsetNid(11);
		deviceField4.setLabelString("Start sampling time(s):");
		deviceField4.setNumCols(5);
		deviceField4.setIdentifier("");
		deviceField3.setOffsetNid(12);
		deviceField3.setLabelString("End sampling time(s):");
		deviceField3.setNumCols(5);
		deviceField3.setIdentifier("");
		deviceField8.setOffsetNid(3);
		deviceField8.setLabelString("Frequency(Hz):");
		deviceField8.setNumCols(5);
		deviceField8.setIdentifier("");
		deviceField7.setOffsetNid(9);
		deviceField7.setLabelString("Control duration(s):");
		deviceField7.setNumCols(5);
		deviceField7.setIdentifier("");
		deviceField6.setOffsetNid(10);
		deviceField6.setLabelString("Sys. duration(s):");
		deviceField6.setNumCols(5);
		deviceField6.setIdentifier("");
		deviceChoice2.setChoiceIntValues(new int[]
		{ 1, 5, 2, 3, 4, 6, 7, 8, 9 });
		deviceChoice2.setChoiceFloatValues(null);
		deviceChoice2.setConvert(true);
		deviceChoice2.setOffsetNid(6);
		deviceChoice2.setLabelString("Initial Control: ");
		deviceChoice2.setChoiceItems(new String[]
		{ "Offset Correction", "Individual Perturbation", "Intelligent Shell", "Rotating Perturbation",
				"Mode Control (Diag)", "Mode control (Full)", "Mode control (Diag-Plasma)",
				"Mode Control (Diag+Rot/Times)", "Mode Control (Diag+Rot/sinusoidal)" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		deviceChoice1.setChoiceIntValues(new int[]
		{ 1, 5, 2, 3, 4, 6, 7, 8, 9 });
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setConvert(true);
		deviceChoice1.setOffsetNid(7);
		deviceChoice1.setLabelString("Trigger control: ");
		deviceChoice1.setChoiceItems(new String[]
		{ "Offset Correction", "Individual Perturbation", "Intelligent Shell", "Rotating Perturbation",
				"Mode Control (Diag)", "Mode control (Full)", "Mode control (Diag-Plasma)",
				"Mode Control (Diag+Rot/Times)", "Mode Control (Diag+Rot/Sinusoidal)" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceField10.setOffsetNid(127);
		deviceField10.setLabelString("Autozero start(s):");
		deviceField10.setNumCols(5);
		deviceField10.setIdentifier("");
		deviceField9.setOffsetNid(128);
		deviceField9.setLabelString("Autozero end(s):");
		deviceField9.setNumCols(5);
		deviceField9.setIdentifier("");
		deviceChoice3.setChoiceIntValues(new int[]
		{ 1, 2, 3, 4, 5, 6, 7 });
		deviceChoice3.setChoiceFloatValues(null);
		deviceChoice3.setConvert(true);
		deviceChoice3.setOffsetNid(130);
		deviceChoice3.setLabelString("Mapping:");
		deviceChoice3.setChoiceItems(new String[]
		{ "(OLD)16 Equispaced Coils", "(OLD)16 Coils", "(OLD)8 Coils", "16 Toroidal angles in-out&top-bottom",
				"32 Toroidal angles in-out only", "32 Toroidal angles top-bottom only",
				"32 Toroidal angles in-out alternated to top-bottom" });
		deviceChoice3.setUpdateIdentifier("");
		deviceChoice3.setIdentifier("");
		deviceChoice5.setChoiceIntValues(new int[]
		{ 0, 1 });
		deviceChoice5.setChoiceFloatValues(null);
		deviceChoice5.setConvert(true);
		deviceChoice5.setOffsetNid(114);
		deviceChoice5.setLabelString("Field to current transform:");
		deviceChoice5.setChoiceItems(new String[]
		{ "Disabled", "Enabled" });
		deviceChoice5.setUpdateIdentifier("");
		deviceChoice5.setIdentifier("");
		deviceChoice4.setChoiceIntValues(new int[]
		{ 0, 1 });
		deviceChoice4.setChoiceFloatValues(null);
		deviceChoice4.setConvert(true);
		deviceChoice4.setOffsetNid(120);
		deviceChoice4.setLabelString("Current to voltage transform:");
		deviceChoice4.setChoiceItems(new String[]
		{ "Disabled", "Enabled" });
		deviceChoice4.setUpdateIdentifier("");
		deviceChoice4.setIdentifier("");
		deviceChoice6.setChoiceIntValues(new int[]
		{ 0, 1 });
		deviceChoice6.setChoiceFloatValues(null);
		deviceChoice6.setConvert(true);
		deviceChoice6.setOffsetNid(110);
		deviceChoice6.setLabelString("Extrapolation: ");
		deviceChoice6.setChoiceItems(new String[]
		{ "Disabled", "Enabled" });
		deviceChoice6.setUpdateIdentifier("");
		deviceChoice6.setIdentifier("");
		deviceField11.setOffsetNid(112);
		deviceField11.setLabelString("Extrapolation delta(s):");
		deviceField11.setNumCols(5);
		deviceField11.setIdentifier("");
		deviceField14.setOffsetNid(14);
		deviceField14.setLabelString("Kp:");
		deviceField14.setNumCols(6);
		deviceField14.setIdentifier("");
		deviceField13.setOffsetNid(16);
		deviceField13.setLabelString("Ki:");
		deviceField13.setNumCols(6);
		deviceField13.setIdentifier("");
		deviceField12.setOffsetNid(18);
		deviceField12.setLabelString("Kd:");
		deviceField12.setNumCols(6);
		deviceField12.setIdentifier("");
		deviceChoice8.setChoiceIntValues(new int[]
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
				30, 31, 1000 });
		deviceChoice8.setChoiceFloatValues(null);
		deviceChoice8.setConvert(true);
		deviceChoice8.setOffsetNid(106);
		deviceChoice8.setLabelString("Exclude Mode:");
		deviceChoice8.setChoiceItems(new String[]
		{ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "-16", "-15", "-14",
				"-13", "-12", "-11", "-10", "-9", "-8", "-7", "-6", "-5", "-4", "-3", "-2", "-1", "None" });
		deviceChoice8.setUpdateIdentifier("");
		deviceChoice8.setIdentifier("");
		deviceChoice7.setChoiceIntValues(new int[]
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
				30, 31, 1000 });
		deviceChoice7.setChoiceFloatValues(null);
		deviceChoice7.setConvert(true);
		deviceChoice7.setOffsetNid(108);
		deviceChoice7.setLabelString("Exclude mode:");
		deviceChoice7.setChoiceItems(new String[]
		{ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "-16", "-15", "-14",
				"-13", "-12", "-11", "-10", "-9", "-8", "-7", "-6", "-5", "-4", "-3", "-2", "-1", "None" });
		deviceChoice7.setUpdateIdentifier("");
		deviceChoice7.setIdentifier("");
		jPanel12.setLayout(gridLayout3);
		gridLayout3.setColumns(1);
		gridLayout3.setRows(4);
		deviceField18.setOffsetNid(68);
		deviceField18.setLabelString("Out id: ");
		deviceField18.setNumCols(5);
		deviceField18.setIdentifier("");
		deviceField17.setOffsetNid(70);
		deviceField17.setLabelString("Amp.(V):");
		deviceField17.setNumCols(5);
		deviceField17.setIdentifier("");
		deviceField16.setOffsetNid(72);
		deviceField16.setLabelString("Frequency(Hz):");
		deviceField16.setNumCols(5);
		deviceField16.setIdentifier("");
		deviceField15.setOffsetNid(74);
		deviceField15.setLabelString("Phase(Rad.): ");
		deviceField15.setNumCols(5);
		deviceField15.setIdentifier("");
		deviceField19.setIdentifier("");
		deviceField19.setNumCols(5);
		deviceField19.setLabelString("Amp.(V):");
		deviceField19.setOffsetNid(94);
		deviceField110.setIdentifier("");
		deviceField110.setNumCols(5);
		deviceField110.setLabelString("Out id: ");
		deviceField110.setOffsetNid(92);
		deviceField111.setIdentifier("");
		deviceField111.setNumCols(5);
		deviceField111.setLabelString("Phase(Rad.): ");
		deviceField111.setOffsetNid(98);
		deviceField112.setOffsetNid(96);
		deviceField112.setLabelString("Frequency(Hz):");
		deviceField112.setNumCols(5);
		deviceField112.setIdentifier("");
		deviceField113.setIdentifier("");
		deviceField113.setNumCols(5);
		deviceField113.setLabelString("Amp.(V):");
		deviceField113.setOffsetNid(86);
		deviceField114.setIdentifier("");
		deviceField114.setNumCols(5);
		deviceField114.setLabelString("Out id: ");
		deviceField114.setOffsetNid(84);
		deviceField115.setIdentifier("");
		deviceField115.setNumCols(5);
		deviceField115.setLabelString("Phase(Rad.): ");
		deviceField115.setOffsetNid(90);
		deviceField116.setOffsetNid(88);
		deviceField116.setLabelString("Frequency(Hz):");
		deviceField116.setNumCols(5);
		deviceField116.setIdentifier("");
		deviceField117.setIdentifier("");
		deviceField117.setNumCols(5);
		deviceField117.setLabelString("Amp.(V):");
		deviceField117.setOffsetNid(78);
		deviceField118.setIdentifier("");
		deviceField118.setNumCols(5);
		deviceField118.setLabelString("Out id: ");
		deviceField118.setOffsetNid(76);
		deviceField119.setIdentifier("");
		deviceField119.setNumCols(5);
		deviceField119.setLabelString("Phase(Rad.): ");
		deviceField119.setOffsetNid(82);
		deviceField1110.setOffsetNid(80);
		deviceField1110.setLabelString("Frequency(Hz):");
		deviceField1110.setNumCols(5);
		deviceField1110.setIdentifier("");
		jPanel13.setLayout(borderLayout3);
		jPanel20.setLayout(gridLayout4);
		gridLayout4.setColumns(1);
		gridLayout4.setRows(4);
		deviceField25.setIdentifier("");
		deviceField25.setNumCols(5);
		deviceField25.setLabelString("Rot. Phase(Rad):");
		deviceField25.setOffsetNid(30);
		deviceField24.setIdentifier("");
		deviceField24.setNumCols(2);
		deviceField24.setLabelString("N1: ");
		deviceField24.setOffsetNid(20);
		deviceField22.setIdentifier("");
		deviceField22.setNumCols(5);
		deviceField22.setLabelString("Freq(Hz)/Start:");
		deviceField22.setOffsetNid(24);
		deviceField20.setOffsetNid(28);
		deviceField20.setLabelString("Rot. Freq(Hz):");
		deviceField20.setNumCols(5);
		deviceField20.setIdentifier("");
		deviceField23.setOffsetNid(22);
		deviceField23.setLabelString("Amp:");
		deviceField23.setNumCols(5);
		deviceField23.setIdentifier("");
		deviceField21.setOffsetNid(26);
		deviceField21.setLabelString("Phase(Rad)/End:");
		deviceField21.setNumCols(5);
		deviceField21.setIdentifier("");
		deviceField26.setIdentifier("");
		deviceField26.setNumCols(5);
		deviceField26.setLabelString("Rot. Phase(Rad):");
		deviceField26.setOffsetNid(66);
		deviceField27.setIdentifier("");
		deviceField27.setNumCols(2);
		deviceField27.setLabelString("N4: ");
		deviceField27.setOffsetNid(56);
		deviceField28.setIdentifier("");
		deviceField28.setNumCols(5);
		deviceField28.setLabelString("Freq(Hz)/Start:");
		deviceField28.setOffsetNid(60);
		deviceField29.setOffsetNid(64);
		deviceField29.setLabelString("Rot. Freq(Hz):");
		deviceField29.setNumCols(5);
		deviceField29.setIdentifier("");
		deviceField210.setOffsetNid(58);
		deviceField210.setLabelString("Amp:");
		deviceField210.setNumCols(5);
		deviceField210.setIdentifier("");
		deviceField211.setOffsetNid(62);
		deviceField211.setLabelString("Phase(Rad)/End:");
		deviceField211.setNumCols(5);
		deviceField211.setIdentifier("");
		deviceField212.setIdentifier("");
		deviceField212.setNumCols(5);
		deviceField212.setLabelString("Rot. Phase(Rad):");
		deviceField212.setOffsetNid(54);
		deviceField213.setIdentifier("");
		deviceField213.setNumCols(2);
		deviceField213.setLabelString("N3: ");
		deviceField213.setOffsetNid(44);
		deviceField214.setIdentifier("");
		deviceField214.setNumCols(5);
		deviceField214.setLabelString("Freq(Hz)/Start:");
		deviceField214.setOffsetNid(48);
		deviceField215.setOffsetNid(52);
		deviceField215.setLabelString("Rot. Freq(Hz):");
		deviceField215.setNumCols(5);
		deviceField215.setIdentifier("");
		deviceField216.setOffsetNid(46);
		deviceField216.setLabelString("Amp:");
		deviceField216.setNumCols(5);
		deviceField216.setIdentifier("");
		deviceField217.setOffsetNid(50);
		deviceField217.setLabelString("Phase(Rad)/End:");
		deviceField217.setNumCols(5);
		deviceField217.setIdentifier("");
		deviceField218.setIdentifier("");
		deviceField218.setNumCols(5);
		deviceField218.setLabelString("Rot. Phase(Rad):");
		deviceField218.setOffsetNid(42);
		deviceField219.setIdentifier("");
		deviceField219.setNumCols(2);
		deviceField219.setLabelString("N2: ");
		deviceField219.setOffsetNid(32);
		deviceField2110.setIdentifier("");
		deviceField2110.setNumCols(5);
		deviceField2110.setLabelString("Freq(Hz)/Start:");
		deviceField2110.setOffsetNid(36);
		deviceField2111.setOffsetNid(40);
		deviceField2111.setLabelString("Rot. Freq(Hz):");
		deviceField2111.setNumCols(5);
		deviceField2111.setIdentifier("");
		deviceField2112.setOffsetNid(34);
		deviceField2112.setLabelString("Amp:");
		deviceField2112.setNumCols(5);
		deviceField2112.setIdentifier("");
		deviceField2113.setOffsetNid(38);
		deviceField2113.setLabelString("Phase(Rad)/End:");
		deviceField2113.setNumCols(5);
		deviceField2113.setIdentifier("");
		jPanel14.setLayout(borderLayout4);
		jPanel15.setLayout(borderLayout5);
		deviceTable5.setOffsetNid(100);
		deviceTable5.setLabelString("Full Mode control Kp");
		deviceTable5.setNumCols(32);
		deviceTable5.setNumRows(32);
		deviceTable5.setIdentifier("");
		deviceTable5.setColumnNames(new String[]
		{ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "-16", "-15", "-14",
				"-13", "-12", "-11", "-10", "-9", "-8", "-7", "-6", "-5", "-4", "-3", "-2", "-1" });
		deviceTable5.setRowNames(new String[]
		{ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "-16", "-15", "-14",
				"-13", "-12", "-11", "-10", "-9", "-8", "-7", "-6", "-5", "-4", "-3", "-2", "-1" });
		deviceTable3.setOffsetNid(102);
		deviceTable3.setLabelString("Real Diagonal Kp");
		deviceTable3.setNumCols(32);
		deviceTable3.setNumRows(1);
		deviceTable3.setIdentifier("");
		deviceTable3.setColumnNames(new String[]
		{ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "-16", "-15", "-14",
				"-13", "-12", "-11", "-10", "-9", "-8", "-7", "-6", "-5", "-4", "-3", "-2", "-1" });
		deviceTable3.setRowNames(null);
		gridLayout5.setColumns(1);
		gridLayout5.setRows(2);
		jPanel25.setLayout(gridLayout5);
		deviceTable4.setOffsetNid(104);
		deviceTable4.setLabelString("Imaginary Diagonal Kp");
		deviceTable4.setNumCols(32);
		deviceTable4.setNumRows(1);
		deviceTable4.setIdentifier("");
		deviceTable4.setColumnNames(new String[]
		{ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "-16", "-15", "-14",
				"-13", "-12", "-11", "-10", "-9", "-8", "-7", "-6", "-5", "-4", "-3", "-2", "-1" });
		deviceTable4.setRowNames(null);
		jPanel16.setLayout(borderLayout6);
		deviceTable6.setOffsetNid(116);
		deviceTable6.setLabelString("Field to Current Transform");
		deviceTable6.setNumCols(40);
		deviceTable6.setNumRows(64);
		deviceTable6.setIdentifier("");
		deviceTable6.setColumnNames(new String[]
		{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
				"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37",
				"38", "39", "40" });
		deviceTable6.setRowNames(null);
		deviceTable6.setDisplayRowNumber(true);
		jPanel17.setLayout(gridLayout6);
		gridLayout6.setColumns(1);
		gridLayout6.setRows(3);
		deviceTable7.setOffsetNid(122);
		deviceTable7.setLabelString("Current to Voltage Kp");
		deviceTable7.setNumCols(32);
		deviceTable7.setNumRows(1);
		deviceTable7.setIdentifier("");
		deviceTable7.setColumnNames(new String[]
		{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
				"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32" });
		deviceTable7.setRowNames(null);
		deviceTable9.setOffsetNid(124);
		deviceTable9.setLabelString("Current to voltage Ki");
		deviceTable9.setNumCols(32);
		deviceTable9.setNumRows(1);
		deviceTable9.setIdentifier("");
		deviceTable9.setColumnNames(new String[]
		{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
				"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32" });
		deviceTable9.setRowNames(null);
		deviceTable8.setOffsetNid(126);
		deviceTable8.setLabelString("Current to voltage Kd");
		deviceTable8.setNumCols(32);
		deviceTable8.setNumRows(1);
		deviceTable8.setIdentifier("");
		deviceTable8.setColumnNames(new String[]
		{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
				"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32" });
		deviceTable8.setRowNames(null);
		deviceTable10.setOffsetNid(118);
		deviceTable10.setLabelString("Mode Selection");
		deviceTable10.setNumCols(16);
		deviceTable10.setNumRows(1);
		deviceTable10.setIdentifier("");
		deviceTable10.setColumnNames(new String[]
		{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" });
		deviceTable10.setRowNames(null);
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jTabbedPane1, BorderLayout.CENTER);
		jTabbedPane1.add(jPanel11, "Intelligent Shell");
		jTabbedPane1.add(jPanel12, "Individual Perturbation");
		jTabbedPane1.add(jPanel13, "Rotating Perturbation");
		jTabbedPane1.add(jPanel14, "Diagonal Mode Control");
		jTabbedPane1.add(jPanel15, "Full Mode Control");
		jTabbedPane1.add(jPanel16, "Field to Current transform");
		jPanel16.add(deviceTable6, BorderLayout.CENTER);
		jPanel16.add(deviceTable10, BorderLayout.NORTH);
		jTabbedPane1.add(jPanel17, "Current to Voltage transform");
		jPanel17.add(deviceTable7, null);
		jPanel17.add(deviceTable9, null);
		jPanel17.add(deviceTable8, null);
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(jPanel2, BorderLayout.WEST);
		jPanel2.add(deviceTable2, null);
		jPanel2.add(deviceTable1, null);
		jPanel1.add(jPanel3, BorderLayout.CENTER);
		jPanel3.add(jPanel4, null);
		jPanel4.add(deviceField2, null);
		jPanel4.add(deviceField1, null);
		jPanel3.add(jPanel9, null);
		jPanel9.add(deviceField5, null);
		jPanel9.add(deviceField4, null);
		jPanel9.add(deviceField3, null);
		jPanel3.add(jPanel8, null);
		jPanel8.add(deviceField8, null);
		jPanel8.add(deviceField7, null);
		jPanel8.add(deviceField6, null);
		jPanel3.add(jPanel7, null);
		jPanel7.add(deviceChoice2, null);
		jPanel7.add(deviceChoice1, null);
		jPanel3.add(jPanel6, null);
		jPanel6.add(deviceField10, null);
		jPanel6.add(deviceField9, null);
		jPanel6.add(deviceChoice3, null);
		jPanel3.add(jPanel5, null);
		jPanel5.add(deviceChoice5, null);
		jPanel5.add(deviceChoice4, null);
		jPanel3.add(jPanel10, null);
		jPanel10.add(deviceChoice6, null);
		jPanel10.add(deviceField11, null);
		jPanel11.add(deviceField14, null);
		jPanel11.add(deviceField13, null);
		jPanel11.add(deviceField12, null);
		jPanel11.add(deviceChoice8, null);
		jPanel11.add(deviceChoice7, null);
		jPanel12.add(jPanel18, null);
		jPanel18.add(deviceField18, null);
		jPanel18.add(deviceField17, null);
		jPanel18.add(deviceField16, null);
		jPanel18.add(deviceField15, null);
		jPanel12.add(jPanel111, null);
		jPanel111.add(deviceField118, null);
		jPanel111.add(deviceField117, null);
		jPanel111.add(deviceField1110, null);
		jPanel111.add(deviceField119, null);
		jPanel12.add(jPanel110, null);
		jPanel110.add(deviceField114, null);
		jPanel110.add(deviceField113, null);
		jPanel110.add(deviceField116, null);
		jPanel110.add(deviceField115, null);
		jPanel12.add(jPanel19, null);
		jPanel19.add(deviceField110, null);
		jPanel19.add(deviceField19, null);
		jPanel19.add(deviceField112, null);
		jPanel19.add(deviceField111, null);
		jPanel13.add(jScrollPane1, BorderLayout.NORTH);
		jScrollPane1.getViewport().add(jPanel20, null);
		jPanel20.add(jPanel21, null);
		jPanel21.add(deviceField24, null);
		jPanel21.add(deviceField23, null);
		jPanel21.add(deviceField22, null);
		jPanel21.add(deviceField21, null);
		jPanel21.add(deviceField20, null);
		jPanel21.add(deviceField25, null);
		jPanel20.add(jPanel24, null);
		jPanel24.add(deviceField219, null);
		jPanel24.add(deviceField2112, null);
		jPanel24.add(deviceField2110, null);
		jPanel24.add(deviceField2113, null);
		jPanel24.add(deviceField2111, null);
		jPanel24.add(deviceField218, null);
		jPanel20.add(jPanel23, null);
		jPanel23.add(deviceField213, null);
		jPanel23.add(deviceField216, null);
		jPanel23.add(deviceField214, null);
		jPanel23.add(deviceField217, null);
		jPanel23.add(deviceField215, null);
		jPanel23.add(deviceField212, null);
		jPanel20.add(jPanel22, null);
		jPanel22.add(deviceField27, null);
		jPanel22.add(deviceField210, null);
		jPanel22.add(deviceField28, null);
		jPanel22.add(deviceField211, null);
		jPanel22.add(deviceField29, null);
		jPanel22.add(deviceField26, null);
		jPanel15.add(deviceTable5, BorderLayout.CENTER);
		jPanel25.add(deviceTable3, null);
		jPanel25.add(deviceTable4, null);
		jPanel14.add(jPanel25, BorderLayout.CENTER);
	}
}

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
public class RFXTimesSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	JPanel jPanel5 = new JPanel();
	BorderLayout borderLayout2 = new BorderLayout();
	JPanel jPanel7 = new JPanel();
	JPanel jPanel8 = new JPanel();
	JPanel jPanel9 = new JPanel();
	JPanel jPanel10 = new JPanel();
	Border border1;
	TitledBorder titledBorder1;
	TitledBorder titledBorder2;
	Border border2;
	TitledBorder titledBorder3;
	TitledBorder titledBorder4;
	TitledBorder titledBorder5;
	TitledBorder titledBorder6;
	TitledBorder titledBorder7;
	TitledBorder titledBorder8;
	TitledBorder titledBorder9;
	TitledBorder titledBorder10;
	TitledBorder titledBorder11;
	GridLayout gridLayout1 = new GridLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel1 = new JPanel();
	JPanel jPanel2 = new JPanel();
	BorderLayout borderLayout4 = new BorderLayout();
	GridLayout gridLayout13 = new GridLayout();
	DeviceField deviceField19 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	JPanel jPanel15 = new JPanel();
	JPanel jPanel16 = new JPanel();
	GridLayout gridLayout14 = new GridLayout();
	DeviceField deviceField14 = new DeviceField();
	GridLayout gridLayout15 = new GridLayout();
	DeviceField deviceField1 = new DeviceField();
	JPanel jPanel17 = new JPanel();
	DeviceField deviceField3 = new DeviceField();
	GridLayout gridLayout16 = new GridLayout();
	DeviceField deviceField18 = new DeviceField();
	DeviceField deviceField20 = new DeviceField();
	DeviceField deviceField15 = new DeviceField();
	DeviceField deviceField9 = new DeviceField();
	JPanel jPanel18 = new JPanel();
	DeviceField deviceField16 = new DeviceField();
	DeviceField deviceField21 = new DeviceField();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField1113 = new DeviceField();
	JPanel jPanel19 = new JPanel();
	GridLayout gridLayout17 = new GridLayout();
	BorderLayout borderLayout5 = new BorderLayout();
	BorderLayout borderLayout6 = new BorderLayout();
	JPanel jPanel3 = new JPanel();
	JPanel jPanel4 = new JPanel();
	BorderLayout borderLayout7 = new BorderLayout();
	BorderLayout borderLayout8 = new BorderLayout();
	DeviceField deviceField1116 = new DeviceField();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField119 = new DeviceField();
	JPanel jPanel11 = new JPanel();
	GridLayout gridLayout7 = new GridLayout();
	DeviceField deviceField112 = new DeviceField();
	GridLayout gridLayout9 = new GridLayout();
	DeviceField deviceField1115 = new DeviceField();
	JPanel jPanel13 = new JPanel();
	DeviceField deviceField1118 = new DeviceField();
	BorderLayout borderLayout9 = new BorderLayout();
	JPanel jPanel20 = new JPanel();
	TitledBorder titledBorder12;
	GridLayout gridLayout3 = new GridLayout();
	DeviceField deviceField116 = new DeviceField();
	DeviceField deviceField12 = new DeviceField();
	DeviceField deviceField11113 = new DeviceField();
	DeviceField deviceField117 = new DeviceField();
	JPanel jPanel21 = new JPanel();
	TitledBorder titledBorder13;
	TitledBorder titledBorder14;
	DeviceField deviceField111 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();
	DeviceField deviceField1119 = new DeviceField();
	DeviceField deviceField113 = new DeviceField();
	DeviceField deviceField10 = new DeviceField();
	DeviceField deviceField11110 = new DeviceField();
	DeviceField deviceField114 = new DeviceField();
	DeviceField deviceField11 = new DeviceField();
	TitledBorder titledBorder15;
	GridLayout gridLayout6 = new GridLayout();
	BorderLayout borderLayout10 = new BorderLayout();
	DeviceField deviceField24 = new DeviceField();
	GridLayout gridLayout10 = new GridLayout();
	JPanel jPanel14 = new JPanel();
	DeviceField deviceField23 = new DeviceField();
	JPanel jPanel12 = new JPanel();
	DeviceField deviceField17 = new DeviceField();
	GridLayout gridLayout8 = new GridLayout();
	DeviceField deviceField4 = new DeviceField();
	TitledBorder titledBorder16;
	JPanel jPanel6 = new JPanel();
	DeviceField deviceField11116 = new DeviceField();
	TitledBorder titledBorder17;
	TitledBorder titledBorder18;
	DeviceField deviceField11117 = new DeviceField();
	GridLayout gridLayout11 = new GridLayout();
	DeviceField deviceField22 = new DeviceField();
	DeviceField deviceField1114 = new DeviceField();
	TitledBorder titledBorder19;
	JPanel jPanel25 = new JPanel();
	DeviceField deviceField110 = new DeviceField();
	DeviceField deviceField11114 = new DeviceField();
	DeviceField deviceField1112 = new DeviceField();
	GridLayout gridLayout4 = new GridLayout();
	DeviceField deviceField11115 = new DeviceField();
	JPanel jPanel23 = new JPanel();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField25 = new DeviceField();
	JPanel jPanel22 = new JPanel();
	GridLayout gridLayout5 = new GridLayout();
	DeviceField deviceField26 = new DeviceField();
	DeviceField deviceField27 = new DeviceField();
	BorderLayout borderLayout3 = new BorderLayout();
	DeviceField deviceField28 = new DeviceField();
	JPanel jPanel24 = new JPanel();
	DeviceField deviceField29 = new DeviceField();
	GridLayout gridLayout2 = new GridLayout();

	public RFXTimesSetup()
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
		titledBorder1 = new TitledBorder(border1, "PM");
		titledBorder2 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140)), "PV");
		border2 = new EtchedBorder(EtchedBorder.RAISED, Color.white, new Color(148, 145, 140));
		titledBorder3 = new TitledBorder(border2, "PC");
		titledBorder4 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140)), "PR");
		titledBorder5 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140)), "PC");
		titledBorder6 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140)), "P");
		titledBorder7 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140)), "TF");
		titledBorder8 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140)),
				"Toroidal Static Switches IS");
		titledBorder9 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140)),
				"Toroidal Choppers");
		titledBorder10 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140)),
				"Toroidal Inverters");
		titledBorder11 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140)),
				"Toroidal Crowbar");
		titledBorder12 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140)),
				"PP");
		titledBorder13 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140)),
				"PTSO");
		titledBorder14 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140)),
				"PTCB");
		titledBorder15 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140)),
				"PTCT");
		titledBorder16 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140)),
				"GAS PUFFING");
		titledBorder17 = new TitledBorder("");
		titledBorder18 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140)),
				"PM");
		titledBorder19 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white, new Color(148, 145, 140)),
				"RFX");
		this.setWidth(750);
		this.setHeight(750);
		this.setDeviceType("RFXTimes");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("RFXTimesSetup");
		this.getContentPane().setLayout(borderLayout1);
		jPanel5.setLayout(borderLayout2);
		jPanel7.setLayout(gridLayout1);
		jPanel8.setLayout(borderLayout9);
		jPanel9.setLayout(borderLayout10);
		jPanel10.setLayout(borderLayout6);
		gridLayout1.setColumns(3);
		jPanel2.setLayout(borderLayout4);
		gridLayout13.setColumns(1);
		gridLayout13.setRows(2);
		deviceField19.setShowState(true);
		deviceField19.setIdentifier("STOP_TF");
		deviceField19.setNumCols(5);
		deviceField19.setLabelString("T_STOP_TF");
		deviceField19.setOffsetNid(24);
		deviceField19.setDisplayEvaluated(false);
		deviceField6.setOffsetNid(23);
		deviceField6.setLabelString("T_START_TF");
		deviceField6.setNumCols(5);
		deviceField6.setIdentifier("START_TF");
		deviceField6.setShowState(true);
		jPanel15.setBorder(titledBorder7);
		jPanel15.setLayout(gridLayout13);
		jPanel16.setBorder(titledBorder8);
		jPanel16.setLayout(gridLayout14);
		gridLayout14.setColumns(1);
		gridLayout14.setRows(2);
		deviceField14.setOffsetNid(32);
		deviceField14.setLabelString("T_START_CHOP");
		deviceField14.setNumCols(5);
		deviceField14.setIdentifier("START_CHOP_TC");
		deviceField14.setShowState(true);
		deviceField14.setDisplayEvaluated(false);
		deviceField14.setPreferredWidth(-1);
		gridLayout15.setColumns(1);
		gridLayout15.setRows(2);
		deviceField1.setOffsetNid(33);
		deviceField1.setLabelString("T_STOP_CHOP");
		deviceField1.setNumCols(5);
		deviceField1.setIdentifier("STOP_CHOP_TC");
		deviceField1.setShowState(false);
		jPanel17.setBorder(titledBorder9);
		jPanel17.setLayout(gridLayout15);
		deviceField3.setOffsetNid(37);
		deviceField3.setLabelString("T_STOP_FORCED");
		deviceField3.setNumCols(5);
		deviceField3.setIdentifier("STOP_PWM_TC");
		deviceField3.setShowState(false);
		gridLayout16.setColumns(1);
		gridLayout16.setRows(8);
		deviceField18.setOffsetNid(42);
		deviceField18.setLabelString("T_STOP_REF_INV");
		deviceField18.setNumCols(5);
		deviceField18.setIdentifier("STOP_INV_TC_REF");
		deviceField18.setShowState(false);
		deviceField20.setOffsetNid(43);
		deviceField20.setLabelString("T_START_FEEDF_INV");
		deviceField20.setNumCols(5);
		deviceField20.setIdentifier("START_FWD_TC_REF");
		deviceField20.setShowState(false);
		deviceField15.setDisplayEvaluated(false);
		deviceField15.setShowState(true);
		deviceField15.setIdentifier("START_INV_TC");
		deviceField15.setNumCols(5);
		deviceField15.setLabelString("T_START_INV");
		deviceField15.setOffsetNid(34);
		deviceField9.setOffsetNid(41);
		deviceField9.setLabelString("T_START_REF_INV");
		deviceField9.setNumCols(5);
		deviceField9.setIdentifier("START_INV_TC_REF");
		deviceField9.setShowState(false);
		jPanel18.setBorder(titledBorder10);
		jPanel18.setDebugGraphicsOptions(0);
		jPanel18.setLayout(gridLayout16);
		deviceField16.setShowState(false);
		deviceField16.setIdentifier("STOP_INV_TC");
		deviceField16.setNumCols(5);
		deviceField16.setLabelString("T_STOP_INV");
		deviceField16.setOffsetNid(35);
		deviceField16.setDisplayEvaluated(false);
		deviceField21.setOffsetNid(44);
		deviceField21.setLabelString("T_STOP_FEEDF_INV");
		deviceField21.setNumCols(5);
		deviceField21.setIdentifier("STOP_FWD_TC_REF");
		deviceField21.setShowState(false);
		deviceField2.setOffsetNid(36);
		deviceField2.setLabelString("T_START_FORCED");
		deviceField2.setNumCols(5);
		deviceField2.setIdentifier("START_PWM_TC");
		deviceField2.setShowState(true);
		deviceField1113.setOffsetNid(40);
		deviceField1113.setLabelString("T_CROWBAR_ON");
		deviceField1113.setNumCols(5);
		deviceField1113.setIdentifier("CROWBAR_ON_TC");
		deviceField1113.setShowState(true);
		deviceField1113.setDisplayEvaluated(false);
		deviceField1113.setPreferredWidth(-1);
		jPanel19.setBorder(titledBorder11);
		jPanel19.setLayout(gridLayout17);
		gridLayout17.setColumns(1);
		jPanel1.setLayout(borderLayout5);
		jPanel4.setLayout(borderLayout7);
		jPanel3.setLayout(borderLayout8);
		deviceField1116.setOffsetNid(25);
		deviceField1116.setLabelString("T_START_PM");
		deviceField1116.setNumCols(5);
		deviceField1116.setIdentifier("START_PM");
		deviceField1116.setShowState(true);
		deviceField1116.setDisplayEvaluated(false);
		deviceField1116.setPreferredWidth(-1);
		deviceField5.setOffsetNid(26);
		deviceField5.setLabelString("T_STOP_PM");
		deviceField5.setNumCols(5);
		deviceField5.setIdentifier("STOP_PM");
		deviceField5.setShowState(true);
		deviceField119.setPreferredWidth(-1);
		deviceField119.setDisplayEvaluated(false);
		deviceField119.setShowState(true);
		deviceField119.setIdentifier("CLOSE_PNSS");
		deviceField119.setNumCols(5);
		deviceField119.setLabelString("T_CLOSE_PNSS");
		deviceField119.setOffsetNid(1);
		jPanel11.setFont(new java.awt.Font("Dialog", 1, 20));
		jPanel11.setBorder(titledBorder18);
		jPanel11.setLayout(gridLayout7);
		gridLayout7.setColumns(1);
		gridLayout7.setRows(3);
		gridLayout7.setVgap(0);
		deviceField112.setShowState(true);
		deviceField112.setIdentifier("START_PC");
		deviceField112.setNumCols(5);
		deviceField112.setLabelString("T_START_PC");
		deviceField112.setOffsetNid(29);
		deviceField112.setDisplayEvaluated(false);
		gridLayout9.setColumns(1);
		gridLayout9.setRows(4);
		deviceField1115.setOffsetNid(30);
		deviceField1115.setLabelString("T_STOP_PC");
		deviceField1115.setNumCols(5);
		deviceField1115.setIdentifier("STOP_PC");
		deviceField1115.setShowState(true);
		deviceField1115.setDisplayEvaluated(false);
		deviceField1115.setPreferredWidth(-1);
		jPanel13.setBorder(titledBorder5);
		jPanel13.setLayout(gridLayout9);
		deviceField1118.setOffsetNid(14);
		deviceField1118.setLabelString("T_INSRT_PC");
		deviceField1118.setNumCols(5);
		deviceField1118.setIdentifier("INSRT_PC");
		deviceField1118.setShowState(true);
		deviceField1118.setDisplayEvaluated(false);
		deviceField1118.setPreferredWidth(-1);
		jPanel20.setBorder(titledBorder12);
		jPanel20.setLayout(gridLayout3);
		gridLayout3.setColumns(1);
		gridLayout3.setRows(4);
		deviceField116.setShowState(true);
		deviceField116.setIdentifier("CLOSE_PP4");
		deviceField116.setNumCols(5);
		deviceField116.setLabelString("T_CLOSE_PP4");
		deviceField116.setOffsetNid(5);
		deviceField116.setDisplayEvaluated(false);
		deviceField12.setOffsetNid(4);
		deviceField12.setLabelString("T_CLOSE_PP3");
		deviceField12.setNumCols(5);
		deviceField12.setIdentifier("CLOSE_PP3");
		deviceField12.setShowState(true);
		deviceField11113.setOffsetNid(3);
		deviceField11113.setLabelString("T_CLOSE_PP2");
		deviceField11113.setNumCols(5);
		deviceField11113.setIdentifier("CLOSE_PP2");
		deviceField11113.setShowState(true);
		deviceField11113.setDisplayEvaluated(false);
		deviceField11113.setPreferredWidth(-1);
		deviceField117.setShowState(true);
		deviceField117.setIdentifier("CLOSE_PP1");
		deviceField117.setNumCols(5);
		deviceField117.setLabelString("T_CLOSE_PP1");
		deviceField117.setOffsetNid(2);
		deviceField117.setDisplayEvaluated(false);
		deviceField111.setShowState(true);
		deviceField111.setIdentifier("CLOSE_PTCT_8");
		deviceField111.setNumCols(5);
		deviceField111.setLabelString("T_CLOSE_PTCT_4B");
		deviceField111.setOffsetNid(13);
		deviceField111.setDisplayEvaluated(false);
		deviceField8.setOffsetNid(12);
		deviceField8.setLabelString("T_CLOSE_PTCT_4A");
		deviceField8.setNumCols(5);
		deviceField8.setIdentifier("CLOSE_PTCT_7");
		deviceField8.setShowState(true);
		deviceField1119.setOffsetNid(11);
		deviceField1119.setLabelString("T_CLOSE_PTCT_3B");
		deviceField1119.setNumCols(5);
		deviceField1119.setIdentifier("CLOSE_PTCT_6");
		deviceField1119.setShowState(true);
		deviceField1119.setDisplayEvaluated(false);
		deviceField1119.setPreferredWidth(-1);
		deviceField113.setShowState(true);
		deviceField113.setIdentifier("CLOSE_PTCT_5");
		deviceField113.setNumCols(5);
		deviceField113.setLabelString("T_CLOSE_PTCT_3A");
		deviceField113.setOffsetNid(10);
		deviceField113.setDisplayEvaluated(false);
		deviceField10.setOffsetNid(9);
		deviceField10.setLabelString("T_CLOSE_PTCT_2B");
		deviceField10.setNumCols(5);
		deviceField10.setIdentifier("CLOSE_PTCT_4");
		deviceField10.setShowState(true);
		deviceField10.setPreferredWidth(-1);
		deviceField11110.setOffsetNid(8);
		deviceField11110.setLabelString("T_CLOSE_PTCT_2A");
		deviceField11110.setNumCols(5);
		deviceField11110.setIdentifier("CLOSE_PTCT_3");
		deviceField11110.setShowState(true);
		deviceField11110.setDisplayEvaluated(false);
		deviceField11110.setPreferredWidth(-1);
		deviceField114.setShowState(true);
		deviceField114.setIdentifier("CLOSE_PTCT_2");
		deviceField114.setNumCols(5);
		deviceField114.setLabelString("T_CLOSE_PTCT_1B");
		deviceField114.setOffsetNid(7);
		deviceField114.setDisplayEvaluated(false);
		deviceField11.setOffsetNid(6);
		deviceField11.setLabelString("T_CLOSE_PTCT_1A");
		deviceField11.setNumCols(5);
		deviceField11.setIdentifier("CLOSE_PTCT_1");
		deviceField11.setShowState(true);
		deviceField11.setPreferredWidth(-1);
		jPanel21.setBorder(titledBorder15);
		jPanel21.setLayout(gridLayout6);
		gridLayout6.setColumns(1);
		gridLayout6.setRows(8);
		jPanel9.setBorder(null);
		deviceField24.setOffsetNid(46);
		deviceField24.setLabelString("T_STOP_PR");
		deviceField24.setNumCols(5);
		deviceField24.setIdentifier("STOP_PR");
		deviceField24.setShowState(false);
		gridLayout10.setColumns(1);
		gridLayout10.setRows(2);
		jPanel14.setBorder(titledBorder4);
		jPanel14.setLayout(gridLayout10);
		deviceField23.setOffsetNid(45);
		deviceField23.setLabelString("T_START_PR");
		deviceField23.setNumCols(5);
		deviceField23.setIdentifier("START_PR");
		deviceField23.setShowState(true);
		jPanel12.setBorder(titledBorder2);
		jPanel12.setLayout(gridLayout8);
		deviceField17.setShowState(true);
		deviceField17.setIdentifier("STOP_PV");
		deviceField17.setNumCols(5);
		deviceField17.setLabelString("T_STOP_PV");
		deviceField17.setOffsetNid(28);
		deviceField17.setDisplayEvaluated(false);
		gridLayout8.setColumns(1);
		gridLayout8.setRows(2);
		deviceField4.setOffsetNid(27);
		deviceField4.setLabelString("START_PV");
		deviceField4.setNumCols(5);
		deviceField4.setIdentifier("START_PV");
		deviceField4.setShowState(true);
		deviceField11116.setShowState(false);
		deviceField11116.setEditable(false);
		deviceField11116.setIdentifier("START_RFX");
		deviceField11116.setNumCols(5);
		deviceField11116.setLabelString("T_START_RFX");
		deviceField11116.setOffsetNid(31);
		deviceField11116.setDisplayEvaluated(false);
		deviceField11117.setDisplayEvaluated(false);
		deviceField11117.setOffsetNid(49);
		deviceField11117.setLabelString("T_END_RFX");
		deviceField11117.setNumCols(5);
		deviceField11117.setIdentifier("END_RFX");
		deviceField11117.setEditable(false);
		deviceField11117.setShowState(false);
		jPanel6.setLayout(gridLayout11);
		gridLayout11.setColumns(1);
		gridLayout11.setRows(2);
		deviceField22.setOffsetNid(39);
		deviceField22.setLabelString("T_OPEN_IS");
		deviceField22.setNumCols(5);
		deviceField22.setIdentifier("OPEN_IS_TC");
		deviceField22.setShowState(false);
		deviceField1114.setOffsetNid(38);
		deviceField1114.setLabelString("T_CLOSE_IS");
		deviceField1114.setNumCols(5);
		deviceField1114.setIdentifier("CLOSE_IS_TC");
		deviceField1114.setShowState(true);
		deviceField1114.setDisplayEvaluated(false);
		deviceField1114.setPreferredWidth(-1);
		jPanel6.setBorder(titledBorder19);
		deviceField110.setShowState(true);
		deviceField110.setEditable(false);
		deviceField110.setIdentifier("OPEN_PTSO_4");
		deviceField110.setNumCols(5);
		deviceField110.setLabelString("T_OPEN_PT_4");
		deviceField110.setOffsetNid(22);
		deviceField110.setDisplayEvaluated(false);
		deviceField11114.setDisplayEvaluated(false);
		deviceField11114.setOffsetNid(20);
		deviceField11114.setLabelString("T_OPEN_PT_2");
		deviceField11114.setNumCols(5);
		deviceField11114.setIdentifier("OPEN_PTSO_2");
		deviceField11114.setShowState(true);
		deviceField11114.setEditable(false);
		deviceField1112.setDisplayEvaluated(false);
		deviceField1112.setOffsetNid(21);
		deviceField1112.setLabelString("T_OPEN_PT_3");
		deviceField1112.setNumCols(5);
		deviceField1112.setIdentifier("OPEN_PTSO_3");
		deviceField1112.setShowState(true);
		deviceField1112.setEditable(false);
		gridLayout4.setColumns(1);
		gridLayout4.setRows(4);
		deviceField11115.setDisplayEvaluated(false);
		deviceField11115.setOffsetNid(19);
		deviceField11115.setLabelString("T_OPEN_PT_1");
		deviceField11115.setNumCols(5);
		deviceField11115.setIdentifier("OPEN_PTSO_1");
		deviceField11115.setShowState(true);
		deviceField11115.setEditable(false);
		jPanel23.setBorder(titledBorder13);
		jPanel23.setLayout(gridLayout4);
		deviceField7.setShowState(true);
		deviceField7.setOffsetNid(18);
		deviceField7.setLabelString("T_INSRT_PTCB_4");
		deviceField7.setNumCols(5);
		deviceField7.setIdentifier("INSRT_PTCB_4");
		deviceField25.setShowState(true);
		deviceField25.setIdentifier("INSRT_PTCB_3");
		deviceField25.setNumCols(5);
		deviceField25.setLabelString("T_INSRT_PTCB_3");
		deviceField25.setOffsetNid(17);
		jPanel22.setBorder(titledBorder14);
		jPanel22.setLayout(gridLayout5);
		gridLayout5.setColumns(1);
		gridLayout5.setRows(4);
		deviceField26.setShowState(true);
		deviceField26.setIdentifier("INSRT_PTCB_2");
		deviceField26.setNumCols(5);
		deviceField26.setLabelString("T_INSRT_PTCB_2");
		deviceField26.setOffsetNid(16);
		deviceField27.setShowState(true);
		deviceField27.setEditable(false);
		deviceField27.setIdentifier("INSRT_PTCB_1");
		deviceField27.setNumCols(5);
		deviceField27.setLabelString("T_INSRT_PTCB_1");
		deviceField27.setOffsetNid(15);
		jPanel25.setLayout(borderLayout3);
		deviceField28.setShowState(false);
		deviceField28.setIdentifier("STOP_GP");
		deviceField28.setNumCols(5);
		deviceField28.setLabelString("T_STOP_GP");
		deviceField28.setOffsetNid(48);
		jPanel24.setBorder(titledBorder16);
		jPanel24.setLayout(gridLayout2);
		deviceField29.setShowState(true);
		deviceField29.setIdentifier("START_GP");
		deviceField29.setNumCols(5);
		deviceField29.setLabelString("T_START_GP");
		deviceField29.setOffsetNid(47);
		gridLayout2.setColumns(1);
		gridLayout2.setRows(2);
		this.getContentPane().add(jPanel5, BorderLayout.CENTER);
		jPanel5.add(jPanel7, BorderLayout.CENTER);
		jPanel7.add(jPanel8, null);
		jPanel8.add(jPanel4, BorderLayout.CENTER);
		jPanel4.add(jPanel11, BorderLayout.NORTH);
		jPanel11.add(deviceField1116, null);
		jPanel11.add(deviceField5, null);
		jPanel11.add(deviceField119, null);
		jPanel4.add(jPanel13, BorderLayout.CENTER);
		jPanel13.add(deviceField112, null);
		jPanel13.add(deviceField1115, null);
		jPanel13.add(deviceField1118, null);
		jPanel4.add(jPanel12, BorderLayout.SOUTH);
		jPanel12.add(deviceField4, null);
		jPanel12.add(deviceField17, null);
		jPanel8.add(jPanel3, BorderLayout.SOUTH);
		jPanel3.add(jPanel20, BorderLayout.NORTH);
		jPanel20.add(deviceField117, null);
		jPanel20.add(deviceField11113, null);
		jPanel20.add(deviceField12, null);
		jPanel20.add(deviceField116, null);
		jPanel3.add(jPanel14, BorderLayout.CENTER);
		jPanel14.add(deviceField23, null);
		jPanel14.add(deviceField24, null);
		jPanel8.add(jPanel6, BorderLayout.NORTH);
		jPanel6.add(deviceField11116, null);
		jPanel6.add(deviceField11117, null);
		jPanel7.add(jPanel9, null);
		jPanel9.add(jPanel21, BorderLayout.CENTER);
		jPanel21.add(deviceField11, null);
		jPanel21.add(deviceField114, null);
		jPanel21.add(deviceField11110, null);
		jPanel21.add(deviceField10, null);
		jPanel21.add(deviceField113, null);
		jPanel21.add(deviceField1119, null);
		jPanel21.add(deviceField8, null);
		jPanel21.add(deviceField111, null);
		jPanel9.add(jPanel25, BorderLayout.NORTH);
		jPanel22.add(deviceField27, null);
		jPanel22.add(deviceField26, null);
		jPanel22.add(deviceField25, null);
		jPanel22.add(deviceField7, null);
		jPanel25.add(jPanel22, BorderLayout.SOUTH);
		jPanel25.add(jPanel23, BorderLayout.NORTH);
		jPanel23.add(deviceField11115, null);
		jPanel23.add(deviceField11114, null);
		jPanel23.add(deviceField1112, null);
		jPanel23.add(deviceField110, null);
		jPanel9.add(jPanel24, BorderLayout.SOUTH);
		jPanel24.add(deviceField29, null);
		jPanel24.add(deviceField28, null);
		jPanel7.add(jPanel10, null);
		jPanel10.add(jPanel2, BorderLayout.NORTH);
		jPanel2.add(jPanel15, BorderLayout.NORTH);
		jPanel15.add(deviceField6, null);
		jPanel15.add(deviceField19, null);
		jPanel2.add(jPanel16, BorderLayout.CENTER);
		jPanel16.add(deviceField1114, null);
		jPanel16.add(deviceField22, null);
		jPanel2.add(jPanel17, BorderLayout.SOUTH);
		jPanel17.add(deviceField14, null);
		jPanel17.add(deviceField1, null);
		jPanel10.add(jPanel1, BorderLayout.CENTER);
		jPanel1.add(jPanel19, BorderLayout.SOUTH);
		jPanel19.add(deviceField1113, null);
		jPanel1.add(jPanel18, BorderLayout.CENTER);
		jPanel18.add(deviceField15, null);
		jPanel18.add(deviceField16, null);
		jPanel18.add(deviceField2, null);
		jPanel18.add(deviceField3, null);
		jPanel18.add(deviceField9, null);
		jPanel18.add(deviceField18, null);
		jPanel18.add(deviceField20, null);
		jPanel18.add(deviceField21, null);
		deviceButtons1.setCheckExpressions(new String[]
		{ "if(_CLOSE_PNSS_state == 1) tcl(\'set node \\\\CLOSE_PNSS /on\'); else "
				+ "tcl(\'set node \\\\CLOSE_PNSS /off\'); 1;",
				"if(_START_PM_state == 1) tcl(\'set node \\\\START_PM/on\'); else tcl(\'set "
						+ "node \\\\START_PM /off\'); 1;",
				"if(_STOP_PM_state == 1) tcl(\'set node \\\\ENABLE_PM/on\'); else tcl(\'set "
						+ "node \\\\ENABLE_PM /off\'); 1;",
				"if(_START_PC_state == 1) tcl(\'set node \\\\START_PC/on\'); else tcl(\'set "
						+ "node \\\\START_PC /off\'); 1;",
				"if(_STOP_PC_state == 1) tcl(\'set node \\\\ENABLE_PC/on\'); else tcl(\'set "
						+ "node \\\\ENABLE_PC /off\'); 1;",
				"if(_INSRT_PC_state == 1) tcl(\'set node \\\\INSRT_PC/on\'); else tcl(\'set "
						+ "node \\\\INSRT_PC /off\'); 1;",
				"if(_START_PV_state == 1) tcl(\'set node \\\\START_PV/on\'); else tcl(\'set "
						+ "node \\\\START_PV/off\'); 1;",
				"if(_STOP_PV_state == 1) tcl(\'set node \\\\ENABLE_PV/on\'); else tcl(\'set "
						+ "node \\\\ENABLE_PV /off\'); 1;",
				"if(_CLOSE_PP1_state == 1) tcl(\'set node \\\\CLOSE_PP1/on\'); else tcl(\'set "
						+ "node \\\\CLOSE_PP1 /off\'); 1;",
				"if(_CLOSE_PP2_state == 1) tcl(\'set node \\\\CLOSE_PP2/on\'); else tcl(\'set "
						+ "node \\\\CLOSE_PP2 /off\'); 1;",
				"if(_CLOSE_PP3_state == 1) tcl(\'set node \\\\CLOSE_PP3/on\'); else tcl(\'set "
						+ "node \\\\CLOSE_PP3 /off\'); 1;",
				"if(_CLOSE_PP4_state == 1) tcl(\'set node \\\\CLOSE_PP4/on\'); else tcl(\'set "
						+ "node \\\\CLOSE_PP4 /off\'); 1;",
				"if(_START_PR_state == 1) tcl(\'set node \\\\ENABLE_PR/on\'); else tcl(\'set "
						+ "node \\\\ENABLE_PR/off\'); 1;",
				"if(_OPEN_PTSO_1_state == 1) tcl(\'set node \\\\OPEN_PTSO_1/on\'); else "
						+ "tcl(\'set node \\\\OPEN_PTSO_1 /off\'); 1;",
				"if(_OPEN_PTSO_2_state == 1) tcl(\'set node \\\\OPEN_PTSO_2/on\'); else "
						+ "tcl(\'set node \\\\OPEN_PTSO_2 /off\'); 1;",
				"if(_OPEN_PTSO_3_state == 1) tcl(\'set node \\\\OPEN_PTSO_3/on\'); else "
						+ "tcl(\'set node \\\\OPEN_PTSO_3 /off\'); 1;",
				"if(_OPEN_PTSO_4_state == 1) tcl(\'set node \\\\OPEN_PTSO_4/on\'); else "
						+ "tcl(\'set node \\\\OPEN_PTSO_4 /off\'); 1;",
				"if(_CLOSE_PTCT_1_state == 1) tcl(\'set node \\\\CLOSE_PTCT_1/on\'); else "
						+ "tcl(\'set node \\\\CLOSE_PTCT_1 /off\'); 1;",
				"if(_CLOSE_PTCT_2_state == 1) tcl(\'set node \\\\CLOSE_PTCT_2/on\'); else "
						+ "tcl(\'set node \\\\CLOSE_PTCT_2 /off\'); 1;",
				"if(_CLOSE_PTCT_3_state == 1) tcl(\'set node \\\\CLOSE_PTCT_3/on\'); else "
						+ "tcl(\'set node \\\\CLOSE_PTCT_3 /off\'); 1;",
				"if(_CLOSE_PTCT_4_state == 1) tcl(\'set node \\\\CLOSE_PTCT_4/on\'); else "
						+ "tcl(\'set node \\\\CLOSE_PTCT_4 /off\'); 1;",
				"if(_CLOSE_PTCT_5_state == 1) tcl(\'set node \\\\CLOSE_PTCT_5/on\'); else "
						+ "tcl(\'set node \\\\CLOSE_PTCT_5 /off\'); 1;",
				"if(_CLOSE_PTCT_6_state == 1) tcl(\'set node \\\\CLOSE_PTCT_6/on\'); else "
						+ "tcl(\'set node \\\\CLOSE_PTCT_6 /off\'); 1;",
				"if(_CLOSE_PTCT_7_state == 1) tcl(\'set node \\\\CLOSE_PTCT_7/on\'); else "
						+ "tcl(\'set node \\\\CLOSE_PTCT_7 /off\'); 1;",
				"if(_CLOSE_PTCT_8_state == 1) tcl(\'set node \\\\CLOSE_PTCT_8/on\'); else "
						+ "tcl(\'set node \\\\CLOSE_PTCT_8 /off\'); 1;",
				"if(_INSRT_PTCB_1_state == 1) tcl(\'set node \\\\INSRT_PTCB_1/on\'); else "
						+ "tcl(\'set node \\\\INSRT_PTCB_1 /off\'); 1;",
				"if(_INSRT_PTCB_2_state == 1) tcl(\'set node \\\\INSRT_PTCB_2/on\'); else "
						+ "tcl(\'set node \\\\INSRT_PTCB_2 /off\'); 1;",
				"if(_INSRT_PTCB_3_state == 1) tcl(\'set node \\\\INSRT_PTCB_3/on\'); else "
						+ "tcl(\'set node \\\\INSRT_PTCB_3 /off\'); 1;",
				"if(_INSRT_PTCB_4_state == 1) tcl(\'set node \\\\INSRT_PTCB_4/on\'); else "
						+ "tcl(\'set node \\\\INSRT_PTCB_4 /off\'); 1;",
				"if(_CROWBAR_ON_TC_state == 1) tcl(\'set node \\\\CROWBAR_ON_TC/on\'); "
						+ "else tcl(\'set node \\\\CROWBAR_ON_TC /off\'); 1;",
				"if(_START_INV_TC_state == 1) tcl(\'set node \\\\EN_INV_TC/on\'); else "
						+ "tcl(\'set node \\\\EN_INV_TC /off\'); 1;",
				"if(_START_PWM_TC_state == 1) tcl(\'set node \\\\FOR_PWM_TC/on\'); else "
						+ "tcl(\'set node \\\\FOR_PWM_TC /off\'); 1;",
				"if(_START_TF_state == 1) tcl(\'set node \\\\START_TF/on\'); else tcl(\'set "
						+ "node \\\\START_TF /off\'); 1;",
				"if(_STOP_TF_state == 1) tcl(\'set node \\\\ENABLE_TF/on\'); else tcl(\'set "
						+ "node \\\\ENABLE_TF /off\'); 1;",
				"if(_CLOSE_IS_TC_state == 1) tcl(\'set node \\\\CLOSE_IS_TC/on\'); else "
						+ "tcl(\'set node \\\\CLOSE_IS_TC /off\'); 1;",
				"if(_START_CHOP_TC_state == 1) tcl(\'set node \\\\EN_CHOP_TC/on\'); else "
						+ "tcl(\'set node \\\\EN_CHOP_TC /off\'); 1;",
				"(_STOP_PM - _START_PM)<= \\P_CONFIG:PM_WINDOW", "(_STOP_PV  - _START_PV)<= \\P_CONFIG:PV_WINDOW",
				"(_STOP_PC  - _START_PC)<= \\P_CONFIG:PC_WINDOW",
				"if(_START_GP_state == 1) tcl(\'set node \\\\START_GP/on\'); else tcl(\'set "
						+ "node \\\\START_GP /off\'); 1;" });
		deviceButtons1.setCheckMessages(new String[]
		{ "Item1", "Item 2", "Item 3", "Item 4", "Item 5", "Item 6", "Item 7", "Item 8", "Item 9", "Item 10", "Item 11",
				"Item 12", "Item 13", "Item 14", "Item 15", "Item 16", "Item 17", "Item 18", "Item 19", "Item 20",
				"Item 21", "Item 22", "Item 23", "Item 24", "Item 25", "Item 26", "Item 27", "Item 28", "Item 29",
				"Item 30", "Item 31", "Item 32", "Item 33", "Item 34", "Item 35", "Item 36",
				"PM window greater than plant configuration", "PV window greater than plant configuration",
				"PC window greater than plant configuration", "Item 40" });
		jPanel5.add(deviceButtons1, BorderLayout.SOUTH);
	}
}

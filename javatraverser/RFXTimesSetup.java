

import java.awt.*;
import javax.swing.*;
/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2003</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class RFXTimesSetup extends DeviceSetup {
  BorderLayout borderLayout1 = new BorderLayout();
  DeviceButtons deviceButtons1 = new DeviceButtons();
  JPanel jPanel1 = new JPanel();
  GridLayout gridLayout1 = new GridLayout();
  JPanel jPanel2 = new JPanel();
  JPanel jPanel3 = new JPanel();
  JPanel jPanel4 = new JPanel();
  DeviceField deviceField4 = new DeviceField();
  DeviceField deviceField5 = new DeviceField();
  DeviceField deviceField6 = new DeviceField();
  DeviceField deviceField7 = new DeviceField();
  DeviceField deviceField8 = new DeviceField();
  DeviceField deviceField10 = new DeviceField();
  DeviceField deviceField11 = new DeviceField();
  DeviceField deviceField12 = new DeviceField();
  DeviceField deviceField13 = new DeviceField();
  DeviceField deviceField14 = new DeviceField();
  DeviceField deviceField16 = new DeviceField();
  DeviceField deviceField17 = new DeviceField();
  DeviceField deviceField19 = new DeviceField();
  DeviceField deviceField110 = new DeviceField();
  DeviceField deviceField111 = new DeviceField();
  DeviceField deviceField112 = new DeviceField();
  DeviceField deviceField113 = new DeviceField();
  DeviceField deviceField114 = new DeviceField();
  DeviceField deviceField115 = new DeviceField();
  DeviceField deviceField116 = new DeviceField();
  DeviceField deviceField117 = new DeviceField();
  DeviceField deviceField15 = new DeviceField();
  DeviceField deviceField118 = new DeviceField();
  GridLayout gridLayout2 = new GridLayout();
  GridLayout gridLayout3 = new GridLayout();
  DeviceField deviceField119 = new DeviceField();
  DeviceField deviceField1110 = new DeviceField();
  GridLayout gridLayout4 = new GridLayout();
  DeviceField deviceField1111 = new DeviceField();
  DeviceField deviceField1113 = new DeviceField();
  DeviceField deviceField1114 = new DeviceField();
  DeviceField deviceField1115 = new DeviceField();
  DeviceField deviceField1116 = new DeviceField();
  DeviceField deviceField1117 = new DeviceField();
  DeviceField deviceField1118 = new DeviceField();
  DeviceField deviceField1119 = new DeviceField();
  DeviceField deviceField11110 = new DeviceField();
  DeviceField deviceField11111 = new DeviceField();
  DeviceField deviceField11112 = new DeviceField();
  DeviceField deviceField11113 = new DeviceField();
  DeviceField deviceField1 = new DeviceField();
  DeviceField deviceField2 = new DeviceField();
  DeviceField deviceField3 = new DeviceField();
  DeviceField deviceField9 = new DeviceField();
  DeviceField deviceField18 = new DeviceField();
  DeviceField deviceField20 = new DeviceField();
  DeviceField deviceField21 = new DeviceField();
  public RFXTimesSetup() {
    try {
      jbInit();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  private void jbInit() throws Exception {
    this.setWidth(830);
    this.setHeight(600);
    this.setDeviceType("RFXTimes");
    this.setDeviceProvider("localhost");
    this.setDeviceTitle("RFXTimesSetup");
    this.getContentPane().setLayout(borderLayout1);
    jPanel1.setLayout(gridLayout1);
    gridLayout1.setColumns(3);
    gridLayout1.setHgap(50);
    deviceField14.setOffsetNid(34);
    deviceField14.setLabelString("T_START_CHOP_TC");
    deviceField14.setNumCols(5);
    deviceField14.setIdentifier("START_CHOP_TC");
    deviceField14.setShowState(true);
    deviceField14.setDisplayEvaluated(false);
    deviceField14.setPreferredWidth(-1);
    deviceField13.setOffsetNid(20);
    deviceField13.setLabelString("T_CLOSE_TNST");
    deviceField13.setNumCols(5);
    deviceField13.setIdentifier("CLOSE_TNST");
    deviceField13.setShowState(true);
    deviceField13.setPreferredWidth(-1);
    deviceField12.setOffsetNid(4);
    deviceField12.setLabelString("T_CLOSE_PP3");
    deviceField12.setNumCols(5);
    deviceField12.setIdentifier("CLOSE_PP3");
    deviceField12.setShowState(true);
    deviceField11.setOffsetNid(10);
    deviceField11.setLabelString("T_CLOSE_PTCT_1");
    deviceField11.setNumCols(5);
    deviceField11.setIdentifier("CLOSE_PTCT_1");
    deviceField11.setShowState(true);
    deviceField11.setPreferredWidth(-1);
    deviceField10.setOffsetNid(13);
    deviceField10.setLabelString("T_CLOSE_PTCT_4");
    deviceField10.setNumCols(5);
    deviceField10.setIdentifier("CLOSE_PTCT_4");
    deviceField10.setShowState(true);
    deviceField10.setPreferredWidth(-1);
    deviceField8.setOffsetNid(16);
    deviceField8.setLabelString("T_CLOSE_PTCT_7");
    deviceField8.setNumCols(5);
    deviceField8.setIdentifier("CLOSE_PTCT_7");
    deviceField8.setShowState(true);
    deviceField7.setOffsetNid(22);
    deviceField7.setLabelString("T_INSRT_PTCB");
    deviceField7.setNumCols(5);
    deviceField7.setIdentifier("INSRT_PTCB");
    deviceField7.setShowState(true);
    deviceField6.setOffsetNid(25);
    deviceField6.setLabelString("T_START_TF");
    deviceField6.setNumCols(5);
    deviceField6.setIdentifier("START_TF");
    deviceField6.setShowState(true);
    deviceField5.setOffsetNid(28);
    deviceField5.setLabelString("T_STOP_PM");
    deviceField5.setNumCols(5);
    deviceField5.setIdentifier("STOP_PM");
    deviceField5.setShowState(true);
    deviceField4.setOffsetNid(29);
    deviceField4.setLabelString("START_PV");
    deviceField4.setNumCols(5);
    deviceField4.setIdentifier("START_PV");
    deviceField4.setShowState(true);
    jPanel2.setLayout(gridLayout2);
    deviceField16.setShowState(true);
    deviceField16.setIdentifier("STOP_INV_TC");
    deviceField16.setNumCols(5);
    deviceField16.setLabelString("T_STOP_INV_TC");
    deviceField16.setOffsetNid(37);
    deviceField16.setDisplayEvaluated(false);
    deviceField17.setShowState(true);
    deviceField17.setIdentifier("STOP_PV");
    deviceField17.setNumCols(5);
    deviceField17.setLabelString("T_STOP_PV");
    deviceField17.setOffsetNid(30);
    deviceField17.setDisplayEvaluated(false);
    jPanel4.setLayout(gridLayout3);
    deviceField19.setShowState(true);
    deviceField19.setIdentifier("STOP_TF");
    deviceField19.setNumCols(5);
    deviceField19.setLabelString("T_STOP_TF");
    deviceField19.setOffsetNid(26);
    deviceField19.setDisplayEvaluated(false);
    deviceField110.setShowState(true);
    deviceField110.setIdentifier("OPEN_PTSO");
    deviceField110.setNumCols(5);
    deviceField110.setLabelString("T_OPEN_PTSO");
    deviceField110.setOffsetNid(23);
    deviceField110.setDisplayEvaluated(false);
    deviceField111.setShowState(true);
    deviceField111.setIdentifier("CLOSE_PTCT_8");
    deviceField111.setNumCols(5);
    deviceField111.setLabelString("T_CLOSE_PTCT_8");
    deviceField111.setOffsetNid(17);
    deviceField111.setDisplayEvaluated(false);
    deviceField112.setShowState(true);
    deviceField112.setIdentifier("START_PC");
    deviceField112.setNumCols(5);
    deviceField112.setLabelString("T_START_PC");
    deviceField112.setOffsetNid(31);
    deviceField112.setDisplayEvaluated(false);
    deviceField113.setShowState(true);
    deviceField113.setIdentifier("CLOSE_PTCT_5");
    deviceField113.setNumCols(5);
    deviceField113.setLabelString("T_CLOSE_PTCT_5");
    deviceField113.setOffsetNid(14);
    deviceField113.setDisplayEvaluated(false);
    deviceField114.setShowState(true);
    deviceField114.setIdentifier("CLOSE_PTCT_2");
    deviceField114.setNumCols(5);
    deviceField114.setLabelString("T_CLOSE_PTCT_2");
    deviceField114.setOffsetNid(11);
    deviceField114.setDisplayEvaluated(false);
    deviceField115.setShowState(true);
    deviceField115.setIdentifier("CLOSE_PPSC3");
    deviceField115.setNumCols(5);
    deviceField115.setLabelString("T_CLOSE_PPSC3");
    deviceField115.setOffsetNid(8);
    deviceField115.setDisplayEvaluated(false);
    deviceField116.setShowState(true);
    deviceField116.setIdentifier("CLOSE_PP4");
    deviceField116.setNumCols(5);
    deviceField116.setLabelString("T_CLOSE_PP4");
    deviceField116.setOffsetNid(5);
    deviceField116.setDisplayEvaluated(false);
    deviceField117.setShowState(true);
    deviceField117.setIdentifier("CLOSE_PP1");
    deviceField117.setNumCols(5);
    deviceField117.setLabelString("T_CLOSE_PP1");
    deviceField117.setOffsetNid(2);
    deviceField117.setDisplayEvaluated(false);
    deviceField15.setDisplayEvaluated(false);
    deviceField15.setShowState(true);
    deviceField15.setIdentifier("START_INV_TC");
    deviceField15.setNumCols(5);
    deviceField15.setLabelString("T_START_INV_TC");
    deviceField15.setOffsetNid(36);
    deviceField118.setDisplayEvaluated(false);
    deviceField118.setShowState(true);
    deviceField118.setIdentifier("CLOSE_PPSC2");
    deviceField118.setNumCols(5);
    deviceField118.setLabelString("T_CLOSE_PPSC2");
    deviceField118.setOffsetNid(7);
    gridLayout2.setColumns(1);
    gridLayout2.setRows(15);
    gridLayout2.setVgap(-5);
    gridLayout3.setColumns(1);
    gridLayout3.setRows(15);
    gridLayout3.setVgap(-5);
    deviceField119.setPreferredWidth(-1);
    deviceField119.setDisplayEvaluated(false);
    deviceField119.setShowState(true);
    deviceField119.setIdentifier("CLOSE_PNSS");
    deviceField119.setNumCols(5);
    deviceField119.setLabelString("T_CLOSE_PNSS");
    deviceField119.setOffsetNid(1);
    deviceField1110.setPreferredWidth(-1);
    deviceField1110.setDisplayEvaluated(false);
    deviceField1110.setShowState(true);
    deviceField1110.setIdentifier("CLOSE_TNSI");
    deviceField1110.setNumCols(5);
    deviceField1110.setLabelString("T_CLOSE_TNSI");
    deviceField1110.setOffsetNid(18);
    jPanel3.setLayout(gridLayout4);
    gridLayout4.setColumns(1);
    gridLayout4.setHgap(0);
    gridLayout4.setRows(15);
    gridLayout4.setVgap(-5);
    deviceField1111.setOffsetNid(19);
    deviceField1111.setLabelString("T_CLOSE_TNSR");
    deviceField1111.setNumCols(5);
    deviceField1111.setIdentifier("CLOSE_TNSR");
    deviceField1111.setShowState(true);
    deviceField1111.setDisplayEvaluated(false);
    deviceField1111.setPreferredWidth(-1);
    deviceField1113.setOffsetNid(41);
    deviceField1113.setLabelString("T_CROWBAR_ON_TC");
    deviceField1113.setNumCols(5);
    deviceField1113.setIdentifier("CROWBAR_ON_TC");
    deviceField1113.setShowState(true);
    deviceField1113.setDisplayEvaluated(false);
    deviceField1113.setPreferredWidth(-1);
    deviceField1114.setOffsetNid(40);
    deviceField1114.setLabelString("T_CLOSE_IS_TC");
    deviceField1114.setNumCols(5);
    deviceField1114.setIdentifier("CLOSE_IS_TC");
    deviceField1114.setShowState(true);
    deviceField1114.setDisplayEvaluated(false);
    deviceField1114.setPreferredWidth(-1);
    deviceField1115.setOffsetNid(32);
    deviceField1115.setLabelString("T_STOP_PC");
    deviceField1115.setNumCols(5);
    deviceField1115.setIdentifier("STOP_PC");
    deviceField1115.setShowState(true);
    deviceField1115.setDisplayEvaluated(false);
    deviceField1115.setPreferredWidth(-1);
    deviceField1116.setOffsetNid(27);
    deviceField1116.setLabelString("T_START_PM");
    deviceField1116.setNumCols(5);
    deviceField1116.setIdentifier("START_PM");
    deviceField1116.setShowState(true);
    deviceField1116.setDisplayEvaluated(false);
    deviceField1116.setPreferredWidth(-1);
    deviceField1117.setOffsetNid(24);
    deviceField1117.setLabelString("T_PPSC_C");
    deviceField1117.setNumCols(5);
    deviceField1117.setIdentifier("PPSC_C");
    deviceField1117.setShowState(true);
    deviceField1117.setDisplayEvaluated(false);
    deviceField1117.setPreferredWidth(-1);
    deviceField1118.setOffsetNid(21);
    deviceField1118.setLabelString("T_INSRT_PC");
    deviceField1118.setNumCols(5);
    deviceField1118.setIdentifier("INSRT_PC");
    deviceField1118.setShowState(true);
    deviceField1118.setDisplayEvaluated(false);
    deviceField1118.setPreferredWidth(-1);
    deviceField1119.setOffsetNid(15);
    deviceField1119.setLabelString("T_CLOSE_PTCT_6");
    deviceField1119.setNumCols(5);
    deviceField1119.setIdentifier("CLOSE_PTCT_6");
    deviceField1119.setShowState(true);
    deviceField1119.setDisplayEvaluated(false);
    deviceField1119.setPreferredWidth(-1);
    deviceField11110.setOffsetNid(12);
    deviceField11110.setLabelString("T_CLOSE_PTCT_3");
    deviceField11110.setNumCols(5);
    deviceField11110.setIdentifier("CLOSE_PTCT_3");
    deviceField11110.setShowState(true);
    deviceField11110.setDisplayEvaluated(false);
    deviceField11110.setPreferredWidth(-1);
    deviceField11111.setOffsetNid(9);
    deviceField11111.setLabelString("T_CLOSE_PPSC4");
    deviceField11111.setNumCols(5);
    deviceField11111.setIdentifier("CLOSE_PPSC4");
    deviceField11111.setShowState(true);
    deviceField11111.setDisplayEvaluated(false);
    deviceField11111.setPreferredWidth(-1);
    deviceField11112.setOffsetNid(6);
    deviceField11112.setLabelString("T_CLOSE_PPSC1");
    deviceField11112.setNumCols(5);
    deviceField11112.setIdentifier("CLOSE_PPSC1");
    deviceField11112.setShowState(true);
    deviceField11112.setDisplayEvaluated(false);
    deviceField11112.setPreferredWidth(-1);
    deviceField11113.setOffsetNid(3);
    deviceField11113.setLabelString("T_CLOSE_PP2");
    deviceField11113.setNumCols(5);
    deviceField11113.setIdentifier("CLOSE_PP2");
    deviceField11113.setShowState(true);
    deviceField11113.setDisplayEvaluated(false);
    deviceField11113.setPreferredWidth(-1);
    deviceButtons1.setCheckExpressions(new String[] {"if(_CLOSE_PNSS_state == 1) tcl(\'set node \\\\CLOSE_PNSS /on\'); else " +
    "tcl(\'set node \\\\CLOSE_PNSS /off\'); 1;",
        "if(_CLOSE_TNST_state == 1) tcl(\'set node \\\\CLOSE_TNST/on\'); else " +
    "tcl(\'set node \\\\CLOSE_TNST /off\'); 1;",
        "if(_CLOSE_PP3_state == 1) tcl(\'set node \\\\CLOSE_PP3/on\'); else tcl(\'set " +
    "node \\\\CLOSE_PP3 /off\'); 1;",
        "if(_CLOSE_PPSC2_state == 1) tcl(\'set node \\\\CLOSE_PPSC2/on\'); else " +
    "tcl(\'set node \\\\CLOSE_PPSC2 /off\'); 1;",
        "if(_CLOSE_PTCT_1_state == 1) tcl(\'set node \\\\CLOSE_PTCT_1/on\'); else " +
    "tcl(\'set node \\\\CLOSE_PTCT_1 /off\'); 1;",
        "if(_CLOSE_PTCT_4_state == 1) tcl(\'set node \\\\CLOSE_PTCT_4/on\'); else " +
    "tcl(\'set node \\\\CLOSE_PTCT_4 /off\'); 1;",
        "if(_CLOSE_PTCT_7_state == 1) tcl(\'set node \\\\CLOSE_PTCT_7/on\'); else " +
    "tcl(\'set node \\\\CLOSE_PTCT_7 /off\'); 1;",
        "if(_INSRT_PTCB_state == 1) tcl(\'set node \\\\INSRT_PTCB/on\'); else " +
    "tcl(\'set node \\\\INSRT_PTCB /off\'); 1;",
        "if(_START_TF_state == 1) tcl(\'set node \\\\START_TF/on\'); else tcl(\'set " +
    "node \\\\START_TF /off\'); 1;",
        "if(_STOP_PM_state == 1) tcl(\'set node \\\\ENABLE_PM/on\'); else tcl(\'set " +
    "node \\\\ENABLE_PM /off\'); 1;",
        "if(_START_PV_state == 1) tcl(\'set node \\\\START_PV/on\'); else tcl(\'set " +
    "node \\\\START_PV/off\'); 1;",
        "if(_EN_INV_TC_state == 1) tcl(\'set node \\\\EN_INV_TC/on\'); else tcl(\'set " +
    "node \\\\EN_INV_TC /off\'); 1;",
        "if(_CROWBAR_ON_TC_state == 1) tcl(\'set node \\\\CROWBAR_ON_TC/on\'); " +
    "else tcl(\'set node \\\\CROWBAR_ON_TC /off\'); 1;",
        "if(_CLOSE_TNSI_state == 1) tcl(\'set node \\\\CLOSE_TNSI/on\'); else " +
    "tcl(\'set node \\\\CLOSE_TNSI /off\'); 1;",
        "if(_CLOSE_PP1_state == 1) tcl(\'set node \\\\CLOSE_PP1/on\'); else tcl(\'set " +
    "node \\\\CLOSE_PP1 /off\'); 1;",
        "if(_CLOSE_PP4_state == 1) tcl(\'set node \\\\CLOSE_PP4/on\'); else tcl(\'set " +
    "node \\\\CLOSE_PP4 /off\'); 1;",
        "if(_CLOSE_PPSC3_state == 1) tcl(\'set node \\\\CLOSE_PPSC3/on\'); else " +
    "tcl(\'set node \\\\CLOSE_PPSC3 /off\'); 1;",
        "if(_CLOSE_PTCT_2_state == 1) tcl(\'set node \\\\CLOSE_PTCT_2/on\'); else " +
    "tcl(\'set node \\\\CLOSE_PTCT_2 /off\'); 1;",
        "if(_CLOSE_PTCT_5_state == 1) tcl(\'set node \\\\CLOSE_PTCT_5/on\'); else " +
    "tcl(\'set node \\\\CLOSE_PTCT_5 /off\'); 1;",
        "if(_CLOSE_PTCT_8_state == 1) tcl(\'set node \\\\CLOSE_PTCT_8/on\'); else " +
    "tcl(\'set node \\\\CLOSE_PTCT_8 /off\'); 1;",
        "if(_OPEN_PTSO_state == 1) tcl(\'set node \\\\OPEN_PTSO/on\'); else tcl(\'set " +
    "node \\\\OPEN_PTSO /off\'); 1;",
        "if(_STOP_TF_state == 1) tcl(\'set node \\\\ENABLE_TF/on\'); else tcl(\'set " +
    "node \\\\ENABLE_TF /off\'); 1;",
        "if(_START_PC_state == 1) tcl(\'set node \\\\START_PC/on\'); else tcl(\'set " +
    "node \\\\START_PC /off\'); 1;",
        "if(_STOP_PV_state == 1) tcl(\'set node \\\\ENABLE_PV/on\'); else tcl(\'set " +
    "node \\\\ENABLE_PV /off\'); 1;",
        "if(_FOR_PWM_TC_state == 1) tcl(\'set node \\\\FOR_PWM_TC/on\'); else " +
    "tcl(\'set node \\\\FOR_PWM_TC /off\'); 1;",
        "if(_CLOSE_TNSR_state == 1) tcl(\'set node \\\\CLOSE_TNSR/on\'); else " +
    "tcl(\'set node \\\\CLOSE_TNSR /off\'); 1;",
        "if(_CLOSE_PP2_state == 1) tcl(\'set node \\\\CLOSE_PP2/on\'); else tcl(\'set " +
    "node \\\\CLOSE_PP2 /off\'); 1;",
        "if(_CLOSE_PPSC1_state == 1) tcl(\'set node \\\\CLOSE_PPSC1/on\'); else " +
    "tcl(\'set node \\\\CLOSE_PPSC1 /off\'); 1;",
        "if(_CLOSE_PTCT_3_state == 1) tcl(\'set node \\\\CLOSE_PTCT_3/on\'); else " +
    "tcl(\'set node \\\\CLOSE_PTCT_3 /off\'); 1;",
        "if(_CLOSE_PTCT_6_state == 1) tcl(\'set node \\\\CLOSE_PTCT_6/on\'); else " +
    "tcl(\'set node \\\\CLOSE_PTCT_6 /off\'); 1;",
        "if(_INSRT_PC_state == 1) tcl(\'set node \\\\INSRT_PC/on\'); else tcl(\'set " +
    "node \\\\INSRT_PC /off\'); 1;",
        "if(_PPSC_C_state == 1) tcl(\'set node \\\\PPSC_C/on\'); else tcl(\'set " +
    "node \\\\PPSC_C /off\'); 1;", "if(_START_PM_state == 1) tcl(\'set node \\\\START_PM/on\'); else tcl(\'set " +
    "node \\\\START_PM /off\'); 1;",
        "if(_STOP_PC_state == 1) tcl(\'set node \\\\ENABLE_PC/on\'); else tcl(\'set " +
    "node \\\\ENABLE_PC /off\'); 1;",
        "if(_START_CHOP_TC_state == 1) tcl(\'set node \\\\EN_CHOP_TC/on\'); else " +
    "tcl(\'set node \\\\EN_CHOP_TC /off\'); 1;",
        "if(_CLOSE_IS_TC_state == 1) tcl(\'set node \\\\CLOSE_IS_TC/on\'); else " +
    "tcl(\'set node \\\\CLOSE_IS_TC /off\'); 1;",
        "if(_START_INV_TC_state == 1) tcl(\'set node \\\\EN_INV_TC/on\'); else " +
    "tcl(\'set node \\\\EN_INV_TC /off\'); 1;",
        "if(_START_PWM_TC_state == 1) tcl(\'set node \\\\FOR_PWM_TC/on\'); else " +
    "tcl(\'set node \\\\FOR_PWM_TC /off\'); 1;"
        });
    deviceButtons1.setCheckMessages(new String[] {"None1<NL>None2<NL>None3<NL>None4<NL>None5<NL>None6<NL>None7<NL>None8<NL>None9<NL>None0<NL>None11<NL>N" +
    "one12<NL>None13<NL>None14<NL>None15<NL>None16<NL>None17<NL>None18<NL>None19<NL>None20<NL>None21<NL>No" +
    "ne22<NL>None23<NL>None24<NL>None25<NL>None26<NL>None27<NL>None28<NL>None29<NL>None30<NL>None31<NL>Non" +
    "e32<NL>None33<NL>None34<NL>None35<NL>None36<NL>None37<NL>"
        });
    deviceField1.setOffsetNid(35);
    deviceField1.setLabelString("T_STOP_CHOP_TC");
    deviceField1.setNumCols(5);
    deviceField1.setIdentifier("STOP_CHOP_TC");
    deviceField1.setShowState(true);
    deviceField2.setOffsetNid(38);
    deviceField2.setLabelString("T_START_PWM_TC");
    deviceField2.setNumCols(5);
    deviceField2.setIdentifier("START_PWM_TC");
    deviceField2.setShowState(true);
    deviceField3.setOffsetNid(39);
    deviceField3.setLabelString("T_STOP_PWM_TC");
    deviceField3.setNumCols(5);
    deviceField3.setIdentifier("STOP_PWM_TC");
    deviceField3.setShowState(true);
    deviceField9.setOffsetNid(42);
    deviceField9.setLabelString("T_START_INV_TC_REF");
    deviceField9.setNumCols(5);
    deviceField9.setIdentifier("START_INV_TC_REF");
    deviceField9.setShowState(true);
    deviceField18.setOffsetNid(43);
    deviceField18.setLabelString("T_STOP_INV_TC_REF");
    deviceField18.setNumCols(5);
    deviceField18.setIdentifier("STOP_INV_TC_REF");
    deviceField18.setShowState(true);
    deviceField20.setOffsetNid(44);
    deviceField20.setLabelString("T_START_PWM_TC_REF");
    deviceField20.setNumCols(5);
    deviceField20.setIdentifier("START_FWD_TC_REF");
    deviceField20.setShowState(true);
    deviceField21.setOffsetNid(45);
    deviceField21.setLabelString("T_STOP_PWM_TC_REF");
    deviceField21.setNumCols(5);
    deviceField21.setIdentifier("STOP_FWD_TC_REF");
    deviceField21.setShowState(true);
    this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
    this.getContentPane().add(jPanel1, BorderLayout.CENTER);
    jPanel1.add(jPanel2, null);
    jPanel2.add(deviceField119, null);
    jPanel2.add(deviceField13, null);
    jPanel2.add(deviceField12, null);
    jPanel2.add(deviceField118, null);
    jPanel2.add(deviceField11, null);
    jPanel2.add(deviceField10, null);
    jPanel2.add(deviceField8, null);
    jPanel2.add(deviceField7, null);
    jPanel2.add(deviceField6, null);
    jPanel2.add(deviceField5, null);
    jPanel2.add(deviceField4, null);
    jPanel1.add(jPanel4, null);
    jPanel1.add(jPanel3, null);
    jPanel3.add(deviceField1111, null);
    jPanel3.add(deviceField11113, null);
    jPanel3.add(deviceField11112, null);
    jPanel3.add(deviceField11111, null);
    jPanel3.add(deviceField11110, null);
    jPanel3.add(deviceField1119, null);
    jPanel3.add(deviceField1118, null);
    jPanel3.add(deviceField1117, null);
    jPanel3.add(deviceField1116, null);
    jPanel3.add(deviceField1115, null);
    jPanel3.add(deviceField1114, null);
    jPanel3.add(deviceField1113, null);
    jPanel3.add(deviceField2, null);
    jPanel3.add(deviceField18, null);
    jPanel4.add(deviceField1110, null);
    jPanel4.add(deviceField117, null);
    jPanel4.add(deviceField116, null);
    jPanel2.add(deviceField15, null);
    jPanel2.add(deviceField14, null);
    jPanel2.add(deviceField3, null);
    jPanel2.add(deviceField20, null);
    jPanel4.add(deviceField115, null);
    jPanel4.add(deviceField114, null);
    jPanel4.add(deviceField113, null);
    jPanel4.add(deviceField111, null);
    jPanel4.add(deviceField110, null);
    jPanel4.add(deviceField19, null);
    jPanel4.add(deviceField112, null);
    jPanel4.add(deviceField17, null);
    jPanel4.add(deviceField16, null);
    jPanel4.add(deviceField1, null);
    jPanel4.add(deviceField9, null);
    jPanel4.add(deviceField21, null);
  }

}
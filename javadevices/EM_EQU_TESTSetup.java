/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


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

public class EM_EQU_TESTSetup extends DeviceSetup {
  BorderLayout borderLayout1 = new BorderLayout();
  JPanel jPanel2 = new JPanel();
  JPanel jPanel1 = new JPanel();
  FlowLayout flowLayout1 = new FlowLayout();
  DeviceField deviceField1 = new DeviceField();
  GridLayout gridLayout1 = new GridLayout();
  DeviceButtons deviceButtons1 = new DeviceButtons();
  JScrollPane jScrollPane1 = new JScrollPane();
  JPanel jPanel6 = new JPanel();
  GridLayout gridLayout2 = new GridLayout();
  DeviceChannel deviceChannel1 = new DeviceChannel();
  JPanel jPanel7 = new JPanel();
  FlowLayout flowLayout5 = new FlowLayout();
  DeviceField deviceField12 = new DeviceField();
  FlowLayout flowLayout6 = new FlowLayout();
  DeviceField deviceField13 = new DeviceField();
  DeviceField deviceField14 = new DeviceField();
  DeviceField deviceField15 = new DeviceField();
  JPanel jPanel8 = new JPanel();
  JLabel jLabel1 = new JLabel();
  DeviceField deviceField16 = new DeviceField();
  DeviceField deviceField17 = new DeviceField();
  FlowLayout flowLayout8 = new FlowLayout();
  DeviceChannel deviceChannel2 = new DeviceChannel();
  DeviceField deviceField18 = new DeviceField();
  DeviceField deviceField19 = new DeviceField();
  FlowLayout flowLayout9 = new FlowLayout();
  JPanel jPanel9 = new JPanel();
  DeviceField deviceField110 = new DeviceField();
  DeviceField deviceField111 = new DeviceField();
  FlowLayout flowLayout10 = new FlowLayout();
  DeviceChannel deviceChannel3 = new DeviceChannel();
  DeviceField deviceField112 = new DeviceField();
  DeviceField deviceField113 = new DeviceField();
  FlowLayout flowLayout11 = new FlowLayout();
  JPanel jPanel10 = new JPanel();
  DeviceField deviceField114 = new DeviceField();
  DeviceField deviceField115 = new DeviceField();
  FlowLayout flowLayout12 = new FlowLayout();
  DeviceChannel deviceChannel4 = new DeviceChannel();
  DeviceField deviceField116 = new DeviceField();
  DeviceField deviceField117 = new DeviceField();
  FlowLayout flowLayout13 = new FlowLayout();
  JPanel jPanel11 = new JPanel();
  DeviceField deviceField118 = new DeviceField();
  DeviceField deviceField119 = new DeviceField();
  FlowLayout flowLayout14 = new FlowLayout();
  DeviceChannel deviceChannel5 = new DeviceChannel();
  DeviceField deviceField1110 = new DeviceField();
  DeviceField deviceField1111 = new DeviceField();
  FlowLayout flowLayout15 = new FlowLayout();
  JPanel jPanel12 = new JPanel();
  DeviceField deviceField1112 = new DeviceField();
  DeviceField deviceField1113 = new DeviceField();
  FlowLayout flowLayout16 = new FlowLayout();
  DeviceChannel deviceChannel6 = new DeviceChannel();
  DeviceField deviceField1114 = new DeviceField();
  DeviceField deviceField1115 = new DeviceField();
  FlowLayout flowLayout17 = new FlowLayout();
  JPanel jPanel13 = new JPanel();
  DeviceField deviceField1116 = new DeviceField();
  DeviceField deviceField1117 = new DeviceField();
  FlowLayout flowLayout18 = new FlowLayout();
  DeviceChannel deviceChannel7 = new DeviceChannel();
  DeviceField deviceField1118 = new DeviceField();
  DeviceField deviceField1119 = new DeviceField();
  FlowLayout flowLayout19 = new FlowLayout();
  JPanel jPanel14 = new JPanel();
  DeviceField deviceField11110 = new DeviceField();
  DeviceField deviceField11111 = new DeviceField();
  FlowLayout flowLayout110 = new FlowLayout();
  DeviceChannel deviceChannel8 = new DeviceChannel();
  DeviceField deviceField11112 = new DeviceField();
  DeviceField deviceField11113 = new DeviceField();
  FlowLayout flowLayout111 = new FlowLayout();
  JPanel jPanel15 = new JPanel();
  DeviceField deviceField11114 = new DeviceField();
  DeviceField deviceField11115 = new DeviceField();
  FlowLayout flowLayout112 = new FlowLayout();
  DeviceChannel deviceChannel9 = new DeviceChannel();
  DeviceField deviceField11116 = new DeviceField();
  DeviceField deviceField11117 = new DeviceField();
  FlowLayout flowLayout113 = new FlowLayout();
  JPanel jPanel16 = new JPanel();
  DeviceField deviceField11118 = new DeviceField();
  DeviceField deviceField11119 = new DeviceField();
  FlowLayout flowLayout114 = new FlowLayout();
  DeviceChannel deviceChannel10 = new DeviceChannel();
  DeviceField deviceField111110 = new DeviceField();
  DeviceField deviceField111111 = new DeviceField();
  FlowLayout flowLayout115 = new FlowLayout();
  JPanel jPanel17 = new JPanel();
  GridLayout gridLayout3 = new GridLayout();
  JPanel jPanel3 = new JPanel();
  DeviceField deviceField2 = new DeviceField();
  DeviceField deviceField3 = new DeviceField();
  DeviceField deviceField4 = new DeviceField();
  FlowLayout flowLayout2 = new FlowLayout();
  public EM_EQU_TESTSetup() {
    try {
      jbInit();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  private void jbInit() throws Exception {
    this.setWidth(700);
    this.setHeight(500);
    this.setDeviceType("EM_EQU_TEST");
    this.setDeviceProvider("150.178.3.33");
    this.setDeviceTitle("Equilibrium Acquisition Test");
    this.getContentPane().setLayout(borderLayout1);
    jPanel1.setLayout(flowLayout1);
    flowLayout1.setAlignment(FlowLayout.LEFT);
    flowLayout1.setHgap(0);
    flowLayout1.setVgap(0);
    deviceField1.setOffsetNid(1);
    deviceField1.setLabelString("Comment : ");
    deviceField1.setNumCols(40);
    deviceField1.setIdentifier("");
    jPanel2.setLayout(gridLayout1);
    gridLayout1.setRows(3);
    jPanel6.setLayout(gridLayout2);
    gridLayout2.setRows(10);
    gridLayout2.setVgap(5);
    deviceChannel1.setLabelString("01");
    deviceChannel1.setOffsetNid(5);
    deviceChannel1.setInSameLine(true);
    deviceChannel1.setUpdateIdentifier("");
    deviceChannel1.setShowVal("");
    deviceChannel1.setLayout(flowLayout5);
    flowLayout5.setAlignment(FlowLayout.RIGHT);
    flowLayout5.setHgap(0);
    flowLayout5.setVgap(3);
    jPanel7.setLayout(flowLayout6);
    flowLayout6.setAlignment(FlowLayout.LEFT);
    flowLayout6.setHgap(0);
    flowLayout6.setVgap(3);
    jPanel8.setLayout(gridLayout3);
    jLabel1.setPreferredSize(new Dimension(474, 15));
    jLabel1.setRequestFocusEnabled(true);
    jLabel1.setText("   CARD      F-END ID                  GAIN                     " +
    "                    Linear ADC                              Integral " +
    "ADC");
    deviceField12.setOffsetNid(6);
    deviceField12.setNumCols(5);
    deviceField12.setIdentifier("");
    deviceField13.setOffsetNid(7);
    deviceField13.setTextOnly(true);
    deviceField13.setNumCols(15);
    deviceField13.setIdentifier("");
    deviceField14.setOffsetNid(8);
    deviceField14.setTextOnly(true);
    deviceField14.setNumCols(15);
    deviceField14.setIdentifier("");
    deviceField15.setOffsetNid(9);
    deviceField15.setTextOnly(true);
    deviceField15.setNumCols(15);
    deviceField15.setIdentifier("");
    deviceField16.setIdentifier("");
    deviceField16.setOffsetNid(53);
    deviceField16.setTextOnly(true);
    deviceField16.setNumCols(15);
    deviceField17.setIdentifier("");
    deviceField17.setOffsetNid(52);
    deviceField17.setTextOnly(true);
    deviceField17.setNumCols(15);
    flowLayout8.setVgap(3);
    flowLayout8.setHgap(0);
    flowLayout8.setAlignment(FlowLayout.RIGHT);
    deviceChannel2.setLayout(flowLayout8);
    deviceChannel2.setShowVal("");
    deviceChannel2.setUpdateIdentifier("");
    deviceChannel2.setInSameLine(true);
    deviceChannel2.setOffsetNid(50);
    deviceChannel2.setLabelString("10");
    deviceField18.setOffsetNid(51);
    deviceField18.setNumCols(5);
    deviceField18.setIdentifier("");
    deviceField18.setPreferredWidth(-1);
    deviceField19.setOffsetNid(54);
    deviceField19.setTextOnly(true);
    deviceField19.setNumCols(15);
    deviceField19.setIdentifier("");
    flowLayout9.setAlignment(FlowLayout.LEFT);
    flowLayout9.setHgap(0);
    flowLayout9.setVgap(0);
    jPanel9.setLayout(flowLayout9);
    deviceField110.setIdentifier("");
    deviceField110.setOffsetNid(48);
    deviceField110.setTextOnly(true);
    deviceField110.setNumCols(15);
    deviceField111.setIdentifier("");
    deviceField111.setOffsetNid(47);
    deviceField111.setTextOnly(true);
    deviceField111.setNumCols(15);
    flowLayout10.setVgap(3);
    flowLayout10.setHgap(0);
    flowLayout10.setAlignment(FlowLayout.RIGHT);
    deviceChannel3.setLayout(flowLayout10);
    deviceChannel3.setShowVal("");
    deviceChannel3.setUpdateIdentifier("");
    deviceChannel3.setInSameLine(true);
    deviceChannel3.setOffsetNid(45);
    deviceChannel3.setLabelString("09");
    deviceField112.setOffsetNid(46);
    deviceField112.setNumCols(5);
    deviceField112.setIdentifier("");
    deviceField113.setOffsetNid(49);
    deviceField113.setTextOnly(true);
    deviceField113.setNumCols(15);
    deviceField113.setIdentifier("");
    flowLayout11.setAlignment(FlowLayout.LEFT);
    flowLayout11.setHgap(0);
    flowLayout11.setVgap(0);
    jPanel10.setLayout(flowLayout11);
    deviceField114.setIdentifier("");
    deviceField114.setOffsetNid(43);
    deviceField114.setTextOnly(true);
    deviceField114.setNumCols(15);
    deviceField115.setIdentifier("");
    deviceField115.setOffsetNid(42);
    deviceField115.setTextOnly(true);
    deviceField115.setNumCols(15);
    flowLayout12.setVgap(3);
    flowLayout12.setHgap(0);
    flowLayout12.setAlignment(FlowLayout.RIGHT);
    deviceChannel4.setLayout(flowLayout12);
    deviceChannel4.setShowVal("");
    deviceChannel4.setUpdateIdentifier("");
    deviceChannel4.setInSameLine(true);
    deviceChannel4.setOffsetNid(40);
    deviceChannel4.setLabelString("08");
    deviceField116.setOffsetNid(41);
    deviceField116.setNumCols(5);
    deviceField116.setIdentifier("");
    deviceField117.setOffsetNid(44);
    deviceField117.setTextOnly(true);
    deviceField117.setNumCols(15);
    deviceField117.setIdentifier("");
    flowLayout13.setAlignment(FlowLayout.LEFT);
    flowLayout13.setHgap(0);
    flowLayout13.setVgap(0);
    jPanel11.setLayout(flowLayout13);
    deviceField118.setIdentifier("");
    deviceField118.setOffsetNid(38);
    deviceField118.setTextOnly(true);
    deviceField118.setNumCols(15);
    deviceField119.setIdentifier("");
    deviceField119.setOffsetNid(37);
    deviceField119.setTextOnly(true);
    deviceField119.setNumCols(15);
    flowLayout14.setVgap(3);
    flowLayout14.setHgap(0);
    flowLayout14.setAlignment(FlowLayout.RIGHT);
    deviceChannel5.setLayout(flowLayout14);
    deviceChannel5.setShowVal("");
    deviceChannel5.setUpdateIdentifier("");
    deviceChannel5.setInSameLine(true);
    deviceChannel5.setOffsetNid(35);
    deviceChannel5.setLabelString("07");
    deviceField1110.setOffsetNid(36);
    deviceField1110.setNumCols(5);
    deviceField1110.setIdentifier("");
    deviceField1111.setOffsetNid(39);
    deviceField1111.setTextOnly(true);
    deviceField1111.setNumCols(15);
    deviceField1111.setIdentifier("");
    flowLayout15.setAlignment(FlowLayout.LEFT);
    flowLayout15.setHgap(0);
    flowLayout15.setVgap(0);
    jPanel12.setLayout(flowLayout15);
    deviceField1112.setIdentifier("");
    deviceField1112.setOffsetNid(33);
    deviceField1112.setTextOnly(true);
    deviceField1112.setNumCols(15);
    deviceField1113.setIdentifier("");
    deviceField1113.setOffsetNid(32);
    deviceField1113.setTextOnly(true);
    deviceField1113.setNumCols(15);
    flowLayout16.setVgap(3);
    flowLayout16.setHgap(0);
    flowLayout16.setAlignment(FlowLayout.RIGHT);
    deviceChannel6.setLayout(flowLayout16);
    deviceChannel6.setShowVal("");
    deviceChannel6.setUpdateIdentifier("");
    deviceChannel6.setInSameLine(true);
    deviceChannel6.setOffsetNid(30);
    deviceChannel6.setLabelString("06");
    deviceField1114.setOffsetNid(31);
    deviceField1114.setNumCols(5);
    deviceField1114.setIdentifier("");
    deviceField1114.setPreferredWidth(-1);
    deviceField1115.setOffsetNid(34);
    deviceField1115.setTextOnly(true);
    deviceField1115.setNumCols(15);
    deviceField1115.setIdentifier("");
    flowLayout17.setAlignment(FlowLayout.LEFT);
    flowLayout17.setHgap(0);
    flowLayout17.setVgap(0);
    jPanel13.setLayout(flowLayout17);
    deviceField1116.setIdentifier("");
    deviceField1116.setOffsetNid(28);
    deviceField1116.setTextOnly(true);
    deviceField1116.setNumCols(15);
    deviceField1117.setIdentifier("");
    deviceField1117.setOffsetNid(27);
    deviceField1117.setTextOnly(true);
    deviceField1117.setNumCols(15);
    flowLayout18.setVgap(3);
    flowLayout18.setHgap(0);
    flowLayout18.setAlignment(FlowLayout.RIGHT);
    deviceChannel7.setLayout(flowLayout18);
    deviceChannel7.setShowVal("");
    deviceChannel7.setUpdateIdentifier("");
    deviceChannel7.setInSameLine(true);
    deviceChannel7.setOffsetNid(25);
    deviceChannel7.setLabelString("05");
    deviceField1118.setOffsetNid(26);
    deviceField1118.setNumCols(5);
    deviceField1118.setIdentifier("");
    deviceField1119.setOffsetNid(29);
    deviceField1119.setTextOnly(true);
    deviceField1119.setNumCols(15);
    deviceField1119.setIdentifier("");
    flowLayout19.setAlignment(FlowLayout.LEFT);
    flowLayout19.setHgap(0);
    flowLayout19.setVgap(0);
    jPanel14.setLayout(flowLayout19);
    deviceField11110.setIdentifier("");
    deviceField11110.setOffsetNid(23);
    deviceField11110.setTextOnly(true);
    deviceField11110.setNumCols(15);
    deviceField11111.setIdentifier("");
    deviceField11111.setOffsetNid(22);
    deviceField11111.setTextOnly(true);
    deviceField11111.setNumCols(15);
    flowLayout110.setVgap(3);
    flowLayout110.setHgap(0);
    flowLayout110.setAlignment(FlowLayout.RIGHT);
    deviceChannel8.setLayout(flowLayout110);
    deviceChannel8.setShowVal("");
    deviceChannel8.setUpdateIdentifier("");
    deviceChannel8.setInSameLine(true);
    deviceChannel8.setOffsetNid(20);
    deviceChannel8.setLines(1);
    deviceChannel8.setLabelString("04");
    deviceField11112.setOffsetNid(21);
    deviceField11112.setNumCols(5);
    deviceField11112.setIdentifier("");
    deviceField11113.setOffsetNid(24);
    deviceField11113.setTextOnly(true);
    deviceField11113.setNumCols(15);
    deviceField11113.setIdentifier("");
    flowLayout111.setAlignment(FlowLayout.LEFT);
    flowLayout111.setHgap(0);
    flowLayout111.setVgap(0);
    jPanel15.setLayout(flowLayout111);
    deviceField11114.setIdentifier("");
    deviceField11114.setOffsetNid(18);
    deviceField11114.setTextOnly(true);
    deviceField11114.setNumCols(15);
    deviceField11115.setIdentifier("");
    deviceField11115.setOffsetNid(17);
    deviceField11115.setTextOnly(true);
    deviceField11115.setNumCols(15);
    flowLayout112.setVgap(3);
    flowLayout112.setHgap(0);
    flowLayout112.setAlignment(FlowLayout.RIGHT);
    deviceChannel9.setLayout(flowLayout112);
    deviceChannel9.setShowVal("");
    deviceChannel9.setUpdateIdentifier("");
    deviceChannel9.setInSameLine(true);
    deviceChannel9.setOffsetNid(15);
    deviceChannel9.setLabelString("03");
    deviceField11116.setOffsetNid(16);
    deviceField11116.setNumCols(5);
    deviceField11116.setIdentifier("");
    deviceField11117.setOffsetNid(19);
    deviceField11117.setTextOnly(true);
    deviceField11117.setNumCols(15);
    deviceField11117.setIdentifier("");
    flowLayout113.setAlignment(FlowLayout.LEFT);
    flowLayout113.setHgap(0);
    flowLayout113.setVgap(0);
    jPanel16.setLayout(flowLayout113);
    deviceField11118.setIdentifier("");
    deviceField11118.setOffsetNid(13);
    deviceField11118.setTextOnly(true);
    deviceField11118.setNumCols(15);
    deviceField11119.setIdentifier("");
    deviceField11119.setOffsetNid(12);
    deviceField11119.setTextOnly(true);
    deviceField11119.setNumCols(15);
    flowLayout114.setVgap(3);
    flowLayout114.setHgap(0);
    flowLayout114.setAlignment(FlowLayout.RIGHT);
    deviceChannel10.setLayout(flowLayout114);
    deviceChannel10.setShowVal("");
    deviceChannel10.setUpdateIdentifier("");
    deviceChannel10.setInSameLine(true);
    deviceChannel10.setOffsetNid(10);
    deviceChannel10.setLabelString("02");
    deviceField111110.setOffsetNid(11);
    deviceField111110.setNumCols(5);
    deviceField111110.setIdentifier("");
    deviceField111111.setOffsetNid(14);
    deviceField111111.setTextOnly(true);
    deviceField111111.setNumCols(15);
    deviceField111111.setIdentifier("");
    flowLayout115.setAlignment(FlowLayout.LEFT);
    flowLayout115.setHgap(0);
    flowLayout115.setVgap(0);
    jPanel17.setLayout(flowLayout115);
    deviceField2.setOffsetNid(2);
    deviceField2.setTextOnly(true);
    deviceField2.setLabelString("Bira : ");
    deviceField2.setNumCols(10);
    deviceField2.setIdentifier("");
    deviceField3.setOffsetNid(3);
    deviceField3.setTextOnly(true);
    deviceField3.setLabelString("Decoder 1:");
    deviceField3.setNumCols(15);
    deviceField3.setIdentifier("");
    deviceField4.setOffsetNid(4);
    deviceField4.setTextOnly(true);
    deviceField4.setLabelString("Decoder 2:");
    deviceField4.setNumCols(15);
    deviceField4.setIdentifier("");
    jPanel3.setLayout(flowLayout2);
    flowLayout2.setAlignment(FlowLayout.LEFT);
    flowLayout2.setHgap(0);
    flowLayout2.setVgap(0);
    this.getContentPane().add(jPanel2, BorderLayout.NORTH);
    jPanel2.add(jPanel1, null);
    jPanel1.add(deviceField1, null);
    jPanel2.add(jPanel3, null);
    jPanel3.add(deviceField2, null);
    jPanel3.add(deviceField3, null);
    jPanel3.add(deviceField4, null);
    jPanel2.add(jPanel8, null);
    jPanel8.add(jLabel1, null);
    this.getContentPane().add(jScrollPane1,  BorderLayout.CENTER);
    this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
    jScrollPane1.getViewport().add(jPanel6, null);
    jPanel6.add(deviceChannel1, null);
    deviceChannel1.add(jPanel7, null);
    jPanel7.add(deviceField12, null);
    jPanel7.add(deviceField13, null);
    jPanel7.add(deviceField14, null);
    jPanel7.add(deviceField15, null);
    jPanel6.add(deviceChannel10, null);
    deviceChannel10.add(jPanel17, null);
    jPanel17.add(deviceField111110, null);
    jPanel17.add(deviceField11119, null);
    jPanel17.add(deviceField11118, null);
    jPanel17.add(deviceField111111, null);
    jPanel6.add(deviceChannel9, null);
    deviceChannel9.add(jPanel16, null);
    jPanel16.add(deviceField11116, null);
    jPanel16.add(deviceField11115, null);
    jPanel16.add(deviceField11114, null);
    jPanel16.add(deviceField11117, null);
    jPanel6.add(deviceChannel8, null);
    deviceChannel8.add(jPanel15, null);
    jPanel15.add(deviceField11112, null);
    jPanel15.add(deviceField11111, null);
    jPanel15.add(deviceField11110, null);
    jPanel15.add(deviceField11113, null);
    jPanel6.add(deviceChannel7, null);
    deviceChannel7.add(jPanel14, null);
    jPanel14.add(deviceField1118, null);
    jPanel14.add(deviceField1117, null);
    jPanel14.add(deviceField1116, null);
    jPanel14.add(deviceField1119, null);
    jPanel6.add(deviceChannel6, null);
    deviceChannel6.add(jPanel13, null);
    jPanel13.add(deviceField1114, null);
    jPanel13.add(deviceField1113, null);
    jPanel13.add(deviceField1112, null);
    jPanel13.add(deviceField1115, null);
    jPanel6.add(deviceChannel5, null);
    deviceChannel5.add(jPanel12, null);
    jPanel12.add(deviceField1110, null);
    jPanel12.add(deviceField119, null);
    jPanel12.add(deviceField118, null);
    jPanel12.add(deviceField1111, null);
    jPanel6.add(deviceChannel4, null);
    deviceChannel4.add(jPanel11, null);
    jPanel11.add(deviceField116, null);
    jPanel11.add(deviceField115, null);
    jPanel11.add(deviceField114, null);
    jPanel11.add(deviceField117, null);
    jPanel6.add(deviceChannel3, null);
    deviceChannel3.add(jPanel10, null);
    jPanel10.add(deviceField112, null);
    jPanel10.add(deviceField111, null);
    jPanel10.add(deviceField110, null);
    jPanel10.add(deviceField113, null);
    jPanel6.add(deviceChannel2, null);
    deviceChannel2.add(jPanel9, null);
    jPanel9.add(deviceField18, null);
    jPanel9.add(deviceField17, null);
    jPanel9.add(deviceField16, null);
    jPanel9.add(deviceField19, null);
  }

}
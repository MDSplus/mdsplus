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

public class MHD_BR_TESTSetup
    extends DeviceSetup {
  BorderLayout borderLayout1 = new BorderLayout();
  JPanel jPanel1 = new JPanel();
  DeviceField deviceField1 = new DeviceField();
  JTabbedPane jTabbedPane1 = new JTabbedPane();
  DeviceButtons deviceButtons1 = new DeviceButtons();
  FlowLayout flowLayout14 = new FlowLayout();
  JPanel jPanel23 = new JPanel();
  BorderLayout borderLayout21 = new BorderLayout();
  DeviceField deviceField27 = new DeviceField();
  BorderLayout borderLayout117 = new BorderLayout();
  BorderLayout borderLayout118 = new BorderLayout();
  DeviceChannel deviceChannel115 = new DeviceChannel();
  DeviceChannel deviceChannel116 = new DeviceChannel();
  DeviceField deviceField2118 = new DeviceField();
  DeviceField deviceField2119 = new DeviceField();
  JPanel jPanel217 = new JPanel();
  DeviceField deviceField21110 = new DeviceField();
  FlowLayout flowLayout1111 = new FlowLayout();
  DeviceField deviceField2120 = new DeviceField();
  GridLayout gridLayout5 = new GridLayout();
  DeviceField deviceField21111 = new DeviceField();
  DeviceChannel deviceChannel117 = new DeviceChannel();
  FlowLayout flowLayout1112 = new FlowLayout();
  FlowLayout flowLayout1113 = new FlowLayout();
  JPanel jPanel218 = new JPanel();
  DeviceField deviceField220 = new DeviceField();
  DeviceField deviceField2121 = new DeviceField();
  FlowLayout flowLayout1114 = new FlowLayout();
  DeviceField deviceField2122 = new DeviceField();
  FlowLayout flowLayout1115 = new FlowLayout();
  JPanel jPanel219 = new JPanel();
  JPanel jPanel31 = new JPanel();
  DeviceField deviceField2123 = new DeviceField();
  DeviceChannel deviceChannel118 = new DeviceChannel();
  DeviceField deviceField21112 = new DeviceField();
  DeviceField deviceField2124 = new DeviceField();
  FlowLayout flowLayout26 = new FlowLayout();
  BorderLayout borderLayout23 = new BorderLayout();
  DeviceField deviceField221 = new DeviceField();
  DeviceField deviceField2125 = new DeviceField();
  DeviceField deviceField21113 = new DeviceField();
  JPanel jPanel2110 = new JPanel();
  DeviceChannel deviceChannel119 = new DeviceChannel();
  FlowLayout flowLayout1116 = new FlowLayout();
  JPanel jPanel220 = new JPanel();
  FlowLayout flowLayout1117 = new FlowLayout();
  DeviceField deviceField222 = new DeviceField();
  JPanel jPanel2111 = new JPanel();
  DeviceField deviceField223 = new DeviceField();
  FlowLayout flowLayout1118 = new FlowLayout();
  JPanel jPanel2112 = new JPanel();
  BorderLayout borderLayout119 = new BorderLayout();
  DeviceField deviceField2126 = new DeviceField();
  DeviceField deviceField224 = new DeviceField();
  JPanel jPanel2113 = new JPanel();
  JPanel jPanel221 = new JPanel();
  BorderLayout borderLayout120 = new BorderLayout();
  BorderLayout borderLayout1110 = new BorderLayout();
  FlowLayout flowLayout120 = new FlowLayout();
  JPanel jPanel4 = new JPanel();
  GridLayout gridLayout2 = new GridLayout();
  JLabel jLabel3 = new JLabel();
  DeviceField deviceField2127 = new DeviceField();
  FlowLayout flowLayout121 = new FlowLayout();
  JPanel jPanel222 = new JPanel();
  DeviceField deviceField21114 = new DeviceField();
  DeviceChannel deviceChannel120 = new DeviceChannel();
  JPanel jPanel2114 = new JPanel();
  JScrollPane jScrollPane3 = new JScrollPane();
  DeviceChannel deviceChannel1110 = new DeviceChannel();
  BorderLayout borderLayout121 = new BorderLayout();
  JPanel jPanel2115 = new JPanel();
  FlowLayout flowLayout1119 = new FlowLayout();
  BorderLayout borderLayout1111 = new BorderLayout();
  DeviceChannel deviceChannel1111 = new DeviceChannel();
  JPanel jPanel223 = new JPanel();
  BorderLayout borderLayout122 = new BorderLayout();
  FlowLayout flowLayout1120 = new FlowLayout();
  BorderLayout borderLayout1112 = new BorderLayout();
  DeviceField deviceField225 = new DeviceField();
  DeviceChannel deviceChannel1112 = new DeviceChannel();
  DeviceField deviceField226 = new DeviceField();
  DeviceChannel deviceChannel121 = new DeviceChannel();
  DeviceField deviceField21115 = new DeviceField();
  JPanel jPanel224 = new JPanel();
  BorderLayout borderLayout1113 = new BorderLayout();
  DeviceField deviceField227 = new DeviceField();
  DeviceChannel deviceChannel1113 = new DeviceChannel();
  JPanel jPanel225 = new JPanel();
  DeviceField deviceField2128 = new DeviceField();
  JPanel jPanel226 = new JPanel();
  DeviceField deviceField21116 = new DeviceField();
  FlowLayout flowLayout122 = new FlowLayout();
  DeviceChannel deviceChannel1114 = new DeviceChannel();
  FlowLayout flowLayout1121 = new FlowLayout();
  BorderLayout borderLayout1114 = new BorderLayout();
  BorderLayout borderLayout1115 = new BorderLayout();
  public MHD_BR_TESTSetup() {
    try {
      jbInit();
    }
    catch (Exception e) {
      e.printStackTrace();
    }
  }

  private void jbInit() throws Exception {
    this.setWidth(670);
    this.setHeight(460);
    this.setDeviceType("MHD_BR_TEST");
    this.setDeviceProvider("150.178.3.33");
    this.setDeviceTitle("Eletromagnetic Test");
    this.getContentPane().setLayout(borderLayout1);
    jPanel1.setLayout(flowLayout14);
    deviceField1.setOffsetNid(1);
    deviceField1.setLabelString("Comment: ");
    deviceField1.setNumCols(50);
    deviceField1.setIdentifier("");
    flowLayout14.setAlignment(FlowLayout.LEFT);
    flowLayout14.setHgap(0);
    flowLayout14.setVgap(0);
    jPanel23.setLayout(borderLayout21);
    deviceButtons1.setMethods(new String[] {"init"});
    deviceField27.setNumCols(25);
    deviceField27.setTextOnly(true);
    deviceField27.setOffsetNid(40);
    deviceChannel115.setOffsetNid(23);
    deviceChannel115.setLabelString("07");
    deviceChannel115.setInSameLine(true);
    deviceChannel115.setLayout(borderLayout1114);
    deviceChannel116.setLayout(borderLayout121);
    deviceChannel116.setInSameLine(true);
    deviceChannel116.setLabelString("12");
    deviceChannel116.setOffsetNid(38);
    deviceField2118.setOffsetNid(27);
    deviceField2118.setTextOnly(true);
    deviceField2118.setNumCols(25);
    deviceField2119.setOffsetNid(19);
    deviceField2119.setTextOnly(true);
    deviceField2119.setNumCols(25);
    jPanel217.setLayout(flowLayout26);
    deviceField21110.setOffsetNid(6);
    deviceField21110.setTextOnly(true);
    deviceField21110.setNumCols(25);
        deviceField21110.setIdentifier("");
    flowLayout1111.setVgap(0);
    flowLayout1111.setHgap(0);
    flowLayout1111.setAlignment(FlowLayout.LEFT);
    deviceField2120.setOffsetNid(30);
    deviceField2120.setTextOnly(true);
    deviceField2120.setNumCols(25);
    gridLayout5.setRows(3);
    deviceField21111.setOffsetNid(18);
    deviceField21111.setTextOnly(true);
    deviceField21111.setNumCols(25);
    deviceChannel117.setLayout(borderLayout117);
    deviceChannel117.setInSameLine(true);
    deviceChannel117.setUpdateIdentifier("");
    deviceChannel117.setShowVal("");
    deviceChannel117.setLabelString("02");
    deviceChannel117.setOffsetNid(8);
    flowLayout1112.setVgap(0);
    flowLayout1112.setHgap(0);
    flowLayout1112.setAlignment(FlowLayout.LEFT);
    flowLayout1113.setVgap(0);
    flowLayout1113.setHgap(0);
    flowLayout1113.setAlignment(FlowLayout.LEFT);
    jPanel218.setLayout(flowLayout1112);
    deviceField220.setOffsetNid(39);
    deviceField220.setTextOnly(true);
    deviceField220.setNumCols(25);
    deviceField2121.setOffsetNid(24);
    deviceField2121.setTextOnly(true);
    deviceField2121.setNumCols(25);
    flowLayout1114.setVgap(0);
    flowLayout1114.setHgap(0);
    flowLayout1114.setAlignment(FlowLayout.LEFT);
    deviceField2122.setOffsetNid(33);
    deviceField2122.setTextOnly(true);
    deviceField2122.setNumCols(25);
    flowLayout1115.setVgap(0);
    flowLayout1115.setHgap(0);
    flowLayout1115.setAlignment(FlowLayout.LEFT);
    jPanel219.setLayout(flowLayout122);
    jPanel31.setLayout(borderLayout23);
    deviceField2123.setOffsetNid(22);
    deviceField2123.setTextOnly(true);
    deviceField2123.setNumCols(25);
    deviceChannel118.setLayout(borderLayout120);
    deviceChannel118.setInSameLine(true);
    deviceChannel118.setLabelString("11");
    deviceChannel118.setOffsetNid(35);
    deviceField21112.setOffsetNid(13);
    deviceField21112.setTextOnly(true);
    deviceField21112.setNumCols(25);
    deviceField2124.setOffsetNid(21);
    deviceField2124.setTextOnly(true);
    deviceField2124.setNumCols(25);
    flowLayout26.setAlignment(FlowLayout.LEFT);
    flowLayout26.setHgap(0);
    flowLayout26.setVgap(0);
    deviceField221.setOffsetNid(10);
    deviceField221.setTextOnly(true);
    deviceField221.setNumCols(25);
    deviceField221.setIdentifier("");
    deviceField2125.setOffsetNid(25);
    deviceField2125.setTextOnly(true);
    deviceField2125.setNumCols(25);
    deviceField21113.setOffsetNid(7);
    deviceField21113.setTextOnly(true);
    deviceField21113.setNumCols(25);
        deviceField21113.setIdentifier("");
    jPanel2110.setLayout(flowLayout1118);
    deviceChannel119.setLayout(borderLayout119);
    deviceChannel119.setInSameLine(true);
    deviceChannel119.setLabelString("09");
    deviceChannel119.setOffsetNid(29);
    flowLayout1116.setAlignment(FlowLayout.LEFT);
    flowLayout1116.setHgap(0);
    flowLayout1116.setVgap(0);
    jPanel220.setLayout(flowLayout1117);
    flowLayout1117.setVgap(0);
    flowLayout1117.setHgap(0);
    flowLayout1117.setAlignment(FlowLayout.LEFT);
    deviceField222.setNumCols(15);
    deviceField222.setIdentifier("");
    deviceField222.setOffsetNid(3);
    deviceField222.setLabelString("Signal Test Delay: ");
    jPanel2111.setLayout(flowLayout1121);
    deviceField223.setOffsetNid(4);
    deviceField223.setLabelString("Signal Test Duration: ");
    deviceField223.setNumCols(15);
    deviceField223.setIdentifier("");
    flowLayout1118.setVgap(0);
    flowLayout1118.setHgap(0);
    flowLayout1118.setAlignment(FlowLayout.LEFT);
    jPanel2112.setLayout(flowLayout1114);
    deviceField2126.setOffsetNid(34);
    deviceField2126.setTextOnly(true);
    deviceField2126.setNumCols(25);
    deviceField224.setOffsetNid(9);
    deviceField224.setTextOnly(true);
    deviceField224.setNumCols(25);
    deviceField224.setIdentifier("");
    jPanel2113.setLayout(flowLayout1111);
    jPanel221.setLayout(flowLayout121);
    flowLayout120.setVgap(0);
    flowLayout120.setHgap(0);
    flowLayout120.setAlignment(FlowLayout.LEFT);
    jPanel4.setLayout(gridLayout2);
    gridLayout2.setRows(12);
    jLabel3.setText(
        "   CARD                                GAIN                      " +
        "                                              ADC");
    deviceField2127.setOffsetNid(31);
    deviceField2127.setTextOnly(true);
    deviceField2127.setNumCols(25);
    flowLayout121.setVgap(0);
    flowLayout121.setHgap(0);
    flowLayout121.setAlignment(FlowLayout.LEFT);
    jPanel222.setLayout(flowLayout1116);
    deviceField21114.setOffsetNid(16);
    deviceField21114.setTextOnly(true);
    deviceField21114.setNumCols(25);
    deviceChannel120.setLayout(borderLayout1112);
    deviceChannel120.setInSameLine(true);
    deviceChannel120.setLabelString("08");
    deviceChannel120.setOffsetNid(26);
    jPanel2114.setLayout(flowLayout1115);
    deviceChannel1110.setLayout(borderLayout118);
    deviceChannel1110.setInSameLine(true);
    deviceChannel1110.setUpdateIdentifier("");
    deviceChannel1110.setShowVal("");
    deviceChannel1110.setLabelString("01");
    deviceChannel1110.setOffsetNid(5);
    jPanel2115.setLayout(flowLayout1113);
    flowLayout1119.setAlignment(FlowLayout.LEFT);
    flowLayout1119.setHgap(0);
    flowLayout1119.setVgap(0);
    deviceChannel1111.setLayout(borderLayout1113);
    deviceChannel1111.setInSameLine(true);
    deviceChannel1111.setLabelString("05");
    deviceChannel1111.setOffsetNid(17);
    deviceChannel1111.setLines(1);
    jPanel223.setLayout(gridLayout5);
    flowLayout1120.setVgap(0);
    flowLayout1120.setHgap(0);
    flowLayout1120.setAlignment(FlowLayout.LEFT);
    deviceField225.setOffsetNid(37);
    deviceField225.setTextOnly(true);
    deviceField225.setNumCols(25);
    deviceChannel1112.setLayout(borderLayout1110);
    deviceChannel1112.setInSameLine(true);
    deviceChannel1112.setLabelString("04");
    deviceChannel1112.setOffsetNid(14);
    deviceChannel1112.setLines(1);
    deviceField226.setOffsetNid(2);
    deviceField226.setTextOnly(true);
    deviceField226.setLabelString("Decoder: ");
    deviceField226.setNumCols(30);
    deviceField226.setIdentifier("");
    deviceChannel121.setLayout(borderLayout122);
    deviceChannel121.setInSameLine(true);
    deviceChannel121.setLabelString("10");
    deviceChannel121.setOffsetNid(32);
    deviceField21115.setOffsetNid(12);
    deviceField21115.setTextOnly(true);
    deviceField21115.setNumCols(25);
    jPanel224.setLayout(flowLayout1119);
    deviceField227.setOffsetNid(36);
    deviceField227.setTextOnly(true);
    deviceField227.setNumCols(25);
    deviceChannel1113.setLayout(borderLayout1111);
    deviceChannel1113.setInSameLine(true);
    deviceChannel1113.setLabelString("03");
    deviceChannel1113.setOffsetNid(11);
    deviceChannel1113.setLines(1);
    jPanel225.setLayout(flowLayout1120);
    deviceField2128.setOffsetNid(28);
    deviceField2128.setTextOnly(true);
    deviceField2128.setNumCols(25);
    jPanel226.setLayout(flowLayout120);
    deviceField21116.setOffsetNid(15);
    deviceField21116.setTextOnly(true);
    deviceField21116.setNumCols(25);
    flowLayout122.setVgap(0);
    flowLayout122.setHgap(0);
    flowLayout122.setAlignment(FlowLayout.LEFT);
    deviceChannel1114.setLayout(borderLayout1115);
    deviceChannel1114.setInSameLine(true);
    deviceChannel1114.setLabelString("06");
    deviceChannel1114.setOffsetNid(20);
    flowLayout1121.setVgap(0);
    flowLayout1121.setHgap(0);
    flowLayout1121.setAlignment(FlowLayout.LEFT);
    this.getContentPane().add(jPanel1, BorderLayout.NORTH);
    jPanel1.add(deviceField1, null);
    this.getContentPane().add(jTabbedPane1, BorderLayout.CENTER);
    jTabbedPane1.add(jPanel23, "MHD BR");
    jPanel23.add(jPanel31, BorderLayout.NORTH);
    jPanel31.add(jScrollPane3, BorderLayout.CENTER);
    jScrollPane3.getViewport().add(jPanel4, null);
    jPanel224.add(deviceField222, null);
    jPanel224.add(deviceField223, null);
    jPanel217.add(jLabel3, null);
    jPanel223.add(jPanel222, null);
    jPanel222.add(deviceField226, null);
    jPanel223.add(jPanel224, null);
    jPanel223.add(jPanel217, null);
    jPanel226.add(deviceField224, null);
    jPanel226.add(deviceField221, null);
    jPanel4.add(deviceChannel1110, null);
    deviceChannel1110.add(jPanel218, BorderLayout.CENTER);
    jPanel4.add(deviceChannel117, null);
    deviceChannel117.add(jPanel226, BorderLayout.CENTER);
    jPanel218.add(deviceField21110, null);
    jPanel218.add(deviceField21113, null);
    jPanel4.add(deviceChannel1113, null);
    deviceChannel1113.add(jPanel2110, BorderLayout.CENTER);
    jPanel2110.add(deviceField21115, null);
    jPanel2110.add(deviceField21112, null);
    jPanel4.add(deviceChannel1112, null);
    deviceChannel1112.add(jPanel2111, BorderLayout.CENTER);
    jPanel2111.add(deviceField21116, null);
    jPanel2111.add(deviceField21114, null);
    jPanel4.add(deviceChannel1111, null);
    deviceChannel1111.add(jPanel2112, BorderLayout.CENTER);
    jPanel2112.add(deviceField21111, null);
    jPanel2112.add(deviceField2119, null);
    jPanel4.add(deviceChannel1114, null);
    deviceChannel1114.add(jPanel2115, BorderLayout.CENTER);
    jPanel2115.add(deviceField2124, null);
    jPanel2115.add(deviceField2123, null);
    jPanel4.add(deviceChannel115, null);
    deviceChannel115.add(jPanel2113, BorderLayout.CENTER);
    jPanel2113.add(deviceField2121, null);
    jPanel2113.add(deviceField2125, null);
    jPanel4.add(deviceChannel120, null);
    deviceChannel120.add(jPanel2114, BorderLayout.CENTER);
    jPanel2114.add(deviceField2118, null);
    jPanel2114.add(deviceField2128, null);
    jPanel4.add(deviceChannel119, null);
    deviceChannel119.add(jPanel225, BorderLayout.CENTER);
    jPanel225.add(deviceField2120, null);
    jPanel225.add(deviceField2127, null);
    jPanel4.add(deviceChannel121, null);
    deviceChannel121.add(jPanel220, BorderLayout.CENTER);
    jPanel220.add(deviceField2122, null);
    jPanel220.add(deviceField2126, null);
    jPanel4.add(deviceChannel118, null);
    deviceChannel118.add(jPanel219, BorderLayout.CENTER);
    jPanel219.add(deviceField227, null);
    jPanel219.add(deviceField225, null);
    jPanel4.add(deviceChannel116, null);
    deviceChannel116.add(jPanel221, BorderLayout.CENTER);
    jPanel221.add(deviceField220, null);
    jPanel221.add(deviceField27, null);
    this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
    jPanel31.add(jPanel223, BorderLayout.NORTH);
  }

}
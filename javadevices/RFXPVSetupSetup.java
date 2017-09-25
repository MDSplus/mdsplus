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

public class RFXPVSetupSetup extends DeviceSetup {
  BorderLayout borderLayout1 = new BorderLayout();
  DeviceButtons deviceButtons1 = new DeviceButtons();
  JPanel jPanel1 = new JPanel();
  JTabbedPane jTabbedPane1 = new JTabbedPane();
  GridLayout gridLayout1 = new GridLayout();
  JPanel jPanel2 = new JPanel();
  JPanel jPanel3 = new JPanel();
  JPanel jPanel4 = new JPanel();
  DeviceField deviceField1 = new DeviceField();
  DeviceField deviceField2 = new DeviceField();
  DeviceField deviceField3 = new DeviceField();
  DeviceField deviceField5 = new DeviceField();

  DeviceField deviceField6 = new DeviceField();


  JPanel jPanel5 = new JPanel();
  JPanel jPanel6 = new JPanel();
  JPanel jPanel7 = new JPanel();
  JPanel jPanel8 = new JPanel();
  JPanel jPanel9 = new JPanel();
  JPanel jPanel10 = new JPanel();
  JPanel jPanel11 = new JPanel();
  JPanel jPanel12 = new JPanel();
  DeviceWave deviceWave1 = new DeviceWave();
  BorderLayout borderLayout2 = new BorderLayout();
  BorderLayout borderLayout3 = new BorderLayout();
  DeviceWave deviceWave2 = new DeviceWave();
  BorderLayout borderLayout4 = new BorderLayout();
  DeviceWave deviceWave3 = new DeviceWave();
  BorderLayout borderLayout5 = new BorderLayout();
  DeviceWave deviceWave4 = new DeviceWave();
  BorderLayout borderLayout6 = new BorderLayout();
  DeviceWave deviceWave5 = new DeviceWave();
  BorderLayout borderLayout7 = new BorderLayout();
  DeviceWave deviceWave6 = new DeviceWave();
  BorderLayout borderLayout8 = new BorderLayout();
  DeviceWave deviceWave7 = new DeviceWave();
  BorderLayout borderLayout9 = new BorderLayout();
  DeviceWave deviceWave8 = new DeviceWave();
  DeviceChoice deviceChoice1 = new DeviceChoice();
  public RFXPVSetupSetup() {
    try {
      jbInit();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  private void jbInit() throws Exception {
    this.setWidth(568);
    this.setHeight(568);
    this.setDeviceType("RFXPVSetup");
    this.setDeviceProvider("localhost");
    this.setDeviceTitle("RFX PV Setup");
    this.getContentPane().setLayout(borderLayout1);
    jPanel1.setLayout(gridLayout1);
    gridLayout1.setColumns(1);
    gridLayout1.setRows(3);
    deviceField1.setOffsetNid(1);
    deviceField1.setTextOnly(true);
    deviceField1.setLabelString("Comment:");
    deviceField1.setNumCols(30);
    deviceField1.setIdentifier("");
    deviceField2.setOffsetNid(4);
    deviceField2.setLabelString("Window: ");
    deviceField2.setNumCols(4);
    deviceField2.setIdentifier("");
    deviceField2.setEditable(false);
    deviceField2.setDisplayEvaluated(true);
    deviceField3.setOffsetNid(5);
    deviceField3.setTextOnly(true);
    deviceField3.setLabelString("Enabled Units: ");
    deviceField3.setNumCols(20);
    deviceField3.setIdentifier("");
    deviceField3.setEditable(false);
    deviceField3.setDisplayEvaluated(true);
    deviceField5.setOffsetNid(2);
    deviceField5.setTextOnly(true);
    deviceField5.setLabelString("Connection: ");
    deviceField5.setNumCols(8);
    deviceField5.setIdentifier("");
    deviceField5.setEditable(false);
    deviceField5.setDisplayEvaluated(true);
    deviceWave1.setOffsetNid(8);
    deviceWave1.setIdentifier("");
    deviceWave1.setUpdateExpression("");
    jPanel5.setLayout(borderLayout2);
    jPanel6.setLayout(borderLayout3);
    deviceWave2.setOffsetNid(14);
    deviceWave2.setIdentifier("");
    deviceWave2.setUpdateExpression("");
    jPanel7.setLayout(borderLayout4);
    deviceWave3.setOffsetNid(20);
    deviceWave3.setIdentifier("");
    deviceWave3.setUpdateExpression("");
    jPanel8.setLayout(borderLayout5);
    deviceWave4.setOffsetNid(26);
    deviceWave4.setIdentifier("");
    deviceWave4.setUpdateExpression("");
    jPanel9.setLayout(borderLayout6);
    deviceWave5.setOffsetNid(32);
    deviceWave5.setIdentifier("");
    deviceWave5.setUpdateExpression("");
    jPanel10.setLayout(borderLayout7);
    deviceWave6.setOffsetNid(38);
    deviceWave6.setIdentifier("");
    deviceWave6.setUpdateExpression("");
    jPanel11.setLayout(borderLayout8);
    deviceWave7.setOffsetNid(44);
    deviceWave7.setIdentifier("");
    deviceWave7.setUpdateExpression("");
    jPanel12.setLayout(borderLayout9);
    deviceWave8.setOffsetNid(50);
    deviceWave8.setIdentifier("");
    deviceWave8.setUpdateExpression("");
/*
    deviceChoice1.setChoiceIntValues(null);
    deviceChoice1.setChoiceFloatValues(null);
    deviceChoice1.setOffsetNid(3);
    deviceChoice1.setLabelString("Control:");
    deviceChoice1.setChoiceItems(new String[] {"CURRENT", "VOLTAGE", "OPEN LOOP"});
    deviceChoice1.setUpdateIdentifier("");
    deviceChoice1.setIdentifier("");
*/

    deviceField6.setOffsetNid(3);
    deviceField6.setTextOnly(true);
    deviceField6.setLabelString("Control: ");
    deviceField6.setNumCols(8);
    deviceField6.setIdentifier("");
    deviceField6.setEditable(false);
    deviceField6.setDisplayEvaluated(true);


    this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
    this.getContentPane().add(jPanel1, BorderLayout.NORTH);
    jPanel1.add(jPanel2, null);
    jPanel2.add(deviceField1, null);
    jPanel1.add(jPanel4, null);
    jPanel4.add(deviceField3, null);
    jPanel4.add(deviceField2, null);
    jPanel1.add(jPanel3, null);
    jPanel3.add(deviceField5, null);

//  jPanel3.add(deviceChoice1, null);
    jPanel3.add(deviceField6, null);

    this.getContentPane().add(jTabbedPane1,  BorderLayout.CENTER);
    jTabbedPane1.add(jPanel5,   "1");
    jPanel5.add(deviceWave1,  BorderLayout.CENTER);
    jTabbedPane1.add(jPanel6,   "2");
    jPanel6.add(deviceWave2,  BorderLayout.CENTER);
    jTabbedPane1.add(jPanel7,   "3");
    jPanel7.add(deviceWave3,  BorderLayout.CENTER);
    jTabbedPane1.add(jPanel8,   "4");
    jPanel8.add(deviceWave4,  BorderLayout.CENTER);
    jTabbedPane1.add(jPanel9,   "5");
    jPanel9.add(deviceWave5,  BorderLayout.CENTER);
    jTabbedPane1.add(jPanel10,   "6");
    jPanel10.add(deviceWave6,  BorderLayout.CENTER);
    jTabbedPane1.add(jPanel11,   "7");
    jPanel11.add(deviceWave7,  BorderLayout.CENTER);
    jTabbedPane1.add(jPanel12,   "8");
    jPanel12.add(deviceWave8,  BorderLayout.CENTER);
  }

}
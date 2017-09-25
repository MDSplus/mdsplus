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

public class RFXPCSetupSetup extends DeviceSetup {
  BorderLayout borderLayout1 = new BorderLayout();
  DeviceButtons deviceButtons1 = new DeviceButtons();
  DeviceWave deviceWave1 = new DeviceWave();
  JPanel jPanel1 = new JPanel();
  GridLayout gridLayout1 = new GridLayout();
  JPanel jPanel2 = new JPanel();
  JPanel jPanel3 = new JPanel();
  JPanel jPanel4 = new JPanel();
  DeviceField deviceField1 = new DeviceField();
  DeviceField deviceField2 = new DeviceField();
  DeviceField deviceField3 = new DeviceField();
  DeviceField deviceField5 = new DeviceField();
  DeviceChoice deviceChoice1 = new DeviceChoice();
  JPanel jPanel5 = new JPanel();
  JLabel jLabel1 = new JLabel();
  DeviceField deviceField4 = new DeviceField();
  DeviceField deviceField6 = new DeviceField();
  DeviceField deviceField7 = new DeviceField();
  DeviceField deviceField8 = new DeviceField();
  DeviceField deviceField9 = new DeviceField();
  DeviceField deviceField10 = new DeviceField();

  public RFXPCSetupSetup() {
    try {
      jbInit();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  private void jbInit() throws Exception {
    this.setWidth(700);
    this.setHeight(515);
    this.setDeviceType("RFXPCSetup");
    this.setDeviceProvider("localhost");
    this.setDeviceTitle("RFX PC Setup");
    this.getContentPane().setLayout(borderLayout1);
    deviceWave1.setOffsetNid(6);
    deviceWave1.setIdentifier("");
    deviceWave1.setUpdateExpression("");
    jPanel1.setLayout(gridLayout1);
    gridLayout1.setColumns(1);
    gridLayout1.setRows(4);
    deviceField1.setOffsetNid(1);
    deviceField1.setTextOnly(true);
    deviceField1.setLabelString("Comment:");
    deviceField1.setNumCols(25);
    deviceField1.setIdentifier("");
    deviceField2.setOffsetNid(4);
    deviceField2.setLabelString("Window");
    deviceField2.setNumCols(4);
    deviceField2.setIdentifier("");
    deviceField2.setEditable(false);
    deviceField2.setDisplayEvaluated(true);
    deviceField3.setOffsetNid(5);
    deviceField3.setTextOnly(true);
    deviceField3.setLabelString("Enabled Units:");
    deviceField3.setNumCols(20);
    deviceField3.setIdentifier("");
    deviceField3.setEditable(false);
    deviceField3.setDisplayEvaluated(true);
    deviceField5.setOffsetNid(2);
    deviceField5.setTextOnly(true);
    deviceField5.setLabelString("Connection:");
    deviceField5.setNumCols(8);
    deviceField5.setIdentifier("");
    deviceField5.setEditable(false);
    deviceField5.setDisplayEvaluated(true);
/*
    deviceChoice1.setChoiceIntValues(null);
    deviceChoice1.setChoiceFloatValues(null);
    deviceChoice1.setOffsetNid(3);
    deviceChoice1.setLabelString("Control:");
    deviceChoice1.setChoiceItems(new String[] {"CURRENT", "VOLTAGE", "OPEN LOOP"});
    deviceChoice1.setUpdateIdentifier("");
    deviceChoice1.setIdentifier("");
*/
    deviceField10.setOffsetNid(3);
    deviceField10.setTextOnly(true);
    deviceField10.setLabelString("Control:");
    deviceField10.setNumCols(8);
    deviceField10.setIdentifier("");
    deviceField10.setEditable(false);
    deviceField10.setDisplayEvaluated(true);

    jLabel1.setIconTextGap(4);
    jLabel1.setText("Pert.: ");
    deviceField8.setOffsetNid(12);
    deviceField8.setLabelString("Ampl. (A): ");
    deviceField8.setNumCols(3);
    deviceField8.setIdentifier("");
    deviceField7.setOffsetNid(13);
    deviceField7.setLabelString("Freq. (Hz): ");
    deviceField7.setNumCols(3);
    deviceField7.setIdentifier("");
    deviceField6.setOffsetNid(14);
    deviceField6.setLabelString("Phase (Rad): ");
    deviceField6.setNumCols(3);
    deviceField6.setIdentifier("");
    deviceField4.setOffsetNid(15);
    deviceField4.setLabelString("Start Time(s): ");
    deviceField4.setNumCols(3);
    deviceField4.setIdentifier("");
    deviceField9.setOffsetNid(16);
    deviceField9.setLabelString("End Time (s):");
    deviceField9.setNumCols(3);
    deviceField9.setIdentifier("");
    this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
    this.getContentPane().add(deviceWave1, BorderLayout.CENTER);
    this.getContentPane().add(jPanel1, BorderLayout.NORTH);
    jPanel1.add(jPanel2, null);
    jPanel2.add(deviceField1, null);
    jPanel1.add(jPanel4, null);
    jPanel4.add(deviceField3, null);
    jPanel4.add(deviceField2, null);
    jPanel1.add(jPanel3, null);
    jPanel3.add(deviceField5, null);

//    jPanel3.add(deviceChoice1, null);
    jPanel3.add(deviceField10, null);

    jPanel1.add(jPanel5, null);
    jPanel5.add(jLabel1, null);
    jPanel5.add(deviceField8, null);
    jPanel5.add(deviceField7, null);
    jPanel5.add(deviceField6, null);
    jPanel5.add(deviceField4, null);
    jPanel5.add(deviceField9, null);
  }

}
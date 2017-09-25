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

public class TSEdgeCCDSetup extends DeviceSetup {
  JPanel jPanel1 = new JPanel();
  BorderLayout borderLayout1 = new BorderLayout();
  GridLayout gridLayout1 = new GridLayout();
  JPanel jPanel2 = new JPanel();
  DeviceField deviceField1 = new DeviceField();
  DeviceDispatch deviceDispatch1 = new DeviceDispatch();
  FlowLayout flowLayout1 = new FlowLayout();
  JPanel jPanel3 = new JPanel();
  FlowLayout flowLayout2 = new FlowLayout();
  DeviceField deviceField2 = new DeviceField();
  DeviceChoice deviceChoice1 = new DeviceChoice();
  DeviceField deviceField3 = new DeviceField();
  JPanel jPanel4 = new JPanel();
  FlowLayout flowLayout3 = new FlowLayout();
  DeviceField deviceField4 = new DeviceField();
  JPanel jPanel5 = new JPanel();
  FlowLayout flowLayout4 = new FlowLayout();
  DeviceField deviceField5 = new DeviceField();
  JPanel jPanel6 = new JPanel();
  DeviceButtons deviceButtons1 = new DeviceButtons();
  public TSEdgeCCDSetup() {
    try {
      jbInit();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  private void jbInit() throws Exception {
    this.setWidth(550);
    this.setHeight(250);
    this.setDeviceType("TSEdgeCCD");
    this.setDeviceProvider("150.178.3.33");
    this.setDeviceTitle("CCD Edge Thomson Scattering");
    this.getContentPane().setLayout(borderLayout1);
    jPanel1.setLayout(gridLayout1);
    gridLayout1.setRows(5);
    deviceField1.setOffsetNid(1);
    deviceField1.setLabelString("Comment: ");
    deviceField1.setNumCols(25);
    deviceField1.setIdentifier("");
    jPanel2.setLayout(flowLayout1);
    flowLayout1.setAlignment(FlowLayout.LEFT);
    jPanel3.setLayout(flowLayout2);
    flowLayout2.setAlignment(FlowLayout.LEFT);
    deviceField2.setOffsetNid(2);
    deviceField2.setLabelString("Interface: ");
    deviceField2.setNumCols(5);
    deviceField2.setIdentifier("");
    deviceChoice1.setChoiceIntValues(null);
    deviceChoice1.setChoiceFloatValues(null);
    deviceChoice1.setOffsetNid(3);
    deviceChoice1.setLabelString("Mode: ");
    deviceChoice1.setChoiceItems(new String[] {"LOCAL", "REMOTE"});
    deviceChoice1.setUpdateIdentifier("");
    deviceChoice1.setIdentifier("");
    deviceField3.setOffsetNid(4);
    deviceField3.setTextOnly(true);
    deviceField3.setLabelString("Ip adress");
    deviceField3.setNumCols(15);
    deviceField3.setIdentifier("");
    jPanel4.setLayout(flowLayout3);
    flowLayout3.setAlignment(FlowLayout.LEFT);
    deviceField4.setOffsetNid(5);
    deviceField4.setLabelString("ARM trigger: ");
    deviceField4.setNumCols(35);
    deviceField4.setIdentifier("");
    jPanel5.setLayout(flowLayout4);
    flowLayout4.setAlignment(FlowLayout.LEFT);
    deviceField5.setOffsetNid(6);
    deviceField5.setLabelString("LASER trigger");
    deviceField5.setNumCols(35);
    deviceField5.setIdentifier("");
    deviceButtons1.setCheckMessages(null);
    deviceButtons1.setMethods(new String[] {"init", "arm", "trigger", "store"});
    this.getContentPane().add(jPanel1,  BorderLayout.CENTER);
    jPanel1.add(jPanel2, null);
    jPanel2.add(deviceField1, null);
    jPanel2.add(deviceDispatch1, null);
    jPanel1.add(jPanel3, null);
    jPanel3.add(deviceChoice1, null);
    jPanel3.add(deviceField3, null);
    jPanel3.add(deviceField2, null);
    jPanel1.add(jPanel4, null);
    jPanel4.add(deviceField4, null);
    jPanel1.add(jPanel5, null);
    jPanel5.add(deviceField5, null);
    jPanel1.add(jPanel6, null);
    jPanel6.add(deviceButtons1, null);
  }

}
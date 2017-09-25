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


import javax.swing.*;
import java.awt.*;
/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2003</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class FEGroupSetup extends DeviceSetup {
  JPanel jPanel1 = new JPanel();
  GridLayout gridLayout1 = new GridLayout();
  DeviceButtons deviceButtons1 = new DeviceButtons();
  JPanel jPanel2 = new JPanel();
  FlowLayout flowLayout1 = new FlowLayout();
  DeviceChoice deviceChoice2 = new DeviceChoice();
  DeviceDispatch deviceDispatch1 = new DeviceDispatch();
  JPanel jPanel3 = new JPanel();
  DeviceField deviceField1 = new DeviceField();
  FlowLayout flowLayout2 = new FlowLayout();
  public FEGroupSetup() {
    try {
      jbInit();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  private void jbInit() throws Exception {
    this.setWidth(380);
    this.setHeight(150);
    this.setDeviceType("FEGroup");
    this.setDeviceProvider("150.178.3.33");
    this.setDeviceTitle("ISIS Front End Gain");
    jPanel1.setLayout(gridLayout1);
    gridLayout1.setRows(3);
    jPanel2.setLayout(flowLayout1);
    deviceChoice2.setChoiceIntValues(null);
    deviceChoice2.setChoiceFloatValues(new float[] {(float)1.0,(float)2.0,(float)5.0,(float)10.0});
    deviceChoice2.setOffsetNid(3);
    deviceChoice2.setLabelString("Gain:");
    deviceChoice2.setChoiceItems(new String[] {"1", "2", "5", "10"});
    deviceChoice2.setUpdateIdentifier("");
    deviceChoice2.setIdentifier("");
    deviceField1.setOffsetNid(1);
    deviceField1.setLabelString("Comment:");
    deviceField1.setNumCols(20);
    deviceField1.setIdentifier("");
    jPanel3.setLayout(flowLayout2);
    flowLayout2.setAlignment(FlowLayout.LEFT);
    flowLayout1.setAlignment(FlowLayout.LEFT);
    deviceButtons1.setMethods(new String[] {"init"});
    this.getContentPane().add(jPanel1,  BorderLayout.CENTER);
    jPanel1.add(jPanel3, null);
    jPanel3.add(deviceField1, null);
    jPanel1.add(jPanel2, null);
    jPanel2.add(deviceChoice2, null);
    jPanel2.add(deviceDispatch1, null);
    jPanel1.add(deviceButtons1, null);
  }

}
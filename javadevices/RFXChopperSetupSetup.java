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

public class RFXChopperSetupSetup extends DeviceSetup {
  BorderLayout borderLayout1 = new BorderLayout();
  DeviceField deviceField1 = new DeviceField();
  DeviceButtons deviceButtons1 = new DeviceButtons();
  JPanel jPanel1 = new JPanel();
  DeviceWave deviceWave1 = new DeviceWave();
  DeviceTable deviceTable1 = new DeviceTable();
  GridLayout gridLayout1 = new GridLayout();
  public RFXChopperSetupSetup() {
    try {
      jbInit();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  private void jbInit() throws Exception {
    this.setWidth(542);
    this.setHeight(542);
    this.setDeviceType("RFXChopperSetup");
    this.setDeviceProvider("localhost");
    this.setDeviceTitle("EDA2 Chopper Setup");
    this.getContentPane().setLayout(borderLayout1);
    deviceField1.setOffsetNid(1);
    deviceField1.setTextOnly(true);
    deviceField1.setLabelString("Comment:");
    deviceField1.setNumCols(35);
    deviceField1.setIdentifier("");
    deviceWave1.setOffsetNid(3);
    deviceWave1.setIdentifier("Waveform: ");
    deviceWave1.setUpdateIdentifier("");
    deviceWave1.setUpdateExpression("");
    deviceTable1.setOffsetNid(8);
    deviceTable1.setLabelString("Unit calibration");
    deviceTable1.setNumCols(2);
    deviceTable1.setNumRows(12);
    deviceTable1.setIdentifier("");
    deviceTable1.setColumnNames(new String[] {"Gain", "Offset"});
    deviceTable1.setRowNames(new String[] {"Unit 1", "Unit 2", "Unit 3", "Unit 4", "Unit 5", "Unit 6", "Unit 7", "Unit 8", "Unit 9", "Unit 10", "Unit 11", "Unit 12"});
    deviceTable1.setDisplayRowNumber(true);
    jPanel1.setLayout(gridLayout1);
    gridLayout1.setColumns(1);
    gridLayout1.setRows(2);
    this.getContentPane().add(deviceField1, BorderLayout.NORTH);
    this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
    this.getContentPane().add(jPanel1, BorderLayout.CENTER);
    jPanel1.add(deviceWave1, null);
    jPanel1.add(deviceTable1, null);
  }

}
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

public class RFXWAVESETUPSetup extends DeviceSetup
{
  BorderLayout borderLayout1 = new BorderLayout();
  DeviceButtons deviceButtons1 = new DeviceButtons();
  JPanel jPanel1 = new JPanel();
  JTabbedPane jTabbedPane1 = new JTabbedPane();
  GridLayout gridLayout1 = new GridLayout();
  JPanel jPanel2 = new JPanel();
  DeviceField deviceField1 = new DeviceField();
  DeviceField deviceField2 = new DeviceField();
  DeviceField deviceField3 = new DeviceField();
  DeviceField deviceField4 = new DeviceField();
  DeviceField deviceField5 = new DeviceField();
  DeviceField deviceField6 = new DeviceField();
  DeviceField deviceField7 = new DeviceField();

  JPanel jPanel3 = new JPanel();
  JPanel jPanel4 = new JPanel();
  JPanel jPanel5 = new JPanel();
  JPanel jPanel6 = new JPanel();
  JPanel jPanel7 = new JPanel();
  JPanel jPanel8 = new JPanel();

  JPanel jPanel9 = new JPanel();
  JPanel jPanel10 = new JPanel();
  JPanel jPanel11 = new JPanel();
  JPanel jPanel12 = new JPanel();
  JPanel jPanel13 = new JPanel();
  JPanel jPanel14 = new JPanel();

  BorderLayout borderLayout2 = new BorderLayout();
  BorderLayout borderLayout3 = new BorderLayout();
  BorderLayout borderLayout4 = new BorderLayout();
  BorderLayout borderLayout5 = new BorderLayout();
  BorderLayout borderLayout6 = new BorderLayout();
  BorderLayout borderLayout7 = new BorderLayout();

  DeviceWave deviceWave1 = new DeviceWave();
  DeviceWave deviceWave2 = new DeviceWave();
  DeviceWave deviceWave3 = new DeviceWave();
  DeviceWave deviceWave4 = new DeviceWave();
  DeviceWave deviceWave5 = new DeviceWave();
  DeviceWave deviceWave6 = new DeviceWave();

  public RFXWAVESETUPSetup()
  {
    try
    {
      jbInit();
    }
    catch(Exception e)
    {
      e.printStackTrace();
    }
  }
  private void jbInit() throws Exception
  {
    this.setWidth(547);
    this.setHeight(547);
    this.setDeviceType("RFXWAVESETUPSetup");
    this.setDeviceProvider("localhost");
    this.setDeviceTitle("Waveform editor");
    this.getContentPane().setLayout(borderLayout1);
    jPanel1.setLayout(gridLayout1);
    gridLayout1.setColumns(1);
    gridLayout1.setRows(1);
    deviceField1.setOffsetNid(1);
    deviceField1.setTextOnly(true);
    deviceField1.setLabelString("Comment: ");
    deviceField1.setNumCols(30);
    deviceField1.setIdentifier("");

    deviceWave1.setOffsetNid(4);
//    deviceWave1.setMaxYVisible(false);
    deviceWave1.setIdentifier("");
    deviceWave1.setUpdateExpression("");
    deviceField2.setOffsetNid(3);
    deviceField2.setTextOnly(true);
    deviceField2.setLabelString("Comment: ");
    deviceField2.setNumCols(30);
    deviceField2.setIdentifier("");
    jPanel9.add(deviceField2, null);

    deviceWave2.setOffsetNid(11);
//    deviceWave2.setMaxYVisible(false);
    deviceWave2.setIdentifier("");
    deviceWave2.setUpdateExpression("");
    deviceField3.setOffsetNid(10);
    deviceField3.setTextOnly(true);
    deviceField3.setLabelString("Comment: ");
    deviceField3.setNumCols(30);
    deviceField3.setIdentifier("");
    jPanel10.add(deviceField3, null);

    deviceWave3.setOffsetNid(18);
//    deviceWave3.setMaxYVisible(false);
    deviceWave3.setIdentifier("");
    deviceWave3.setUpdateExpression("");
    deviceField4.setOffsetNid(17);
    deviceField4.setTextOnly(true);
    deviceField4.setLabelString("Comment: ");
    deviceField4.setNumCols(30);
    deviceField4.setIdentifier("");
    jPanel11.add(deviceField4, null);

    deviceWave4.setOffsetNid(25);
//    deviceWave4.setMaxYVisible(false);
    deviceWave4.setIdentifier("");
    deviceWave4.setUpdateExpression("");
    deviceField5.setOffsetNid(24);
    deviceField5.setTextOnly(true);
    deviceField5.setLabelString("Comment: ");
    deviceField5.setNumCols(30);
    deviceField5.setIdentifier("");
    jPanel12.add(deviceField5, null);

    deviceWave5.setOffsetNid(32);
//    deviceWave5.setMaxYVisible(false);
    deviceWave5.setIdentifier("");
    deviceWave5.setUpdateExpression("");
    deviceField6.setOffsetNid(31);
    deviceField6.setTextOnly(true);
    deviceField6.setLabelString("Comment: ");
    deviceField6.setNumCols(30);
    deviceField6.setIdentifier("");
    jPanel13.add(deviceField6, null);


    deviceWave6.setOffsetNid(39);
//    deviceWave6.setMaxYVisible(false);
    deviceWave6.setIdentifier("");
    deviceWave6.setUpdateExpression("");
    deviceField7.setOffsetNid(38);
    deviceField7.setTextOnly(true);
    deviceField7.setLabelString("Comment: ");
    deviceField7.setNumCols(30);
    deviceField7.setIdentifier("");
    jPanel14.add(deviceField7, null);



    jPanel3.setLayout(borderLayout2);
    jPanel4.setLayout(borderLayout3);
    jPanel5.setLayout(borderLayout4);
    jPanel6.setLayout(borderLayout5);
    jPanel7.setLayout(borderLayout6);
    jPanel8.setLayout(borderLayout7);

    this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
    this.getContentPane().add(jPanel1, BorderLayout.NORTH);
    jPanel1.add(jPanel2, null);
    jPanel2.add(deviceField1, null);
    this.getContentPane().add(jTabbedPane1, BorderLayout.CENTER);

    jPanel3.add(deviceWave1,  BorderLayout.CENTER);
    jPanel3.add(jPanel9, BorderLayout.NORTH);
    jPanel4.add(deviceWave2,  BorderLayout.CENTER);
    jPanel4.add(jPanel10, BorderLayout.NORTH);

    jPanel5.add(deviceWave3,  BorderLayout.CENTER);
    jPanel5.add(jPanel11, BorderLayout.NORTH);

    jPanel6.add(deviceWave4,  BorderLayout.CENTER);
    jPanel6.add(jPanel12, BorderLayout.NORTH);

    jPanel7.add(deviceWave5,  BorderLayout.CENTER);
    jPanel7.add(jPanel13, BorderLayout.NORTH);

    jPanel8.add(deviceWave6,  BorderLayout.CENTER);
    jPanel8.add(jPanel14, BorderLayout.NORTH);




    jTabbedPane1.add(jPanel3,   "WAVE 1");	
    jTabbedPane1.add(jPanel4,   "WAVE 2");
    jTabbedPane1.add(jPanel5,   "WAVE 3");	
    jTabbedPane1.add(jPanel6,   "WAVE 4");
    jTabbedPane1.add(jPanel7,   "WAVE 5");	
    jTabbedPane1.add(jPanel8,   "WAVE 6");


  }

}

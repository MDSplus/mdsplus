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

public class RFXAxiToroidalControlSetup extends DeviceSetup
{
    BorderLayout borderLayout1 = new BorderLayout();
    DeviceButtons deviceButtons1 = new DeviceButtons();
    JTabbedPane jTabbedPane1 = new JTabbedPane();
    JPanel jPanel1 = new JPanel();
    DeviceField deviceField1 = new DeviceField();
    DeviceWave deviceWave1 = new DeviceWave();
    DeviceWave deviceWave2 = new DeviceWave();
  DeviceWave deviceWave3 = new DeviceWave();
  DeviceWave deviceWave4 = new DeviceWave();
    public RFXAxiToroidalControlSetup()
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
      this.setWidth(471);
      this.setHeight(471);
      this.setDeviceType("RFXAxiToroidalControl");
      this.setDeviceProvider("localhost");
      this.setDeviceTitle("Toroidal Axisymmetric Control Setup");
      this.getContentPane().setLayout(borderLayout1);
      deviceField1.setOffsetNid(1);
      deviceField1.setTextOnly(true);
      deviceField1.setLabelString("Comment: ");
      deviceField1.setNumCols(30);
      deviceField1.setIdentifier("");
      deviceWave1.setOffsetNid(3);
      deviceWave1.setMinYVisible(true);
      deviceWave1.setIdentifier("");
      deviceWave1.setUpdateExpression("");
      deviceWave2.setOffsetNid(9);
      deviceWave2.setMinYVisible(true);
      deviceWave2.setIdentifier("");
      deviceWave2.setUpdateExpression("");
      deviceWave3.setOffsetNid(15);
        deviceWave3.setMinYVisible(true);
        deviceWave3.setIdentifier("");
        deviceWave3.setUpdateExpression("");
        deviceWave4.setUpdateExpression("");
    deviceWave4.setIdentifier("");
    deviceWave4.setMinYVisible(true);
    deviceWave4.setOffsetNid(21);
    this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
      this.getContentPane().add(jTabbedPane1, BorderLayout.CENTER);
      jTabbedPane1.add(deviceWave1,  "Wall Bt");
      jTabbedPane1.add(deviceWave2,  "F");
    jTabbedPane1.add(deviceWave3,  "Q");
    jTabbedPane1.add(deviceWave4,   "Flux");
      this.getContentPane().add(jPanel1, BorderLayout.NORTH);
      jPanel1.add(deviceField1, null);
    }

}

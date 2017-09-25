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
/*
 * EM_EQU_TEST_NEWSetup.java
 *
 * Created on September 16, 2009, 3:18 PM
 */



/**
 *
 * @author  taliercio
 */
public class EM_EQU_TEST_NEWSetup extends DeviceSetup { 

    /** Creates new form EM_EQU_TEST_NEWSetup */
    public EM_EQU_TEST_NEWSetup() {
        initComponents();
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        deviceButtons1 = new DeviceButtons();
        jPanel2 = new javax.swing.JPanel();
        jPanel3 = new javax.swing.JPanel();
        deviceField1 = new DeviceField();
        deviceDispatch1 = new DeviceDispatch();
        jPanel4 = new javax.swing.JPanel();
        deviceField2 = new DeviceField();
        deviceField3 = new DeviceField();
        deviceField4 = new DeviceField();
        jPanel6 = new javax.swing.JPanel();
        jLabel1 = new javax.swing.JLabel();
        jScrollPane1 = new javax.swing.JScrollPane();
        jPanel1 = new javax.swing.JPanel();
        jPanel5 = new javax.swing.JPanel();
        deviceChannel1 = new DeviceChannel();
        deviceField5 = new DeviceField();
        deviceField6 = new DeviceField();
        deviceField7 = new DeviceField();
        jPanel7 = new javax.swing.JPanel();
        deviceChannel2 = new DeviceChannel();
        deviceField8 = new DeviceField();
        deviceField9 = new DeviceField();
        deviceField10 = new DeviceField();
        jPanel8 = new javax.swing.JPanel();
        deviceChannel3 = new DeviceChannel();
        deviceField11 = new DeviceField();
        deviceField12 = new DeviceField();
        deviceField13 = new DeviceField();
        jPanel9 = new javax.swing.JPanel();
        deviceChannel4 = new DeviceChannel();
        deviceField14 = new DeviceField();
        deviceField15 = new DeviceField();
        deviceField16 = new DeviceField();
        jPanel10 = new javax.swing.JPanel();
        deviceChannel5 = new DeviceChannel();
        deviceField17 = new DeviceField();
        deviceField18 = new DeviceField();
        deviceField19 = new DeviceField();
        jPanel11 = new javax.swing.JPanel();
        deviceChannel6 = new DeviceChannel();
        deviceField20 = new DeviceField();
        deviceField21 = new DeviceField();
        deviceField22 = new DeviceField();
        jPanel12 = new javax.swing.JPanel();
        deviceChannel7 = new DeviceChannel();
        deviceField23 = new DeviceField();
        deviceField24 = new DeviceField();
        deviceField25 = new DeviceField();
        jPanel13 = new javax.swing.JPanel();
        deviceChannel8 = new DeviceChannel();
        deviceField26 = new DeviceField();
        deviceField27 = new DeviceField();
        deviceField28 = new DeviceField();
        jPanel14 = new javax.swing.JPanel();
        deviceChannel9 = new DeviceChannel();
        deviceField29 = new DeviceField();
        deviceField30 = new DeviceField();
        deviceField31 = new DeviceField();
        jPanel15 = new javax.swing.JPanel();
        deviceChannel10 = new DeviceChannel();
        deviceField32 = new DeviceField();
        deviceField33 = new DeviceField();
        deviceField34 = new DeviceField();
        jPanel16 = new javax.swing.JPanel();
        deviceChannel11 = new DeviceChannel();
        deviceField35 = new DeviceField();
        deviceField36 = new DeviceField();
        deviceField37 = new DeviceField();
        jPanel17 = new javax.swing.JPanel();
        deviceChannel12 = new DeviceChannel();
        deviceField38 = new DeviceField();
        deviceField39 = new DeviceField();
        deviceField40 = new DeviceField();

        setDeviceProvider("localhost");
        setDeviceTitle("EQU Test Device");
        setDeviceType("EM_EQU_TEST_NEW");
        getContentPane().add(deviceButtons1, java.awt.BorderLayout.SOUTH);

        jPanel2.setLayout(new java.awt.GridLayout(3, 0));

        jPanel3.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceField1.setIdentifier("");
        deviceField1.setLabelString("Comment : ");
        deviceField1.setNumCols(22);
        deviceField1.setOffsetNid(1);
        deviceField1.setTextOnly(true);
        jPanel3.add(deviceField1);
        jPanel3.add(deviceDispatch1);

        jPanel2.add(jPanel3);

        jPanel4.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 5));

        deviceField2.setIdentifier("");
        deviceField2.setLabelString("Decoder : ");
        deviceField2.setOffsetNid(2);
        jPanel4.add(deviceField2);

        deviceField3.setIdentifier("");
        deviceField3.setLabelString("Signal Test : Delay");
        deviceField3.setOffsetNid(3);
        jPanel4.add(deviceField3);

        deviceField4.setIdentifier("");
        deviceField4.setLabelString("Duration");
        deviceField4.setOffsetNid(4);
        jPanel4.add(deviceField4);

        jPanel2.add(jPanel4);

        jPanel6.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        jLabel1.setText("Card                        GAIN                                              Linear ADC                                              Integral ADC");
        jPanel6.add(jLabel1);

        jPanel2.add(jPanel6);

        getContentPane().add(jPanel2, java.awt.BorderLayout.NORTH);

        jPanel1.setLayout(new java.awt.GridLayout(12, 0));

        jPanel5.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceChannel1.setInSameLine(true);
        deviceChannel1.setLabelString("01");
        deviceChannel1.setOffsetNid(6);
        deviceChannel1.setShowVal("");
        deviceChannel1.setUpdateIdentifier("");
        deviceChannel1.getContainer().setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceField5.setIdentifier("");
        deviceField5.setNumCols(17);
        deviceField5.setOffsetNid(7);
        deviceChannel1.getContainer().add(deviceField5);

        deviceField6.setIdentifier("");
        deviceField6.setNumCols(17);
        deviceField6.setOffsetNid(8);
        deviceChannel1.getContainer().add(deviceField6);

        deviceField7.setIdentifier("");
        deviceField7.setNumCols(17);
        deviceField7.setOffsetNid(9);
        deviceChannel1.getContainer().add(deviceField7);

        jPanel5.add(deviceChannel1);

        jPanel1.add(jPanel5);

        jPanel7.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceChannel2.setInSameLine(true);
        deviceChannel2.setLabelString("02");
        deviceChannel2.setOffsetNid(10);
        deviceChannel2.setShowVal("");
        deviceChannel2.setUpdateIdentifier("");
        deviceChannel2.getContainer().setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceField8.setIdentifier("");
        deviceField8.setNumCols(17);
        deviceField8.setOffsetNid(11);
        deviceChannel2.getContainer().add(deviceField8);

        deviceField9.setIdentifier("");
        deviceField9.setNumCols(17);
        deviceField9.setOffsetNid(12);
        deviceChannel2.getContainer().add(deviceField9);

        deviceField10.setIdentifier("");
        deviceField10.setNumCols(17);
        deviceField10.setOffsetNid(13);
        deviceChannel2.getContainer().add(deviceField10);

        jPanel7.add(deviceChannel2);

        jPanel1.add(jPanel7);

        jPanel8.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceChannel3.setInSameLine(true);
        deviceChannel3.setLabelString("03");
        deviceChannel3.setOffsetNid(14);
        deviceChannel3.setShowVal("");
        deviceChannel3.setUpdateIdentifier("");
        deviceChannel3.getContainer().setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceField11.setIdentifier("");
        deviceField11.setNumCols(17);
        deviceField11.setOffsetNid(15);
        deviceChannel3.getContainer().add(deviceField11);

        deviceField12.setIdentifier("");
        deviceField12.setNumCols(17);
        deviceField12.setOffsetNid(16);
        deviceChannel3.getContainer().add(deviceField12);

        deviceField13.setIdentifier("");
        deviceField13.setNumCols(17);
        deviceField13.setOffsetNid(17);
        deviceChannel3.getContainer().add(deviceField13);

        jPanel8.add(deviceChannel3);

        jPanel1.add(jPanel8);

        jPanel9.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceChannel4.setInSameLine(true);
        deviceChannel4.setLabelString("04");
        deviceChannel4.setOffsetNid(18);
        deviceChannel4.setShowVal("");
        deviceChannel4.setUpdateIdentifier("");
        deviceChannel4.getContainer().setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceField14.setIdentifier("");
        deviceField14.setNumCols(17);
        deviceField14.setOffsetNid(19);
        deviceChannel4.getContainer().add(deviceField14);

        deviceField15.setIdentifier("");
        deviceField15.setNumCols(17);
        deviceField15.setOffsetNid(20);
        deviceChannel4.getContainer().add(deviceField15);

        deviceField16.setIdentifier("");
        deviceField16.setNumCols(17);
        deviceField16.setOffsetNid(21);
        deviceChannel4.getContainer().add(deviceField16);

        jPanel9.add(deviceChannel4);

        jPanel1.add(jPanel9);

        jPanel10.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceChannel5.setInSameLine(true);
        deviceChannel5.setLabelString("05");
        deviceChannel5.setOffsetNid(22);
        deviceChannel5.setShowVal("");
        deviceChannel5.setUpdateIdentifier("");
        deviceChannel5.getContainer().setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceField17.setIdentifier("");
        deviceField17.setNumCols(17);
        deviceField17.setOffsetNid(23);
        deviceChannel5.getContainer().add(deviceField17);

        deviceField18.setIdentifier("");
        deviceField18.setNumCols(17);
        deviceField18.setOffsetNid(24);
        deviceChannel5.getContainer().add(deviceField18);

        deviceField19.setIdentifier("");
        deviceField19.setNumCols(17);
        deviceField19.setOffsetNid(25);
        deviceChannel5.getContainer().add(deviceField19);

        jPanel10.add(deviceChannel5);

        jPanel1.add(jPanel10);

        jPanel11.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceChannel6.setInSameLine(true);
        deviceChannel6.setLabelString("06");
        deviceChannel6.setOffsetNid(26);
        deviceChannel6.setShowVal("");
        deviceChannel6.setUpdateIdentifier("");
        deviceChannel6.getContainer().setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceField20.setIdentifier("");
        deviceField20.setNumCols(17);
        deviceField20.setOffsetNid(27);
        deviceChannel6.getContainer().add(deviceField20);

        deviceField21.setIdentifier("");
        deviceField21.setNumCols(17);
        deviceField21.setOffsetNid(28);
        deviceChannel6.getContainer().add(deviceField21);

        deviceField22.setIdentifier("");
        deviceField22.setNumCols(17);
        deviceField22.setOffsetNid(29);
        deviceChannel6.getContainer().add(deviceField22);

        jPanel11.add(deviceChannel6);

        jPanel1.add(jPanel11);

        jPanel12.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceChannel7.setInSameLine(true);
        deviceChannel7.setLabelString("07");
        deviceChannel7.setOffsetNid(30);
        deviceChannel7.setShowVal("");
        deviceChannel7.setUpdateIdentifier("");
        deviceChannel7.getContainer().setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceField23.setIdentifier("");
        deviceField23.setNumCols(17);
        deviceField23.setOffsetNid(31);
        deviceChannel7.getContainer().add(deviceField23);

        deviceField24.setIdentifier("");
        deviceField24.setNumCols(17);
        deviceField24.setOffsetNid(32);
        deviceChannel7.getContainer().add(deviceField24);

        deviceField25.setIdentifier("");
        deviceField25.setNumCols(17);
        deviceField25.setOffsetNid(33);
        deviceChannel7.getContainer().add(deviceField25);

        jPanel12.add(deviceChannel7);

        jPanel1.add(jPanel12);

        jPanel13.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceChannel8.setInSameLine(true);
        deviceChannel8.setLabelString("08");
        deviceChannel8.setOffsetNid(34);
        deviceChannel8.setShowVal("");
        deviceChannel8.setUpdateIdentifier("");
        deviceChannel8.getContainer().setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceField26.setIdentifier("");
        deviceField26.setNumCols(17);
        deviceField26.setOffsetNid(35);
        deviceChannel8.getContainer().add(deviceField26);

        deviceField27.setIdentifier("");
        deviceField27.setNumCols(17);
        deviceField27.setOffsetNid(36);
        deviceChannel8.getContainer().add(deviceField27);

        deviceField28.setIdentifier("");
        deviceField28.setNumCols(17);
        deviceField28.setOffsetNid(37);
        deviceChannel8.getContainer().add(deviceField28);

        jPanel13.add(deviceChannel8);

        jPanel1.add(jPanel13);

        jPanel14.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceChannel9.setInSameLine(true);
        deviceChannel9.setLabelString("09");
        deviceChannel9.setOffsetNid(38);
        deviceChannel9.setShowVal("");
        deviceChannel9.setUpdateIdentifier("");
        deviceChannel9.getContainer().setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceField29.setIdentifier("");
        deviceField29.setNumCols(17);
        deviceField29.setOffsetNid(39);
        deviceChannel9.getContainer().add(deviceField29);

        deviceField30.setIdentifier("");
        deviceField30.setNumCols(17);
        deviceField30.setOffsetNid(40);
        deviceChannel9.getContainer().add(deviceField30);

        deviceField31.setIdentifier("");
        deviceField31.setNumCols(17);
        deviceField31.setOffsetNid(41);
        deviceChannel9.getContainer().add(deviceField31);

        jPanel14.add(deviceChannel9);

        jPanel1.add(jPanel14);

        jPanel15.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceChannel10.setInSameLine(true);
        deviceChannel10.setLabelString("10");
        deviceChannel10.setOffsetNid(42);
        deviceChannel10.setShowVal("");
        deviceChannel10.setUpdateIdentifier("");
        deviceChannel10.getContainer().setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceField32.setIdentifier("");
        deviceField32.setNumCols(17);
        deviceField32.setOffsetNid(43);
        deviceChannel10.getContainer().add(deviceField32);

        deviceField33.setIdentifier("");
        deviceField33.setNumCols(17);
        deviceField33.setOffsetNid(44);
        deviceChannel10.getContainer().add(deviceField33);

        deviceField34.setIdentifier("");
        deviceField34.setNumCols(17);
        deviceField34.setOffsetNid(45);
        deviceChannel10.getContainer().add(deviceField34);

        jPanel15.add(deviceChannel10);

        jPanel1.add(jPanel15);

        jPanel16.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceChannel11.setInSameLine(true);
        deviceChannel11.setLabelString("11");
        deviceChannel11.setOffsetNid(46);
        deviceChannel11.setShowVal("");
        deviceChannel11.setUpdateIdentifier("");
        deviceChannel11.getContainer().setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceField35.setIdentifier("");
        deviceField35.setNumCols(17);
        deviceField35.setOffsetNid(47);
        deviceChannel11.getContainer().add(deviceField35);

        deviceField36.setIdentifier("");
        deviceField36.setNumCols(17);
        deviceField36.setOffsetNid(48);
        deviceChannel11.getContainer().add(deviceField36);

        deviceField37.setIdentifier("");
        deviceField37.setNumCols(17);
        deviceField37.setOffsetNid(49);
        deviceChannel11.getContainer().add(deviceField37);

        jPanel16.add(deviceChannel11);

        jPanel1.add(jPanel16);

        jPanel17.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceChannel12.setInSameLine(true);
        deviceChannel12.setLabelString("12");
        deviceChannel12.setOffsetNid(50);
        deviceChannel12.setShowVal("");
        deviceChannel12.setUpdateIdentifier("");
        deviceChannel12.getContainer().setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT, 0, 0));

        deviceField38.setIdentifier("");
        deviceField38.setNumCols(17);
        deviceField38.setOffsetNid(51);
        deviceChannel12.getContainer().add(deviceField38);

        deviceField39.setIdentifier("");
        deviceField39.setNumCols(17);
        deviceField39.setOffsetNid(52);
        deviceChannel12.getContainer().add(deviceField39);

        deviceField40.setIdentifier("");
        deviceField40.setNumCols(17);
        deviceField40.setOffsetNid(53);
        deviceChannel12.getContainer().add(deviceField40);

        jPanel17.add(deviceChannel12);

        jPanel1.add(jPanel17);

        jScrollPane1.setViewportView(jPanel1);

        getContentPane().add(jScrollPane1, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private DeviceButtons deviceButtons1;
    private DeviceChannel deviceChannel1;
    private DeviceChannel deviceChannel10;
    private DeviceChannel deviceChannel11;
    private DeviceChannel deviceChannel12;
    private DeviceChannel deviceChannel2;
    private DeviceChannel deviceChannel3;
    private DeviceChannel deviceChannel4;
    private DeviceChannel deviceChannel5;
    private DeviceChannel deviceChannel6;
    private DeviceChannel deviceChannel7;
    private DeviceChannel deviceChannel8;
    private DeviceChannel deviceChannel9;
    private DeviceDispatch deviceDispatch1;
    private DeviceField deviceField1;
    private DeviceField deviceField10;
    private DeviceField deviceField11;
    private DeviceField deviceField12;
    private DeviceField deviceField13;
    private DeviceField deviceField14;
    private DeviceField deviceField15;
    private DeviceField deviceField16;
    private DeviceField deviceField17;
    private DeviceField deviceField18;
    private DeviceField deviceField19;
    private DeviceField deviceField2;
    private DeviceField deviceField20;
    private DeviceField deviceField21;
    private DeviceField deviceField22;
    private DeviceField deviceField23;
    private DeviceField deviceField24;
    private DeviceField deviceField25;
    private DeviceField deviceField26;
    private DeviceField deviceField27;
    private DeviceField deviceField28;
    private DeviceField deviceField29;
    private DeviceField deviceField3;
    private DeviceField deviceField30;
    private DeviceField deviceField31;
    private DeviceField deviceField32;
    private DeviceField deviceField33;
    private DeviceField deviceField34;
    private DeviceField deviceField35;
    private DeviceField deviceField36;
    private DeviceField deviceField37;
    private DeviceField deviceField38;
    private DeviceField deviceField39;
    private DeviceField deviceField4;
    private DeviceField deviceField40;
    private DeviceField deviceField5;
    private DeviceField deviceField6;
    private DeviceField deviceField7;
    private DeviceField deviceField8;
    private DeviceField deviceField9;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel10;
    private javax.swing.JPanel jPanel11;
    private javax.swing.JPanel jPanel12;
    private javax.swing.JPanel jPanel13;
    private javax.swing.JPanel jPanel14;
    private javax.swing.JPanel jPanel15;
    private javax.swing.JPanel jPanel16;
    private javax.swing.JPanel jPanel17;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JPanel jPanel4;
    private javax.swing.JPanel jPanel5;
    private javax.swing.JPanel jPanel6;
    private javax.swing.JPanel jPanel7;
    private javax.swing.JPanel jPanel8;
    private javax.swing.JPanel jPanel9;
    private javax.swing.JScrollPane jScrollPane1;
    // End of variables declaration//GEN-END:variables

}

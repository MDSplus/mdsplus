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
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * CAENDT5720.java
 *
 * Created on Jan 12, 2011, 4:15:14 PM
 */

/**
 *
 * @author manduchi
 */
public class CAENDT5724Setup extends DeviceSetup {

    /** Creates new form CAENDT5724 */
    public CAENDT5724Setup() {
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
        jPanel1 = new javax.swing.JPanel();
        jPanel26 = new javax.swing.JPanel();
        deviceField1 = new DeviceField();
        deviceField3 = new DeviceField();
        deviceDispatch1 = new DeviceDispatch();
        jPanel27 = new javax.swing.JPanel();
        deviceChoice15 = new DeviceChoice();
        deviceField2 = new DeviceField();
        jPanel2 = new javax.swing.JPanel();
        jPanel3 = new javax.swing.JPanel();
        jPanel6 = new javax.swing.JPanel();
        jPanel10 = new javax.swing.JPanel();
        deviceChoice4 = new DeviceChoice();
        jPanel11 = new javax.swing.JPanel();
        deviceField5 = new DeviceField();
        jPanel8 = new javax.swing.JPanel();
        jPanel12 = new javax.swing.JPanel();
        deviceChoice5 = new DeviceChoice();
        deviceField6 = new DeviceField();
        deviceChoice6 = new DeviceChoice();
        jPanel13 = new javax.swing.JPanel();
        deviceField7 = new DeviceField();
        deviceField8 = new DeviceField();
        deviceField9 = new DeviceField();
        deviceField10 = new DeviceField();
        jPanel4 = new javax.swing.JPanel();
        jPanel7 = new javax.swing.JPanel();
        deviceChoice1 = new DeviceChoice();
        deviceChoice2 = new DeviceChoice();
        deviceChoice3 = new DeviceChoice();
        jPanel9 = new javax.swing.JPanel();
        deviceField4 = new DeviceField();
        jPanel5 = new javax.swing.JPanel();
        jTabbedPane1 = new javax.swing.JTabbedPane();
        jPanel14 = new javax.swing.JPanel();
        jPanel15 = new javax.swing.JPanel();
        deviceChoice7 = new DeviceChoice();
        deviceChoice8 = new DeviceChoice();
        deviceField11 = new DeviceField();
        jPanel16 = new javax.swing.JPanel();
        deviceField12 = new DeviceField();
        deviceField13 = new DeviceField();
        jPanel17 = new javax.swing.JPanel();
        jPanel18 = new javax.swing.JPanel();
        deviceChoice9 = new DeviceChoice();
        deviceChoice10 = new DeviceChoice();
        deviceField14 = new DeviceField();
        jPanel19 = new javax.swing.JPanel();
        deviceField15 = new DeviceField();
        deviceField16 = new DeviceField();
        jPanel20 = new javax.swing.JPanel();
        jPanel21 = new javax.swing.JPanel();
        deviceChoice11 = new DeviceChoice();
        deviceChoice12 = new DeviceChoice();
        deviceField17 = new DeviceField();
        jPanel22 = new javax.swing.JPanel();
        deviceField18 = new DeviceField();
        deviceField19 = new DeviceField();
        jPanel23 = new javax.swing.JPanel();
        jPanel24 = new javax.swing.JPanel();
        deviceChoice13 = new DeviceChoice();
        deviceChoice14 = new DeviceChoice();
        deviceField20 = new DeviceField();
        jPanel25 = new javax.swing.JPanel();
        deviceField21 = new DeviceField();
        deviceField22 = new DeviceField();

        setDeviceProvider("localhost");
        setDeviceTitle("CAEN DT5724");
        setDeviceType("CAENDT5724");
        setHeight(650);
        setWidth(700);
        getContentPane().add(deviceButtons1, java.awt.BorderLayout.PAGE_END);

        jPanel1.setLayout(new java.awt.GridLayout(2, 1));

        deviceField1.setIdentifier("");
        deviceField1.setLabelString("Comment");
        deviceField1.setNumCols(15);
        deviceField1.setOffsetNid(2);
        deviceField1.setTextOnly(true);
        jPanel26.add(deviceField1);

        deviceField3.setIdentifier("");
        deviceField3.setLabelString("Board Id");
        deviceField3.setNumCols(4);
        deviceField3.setOffsetNid(1);
        jPanel26.add(deviceField3);
        jPanel26.add(deviceDispatch1);

        jPanel1.add(jPanel26);

        deviceChoice15.setChoiceItems(new String[] {"TRANSIENT RECORDER", "CONTINUOUS", "CONTINUOUS WITH COUNTER"});
        deviceChoice15.setIdentifier("");
        deviceChoice15.setLabelString("Acquisiton mode: ");
        deviceChoice15.setOffsetNid(16);
        deviceChoice15.setUpdateIdentifier("");
        jPanel27.add(deviceChoice15);

        deviceField2.setIdentifier("");
        deviceField2.setLabelString("Events per IRQ:");
        deviceField2.setOffsetNid(17);
        jPanel27.add(deviceField2);

        jPanel1.add(jPanel27);

        getContentPane().add(jPanel1, java.awt.BorderLayout.PAGE_START);

        jPanel2.setLayout(new java.awt.BorderLayout());

        jPanel3.setLayout(new java.awt.BorderLayout());

        jPanel6.setBorder(javax.swing.BorderFactory.createTitledBorder("Clock"));
        jPanel6.setLayout(new java.awt.GridLayout(2, 0));

        deviceChoice4.setChoiceItems(new String[] {"100 MHz", "EXTERNAL"});
        deviceChoice4.setIdentifier("");
        deviceChoice4.setLabelString("Clock Mode: ");
        deviceChoice4.setOffsetNid(7);
        deviceChoice4.setUpdateIdentifier("");
        jPanel10.add(deviceChoice4);

        jPanel6.add(jPanel10);

        deviceField5.setIdentifier("");
        deviceField5.setLabelString("Clock Source: ");
        deviceField5.setNumCols(30);
        deviceField5.setOffsetNid(8);
        jPanel11.add(deviceField5);

        jPanel6.add(jPanel11);

        jPanel3.add(jPanel6, java.awt.BorderLayout.CENTER);

        jPanel8.setBorder(javax.swing.BorderFactory.createTitledBorder("Data"));
        jPanel8.setLayout(new java.awt.GridLayout(2, 0));

        deviceChoice5.setChoiceItems(new String[] {"YES", "NO"});
        deviceChoice5.setIdentifier("");
        deviceChoice5.setLabelString("Use Time: ");
        deviceChoice5.setOffsetNid(10);
        deviceChoice5.setUpdateIdentifier("");
        jPanel12.add(deviceChoice5);

        deviceField6.setIdentifier("");
        deviceField6.setLabelString("PTS");
        deviceField6.setOffsetNid(11);
        jPanel12.add(deviceField6);

        deviceChoice6.setChoiceIntValues(new int[] {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, -1});
        deviceChoice6.setChoiceItems(new String[] {"1", "2", "4", "8", "16", "32", "64", "128", "256", "512", "1024", "-1"});
        deviceChoice6.setIdentifier("");
        deviceChoice6.setLabelString("Num Segments: ");
        deviceChoice6.setOffsetNid(9);
        deviceChoice6.setUpdateIdentifier("");
        jPanel12.add(deviceChoice6);

        jPanel8.add(jPanel12);

        deviceField7.setIdentifier("");
        deviceField7.setLabelString("Start Time:");
        deviceField7.setNumCols(8);
        deviceField7.setOffsetNid(14);
        jPanel13.add(deviceField7);

        deviceField8.setIdentifier("");
        deviceField8.setLabelString("End Time: ");
        deviceField8.setNumCols(8);
        deviceField8.setOffsetNid(15);
        jPanel13.add(deviceField8);

        deviceField9.setIdentifier("");
        deviceField9.setLabelString("Start Idx:  ");
        deviceField9.setNumCols(8);
        deviceField9.setOffsetNid(12);
        jPanel13.add(deviceField9);

        deviceField10.setIdentifier("");
        deviceField10.setLabelString("End Idx: ");
        deviceField10.setNumCols(8);
        deviceField10.setOffsetNid(13);
        jPanel13.add(deviceField10);

        jPanel8.add(jPanel13);

        jPanel3.add(jPanel8, java.awt.BorderLayout.PAGE_END);

        jPanel2.add(jPanel3, java.awt.BorderLayout.CENTER);

        jPanel4.setBorder(javax.swing.BorderFactory.createTitledBorder("Trigger"));
        jPanel4.setLayout(new java.awt.GridLayout(2, 0));

        deviceChoice1.setChoiceItems(new String[] {"OVER THRESHOLD", "UNDER THRESHOLD"});
        deviceChoice1.setIdentifier("");
        deviceChoice1.setLabelString("Mode: ");
        deviceChoice1.setOffsetNid(3);
        deviceChoice1.setUpdateIdentifier("");
        jPanel7.add(deviceChoice1);

        deviceChoice2.setChoiceItems(new String[] {"ENABLED", "DISABLED"});
        deviceChoice2.setIdentifier("");
        deviceChoice2.setLabelString("Soft Trig.: ");
        deviceChoice2.setOffsetNid(4);
        deviceChoice2.setUpdateIdentifier("");
        jPanel7.add(deviceChoice2);

        deviceChoice3.setChoiceItems(new String[] {"ENABLED", "DISABLED"});
        deviceChoice3.setIdentifier("");
        deviceChoice3.setLabelString("Ext Trig.: ");
        deviceChoice3.setOffsetNid(5);
        deviceChoice3.setUpdateIdentifier("");
        jPanel7.add(deviceChoice3);

        jPanel4.add(jPanel7);

        deviceField4.setIdentifier("");
        deviceField4.setLabelString("Trig Time: ");
        deviceField4.setNumCols(30);
        deviceField4.setOffsetNid(6);
        jPanel9.add(deviceField4);

        jPanel4.add(jPanel9);

        jPanel2.add(jPanel4, java.awt.BorderLayout.PAGE_START);

        jPanel5.setBorder(javax.swing.BorderFactory.createTitledBorder("Channels"));
        jPanel5.setLayout(new java.awt.BorderLayout());

        jPanel14.setLayout(new java.awt.GridLayout(2, 0));

        deviceChoice7.setChoiceItems(new String[] {"ENABLED", "DISABLED"});
        deviceChoice7.setIdentifier("");
        deviceChoice7.setLabelString("State");
        deviceChoice7.setOffsetNid(19);
        deviceChoice7.setUpdateIdentifier("");
        jPanel15.add(deviceChoice7);

        deviceChoice8.setChoiceItems(new String[] {"ENABLED", "DISABLED"});
        deviceChoice8.setIdentifier("");
        deviceChoice8.setLabelString("Trig State: ");
        deviceChoice8.setOffsetNid(20);
        deviceChoice8.setUpdateIdentifier("");
        jPanel15.add(deviceChoice8);

        deviceField11.setIdentifier("");
        deviceField11.setLabelString("DAC Offset: ");
        deviceField11.setOffsetNid(22);
        jPanel15.add(deviceField11);

        jPanel14.add(jPanel15);

        deviceField12.setIdentifier("");
        deviceField12.setLabelString("Trig Thresh. Lev.:");
        deviceField12.setOffsetNid(23);
        jPanel16.add(deviceField12);

        deviceField13.setIdentifier("");
        deviceField13.setLabelString("Trig Tresh. Samples:");
        deviceField13.setOffsetNid(33);
        jPanel16.add(deviceField13);

        jPanel14.add(jPanel16);

        jTabbedPane1.addTab("Chan 1", jPanel14);

        jPanel17.setLayout(new java.awt.GridLayout(2, 0));

        deviceChoice9.setChoiceItems(new String[] {"ENABLED", "DISABLED"});
        deviceChoice9.setIdentifier("");
        deviceChoice9.setLabelString("State");
        deviceChoice9.setOffsetNid(28);
        deviceChoice9.setUpdateIdentifier("");
        jPanel18.add(deviceChoice9);

        deviceChoice10.setChoiceItems(new String[] {"ENABLED", "DISABLED"});
        deviceChoice10.setIdentifier("");
        deviceChoice10.setLabelString("Trig State: ");
        deviceChoice10.setOffsetNid(29);
        deviceChoice10.setUpdateIdentifier("");
        jPanel18.add(deviceChoice10);

        deviceField14.setIdentifier("");
        deviceField14.setLabelString("DAC Offset: ");
        deviceField14.setOffsetNid(31);
        jPanel18.add(deviceField14);

        jPanel17.add(jPanel18);

        deviceField15.setIdentifier("");
        deviceField15.setLabelString("Trig Thresh. Lev.:");
        deviceField15.setOffsetNid(32);
        jPanel19.add(deviceField15);

        deviceField16.setIdentifier("");
        deviceField16.setLabelString("Trig Tresh. Samples:");
        deviceField16.setOffsetNid(33);
        jPanel19.add(deviceField16);

        jPanel17.add(jPanel19);

        jTabbedPane1.addTab("Chan 2", jPanel17);

        jPanel20.setLayout(new java.awt.GridLayout(2, 0));

        deviceChoice11.setChoiceItems(new String[] {"ENABLED", "DISABLED"});
        deviceChoice11.setIdentifier("");
        deviceChoice11.setLabelString("State");
        deviceChoice11.setOffsetNid(37);
        deviceChoice11.setUpdateIdentifier("");
        jPanel21.add(deviceChoice11);

        deviceChoice12.setChoiceItems(new String[] {"ENABLED", "DISABLED"});
        deviceChoice12.setIdentifier("");
        deviceChoice12.setLabelString("Trig State: ");
        deviceChoice12.setOffsetNid(38);
        deviceChoice12.setUpdateIdentifier("");
        jPanel21.add(deviceChoice12);

        deviceField17.setIdentifier("");
        deviceField17.setLabelString("DAC Offset: ");
        deviceField17.setOffsetNid(40);
        jPanel21.add(deviceField17);

        jPanel20.add(jPanel21);

        deviceField18.setIdentifier("");
        deviceField18.setLabelString("Trig Thresh. Lev.:");
        deviceField18.setOffsetNid(41);
        jPanel22.add(deviceField18);

        deviceField19.setIdentifier("");
        deviceField19.setLabelString("Trig Tresh. Samples:");
        deviceField19.setOffsetNid(42);
        jPanel22.add(deviceField19);

        jPanel20.add(jPanel22);

        jTabbedPane1.addTab("Chan 3", jPanel20);

        jPanel23.setLayout(new java.awt.GridLayout(2, 0));

        deviceChoice13.setChoiceItems(new String[] {"ENABLED", "DISABLED"});
        deviceChoice13.setIdentifier("");
        deviceChoice13.setLabelString("State");
        deviceChoice13.setOffsetNid(46);
        deviceChoice13.setUpdateIdentifier("");
        jPanel24.add(deviceChoice13);

        deviceChoice14.setChoiceItems(new String[] {"ENABLED", "DISABLED"});
        deviceChoice14.setIdentifier("");
        deviceChoice14.setLabelString("Trig State: ");
        deviceChoice14.setOffsetNid(47);
        deviceChoice14.setUpdateIdentifier("");
        jPanel24.add(deviceChoice14);

        deviceField20.setIdentifier("");
        deviceField20.setLabelString("DAC Offset: ");
        deviceField20.setOffsetNid(49);
        jPanel24.add(deviceField20);

        jPanel23.add(jPanel24);

        deviceField21.setIdentifier("");
        deviceField21.setLabelString("Trig Thresh. Lev.:");
        deviceField21.setOffsetNid(50);
        jPanel25.add(deviceField21);

        deviceField22.setIdentifier("");
        deviceField22.setLabelString("Trig Tresh. Samples:");
        deviceField22.setOffsetNid(51);
        jPanel25.add(deviceField22);

        jPanel23.add(jPanel25);

        jTabbedPane1.addTab("Chan 4", jPanel23);

        jPanel5.add(jTabbedPane1, java.awt.BorderLayout.CENTER);

        jPanel2.add(jPanel5, java.awt.BorderLayout.PAGE_END);

        getContentPane().add(jPanel2, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private DeviceButtons deviceButtons1;
    private DeviceChoice deviceChoice1;
    private DeviceChoice deviceChoice10;
    private DeviceChoice deviceChoice11;
    private DeviceChoice deviceChoice12;
    private DeviceChoice deviceChoice13;
    private DeviceChoice deviceChoice14;
    private DeviceChoice deviceChoice15;
    private DeviceChoice deviceChoice2;
    private DeviceChoice deviceChoice3;
    private DeviceChoice deviceChoice4;
    private DeviceChoice deviceChoice5;
    private DeviceChoice deviceChoice6;
    private DeviceChoice deviceChoice7;
    private DeviceChoice deviceChoice8;
    private DeviceChoice deviceChoice9;
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
    private DeviceField deviceField3;
    private DeviceField deviceField4;
    private DeviceField deviceField5;
    private DeviceField deviceField6;
    private DeviceField deviceField7;
    private DeviceField deviceField8;
    private DeviceField deviceField9;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel10;
    private javax.swing.JPanel jPanel11;
    private javax.swing.JPanel jPanel12;
    private javax.swing.JPanel jPanel13;
    private javax.swing.JPanel jPanel14;
    private javax.swing.JPanel jPanel15;
    private javax.swing.JPanel jPanel16;
    private javax.swing.JPanel jPanel17;
    private javax.swing.JPanel jPanel18;
    private javax.swing.JPanel jPanel19;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel20;
    private javax.swing.JPanel jPanel21;
    private javax.swing.JPanel jPanel22;
    private javax.swing.JPanel jPanel23;
    private javax.swing.JPanel jPanel24;
    private javax.swing.JPanel jPanel25;
    private javax.swing.JPanel jPanel26;
    private javax.swing.JPanel jPanel27;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JPanel jPanel4;
    private javax.swing.JPanel jPanel5;
    private javax.swing.JPanel jPanel6;
    private javax.swing.JPanel jPanel7;
    private javax.swing.JPanel jPanel8;
    private javax.swing.JPanel jPanel9;
    private javax.swing.JTabbedPane jTabbedPane1;
    // End of variables declaration//GEN-END:variables

}

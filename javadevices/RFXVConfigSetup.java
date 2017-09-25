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

import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.rmi.RemoteException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JCheckBox;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * RFXVConfigSetup.java
 *
 * Created on Dec 29, 2010, 3:19:31 PM
 */

/**
 *
 * @author taliercio
 */
public class RFXVConfigSetup extends DeviceSetup {

    /** Creates new form RFXVConfigSetup */
    public RFXVConfigSetup() {
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

        buttonGroup1 = new javax.swing.ButtonGroup();
        buttonGroup2 = new javax.swing.ButtonGroup();
        buttonGroup3 = new javax.swing.ButtonGroup();
        jPanel1 = new javax.swing.JPanel();
        deviceButtons1 = new DeviceButtons();
        jPanel2 = new javax.swing.JPanel();
        jPanel3 = new javax.swing.JPanel();
        deviceField1 = new DeviceField();
        deviceDispatch1 = new DeviceDispatch();
        jPanel4 = new javax.swing.JPanel();
        jPanel33 = new javax.swing.JPanel();
        deviceField3 = new DeviceField();
        deviceField5 = new DeviceField();
        jPanel5 = new javax.swing.JPanel();
        jPanel31 = new javax.swing.JPanel();
        deviceField2 = new DeviceField();
        jPanel32 = new javax.swing.JPanel();
        deviceField4 = new DeviceField();
        deviceChoice1 = new DeviceChoice();
        deviceChoice2 = new DeviceChoice();
        deviceChoice3 = new DeviceChoice();

        jPanel6 = new javax.swing.JPanel();
        jPanel7 = new javax.swing.JPanel();
        jPanel9 = new javax.swing.JPanel();
        jLabel4 = new javax.swing.JLabel();
        jPanel10 = new javax.swing.JPanel();
        jLabel2 = new javax.swing.JLabel();
        jPanel11 = new javax.swing.JPanel();
        jLabel3 = new javax.swing.JLabel();
        jPanel12 = new javax.swing.JPanel();
        jLabel1 = new javax.swing.JLabel();
        jPanel13 = new javax.swing.JPanel();
        jLabel5 = new javax.swing.JLabel();
        jPanel8 = new javax.swing.JPanel();
        jPanel14 = new javax.swing.JPanel();
        deviceLabel1 = new DeviceLabel();
        jPanel15 = new javax.swing.JPanel();
        deviceLabel2 = new DeviceLabel();
        jPanel16 = new javax.swing.JPanel();
        deviceLabel3 = new DeviceLabel();
        jPanel17 = new javax.swing.JPanel();
        controlled1 = new javax.swing.JCheckBox();
        jPanel18 = new javax.swing.JPanel();
        notControlled1 = new javax.swing.JCheckBox();
        jPanel19 = new javax.swing.JPanel();
        jPanel20 = new javax.swing.JPanel();
        deviceLabel4 = new DeviceLabel();
        jPanel21 = new javax.swing.JPanel();
        deviceLabel6 = new DeviceLabel();
        jPanel22 = new javax.swing.JPanel();
        deviceLabel7 = new DeviceLabel();
        jPanel23 = new javax.swing.JPanel();
        controlled2 = new javax.swing.JCheckBox();
        jPanel24 = new javax.swing.JPanel();
        notControlled2 = new javax.swing.JCheckBox();
        jPanel25 = new javax.swing.JPanel();
        jPanel26 = new javax.swing.JPanel();
        deviceLabel9 = new DeviceLabel();
        jPanel27 = new javax.swing.JPanel();
        deviceLabel10 = new DeviceLabel();
        jPanel28 = new javax.swing.JPanel();
        deviceLabel11 = new DeviceLabel();
        jPanel29 = new javax.swing.JPanel();
        controlled3 = new javax.swing.JCheckBox();
        jPanel30 = new javax.swing.JPanel();
        notControlled3 = new javax.swing.JCheckBox();

        setDeviceProvider("localhost");
        setDeviceTitle("RFX Vessel Configuration");
        setDeviceType("RFXVConfig");
        setHeight(500);
        setWidth(1080);

        jPanel1.setLayout(new java.awt.BorderLayout());
        jPanel1.add(deviceButtons1, java.awt.BorderLayout.PAGE_END);

        jPanel2.setLayout(new java.awt.GridLayout(1, 0));

        jPanel3.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT));

        deviceField1.setIdentifier("");
        deviceField1.setLabelString("Comment:");
        deviceField1.setNumCols(40);
        deviceField1.setOffsetNid(1);
        deviceField1.setTextOnly(true);
        jPanel3.add(deviceField1);
        jPanel3.add(deviceDispatch1);

        jPanel2.add(jPanel3);

        jPanel1.add(jPanel2, java.awt.BorderLayout.NORTH);

        jPanel4.setLayout(new java.awt.BorderLayout());

        jPanel33.setBorder(javax.swing.BorderFactory.createTitledBorder("DPEL PELLET INJECTORS"));
        jPanel33.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT));

        deviceField3.setIdentifier("");
        deviceField3.setLabelString("Angle [deg] :");
        deviceField3.setOffsetNid(61);
        jPanel33.add(deviceField3);

        deviceField5.setIdentifier("");
        deviceField5.setLabelString("DRIGAS max [bar] :");
        deviceField5.setOffsetNid(62);
        jPanel33.add(deviceField5);

        jPanel4.add(jPanel33, java.awt.BorderLayout.SOUTH);

        jPanel5.setBorder(javax.swing.BorderFactory.createTitledBorder("  VI  "));
        jPanel5.setLayout(new java.awt.GridLayout(3, 0, 2, 0));

        deviceField2.setIdentifier("");
        deviceField2.setLabelString("N. Group Filling Valves :");
        deviceField2.setOffsetNid(4);
        jPanel31.add(deviceField2);

        jPanel5.add(jPanel31);

        deviceField4.setIdentifier("");
        deviceField4.setLabelString("N. Group Puffing Valves:");
        deviceField4.setOffsetNid(5);
        jPanel32.add(deviceField4);

        jPanel5.add(jPanel32);

        deviceChoice1.setChoiceItems(new String[] {"H2", "He", "D2", "Not used"});
        deviceChoice1.setIdentifier("");
        deviceChoice1.setLabelString("VIK1 Gas  :");
        deviceChoice1.setOffsetNid(7);
        deviceChoice1.setUpdateIdentifier("");
        jPanel5.add(deviceChoice1);

        deviceChoice2.setChoiceItems(new String[] {"H2", "He", "Ne", "Ar", "Not used"});
        deviceChoice2.setIdentifier("");
        deviceChoice2.setLabelString("Bottle Gas");
        deviceChoice2.setOffsetNid(9);
        deviceChoice2.setUpdateIdentifier("");
        jPanel5.add(deviceChoice2);

        deviceChoice3.setChoiceIntValues(new int[] {0, 1});
        deviceChoice3.setChoiceItems(new String[] {"Disabled", "Enabled"});
        deviceChoice3.setConvert(true);
        deviceChoice3.setIdentifier("");
        deviceChoice3.setLabelString("NE Control : ");
        deviceChoice3.setOffsetNid(64);
        deviceChoice3.setUpdateIdentifier("");
        jPanel5.add(deviceChoice3);


        jPanel4.add(jPanel5, java.awt.BorderLayout.NORTH);

        jPanel6.setBorder(javax.swing.BorderFactory.createTitledBorder("  VD  "));
        jPanel6.setLayout(new java.awt.GridLayout(4, 0));

        jPanel7.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
        jPanel7.setLayout(new java.awt.GridLayout(1, 0));

        jLabel4.setText("POSITION");
        jPanel9.add(jLabel4);

        jPanel7.add(jPanel9);

        jLabel2.setText("NAME");
        jPanel10.add(jLabel2);

        jPanel7.add(jPanel10);

        jLabel3.setText("VD MODULE");
        jPanel11.add(jLabel3);

        jPanel7.add(jPanel11);

        jLabel1.setText("CONTROLLED");
        jPanel12.add(jLabel1);

        jPanel7.add(jPanel12);

        jLabel5.setText("UNCONTROLLED");
        jPanel13.add(jLabel5);

        jPanel7.add(jPanel13);

        jPanel6.add(jPanel7);

        jPanel8.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
        jPanel8.setLayout(new java.awt.GridLayout(1, 5));

        deviceLabel1.setIdentifier("");
        deviceLabel1.setOffsetNid(16);
        deviceLabel1.setTextOnly(true);
        jPanel14.add(deviceLabel1);

        jPanel8.add(jPanel14);

        deviceLabel2.setIdentifier("");
        deviceLabel2.setOffsetNid(17);
        deviceLabel2.setTextOnly(true);
        jPanel15.add(deviceLabel2);

        jPanel8.add(jPanel15);

        deviceLabel3.setIdentifier("");
        deviceLabel3.setOffsetNid(18);
        deviceLabel3.setTextOnly(true);
        jPanel16.add(deviceLabel3);

        jPanel8.add(jPanel16);

        jPanel17.setName(""); // NOI18N

        buttonGroup1.add(controlled1);
        controlled1.setActionCommand("jCheckBox1");
        controlled1.setName("controlled1"); // NOI18N
        jPanel17.add(controlled1);

        jPanel8.add(jPanel17);

        buttonGroup1.add(notControlled1);
        notControlled1.setName("notControlled1"); // NOI18N
        jPanel18.add(notControlled1);

        jPanel8.add(jPanel18);

        jPanel6.add(jPanel8);

        jPanel19.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
        jPanel19.setLayout(new java.awt.GridLayout(1, 5));

        deviceLabel4.setIdentifier("");
        deviceLabel4.setOffsetNid(21);
        deviceLabel4.setTextOnly(true);
        jPanel20.add(deviceLabel4);

        jPanel19.add(jPanel20);

        deviceLabel6.setIdentifier("");
        deviceLabel6.setOffsetNid(22);
        deviceLabel6.setTextOnly(true);
        jPanel21.add(deviceLabel6);

        jPanel19.add(jPanel21);

        deviceLabel7.setIdentifier("");
        deviceLabel7.setOffsetNid(23);
        deviceLabel7.setTextOnly(true);
        jPanel22.add(deviceLabel7);

        jPanel19.add(jPanel22);

        buttonGroup2.add(controlled2);
        controlled2.setName("controlled2"); // NOI18N
        jPanel23.add(controlled2);

        jPanel19.add(jPanel23);

        buttonGroup2.add(notControlled2);
        notControlled2.setName("notControlled2"); // NOI18N
        jPanel24.add(notControlled2);

        jPanel19.add(jPanel24);

        jPanel6.add(jPanel19);

        jPanel25.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
        jPanel25.setLayout(new java.awt.GridLayout(1, 5));

        deviceLabel9.setIdentifier("");
        deviceLabel9.setOffsetNid(26);
        deviceLabel9.setTextOnly(true);
        jPanel26.add(deviceLabel9);

        jPanel25.add(jPanel26);

        deviceLabel10.setIdentifier("");
        deviceLabel10.setOffsetNid(27);
        deviceLabel10.setTextOnly(true);
        jPanel27.add(deviceLabel10);

        jPanel25.add(jPanel27);

        deviceLabel11.setIdentifier("");
        deviceLabel11.setOffsetNid(28);
        deviceLabel11.setTextOnly(true);
        jPanel28.add(deviceLabel11);

        jPanel25.add(jPanel28);

        buttonGroup3.add(controlled3);
        controlled3.setName("controlled3"); // NOI18N
        jPanel29.add(controlled3);

        jPanel25.add(jPanel29);

        buttonGroup3.add(notControlled3);
        notControlled3.setName("notControlled3"); // NOI18N
        jPanel30.add(notControlled3);

        jPanel25.add(jPanel30);

        jPanel6.add(jPanel25);

        jPanel4.add(jPanel6, java.awt.BorderLayout.CENTER);

        jPanel1.add(jPanel4, java.awt.BorderLayout.CENTER);

        getContentPane().add(jPanel1, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents

    private Component getComponentByBame(Container c, String name)
    {
        Component comps[] = c.getComponents();
        Component comp;


        for(int i = 0; i < comps.length; i++)
        {
            if( comps[i] instanceof java.awt.Container )
                if( ( comp = getComponentByBame((Container)comps[i],  name)) != null )
                    return comp;
            else
                if(comps[i].getName() != null && comps[i].getName().equals(name))
                    return comps[i];
        }
        return null;
    }

   private void resetCtrlState()
   {
       for(int i = 1; i <= 3; i++)
       {
           NidData ctrlNid = new NidData(this.baseNid + 14 +  i  * 5);
           String val;
           JCheckBox cb;
           try {
                val = Tree.dataToString((Data) subtree.getData(ctrlNid, Tree.context ));
                if( val.equals("\"CONTROLLED\"") )
                {
                    cb =  (JCheckBox)getComponentByBame( this, "controlled" + i );
                }
                else
                {
                    cb = (JCheckBox)getComponentByBame( this, "notControlled" + i );
                }
                cb.setSelected(true);
            }
            catch(Exception e)
            {
                System.out.println("Error set button state: " + e);
            }
       }
   }

   public void configure(RemoteTree subtree, int baseNid, Node node)
   {
       super.configure(subtree, baseNid, node);
       resetCtrlState();
       return;       
   }

   public void reset()
   {
       super.reset();
       resetCtrlState();
       return;
   }

   public void apply()
   {
       super.apply();

       for(int i = 1; i <= 3; i++)
       {
           NidData ctrlNid = new NidData(this.baseNid + 14 +  i  * 5);
           JCheckBox cb1 =  (JCheckBox)getComponentByBame( this, "controlled" + i );
           String val =  cb1.isSelected() ? "CONTROLLED" : "UNCONTROLLED";
            try {
                subtree.putData(ctrlNid, new StringData(val), Tree.context);
            }
            catch(Exception e)
            {
                System.out.println("Error writing device data: " + e);
            }
       }

       return;
   }



    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.ButtonGroup buttonGroup1;
    private javax.swing.ButtonGroup buttonGroup2;
    private javax.swing.ButtonGroup buttonGroup3;
    private javax.swing.JCheckBox controlled1;
    private javax.swing.JCheckBox controlled2;
    private javax.swing.JCheckBox controlled3;
    private DeviceButtons deviceButtons1;
    private DeviceChoice deviceChoice1;
    private DeviceChoice deviceChoice2;
    private DeviceChoice deviceChoice3;
    private DeviceDispatch deviceDispatch1;
    private DeviceField deviceField1;
    private DeviceField deviceField2;
    private DeviceField deviceField3;
    private DeviceField deviceField4;
    private DeviceField deviceField5;
    private DeviceLabel deviceLabel1;
    private DeviceLabel deviceLabel10;
    private DeviceLabel deviceLabel11;
    private DeviceLabel deviceLabel2;
    private DeviceLabel deviceLabel3;
    private DeviceLabel deviceLabel4;
    private DeviceLabel deviceLabel6;
    private DeviceLabel deviceLabel7;
    private DeviceLabel deviceLabel9;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JLabel jLabel5;
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
    private javax.swing.JPanel jPanel28;
    private javax.swing.JPanel jPanel29;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JPanel jPanel30;
    private javax.swing.JPanel jPanel31;
    private javax.swing.JPanel jPanel32;
    private javax.swing.JPanel jPanel33;
    private javax.swing.JPanel jPanel4;
    private javax.swing.JPanel jPanel5;
    private javax.swing.JPanel jPanel6;
    private javax.swing.JPanel jPanel7;
    private javax.swing.JPanel jPanel8;
    private javax.swing.JPanel jPanel9;
    private javax.swing.JCheckBox notControlled1;
    private javax.swing.JCheckBox notControlled2;
    private javax.swing.JCheckBox notControlled3;
    // End of variables declaration//GEN-END:variables

}

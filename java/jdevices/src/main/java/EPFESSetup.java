/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author capobianco
 */
public class EPFESSetup extends DeviceSetup {

    /**
     * Creates new form CiccioSetup
     */
    public EPFESSetup() {
        initComponents();
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jScrollPane2 = new javax.swing.JScrollPane();
        jPanel5 = new javax.swing.JPanel();
        jPanel96 = new javax.swing.JPanel();
        jPanel97 = new javax.swing.JPanel();
        deviceField25 = new DeviceField();
        deviceField26 = new DeviceField();
        deviceDispatch8 = new DeviceDispatch();
        jPanel99 = new javax.swing.JPanel();
        deviceField28 = new DeviceField();
        deviceField1 = new DeviceField();
        jPanel98 = new javax.swing.JPanel();
        deviceField2 = new DeviceField();
        deviceField3 = new DeviceField();
        jLabel2 = new javax.swing.JLabel();
        deviceField4 = new DeviceField();
        jLabel3 = new javax.swing.JLabel();
        deviceField27 = new DeviceField();
        jLabel1 = new javax.swing.JLabel();
        jSeparator9 = new javax.swing.JSeparator();
        jPanel1 = new javax.swing.JPanel();
        jPanel348 = new javax.swing.JPanel();
        deviceChannel77 = new DeviceChannel();
        deviceChoice246 = new DeviceChoice();
        deviceChoice247 = new DeviceChoice();
        jPanel356 = new javax.swing.JPanel();
        deviceChannel85 = new DeviceChannel();
        deviceChoice270 = new DeviceChoice();
        deviceChoice271 = new DeviceChoice();
        jPanel357 = new javax.swing.JPanel();
        deviceChannel86 = new DeviceChannel();
        deviceChoice273 = new DeviceChoice();
        deviceChoice274 = new DeviceChoice();
        jPanel358 = new javax.swing.JPanel();
        deviceChannel87 = new DeviceChannel();
        deviceChoice276 = new DeviceChoice();
        deviceChoice277 = new DeviceChoice();
        jPanel359 = new javax.swing.JPanel();
        deviceChannel88 = new DeviceChannel();
        deviceChoice279 = new DeviceChoice();
        deviceChoice280 = new DeviceChoice();
        jPanel360 = new javax.swing.JPanel();
        deviceChannel89 = new DeviceChannel();
        deviceChoice282 = new DeviceChoice();
        deviceChoice283 = new DeviceChoice();
        jPanel361 = new javax.swing.JPanel();
        deviceChannel90 = new DeviceChannel();
        deviceChoice285 = new DeviceChoice();
        deviceChoice286 = new DeviceChoice();
        jPanel362 = new javax.swing.JPanel();
        deviceChannel91 = new DeviceChannel();
        deviceChoice288 = new DeviceChoice();
        deviceChoice289 = new DeviceChoice();
        deviceButtons10 = new DeviceButtons();

        setDeviceProvider("192.168.110.22");
        setDeviceTitle("EPFES");
        setDeviceType("EPFES");
        setHeight(650);
        setWidth(1000);

        jScrollPane2.setPreferredSize(new java.awt.Dimension(900, 500));

        jPanel5.setPreferredSize(new java.awt.Dimension(900, 876));
        jPanel5.setLayout(new java.awt.BorderLayout());

        jPanel96.setPreferredSize(new java.awt.Dimension(678, 150));
        jPanel96.setLayout(new java.awt.GridLayout(4, 0));

        jPanel97.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT));

        deviceField25.setIdentifier("");
        deviceField25.setLabelString("IP:");
        deviceField25.setOffsetNid(2);
        deviceField25.setTextOnly(true);
        jPanel97.add(deviceField25);

        deviceField26.setIdentifier("");
        deviceField26.setLabelString("PORT:");
        deviceField26.setOffsetNid(3);
        jPanel97.add(deviceField26);
        jPanel97.add(deviceDispatch8);

        jPanel96.add(jPanel97);

        jPanel99.setPreferredSize(new java.awt.Dimension(700, 44));
        jPanel99.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT));

        deviceField28.setIdentifier("");
        deviceField28.setLabelString("COMMENT:");
        deviceField28.setNumCols(30);
        deviceField28.setOffsetNid(1);
        deviceField28.setTextOnly(true);
        jPanel99.add(deviceField28);

        deviceField1.setIdentifier("");
        deviceField1.setLabelString("EVT:");
        deviceField1.setNumCols(20);
        deviceField1.setOffsetNid(4);
        jPanel99.add(deviceField1);

        jPanel96.add(jPanel99);

        jPanel98.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT));

        deviceField2.setIdentifier("");
        deviceField2.setLabelString("N_CYCL");
        deviceField2.setOffsetNid(7);
        jPanel98.add(deviceField2);

        deviceField3.setIdentifier("");
        deviceField3.setLabelString("T_ON:");
        deviceField3.setOffsetNid(5);
        jPanel98.add(deviceField3);

        jLabel2.setText("(s)");
        jPanel98.add(jLabel2);

        deviceField4.setIdentifier("");
        deviceField4.setLabelString("T_OFF:");
        deviceField4.setOffsetNid(6);
        jPanel98.add(deviceField4);

        jLabel3.setText("(s)");
        jPanel98.add(jLabel3);

        deviceField27.setIdentifier("");
        deviceField27.setLabelString("WD_TMT:");
        deviceField27.setOffsetNid(8);
        jPanel98.add(deviceField27);

        jLabel1.setText("(m)");
        jPanel98.add(jLabel1);

        jPanel96.add(jPanel98);

        jSeparator9.setPreferredSize(new java.awt.Dimension(0, 12));
        jPanel96.add(jSeparator9);

        jPanel5.add(jPanel96, java.awt.BorderLayout.PAGE_START);

        jPanel1.setLayout(new java.awt.GridLayout(8, 0));

        jPanel348.setBorder(javax.swing.BorderFactory.createTitledBorder("CH 1"));
        jPanel348.setLayout(new java.awt.BorderLayout());

        deviceChannel77.setLabelString("ON-OFF");
        deviceChannel77.setOffsetNid(9);
        deviceChannel77.setShowVal("");
        deviceChannel77.setUpdateIdentifier("");

        deviceChoice246.setChoiceItems(new String[] {"INT", "EXT"});
        deviceChoice246.setIdentifier("");
        deviceChoice246.setLabelString("SOURCE: ");
        deviceChoice246.setOffsetNid(10);
        deviceChoice246.setUpdateIdentifier("");
        deviceChannel77.getContainer().add(deviceChoice246);

        deviceChoice247.setChoiceItems(new String[] {"LOW", "HIGH"});
        deviceChoice247.setIdentifier("");
        deviceChoice247.setLabelString("IRANGE: ");
        deviceChoice247.setOffsetNid(11);
        deviceChoice247.setUpdateIdentifier("");
        deviceChannel77.getContainer().add(deviceChoice247);

        jPanel348.add(deviceChannel77, java.awt.BorderLayout.CENTER);

        jPanel1.add(jPanel348);

        jPanel356.setBorder(javax.swing.BorderFactory.createTitledBorder("CH 2"));
        jPanel356.setLayout(new java.awt.BorderLayout());

        deviceChannel85.setLabelString("ON-OFF");
        deviceChannel85.setOffsetNid(12);
        deviceChannel85.setShowVal("");
        deviceChannel85.setUpdateIdentifier("");

        deviceChoice270.setChoiceItems(new String[] {"INT", "EXT"});
        deviceChoice270.setIdentifier("");
        deviceChoice270.setLabelString("SOURCE: ");
        deviceChoice270.setOffsetNid(13);
        deviceChoice270.setUpdateIdentifier("");
        deviceChannel85.getContainer().add(deviceChoice270);

        deviceChoice271.setChoiceItems(new String[] {"LOW", "HIGH"});
        deviceChoice271.setIdentifier("");
        deviceChoice271.setLabelString("IRANGE: ");
        deviceChoice271.setOffsetNid(14);
        deviceChoice271.setUpdateIdentifier("");
        deviceChannel85.getContainer().add(deviceChoice271);

        jPanel356.add(deviceChannel85, java.awt.BorderLayout.CENTER);

        jPanel1.add(jPanel356);

        jPanel357.setBorder(javax.swing.BorderFactory.createTitledBorder("CH 3"));
        jPanel357.setLayout(new java.awt.BorderLayout());

        deviceChannel86.setLabelString("ON-OFF");
        deviceChannel86.setOffsetNid(15);
        deviceChannel86.setShowVal("");
        deviceChannel86.setUpdateIdentifier("");

        deviceChoice273.setChoiceItems(new String[] {"INT", "EXT"});
        deviceChoice273.setIdentifier("");
        deviceChoice273.setLabelString("SOURCE: ");
        deviceChoice273.setOffsetNid(16);
        deviceChoice273.setUpdateIdentifier("");
        deviceChannel86.getContainer().add(deviceChoice273);

        deviceChoice274.setChoiceItems(new String[] {"LOW", "HIGH"});
        deviceChoice274.setIdentifier("");
        deviceChoice274.setLabelString("IRANGE: ");
        deviceChoice274.setOffsetNid(17);
        deviceChoice274.setUpdateIdentifier("");
        deviceChannel86.getContainer().add(deviceChoice274);

        jPanel357.add(deviceChannel86, java.awt.BorderLayout.CENTER);

        jPanel1.add(jPanel357);

        jPanel358.setBorder(javax.swing.BorderFactory.createTitledBorder("CH 4"));
        jPanel358.setLayout(new java.awt.BorderLayout());

        deviceChannel87.setLabelString("ON-OFF");
        deviceChannel87.setOffsetNid(18);
        deviceChannel87.setShowVal("");
        deviceChannel87.setUpdateIdentifier("");

        deviceChoice276.setChoiceItems(new String[] {"INT", "EXT"});
        deviceChoice276.setIdentifier("");
        deviceChoice276.setLabelString("SOURCE: ");
        deviceChoice276.setOffsetNid(19);
        deviceChoice276.setUpdateIdentifier("");
        deviceChannel87.getContainer().add(deviceChoice276);

        deviceChoice277.setChoiceItems(new String[] {"LOW", "HIGH"});
        deviceChoice277.setIdentifier("");
        deviceChoice277.setLabelString("IRANGE: ");
        deviceChoice277.setOffsetNid(20);
        deviceChoice277.setUpdateIdentifier("");
        deviceChannel87.getContainer().add(deviceChoice277);

        jPanel358.add(deviceChannel87, java.awt.BorderLayout.CENTER);

        jPanel1.add(jPanel358);

        jPanel359.setBorder(javax.swing.BorderFactory.createTitledBorder("CH 5"));
        jPanel359.setLayout(new java.awt.BorderLayout());

        deviceChannel88.setLabelString("ON-OFF");
        deviceChannel88.setOffsetNid(21);
        deviceChannel88.setShowVal("");
        deviceChannel88.setUpdateIdentifier("");

        deviceChoice279.setChoiceItems(new String[] {"INT", "EXT"});
        deviceChoice279.setIdentifier("");
        deviceChoice279.setLabelString("SOURCE: ");
        deviceChoice279.setOffsetNid(22);
        deviceChoice279.setUpdateIdentifier("");
        deviceChannel88.getContainer().add(deviceChoice279);

        deviceChoice280.setChoiceItems(new String[] {"LOW", "HIGH"});
        deviceChoice280.setIdentifier("");
        deviceChoice280.setLabelString("IRANGE: ");
        deviceChoice280.setOffsetNid(23);
        deviceChoice280.setUpdateIdentifier("");
        deviceChannel88.getContainer().add(deviceChoice280);

        jPanel359.add(deviceChannel88, java.awt.BorderLayout.CENTER);

        jPanel1.add(jPanel359);

        jPanel360.setBorder(javax.swing.BorderFactory.createTitledBorder("CH 6"));
        jPanel360.setLayout(new java.awt.BorderLayout());

        deviceChannel89.setLabelString("ON-OFF");
        deviceChannel89.setOffsetNid(24);
        deviceChannel89.setShowVal("");
        deviceChannel89.setUpdateIdentifier("");

        deviceChoice282.setChoiceItems(new String[] {"INT", "EXT"});
        deviceChoice282.setIdentifier("");
        deviceChoice282.setLabelString("SOURCE: ");
        deviceChoice282.setOffsetNid(25);
        deviceChoice282.setUpdateIdentifier("");
        deviceChannel89.getContainer().add(deviceChoice282);

        deviceChoice283.setChoiceItems(new String[] {"LOW", "HIGH"});
        deviceChoice283.setIdentifier("");
        deviceChoice283.setLabelString("IRANGE: ");
        deviceChoice283.setOffsetNid(26);
        deviceChoice283.setUpdateIdentifier("");
        deviceChannel89.getContainer().add(deviceChoice283);

        jPanel360.add(deviceChannel89, java.awt.BorderLayout.CENTER);

        jPanel1.add(jPanel360);

        jPanel361.setBorder(javax.swing.BorderFactory.createTitledBorder("CH 7"));
        jPanel361.setLayout(new java.awt.BorderLayout());

        deviceChannel90.setLabelString("ON-OFF");
        deviceChannel90.setOffsetNid(27);
        deviceChannel90.setShowVal("");
        deviceChannel90.setUpdateIdentifier("");

        deviceChoice285.setChoiceItems(new String[] {"INT", "EXT"});
        deviceChoice285.setIdentifier("");
        deviceChoice285.setLabelString("SOURCE: ");
        deviceChoice285.setOffsetNid(28);
        deviceChoice285.setUpdateIdentifier("");
        deviceChannel90.getContainer().add(deviceChoice285);

        deviceChoice286.setChoiceItems(new String[] {"LOW", "HIGH"});
        deviceChoice286.setIdentifier("");
        deviceChoice286.setLabelString("IRANGE: ");
        deviceChoice286.setOffsetNid(29);
        deviceChoice286.setUpdateIdentifier("");
        deviceChannel90.getContainer().add(deviceChoice286);

        jPanel361.add(deviceChannel90, java.awt.BorderLayout.CENTER);

        jPanel1.add(jPanel361);

        jPanel362.setBorder(javax.swing.BorderFactory.createTitledBorder("CH 8"));
        jPanel362.setLayout(new java.awt.BorderLayout());

        deviceChannel91.setLabelString("ON-OFF");
        deviceChannel91.setOffsetNid(30);
        deviceChannel91.setShowVal("");
        deviceChannel91.setUpdateIdentifier("");

        deviceChoice288.setChoiceItems(new String[] {"INT", "EXT"});
        deviceChoice288.setIdentifier("");
        deviceChoice288.setLabelString("SOURCE: ");
        deviceChoice288.setOffsetNid(31);
        deviceChoice288.setUpdateIdentifier("");
        deviceChannel91.getContainer().add(deviceChoice288);

        deviceChoice289.setChoiceItems(new String[] {"LOW", "HIGH"});
        deviceChoice289.setIdentifier("");
        deviceChoice289.setLabelString("IRANGE: ");
        deviceChoice289.setOffsetNid(32);
        deviceChoice289.setUpdateIdentifier("");
        deviceChannel91.getContainer().add(deviceChoice289);

        jPanel362.add(deviceChannel91, java.awt.BorderLayout.CENTER);

        jPanel1.add(jPanel362);

        jPanel5.add(jPanel1, java.awt.BorderLayout.CENTER);

        jScrollPane2.setViewportView(jPanel5);

        getContentPane().add(jScrollPane2, java.awt.BorderLayout.CENTER);

        deviceButtons10.setCheckExpressions(new String[] {});
        deviceButtons10.setCheckMessages(new String[] {});
        deviceButtons10.setMethods(new String[] {"init", "start", "stop", "store"});
        getContentPane().add(deviceButtons10, java.awt.BorderLayout.SOUTH);
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private DeviceButtons deviceButtons10;
    private DeviceChannel deviceChannel77;
    private DeviceChannel deviceChannel85;
    private DeviceChannel deviceChannel86;
    private DeviceChannel deviceChannel87;
    private DeviceChannel deviceChannel88;
    private DeviceChannel deviceChannel89;
    private DeviceChannel deviceChannel90;
    private DeviceChannel deviceChannel91;
    private DeviceChoice deviceChoice246;
    private DeviceChoice deviceChoice247;
    private DeviceChoice deviceChoice270;
    private DeviceChoice deviceChoice271;
    private DeviceChoice deviceChoice273;
    private DeviceChoice deviceChoice274;
    private DeviceChoice deviceChoice276;
    private DeviceChoice deviceChoice277;
    private DeviceChoice deviceChoice279;
    private DeviceChoice deviceChoice280;
    private DeviceChoice deviceChoice282;
    private DeviceChoice deviceChoice283;
    private DeviceChoice deviceChoice285;
    private DeviceChoice deviceChoice286;
    private DeviceChoice deviceChoice288;
    private DeviceChoice deviceChoice289;
    private DeviceDispatch deviceDispatch8;
    private DeviceField deviceField1;
    private DeviceField deviceField2;
    private DeviceField deviceField25;
    private DeviceField deviceField26;
    private DeviceField deviceField27;
    private DeviceField deviceField28;
    private DeviceField deviceField3;
    private DeviceField deviceField4;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel348;
    private javax.swing.JPanel jPanel356;
    private javax.swing.JPanel jPanel357;
    private javax.swing.JPanel jPanel358;
    private javax.swing.JPanel jPanel359;
    private javax.swing.JPanel jPanel360;
    private javax.swing.JPanel jPanel361;
    private javax.swing.JPanel jPanel362;
    private javax.swing.JPanel jPanel5;
    private javax.swing.JPanel jPanel96;
    private javax.swing.JPanel jPanel97;
    private javax.swing.JPanel jPanel98;
    private javax.swing.JPanel jPanel99;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JSeparator jSeparator9;
    // End of variables declaration//GEN-END:variables
}

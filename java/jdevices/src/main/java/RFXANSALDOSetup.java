import javax.swing.*;
import java.awt.*;
import javax.swing.border.*;

//import com.borland.jbcl.layout.*;
/**
 * <p>
 * Title:
 * </p>
 * <p>
 * Description:
 * </p>
 * <p>
 * Copyright: Copyright (c) 2004
 * </p>
 * <p>
 * Company:
 * </p>
 *
 * @author not attributable
 * @version 1.0
 */
public class RFXANSALDOSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel1 = new JPanel();
	JTabbedPane jTabbedPane1 = new JTabbedPane();
	JPanel jPanel2 = new JPanel();
	FlowLayout flowLayout1 = new FlowLayout();
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceField deviceField1 = new DeviceField();
	TitledBorder titledBorder1;
	TitledBorder titledBorder2;
	FlowLayout flowLayout2111112 = new FlowLayout();
	FlowLayout flowLayout2111113 = new FlowLayout();
	FlowLayout flowLayout2111125 = new FlowLayout();
	FlowLayout flowLayout2111114 = new FlowLayout();
	FlowLayout flowLayout2111115 = new FlowLayout();
	FlowLayout flowLayout2111126 = new FlowLayout();
	FlowLayout flowLayout2111116 = new FlowLayout();
	FlowLayout flowLayout7 = new FlowLayout();
	FlowLayout flowLayout2111117 = new FlowLayout();
	FlowLayout flowLayout2111118 = new FlowLayout();
	FlowLayout flowLayout8 = new FlowLayout();
	FlowLayout flowLayout2111127 = new FlowLayout();
	FlowLayout flowLayout2111128 = new FlowLayout();
	FlowLayout flowLayout2111119 = new FlowLayout();
	FlowLayout flowLayout9 = new FlowLayout();
	FlowLayout flowLayout11 = new FlowLayout();
	FlowLayout flowLayout12 = new FlowLayout();
	FlowLayout flowLayout2111120 = new FlowLayout();
	FlowLayout flowLayout21111110 = new FlowLayout();
	FlowLayout flowLayout21111111 = new FlowLayout();
	FlowLayout flowLayout2111129 = new FlowLayout();
	FlowLayout flowLayout21111210 = new FlowLayout();
	FlowLayout flowLayout2111121 = new FlowLayout();
	FlowLayout flowLayout2111122 = new FlowLayout();
	FlowLayout flowLayout21111211 = new FlowLayout();
	FlowLayout flowLayout2111130 = new FlowLayout();
	FlowLayout flowLayout21111112 = new FlowLayout();
	FlowLayout flowLayout21111113 = new FlowLayout();
	FlowLayout flowLayout6 = new FlowLayout();
	FlowLayout flowLayout14 = new FlowLayout();
	FlowLayout flowLayout2111131 = new FlowLayout();
	FlowLayout flowLayout21111212 = new FlowLayout();
	FlowLayout flowLayout21111213 = new FlowLayout();
	FlowLayout flowLayout2111132 = new FlowLayout();
	FlowLayout flowLayout15 = new FlowLayout();
	FlowLayout flowLayout21111114 = new FlowLayout();
	FlowLayout flowLayout21111115 = new FlowLayout();
	FlowLayout flowLayout13 = new FlowLayout();
	FlowLayout flowLayout17 = new FlowLayout();
	FlowLayout flowLayout2111133 = new FlowLayout();
	FlowLayout flowLayout21111214 = new FlowLayout();
	FlowLayout flowLayout21111215 = new FlowLayout();
	FlowLayout flowLayout2111134 = new FlowLayout();
	JPanel UNITS_A = new JPanel();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel4 = new JPanel();
	JPanel jPanel7 = new JPanel();
	BorderLayout borderLayout3 = new BorderLayout();
	JLabel jLabel1 = new JLabel();
	JLabel jLabel2 = new JLabel();
	JLabel jLabel3 = new JLabel();
	JLabel jLabel4 = new JLabel();
	JLabel jLabel5 = new JLabel();
	FlowLayout flowLayout2 = new FlowLayout();
	JScrollPane jScrollPane3 = new JScrollPane();
	GridLayout gridLayout2 = new GridLayout();
	JPanel jPanel8 = new JPanel();
	JLabel jLabel6 = new JLabel();
	JLabel jLabel7 = new JLabel();
	JLabel jLabel8 = new JLabel();
	JLabel jLabel9 = new JLabel();
	JLabel jLabel10 = new JLabel();
	FlowLayout flowLayout3 = new FlowLayout();
	JLabel jLabel11 = new JLabel();
	JPanel jPanel9 = new JPanel();
	JLabel jLabel12 = new JLabel();
	JPanel jPanel5 = new JPanel();
	JLabel jLabel13 = new JLabel();
	JLabel jLabel14 = new JLabel();
	JPanel jPanel10 = new JPanel();
	BorderLayout borderLayout4 = new BorderLayout();
	JLabel jLabel15 = new JLabel();
	JLabel jLabel16 = new JLabel();
	GridLayout gridLayout3 = new GridLayout();
	JLabel jLabel17 = new JLabel();
	JLabel jLabel18 = new JLabel();
	JLabel jLabel19 = new JLabel();
	JLabel jLabel20 = new JLabel();
	GridLayout gridLayout4 = new GridLayout();
	JPanel UNITS_B = new JPanel();
	JScrollPane jScrollPane4 = new JScrollPane();
	DeviceChoice deviceChoice2111120 = new DeviceChoice();
	JPanel jPanel11126 = new JPanel();
	DeviceChoice deviceChoice11130 = new DeviceChoice();
	JPanel jPanel11127 = new JPanel();
	DeviceChoice deviceChoice2111121 = new DeviceChoice();
	DeviceChoice deviceChoice211120 = new DeviceChoice();
	JPanel jPanel12118 = new JPanel();
	GridBagLayout gridBagLayout24 = new GridBagLayout();
	DeviceChannel deviceChannel124 = new DeviceChannel();
	DeviceChoice deviceChoice111120 = new DeviceChoice();
	DeviceChoice deviceChoice11131 = new DeviceChoice();
	DeviceChannel deviceChannel127 = new DeviceChannel();
	DeviceChoice deviceChoice11132 = new DeviceChoice();
	DeviceChoice deviceChoice11133 = new DeviceChoice();
	JPanel jPanel11128 = new JPanel();
	DeviceChoice deviceChoice2111122 = new DeviceChoice();
	DeviceChannel deviceChannel128 = new DeviceChannel();
	JPanel jPanel12119 = new JPanel();
	GridBagLayout gridBagLayout25 = new GridBagLayout();
	DeviceChoice deviceChoice11134 = new DeviceChoice();
	DeviceChoice deviceChoice111121 = new DeviceChoice();
	DeviceChannel deviceChannel129 = new DeviceChannel();
	DeviceChoice deviceChoice211121 = new DeviceChoice();
	DeviceChoice deviceChoice2111123 = new DeviceChoice();
	JPanel jPanel11129 = new JPanel();
	DeviceChoice deviceChoice11135 = new DeviceChoice();
	DeviceChoice deviceChoice11136 = new DeviceChoice();
	JPanel jPanel111210 = new JPanel();
	DeviceChoice deviceChoice2111124 = new DeviceChoice();
	DeviceChannel deviceChannel1210 = new DeviceChannel();
	JPanel jPanel121110 = new JPanel();
	GridBagLayout gridBagLayout26 = new GridBagLayout();
	DeviceChoice deviceChoice11137 = new DeviceChoice();
	DeviceChoice deviceChoice111122 = new DeviceChoice();
	DeviceChannel deviceChannel1211 = new DeviceChannel();
	DeviceChoice deviceChoice211122 = new DeviceChoice();
	DeviceChoice deviceChoice2111125 = new DeviceChoice();
	JPanel jPanel111211 = new JPanel();
	DeviceChoice deviceChoice11138 = new DeviceChoice();
	DeviceChoice deviceChoice11139 = new DeviceChoice();
	JPanel jPanel111212 = new JPanel();
	DeviceChoice deviceChoice2111126 = new DeviceChoice();
	DeviceChannel deviceChannel1212 = new DeviceChannel();
	JPanel jPanel121111 = new JPanel();
	GridBagLayout gridBagLayout27 = new GridBagLayout();
	DeviceChoice deviceChoice111310 = new DeviceChoice();
	DeviceChoice deviceChoice111123 = new DeviceChoice();
	DeviceChannel deviceChannel1213 = new DeviceChannel();
	DeviceChoice deviceChoice211123 = new DeviceChoice();
	DeviceChoice deviceChoice2111127 = new DeviceChoice();
	JPanel jPanel111213 = new JPanel();
	DeviceChoice deviceChoice111311 = new DeviceChoice();
	DeviceChoice deviceChoice111312 = new DeviceChoice();
	JPanel jPanel111214 = new JPanel();
	DeviceChoice deviceChoice2111128 = new DeviceChoice();
	DeviceChannel deviceChannel1214 = new DeviceChannel();
	JPanel jPanel121112 = new JPanel();
	GridBagLayout gridBagLayout28 = new GridBagLayout();
	DeviceChoice deviceChoice111313 = new DeviceChoice();
	DeviceChoice deviceChoice111124 = new DeviceChoice();
	DeviceChannel deviceChannel1215 = new DeviceChannel();
	DeviceChoice deviceChoice211124 = new DeviceChoice();
	DeviceChoice deviceChoice2111129 = new DeviceChoice();
	JPanel jPanel111215 = new JPanel();
	DeviceChoice deviceChoice111314 = new DeviceChoice();
	DeviceChoice deviceChoice111315 = new DeviceChoice();
	JPanel jPanel111216 = new JPanel();
	DeviceChoice deviceChoice21111210 = new DeviceChoice();
	DeviceChannel deviceChannel1216 = new DeviceChannel();
	JPanel jPanel121113 = new JPanel();
	GridBagLayout gridBagLayout29 = new GridBagLayout();
	DeviceChoice deviceChoice111316 = new DeviceChoice();
	DeviceChoice deviceChoice111125 = new DeviceChoice();
	DeviceChannel deviceChannel1217 = new DeviceChannel();
	DeviceChoice deviceChoice211125 = new DeviceChoice();
	DeviceChoice deviceChoice21111211 = new DeviceChoice();
	JPanel jPanel111217 = new JPanel();
	DeviceChoice deviceChoice111317 = new DeviceChoice();
	DeviceChoice deviceChoice111318 = new DeviceChoice();
	JPanel jPanel111218 = new JPanel();
	DeviceChoice deviceChoice21111212 = new DeviceChoice();
	DeviceChannel deviceChannel1218 = new DeviceChannel();
	JPanel jPanel121114 = new JPanel();
	GridBagLayout gridBagLayout210 = new GridBagLayout();
	DeviceChoice deviceChoice111319 = new DeviceChoice();
	DeviceChoice deviceChoice111126 = new DeviceChoice();
	DeviceChannel deviceChannel1219 = new DeviceChannel();
	DeviceChoice deviceChoice211126 = new DeviceChoice();
	DeviceChoice deviceChoice21111213 = new DeviceChoice();
	JPanel jPanel111219 = new JPanel();
	DeviceChoice deviceChoice1113110 = new DeviceChoice();
	DeviceChoice deviceChoice1113111 = new DeviceChoice();
	JPanel jPanel1112110 = new JPanel();
	DeviceChoice deviceChoice21111214 = new DeviceChoice();
	DeviceChannel deviceChannel12110 = new DeviceChannel();
	JPanel jPanel121115 = new JPanel();
	GridBagLayout gridBagLayout211 = new GridBagLayout();
	DeviceChoice deviceChoice1113112 = new DeviceChoice();
	DeviceChoice deviceChoice111127 = new DeviceChoice();
	DeviceChannel deviceChannel12111 = new DeviceChannel();
	DeviceChoice deviceChoice211127 = new DeviceChoice();
	DeviceChoice deviceChoice21111215 = new DeviceChoice();
	JPanel jPanel1112111 = new JPanel();
	DeviceChoice deviceChoice1113113 = new DeviceChoice();
	DeviceChoice deviceChoice1113114 = new DeviceChoice();
	JPanel jPanel1112112 = new JPanel();
	DeviceChoice deviceChoice21111216 = new DeviceChoice();
	DeviceChannel deviceChannel12112 = new DeviceChannel();
	JPanel jPanel121116 = new JPanel();
	GridBagLayout gridBagLayout212 = new GridBagLayout();
	DeviceChoice deviceChoice1113115 = new DeviceChoice();
	DeviceChoice deviceChoice111128 = new DeviceChoice();
	DeviceChannel deviceChannel12113 = new DeviceChannel();
	DeviceChoice deviceChoice211128 = new DeviceChoice();
	DeviceChoice deviceChoice21111217 = new DeviceChoice();
	JPanel jPanel1112113 = new JPanel();
	DeviceChoice deviceChoice1113116 = new DeviceChoice();
	DeviceChoice deviceChoice1113117 = new DeviceChoice();
	JPanel jPanel1112114 = new JPanel();
	DeviceChoice deviceChoice21111218 = new DeviceChoice();
	DeviceChannel deviceChannel12114 = new DeviceChannel();
	JPanel jPanel121117 = new JPanel();
	GridBagLayout gridBagLayout213 = new GridBagLayout();
	DeviceChoice deviceChoice1113118 = new DeviceChoice();
	DeviceChoice deviceChoice111129 = new DeviceChoice();
	DeviceChannel deviceChannel12115 = new DeviceChannel();
	DeviceChoice deviceChoice211129 = new DeviceChoice();
	DeviceChoice deviceChoice21111219 = new DeviceChoice();
	JPanel jPanel1112115 = new JPanel();
	DeviceChoice deviceChoice1113119 = new DeviceChoice();
	DeviceChoice deviceChoice11131110 = new DeviceChoice();
	JPanel jPanel1112116 = new JPanel();
	DeviceChoice deviceChoice211112110 = new DeviceChoice();
	DeviceChannel deviceChannel12116 = new DeviceChannel();
	JPanel jPanel121118 = new JPanel();
	GridBagLayout gridBagLayout214 = new GridBagLayout();
	DeviceChoice deviceChoice11131111 = new DeviceChoice();
	DeviceChoice deviceChoice1111210 = new DeviceChoice();
	DeviceChannel deviceChannel12117 = new DeviceChannel();
	DeviceChoice deviceChoice2111210 = new DeviceChoice();
	DeviceChoice deviceChoice211112111 = new DeviceChoice();
	JPanel jPanel1112117 = new JPanel();
	DeviceChoice deviceChoice11131112 = new DeviceChoice();
	DeviceChoice deviceChoice11131113 = new DeviceChoice();
	JPanel jPanel1112118 = new JPanel();
	DeviceChoice deviceChoice211112112 = new DeviceChoice();
	DeviceChannel deviceChannel12118 = new DeviceChannel();
	JPanel jPanel121119 = new JPanel();
	GridBagLayout gridBagLayout215 = new GridBagLayout();
	DeviceChoice deviceChoice11131114 = new DeviceChoice();
	DeviceChoice deviceChoice1111211 = new DeviceChoice();
	DeviceChannel deviceChannel12119 = new DeviceChannel();
	DeviceChoice deviceChoice2111211 = new DeviceChoice();
	DeviceChoice deviceChoice211112113 = new DeviceChoice();
	JPanel jPanel1112119 = new JPanel();
	DeviceChoice deviceChoice11131115 = new DeviceChoice();
	DeviceChoice deviceChoice111320 = new DeviceChoice();
	JPanel jPanel111220 = new JPanel();
	DeviceChoice deviceChoice21111220 = new DeviceChoice();
	DeviceChannel deviceChannel1220 = new DeviceChannel();
	JPanel jPanel121120 = new JPanel();
	GridBagLayout gridBagLayout216 = new GridBagLayout();
	DeviceChoice deviceChoice111321 = new DeviceChoice();
	DeviceChoice deviceChoice1111212 = new DeviceChoice();
	DeviceChannel deviceChannel125 = new DeviceChannel();
	DeviceChoice deviceChoice2111212 = new DeviceChoice();
	DeviceChoice deviceChoice21111221 = new DeviceChoice();
	JPanel jPanel111221 = new JPanel();
	DeviceChoice deviceChoice111322 = new DeviceChoice();
	DeviceChoice deviceChoice111323 = new DeviceChoice();
	JPanel jPanel111222 = new JPanel();
	DeviceChoice deviceChoice21111222 = new DeviceChoice();
	DeviceChannel deviceChannel1221 = new DeviceChannel();
	JPanel jPanel121121 = new JPanel();
	GridBagLayout gridBagLayout217 = new GridBagLayout();
	DeviceChoice deviceChoice111324 = new DeviceChoice();
	DeviceChoice deviceChoice1111213 = new DeviceChoice();
	DeviceChannel deviceChannel126 = new DeviceChannel();
	DeviceChoice deviceChoice2111213 = new DeviceChoice();
	DeviceChoice deviceChoice21111223 = new DeviceChoice();
	JPanel jPanel111223 = new JPanel();
	DeviceChoice deviceChoice111325 = new DeviceChoice();
	DeviceChoice deviceChoice111326 = new DeviceChoice();
	JPanel jPanel111224 = new JPanel();
	DeviceChoice deviceChoice21111224 = new DeviceChoice();
	DeviceChannel deviceChannel1222 = new DeviceChannel();
	JPanel jPanel121122 = new JPanel();
	GridBagLayout gridBagLayout218 = new GridBagLayout();
	DeviceChoice deviceChoice111327 = new DeviceChoice();
	DeviceChoice deviceChoice1111214 = new DeviceChoice();
	DeviceChannel deviceChannel1223 = new DeviceChannel();
	DeviceChoice deviceChoice2111214 = new DeviceChoice();
	DeviceChoice deviceChoice21111225 = new DeviceChoice();
	JPanel jPanel111225 = new JPanel();
	DeviceChoice deviceChoice111328 = new DeviceChoice();
	DeviceChoice deviceChoice111329 = new DeviceChoice();
	JPanel jPanel111226 = new JPanel();
	DeviceChoice deviceChoice21111226 = new DeviceChoice();
	DeviceChannel deviceChannel1224 = new DeviceChannel();
	JPanel jPanel121123 = new JPanel();
	GridBagLayout gridBagLayout219 = new GridBagLayout();
	DeviceChoice deviceChoice1113210 = new DeviceChoice();
	DeviceChoice deviceChoice1111215 = new DeviceChoice();
	DeviceChannel deviceChannel1225 = new DeviceChannel();
	DeviceChoice deviceChoice2111215 = new DeviceChoice();
	DeviceChoice deviceChoice21111227 = new DeviceChoice();
	JPanel jPanel111227 = new JPanel();
	DeviceChoice deviceChoice1113211 = new DeviceChoice();
	DeviceChoice deviceChoice1113212 = new DeviceChoice();
	JPanel jPanel111228 = new JPanel();
	DeviceChoice deviceChoice21111228 = new DeviceChoice();
	DeviceChannel deviceChannel1226 = new DeviceChannel();
	JPanel jPanel121124 = new JPanel();
	GridBagLayout gridBagLayout2110 = new GridBagLayout();
	DeviceChoice deviceChoice1113213 = new DeviceChoice();
	DeviceChoice deviceChoice1111216 = new DeviceChoice();
	DeviceChannel deviceChannel1227 = new DeviceChannel();
	DeviceChoice deviceChoice2111216 = new DeviceChoice();
	DeviceChoice deviceChoice21111229 = new DeviceChoice();
	JPanel jPanel111229 = new JPanel();
	DeviceChoice deviceChoice1113214 = new DeviceChoice();
	DeviceChoice deviceChoice1113215 = new DeviceChoice();
	JPanel jPanel1112210 = new JPanel();
	DeviceChoice deviceChoice211112210 = new DeviceChoice();
	DeviceChannel deviceChannel1228 = new DeviceChannel();
	JPanel jPanel121125 = new JPanel();
	GridBagLayout gridBagLayout2111 = new GridBagLayout();
	DeviceChoice deviceChoice1113216 = new DeviceChoice();
	DeviceChoice deviceChoice1111217 = new DeviceChoice();
	DeviceChannel deviceChannel1229 = new DeviceChannel();
	DeviceChoice deviceChoice2111217 = new DeviceChoice();
	DeviceChoice deviceChoice211112211 = new DeviceChoice();
	JPanel jPanel1112211 = new JPanel();
	DeviceChoice deviceChoice1113217 = new DeviceChoice();
	DeviceChoice deviceChoice1113218 = new DeviceChoice();
	JPanel jPanel1112212 = new JPanel();
	DeviceChoice deviceChoice211112212 = new DeviceChoice();
	DeviceChannel deviceChannel12210 = new DeviceChannel();
	JPanel jPanel121126 = new JPanel();
	GridBagLayout gridBagLayout2112 = new GridBagLayout();
	DeviceChoice deviceChoice1113219 = new DeviceChoice();
	DeviceChoice deviceChoice1111218 = new DeviceChoice();
	DeviceChannel deviceChannel12211 = new DeviceChannel();
	DeviceChoice deviceChoice2111218 = new DeviceChoice();
	DeviceChoice deviceChoice211112213 = new DeviceChoice();
	JPanel jPanel1112213 = new JPanel();
	DeviceChoice deviceChoice11132110 = new DeviceChoice();
	DeviceChoice deviceChoice11132111 = new DeviceChoice();
	JPanel jPanel1112214 = new JPanel();
	DeviceChoice deviceChoice211112214 = new DeviceChoice();
	DeviceChannel deviceChannel12212 = new DeviceChannel();
	JPanel jPanel121127 = new JPanel();
	GridBagLayout gridBagLayout2113 = new GridBagLayout();
	DeviceChoice deviceChoice11132112 = new DeviceChoice();
	DeviceChoice deviceChoice1111219 = new DeviceChoice();
	DeviceChannel deviceChannel12213 = new DeviceChannel();
	DeviceChoice deviceChoice2111219 = new DeviceChoice();
	DeviceChoice deviceChoice211112215 = new DeviceChoice();
	JPanel jPanel1112215 = new JPanel();
	DeviceChoice deviceChoice11132113 = new DeviceChoice();
	DeviceChoice deviceChoice1111220 = new DeviceChoice();
	DeviceChoice deviceChoice11112110 = new DeviceChoice();
	DeviceChoice deviceChoice1111221 = new DeviceChoice();
	DeviceChoice deviceChoice1111222 = new DeviceChoice();
	DeviceChoice deviceChoice1111223 = new DeviceChoice();
	DeviceChoice deviceChoice1111224 = new DeviceChoice();
	DeviceChoice deviceChoice1111225 = new DeviceChoice();
	DeviceChoice deviceChoice1111226 = new DeviceChoice();
	DeviceChoice deviceChoice1111227 = new DeviceChoice();
	DeviceChoice deviceChoice1111228 = new DeviceChoice();
	DeviceChoice deviceChoice1111229 = new DeviceChoice();
	DeviceChoice deviceChoice11112111 = new DeviceChoice();
	DeviceChoice deviceChoice11112112 = new DeviceChoice();
	DeviceChoice deviceChoice11112113 = new DeviceChoice();
	DeviceChoice deviceChoice11112114 = new DeviceChoice();
	DeviceChoice deviceChoice11112115 = new DeviceChoice();
	DeviceChoice deviceChoice11112116 = new DeviceChoice();
	DeviceChoice deviceChoice11112117 = new DeviceChoice();
	DeviceChoice deviceChoice11112118 = new DeviceChoice();
	DeviceChoice deviceChoice11112119 = new DeviceChoice();

	public RFXANSALDOSetup()
	{
		try
		{
			jbInit();
		}
		catch (final Exception e)
		{
			e.printStackTrace();
		}
	}

	private void jbInit() throws Exception
	{
		titledBorder1 = new TitledBorder("");
		titledBorder2 = new TitledBorder("");
		this.setWidth(1069);
		this.setHeight(680);
		this.setDeviceType("RFXANSALDO");
		this.setDeviceProvider("150.178.3.10");
		this.setDeviceTitle("RfxAnsaldo");
		deviceButtons1.setMethods(new String[]
		{ "SendAnsaldo" });
		jPanel1.setLayout(borderLayout1);
		jPanel2.setLayout(flowLayout1);
		deviceField1.setOffsetNid(1);
		deviceField1.setLabelString("Comment:");
		deviceField1.setNumCols(40);
		deviceField1.setIdentifier("");
		// deviceField2.setLabelString("Confi");
		flowLayout2111112.setAlignment(FlowLayout.LEFT);
		flowLayout2111113.setAlignment(FlowLayout.LEFT);
		flowLayout2111125.setAlignment(FlowLayout.LEFT);
		flowLayout2111114.setAlignment(FlowLayout.LEFT);
		flowLayout2111115.setAlignment(FlowLayout.LEFT);
		flowLayout2111126.setAlignment(FlowLayout.LEFT);
		flowLayout2111116.setAlignment(FlowLayout.LEFT);
		flowLayout7.setAlignment(FlowLayout.LEFT);
		flowLayout2111117.setAlignment(FlowLayout.LEFT);
		flowLayout2111118.setAlignment(FlowLayout.LEFT);
		flowLayout8.setAlignment(FlowLayout.LEFT);
		flowLayout2111127.setAlignment(FlowLayout.LEFT);
		flowLayout2111128.setAlignment(FlowLayout.LEFT);
		flowLayout2111119.setAlignment(FlowLayout.LEFT);
		flowLayout9.setAlignment(FlowLayout.LEFT);
		flowLayout11.setAlignment(FlowLayout.LEFT);
		flowLayout12.setAlignment(FlowLayout.LEFT);
		flowLayout2111120.setAlignment(FlowLayout.LEFT);
		flowLayout21111110.setAlignment(FlowLayout.LEFT);
		flowLayout21111111.setAlignment(FlowLayout.LEFT);
		flowLayout2111129.setAlignment(FlowLayout.LEFT);
		flowLayout21111210.setAlignment(FlowLayout.LEFT);
		flowLayout2111121.setAlignment(FlowLayout.LEFT);
		flowLayout2111122.setAlignment(FlowLayout.LEFT);
		flowLayout21111211.setAlignment(FlowLayout.LEFT);
		flowLayout2111130.setAlignment(FlowLayout.LEFT);
		flowLayout21111112.setAlignment(FlowLayout.LEFT);
		flowLayout21111113.setAlignment(FlowLayout.LEFT);
		flowLayout6.setAlignment(FlowLayout.LEFT);
		flowLayout14.setAlignment(FlowLayout.LEFT);
		flowLayout2111131.setAlignment(FlowLayout.LEFT);
		flowLayout21111212.setAlignment(FlowLayout.LEFT);
		flowLayout21111213.setAlignment(FlowLayout.LEFT);
		flowLayout2111132.setAlignment(FlowLayout.LEFT);
		flowLayout21111114.setAlignment(FlowLayout.LEFT);
		flowLayout21111115.setAlignment(FlowLayout.LEFT);
		flowLayout13.setAlignment(FlowLayout.LEFT);
		flowLayout17.setAlignment(FlowLayout.LEFT);
		flowLayout2111133.setAlignment(FlowLayout.LEFT);
		flowLayout21111214.setAlignment(FlowLayout.LEFT);
		flowLayout21111215.setAlignment(FlowLayout.LEFT);
		flowLayout2111134.setAlignment(FlowLayout.LEFT);
		UNITS_A.setLayout(gridLayout1);
		gridLayout1.setColumns(2);
		gridLayout1.setRows(1);
		jPanel4.setBorder(BorderFactory.createEtchedBorder());
		jPanel4.setMaximumSize(new Dimension(2147483647, 2147483647));
		jPanel4.setPreferredSize(new Dimension(908, 473));
		jPanel4.setLayout(borderLayout3);
		jPanel7.setLayout(flowLayout2);
		jLabel1.setText("CONFIGURATION    ");
		jLabel2.setText("        ENABLE UNIT");
		jLabel3.setText("    REGULATION ");
		jLabel4.setText(" WINDOW");
		jLabel5.setText(" PROTECTION");
		flowLayout2.setAlignment(FlowLayout.LEFT);
		flowLayout2.setHgap(10);
		gridLayout2.setColumns(2);
		gridLayout2.setRows(6);
		gridLayout2.setVgap(0);
		jPanel8.setLayout(gridLayout2);
		jPanel8.setPreferredSize(new Dimension(1000, 528));
		jLabel6.setText("     PROTECTION      ");
		jLabel7.setText("WINDOW");
		jLabel8.setText("CONFIGURATION    ");
		jLabel9.setText("     REGULATION    ");
		jLabel10.setText("ENABLE UNIT  ");
		flowLayout3.setAlignment(FlowLayout.LEFT);
		jLabel11.setText("      REGULATION");
		jPanel9.setLayout(flowLayout3);
		jLabel12.setText("CONFIGURATION    ");
		jPanel5.setBorder(BorderFactory.createEtchedBorder());
		jPanel5.setLayout(borderLayout4);
		jLabel13.setText("CONFIGURATION    ");
		jLabel14.setText("             ENABLE UNIT  ");
		jPanel10.setLayout(gridLayout3);
		jPanel10.setPreferredSize(new Dimension(1000, 528));
		jLabel15.setText("WINDOW");
		jLabel16.setText("ENABLE UNIT  ");
		gridLayout3.setColumns(2);
		gridLayout3.setRows(6);
		gridLayout3.setVgap(0);
		jLabel17.setText("       PROTECTION      ");
		jLabel18.setText("        PROTECTION");
		jLabel19.setText(" WINDOW");
		jLabel20.setText("      REGULATION ");
		gridLayout4.setColumns(2);
		gridLayout4.setRows(1);
		UNITS_B.setLayout(gridLayout4);
		deviceChoice2111120.setChoiceIntValues(null);
		deviceChoice2111120.setChoiceFloatValues(null);
		deviceChoice2111120.setOffsetNid(7);
		deviceChoice2111120.setLabelString("");
		deviceChoice2111120.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice2111120.setUpdateIdentifier("");
		deviceChoice2111120.setIdentifier("");
		deviceChoice11130.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice11130.setUpdateIdentifier("");
		deviceChoice11130.setIdentifier("");
		deviceChoice11130.setChoiceIntValues(null);
		deviceChoice11130.setChoiceFloatValues(null);
		deviceChoice11130.setOffsetNid(11);
		deviceChoice11130.setLabelString("");
		deviceChoice2111121.setChoiceIntValues(null);
		deviceChoice2111121.setChoiceFloatValues(null);
		deviceChoice2111121.setOffsetNid(9);
		deviceChoice2111121.setLabelString("");
		deviceChoice2111121.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice2111121.setUpdateIdentifier("");
		deviceChoice2111121.setIdentifier("");
		deviceChoice211120.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice211120.setUpdateIdentifier("");
		deviceChoice211120.setIdentifier("");
		deviceChoice211120.setChoiceIntValues(null);
		deviceChoice211120.setChoiceFloatValues(null);
		deviceChoice211120.setOffsetNid(8);
		deviceChoice211120.setLabelString("");
		jPanel12118.setLayout(gridBagLayout24);
		jPanel12118.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel12118.setMinimumSize(new Dimension(400, 88));
		jPanel12118.setPreferredSize(new Dimension(1001, 120));
		deviceChannel124.setLabelString("A01_S");
		deviceChannel124.setOffsetNid(6);
		deviceChannel124.setLines(1);
		deviceChannel124.setInSameLine(true);
		deviceChannel124.setUpdateIdentifier("");
		deviceChannel124.setShowVal("");
		deviceChoice111120.setChoiceIntValues(null);
		deviceChoice111120.setChoiceFloatValues(null);
		deviceChoice111120.setOffsetNid(5);
		deviceChoice111120.setLabelString("      ");
		deviceChoice111120.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice111120.setUpdateIdentifier("");
		deviceChoice111120.setIdentifier("");
		deviceChoice11131.setChoiceIntValues(null);
		deviceChoice11131.setChoiceFloatValues(null);
		deviceChoice11131.setOffsetNid(12);
		deviceChoice11131.setLabelString("");
		deviceChoice11131.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice11131.setUpdateIdentifier("");
		deviceChoice11131.setIdentifier("");
		deviceChannel127.setInSameLine(true);
		deviceChannel127.setUpdateIdentifier("");
		deviceChannel127.setShowVal("");
		deviceChannel127.setLines(1);
		deviceChannel127.setLabelString("A01_T");
		deviceChannel127.setOffsetNid(10);
		deviceChoice11132.setChoiceIntValues(null);
		deviceChoice11132.setChoiceFloatValues(null);
		deviceChoice11132.setOffsetNid(13);
		deviceChoice11132.setLabelString("");
		deviceChoice11132.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice11132.setUpdateIdentifier("");
		deviceChoice11132.setIdentifier("");
		deviceChoice11133.setOffsetNid(121);
		deviceChoice11133.setLabelString("");
		deviceChoice11133.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice2111122.setOffsetNid(117);
		deviceChoice2111122.setLabelString("");
		deviceChoice2111122.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel128.setInSameLine(true);
		deviceChannel128.setLines(1);
		deviceChannel128.setLabelString("A12_T");
		deviceChannel128.setOffsetNid(120);
		jPanel12119.setLayout(gridBagLayout25);
		jPanel12119.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel12119.setMinimumSize(new Dimension(400, 88));
		jPanel12119.setPreferredSize(new Dimension(1001, 120));
		deviceChoice11134.setOffsetNid(122);
		deviceChoice11134.setLabelString("");
		deviceChoice11134.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice111121.setOffsetNid(115);
		deviceChoice111121.setLabelString("      ");
		deviceChoice111121.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChannel129.setLabelString("A12_S");
		deviceChannel129.setOffsetNid(116);
		deviceChannel129.setLines(1);
		deviceChannel129.setInSameLine(true);
		deviceChoice211121.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice211121.setOffsetNid(118);
		deviceChoice211121.setLabelString("");
		deviceChoice2111123.setOffsetNid(119);
		deviceChoice2111123.setLabelString("");
		deviceChoice2111123.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice11135.setOffsetNid(123);
		deviceChoice11135.setLabelString("");
		deviceChoice11135.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice11136.setOffsetNid(61);
		deviceChoice11136.setLabelString("");
		deviceChoice11136.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice2111124.setOffsetNid(57);
		deviceChoice2111124.setLabelString("");
		deviceChoice2111124.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel1210.setInSameLine(true);
		deviceChannel1210.setLines(1);
		deviceChannel1210.setLabelString("A06_T");
		deviceChannel1210.setOffsetNid(60);
		jPanel121110.setLayout(gridBagLayout26);
		jPanel121110.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121110.setMinimumSize(new Dimension(400, 88));
		jPanel121110.setPreferredSize(new Dimension(1001, 120));
		deviceChoice11137.setOffsetNid(62);
		deviceChoice11137.setLabelString("");
		deviceChoice11137.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice111122.setOffsetNid(55);
		deviceChoice111122.setLabelString("      ");
		deviceChoice111122.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChannel1211.setLabelString("A06_S");
		deviceChannel1211.setOffsetNid(56);
		deviceChannel1211.setLines(1);
		deviceChannel1211.setInSameLine(true);
		deviceChoice211122.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice211122.setOffsetNid(58);
		deviceChoice211122.setLabelString("");
		deviceChoice2111125.setOffsetNid(59);
		deviceChoice2111125.setLabelString("");
		deviceChoice2111125.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice11138.setOffsetNid(63);
		deviceChoice11138.setLabelString("");
		deviceChoice11138.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice11139.setOffsetNid(111);
		deviceChoice11139.setLabelString("");
		deviceChoice11139.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice2111126.setOffsetNid(107);
		deviceChoice2111126.setLabelString("");
		deviceChoice2111126.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel1212.setInSameLine(true);
		deviceChannel1212.setLines(1);
		deviceChannel1212.setLabelString("A11_T");
		deviceChannel1212.setOffsetNid(110);
		jPanel121111.setLayout(gridBagLayout27);
		jPanel121111.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121111.setMinimumSize(new Dimension(400, 88));
		jPanel121111.setPreferredSize(new Dimension(1001, 120));
		deviceChoice111310.setOffsetNid(112);
		deviceChoice111310.setLabelString("");
		deviceChoice111310.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice111123.setOffsetNid(105);
		deviceChoice111123.setLabelString("      ");
		deviceChoice111123.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChannel1213.setLabelString("A11_S");
		deviceChannel1213.setOffsetNid(106);
		deviceChannel1213.setLines(1);
		deviceChannel1213.setInSameLine(true);
		deviceChoice211123.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice211123.setOffsetNid(108);
		deviceChoice211123.setLabelString("");
		deviceChoice2111127.setOffsetNid(109);
		deviceChoice2111127.setLabelString("");
		deviceChoice2111127.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice111311.setOffsetNid(113);
		deviceChoice111311.setLabelString("");
		deviceChoice111311.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice111312.setOffsetNid(51);
		deviceChoice111312.setLabelString("");
		deviceChoice111312.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice2111128.setOffsetNid(47);
		deviceChoice2111128.setLabelString("");
		deviceChoice2111128.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel1214.setInSameLine(true);
		deviceChannel1214.setLines(1);
		deviceChannel1214.setLabelString("A05_T");
		deviceChannel1214.setOffsetNid(50);
		jPanel121112.setLayout(gridBagLayout28);
		jPanel121112.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121112.setMinimumSize(new Dimension(400, 88));
		jPanel121112.setPreferredSize(new Dimension(1001, 120));
		deviceChoice111313.setOffsetNid(52);
		deviceChoice111313.setLabelString("");
		deviceChoice111313.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice111124.setOffsetNid(45);
		deviceChoice111124.setLabelString("      ");
		deviceChoice111124.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChannel1215.setLabelString("A05_S");
		deviceChannel1215.setOffsetNid(46);
		deviceChannel1215.setLines(1);
		deviceChannel1215.setInSameLine(true);
		deviceChoice211124.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice211124.setOffsetNid(48);
		deviceChoice211124.setLabelString("");
		deviceChoice2111129.setOffsetNid(49);
		deviceChoice2111129.setLabelString("");
		deviceChoice2111129.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice111314.setOffsetNid(53);
		deviceChoice111314.setLabelString("");
		deviceChoice111314.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice111315.setOffsetNid(101);
		deviceChoice111315.setLabelString("");
		deviceChoice111315.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice21111210.setOffsetNid(97);
		deviceChoice21111210.setLabelString("");
		deviceChoice21111210.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel1216.setInSameLine(true);
		deviceChannel1216.setLines(1);
		deviceChannel1216.setLabelString("A10_T");
		deviceChannel1216.setOffsetNid(100);
		jPanel121113.setLayout(gridBagLayout29);
		jPanel121113.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121113.setMinimumSize(new Dimension(400, 88));
		jPanel121113.setPreferredSize(new Dimension(1001, 120));
		deviceChoice111316.setOffsetNid(102);
		deviceChoice111316.setLabelString("");
		deviceChoice111316.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice111125.setOffsetNid(95);
		deviceChoice111125.setLabelString("      ");
		deviceChoice111125.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChannel1217.setLabelString("A10_S");
		deviceChannel1217.setOffsetNid(96);
		deviceChannel1217.setLines(1);
		deviceChannel1217.setInSameLine(true);
		deviceChoice211125.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice211125.setOffsetNid(98);
		deviceChoice211125.setLabelString("");
		deviceChoice21111211.setOffsetNid(99);
		deviceChoice21111211.setLabelString("");
		deviceChoice21111211.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice111317.setOffsetNid(103);
		deviceChoice111317.setLabelString("");
		deviceChoice111317.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice111318.setOffsetNid(41);
		deviceChoice111318.setLabelString("");
		deviceChoice111318.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice21111212.setOffsetNid(37);
		deviceChoice21111212.setLabelString("");
		deviceChoice21111212.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel1218.setInSameLine(true);
		deviceChannel1218.setLines(1);
		deviceChannel1218.setLabelString("A04_T");
		deviceChannel1218.setOffsetNid(40);
		jPanel121114.setLayout(gridBagLayout210);
		jPanel121114.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121114.setMinimumSize(new Dimension(400, 88));
		jPanel121114.setPreferredSize(new Dimension(1001, 120));
		deviceChoice111319.setOffsetNid(42);
		deviceChoice111319.setLabelString("");
		deviceChoice111319.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice111126.setOffsetNid(35);
		deviceChoice111126.setLabelString("      ");
		deviceChoice111126.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChannel1219.setLabelString("A04_S");
		deviceChannel1219.setOffsetNid(36);
		deviceChannel1219.setLines(1);
		deviceChannel1219.setInSameLine(true);
		deviceChoice211126.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice211126.setOffsetNid(38);
		deviceChoice211126.setLabelString("");
		deviceChoice21111213.setOffsetNid(39);
		deviceChoice21111213.setLabelString("");
		deviceChoice21111213.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice1113110.setOffsetNid(43);
		deviceChoice1113110.setLabelString("");
		deviceChoice1113110.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice1113111.setOffsetNid(91);
		deviceChoice1113111.setLabelString("");
		deviceChoice1113111.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice21111214.setOffsetNid(87);
		deviceChoice21111214.setLabelString("");
		deviceChoice21111214.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel12110.setInSameLine(true);
		deviceChannel12110.setLines(1);
		deviceChannel12110.setLabelString("A09_T");
		deviceChannel12110.setOffsetNid(90);
		jPanel121115.setLayout(gridBagLayout211);
		jPanel121115.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121115.setMinimumSize(new Dimension(400, 88));
		jPanel121115.setPreferredSize(new Dimension(1001, 120));
		deviceChoice1113112.setOffsetNid(92);
		deviceChoice1113112.setLabelString("");
		deviceChoice1113112.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice111127.setOffsetNid(85);
		deviceChoice111127.setLabelString("      ");
		deviceChoice111127.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChannel12111.setLabelString("A09_S");
		deviceChannel12111.setOffsetNid(86);
		deviceChannel12111.setLines(1);
		deviceChannel12111.setInSameLine(true);
		deviceChoice211127.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice211127.setOffsetNid(88);
		deviceChoice211127.setLabelString("");
		deviceChoice21111215.setOffsetNid(89);
		deviceChoice21111215.setLabelString("");
		deviceChoice21111215.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice1113113.setOffsetNid(93);
		deviceChoice1113113.setLabelString("");
		deviceChoice1113113.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice1113114.setOffsetNid(31);
		deviceChoice1113114.setLabelString("");
		deviceChoice1113114.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice21111216.setOffsetNid(27);
		deviceChoice21111216.setLabelString("");
		deviceChoice21111216.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel12112.setInSameLine(true);
		deviceChannel12112.setLines(1);
		deviceChannel12112.setLabelString("A03_T");
		deviceChannel12112.setOffsetNid(30);
		jPanel121116.setLayout(gridBagLayout212);
		jPanel121116.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121116.setMinimumSize(new Dimension(400, 88));
		jPanel121116.setPreferredSize(new Dimension(1001, 120));
		deviceChoice1113115.setOffsetNid(32);
		deviceChoice1113115.setLabelString("");
		deviceChoice1113115.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice111128.setChoiceIntValues(null);
		deviceChoice111128.setChoiceFloatValues(null);
		deviceChoice111128.setOffsetNid(25);
		deviceChoice111128.setLabelString("      ");
		deviceChoice111128.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice111128.setUpdateIdentifier("");
		deviceChoice111128.setIdentifier("");
		deviceChannel12113.setLabelString("A03_S");
		deviceChannel12113.setOffsetNid(26);
		deviceChannel12113.setLines(1);
		deviceChannel12113.setInSameLine(true);
		deviceChoice211128.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice211128.setOffsetNid(28);
		deviceChoice211128.setLabelString("");
		deviceChoice21111217.setOffsetNid(29);
		deviceChoice21111217.setLabelString("");
		deviceChoice21111217.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice1113116.setOffsetNid(33);
		deviceChoice1113116.setLabelString("");
		deviceChoice1113116.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice1113117.setOffsetNid(81);
		deviceChoice1113117.setLabelString("");
		deviceChoice1113117.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice21111218.setOffsetNid(77);
		deviceChoice21111218.setLabelString("");
		deviceChoice21111218.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel12114.setInSameLine(true);
		deviceChannel12114.setLines(1);
		deviceChannel12114.setLabelString("A08_T");
		deviceChannel12114.setOffsetNid(80);
		jPanel121117.setLayout(gridBagLayout213);
		jPanel121117.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121117.setMinimumSize(new Dimension(400, 88));
		jPanel121117.setPreferredSize(new Dimension(1001, 120));
		deviceChoice1113118.setOffsetNid(82);
		deviceChoice1113118.setLabelString("");
		deviceChoice1113118.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice111129.setOffsetNid(75);
		deviceChoice111129.setLabelString("      ");
		deviceChoice111129.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChannel12115.setLabelString("A08_S");
		deviceChannel12115.setOffsetNid(76);
		deviceChannel12115.setLines(1);
		deviceChannel12115.setInSameLine(true);
		deviceChoice211129.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice211129.setOffsetNid(78);
		deviceChoice211129.setLabelString("");
		deviceChoice21111219.setOffsetNid(79);
		deviceChoice21111219.setLabelString("");
		deviceChoice21111219.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice1113119.setOffsetNid(83);
		deviceChoice1113119.setLabelString("");
		deviceChoice1113119.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice11131110.setChoiceIntValues(null);
		deviceChoice11131110.setChoiceFloatValues(null);
		deviceChoice11131110.setOffsetNid(21);
		deviceChoice11131110.setLabelString("");
		deviceChoice11131110.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice11131110.setUpdateIdentifier("");
		deviceChoice11131110.setIdentifier("");
		deviceChoice211112110.setChoiceIntValues(null);
		deviceChoice211112110.setChoiceFloatValues(null);
		deviceChoice211112110.setOffsetNid(17);
		deviceChoice211112110.setLabelString("");
		deviceChoice211112110.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice211112110.setUpdateIdentifier("");
		deviceChoice211112110.setIdentifier("");
		deviceChannel12116.setInSameLine(true);
		deviceChannel12116.setUpdateIdentifier("");
		deviceChannel12116.setShowVal("");
		deviceChannel12116.setLines(1);
		deviceChannel12116.setLabelString("A02_T");
		deviceChannel12116.setOffsetNid(20);
		jPanel121118.setLayout(gridBagLayout214);
		jPanel121118.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121118.setMinimumSize(new Dimension(400, 88));
		jPanel121118.setPreferredSize(new Dimension(1001, 120));
		deviceChoice11131111.setChoiceIntValues(null);
		deviceChoice11131111.setChoiceFloatValues(null);
		deviceChoice11131111.setOffsetNid(22);
		deviceChoice11131111.setLabelString("");
		deviceChoice11131111.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice11131111.setUpdateIdentifier("");
		deviceChoice11131111.setIdentifier("");
		deviceChoice1111210.setChoiceIntValues(null);
		deviceChoice1111210.setChoiceFloatValues(null);
		deviceChoice1111210.setOffsetNid(15);
		deviceChoice1111210.setLabelString("      ");
		deviceChoice1111210.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice1111210.setUpdateIdentifier("");
		deviceChoice1111210.setIdentifier("");
		deviceChannel12117.setLabelString("A02_S");
		deviceChannel12117.setOffsetNid(16);
		deviceChannel12117.setLines(1);
		deviceChannel12117.setInSameLine(true);
		deviceChannel12117.setUpdateIdentifier("");
		deviceChannel12117.setShowVal("");
		deviceChoice2111210.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice2111210.setUpdateIdentifier("");
		deviceChoice2111210.setIdentifier("");
		deviceChoice2111210.setChoiceIntValues(null);
		deviceChoice2111210.setChoiceFloatValues(null);
		deviceChoice2111210.setOffsetNid(18);
		deviceChoice2111210.setLabelString("");
		deviceChoice211112111.setChoiceIntValues(null);
		deviceChoice211112111.setChoiceFloatValues(null);
		deviceChoice211112111.setOffsetNid(19);
		deviceChoice211112111.setLabelString("");
		deviceChoice211112111.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice211112111.setUpdateIdentifier("");
		deviceChoice211112111.setIdentifier("");
		deviceChoice11131112.setChoiceIntValues(null);
		deviceChoice11131112.setChoiceFloatValues(null);
		deviceChoice11131112.setOffsetNid(23);
		deviceChoice11131112.setLabelString("");
		deviceChoice11131112.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice11131112.setUpdateIdentifier("");
		deviceChoice11131112.setIdentifier("");
		deviceChoice11131113.setOffsetNid(71);
		deviceChoice11131113.setLabelString("");
		deviceChoice11131113.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice211112112.setOffsetNid(67);
		deviceChoice211112112.setLabelString("");
		deviceChoice211112112.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel12118.setInSameLine(true);
		deviceChannel12118.setLines(1);
		deviceChannel12118.setLabelString("A07_T");
		deviceChannel12118.setOffsetNid(70);
		jPanel121119.setLayout(gridBagLayout215);
		jPanel121119.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121119.setMinimumSize(new Dimension(400, 88));
		jPanel121119.setPreferredSize(new Dimension(1001, 120));
		deviceChoice11131114.setOffsetNid(72);
		deviceChoice11131114.setLabelString("");
		deviceChoice11131114.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice1111211.setOffsetNid(65);
		deviceChoice1111211.setLabelString("      ");
		deviceChoice1111211.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChannel12119.setLabelString("A07_S");
		deviceChannel12119.setOffsetNid(66);
		deviceChannel12119.setLines(1);
		deviceChannel12119.setInSameLine(true);
		deviceChoice2111211.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice2111211.setOffsetNid(68);
		deviceChoice2111211.setLabelString("");
		deviceChoice211112113.setOffsetNid(69);
		deviceChoice211112113.setLabelString("");
		deviceChoice211112113.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice11131115.setOffsetNid(73);
		deviceChoice11131115.setLabelString("");
		deviceChoice11131115.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice111320.setOffsetNid(131);
		deviceChoice111320.setLabelString("");
		deviceChoice111320.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice21111220.setOffsetNid(127);
		deviceChoice21111220.setLabelString("");
		deviceChoice21111220.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel1220.setInSameLine(true);
		deviceChannel1220.setUpdateIdentifier("");
		deviceChannel1220.setShowVal("");
		deviceChannel1220.setLines(1);
		deviceChannel1220.setLabelString("B01_T");
		deviceChannel1220.setOffsetNid(130);
		jPanel121120.setLayout(gridBagLayout216);
		jPanel121120.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121120.setMinimumSize(new Dimension(400, 88));
		jPanel121120.setPreferredSize(new Dimension(1001, 120));
		deviceChoice111321.setOffsetNid(132);
		deviceChoice111321.setLabelString("");
		deviceChoice111321.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice1111212.setChoiceIntValues(null);
		deviceChoice1111212.setChoiceFloatValues(null);
		deviceChoice1111212.setOffsetNid(125);
		deviceChoice1111212.setLabelString("      ");
		deviceChoice1111212.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice1111212.setUpdateIdentifier("");
		deviceChoice1111212.setIdentifier("");
		deviceChannel125.setLabelString("B01_S");
		deviceChannel125.setOffsetNid(126);
		deviceChannel125.setLines(1);
		deviceChannel125.setInSameLine(true);
		deviceChannel125.setUpdateIdentifier("");
		deviceChannel125.setShowVal("");
		deviceChoice2111212.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice2111212.setOffsetNid(128);
		deviceChoice2111212.setLabelString("");
		deviceChoice21111221.setOffsetNid(129);
		deviceChoice21111221.setLabelString("");
		deviceChoice21111221.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice111322.setOffsetNid(133);
		deviceChoice111322.setLabelString("");
		deviceChoice111322.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice111323.setOffsetNid(201);
		deviceChoice111323.setLabelString("");
		deviceChoice111323.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice21111222.setOffsetNid(197);
		deviceChoice21111222.setLabelString("");
		deviceChoice21111222.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel1221.setInSameLine(true);
		deviceChannel1221.setLines(1);
		deviceChannel1221.setLabelString("B08_T");
		deviceChannel1221.setOffsetNid(200);
		jPanel121121.setLayout(gridBagLayout217);
		jPanel121121.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121121.setMinimumSize(new Dimension(400, 88));
		jPanel121121.setPreferredSize(new Dimension(1001, 120));
		deviceChoice111324.setOffsetNid(202);
		deviceChoice111324.setLabelString("");
		deviceChoice111324.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice1111213.setOffsetNid(195);
		deviceChoice1111213.setLabelString("      ");
		deviceChoice1111213.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChannel126.setLabelString("B08_S");
		deviceChannel126.setOffsetNid(196);
		deviceChannel126.setLines(1);
		deviceChannel126.setInSameLine(true);
		deviceChoice2111213.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice2111213.setOffsetNid(198);
		deviceChoice2111213.setLabelString("");
		deviceChoice21111223.setOffsetNid(199);
		deviceChoice21111223.setLabelString("");
		deviceChoice21111223.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice111325.setOffsetNid(203);
		deviceChoice111325.setLabelString("");
		deviceChoice111325.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice111326.setOffsetNid(161);
		deviceChoice111326.setLabelString("");
		deviceChoice111326.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice21111224.setOffsetNid(157);
		deviceChoice21111224.setLabelString("");
		deviceChoice21111224.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel1222.setInSameLine(true);
		deviceChannel1222.setLines(1);
		deviceChannel1222.setLabelString("B04_T");
		deviceChannel1222.setOffsetNid(160);
		jPanel121122.setLayout(gridBagLayout218);
		jPanel121122.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121122.setMinimumSize(new Dimension(400, 88));
		jPanel121122.setPreferredSize(new Dimension(1001, 120));
		deviceChoice111327.setOffsetNid(162);
		deviceChoice111327.setLabelString("");
		deviceChoice111327.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice1111214.setOffsetNid(155);
		deviceChoice1111214.setLabelString("      ");
		deviceChoice1111214.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChannel1223.setLabelString("B04_S");
		deviceChannel1223.setOffsetNid(156);
		deviceChannel1223.setLines(1);
		deviceChannel1223.setInSameLine(true);
		deviceChoice2111214.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice2111214.setOffsetNid(158);
		deviceChoice2111214.setLabelString("");
		deviceChoice21111225.setOffsetNid(159);
		deviceChoice21111225.setLabelString("");
		deviceChoice21111225.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice111328.setOffsetNid(163);
		deviceChoice111328.setLabelString("");
		deviceChoice111328.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice111329.setOffsetNid(191);
		deviceChoice111329.setLabelString("");
		deviceChoice111329.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice21111226.setOffsetNid(187);
		deviceChoice21111226.setLabelString("");
		deviceChoice21111226.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel1224.setInSameLine(true);
		deviceChannel1224.setLines(1);
		deviceChannel1224.setLabelString("B07_T");
		deviceChannel1224.setOffsetNid(190);
		jPanel121123.setLayout(gridBagLayout219);
		jPanel121123.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121123.setMinimumSize(new Dimension(400, 88));
		jPanel121123.setPreferredSize(new Dimension(1001, 120));
		deviceChoice1113210.setOffsetNid(192);
		deviceChoice1113210.setLabelString("");
		deviceChoice1113210.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice1111215.setOffsetNid(185);
		deviceChoice1111215.setLabelString("      ");
		deviceChoice1111215.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChannel1225.setLabelString("B07_S");
		deviceChannel1225.setOffsetNid(186);
		deviceChannel1225.setLines(1);
		deviceChannel1225.setInSameLine(true);
		deviceChoice2111215.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice2111215.setOffsetNid(188);
		deviceChoice2111215.setLabelString("");
		deviceChoice21111227.setOffsetNid(189);
		deviceChoice21111227.setLabelString("");
		deviceChoice21111227.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice1113211.setOffsetNid(193);
		deviceChoice1113211.setLabelString("");
		deviceChoice1113211.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice1113212.setOffsetNid(151);
		deviceChoice1113212.setLabelString("");
		deviceChoice1113212.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice21111228.setOffsetNid(147);
		deviceChoice21111228.setLabelString("");
		deviceChoice21111228.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel1226.setInSameLine(true);
		deviceChannel1226.setLines(1);
		deviceChannel1226.setLabelString("B03_T");
		deviceChannel1226.setOffsetNid(150);
		jPanel121124.setLayout(gridBagLayout2110);
		jPanel121124.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121124.setMinimumSize(new Dimension(400, 88));
		jPanel121124.setPreferredSize(new Dimension(1001, 120));
		deviceChoice1113213.setOffsetNid(152);
		deviceChoice1113213.setLabelString("");
		deviceChoice1113213.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice1111216.setOffsetNid(145);
		deviceChoice1111216.setLabelString("      ");
		deviceChoice1111216.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChannel1227.setLabelString("B03_S");
		deviceChannel1227.setOffsetNid(146);
		deviceChannel1227.setLines(1);
		deviceChannel1227.setInSameLine(true);
		deviceChoice2111216.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice2111216.setOffsetNid(148);
		deviceChoice2111216.setLabelString("");
		deviceChoice21111229.setOffsetNid(149);
		deviceChoice21111229.setLabelString("");
		deviceChoice21111229.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice1113214.setOffsetNid(153);
		deviceChoice1113214.setLabelString("");
		deviceChoice1113214.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice1113215.setOffsetNid(181);
		deviceChoice1113215.setLabelString("");
		deviceChoice1113215.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice211112210.setOffsetNid(177);
		deviceChoice211112210.setLabelString("");
		deviceChoice211112210.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel1228.setInSameLine(true);
		deviceChannel1228.setLines(1);
		deviceChannel1228.setLabelString("B06_T");
		deviceChannel1228.setOffsetNid(180);
		jPanel121125.setLayout(gridBagLayout2111);
		jPanel121125.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121125.setMinimumSize(new Dimension(400, 88));
		jPanel121125.setPreferredSize(new Dimension(1001, 120));
		deviceChoice1113216.setOffsetNid(182);
		deviceChoice1113216.setLabelString("");
		deviceChoice1113216.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice1111217.setOffsetNid(175);
		deviceChoice1111217.setLabelString("      ");
		deviceChoice1111217.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChannel1229.setLabelString("B06_S");
		deviceChannel1229.setOffsetNid(176);
		deviceChannel1229.setLines(1);
		deviceChannel1229.setInSameLine(true);
		deviceChoice2111217.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice2111217.setOffsetNid(178);
		deviceChoice2111217.setLabelString("");
		deviceChoice211112211.setOffsetNid(179);
		deviceChoice211112211.setLabelString("");
		deviceChoice211112211.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice1113217.setOffsetNid(183);
		deviceChoice1113217.setLabelString("");
		deviceChoice1113217.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice1113218.setOffsetNid(141);
		deviceChoice1113218.setLabelString("");
		deviceChoice1113218.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice211112212.setOffsetNid(137);
		deviceChoice211112212.setLabelString("");
		deviceChoice211112212.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel12210.setInSameLine(true);
		deviceChannel12210.setLines(1);
		deviceChannel12210.setLabelString("B02_T");
		deviceChannel12210.setOffsetNid(140);
		jPanel121126.setLayout(gridBagLayout2112);
		jPanel121126.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121126.setMinimumSize(new Dimension(400, 88));
		jPanel121126.setPreferredSize(new Dimension(1001, 120));
		deviceChoice1113219.setOffsetNid(142);
		deviceChoice1113219.setLabelString("");
		deviceChoice1113219.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice1111218.setOffsetNid(135);
		deviceChoice1111218.setLabelString("      ");
		deviceChoice1111218.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChannel12211.setLabelString("B02_S");
		deviceChannel12211.setOffsetNid(136);
		deviceChannel12211.setLines(1);
		deviceChannel12211.setInSameLine(true);
		deviceChoice2111218.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice2111218.setOffsetNid(138);
		deviceChoice2111218.setLabelString("");
		deviceChoice211112213.setOffsetNid(139);
		deviceChoice211112213.setLabelString("");
		deviceChoice211112213.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice11132110.setOffsetNid(143);
		deviceChoice11132110.setLabelString("");
		deviceChoice11132110.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice11132111.setOffsetNid(171);
		deviceChoice11132111.setLabelString("");
		deviceChoice11132111.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChoice211112214.setOffsetNid(167);
		deviceChoice211112214.setLabelString("");
		deviceChoice211112214.setChoiceItems(new String[]
		{ "0.5", "5.0" });
		deviceChannel12212.setInSameLine(true);
		deviceChannel12212.setLines(1);
		deviceChannel12212.setLabelString("B05_T");
		deviceChannel12212.setOffsetNid(170);
		jPanel121127.setLayout(gridBagLayout2113);
		jPanel121127.setBorder(BorderFactory.createLoweredBevelBorder());
		jPanel121127.setMinimumSize(new Dimension(400, 88));
		jPanel121127.setPreferredSize(new Dimension(1001, 120));
		deviceChoice11132112.setOffsetNid(172);
		deviceChoice11132112.setLabelString("");
		deviceChoice11132112.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice1111219.setOffsetNid(165);
		deviceChoice1111219.setLabelString("      ");
		deviceChoice1111219.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChannel12213.setLabelString("B05_S");
		deviceChannel12213.setOffsetNid(166);
		deviceChannel12213.setLines(1);
		deviceChannel12213.setInSameLine(true);
		deviceChoice2111219.setChoiceItems(new String[]
		{ "CURRENT", "VOLTAGE", "OPEN LOOP" });
		deviceChoice2111219.setOffsetNid(168);
		deviceChoice2111219.setLabelString("");
		deviceChoice211112215.setOffsetNid(169);
		deviceChoice211112215.setLabelString("");
		deviceChoice211112215.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice11132113.setOffsetNid(173);
		deviceChoice11132113.setLabelString("");
		deviceChoice11132113.setChoiceItems(new String[]
		{ "ACTIVE", "INACTIVE" });
		deviceChoice1111220.setIdentifier(" ");
		deviceChoice1111220.setUpdateIdentifier("");
		deviceChoice1111220.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice1111220.setLabelString("      ");
		deviceChoice1111220.setOffsetNid(5);
		deviceChoice1111220.setChoiceFloatValues(null);
		deviceChoice1111220.setChoiceIntValues(null);
		deviceChoice11112110.setIdentifier("      ");
		deviceChoice11112110.setUpdateIdentifier("");
		deviceChoice11112110.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice11112110.setLabelString("      ");
		deviceChoice11112110.setOffsetNid(15);
		deviceChoice11112110.setChoiceFloatValues(null);
		deviceChoice11112110.setChoiceIntValues(null);
		deviceChoice1111221.setIdentifier("");
		deviceChoice1111221.setUpdateIdentifier("");
		deviceChoice1111221.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice1111221.setLabelString("      ");
		deviceChoice1111221.setOffsetNid(25);
		deviceChoice1111221.setChoiceFloatValues(null);
		deviceChoice1111221.setChoiceIntValues(null);
		deviceChoice1111222.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice1111222.setLabelString("      ");
		deviceChoice1111222.setOffsetNid(35);
		deviceChoice1111223.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice1111223.setLabelString("      ");
		deviceChoice1111223.setOffsetNid(45);
		deviceChoice1111224.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice1111224.setLabelString("      ");
		deviceChoice1111224.setOffsetNid(55);
		deviceChoice1111225.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice1111225.setLabelString("      ");
		deviceChoice1111225.setOffsetNid(75);
		deviceChoice1111226.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice1111226.setLabelString("      ");
		deviceChoice1111226.setOffsetNid(85);
		deviceChoice1111227.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice1111227.setLabelString("      ");
		deviceChoice1111227.setOffsetNid(95);
		deviceChoice1111228.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice1111228.setLabelString("      ");
		deviceChoice1111228.setOffsetNid(105);
		deviceChoice1111229.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice1111229.setLabelString("      ");
		deviceChoice1111229.setOffsetNid(115);
		deviceChoice11112111.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice11112111.setLabelString("      ");
		deviceChoice11112111.setOffsetNid(65);
		deviceChoice11112112.setIdentifier("");
		deviceChoice11112112.setUpdateIdentifier("");
		deviceChoice11112112.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice11112112.setLabelString("      ");
		deviceChoice11112112.setOffsetNid(125);
		deviceChoice11112112.setChoiceFloatValues(null);
		deviceChoice11112112.setChoiceIntValues(null);
		deviceChoice11112113.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice11112113.setLabelString("      ");
		deviceChoice11112113.setOffsetNid(135);
		deviceChoice11112114.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice11112114.setLabelString("      ");
		deviceChoice11112114.setOffsetNid(145);
		deviceChoice11112115.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice11112115.setLabelString("      ");
		deviceChoice11112115.setOffsetNid(155);
		deviceChoice11112116.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice11112116.setLabelString("      ");
		deviceChoice11112116.setOffsetNid(165);
		deviceChoice11112117.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice11112117.setLabelString("      ");
		deviceChoice11112117.setOffsetNid(175);
		deviceChoice11112118.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice11112118.setLabelString("      ");
		deviceChoice11112118.setOffsetNid(185);
		deviceChoice11112119.setChoiceItems(new String[]
		{ "SERIES", "INDIPENDENT", "PARALLEL" });
		deviceChoice11112119.setLabelString("      ");
		deviceChoice11112119.setOffsetNid(195);
		jPanel1.add(jPanel2, BorderLayout.NORTH);
		jPanel2.add(deviceField1, null);
		jPanel1.add(jTabbedPane1, BorderLayout.CENTER);
		jTabbedPane1.add(UNITS_A, "UNITS A");
		UNITS_A.add(jPanel4, null);
		jPanel4.add(jPanel7, BorderLayout.NORTH);
		jPanel7.add(jLabel10, null);
		jPanel7.add(jLabel8, null);
		jPanel7.add(jLabel4, null);
		jPanel7.add(jLabel3, null);
		jPanel7.add(jLabel6, null);
		jPanel7.add(jLabel2, null);
		jPanel7.add(jLabel1, null);
		jPanel7.add(jLabel7, null);
		jPanel7.add(jLabel9, null);
		jPanel7.add(jLabel5, null);
		jPanel4.add(jScrollPane3, BorderLayout.CENTER);
		jScrollPane3.getViewport().add(jPanel8, null);
		jTabbedPane1.add(UNITS_B, "UNITS_B");
		UNITS_B.add(jPanel5, null);
		jPanel9.add(jLabel16, null);
		jPanel9.add(jLabel12, null);
		jPanel9.add(jLabel19, null);
		jPanel9.add(jLabel20, null);
		jPanel9.add(jLabel17, null);
		jPanel9.add(jLabel14, null);
		jPanel9.add(jLabel13, null);
		jPanel9.add(jLabel15, null);
		jPanel9.add(jLabel11, null);
		jPanel9.add(jLabel18, null);
		jPanel1.add(deviceButtons1, BorderLayout.SOUTH);
		jPanel5.add(jScrollPane4, BorderLayout.CENTER);
		// jScrollPane4.add(jPanel10, null);
		jScrollPane4.getViewport().add(jPanel10, null);
		jPanel10.add(jPanel121120, null);
		jPanel111221.add(deviceChoice1111212, null);
		jPanel111221.add(deviceChoice21111220, null);
		jPanel111221.add(deviceChoice2111212, null);
		jPanel111221.add(deviceChoice21111221, null);
		jPanel121120.add(deviceChannel1220, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1220.add(jPanel111220, BorderLayout.EAST);
		jPanel121120.add(deviceChannel125, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel125.add(jPanel111221, BorderLayout.EAST);
		jPanel111220.add(deviceChoice11112112, null);
		jPanel111220.add(deviceChoice111320, null);
		jPanel111220.add(deviceChoice111321, null);
		jPanel111220.add(deviceChoice111322, null);
		jPanel10.add(jPanel121127, null);
		jPanel1112215.add(deviceChoice1111219, null);
		jPanel1112215.add(deviceChoice211112214, null);
		jPanel1112215.add(deviceChoice2111219, null);
		jPanel1112215.add(deviceChoice211112215, null);
		jPanel121127.add(deviceChannel12212, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel12212.add(jPanel1112214, BorderLayout.EAST);
		jPanel121127.add(deviceChannel12213, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel12213.add(jPanel1112215, BorderLayout.EAST);
		jPanel1112214.add(deviceChoice11112116, null);
		jPanel1112214.add(deviceChoice11132111, null);
		jPanel1112214.add(deviceChoice11132112, null);
		jPanel1112214.add(deviceChoice11132113, null);
		jPanel10.add(jPanel121126, null);
		jPanel1112213.add(deviceChoice1111218, null);
		jPanel1112213.add(deviceChoice211112212, null);
		jPanel1112213.add(deviceChoice2111218, null);
		jPanel1112213.add(deviceChoice211112213, null);
		jPanel121126.add(deviceChannel12210, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel12210.add(jPanel1112212, BorderLayout.EAST);
		jPanel121126.add(deviceChannel12211, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel12211.add(jPanel1112213, BorderLayout.EAST);
		jPanel1112212.add(deviceChoice11112113, null);
		jPanel1112212.add(deviceChoice1113218, null);
		jPanel1112212.add(deviceChoice1113219, null);
		jPanel1112212.add(deviceChoice11132110, null);
		jPanel10.add(jPanel121125, null);
		jPanel1112211.add(deviceChoice1111217, null);
		jPanel1112211.add(deviceChoice211112210, null);
		jPanel1112211.add(deviceChoice2111217, null);
		jPanel1112211.add(deviceChoice211112211, null);
		jPanel121125.add(deviceChannel1228, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1228.add(jPanel1112210, BorderLayout.EAST);
		jPanel121125.add(deviceChannel1229, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1229.add(jPanel1112211, BorderLayout.EAST);
		jPanel1112210.add(deviceChoice11112117, null);
		jPanel1112210.add(deviceChoice1113215, null);
		jPanel1112210.add(deviceChoice1113216, null);
		jPanel1112210.add(deviceChoice1113217, null);
		jPanel10.add(jPanel121124, null);
		jPanel111229.add(deviceChoice1111216, null);
		jPanel111229.add(deviceChoice21111228, null);
		jPanel111229.add(deviceChoice2111216, null);
		jPanel111229.add(deviceChoice21111229, null);
		jPanel121124.add(deviceChannel1226, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1226.add(jPanel111228, BorderLayout.EAST);
		jPanel121124.add(deviceChannel1227, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1227.add(jPanel111229, BorderLayout.EAST);
		jPanel111228.add(deviceChoice11112114, null);
		jPanel111228.add(deviceChoice1113212, null);
		jPanel111228.add(deviceChoice1113213, null);
		jPanel111228.add(deviceChoice1113214, null);
		jPanel10.add(jPanel121123, null);
		jPanel111227.add(deviceChoice1111215, null);
		jPanel111227.add(deviceChoice21111226, null);
		jPanel111227.add(deviceChoice2111215, null);
		jPanel111227.add(deviceChoice21111227, null);
		jPanel121123.add(deviceChannel1224, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1224.add(jPanel111226, BorderLayout.EAST);
		jPanel121123.add(deviceChannel1225, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1225.add(jPanel111227, BorderLayout.EAST);
		jPanel111226.add(deviceChoice11112118, null);
		jPanel111226.add(deviceChoice111329, null);
		jPanel111226.add(deviceChoice1113210, null);
		jPanel111226.add(deviceChoice1113211, null);
		jPanel10.add(jPanel121122, null);
		jPanel111225.add(deviceChoice1111214, null);
		jPanel111225.add(deviceChoice21111224, null);
		jPanel111225.add(deviceChoice2111214, null);
		jPanel111225.add(deviceChoice21111225, null);
		jPanel121122.add(deviceChannel1222, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1222.add(jPanel111224, BorderLayout.EAST);
		jPanel121122.add(deviceChannel1223, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1223.add(jPanel111225, BorderLayout.EAST);
		jPanel111224.add(deviceChoice11112115, null);
		jPanel111224.add(deviceChoice111326, null);
		jPanel111224.add(deviceChoice111327, null);
		jPanel111224.add(deviceChoice111328, null);
		jPanel10.add(jPanel121121, null);
		jPanel111223.add(deviceChoice1111213, null);
		jPanel111223.add(deviceChoice21111222, null);
		jPanel111223.add(deviceChoice2111213, null);
		jPanel111223.add(deviceChoice21111223, null);
		jPanel121121.add(deviceChannel1221, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1221.add(jPanel111222, BorderLayout.EAST);
		jPanel121121.add(deviceChannel126, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel126.add(jPanel111223, BorderLayout.EAST);
		jPanel111222.add(deviceChoice11112119, null);
		jPanel111222.add(deviceChoice111323, null);
		jPanel111222.add(deviceChoice111324, null);
		jPanel111222.add(deviceChoice111325, null);
		jPanel5.add(jPanel9, BorderLayout.NORTH);
		jPanel11126.add(deviceChoice1111220, null);
		jPanel11126.add(deviceChoice2111120, null);
		jPanel11126.add(deviceChoice211120, null);
		jPanel11126.add(deviceChoice2111121, null);
		jPanel12118.add(deviceChannel127, new GridBagConstraints(0, 1, 2, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel127.add(jPanel11127, BorderLayout.EAST);
		jPanel12118.add(deviceChannel124, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 0), -24, -10));
		deviceChannel124.add(jPanel11126, BorderLayout.EAST);
		jPanel11127.add(deviceChoice111120, null);
		jPanel11127.add(deviceChoice11130, null);
		jPanel11127.add(deviceChoice11131, null);
		jPanel11127.add(deviceChoice11132, null);
		jPanel8.add(jPanel12118, null);
		jPanel8.add(jPanel121119, null);
		jPanel1112119.add(deviceChoice1111211, null);
		jPanel1112119.add(deviceChoice211112112, null);
		jPanel1112119.add(deviceChoice2111211, null);
		jPanel1112119.add(deviceChoice211112113, null);
		jPanel121119.add(deviceChannel12118, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel12118.add(jPanel1112118, BorderLayout.EAST);
		jPanel121119.add(deviceChannel12119, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel12119.add(jPanel1112119, BorderLayout.EAST);
		jPanel1112118.add(deviceChoice11112111, null);
		jPanel1112118.add(deviceChoice11131113, null);
		jPanel1112118.add(deviceChoice11131114, null);
		jPanel1112118.add(deviceChoice11131115, null);
		jPanel8.add(jPanel121118, null);
		jPanel1112117.add(deviceChoice1111210, null);
		jPanel1112117.add(deviceChoice211112110, null);
		jPanel1112117.add(deviceChoice2111210, null);
		jPanel1112117.add(deviceChoice211112111, null);
		jPanel121118.add(deviceChannel12116, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel12116.add(jPanel1112116, BorderLayout.EAST);
		jPanel121118.add(deviceChannel12117, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel12117.add(jPanel1112117, BorderLayout.EAST);
		jPanel1112116.add(deviceChoice11112110, null);
		jPanel1112116.add(deviceChoice11131110, null);
		jPanel1112116.add(deviceChoice11131111, null);
		jPanel1112116.add(deviceChoice11131112, null);
		jPanel8.add(jPanel121117, null);
		jPanel1112115.add(deviceChoice111129, null);
		jPanel1112115.add(deviceChoice21111218, null);
		jPanel1112115.add(deviceChoice211129, null);
		jPanel1112115.add(deviceChoice21111219, null);
		jPanel121117.add(deviceChannel12114, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel12114.add(jPanel1112114, BorderLayout.EAST);
		jPanel121117.add(deviceChannel12115, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel12115.add(jPanel1112115, BorderLayout.EAST);
		jPanel1112114.add(deviceChoice1111225, null);
		jPanel1112114.add(deviceChoice1113117, null);
		jPanel1112114.add(deviceChoice1113118, null);
		jPanel1112114.add(deviceChoice1113119, null);
		jPanel8.add(jPanel121116, null);
		jPanel1112113.add(deviceChoice111128, null);
		jPanel1112113.add(deviceChoice21111216, null);
		jPanel1112113.add(deviceChoice211128, null);
		jPanel1112113.add(deviceChoice21111217, null);
		jPanel121116.add(deviceChannel12112, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel12112.add(jPanel1112112, BorderLayout.EAST);
		jPanel121116.add(deviceChannel12113, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel12113.add(jPanel1112113, BorderLayout.EAST);
		jPanel1112112.add(deviceChoice1111221, null);
		jPanel1112112.add(deviceChoice1113114, null);
		jPanel1112112.add(deviceChoice1113115, null);
		jPanel1112112.add(deviceChoice1113116, null);
		jPanel8.add(jPanel121115, null);
		jPanel1112111.add(deviceChoice111127, null);
		jPanel1112111.add(deviceChoice21111214, null);
		jPanel1112111.add(deviceChoice211127, null);
		jPanel1112111.add(deviceChoice21111215, null);
		jPanel121115.add(deviceChannel12110, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel12110.add(jPanel1112110, BorderLayout.EAST);
		jPanel121115.add(deviceChannel12111, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -22, -10));
		deviceChannel12111.add(jPanel1112111, BorderLayout.EAST);
		jPanel1112110.add(deviceChoice1111226, null);
		jPanel1112110.add(deviceChoice1113111, null);
		jPanel1112110.add(deviceChoice1113112, null);
		jPanel1112110.add(deviceChoice1113113, null);
		jPanel8.add(jPanel121114, null);
		jPanel111219.add(deviceChoice111126, null);
		jPanel111219.add(deviceChoice21111212, null);
		jPanel111219.add(deviceChoice211126, null);
		jPanel111219.add(deviceChoice21111213, null);
		jPanel121114.add(deviceChannel1218, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1218.add(jPanel111218, BorderLayout.EAST);
		jPanel121114.add(deviceChannel1219, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1219.add(jPanel111219, BorderLayout.EAST);
		jPanel111218.add(deviceChoice1111222, null);
		jPanel111218.add(deviceChoice111318, null);
		jPanel111218.add(deviceChoice111319, null);
		jPanel111218.add(deviceChoice1113110, null);
		jPanel8.add(jPanel121113, null);
		jPanel111217.add(deviceChoice111125, null);
		jPanel111217.add(deviceChoice21111210, null);
		jPanel111217.add(deviceChoice211125, null);
		jPanel111217.add(deviceChoice21111211, null);
		jPanel121113.add(deviceChannel1216, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1216.add(jPanel111216, BorderLayout.EAST);
		jPanel121113.add(deviceChannel1217, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1217.add(jPanel111217, BorderLayout.EAST);
		jPanel111216.add(deviceChoice1111227, null);
		jPanel111216.add(deviceChoice111315, null);
		jPanel111216.add(deviceChoice111316, null);
		jPanel111216.add(deviceChoice111317, null);
		jPanel8.add(jPanel121112, null);
		jPanel111215.add(deviceChoice111124, null);
		jPanel111215.add(deviceChoice2111128, null);
		jPanel111215.add(deviceChoice211124, null);
		jPanel111215.add(deviceChoice2111129, null);
		jPanel121112.add(deviceChannel1214, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1214.add(jPanel111214, BorderLayout.EAST);
		jPanel121112.add(deviceChannel1215, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1215.add(jPanel111215, BorderLayout.EAST);
		jPanel111214.add(deviceChoice1111223, null);
		jPanel111214.add(deviceChoice111312, null);
		jPanel111214.add(deviceChoice111313, null);
		jPanel111214.add(deviceChoice111314, null);
		jPanel8.add(jPanel121111, null);
		jPanel111213.add(deviceChoice111123, null);
		jPanel111213.add(deviceChoice2111126, null);
		jPanel111213.add(deviceChoice211123, null);
		jPanel111213.add(deviceChoice2111127, null);
		jPanel121111.add(deviceChannel1212, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1212.add(jPanel111212, BorderLayout.EAST);
		jPanel121111.add(deviceChannel1213, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1213.add(jPanel111213, BorderLayout.EAST);
		jPanel111212.add(deviceChoice1111228, null);
		jPanel111212.add(deviceChoice11139, null);
		jPanel111212.add(deviceChoice111310, null);
		jPanel111212.add(deviceChoice111311, null);
		jPanel8.add(jPanel121110, null);
		jPanel111211.add(deviceChoice111122, null);
		jPanel111211.add(deviceChoice2111124, null);
		jPanel111211.add(deviceChoice211122, null);
		jPanel111211.add(deviceChoice2111125, null);
		jPanel121110.add(deviceChannel1210, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1210.add(jPanel111210, BorderLayout.EAST);
		jPanel121110.add(deviceChannel1211, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel1211.add(jPanel111211, BorderLayout.EAST);
		jPanel111210.add(deviceChoice1111224, null);
		jPanel111210.add(deviceChoice11136, null);
		jPanel111210.add(deviceChoice11137, null);
		jPanel111210.add(deviceChoice11138, null);
		jPanel8.add(jPanel12119, null);
		jPanel11129.add(deviceChoice111121, null);
		jPanel11129.add(deviceChoice2111122, null);
		jPanel11129.add(deviceChoice211121, null);
		jPanel11129.add(deviceChoice2111123, null);
		jPanel12119.add(deviceChannel128, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel128.add(jPanel11128, BorderLayout.EAST);
		jPanel12119.add(deviceChannel129, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.NORTH,
				GridBagConstraints.NONE, new Insets(2, 100, 6, 110), -24, -10));
		deviceChannel129.add(jPanel11129, BorderLayout.EAST);
		jPanel11128.add(deviceChoice1111229, null);
		jPanel11128.add(deviceChoice11133, null);
		jPanel11128.add(deviceChoice11134, null);
		jPanel11128.add(deviceChoice11135, null);
		this.getContentPane().add(jPanel1, BorderLayout.CENTER);
	}
}

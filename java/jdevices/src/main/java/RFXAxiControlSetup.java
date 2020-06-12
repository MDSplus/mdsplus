import java.awt.*;
import javax.swing.*;

/**
 * <p>
 * Title:
 * </p>
 * <p>
 * Description:
 * </p>
 * <p>
 * Copyright: Copyright (c) 2003
 * </p>
 * <p>
 * Company:
 * </p>
 *
 * @author not attributable
 * @version 1.0
 */
public class RFXAxiControlSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JTabbedPane jTabbedPane1 = new JTabbedPane();
	DeviceWave deviceWave1 = new DeviceWave();
	DeviceWave deviceWave2 = new DeviceWave();
	DeviceWave deviceWave3 = new DeviceWave();
	DeviceWave deviceWave4 = new DeviceWave();
	DeviceWave deviceWave5 = new DeviceWave();
	DeviceWave deviceWave6 = new DeviceWave();
	DeviceWave deviceWave7 = new DeviceWave();
	DeviceWave deviceWave8 = new DeviceWave();
	DeviceWave deviceWave9 = new DeviceWave();
	DeviceWave deviceWave10 = new DeviceWave();
	DeviceWave deviceWave11 = new DeviceWave();
	JPanel jPanel1 = new JPanel();
	JPanel jPanel2 = new JPanel();
	DeviceField deviceField1 = new DeviceField();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel3 = new JPanel();
	DeviceField deviceField2 = new DeviceField();
	DeviceWave deviceWave12 = new DeviceWave();
	DeviceWave deviceWave13 = new DeviceWave();
	DeviceWave deviceWave14 = new DeviceWave();
	DeviceWave deviceWave15 = new DeviceWave();
	DeviceWave deviceWave16 = new DeviceWave();
	DeviceWave deviceWave17 = new DeviceWave();
	DeviceWave deviceWave18 = new DeviceWave();
	DeviceWave deviceWave19 = new DeviceWave();
	DeviceWave deviceWave20 = new DeviceWave();
	DeviceWave deviceWave21 = new DeviceWave();
	DeviceWave deviceWave22 = new DeviceWave();
	DeviceWave deviceWave23 = new DeviceWave();
	DeviceWave deviceWave24 = new DeviceWave();
	DeviceWave deviceWave25 = new DeviceWave();
	DeviceWave deviceWave26 = new DeviceWave();
	DeviceWave deviceWave27 = new DeviceWave();
	DeviceWave deviceWave28 = new DeviceWave();
	DeviceWave deviceWave29 = new DeviceWave();
	DeviceWave deviceWave30 = new DeviceWave();
	DeviceWave deviceWave31 = new DeviceWave();
	DeviceField deviceField3 = new DeviceField();

	public RFXAxiControlSetup()
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
		this.setWidth(510);
		this.setHeight(510);
		this.setDeviceType("RFXAxiControl");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("Axisymmetric Control Setup");
		this.getContentPane().setLayout(borderLayout1);
		deviceWave1.setOffsetNid(3);
		deviceWave1.setMinXVisible(false);
		deviceWave1.setMinYVisible(true);
		deviceWave1.setIdentifier("");
		deviceWave1.setUpdateExpression("");
		deviceWave2.setOffsetNid(9);
		deviceWave2.setMinXVisible(false);
		deviceWave2.setMinYVisible(true);
		deviceWave2.setIdentifier("");
		deviceWave2.setUpdateExpression("");
		deviceWave3.setOffsetNid(15);
		deviceWave3.setMinXVisible(false);
		deviceWave3.setMinYVisible(true);
		deviceWave3.setMaxYVisible(true);
		deviceWave3.setIdentifier("");
		deviceWave3.setUpdateExpression("");
		deviceWave4.setOffsetNid(21);
		deviceWave4.setMinXVisible(false);
		deviceWave4.setMinYVisible(true);
		deviceWave4.setIdentifier("");
		deviceWave4.setUpdateExpression("");
		deviceWave5.setOffsetNid(27);
		deviceWave5.setMinXVisible(false);
		deviceWave5.setMinYVisible(true);
		deviceWave5.setIdentifier("");
		deviceWave5.setUpdateExpression("");
		deviceWave6.setOffsetNid(33);
		deviceWave6.setMinXVisible(false);
		deviceWave6.setMinYVisible(true);
		deviceWave6.setIdentifier("");
		deviceWave6.setUpdateExpression("");
		deviceWave7.setOffsetNid(39);
		deviceWave7.setMinXVisible(false);
		deviceWave7.setMinYVisible(true);
		deviceWave7.setIdentifier("");
		deviceWave7.setUpdateExpression("");
		deviceWave8.setOffsetNid(45);
		deviceWave8.setMinXVisible(false);
		deviceWave8.setMinYVisible(true);
		deviceWave8.setIdentifier("");
		deviceWave8.setUpdateExpression("");
		deviceWave9.setOffsetNid(51);
		deviceWave9.setMinYVisible(true);
		deviceWave9.setIdentifier("");
		deviceWave9.setUpdateExpression("");
		deviceWave10.setOffsetNid(57);
		deviceWave10.setMinYVisible(true);
		deviceWave10.setIdentifier("");
		deviceWave10.setUpdateExpression("");
		deviceWave11.setOffsetNid(63);
		deviceWave11.setMinYVisible(true);
		deviceWave11.setIdentifier("");
		deviceWave11.setUpdateExpression("");
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Comment: ");
		deviceField1.setNumCols(30);
		deviceField1.setIdentifier("");
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(2);
//        deviceField2.setOffsetNid(116+12);
		deviceField2.setOffsetNid(111 + 77);
		deviceField2.setLabelString("Bv_Gain");
		deviceField2.setIdentifier("");
		deviceWave12.setUpdateExpression("");
		deviceWave12.setIdentifier("");
		deviceWave12.setMinYVisible(true);
		deviceWave12.setOffsetNid(69);
		deviceWave13.setUpdateExpression("");
		deviceWave13.setIdentifier("");
		deviceWave13.setMinYVisible(true);
		deviceWave13.setOffsetNid(75);
		deviceWave14.setUpdateExpression("");
		deviceWave14.setIdentifier("");
		deviceWave14.setMinYVisible(true);
		deviceWave14.setOffsetNid(81);
		deviceWave15.setUpdateExpression("");
		deviceWave15.setIdentifier("");
		deviceWave15.setMinYVisible(true);
		deviceWave15.setOffsetNid(87);
		deviceWave16.setUpdateExpression("");
		deviceWave16.setIdentifier("");
		deviceWave16.setMinYVisible(true);
		deviceWave16.setOffsetNid(93);
		deviceWave17.setUpdateExpression("");
		deviceWave17.setIdentifier("");
		deviceWave17.setMinYVisible(true);
		deviceWave17.setOffsetNid(99);
		deviceWave18.setUpdateExpression("");
		deviceWave18.setIdentifier("");
		deviceWave18.setMinYVisible(true);
		deviceWave18.setOffsetNid(105);
		deviceWave19.setUpdateExpression("");
		deviceWave19.setIdentifier("");
		deviceWave19.setMinYVisible(true);
		deviceWave19.setOffsetNid(111);
		deviceWave20.setUpdateExpression("");
		deviceWave20.setIdentifier("");
		deviceWave20.setMinYVisible(true);
		deviceWave20.setOffsetNid(111 + 6);
		deviceWave21.setUpdateExpression("");
		deviceWave21.setIdentifier("");
		deviceWave21.setMinYVisible(true);
		deviceWave21.setOffsetNid(111 + 12);
		deviceWave22.setUpdateExpression("");
		deviceWave22.setIdentifier("");
		deviceWave22.setMinYVisible(true);
		deviceWave22.setOffsetNid(111 + 18);
		deviceWave23.setUpdateExpression("");
		deviceWave23.setIdentifier("");
		deviceWave23.setMinYVisible(true);
		deviceWave23.setOffsetNid(111 + 24);
		deviceWave24.setUpdateExpression("");
		deviceWave24.setIdentifier("");
		deviceWave24.setMinYVisible(true);
		deviceWave24.setOffsetNid(111 + 30);
		deviceWave25.setUpdateExpression("");
		deviceWave25.setIdentifier("");
		deviceWave25.setMinYVisible(true);
		deviceWave25.setOffsetNid(111 + 36);
		deviceWave26.setUpdateExpression("");
		deviceWave26.setIdentifier("");
		deviceWave26.setMinYVisible(true);
		deviceWave26.setOffsetNid(111 + 42);
		deviceWave27.setUpdateExpression("");
		deviceWave27.setIdentifier("");
		deviceWave27.setMinYVisible(true);
		deviceWave27.setOffsetNid(111 + 48);
		deviceWave28.setUpdateExpression("");
		deviceWave28.setIdentifier("");
		deviceWave28.setMinYVisible(true);
		deviceWave28.setOffsetNid(111 + 54);
		deviceWave29.setUpdateExpression("");
		deviceWave29.setIdentifier("");
		deviceWave29.setMinYVisible(true);
		deviceWave29.setOffsetNid(111 + 60);
		deviceWave30.setUpdateExpression("");
		deviceWave30.setIdentifier("");
		deviceWave30.setMinYVisible(true);
		deviceWave30.setOffsetNid(111 + 66);
		deviceWave31.setUpdateExpression("");
		deviceWave31.setIdentifier("");
		deviceWave31.setMinYVisible(true);
		deviceWave31.setOffsetNid(111 + 72);
		// deviceField3.setOffsetNid(117+12+12);
		// deviceField3.setOffsetNid(1+138+2);
		deviceField3.setOffsetNid(111 + 78);
		deviceField3.setLabelString("T Start Int. Action");
		deviceField3.setIdentifier("");
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jTabbedPane1, BorderLayout.CENTER);
		jTabbedPane1.add(deviceWave1, "BV_add");
		jTabbedPane1.add(deviceWave2, "Delta_Ip");
		jTabbedPane1.add(deviceWave3, "Ref_delta_H");
		jTabbedPane1.add(deviceWave4, "I_FS1_add");
		jTabbedPane1.add(deviceWave5, "I_FS2_add");
		jTabbedPane1.add(deviceWave6, "I_FS3_add");
		jTabbedPane1.add(deviceWave7, "I_FS4_add");
		jTabbedPane1.add(deviceWave8, "I_FS5_add");
		jTabbedPane1.add(deviceWave9, "I_FS6_add");
		jTabbedPane1.add(deviceWave10, "I_FS7_add");
		jTabbedPane1.add(deviceWave11, "I_FS8_add");
		jTabbedPane1.add(deviceWave12, "delta_I_FS1");
		jTabbedPane1.add(deviceWave13, "delta_I_FS2");
		jTabbedPane1.add(deviceWave14, "delta_I_FS3");
		jTabbedPane1.add(deviceWave15, "delta_I_FS4");
		jTabbedPane1.add(deviceWave16, "delta_I_FS5");
		jTabbedPane1.add(deviceWave17, "delta_I_FS6");
		jTabbedPane1.add(deviceWave18, "delta_I_FS7");
		jTabbedPane1.add(deviceWave19, "delta_I_FS8");
		jTabbedPane1.add(deviceWave20, "RplaCos2Ref");
		jTabbedPane1.add(deviceWave21, "RplaCos3Ref");
		jTabbedPane1.add(deviceWave22, "avgRplasmaRef");
		jTabbedPane1.add(deviceWave23, "REF_DELTA_V");
		jTabbedPane1.add(deviceWave23, "REF_DELTA_V");
		jTabbedPane1.add(deviceWave23, "REF_DELTA_V");
		jTabbedPane1.add(deviceWave24, "gapRef_1");
		jTabbedPane1.add(deviceWave25, "gapRef_2");
		jTabbedPane1.add(deviceWave26, "gapRef_3");
		jTabbedPane1.add(deviceWave27, "gapRef_4");
		jTabbedPane1.add(deviceWave28, "gapRef_5");
		jTabbedPane1.add(deviceWave29, "gapRef_6");
		jTabbedPane1.add(deviceWave30, "gapRef_7");
		jTabbedPane1.add(deviceWave31, "gapRef_8");
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceField1, null);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceField2, null);
		jPanel3.add(deviceField3, null);
	}
}

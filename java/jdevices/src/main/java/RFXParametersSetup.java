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
public class RFXParametersSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel1 = new JPanel();
	JPanel jPanel2 = new JPanel();
	DeviceField deviceField1 = new DeviceField();
	GridLayout gridLayout1 = new GridLayout();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();
	DeviceField deviceField9 = new DeviceField();
	DeviceField deviceField10 = new DeviceField();
	DeviceField deviceField11 = new DeviceField();
	DeviceField deviceField12 = new DeviceField();
	DeviceField deviceField13 = new DeviceField();
	DeviceField deviceField14 = new DeviceField();
	DeviceField deviceField15 = new DeviceField();
	DeviceField deviceField16 = new DeviceField();
	DeviceField deviceField17 = new DeviceField();
	DeviceField deviceField18 = new DeviceField();
	DeviceField deviceField19 = new DeviceField();

	public RFXParametersSetup()
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
		this.setWidth(608);
		this.setHeight(608);
		this.setDeviceType("RFXParameters");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("RFX Parameters");
		this.getContentPane().setLayout(borderLayout1);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Comment: ");
		deviceField1.setNumCols(35);
		deviceField1.setIdentifier("");
		jPanel2.setLayout(gridLayout1);
		gridLayout1.setColumns(3);
		gridLayout1.setRows(6);
		deviceField2.setIdentifier("");
		deviceField2.setNumCols(6);
		deviceField2.setLabelString("COILS_E_RAD: ");
		deviceField2.setTextOnly(false);
		deviceField2.setOffsetNid(2);
		deviceField3.setOffsetNid(19);
		deviceField3.setTextOnly(false);
		deviceField3.setLabelString("WALL_RAD: ");
		deviceField3.setNumCols(6);
		deviceField3.setIdentifier("");
		deviceField4.setOffsetNid(18);
		deviceField4.setTextOnly(false);
		deviceField4.setLabelString("TORVLOOP_RAD: ");
		deviceField4.setNumCols(6);
		deviceField4.setIdentifier("");
		deviceField5.setOffsetNid(17);
		deviceField5.setTextOnly(false);
		deviceField5.setLabelString("SHELL_RES: ");
		deviceField5.setNumCols(6);
		deviceField5.setIdentifier("");
		deviceField6.setOffsetNid(16);
		deviceField6.setTextOnly(false);
		deviceField6.setLabelString("SHELL_I_RAD: ");
		deviceField6.setNumCols(6);
		deviceField6.setIdentifier("");
		deviceField7.setOffsetNid(15);
		deviceField7.setTextOnly(false);
		deviceField7.setLabelString("SHELL_E_RAD: ");
		deviceField7.setNumCols(6);
		deviceField7.setIdentifier("");
		deviceField8.setOffsetNid(14);
		deviceField8.setTextOnly(false);
		deviceField8.setLabelString("R_T_LINEAR: ");
		deviceField8.setNumCols(6);
		deviceField8.setIdentifier("");
		deviceField9.setOffsetNid(13);
		deviceField9.setTextOnly(false);
		deviceField9.setLabelString("R_P_LINEAR: ");
		deviceField9.setNumCols(6);
		deviceField9.setIdentifier("");
		deviceField10.setOffsetNid(12);
		deviceField10.setTextOnly(false);
		deviceField10.setLabelString("POLVLOOP_RAD: ");
		deviceField10.setNumCols(6);
		deviceField10.setIdentifier("");
		deviceField11.setOffsetNid(11);
		deviceField11.setTextOnly(false);
		deviceField11.setLabelString("MAJOR_RAD: ");
		deviceField11.setNumCols(6);
		deviceField11.setIdentifier("");
		deviceField12.setOffsetNid(10);
		deviceField12.setTextOnly(false);
		deviceField12.setLabelString("L_T_LINER: ");
		deviceField12.setNumCols(6);
		deviceField12.setIdentifier("");
		deviceField13.setOffsetNid(9);
		deviceField13.setTextOnly(false);
		deviceField13.setLabelString("L_P_LINER: ");
		deviceField13.setNumCols(6);
		deviceField13.setIdentifier("");
		deviceField14.setOffsetNid(8);
		deviceField14.setTextOnly(false);
		deviceField14.setLabelString("LINER_I_RAD: ");
		deviceField14.setNumCols(6);
		deviceField14.setIdentifier("");
		deviceField15.setOffsetNid(7);
		deviceField15.setTextOnly(false);
		deviceField15.setLabelString("LINER_E_RAD: ");
		deviceField15.setNumCols(6);
		deviceField15.setIdentifier("");
		deviceField16.setOffsetNid(6);
		deviceField16.setTextOnly(false);
		deviceField16.setLabelString("GRPHTE_I_RAD: ");
		deviceField16.setNumCols(6);
		deviceField16.setIdentifier("");
		deviceField17.setOffsetNid(5);
		deviceField17.setTextOnly(false);
		deviceField17.setLabelString("GRPHTE_E_RAD:");
		deviceField17.setNumCols(6);
		deviceField17.setIdentifier("");
		deviceField18.setOffsetNid(4);
		deviceField18.setTextOnly(false);
		deviceField18.setLabelString("COILS_RES: ");
		deviceField18.setNumCols(6);
		deviceField18.setIdentifier("");
		deviceField19.setOffsetNid(3);
		deviceField19.setTextOnly(false);
		deviceField19.setLabelString("COILS_I_RAD: ");
		deviceField19.setNumCols(6);
		deviceField19.setIdentifier("");
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(deviceField1, null);
		this.getContentPane().add(jPanel2, BorderLayout.CENTER);
		jPanel2.add(deviceField2, null);
		jPanel2.add(deviceField19, null);
		jPanel2.add(deviceField18, null);
		jPanel2.add(deviceField17, null);
		jPanel2.add(deviceField16, null);
		jPanel2.add(deviceField15, null);
		jPanel2.add(deviceField14, null);
		jPanel2.add(deviceField13, null);
		jPanel2.add(deviceField12, null);
		jPanel2.add(deviceField11, null);
		jPanel2.add(deviceField10, null);
		jPanel2.add(deviceField9, null);
		jPanel2.add(deviceField8, null);
		jPanel2.add(deviceField7, null);
		jPanel2.add(deviceField6, null);
		jPanel2.add(deviceField5, null);
		jPanel2.add(deviceField4, null);
		jPanel2.add(deviceField3, null);
	}
}

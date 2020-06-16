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
public class LASER_NDSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	JPanel jPanel1 = new JPanel();
	BorderLayout borderLayout1 = new BorderLayout();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel2 = new JPanel();
	DeviceField deviceField1 = new DeviceField();
	FlowLayout flowLayout1 = new FlowLayout();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	JPanel jPanel3 = new JPanel();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	FlowLayout flowLayout2 = new FlowLayout();
	JPanel jPanel4 = new JPanel();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel5 = new JPanel();
	DeviceField deviceField2 = new DeviceField();
	FlowLayout flowLayout3 = new FlowLayout();

	public LASER_NDSetup()
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
		this.setWidth(500);
		this.setHeight(200);
		this.setDeviceType("LASER_ND");
		this.setDeviceProvider("150.178.3.33");
		this.setDeviceTitle("Laser Neodymium Yag");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Commet :");
		deviceField1.setNumCols(25);
		deviceField1.setIdentifier("");
		jPanel2.setLayout(flowLayout1);
		flowLayout1.setAlignment(FlowLayout.LEFT);
		gridLayout1.setRows(4);
		deviceField4.setOffsetNid(5);
		deviceField4.setLabelString("Num Pulses:");
		deviceField4.setIdentifier("");
		deviceField3.setOffsetNid(6);
		deviceField3.setLabelString("Delay Pulse [s] :");
		deviceField3.setIdentifier("");
		jPanel3.setLayout(flowLayout2);
		flowLayout2.setAlignment(FlowLayout.LEFT);
		deviceField2.setOffsetNid(4);
		deviceField2.setLabelString("Trig. source :");
		deviceField2.setNumCols(30);
		deviceField2.setIdentifier("");
		jPanel5.setLayout(flowLayout3);
		flowLayout3.setAlignment(FlowLayout.LEFT);
		deviceButtons1.setMethods(new String[]
		{ "init", "trigger", "dump", "reset", "check" });
		jPanel2.add(deviceField1, null);
		jPanel2.add(deviceDispatch1, null);
		jPanel1.add(jPanel2, null);
		jPanel1.add(jPanel5, null);
		jPanel5.add(deviceField2, null);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceField4, null);
		jPanel3.add(deviceField3, null);
		jPanel1.add(jPanel4, null);
		jPanel4.add(deviceButtons1, null);
		this.getContentPane().add(jPanel1, BorderLayout.CENTER);
	}
}

import javax.swing.*;

import java.awt.*;

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
public class FEGroupSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	JPanel jPanel1 = new JPanel();
	GridLayout gridLayout1 = new GridLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel2 = new JPanel();
	FlowLayout flowLayout1 = new FlowLayout();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	JPanel jPanel3 = new JPanel();
	DeviceField deviceField1 = new DeviceField();
	FlowLayout flowLayout2 = new FlowLayout();

	public FEGroupSetup()
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
		this.setWidth(380);
		this.setHeight(150);
		this.setDeviceType("FEGroup");
		this.setDeviceProvider("150.178.3.33");
		this.setDeviceTitle("ISIS Front End Gain");
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setRows(3);
		jPanel2.setLayout(flowLayout1);
		deviceChoice2.setChoiceIntValues(null);
		deviceChoice2.setChoiceFloatValues(new float[]
		{ (float) 1.0, (float) 2.0, (float) 5.0, (float) 10.0 });
		deviceChoice2.setOffsetNid(3);
		deviceChoice2.setLabelString("Gain:");
		deviceChoice2.setChoiceItems(new String[]
		{ "1", "2", "5", "10" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		deviceField1.setOffsetNid(1);
		deviceField1.setLabelString("Comment:");
		deviceField1.setNumCols(20);
		deviceField1.setIdentifier("");
		jPanel3.setLayout(flowLayout2);
		flowLayout2.setAlignment(FlowLayout.LEFT);
		flowLayout1.setAlignment(FlowLayout.LEFT);
		deviceButtons1.setMethods(new String[]
		{ "init" });
		this.getContentPane().add(jPanel1, BorderLayout.CENTER);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceField1, null);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceChoice2, null);
		jPanel2.add(deviceDispatch1, null);
		jPanel1.add(deviceButtons1, null);
	}
}

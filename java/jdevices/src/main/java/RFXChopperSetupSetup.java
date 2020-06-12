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
public class RFXChopperSetupSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceField deviceField1 = new DeviceField();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel1 = new JPanel();
	DeviceWave deviceWave1 = new DeviceWave();
	DeviceTable deviceTable1 = new DeviceTable();
	GridLayout gridLayout1 = new GridLayout();

	public RFXChopperSetupSetup()
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
		this.setWidth(542);
		this.setHeight(542);
		this.setDeviceType("RFXChopperSetup");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("EDA2 Chopper Setup");
		this.getContentPane().setLayout(borderLayout1);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Comment:");
		deviceField1.setNumCols(35);
		deviceField1.setIdentifier("");
		deviceWave1.setOffsetNid(3);
		deviceWave1.setIdentifier("Waveform: ");
		deviceWave1.setUpdateIdentifier("");
		deviceWave1.setUpdateExpression("");
		deviceTable1.setOffsetNid(8);
		deviceTable1.setLabelString("Unit calibration");
		deviceTable1.setNumCols(2);
		deviceTable1.setNumRows(12);
		deviceTable1.setIdentifier("");
		deviceTable1.setColumnNames(new String[]
		{ "Gain", "Offset" });
		deviceTable1.setRowNames(new String[]
		{ "Unit 1", "Unit 2", "Unit 3", "Unit 4", "Unit 5", "Unit 6", "Unit 7", "Unit 8", "Unit 9", "Unit 10",
				"Unit 11", "Unit 12" });
		deviceTable1.setDisplayRowNumber(true);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(2);
		this.getContentPane().add(deviceField1, BorderLayout.NORTH);
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.CENTER);
		jPanel1.add(deviceWave1, null);
		jPanel1.add(deviceTable1, null);
	}
}

import javax.swing.*;
import java.awt.*;
import javax.swing.border.*;

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
public class WEGroupSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	JPanel jPanel1 = new JPanel();
	FlowLayout flowLayout1 = new FlowLayout();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	DeviceField deviceField1 = new DeviceField();
	JPanel jPanel2 = new JPanel();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	TitledBorder titledBorder1;
	TitledBorder titledBorder2;
	JPanel jPanel3 = new JPanel();
	GridLayout gridLayout2 = new GridLayout();
	DeviceField deviceField111115 = new DeviceField();
	DeviceChoice deviceChoice111113 = new DeviceChoice();
	JPanel jPanel119 = new JPanel();
	DeviceChoice deviceChoice111115 = new DeviceChoice();
	DeviceField deviceField111116 = new DeviceField();
	FlowLayout flowLayout2 = new FlowLayout();
	DeviceField deviceField111114 = new DeviceField();
	DeviceChoice deviceChoice111114 = new DeviceChoice();
	JPanel jPanel4 = new JPanel();
	GridLayout gridLayout3 = new GridLayout();
	DeviceChoice deviceChoice11111111117 = new DeviceChoice();
	DeviceChoice deviceChoice11111111118 = new DeviceChoice();
	DeviceField deviceField1111111114 = new DeviceField();
	FlowLayout flowLayout11114 = new FlowLayout();
	DeviceChoice deviceChoice110 = new DeviceChoice();
	DeviceChoice deviceChoice11111111119 = new DeviceChoice();
	DeviceField deviceField1111111115 = new DeviceField();
	JPanel jPanel11214 = new JPanel();
	JLabel jLabel1 = new JLabel();
	JLabel jLabel2 = new JLabel();
	GridBagLayout gridBagLayout1 = new GridBagLayout();

	public WEGroupSetup()
	{
		try
		{
			jbInit();
			setSize(new Dimension(610, 300));
		}
		catch (final Exception e)
		{
			e.printStackTrace();
		}
	}

	private void jbInit() throws Exception
	{
		titledBorder1 = new TitledBorder(BorderFactory.createLineBorder(Color.black, 2), " WE7116");
		titledBorder2 = new TitledBorder(BorderFactory.createLineBorder(Color.black, 2), " WE7275 ");
		this.setWidth(588);
		this.setHeight(588);
		this.setDeviceType("WEGroup");
		this.setDeviceProvider("150.178.3.33");
		this.setDeviceTitle("WE7116 / WE7275  signals group ");
		jPanel1.setLayout(gridBagLayout1);
		flowLayout1.setAlignment(FlowLayout.LEFT);
		deviceField1.setOffsetNid(1);
		deviceField1.setLabelString("Comment:");
		deviceField1.setNumCols(30);
		deviceField1.setIdentifier("");
		jPanel2.setLayout(flowLayout1);
		jPanel3.setLayout(gridLayout2);
		gridLayout2.setColumns(1);
		gridLayout2.setHgap(0);
		gridLayout2.setRows(2);
		gridLayout2.setVgap(0);
		deviceField111115.setOffsetNid(8);
		deviceField111115.setNumCols(7);
		deviceChoice111113.setChoiceItems(new String[]
		{ "100e-3", "200e-3", "500e-3", "1", "2", "5", "10", "20", "50" });
		deviceChoice111113.setChoiceFloatValues(new float[]
		{ (float) 100e-3, (float) 200e-3, (float) 500e-3, 1, 2, 5, 10, 20, 50 });
		deviceChoice111113.setOffsetNid(7);
		deviceChoice111113.setLabelString("  ");
		jPanel119.setLayout(flowLayout2);
		deviceChoice111115.setChoiceIntValues(null);
		deviceChoice111115.setChoiceFloatValues(null);
		deviceChoice111115.setOffsetNid(6);
		deviceChoice111115.setLabelString(" ");
		deviceChoice111115.setChoiceItems(new String[]
		{ "DC", "AC", "GND" });
		deviceChoice111115.setUpdateIdentifier("");
		deviceChoice111115.setIdentifier("");
		deviceField111116.setOffsetNid(5);
		deviceField111116.setNumCols(7);
		flowLayout2.setAlignment(FlowLayout.LEFT);
		flowLayout2.setHgap(5);
		flowLayout2.setVgap(0);
		deviceField111114.setOffsetNid(4);
		deviceField111114.setNumCols(7);
		deviceChoice111114.setChoiceIntValues(new int[]
		{ 0, 500000, 1000000 });
		deviceChoice111114.setChoiceFloatValues(null);
		deviceChoice111114.setConvert(true);
		deviceChoice111114.setOffsetNid(9);
		deviceChoice111114.setLabelString(" ");
		deviceChoice111114.setChoiceItems(new String[]
		{ "OFF", "500 KHz", "1 MHz" });
		deviceChoice111114.setUpdateIdentifier("");
		deviceChoice111114.setIdentifier("");
		jPanel119.setBorder(null);
		jPanel3.setBorder(titledBorder1);
		jPanel4.setLayout(gridLayout3);
		gridLayout3.setColumns(1);
		gridLayout3.setHgap(0);
		gridLayout3.setRows(2);
		gridLayout3.setVgap(0);
		deviceChoice11111111117.setLabelString("  ");
		deviceChoice11111111117.setOffsetNid(14);
		deviceChoice11111111117.setChoiceFloatValues(new float[]
		{ (float) 100e-3, (float) 200e-3, (float) 500e-3, 1, 2, 5, 10, 20, 50, 100, 200, 350 });
		deviceChoice11111111117.setChoiceItems(new String[]
		{ "100e-3", "200e-3", "500e-3", "1", "2", "5", "10", "20", "50", "100", "200", "350" });
		deviceChoice11111111118.setChoiceIntValues(null);
		deviceChoice11111111118.setChoiceFloatValues(null);
		deviceChoice11111111118.setOffsetNid(13);
		deviceChoice11111111118.setLabelString(" ");
		deviceChoice11111111118.setChoiceItems(new String[]
		{ "DC", "AC" });
		deviceChoice11111111118.setUpdateIdentifier("");
		deviceChoice11111111118.setIdentifier("");
		deviceField1111111114.setOffsetNid(11);
		deviceField1111111114.setNumCols(8);
		deviceField1111111114.setIdentifier("");
		flowLayout11114.setVgap(0);
		flowLayout11114.setHgap(0);
		flowLayout11114.setAlignment(FlowLayout.LEFT);
		deviceChoice110.setChoiceIntValues(new int[]
		{ 0, 20, 40, 80, 200, 400, 800, 2000, 4000, 8000, 20000, 40000 });
		deviceChoice110.setChoiceFloatValues(null);
		deviceChoice110.setConvert(true);
		deviceChoice110.setOffsetNid(15);
		deviceChoice110.setLabelString(" ");
		deviceChoice110.setChoiceItems(new String[]
		{ "OFF", "20 Hz", "40 Hz", "80 Hz", "200 Hz", "400 Hz", "800 Hz", "2 kHz", "4 kHz", "8 kHz", "20 kHz",
				"40 kHz" });
		deviceChoice110.setUpdateIdentifier("");
		deviceChoice110.setIdentifier("");
		deviceChoice11111111119.setChoiceIntValues(new int[]
		{ 0, 400, 4000, 40000, 100000 });
		deviceChoice11111111119.setChoiceFloatValues(null);
		deviceChoice11111111119.setConvert(true);
		deviceChoice11111111119.setOffsetNid(16);
		deviceChoice11111111119.setLabelString(" ");
		deviceChoice11111111119.setChoiceItems(new String[]
		{ "OFF", "400 Hz", "4 kNz", "40 kHz", "100 kHz" });
		deviceChoice11111111119.setUpdateIdentifier("");
		deviceChoice11111111119.setIdentifier("");
		deviceField1111111115.setOffsetNid(12);
		deviceField1111111115.setNumCols(8);
		deviceField1111111115.setIdentifier("");
		jPanel11214.setLayout(flowLayout11114);
		jPanel11214.setBorder(null);
		jPanel11214.setDebugGraphicsOptions(0);
		jLabel1.setVerifyInputWhenFocusTarget(true);
		jLabel1.setText("     Coupling            Range                L.P. Filter        "
				+ "    A. A. Filter                   Start                       End");
		jLabel2.setText("      Coupling          Range                     Filter         "
				+ "               Offset                    Start                   " + "        End");
		jLabel2.setAlignmentY((float) 0.0);
		jLabel2.setVerifyInputWhenFocusTarget(true);
		jPanel4.setBorder(titledBorder2);
		jPanel1.setPreferredSize(new Dimension(570, 270));
		deviceButtons1.setMethods(new String[]
		{ "init" });
		this.getContentPane().add(jPanel1, BorderLayout.CENTER);
		jPanel1.add(jPanel2, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
				GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
		jPanel2.add(deviceField1, null);
		jPanel2.add(deviceDispatch1, null);
		jPanel1.add(jPanel4, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
				GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
		jPanel4.add(jLabel1, null);
		jPanel4.add(jPanel11214, null);
		jPanel11214.add(deviceChoice11111111118, null);
		jPanel11214.add(deviceChoice11111111117, null);
		jPanel11214.add(deviceChoice11111111119, null);
		jPanel11214.add(deviceChoice110, null);
		jPanel11214.add(deviceField1111111114, null);
		jPanel11214.add(deviceField1111111115, null);
		jPanel1.add(jPanel3, new GridBagConstraints(0, 2, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
				GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
		jPanel3.add(jLabel2, null);
		jPanel3.add(jPanel119, null);
		jPanel119.add(deviceChoice111115, null);
		jPanel119.add(deviceChoice111113, null);
		jPanel119.add(deviceChoice111114, null);
		jPanel119.add(deviceField111115, null);
		jPanel119.add(deviceField111114, null);
		jPanel119.add(deviceField111116, null);
		jPanel1.add(deviceButtons1, new GridBagConstraints(0, 3, 1, 1, 0.0, 0.0, GridBagConstraints.CENTER,
				GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
	}
}

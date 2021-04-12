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
public class RFXFeedforwardSetupSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceField deviceField1 = new DeviceField();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JTabbedPane jTabbedPane1 = new JTabbedPane();
	DeviceChannel deviceChannel1 = new DeviceChannel();
	DeviceWave deviceWave1 = new DeviceWave();
	DeviceWave deviceWave2 = new DeviceWave();
	DeviceChannel deviceChannel2 = new DeviceChannel();
	DeviceWave deviceWave3 = new DeviceWave();
	DeviceChannel deviceChannel3 = new DeviceChannel();
	DeviceWave deviceWave4 = new DeviceWave();
	DeviceChannel deviceChannel4 = new DeviceChannel();
	DeviceWave deviceWave5 = new DeviceWave();
	DeviceChannel deviceChannel5 = new DeviceChannel();
	DeviceWave deviceWave6 = new DeviceWave();
	DeviceChannel deviceChannel6 = new DeviceChannel();
	DeviceWave deviceWave7 = new DeviceWave();
	DeviceChannel deviceChannel7 = new DeviceChannel();
	DeviceWave deviceWave8 = new DeviceWave();
	DeviceChannel deviceChannel8 = new DeviceChannel();
	DeviceWave deviceWave9 = new DeviceWave();
	DeviceChannel deviceChannel9 = new DeviceChannel();
	DeviceWave deviceWave10 = new DeviceWave();
	DeviceChannel deviceChannel10 = new DeviceChannel();
	DeviceWave deviceWave11 = new DeviceWave();
	DeviceChannel deviceChannel11 = new DeviceChannel();
	DeviceWave deviceWave12 = new DeviceWave();
	DeviceChannel deviceChannel12 = new DeviceChannel();

	public RFXFeedforwardSetupSetup()
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
		this.setHeight(500);
		this.setDeviceType("RFXFeedforwardSetup");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("EDA2 Feedforward Setup");
		this.getContentPane().setLayout(borderLayout1);
		deviceField1.setOffsetNid(1);
		deviceField1.setLabelString("Comment: ");
		deviceField1.setNumCols(25);
		deviceField1.setIdentifier("");
		deviceChannel1.setLabelString("On");
		deviceChannel1.setOffsetNid(3);
		deviceChannel1.setUpdateIdentifier("");
		deviceChannel1.setShowVal("");
		deviceWave1.setOffsetNid(4);
		deviceWave1.setMinYVisible(true);
		deviceWave1.setIdentifier("");
		deviceWave1.setUpdateExpression("");
		deviceWave2.setUpdateExpression("");
		deviceWave2.setIdentifier("");
		deviceWave2.setMinYVisible(true);
		deviceWave2.setOffsetNid(10);
		deviceChannel2.setShowVal("");
		deviceChannel2.setUpdateIdentifier("");
		deviceChannel2.setOffsetNid(9);
		deviceChannel2.setLabelString("On");
		deviceWave3.setUpdateExpression("");
		deviceWave3.setIdentifier("");
		deviceWave3.setMinYVisible(true);
		deviceWave3.setOffsetNid(16);
		deviceChannel3.setShowVal("");
		deviceChannel3.setUpdateIdentifier("");
		deviceChannel3.setOffsetNid(15);
		deviceChannel3.setLabelString("On");
		deviceWave4.setUpdateExpression("");
		deviceWave4.setIdentifier("");
		deviceWave4.setMinYVisible(true);
		deviceWave4.setOffsetNid(22);
		deviceChannel4.setShowVal("");
		deviceChannel4.setUpdateIdentifier("");
		deviceChannel4.setOffsetNid(21);
		deviceChannel4.setLabelString("On");
		deviceWave5.setUpdateExpression("");
		deviceWave5.setIdentifier("");
		deviceWave5.setMinYVisible(true);
		deviceWave5.setOffsetNid(28);
		deviceChannel5.setShowVal("");
		deviceChannel5.setUpdateIdentifier("");
		deviceChannel5.setOffsetNid(27);
		deviceChannel5.setLabelString("On");
		deviceWave6.setUpdateExpression("");
		deviceWave6.setIdentifier("");
		deviceWave6.setMinYVisible(true);
		deviceWave6.setOffsetNid(34);
		deviceChannel6.setShowVal("");
		deviceChannel6.setUpdateIdentifier("");
		deviceChannel6.setOffsetNid(33);
		deviceChannel6.setLabelString("On");
		deviceWave7.setUpdateExpression("");
		deviceWave7.setIdentifier("");
		deviceWave7.setMinYVisible(true);
		deviceWave7.setOffsetNid(40);
		deviceChannel7.setShowVal("");
		deviceChannel7.setUpdateIdentifier("");
		deviceChannel7.setOffsetNid(39);
		deviceChannel7.setLabelString("On");
		deviceWave8.setUpdateExpression("");
		deviceWave8.setIdentifier("");
		deviceWave8.setMinYVisible(true);
		deviceWave8.setOffsetNid(46);
		deviceChannel8.setShowVal("");
		deviceChannel8.setUpdateIdentifier("");
		deviceChannel8.setOffsetNid(45);
		deviceChannel8.setLabelString("On");
		deviceWave9.setUpdateExpression("");
		deviceWave9.setIdentifier("");
		deviceWave9.setMinYVisible(true);
		deviceWave9.setOffsetNid(52);
		deviceChannel9.setShowVal("");
		deviceChannel9.setUpdateIdentifier("");
		deviceChannel9.setOffsetNid(51);
		deviceChannel9.setLines(1);
		deviceChannel9.setLabelString("On");
		deviceWave10.setUpdateExpression("");
		deviceWave10.setIdentifier("");
		deviceWave10.setMinYVisible(true);
		deviceWave10.setOffsetNid(58);
		deviceChannel10.setShowVal("");
		deviceChannel10.setUpdateIdentifier("");
		deviceChannel10.setOffsetNid(57);
		deviceChannel10.setLabelString("On");
		deviceWave11.setUpdateExpression("");
		deviceWave11.setIdentifier("");
		deviceWave11.setMinYVisible(true);
		deviceWave11.setOffsetNid(64);
		deviceChannel11.setShowVal("");
		deviceChannel11.setUpdateIdentifier("");
		deviceChannel11.setOffsetNid(63);
		deviceChannel11.setLabelString("On");
		deviceWave12.setUpdateExpression("");
		deviceWave12.setIdentifier("");
		deviceWave12.setMinYVisible(true);
		deviceWave12.setOffsetNid(70);
		deviceChannel12.setShowVal("");
		deviceChannel12.setUpdateIdentifier("");
		deviceChannel12.setOffsetNid(69);
		deviceChannel12.setLabelString("On");
		this.getContentPane().add(deviceField1, BorderLayout.NORTH);
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jTabbedPane1, BorderLayout.CENTER);
		jTabbedPane1.add(deviceChannel1, "1");
		deviceChannel1.add(deviceWave1, BorderLayout.CENTER);
		jTabbedPane1.add(deviceChannel2, "2");
		deviceChannel2.add(deviceWave2, BorderLayout.CENTER);
		jTabbedPane1.add(deviceChannel3, "3");
		deviceChannel3.add(deviceWave3, BorderLayout.CENTER);
		jTabbedPane1.add(deviceChannel4, "4");
		deviceChannel4.add(deviceWave4, BorderLayout.CENTER);
		jTabbedPane1.add(deviceChannel5, "5");
		deviceChannel5.add(deviceWave5, BorderLayout.CENTER);
		jTabbedPane1.add(deviceChannel6, "6");
		deviceChannel6.add(deviceWave6, BorderLayout.CENTER);
		jTabbedPane1.add(deviceChannel7, "7");
		deviceChannel7.add(deviceWave7, BorderLayout.CENTER);
		jTabbedPane1.add(deviceChannel8, "8");
		deviceChannel8.add(deviceWave8, BorderLayout.CENTER);
		jTabbedPane1.add(deviceChannel9, "9");
		deviceChannel9.add(deviceWave9, BorderLayout.CENTER);
		jTabbedPane1.add(deviceChannel10, "10");
		deviceChannel10.add(deviceWave10, BorderLayout.CENTER);
		jTabbedPane1.add(deviceChannel11, "11");
		deviceChannel11.add(deviceWave11, BorderLayout.CENTER);
		jTabbedPane1.add(deviceChannel12, "12");
		deviceChannel12.add(deviceWave12, BorderLayout.CENTER);
	}
}

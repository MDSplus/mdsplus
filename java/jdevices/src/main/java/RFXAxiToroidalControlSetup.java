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
public class RFXAxiToroidalControlSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JTabbedPane jTabbedPane1 = new JTabbedPane();
	JPanel jPanel1 = new JPanel();
	DeviceField deviceField1 = new DeviceField();
	DeviceWave deviceWave1 = new DeviceWave();
	DeviceWave deviceWave2 = new DeviceWave();
	DeviceWave deviceWave3 = new DeviceWave();
	DeviceWave deviceWave4 = new DeviceWave();

	public RFXAxiToroidalControlSetup()
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
		this.setWidth(471);
		this.setHeight(471);
		this.setDeviceType("RFXAxiToroidalControl");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("Toroidal Axisymmetric Control Setup");
		this.getContentPane().setLayout(borderLayout1);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Comment: ");
		deviceField1.setNumCols(30);
		deviceField1.setIdentifier("");
		deviceWave1.setOffsetNid(3);
		deviceWave1.setMinYVisible(true);
		deviceWave1.setIdentifier("");
		deviceWave1.setUpdateExpression("");
		deviceWave2.setOffsetNid(9);
		deviceWave2.setMinYVisible(true);
		deviceWave2.setIdentifier("");
		deviceWave2.setUpdateExpression("");
		deviceWave3.setOffsetNid(15);
		deviceWave3.setMinYVisible(true);
		deviceWave3.setIdentifier("");
		deviceWave3.setUpdateExpression("");
		deviceWave4.setUpdateExpression("");
		deviceWave4.setIdentifier("");
		deviceWave4.setMinYVisible(true);
		deviceWave4.setOffsetNid(21);
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jTabbedPane1, BorderLayout.CENTER);
		jTabbedPane1.add(deviceWave1, "Wall Bt");
		jTabbedPane1.add(deviceWave2, "F");
		jTabbedPane1.add(deviceWave3, "Q");
		jTabbedPane1.add(deviceWave4, "Flux");
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(deviceField1, null);
	}
}

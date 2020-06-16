
//package jTraverser;
import javax.swing.*;
import java.awt.*;

public class DeviceLabeledExprEditor extends JPanel
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	DeviceExprEditor expr;

	/*
	 * public LabeledExprEditor(String data) { this("Expression", new
	 * ExprEditor(data, (data != null && data.dtype == Data.DTYPE_T), 4, 20)); }
	 * public LabeledExprEditor(String label_str) { this(label_str, new
	 * ExprEditor(null, false)); }
	 */
	public DeviceLabeledExprEditor(String label_str, DeviceExprEditor expr)
	{
		this.expr = expr;
		setLayout(new BorderLayout());
		setBorder(BorderFactory.createTitledBorder(label_str));
		add(expr, "Center");
	}

	public String getData()
	{ return expr.getData(); }

	public void reset()
	{
		expr.reset();
	}

	public void setData(String data)
	{
		expr.setData(data);
	}

	public void setEditable(boolean editable)
	{
		expr.setEditable(editable);
	}
}

package mds.jtraverser;

//package jTraverser;
import javax.swing.*;
import java.awt.*;

public class LabeledExprEditor extends JPanel implements Editor
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	ExprEditor expr;

	public LabeledExprEditor(MDSplus.Data data)
	{
		this("Expression", new ExprEditor(data, (data != null && (data instanceof MDSplus.String)), 4, 20));
	}

	public LabeledExprEditor(String label_str)
	{
		this(label_str, new ExprEditor(null, false));
	}

	public LabeledExprEditor(String label_str, ExprEditor expr)
	{
		this.expr = expr;
		setLayout(new BorderLayout());
		setBorder(BorderFactory.createTitledBorder(label_str));
		add(expr, "Center");
	}

	@Override
	public MDSplus.Data getData()
	{ return expr.getData(); }

	@Override
	public void reset()
	{
		expr.reset();
	}

	public void setData(MDSplus.Data data)
	{
		expr.setData(data);
	}

	@Override
	public void setEditable(boolean editable)
	{
		expr.setEditable(editable);
	}
}

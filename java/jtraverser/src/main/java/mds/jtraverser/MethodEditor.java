package mds.jtraverser;

//package jTraverser;
import javax.swing.*;
import java.awt.*;

public class MethodEditor extends JPanel implements Editor
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	MDSplus.Method method;
	LabeledExprEditor device_edit, method_edit, timeout_edit;
	ArgEditor arg_edit;

	public MethodEditor()
	{
		this(null);
	}

	public MethodEditor(MDSplus.Method method)
	{
		this.method = method;
		if (this.method == null)
		{
			this.method = new MDSplus.Method(null, null, null, new MDSplus.Data[0]);
		}
		this.method.setCtxTree(Tree.curr_experiment);
		setLayout(new BorderLayout());
		device_edit = new LabeledExprEditor("Device", new ExprEditor(this.method.getObject(), true));
		method_edit = new LabeledExprEditor("Method", new ExprEditor(this.method.getMethod(), true));
		final JPanel jp = new JPanel();
		jp.setLayout(new GridLayout(2, 1));
		jp.add(device_edit);
		jp.add(method_edit);
		add(jp, BorderLayout.NORTH);
		arg_edit = new ArgEditor(this.method.getArguments());
		add(arg_edit, BorderLayout.CENTER);
		timeout_edit = new LabeledExprEditor("Timeout", new ExprEditor(this.method.getTimeout(), false));
		add(timeout_edit, BorderLayout.SOUTH);
	}

	@Override
	public MDSplus.Data getData()
	{
		final MDSplus.Method m = new MDSplus.Method(timeout_edit.getData(), method_edit.getData(),
				device_edit.getData(), arg_edit.getData());
		m.setCtxTree(Tree.curr_experiment);
		return m;
	}

	@Override
	public void reset()
	{
		device_edit.reset();
		method_edit.reset();
		arg_edit.reset();
		timeout_edit.reset();
	}

	public void setData(MDSplus.Data data)
	{
		this.method = (MDSplus.Method) data;
		if (this.method == null)
		{
			this.method = new MDSplus.Method(null, null, null, new MDSplus.Data[0]);
		}
		this.method.setCtxTree(Tree.curr_experiment);
		reset();
	}

	@Override
	public void setEditable(boolean editable)
	{
		if (device_edit != null)
			device_edit.setEditable(editable);
		if (method_edit != null)
			method_edit.setEditable(editable);
		if (timeout_edit != null)
			timeout_edit.setEditable(editable);
		if (arg_edit != null)
			arg_edit.setEditable(editable);
	}
}

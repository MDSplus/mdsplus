package mds.jtraverser;

//package jTraverser;
import javax.swing.*;
import java.awt.*;

public class RoutineEditor extends JPanel implements Editor
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	MDSplus.Routine routine;
	LabeledExprEditor image_edit, routine_edit, timeout_edit;
	ArgEditor arg_edit;

	public RoutineEditor()
	{
		this(null);
	}

	public RoutineEditor(MDSplus.Routine routine)
	{
		this.routine = routine;
		if (this.routine == null)
		{
			this.routine = new MDSplus.Routine(null, null, null, new MDSplus.Data[0]);
		}
		this.routine.setCtxTree(Tree.curr_experiment);
		setLayout(new BorderLayout());
		final JPanel jp = new JPanel();
		final GridLayout gl = new GridLayout(2, 1);
		gl.setVgap(0);
		jp.setLayout(gl);
		image_edit = new LabeledExprEditor("Image", new ExprEditor(this.routine.getImage(), true));
		routine_edit = new LabeledExprEditor("Routine", new ExprEditor(this.routine.getRoutine(), true));
		jp.add(image_edit);
		jp.add(routine_edit);
		add(jp, "North");
		arg_edit = new ArgEditor(this.routine.getArguments());
		add(arg_edit, "Center");
		timeout_edit = new LabeledExprEditor("Timeout", new ExprEditor(this.routine.getTimeout(), false));
		add(timeout_edit, "South");
	}

	@Override
	public MDSplus.Data getData()
	{
		timeout_edit.getData();
		image_edit.getData();
		routine_edit.getData();
		arg_edit.getData();
		final MDSplus.Data r = new MDSplus.Routine(timeout_edit.getData(), image_edit.getData(), routine_edit.getData(),
				arg_edit.getData());
		r.setCtxTree(Tree.curr_experiment);
		return r;
	}

	@Override
	public void reset()
	{
		image_edit.reset();
		routine_edit.reset();
		arg_edit.reset();
		timeout_edit.reset();
	}

	public void setData(MDSplus.Data data)
	{
		this.routine = (MDSplus.Routine) data;
		if (this.routine == null)
		{
			this.routine = new MDSplus.Routine(null, null, null, new MDSplus.Data[0]);
		}
		this.routine.setCtxTree(Tree.curr_experiment);
		reset();
	}

	@Override
	public void setEditable(boolean editable)
	{
		if (image_edit != null)
			image_edit.setEditable(editable);
		if (routine_edit != null)
			routine_edit.setEditable(editable);
		if (timeout_edit != null)
			timeout_edit.setEditable(editable);
		if (arg_edit != null)
			arg_edit.setEditable(editable);
	}
}

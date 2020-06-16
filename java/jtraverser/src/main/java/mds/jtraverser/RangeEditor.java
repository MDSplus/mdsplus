package mds.jtraverser;

//package jTraverser;
import javax.swing.*;
import java.awt.*;

public class RangeEditor extends JPanel implements Editor
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	MDSplus.Range range;
	LabeledExprEditor begin_edit, end_edit, delta_edit;

	public RangeEditor()
	{
		this(null);
	}

	public RangeEditor(MDSplus.Range range)
	{
		this.range = range;
		if (this.range == null)
		{
			this.range = new MDSplus.Range(null, null, null);
		}
		this.range.setCtxTree(Tree.curr_experiment);
		final GridLayout gl = new GridLayout(3, 1);
		gl.setVgap(0);
		setLayout(gl);
		begin_edit = new LabeledExprEditor("Start", new ExprEditor(this.range.getBegin(), false));
		add(begin_edit);
		end_edit = new LabeledExprEditor("End", new ExprEditor(this.range.getEnding(), false));
		add(end_edit);
		delta_edit = new LabeledExprEditor("Increment", new ExprEditor(this.range.getDeltaVal(), false));
		add(delta_edit);
	}

	@Override
	public MDSplus.Data getData()
	{
		final MDSplus.Data r = new MDSplus.Range(begin_edit.getData(), end_edit.getData(), delta_edit.getData());
		r.setCtxTree(Tree.curr_experiment);
		return r;
	}

	@Override
	public void reset()
	{
		begin_edit.reset();
		end_edit.reset();
		delta_edit.reset();
	}

	public void setData(MDSplus.Data data)
	{
		this.range = (MDSplus.Range) data;
		if (this.range == null)
		{
			this.range = new MDSplus.Range(null, null, null);
		}
		this.range.setCtxTree(Tree.curr_experiment);
		begin_edit.setData(range.getBegin());
		end_edit.setData(range.getEnding());
		delta_edit.setData(range.getDeltaVal());
		reset();
	}

	@Override
	public void setEditable(boolean editable)
	{
		if (begin_edit != null)
			begin_edit.setEditable(editable);
		if (end_edit != null)
			end_edit.setEditable(editable);
		if (delta_edit != null)
			delta_edit.setEditable(editable);
	}
}

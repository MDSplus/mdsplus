package mds.jtraverser;

//package jTraverser;
import javax.swing.*;
import java.awt.*;

public class ProgramEditor extends JPanel implements Editor
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	MDSplus.Program program;
	LabeledExprEditor program_edit, timeout_edit;

	public ProgramEditor()
	{
		this(null);
	}

	public ProgramEditor(MDSplus.Program program)
	{
		this.program = program;
		if (this.program == null)
		{
			this.program = new MDSplus.Program(null, null);
		}
		this.program.setCtxTree(Tree.curr_experiment);
		program_edit = new LabeledExprEditor("Program", new ExprEditor(this.program.getProgram(), true));
		timeout_edit = new LabeledExprEditor("Timeout", new ExprEditor(this.program.getTimeout(), false));
		final JPanel jp = new JPanel();
		jp.setLayout(new GridLayout(2, 1));
		jp.add(program_edit);
		jp.add(timeout_edit);
		setLayout(new BorderLayout());
		add(jp, BorderLayout.NORTH);
	}

	@Override
	public MDSplus.Data getData()
	{
		final MDSplus.Data p = new MDSplus.Program(timeout_edit.getData(), program_edit.getData());
		p.setCtxTree(Tree.curr_experiment);
		return p;
	}

	@Override
	public void reset()
	{
		program_edit.reset();
		timeout_edit.reset();
	}

	public void setData(MDSplus.Data data)
	{
		this.program = (MDSplus.Program) data;
		if (this.program == null)
		{
			this.program = new MDSplus.Program(null, null);
		}
		reset();
	}

	@Override
	public void setEditable(boolean editable)
	{
		if (program_edit != null)
			program_edit.setEditable(editable);
		if (timeout_edit != null)
			timeout_edit.setEditable(editable);
	}
}

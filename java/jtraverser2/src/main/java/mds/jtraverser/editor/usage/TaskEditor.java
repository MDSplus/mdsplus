package mds.jtraverser.editor.usage;

import java.awt.Window;

import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Method;
import mds.data.descriptor_r.Procedure;
import mds.data.descriptor_r.Program;
import mds.data.descriptor_r.Routine;
import mds.jtraverser.editor.MethodEditor;
import mds.jtraverser.editor.ProcedureEditor;
import mds.jtraverser.editor.ProgramEditor;
import mds.jtraverser.editor.RoutineEditor;

@SuppressWarnings("deprecation")
public class TaskEditor extends TextEditor
{
	private static final long serialVersionUID = 1L;

	public TaskEditor(final boolean editable, final CTX ctx, final Window window)
	{
		this(null, editable, ctx, window);
	}

	public TaskEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final Window window)
	{
		super(data, editable, ctx, window, "Task", false, "Method", "Routine", "Procedure (depreciated)",
				"Program (depreciated)");
	}

	@Override
	protected boolean addExtraEditor()
	{
		switch (this.curr_mode_idx - this.mode_idx_usr)
		{
		case 0:
			if (this.mode_idx == this.curr_mode_idx)
				this.editor.add(this.data_edit = new MethodEditor((Method) this.data, this.editable, this.ctx));
			else
				this.editor.add(this.data_edit = new MethodEditor(null, this.editable, this.ctx));
			return true;
		case 1:
			if (this.mode_idx == this.curr_mode_idx)
				this.editor.add(this.data_edit = new RoutineEditor((Routine) this.data, this.editable, this.ctx));
			else
				this.editor.add(this.data_edit = new RoutineEditor(null, this.editable, this.ctx));
			return true;
		case 2:
			if (this.mode_idx == this.curr_mode_idx)
				this.editor.add(this.data_edit = new ProcedureEditor((Procedure) this.data, this.editable, this.ctx));
			else
				this.editor.add(this.data_edit = new ProcedureEditor(null, this.editable, this.ctx));
			return true;
		case 3:
			if (this.mode_idx == this.curr_mode_idx)
				this.editor.add(this.data_edit = new ProgramEditor((Program) this.data, this.editable, this.ctx));
			else
				this.editor.add(this.data_edit = new ProgramEditor(null, this.editable, this.ctx));
			return true;
		default:
			return false;
		}
	}

	@Override
	protected boolean checkUsrData()
	{
		if (this.data instanceof Method)
			this.mode_idx = this.mode_idx_usr;
		else if (this.data instanceof Routine)
			this.mode_idx = this.mode_idx_usr + 1;
		else if (this.data instanceof Procedure)
			this.mode_idx = this.mode_idx_usr + 2;
		else if (this.data instanceof Program)
			this.mode_idx = this.mode_idx_usr + 3;
		else
			return false;
		return true;
	}
}

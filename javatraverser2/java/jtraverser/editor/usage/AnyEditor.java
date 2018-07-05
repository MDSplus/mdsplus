package jtraverser.editor.usage;

import java.awt.Window;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;

@SuppressWarnings("serial")
public final class AnyEditor extends TextEditor{
    public AnyEditor(final boolean editable, final CTX ctx, final Window window){
        this(null, editable, ctx, window);
    }

    public AnyEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final Window window){
        super(data, editable, ctx, window, "Data", "Action", "Python Expression", "Signal", "Window", "Method", "Routine", "Program (depreciated)", "Procedure (depreciated)");
    }

    @Override
    protected final boolean addExtraEditor() {
        if(this.curr_mode_idx < this.mode_idx_usr) return false;
        switch(this.curr_mode_idx - this.mode_idx_usr){
            case 0:
                this.editor.add(this.data_edit = new jtraverser.editor.ActionEditor(this.editable, this.ctx, this.window));
                return true;
            case 1:
                this.editor.add(this.data_edit = new jtraverser.editor.PythonEditor(this.editable, this.ctx));
                return true;
            case 2:
                this.editor.add(this.data_edit = new jtraverser.editor.SignalEditor(this.data, this.editable, this.ctx, this.window));
                return true;
            case 3:
                this.editor.add(this.data_edit = new jtraverser.editor.WindowEditor(this.editable, this.ctx));
                return true;
            case 4:
                this.editor.add(this.data_edit = new jtraverser.editor.MethodEditor(this.editable, this.ctx));
                return true;
            case 5:
                this.editor.add(this.data_edit = new jtraverser.editor.RoutineEditor(this.editable, this.ctx));
                return true;
            case 6:
                this.editor.add(this.data_edit = new jtraverser.editor.ProgramEditor(this.editable, this.ctx));
                return true;
            case 7:
                this.editor.add(this.data_edit = new jtraverser.editor.ProcedureEditor(this.editable, this.ctx));
                return true;
            default:
                return false;
        }
    }

    @Override
    protected final boolean checkUsrData() {
        final int usrmde = this.getUsrMode();
        if(usrmde < 0) return false;
        this.mode_idx = this.mode_idx_usr + usrmde;
        return true;
    }

    private final int getUsrMode() {
        if(jtraverser.editor.ActionEditor.checkData(this.data)) return 0;
        if(jtraverser.editor.PythonEditor.checkData(this.data)) return 1;
        if(jtraverser.editor.SignalEditor.checkData(this.data)) return 2;
        if(jtraverser.editor.WindowEditor.checkData(this.data)) return 3;
        if(jtraverser.editor.MethodEditor.checkData(this.data)) return 4;
        if(jtraverser.editor.RoutineEditor.checkData(this.data)) return 5;
        if(jtraverser.editor.ProgramEditor.checkData(this.data)) return 6;
        if(jtraverser.editor.ProcedureEditor.checkData(this.data)) return 7;
        return -1;
    }
}
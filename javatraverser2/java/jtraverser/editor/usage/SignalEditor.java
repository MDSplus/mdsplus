package jtraverser.editor.usage;

import java.awt.Window;
import mds.data.descriptor.Descriptor;

@SuppressWarnings("serial")
public class SignalEditor extends TextEditor{
    public SignalEditor(final boolean editable, final Window window){
        this(null, editable, window);
    }

    public SignalEditor(final Descriptor<?> data, final boolean editable, final Window window){
        super(data, editable, window, "Signal", "Signal");
    }

    @Override
    protected final boolean addExtraEditor() {
        if(this.curr_mode_idx != this.mode_idx_usr) return false;
        this.editor.add(this.data_edit = new jtraverser.editor.SignalEditor(this.data, this.editable, this.window));
        return true;
    }

    @Override
    protected final boolean checkUsrData() {
        if(!jtraverser.editor.SignalEditor.checkData(this.data)) return false;
        this.mode_idx = this.mode_idx_usr;
        return true;
    }
}
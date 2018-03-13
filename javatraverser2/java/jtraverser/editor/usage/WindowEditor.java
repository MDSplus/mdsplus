package jtraverser.editor.usage;

import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Window;

@SuppressWarnings("serial")
public class WindowEditor extends TextEditor{
    public WindowEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final java.awt.Window window){
        super(data, editable, ctx, window, "Window", "Window");
    }

    @Override
    protected final boolean addExtraEditor() {
        if(this.curr_mode_idx != this.mode_idx_usr) return false;
        this.editor.add(this.data_edit = new jtraverser.editor.WindowEditor((Window)this.data, this.editable, this.ctx));
        return true;
    }

    @Override
    protected final boolean checkUsrData() {
        if(!(this.data instanceof Window)) return false;
        this.mode_idx = this.mode_idx_usr;
        return true;
    }
}
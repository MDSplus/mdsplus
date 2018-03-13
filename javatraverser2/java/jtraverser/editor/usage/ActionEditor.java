package jtraverser.editor.usage;

import java.awt.BorderLayout;
import java.awt.Window;
import javax.swing.JLabel;
import javax.swing.SwingConstants;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Action;

@SuppressWarnings("serial")
public final class ActionEditor extends TextEditor{
    private final int     status;
    private final boolean ismodel;

    public ActionEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final Window window){
        this(data, editable, ctx, window, 0, true);
    }

    public ActionEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final Window window, final int status, final boolean ismodel){
        super(null, editable, ctx, window, "Action", false, "Action");
        this.status = status;
        this.ismodel = ismodel;
        this.setData(data);
    }

    @Override
    protected final boolean addExtraEditor() {
        if(!this.ismodel) this.editor.add(new JLabel(Action.getStatusMsg(this.status), SwingConstants.CENTER), BorderLayout.SOUTH);
        if(this.curr_mode_idx < this.mode_idx_usr) return false;
        this.editor.add(this.data_edit = new jtraverser.editor.ActionEditor(this.editable, this.ctx, this.window), BorderLayout.CENTER);
        return true;
    }

    @Override
    protected final boolean checkUsrData() {
        if(!jtraverser.editor.ActionEditor.checkData(this.data)) return false;
        this.mode_idx = this.mode_idx_usr;
        return true;
    }
}
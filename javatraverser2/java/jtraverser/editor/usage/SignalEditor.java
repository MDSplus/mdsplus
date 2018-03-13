package jtraverser.editor.usage;

import java.awt.Window;
import jtraverser.editor.ArrayEditor;
import jtraverser.editor.ParameterEditor;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_CA;
import mds.data.descriptor_a.NUMBERArray;
import mds.data.descriptor_r.PARAMETER;

@SuppressWarnings("serial")
public class SignalEditor extends TextEditor{
    public SignalEditor(final boolean editable, final CTX ctx, final Window window){
        this(null, editable, ctx, window);
    }

    public SignalEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final Window window){
        super(data, editable, ctx, window, "Signal", "Array", "Array w/ Param", "Signal");
    }

    @Override
    protected final boolean addExtraEditor() {
        if(this.curr_mode_idx < this.mode_idx_usr) return false;
        if(this.curr_mode_idx == this.mode_idx_usr) this.editor.add(this.data_edit = new ArrayEditor(this.data, this.editable, this.ctx));
        else if(this.curr_mode_idx == this.mode_idx_usr + 1) this.editor.add(this.data_edit = new ParameterEditor(this.data, this.editable, this.ctx, new ArrayEditor(null, this.editable, this.ctx)));
        else this.editor.add(this.data_edit = new jtraverser.editor.SignalEditor(this.data, this.editable, this.ctx, this.window));
        return true;
    }

    @Override
    protected final boolean checkUsrData() {
        if(jtraverser.editor.SignalEditor.checkData(this.data)){
            this.mode_idx = this.mode_idx_usr + 2;
            return true;
        }
        Descriptor<?> tdata = this.data;
        while(ParameterEditor.hasParams(tdata))
            tdata = ((PARAMETER)tdata).getValue();
        final boolean noparam = tdata == this.data;
        if(tdata instanceof NUMBERArray || tdata instanceof Descriptor_CA){
            this.mode_idx = noparam ? this.mode_idx_usr : this.mode_idx_usr + 1;
            return true;
        }
        return false;
    }
}
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
public class NumericEditor extends TextEditor{
    public NumericEditor(final boolean editable, final CTX ctx, final Window window, final String name){
        this(null, editable, ctx, window, name);
    }

    public NumericEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final Window window){
        this(data, editable, ctx, window, "Numeric");
    }

    public NumericEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final Window window, final String name){
        super(data, editable, ctx, window, name, "Array", "Array w/ Param");
    }

    @Override
    protected final boolean addExtraEditor() {
        if(this.curr_mode_idx < this.mode_idx_usr) return false;
        if(this.curr_mode_idx == this.mode_idx_usr) this.editor.add(this.data_edit = new ArrayEditor(this.data, this.editable, this.ctx));
        else this.editor.add(this.data_edit = new ParameterEditor(this.data, this.editable, this.ctx, new ArrayEditor(null, this.editable, this.ctx)));
        return true;
    }

    @Override
    protected final boolean checkUsrData() {
        Descriptor<?> tdata = this.data;
        while(ParameterEditor.hasParams(tdata))
            tdata = ((PARAMETER)tdata).getValue();
        final boolean noparam = tdata == this.data;
        if(!(tdata instanceof NUMBERArray || tdata instanceof Descriptor_CA)) return false;
        this.mode_idx = noparam ? this.mode_idx_usr : this.mode_idx_usr + 1;
        return true;
    }
}
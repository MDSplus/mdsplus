package jtraverser.editor.usage;

import java.awt.Window;
import jtraverser.editor.ParameterEditor;
import jtraverser.editor.RangeEditor;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.PARAMETER;
import mds.data.descriptor_r.Range;

@SuppressWarnings("serial")
public class AxisEditor extends TextEditor{
    public AxisEditor(final boolean editable, final CTX ctx, final Window window){
        this(null, editable, ctx, window);
    }

    public AxisEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final Window window){
        super(data, editable, ctx, window, "Range", "Range", "Range W/ Param");
    }

    @Override
    protected final boolean addExtraEditor() {
        if(this.curr_mode_idx < this.mode_idx_usr) return false;
        if(this.curr_mode_idx == this.mode_idx_usr) this.editor.add(this.data_edit = new RangeEditor(this.editable, this.ctx));
        else this.editor.add(this.data_edit = new ParameterEditor(this.data, this.editable, this.ctx, new RangeEditor(this.editable, this.ctx)));
        return true;
    }

    @Override
    public final boolean checkUsrData() {
        Descriptor<?> tdata = this.data;
        while(ParameterEditor.hasParams(tdata))
            tdata = ((PARAMETER)tdata).getValue();
        final boolean noparam = tdata == this.data;
        if(!(tdata instanceof Range)) return false;
        this.mode_idx = noparam ? this.mode_idx_usr : this.mode_idx_usr + 1;
        return true;
    }
}

package jtraverser.editor;

import java.awt.GridLayout;
import mds.MdsException;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Window;

@SuppressWarnings("serial")
public final class WindowEditor extends Editor{
    public static final boolean checkData(final Descriptor<?> data) {
        return data instanceof Window;
    }
    Editor startidx_edit, endidx_edit, value0_edit;

    public WindowEditor(final boolean editable, final CTX ctx){
        this(null, editable, ctx);
    }

    public WindowEditor(final Window data, final boolean editable, final CTX ctx){
        super(data, editable, ctx, 3);
        this.setLayout(new GridLayout(3, 1, 0, 0));
        this.add(this.edit[0] = Editor.addLabel("Start Idx", new ExprEditor(WindowEditor.this.editable, ctx, false, false)));
        this.add(this.edit[1] = Editor.addLabel("End Idx", new ExprEditor(WindowEditor.this.editable, ctx, false, false)));
        this.add(this.edit[2] = Editor.addLabel("Time of Zero", new ExprEditor(WindowEditor.this.editable, ctx, false, false)));
    }

    @Override
    public final Descriptor<?> getData() throws MdsException {
        return new Window(this.startidx_edit.getData(), this.endidx_edit.getData(), this.value0_edit.getData());
    }

    @Override
    public final void setData(final Descriptor<?> data) {
        this.data = data;
        this.setDescR();
    }
}

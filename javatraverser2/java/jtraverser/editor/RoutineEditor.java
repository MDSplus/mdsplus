package jtraverser.editor;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import javax.swing.JPanel;
import mds.MdsException;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;
import mds.data.descriptor_r.Routine;

@SuppressWarnings("serial")
public final class RoutineEditor extends Editor{
    public static final boolean checkData(final Descriptor<?> data) {
        return data instanceof Routine;
    }
    ArgEditor arg_edit;

    public RoutineEditor(final boolean editable, final CTX ctx){
        this(null, editable, ctx);
    }

    public RoutineEditor(final CTX ctx){
        this(null, true, ctx);
    }

    public RoutineEditor(final Routine data, final boolean editable, final CTX ctx){
        super(data, editable, ctx, 3);
        final JPanel jp = new JPanel();
        jp.setLayout(new GridLayout(3, 1, 0, 0));
        jp.add(this.edit[0] = Editor.addLabel("Timeout", new ExprEditor(editable, ctx, false, false)));
        jp.add(this.edit[1] = Editor.addLabel("Image", new ExprEditor(editable, ctx, true, false)));
        jp.add(this.edit[2] = Editor.addLabel("Routine", new ExprEditor(editable, ctx, true, false)));
        this.setLayout(new BorderLayout());
        this.add(jp, BorderLayout.NORTH);
        this.add(this.arg_edit = new ArgEditor(editable, ctx), BorderLayout.CENTER);
    }

    @Override
    public final Routine getData() throws MdsException {
        return new Routine(this.edit[0].getData(), this.edit[1].getData(), this.edit[2].getData(), this.arg_edit.getDataArray());
    }

    @Override
    public final void setData(final Descriptor<?> data) {
        this.data = data;
        if(this.data instanceof Descriptor_R<?>){
            this.setDescR();
            this.arg_edit.setData(((Descriptor_R<?>)data).getDescriptors(3, -1));
        }else{
            for(final Editor element : this.edit)
                element.setData(null);
            this.arg_edit.setData();
        }
    }
}
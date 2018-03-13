package jtraverser.editor;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import javax.swing.JPanel;
import mds.MdsException;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Program;

@SuppressWarnings({"deprecation", "serial"})
public final class ProgramEditor extends Editor{
    public static final boolean checkData(final Descriptor<?> data) {
        return data instanceof Program;
    }

    public ProgramEditor(final boolean editable, final CTX ctx){
        this(null, editable, ctx);
    }

    public ProgramEditor(final CTX ctx){
        this(null, true, ctx);
    }

    public ProgramEditor(final Program data, final boolean editable, final CTX ctx){
        super(data, editable, ctx, 2);
        final JPanel jp = new JPanel();
        jp.setLayout(new GridLayout(2, 1));
        jp.add(this.edit[0] = Editor.addLabel("Timeout", new ExprEditor(editable, ctx, false, false)));
        jp.add(this.edit[1] = Editor.addLabel("Program", new ExprEditor(editable, ctx, true, false)));
        this.setLayout(new BorderLayout());
        this.add(jp, BorderLayout.NORTH);
    }

    @Override
    public final Descriptor<?> getData() throws MdsException {
        return new Program(this.edit[0].getData(), this.edit[1].getData());
    }

    @Override
    public final void setData(final Descriptor<?> data) {
        this.data = data;
        this.setDescR();
    }
}
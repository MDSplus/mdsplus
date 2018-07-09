package jtraverser.editor;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.Window;
import javax.swing.JPanel;
import jtraverser.editor.usage.NumericEditor;
import mds.MdsException;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Signal;

@SuppressWarnings("serial")
public class SignalEditor extends Editor{
    public static final boolean checkData(final Descriptor<?> data) {
        return data instanceof Signal;
    }

    public SignalEditor(final boolean editable, final CTX ctx, final Window window){
        this(null, editable, ctx, window);
    }

    public SignalEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final Window window){
        super(data, editable, ctx, 3);
        this.setLayout(new BorderLayout());
        final JPanel columns = new JPanel(new GridLayout(1, 3));
        columns.add(this.edit[0] = new NumericEditor(this.editable, ctx, window, "Data"));
        columns.add(this.edit[1] = new NumericEditor(this.editable, ctx, window, "Raw"));
        columns.add(this.edit[2] = new NumericEditor(this.editable, ctx, window, "Dimension"));
        this.add(columns);
        this.setData(data);
    }

    @Override
    public final Descriptor<?> getData() throws MdsException {
        return new Signal(this.edit[0].getData(), this.edit[1].getData(), this.edit[2].getData());
    }

    @Override
    public final void setData(final Descriptor<?> data) {
        this.data = data;
        if(SignalEditor.checkData(data)) this.setDescR();
        else{
            this.edit[0].setData(data);
            this.edit[1].data = null;
            this.edit[2].data = null;
        }
    }
}
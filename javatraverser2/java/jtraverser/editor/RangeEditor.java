package jtraverser.editor;

import java.awt.GridLayout;
import mds.MdsException;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Range;

@SuppressWarnings("serial")
public class RangeEditor extends Editor{
    public RangeEditor(){
        this(true);
    }

    public RangeEditor(final boolean editable){
        this(null, editable);
    }

    public RangeEditor(final Range data, final boolean editable){
        super(data, editable, 3);
        this.setLayout(new GridLayout(3, 1, 0, 0));
        this.add(this.edit[0] = Editor.addLabel("Start", new ExprEditor(false, editable, false)));
        this.add(this.edit[1] = Editor.addLabel("End", new ExprEditor(false, editable, false)));
        this.add(this.edit[2] = Editor.addLabel("Increment", new ExprEditor(false, editable, false)));
    }

    @Override
    public final Range getData() throws MdsException {
        return new Range(this.edit[0].getData(), this.edit[1].getData(), this.edit[2].getData());
    }

    @Override
    public final void setData(final Descriptor<?> data) {
        this.data = data;
        this.setDescR();
    }
}
package jtraverser.editor;

import java.awt.Component;
import javax.swing.BorderFactory;
import javax.swing.JComponent;
import javax.swing.JPanel;
import mds.Mds;
import mds.MdsException;
import mds.data.CTX;
import mds.data.DTYPE;
import mds.data.TREE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;
import mds.data.descriptor_s.NUMBER;

@SuppressWarnings("serial")
public abstract class Editor extends JPanel{
    public static final <T extends JComponent> T addLabel(final String label, final T editor) {
        ((JComponent)editor).setBorder(BorderFactory.createTitledBorder(label));
        return editor;
    }

    protected static final boolean isNoData(final Descriptor<?> data) {
        return data == null || data.dtype() == DTYPE.MISSING;
    }
    protected final boolean editable;
    protected final Editor  edit[];
    protected Descriptor<?> data;
    protected CTX           ctx;

    protected Editor(final Descriptor<?> data, final boolean editable, final CTX ctx, final int nargs){
        this.data = data;
        this.editable = editable;
        this.edit = new Editor[nargs];
        this.ctx = ctx;
    }

    public abstract Descriptor<?> getData() throws MdsException;

    protected Mds getMds() {
        if(this.ctx instanceof TREE) return ((TREE)this.ctx).mds;
        return Mds.getActiveMds();
    }

    protected NUMBER<?> getNumber() throws MdsException {
        final Descriptor<?> num = this.getData().getDataD();
        if(num instanceof NUMBER<?>) return (NUMBER<?>)num;
        throw new MdsException(MdsException.TdiNOT_NUMBER);
    }

    public void interrupt() {/*stub*/}

    @SuppressWarnings("static-method")
    public boolean isNull() {
        return false;
    }

    public void reset(final boolean hard) {
        for(final Component component : this.getComponents())
            if(component instanceof Editor) ((Editor)component).reset(hard);
    }

    public void setData(final Descriptor<?> data) {
        this.data = data;
        this.reset(false);
    }

    protected final void setDescR() {
        if(this.data instanceof Descriptor_R<?>){
            final Descriptor_R<?> dsc_r = (Descriptor_R<?>)this.data;
            for(int i = 0; i < this.edit.length; i++)
                this.edit[i].setData(dsc_r.getDescriptor(i));
        }else for(final Editor element : this.edit)
            element.setData(null);
    }
}
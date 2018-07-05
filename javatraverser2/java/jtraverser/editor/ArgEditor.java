package jtraverser.editor;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridLayout;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import mds.MdsException;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;

@SuppressWarnings("serial")
public class ArgEditor extends JScrollPane{
    private final ExprEditor[] args;
    private final int          num_args;
    protected final Dimension  preferred;

    public ArgEditor(final boolean editable, final CTX ctx, final Descriptor<?>... descriptors){
        this(descriptors, editable, ctx, 9, new Dimension(220, 89));
    }

    public ArgEditor(final CTX ctx){
        this(null, true, ctx, 9, new Dimension(220, 89));
    }

    public ArgEditor(final Descriptor<?>[] data, final boolean editable, final CTX ctx, final int num_args, final Dimension preferred){
        this.preferred = preferred;
        this.num_args = num_args;
        final JPanel jp = new JPanel();
        jp.setLayout(new GridLayout(num_args, 1));
        this.args = new ExprEditor[num_args];
        for(int i = 0; i < num_args; i++)
            jp.add(this.args[i] = Editor.addLabel("Argument " + (i + 1), new ExprEditor(editable, ctx, true, false)));
        final JPanel jp2 = new JPanel();
        jp2.setLayout(new BorderLayout());
        jp2.add(jp, BorderLayout.NORTH);
        this.setViewportView(jp2);
        this.setPreferredSize(preferred);
        this.getVerticalScrollBar().setUnitIncrement(43);
        if(data != null) this.setData(data);
    }

    public final Descriptor<?> getData(final int i) throws MdsException {
        return this.args[i].getData();
    }

    public Descriptor<?>[] getDataArray() throws MdsException {
        final Descriptor<?> data[] = new Descriptor[this.num_args];
        int len = 0;
        for(int i = 0; i < this.num_args; i++)
            if((data[i] = this.args[i].getData()) != null) len = i + 1;
        final Descriptor<?> out[] = new Descriptor[len];
        System.arraycopy(data, 0, out, 0, len);
        return out;
    }

    public boolean isNull() {
        for(final ExprEditor arg : this.args)
            if(!arg.isNull()) return false;
        return true;
    }

    public final void reset(final boolean hard) {
        for(final ExprEditor arg : this.args)
            arg.reset(hard);
    }

    public final void setData(final Descriptor<?>... data) {
        int min_len = 0, i = 0;
        if(data != null){
            if(data.length < this.num_args) min_len = data.length;
            else min_len = this.num_args;
            for(; i < min_len; i++)
                this.args[i].setData(data[i]);
        }
        for(; i < this.num_args; i++)
            this.args[i].setData(null);
    }
}
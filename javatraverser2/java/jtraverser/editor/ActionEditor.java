package jtraverser.editor;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.Window;
import javax.swing.JPanel;
import jtraverser.editor.usage.DispatchEditor;
import jtraverser.editor.usage.TaskEditor;
import mds.MdsException;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Action;

@SuppressWarnings("serial")
public class ActionEditor extends Editor{
    public static final boolean checkData(final Descriptor<?> data) {
        return data instanceof Action;
    }
    JPanel action_panel, debug_panel;

    public ActionEditor(final boolean editable, final CTX ctx, final Window window){
        this(null, editable, ctx, window);
    }

    public ActionEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final Window window){
        super(data, editable, ctx, 5);
        this.setLayout(new BorderLayout());
        this.edit[0] = new DispatchEditor(this.editable, ctx, window);
        this.edit[1] = new TaskEditor(this.editable, ctx, window);
        this.edit[2] = Editor.addLabel("ErrorLogs", new ExprEditor(this.editable, ctx, true, false));
        this.edit[3] = Editor.addLabel("Completion", new ExprEditor(this.editable, ctx, true, false));
        this.edit[4] = Editor.addLabel("Performance", new ExprEditor(this.editable, ctx, false, false));
        this.action_panel = new JPanel();
        this.action_panel.setLayout(new GridLayout(1, 2));
        this.action_panel.add(this.edit[0]);
        this.action_panel.add(this.edit[1]);
        this.debug_panel = new JPanel(new GridLayout(3, 1));
        this.debug_panel.add(this.edit[2]);
        this.debug_panel.add(this.edit[3]);
        this.debug_panel.add(this.edit[4]);
        this.add(this.action_panel, BorderLayout.CENTER);
        this.add(this.debug_panel, BorderLayout.SOUTH);
    }

    @Override
    public final Descriptor<?> getData() throws MdsException {
        return new Action(this.edit[0].getData(), this.edit[1].getData(), this.edit[2].getData(), this.edit[3].getData(), this.edit[4].getData());
    }

    @Override
    public final void setData(final Descriptor<?> data) {
        this.data = data;
        this.setDescR();
    }
}
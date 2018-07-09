package jtraverser.editor.usage;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.Window;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;
import jtraverser.editor.Editor;
import jtraverser.editor.ExprEditor;
import jtraverser.editor.ParameterEditor;
import mds.MdsException;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;

@SuppressWarnings("serial")
public class TextEditor extends Editor implements ActionListener{
    protected final int               mode_idx_usr;
    protected final Window            window;
    protected final JComboBox<String> combo;
    protected final JPanel            editor;
    protected final String            name;
    private final boolean             allowparams;
    protected int                     mode_idx      = 0;
    protected int                     curr_mode_idx = 0;
    protected Editor                  data_edit;

    public TextEditor(final boolean editable, final CTX ctx, final Window window){
        this(null, editable, ctx, window);
    }

    public TextEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final Window window){
        this(data, editable, ctx, window, "Text");
    }

    protected TextEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final Window window, final String name, final boolean allowparams, final String... modes){
        super(data, editable, ctx, 0);
        this.allowparams = allowparams;
        this.name = name;
        this.window = window;
        this.editor = new JPanel(new BorderLayout());
        final DefaultComboBoxModel<String> model = new DefaultComboBoxModel<String>(new String[]{"No Data", "Expression"});
        if(allowparams) model.addElement("With Parameter");
        this.mode_idx_usr = model.getSize();
        for(final String mode : modes)
            model.addElement(mode);
        this.combo = new JComboBox<String>(model);
        final JLabel label = new JLabel(name + ":");
        label.setHorizontalAlignment(SwingConstants.CENTER);
        final JPanel jp = new JPanel(new FlowLayout(FlowLayout.CENTER, 8, 2));
        jp.add(label);
        jp.add(this.combo);
        this.setLayout(new BorderLayout());
        this.add(jp, BorderLayout.PAGE_START);
        this.add(this.editor, BorderLayout.CENTER);
        this.combo.addActionListener(this);
        if(data != null) this.setData(data);
    }

    protected TextEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final Window window, final String name, final String... modes){
        this(data, editable, ctx, window, name, true, modes);
    }

    @Override
    public void actionPerformed(final ActionEvent e) {
        final int idx = this.combo.getSelectedIndex();
        if(idx == this.curr_mode_idx) return;
        this.setMode(idx);
    }

    protected final void addEditor() {
        if(this.addExtraEditor()) return;
        if(this.curr_mode_idx == 0) return;
        if(this.allowparams && this.curr_mode_idx == 2){
            this.editor.add(this.data_edit = new ParameterEditor(this.data, this.editable, this.ctx, Editor.addLabel("Expression", new ExprEditor(this.editable, this.ctx, this.getClass() == TextEditor.class, true))), BorderLayout.CENTER);
            return;
        }
        this.curr_mode_idx = 1;
        this.editor.add(this.data_edit = Editor.addLabel("Expression", new ExprEditor(this.data, this.editable, this.ctx)), BorderLayout.CENTER);
    }

    @SuppressWarnings("static-method")
    protected boolean addExtraEditor() {
        return false;
    }

    protected final void checkData() {
        if(this.checkUsrData()) return;
        if(Editor.isNoData(this.data)) this.mode_idx = 0;
        else if(this.allowparams && ParameterEditor.hasParams(this.data)) this.mode_idx = 2;
        else this.mode_idx = 1;
    }

    @SuppressWarnings("static-method")
    protected boolean checkUsrData() {
        return false;
    }

    @Override
    public final Descriptor<?> getData() throws MdsException {
        if(this.curr_mode_idx == 0) return null;
        if(this.curr_mode_idx < this.mode_idx_usr) return this.data_edit.getData();
        return this.getUsrData();
    }

    protected Descriptor<?> getUsrData() throws MdsException {
        return this.data_edit.getData();
    }

    @Override
    public void interrupt() {
        for(final Component component : this.editor.getComponents())
            if(component instanceof Editor) ((Editor)component).interrupt();
    }

    @Override
    public final boolean isNull() {
        return this.curr_mode_idx == 0;
    }

    protected final void refresh() {
        if(!(this.window instanceof Frame)){
            this.window.invalidate();
            this.editor.invalidate();
            this.editor.repaint();
        }
        this.window.pack();
        this.window.repaint();
    }

    @Override
    public final void reset(final boolean hard) {
        super.reset(hard);
        this.combo.setSelectedIndex(this.mode_idx);
        if(hard) this.setMode(this.mode_idx);
    }

    @Override
    public final void setData(final Descriptor<?> data) {
        this.data = data;
        this.combo.setEnabled(this.editable || !Editor.isNoData(data));
        this.checkData();
        if(this.curr_mode_idx != this.mode_idx){
            this.setMode(this.mode_idx);
        }else if(this.curr_mode_idx > 0){
            if(this.curr_mode_idx < this.mode_idx_usr) this.data_edit.setData(data);
            else this.setUsrData();
        }
    }

    protected final void setMode(final int idx) {
        this.interrupt();
        this.editor.removeAll();
        this.combo.setSelectedIndex(this.curr_mode_idx = idx);
        this.addEditor();
        if(this.curr_mode_idx < this.mode_idx_usr) this.data_edit.setData(this.data);
        else this.setUsrData();
        this.refresh();
    }

    protected void setUsrData() {
        this.data_edit.setData(this.data);
    }
}
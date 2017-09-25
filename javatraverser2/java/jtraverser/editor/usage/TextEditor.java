/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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

    public TextEditor(final boolean editable, final Window window){
        this(null, editable, window);
    }

    public TextEditor(final Descriptor<?> data, final boolean editable, final Window window){
        this(data, editable, window, "Text");
    }

    protected TextEditor(final Descriptor<?> data, final boolean editable, final Window window, final String name, final boolean allowparams, final String... modes){
        super(data, editable, 0);
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

    protected TextEditor(final Descriptor<?> data, final boolean editable, final Window window, final String name, final String... modes){
        this(data, editable, window, name, true, modes);
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
            this.editor.add(this.data_edit = new ParameterEditor(this.data, this.editable, Editor.addLabel("Expression", new ExprEditor(this.getClass() == TextEditor.class, this.editable, true))), BorderLayout.CENTER);
            return;
        }
        this.curr_mode_idx = 1;
        this.editor.add(this.data_edit = Editor.addLabel("Expression", new ExprEditor(this.data, this.editable)), BorderLayout.CENTER);
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
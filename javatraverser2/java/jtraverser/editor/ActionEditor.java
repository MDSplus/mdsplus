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
package jtraverser.editor;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.Window;
import javax.swing.JPanel;
import jtraverser.editor.usage.DispatchEditor;
import jtraverser.editor.usage.TaskEditor;
import mds.MdsException;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Action;

@SuppressWarnings("serial")
public class ActionEditor extends Editor{
    public static final boolean checkData(final Descriptor<?> data) {
        return data instanceof Action;
    }
    JPanel action_panel, debug_panel;

    public ActionEditor(final boolean editable, final Window window){
        this(null, editable, window);
    }

    public ActionEditor(final Descriptor<?> data, final boolean editable, final Window window){
        super(data, editable, 5);
        this.setLayout(new BorderLayout());
        this.edit[0] = new DispatchEditor(this.editable, window);
        this.edit[1] = new TaskEditor(this.editable, window);
        this.edit[2] = Editor.addLabel("ErrorLogs", new ExprEditor(true, this.editable, false));
        this.edit[3] = Editor.addLabel("Completion", new ExprEditor(true, this.editable, false));
        this.edit[4] = Editor.addLabel("Performance", new ExprEditor(false, this.editable, false));
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
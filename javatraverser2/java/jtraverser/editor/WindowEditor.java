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

import java.awt.GridLayout;
import mds.MdsException;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Window;

@SuppressWarnings("serial")
public final class WindowEditor extends Editor{
    public static final boolean checkData(final Descriptor<?> data) {
        return data instanceof Window;
    }
    Editor startidx_edit, endidx_edit, value0_edit;

    public WindowEditor(final boolean editable){
        this(null, editable);
    }

    public WindowEditor(final Window data, final boolean editable){
        super(data, editable, 3);
        this.setLayout(new GridLayout(3, 1, 0, 0));
        this.add(this.edit[0] = Editor.addLabel("Start Idx", new ExprEditor(false, WindowEditor.this.editable, false)));
        this.add(this.edit[1] = Editor.addLabel("End Idx", new ExprEditor(false, WindowEditor.this.editable, false)));
        this.add(this.edit[2] = Editor.addLabel("Time of Zero", new ExprEditor(false, WindowEditor.this.editable, false)));
    }

    @Override
    public final Descriptor<?> getData() throws MdsException {
        return new Window(this.startidx_edit.getData(), this.endidx_edit.getData(), this.value0_edit.getData());
    }

    @Override
    public final void setData(final Descriptor<?> data) {
        this.data = data;
        this.setDescR();
    }
}

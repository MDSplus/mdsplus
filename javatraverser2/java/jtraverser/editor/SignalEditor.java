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
import jtraverser.editor.usage.NumericEditor;
import mds.MdsException;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Signal;

@SuppressWarnings("serial")
public class SignalEditor extends Editor{
    public static final boolean checkData(final Descriptor<?> data) {
        return data instanceof Signal;
    }

    public SignalEditor(final boolean editable, final Window window){
        this(null, editable, window);
    }

    public SignalEditor(final Descriptor<?> data, final boolean editable, final Window window){
        super(data, editable, 3);
        this.setLayout(new BorderLayout());
        final JPanel columns = new JPanel(new GridLayout(1, 3));
        columns.add(this.edit[0] = new NumericEditor(this.editable, window, "Data"));
        columns.add(this.edit[1] = new NumericEditor(this.editable, window, "Raw"));
        columns.add(this.edit[2] = new NumericEditor(this.editable, window, "Dimension"));
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
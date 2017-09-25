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
import javax.swing.JPanel;
import mds.MdsException;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;
import mds.data.descriptor_r.Routine;

@SuppressWarnings("serial")
public final class RoutineEditor extends Editor{
    public static final boolean checkData(final Descriptor<?> data) {
        return data instanceof Routine;
    }
    ArgEditor arg_edit;

    public RoutineEditor(){
        this(null, true);
    }

    public RoutineEditor(final boolean editable){
        this(null, editable);
    }

    public RoutineEditor(final Routine data, final boolean editable){
        super(data, editable, 3);
        final JPanel jp = new JPanel();
        jp.setLayout(new GridLayout(3, 1, 0, 0));
        jp.add(this.edit[0] = Editor.addLabel("Timeout", new ExprEditor(false, editable, false)));
        jp.add(this.edit[1] = Editor.addLabel("Image", new ExprEditor(true, editable, false)));
        jp.add(this.edit[2] = Editor.addLabel("Routine", new ExprEditor(true, editable, false)));
        this.setLayout(new BorderLayout());
        this.add(jp, BorderLayout.NORTH);
        this.add(this.arg_edit = new ArgEditor(editable), BorderLayout.CENTER);
    }

    @Override
    public final Routine getData() throws MdsException {
        return new Routine(this.edit[0].getData(), this.edit[1].getData(), this.edit[2].getData(), this.arg_edit.getDataArray());
    }

    @Override
    public final void setData(final Descriptor<?> data) {
        this.data = data;
        if(this.data instanceof Descriptor_R<?>){
            this.setDescR();
            this.arg_edit.setData(((Descriptor_R<?>)data).getDescriptors(3, -1));
        }else{
            for(final Editor element : this.edit)
                element.setData(null);
            this.arg_edit.setData();
        }
    }
}
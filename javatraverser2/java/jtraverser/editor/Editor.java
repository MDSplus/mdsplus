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

import java.awt.Component;
import javax.swing.BorderFactory;
import javax.swing.JComponent;
import javax.swing.JPanel;
import mds.MdsException;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

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

    protected Editor(final Descriptor<?> data, final boolean editable, final int nargs){
        this.data = data;
        this.editable = editable;
        this.edit = new Editor[nargs];
    }

    public abstract Descriptor<?> getData() throws MdsException;

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
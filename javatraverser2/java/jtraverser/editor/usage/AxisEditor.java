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

import java.awt.Window;
import jtraverser.editor.ParameterEditor;
import jtraverser.editor.RangeEditor;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.PARAMETER;
import mds.data.descriptor_r.Range;

@SuppressWarnings("serial")
public class AxisEditor extends TextEditor{
    public AxisEditor(final boolean editable, final Window window){
        this(null, editable, window);
    }

    public AxisEditor(final Descriptor<?> data, final boolean editable, final Window window){
        super(data, editable, window, "Range", "Range", "Range W/ Param");
    }

    @Override
    protected final boolean addExtraEditor() {
        if(this.curr_mode_idx < this.mode_idx_usr) return false;
        if(this.curr_mode_idx == this.mode_idx_usr) this.editor.add(this.data_edit = new RangeEditor(this.editable));
        else this.editor.add(this.data_edit = new ParameterEditor(this.data, this.editable, new RangeEditor(this.editable)));
        return true;
    }

    @Override
    public final boolean checkUsrData() {
        Descriptor<?> tdata = this.data;
        while(ParameterEditor.hasParams(tdata))
            tdata = ((PARAMETER)tdata).getValue();
        final boolean noparam = tdata == this.data;
        if(!(tdata instanceof Range)) return false;
        this.mode_idx = noparam ? this.mode_idx_usr : this.mode_idx_usr + 1;
        return true;
    }
}

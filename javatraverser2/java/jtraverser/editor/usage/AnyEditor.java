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
import mds.data.descriptor.Descriptor;

@SuppressWarnings("serial")
public final class AnyEditor extends TextEditor{
    public AnyEditor(final boolean editable, final Window window){
        this(null, editable, window);
    }

    public AnyEditor(final Descriptor<?> data, final boolean editable, final Window window){
        super(data, editable, window, "Data", "Action", "Python Expression", "Signal", "Window", "Method", "Routine", "Program (depreciated)", "Procedure (depreciated)");
    }

    @Override
    protected final boolean addExtraEditor() {
        if(this.curr_mode_idx < this.mode_idx_usr) return false;
        switch(this.curr_mode_idx - this.mode_idx_usr){
            case 0:
                this.editor.add(this.data_edit = new jtraverser.editor.ActionEditor(this.editable, this.window));
                return true;
            case 1:
                this.editor.add(this.data_edit = new jtraverser.editor.PythonEditor(this.editable));
                return true;
            case 2:
                this.editor.add(this.data_edit = new jtraverser.editor.SignalEditor(this.data, this.editable, this.window));
                return true;
            case 3:
                this.editor.add(this.data_edit = new jtraverser.editor.WindowEditor(this.editable));
                return true;
            case 4:
                this.editor.add(this.data_edit = new jtraverser.editor.MethodEditor(this.editable));
                return true;
            case 5:
                this.editor.add(this.data_edit = new jtraverser.editor.RoutineEditor(this.editable));
                return true;
            case 6:
                this.editor.add(this.data_edit = new jtraverser.editor.ProgramEditor(this.editable));
                return true;
            case 7:
                this.editor.add(this.data_edit = new jtraverser.editor.ProcedureEditor(this.editable));
                return true;
            default:
                return false;
        }
    }

    @Override
    protected final boolean checkUsrData() {
        final int usrmde = this.getUsrMode();
        if(usrmde < 0) return false;
        this.mode_idx = this.mode_idx_usr + usrmde;
        return true;
    }

    private final int getUsrMode() {
        if(jtraverser.editor.ActionEditor.checkData(this.data)) return 0;
        if(jtraverser.editor.PythonEditor.checkData(this.data)) return 1;
        if(jtraverser.editor.SignalEditor.checkData(this.data)) return 2;
        if(jtraverser.editor.WindowEditor.checkData(this.data)) return 3;
        if(jtraverser.editor.MethodEditor.checkData(this.data)) return 4;
        if(jtraverser.editor.RoutineEditor.checkData(this.data)) return 5;
        if(jtraverser.editor.ProgramEditor.checkData(this.data)) return 6;
        if(jtraverser.editor.ProcedureEditor.checkData(this.data)) return 7;
        return -1;
    }
}
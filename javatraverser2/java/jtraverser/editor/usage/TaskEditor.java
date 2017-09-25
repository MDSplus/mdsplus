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
import jtraverser.editor.MethodEditor;
import jtraverser.editor.ProcedureEditor;
import jtraverser.editor.ProgramEditor;
import jtraverser.editor.RoutineEditor;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Method;
import mds.data.descriptor_r.Procedure;
import mds.data.descriptor_r.Program;
import mds.data.descriptor_r.Routine;

@SuppressWarnings({"deprecation", "serial"})
public class TaskEditor extends TextEditor{
    public TaskEditor(final boolean editable, final Window window){
        this(null, editable, window);
    }

    public TaskEditor(final Descriptor<?> data, final boolean editable, final Window window){
        super(data, editable, window, "Task", false, "Method", "Routine", "Procedure (depreciated)", "Program (depreciated)");
    }

    @Override
    protected boolean addExtraEditor() {
        switch(this.curr_mode_idx - this.mode_idx_usr){
            case 0:
                if(this.mode_idx == this.curr_mode_idx) this.editor.add(this.data_edit = new MethodEditor((Method)this.data, this.editable));
                else this.editor.add(this.data_edit = new MethodEditor(null, this.editable));
                return true;
            case 1:
                if(this.mode_idx == this.curr_mode_idx) this.editor.add(this.data_edit = new RoutineEditor((Routine)this.data, this.editable));
                else this.editor.add(this.data_edit = new RoutineEditor(null, this.editable));
                return true;
            case 2:
                if(this.mode_idx == this.curr_mode_idx) this.editor.add(this.data_edit = new ProcedureEditor((Procedure)this.data, this.editable));
                else this.editor.add(this.data_edit = new ProcedureEditor(null, this.editable));
                return true;
            case 3:
                if(this.mode_idx == this.curr_mode_idx) this.editor.add(this.data_edit = new ProgramEditor((Program)this.data, this.editable));
                else this.editor.add(this.data_edit = new ProgramEditor(null, this.editable));
                return true;
            default:
                return false;
        }
    }

    @Override
    protected boolean checkUsrData() {
        if(this.data instanceof Method) this.mode_idx = this.mode_idx_usr;
        else if(this.data instanceof Routine) this.mode_idx = this.mode_idx_usr + 1;
        else if(this.data instanceof Procedure) this.mode_idx = this.mode_idx_usr + 2;
        else if(this.data instanceof Program) this.mode_idx = this.mode_idx_usr + 3;
        else return false;
        return true;
    }
}
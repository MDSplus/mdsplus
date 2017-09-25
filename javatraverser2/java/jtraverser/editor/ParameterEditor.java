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
import mds.data.descriptor_r.Param;
import mds.data.descriptor_r.With_Units;

@SuppressWarnings("serial")
public class ParameterEditor extends Editor{
    public static final boolean hasParams(final Descriptor<?> data) {
        return((data instanceof With_Units) || data instanceof Param);
    }
    private final ExprEditor help, validation, units;
    private final Editor     value;

    public ParameterEditor(final Descriptor<?> data, final boolean editable){
        this(data, editable, new ExprEditor(false, editable, true));
    }

    public ParameterEditor(final Descriptor<?> data, final boolean editable, final Editor value){
        super(data, editable, 0);
        this.value = value;
        this.units = new ExprEditor(true, editable, false);
        this.help = new ExprEditor(true, editable, false);
        this.validation = new ExprEditor(false, editable, false);
        this.setLayout(new BorderLayout());
        this.add(Editor.addLabel("Value", this.value), BorderLayout.CENTER);
        final JPanel jp = new JPanel(new GridLayout(3, 1));
        jp.add(Editor.addLabel("Units", this.units));
        jp.add(Editor.addLabel("Help", this.help));
        jp.add(Editor.addLabel("Validation", this.validation));
        this.add(jp, BorderLayout.PAGE_END);
        if(!Editor.isNoData(this.data)) this.setData(data);
    }

    @Override
    public final Descriptor<?> getData() throws MdsException {
        final Descriptor<?> valueout, unitout, helpout, validout;
        unitout = this.units.getData();
        helpout = this.help.getData();
        validout = this.validation.getData();
        if(Editor.isNoData(unitout)) valueout = this.value.getData();
        else valueout = new With_Units(this.value.getData(), unitout);
        if(Editor.isNoData(helpout) && Editor.isNoData(validout)) return valueout;
        return new Param(valueout, helpout, validout);
    }

    @Override
    public final void setData(final Descriptor<?> data) {
        if(data instanceof Param){
            final Param param = (Param)data;
            this.help.setData(param.getHelp());
            this.validation.setData(param.getValidation());
            final Descriptor<?> pvalue = param.getValue();
            if(pvalue instanceof With_Units){
                final With_Units with_units = (With_Units)pvalue;
                this.value.setData(with_units.getValue());
                this.units.setData(with_units.getUnits());
            }else this.value.setData(pvalue);
        }else if(data instanceof With_Units){
            final With_Units with_units = (With_Units)data;
            this.units.setData(with_units.getUnits());
            final Descriptor<?> uvalue = with_units.getValue();
            if(uvalue instanceof Param){
                final Param param = (Param)uvalue;
                this.value.setData(param.getValue());
                this.help.setData(param.getHelp());
                this.validation.setData(param.getValidation());
            }else this.value.setData(uvalue);
        }else this.value.setData(data);
    }
}

package jtraverser.editor;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import javax.swing.JPanel;
import mds.MdsException;
import mds.data.CTX;
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

    public ParameterEditor(final Descriptor<?> data, final boolean editable, final CTX ctx){
        this(data, editable, ctx, new ExprEditor(editable, ctx, false, true));
    }

    public ParameterEditor(final Descriptor<?> data, final boolean editable, final CTX ctx, final Editor value){
        super(data, editable, ctx, 0);
        this.value = value;
        this.units = new ExprEditor(editable, ctx, true, false);
        this.help = new ExprEditor(editable, ctx, true, false);
        this.validation = new ExprEditor(editable, ctx, false, false);
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

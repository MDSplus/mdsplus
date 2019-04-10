//package jTraverser;
import javax.swing.*;
import java.awt.*;

public class ProcedureEditor extends JPanel implements Editor
{
    ProcedureData procedure;
    LabeledExprEditor procedure_edit, language_edit, timeout_edit;
    ArgEditor arg_edit;

    public ProcedureEditor() {this(null);}
    public ProcedureEditor(ProcedureData procedure)
    {
	this.procedure = procedure;
	if(this.procedure == null)
	{
	    this.procedure = new ProcedureData(null, null, null, new Data[0]);
	}
	setLayout(new BorderLayout());
	JPanel jp = new JPanel();
	GridLayout gl = new GridLayout(2,1);
	gl.setVgap(0);
	jp.setLayout(gl);
	procedure_edit = new LabeledExprEditor(	"Procedure", new ExprEditor(
	    this.procedure.getProcedure(), true));
	language_edit = new LabeledExprEditor(	"Language", new ExprEditor(
	    this.procedure.getLanguage(), true));
	jp.add(procedure_edit);
	jp.add(language_edit);
	add(jp, "North");
	arg_edit = new ArgEditor(this.procedure.getArguments());
	add(arg_edit, "Center");
	timeout_edit = new LabeledExprEditor("Timeout", new ExprEditor(
	    this.procedure.getTimeout(), false));
	add(timeout_edit, "South");
    }

    public void reset()
    {
	procedure_edit.reset();
	language_edit.reset();
	arg_edit.reset();
	timeout_edit.reset();
    }

    public Data getData()
    {
	return new ProcedureData(timeout_edit.getData(), language_edit.getData(),
	                     procedure_edit.getData(),arg_edit.getData());
    }

    public void setData(Data data)
    {
	this.procedure = (ProcedureData)data;
	if(this.procedure == null)
	{
	    this.procedure = new ProcedureData(null, null, null, new Data[0]);
	}
	reset();
    }

    public void setEditable(boolean editable)
    {
	if(procedure_edit != null) procedure_edit.setEditable(editable);
	if(language_edit != null) language_edit.setEditable(editable);
	if(timeout_edit != null) timeout_edit.setEditable(editable);
	if(arg_edit != null) arg_edit.setEditable(editable);
    }

}
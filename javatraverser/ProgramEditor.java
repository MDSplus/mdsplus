//package jTraverser;
import javax.swing.*;
import java.awt.*;

public class ProgramEditor extends JPanel implements Editor
{
    ProgramData program;
    LabeledExprEditor program_edit, timeout_edit;

    public ProgramEditor() {this(null);}
    public ProgramEditor(ProgramData program)
    {
	this.program = program;
	if(this.program == null)
	{
	    this.program = new ProgramData(null, null);
	}
	program_edit = new LabeledExprEditor("Program", new ExprEditor(
	    this.program.getProgram(), true));
	timeout_edit = new LabeledExprEditor("Timeout", new ExprEditor(
	    this.program.getTimeout(), false));
    JPanel jp =new JPanel();
	jp.setLayout(new GridLayout(2,1));
	jp.add(program_edit);
	jp.add(timeout_edit);
	setLayout(new BorderLayout());
    add(jp, BorderLayout.NORTH);
    }

    public void reset()
    {
	program_edit.reset();
	timeout_edit.reset();
    }

    public Data getData()
    {
	return new ProgramData(timeout_edit.getData(), program_edit.getData());
    }

    public void setData(Data data)
    {
	this.program = (ProgramData)data;
	if(this.program == null)
	{
	    this.program = new ProgramData(null, null);
	}
	reset();
    }

    public void setEditable(boolean editable)
    {
	if(program_edit != null) program_edit.setEditable(editable);
	if(timeout_edit != null) timeout_edit.setEditable(editable);
    }

}
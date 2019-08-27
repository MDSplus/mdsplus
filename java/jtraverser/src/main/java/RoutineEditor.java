//package jTraverser;
import javax.swing.*;
import java.awt.*;

public class RoutineEditor extends JPanel implements Editor
{
    RoutineData routine;
    LabeledExprEditor image_edit, routine_edit, timeout_edit;
    ArgEditor arg_edit;

    public RoutineEditor() {this(null);}
    public RoutineEditor(RoutineData routine)
    {
	this.routine = routine;
	if(this.routine == null)
	{
	    this.routine = new RoutineData(null, null, null, new Data[0]);
	}
	setLayout(new BorderLayout());
	JPanel jp = new JPanel();
	GridLayout gl = new GridLayout(2,1);
	gl.setVgap(0);
	jp.setLayout(gl);
	image_edit = new LabeledExprEditor("Image", new ExprEditor(
	    this.routine.getImage(), true));
	routine_edit = new LabeledExprEditor("Routine", new ExprEditor(
	    this.routine.getRoutine(), true));
	jp.add(image_edit);
	jp.add(routine_edit);
	add(jp, "North");
	arg_edit = new ArgEditor(this.routine.getArguments());
	add(arg_edit, "Center");
	timeout_edit = new LabeledExprEditor("Timeout", new ExprEditor(
	    this.routine.getTimeout(), false));
	add(timeout_edit, "South");
    }

    public void reset()
    {
	image_edit.reset();
	routine_edit.reset();
	arg_edit.reset();
	timeout_edit.reset();
    }

    public Data getData()
    {
	Data data1 = timeout_edit.getData();
	data1 = image_edit.getData();
	data1 = routine_edit.getData();
	Data data[] = arg_edit.getData();

	return new RoutineData(timeout_edit.getData(), image_edit.getData(),
	    routine_edit.getData(), arg_edit.getData());
    }

    public void setData(Data data)
    {
	this.routine = (RoutineData)data;
	if(this.routine == null)
	{
	    this.routine = new RoutineData(null, null, null, new Data[0]);
	}
	reset();
    }
    public void setEditable(boolean editable)
    {
	if(image_edit != null) image_edit.setEditable(editable);
	if(routine_edit != null) routine_edit.setEditable(editable);
	if(timeout_edit != null) timeout_edit.setEditable(editable);
	if(arg_edit != null) arg_edit.setEditable(editable);
    }


}
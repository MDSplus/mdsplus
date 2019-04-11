//package jTraverser;
import javax.swing.*;
import java.awt.*;

public class RangeEditor extends JPanel implements Editor
{
    RangeData range;
    LabeledExprEditor begin_edit, end_edit, delta_edit;

    public RangeEditor() {this(null);}
    public RangeEditor(RangeData range)
    {
	this.range = range;
	if(this.range == null)
	{
	    this.range = new RangeData(null, null, null);
	}
	GridLayout gl = new GridLayout(3, 1);
	gl.setVgap(0);
	setLayout(gl);
	begin_edit = new LabeledExprEditor( "Start", new ExprEditor(
	    this.range.getBegin(), false));
	add(begin_edit);
	end_edit = new LabeledExprEditor( "End", new ExprEditor(
	    this.range.getEnd(), false));
	add(end_edit);
	delta_edit = new LabeledExprEditor( "Increment", new ExprEditor(
	    this.range.getDelta(), false));
	add(delta_edit);
    }

    public void reset()
    {
	begin_edit.reset();
	end_edit.reset();
	delta_edit.reset();
    }

    public Data getData()
    {
	return new RangeData(begin_edit.getData(), end_edit.getData(), delta_edit.getData());
    }

    public void setData(Data data)
    {
	this.range = (RangeData)data;
	if(this.range == null)
	{
	    this.range = new RangeData(null, null, null);
	}
	begin_edit.setData(range.getBegin());
	end_edit.setData(range.getEnd());
	delta_edit.setData(range.getDelta());
	reset();
    }

    public void setEditable(boolean editable)
    {
	if(begin_edit != null) begin_edit.setEditable(editable);
	if(end_edit != null) end_edit.setEditable(editable);
	if(delta_edit != null) delta_edit.setEditable(editable);
    }

}
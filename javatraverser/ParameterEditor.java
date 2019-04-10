//package jTraverser;
import javax.swing.*;
import java.awt.*;

public class ParameterEditor extends JPanel implements Editor
{
    ExprEditor expr, help, validity;
    public ParameterEditor()
    {
	this(new ExprEditor(null, false), new ExprEditor(null, true), new ExprEditor(null, false));
    }
    public ParameterEditor(ExprEditor expr, ExprEditor help, ExprEditor validity)
    {
	this.expr = expr;
	this.help = help;
	this.validity = validity;
	setLayout(new BorderLayout());
	add(new LabeledExprEditor("Data", expr), "North");
	add(new LabeledExprEditor("Help", help), "Center");
	add(new LabeledExprEditor("Validity", validity), "South");
    }

    public void reset()
    {
      expr.reset();
      help.reset();
      validity.reset();
    }
    public void setEditable(boolean editable)
    {
      expr.setEditable(editable);
      help.setEditable(editable);
      validity.setEditable(editable);
    }

    public Data getData() {return new ParameterData(expr.getData(), help.getData(), validity.getData());}
    public void setData(Data data)
    {
      if (data instanceof ParameterData) {
	expr.setData( ( (ParameterData) data).getDatum());
	help.setData( ( (ParameterData) data).getHelp());
	validity.setData( ( (ParameterData) data).getValidation());
      }
    }
}

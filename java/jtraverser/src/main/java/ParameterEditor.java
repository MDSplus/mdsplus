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

    public MDSplus.Data getData() 
    {
        MDSplus.Data retData = expr.getData();
        retData.setHelp(help.getData());
        retData.setValidation(validity.getData());
        return retData;
    }
    public void setData(MDSplus.Data data)
    {
	expr.setData(data);
	help.setData(data.getHelp());
	validity.setData(data.getValidation());
    }
}

//package jTraverser;
import javax.swing.*;
import java.awt.*;

public class LabeledExprEditor extends JPanel implements Editor
{
    ExprEditor expr;
    public LabeledExprEditor(String label_str)
    {
	this(label_str, new ExprEditor(null, false));
    }
    public LabeledExprEditor(String label_str, ExprEditor expr)
    {
	this.expr = expr;
	BorderLayout bl = new BorderLayout();
	bl.setVgap(0);
	setLayout(bl);
	add(new JLabel(label_str), "West");
	add(expr, "Center");
    }

    public void reset()
    {
	expr.reset();
    }
    public void setEditable(boolean editable)
    {
	expr.setEditable(editable);
    }
    
    public Data getData() {return expr.getData();}
    public void setData(Data data) {expr.setData(data);}
    
}
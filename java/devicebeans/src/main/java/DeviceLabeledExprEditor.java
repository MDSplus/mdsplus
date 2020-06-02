//package jTraverser;
import javax.swing.*;
import java.awt.*;

public class DeviceLabeledExprEditor extends JPanel
{
    DeviceExprEditor expr;
/*    public LabeledExprEditor(String data)
    {
    this("Expression", new ExprEditor(data, (data != null && data.dtype == Data.DTYPE_T), 4, 20));
    }
    public LabeledExprEditor(String label_str)
    {
	this(label_str, new ExprEditor(null, false));
    }*/
    public DeviceLabeledExprEditor(String label_str, DeviceExprEditor expr)
    {
	this.expr = expr;
	setLayout(new BorderLayout());
    setBorder(BorderFactory.createTitledBorder(label_str));
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

    public String getData() {return expr.getData();}
    public void setData(String data) {expr.setData(data);}

}
//package jTraverser;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class DisplayNci extends NodeEditor implements ActionListener
{
    JLabel label1, label2, label3, label4;
    public DisplayNci()
    {
	setLayout(new BorderLayout());
	JPanel jp = new JPanel();
	jp.setLayout(new GridLayout(4,1));
	jp.add(label1 = new JLabel("prova1"));
	jp.add(label2 = new JLabel("prova2"));
	jp.add(label3 = new JLabel("prova3"));
	jp.add(label4 = new JLabel("prova4"));
	add(jp, "North");
	JPanel jp1 = new JPanel();
	JButton cancel = new JButton("Cancel");
	jp1.add(cancel);
	add(jp1, "South");
	cancel.addActionListener(this);
    }
    public void actionPerformed(ActionEvent e) {
		frame.dispose();
    }


    public void setNode(Node _node)
    {	
	NodeInfo info;
	node = _node;
	frame.setTitle("Display Nci information");
	try{
	    info = node.getInfo();
	}catch(Exception e){System.out.println("Error retieving Nci"); return; }
	label1.setText(info.name);
	StringBuffer sb = new StringBuffer("Status: ");
	if(info.on)
	    sb.append("on, ");
	else
	    sb.append("off, ");
	if(info.parent_on)
	    sb.append("parent is on");
	else
	    sb.append("parent is off");
	if(info.setup)
	    sb.append(", setup");
	if(info.write_once)
	    sb.append(", write once");
	if(info.compressible)
	    sb.append(", compressible");
	if(info.compress_on_put)
	    sb.append(", compress on put");
	if(info.no_write_model)
	    sb.append(", no write model");
	if(info.no_write_shot)
	    sb.append(", no write shot");
	label2.setText(sb.toString());
	label3.setText("Data inserted: "+ info.date_inserted); 
	
	String dtype = "", dclass = "";
	if(info.length == 0)
	    label4.setText("There is no data stored for this node");
	else
	{
	    switch(info.dtype) {
		case  Data.DTYPE_BU: dtype = "DTYPE_BU"; break;
		case Data.DTYPE_WU: dtype = "DTYPE_WU"; break;
		case Data.DTYPE_LU: dtype = "DTYPE_LU"; break;
		case Data.DTYPE_QU: dtype = "DTYPE_QU"; break;
		case Data.DTYPE_OU: dtype = "DTYPE_OU"; break;
		case Data.DTYPE_B: dtype = "DTYPE_B"; break;
		case Data.DTYPE_W: dtype = "DTYPE_W"; break;
		case Data.DTYPE_L: dtype = "DTYPE_L"; break;
		case Data.DTYPE_Q: dtype = "DTYPE_Q"; break;
		case Data.DTYPE_O: dtype = "DTYPE_O"; break;
		case Data.DTYPE_FLOAT: dtype = "DTYPE_FLOAT"; break;
		case Data.DTYPE_DOUBLE: dtype = "DTYPE_DOUBLE"; break;
		case Data.DTYPE_T: dtype = "DTYPE_T"; break;
		case Data.DTYPE_IDENT: dtype = "DTYPE_IDENT"; break;
		case Data.DTYPE_NID: dtype = "DTYPE_NID"; break;
		case Data.DTYPE_PATH: dtype = "DTYPE_PATH"; break;
		case Data.DTYPE_PARAM: dtype = "DTYPE_PARAM"; break;
		case Data.DTYPE_SIGNAL: dtype = "DTYPE_SIGNAL"; break;
		case Data.DTYPE_DIMENSION: dtype = "DTYPE_DIMENSION"; break;
		case Data.DTYPE_WINDOW: dtype = "DTYPE_WINDOW"; break;
		case Data.DTYPE_FUNCTION: dtype = "DTYPE_FUNCTION"; break;
		case Data.DTYPE_CONGLOM: dtype = "DTYPE_CONGLOM"; break;
		case Data.DTYPE_RANGE: dtype = "DTYPE_RANGE"; break;
		case Data.DTYPE_ACTION: dtype = "DTYPE_ACTION"; break;
		case Data.DTYPE_DISPATCH: dtype = "DTYPE_DISPATCH"; break;
		case Data.DTYPE_PROGRAM: dtype = "DTYPE_PROGRAM"; break;
		case Data.DTYPE_ROUTINE: dtype = "DTYPE_ROUTINE"; break;
		case Data.DTYPE_PROCEDURE: dtype = "DTYPE_PROCEDURE"; break;
		case Data.DTYPE_METHOD: dtype = "DTYPE_METHOD"; break;
		case Data.DTYPE_DEPENDENCY: dtype = "DTYPE_DEPENDENCY"; break;
		case Data.DTYPE_CONDITION: dtype = "DTYPE_CONDITION"; break;
		case Data.DTYPE_EVENT: dtype = "DTYPE_EVENT"; break;
		case Data.DTYPE_WITH_UNITS: dtype = "DTYPE_WITH_UNITS"; break;
		case Data.DTYPE_CALL: dtype = "DTYPE_CALL"; break;
		case Data.DTYPE_WITH_ERROR: dtype = "DTYPE_WITH_ERROR"; break;
	    }
	    switch(info.dclass) {
		case Data.CLASS_S: dclass = "CLASS_S"; break;
		case Data.CLASS_D: dclass = "CLASS_D"; break;
		case Data.CLASS_A: dclass = "CLASS_A"; break;
		case Data.CLASS_R: dclass = "CLASS_R"; break;
		case Data.CLASS_APD: dclass = "CLASS_APD"; break;
	    }
	    label4.setText("Dtype: "+dtype+"  Class: " + dclass + "  Length: "+info.length + " bytes");
	}
	
	
	
}

}
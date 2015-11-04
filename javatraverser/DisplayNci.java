//package jTraverser;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class DisplayNci extends NodeEditor implements ActionListener
{
    JLabel label0, label1, label2, label3;
    public DisplayNci()
    {
	    setLayout(new BorderLayout());
	    JPanel jp, jpsub;
        add(jp = new JPanel(), "North");
	    jp.setLayout(new GridBagLayout());
        GridBagConstraints c = new GridBagConstraints();
        c.fill = GridBagConstraints.EAST;
        c.gridx=0;
        c.gridy=0;
	    jp.add(label0 = new JLabel(""), c);
        c.gridy=1;
	    jp.add(label1 = new JLabel(""), c);
        c.gridy=2;
	    jp.add(label2 = new JLabel(""), c);
        c.gridy=3;
	    jp.add(label3 = new JLabel(""), c);
	    add(jp = new JPanel(), "South");
	    JButton close;
	    jp.add(close = new JButton("Close"));
	    close.addActionListener(this);
    }
    public void actionPerformed(ActionEvent e) {
		frame.dispose();
    }

    public void setNode(Node node)
    {	
	    this.node = node;
	    frame.setTitle("Display Nci information");
	    try{
	        node.getInfo();
	    }catch(Exception e){System.out.println("Error retieving Nci"); return; }
	    label0.setText(node.getFullPath());
	    StringBuffer sb = new StringBuffer("<html><table cols=1 width=300><td><nobr>Status: ");
        final String sep = "</nobr>, <nobr>";
	    if(node.isOn())
	        sb.append("on");
	    else
	        sb.append("off");
        sb.append(sep+"parent is ");
	    if(node.isParentState())
	        sb.append("off");
	    else
	        sb.append("on");
	    if(node.isSetup())
	        sb.append(sep+"setup");
	    if(node.isEssential())
	        sb.append(sep+"essential");
	    if(node.isCached())
	        sb.append(sep+"cached");
	    if(node.isVersion())
	        sb.append(sep+"version");
	    if(node.isSegmented())
	        sb.append(sep+"segmented");
	    if(node.isWriteOnce())
	        sb.append(sep+"write once");
	    if(node.isCompressible())
	        sb.append(sep+"compressible");
	    if(node.isDoNotCompress())
	        sb.append(sep+"do not compress");
	    if(node.isCompressOnPut())
	        sb.append(sep+"compress on put");
	    if(node.isNoWriteModel())
	        sb.append(sep+"no write model");
	    if(node.isNoWriteShot())
	        sb.append(sep+"no write shot");
	    if(node.isPathReference())
	        sb.append(sep+"path reference");
	    if(node.isNidReference())
	        sb.append(sep+"nid reference");
	    if(node.isCompressSegments())
	        sb.append(sep+"compress segments");
	    if(node.isIncludeInPulse())
	        sb.append(sep+"include in pulse");
        sb.append("</nobr></td></table></html>");
	    label1.setText(sb.toString());
	    label2.setText("Data inserted: "+ node.getDate()); 
	    String dtype, dclass;
	    if(node.getLength() == 0)
	        label3.setText("There is no data stored for this node");
	    else
	    {
	        switch(node.getDType()) {
		        case Data.DTYPE_BU: dtype = "DTYPE_BU"; break;
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
		        case Data.DTYPE_FSC: dtype = "DTYPE_FSC"; break;
		        case Data.DTYPE_FTC: dtype = "DTYPE_FTC"; break;
		        case Data.DTYPE_F: dtype = "DTYPE_F"; break;
		        case Data.DTYPE_D: dtype = "DTYPE_D"; break;
		        case Data.DTYPE_G: dtype = "DTYPE_G"; break;
		        case Data.DTYPE_H: dtype = "DTYPE_H"; break;
		        case Data.DTYPE_T: dtype = "DTYPE_T"; break;
		        case Data.DTYPE_POINTER: dtype = "DTYPE_POINTER"; break;
		        case Data.DTYPE_DSC: dtype = "DTYPE_DSC"; break;
		        case Data.DTYPE_IDENT: dtype = "DTYPE_IDENT"; break;
		        case Data.DTYPE_NID: dtype = "DTYPE_NID"; break;
		        case Data.DTYPE_PATH: dtype = "DTYPE_PATH"; break;
		        case Data.DTYPE_PARAM: dtype = "DTYPE_PARAM"; break;
		        case Data.DTYPE_SIGNAL: dtype = "DTYPE_SIGNAL"; break;
		        case Data.DTYPE_DIMENSION: dtype = "DTYPE_DIMENSION"; break;
		        case Data.DTYPE_WINDOW: dtype = "DTYPE_WINDOW"; break;
		        case Data.DTYPE_SLOPE: dtype = "DTYPE_SLOPE"; break;
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
		        case Data.DTYPE_LIST: dtype = "DTYPE_LIST"; break;
		        case Data.DTYPE_TUPLE: dtype = "DTYPE_TUPLE"; break;
		        case Data.DTYPE_DICTIONARY: dtype = "DTYPE_DICTIONARY"; break;
                default: dtype = "DTYPE" + ((int)node.getDType() & 0xFF); break;
	        }
	        switch(node.getDClass()) {
		        case Data.CLASS_S: dclass = "CLASS_S"; break;
		        case Data.CLASS_D: dclass = "CLASS_D"; break;
		        case Data.CLASS_A: dclass = "CLASS_A"; break;
		        case Data.CLASS_R: dclass = "CLASS_R"; break;
		        case Data.CLASS_APD: dclass = "CLASS_APD"; break;
                default: dclass = "CLASS" + ((int)node.getDClass() & 0xFF); break;
	        }
	        label3.setText(dtype+"   "+dclass+"   Length: "+node.getLength()+" bytes");
	    }
    }
}
//package jTraverser;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class DisplayNci extends NodeEditor implements ActionListener
{
    JLabel label;
    public DisplayNci()
    {
	    setLayout(new BorderLayout());
	    JPanel jp;
	add(jp = new JPanel(), "North");
	    jp.setLayout(new GridBagLayout());
	GridBagConstraints c = new GridBagConstraints();
	c.fill = GridBagConstraints.EAST;
	c.gridx=0;
	c.gridy=0;
	    jp.add(label = new JLabel(""), c);
	    add(jp = new JPanel(), "South");
	c.gridy=1;
	    JButton close;
	    jp.add(close = new JButton("Close"), c);
	    close.addActionListener(this);
    }
    public void actionPerformed(ActionEvent e) {
		frame.dispose();
    }

    public void setNode(Node node)
    {
	    this.node = node;
	    frame.setTitle("Display Nci information");
	    try{node.getInfo();}
	catch (Exception exc){jTraverser.stderr("Error retieving Nci", exc);}
	    StringBuffer sb = new StringBuffer("<html><table width=\"320\"> <tr><td width=\"60\" align=\"left\"/><nobr>full path:</nobr></td><td align=\"left\">");
	sb.append(node.getFullPath());
	sb.append("</td></tr><tr><td align=\"left\" valign=\"top\">Status:</td><td align=\"left\"><nobr>");
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
	sb.append("</nobr></td></tr><tr><td align=\"left\">Data:</td><td align=\"left\">");
	    String dtype, dclass;
	    if(node.getLength() == 0)
		sb.append("There is no data stored for this node");
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
	    sb.append("<nobr>"+dtype+"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"+dclass+"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"+node.getLength()+" Bytes</nobr>");
	    sb.append("</td></tr><tr><td align=\"left\">Inserted:</td><td align=\"left\">");
	    sb.append(node.getDate());
	    sb.append("</td></tr></table></html>");
	    }
	    label.setText(sb.toString());
    }
}
import java.util.Vector;

import javax.swing.JOptionPane;
import javax.swing.JPanel;

import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Nid;
public class  DeviceParameters extends DeviceMultiComponent
{

    protected Vector parameters = new Vector();
    protected String getParameterName() {return "PAR";}
    protected String getComponentNameAt(Nid nidData, int idx)
    {
	String parName;
	int olddef;
	String paramName = getParameterName();
	parName = String.format("%s_03d", paramName, idx+1);
	try {
		olddef = subtree.getDefault();
	    subtree.setDefault(nidData.getNidNumber());
	    Nid currNid = subtree.getNode(parName+":NAME");
	    parName = currNid.getDATA().toString();
	    subtree.setDefault(olddef);
	}catch(Exception exc)
	{
	    JOptionPane.showMessageDialog(null, "Error getting Component Name in DeviceParameters: " + exc);
	    parName = "";
	}
	return parName;
    }
    protected Nid getComponentNidAt(Nid nidData, int idx)
    {
	String parName;
	int olddef;
	String paramName = getParameterName();
	if(idx < 10)
	    parName = paramName+"_00"+(idx+1);
	else if(idx < 100)
	    parName =  paramName+"_0"+(idx+1);
	else
	    parName = paramName+"_" + (idx+1);
	try {
		olddef = subtree.getDefault();
	    subtree.setDefault(nidData.getNidNumber());
	    Nid currNid;
	    currNid = subtree.getNode(parName);
	    subtree.setDefault(olddef);
	    return currNid;
	}catch(Exception exc)
	{
	     JOptionPane.showMessageDialog(null, "Error getting Component Nid in DeviceParameters: " + exc);
	    return null;
	}
    }
    protected int getNumComponents(Nid nidData)
    {
	try {
	    int olddef = subtree.getDefault();
	    subtree.setDefault(nidData.getNidNumber());
	    Nid currNid;
	    currNid = subtree.getNode(":NUM_ACTIVE");
	    int numComponents = currNid.toInt();
	    subtree.setDefault(olddef);
	    return numComponents;
	 }catch(Exception exc)
	{
	     JOptionPane.showMessageDialog(null, "Error getting Num Components in DeviceParameters: " + exc);
	    return 0;
	}
    }

    protected void addParameter(JPanel jp, Nid nidData)
     {
	try {
    	int olddef = subtree.getDefault();
	    subtree.setDefault(nidData.getNidNumber());
	    Nid currNid;
	    currNid = subtree.getNode(":DESCRIPTION");
	    String description = currNid.getDATA().toString();
	    currNid = subtree.getNode(":TYPE");
	    String typeStr = currNid.getDATA().toString();
	    currNid = subtree.getNode(":DIMS");
	    int[]dims = currNid.toIntArray();
	    currNid = subtree.getNode(":DATA");
	    if(dims[0] == 0) //Scalar
	    {
	        DeviceField currField = new DeviceField();
	        currField.setSubtree(subtree);
	        currField.setBaseNid(currNid.toInt());
	        currField.setOffsetNid(0);
	        currField.setLabelString(description);
	        JPanel jp1 = new JPanel();
	        jp1.add(currField);
	        jp.add(jp1);
	        currField.configure(currNid.toInt());
	        parameters.addElement(currField);
	    }
	    else //Array or Matrix, use DeviceTable
	    {
	        DeviceTable currField = new DeviceTable();
	        currField.setSubtree(subtree);
	        currField.setBaseNid(currNid.toInt());
	        currField.setOffsetNid(0);
	        if(typeStr.toUpperCase().trim().equals("BINARY"))
	            currField.setBinary(true);
	        else
	            currField.setBinary(false);
	        if(typeStr.toUpperCase().equals("REFLEX"))
	            currField.setRefMode(DeviceTable.REFLEX);
	        if(typeStr.toUpperCase().equals("REFLEX_INVERT"))
	            currField.setRefMode(DeviceTable.REFLEX_INVERT);
	        currField.setUseExpressions(true);
	        currField.setDisplayRowNumber(true);
	        currField.setLabelString(description);
	        int numCols;
	        if(dims.length == 1)
	        {
	            currField.setNumRows(1);
	            currField.setNumCols(numCols = dims[0]);
	        }
	        else
	        {
	            currField.setNumRows(dims[0]);
	            currField.setNumCols(numCols = dims[1]);
	        }
	        String colNames[] = new String[numCols];
	        if(typeStr.toUpperCase().equals("REFLEX_INVERT")||typeStr.toUpperCase().equals("REFLEX"))
	        {
	            for(int i = 0; i <= numCols/2; i++)
	                colNames[i] = "" + (-i);
	            for(int i = 1; i < numCols/2; i++)
	                colNames[numCols/2+i] = ""+(numCols/2-i);
	        }
	        else
	        {
	            for(int i = 0; i < numCols; i++)
	                colNames[i] = "" + i;

	        }
	        currField.setColumnNames(colNames);
	        jp.add(currField);
	        currField.configure(currNid.toInt());
	        parameters.addElement(currField);
	    }
	    subtree.setDefault(olddef);
	}catch(Exception exc)
	{
	    System.err.println("Error in DeviceParameters.addParam: " + exc);
	}
    }
    protected void resetComponent(Nid nidData)
    {
	for(int i = 0; i < parameters.size(); i++)
	{
	    ((DeviceField)parameters.elementAt(i)).reset();
	}
    }
    protected void applyComponent(Nid nidData)
    {

	try {
	    for(int i = 0; i < parameters.size(); i++)
	    {
	        ((DeviceComponent)parameters.elementAt(i)).apply();
	    }
	} catch (Exception exc)
	{
	        System.err.println("Error in DeviceParameters.apply: " + exc);
	}
    }
    public static void main(String args[])
    {
	DeviceParameters dp = new DeviceParameters();
    }
}






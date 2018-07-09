import java.util.Vector;

import javax.swing.JOptionPane;
import javax.swing.JPanel;
public class  DeviceParameters extends DeviceMultiComponent
{

    protected Vector parameters = new Vector();
    protected String getParameterName() {return "PAR";}
    protected String getComponentNameAt(NidData nidData, int idx)
    {
        String parName;
        NidData prevDefNid;
        String paramName = getParameterName();
        if(idx < 10)
            parName = paramName+"_00"+(idx+1);
        else if(idx < 100)
            parName =  paramName+"_0"+(idx+1);
        else
            parName = paramName+"_" + (idx+1);
        try {
            prevDefNid = subtree.getDefault(0);
            subtree.setDefault(nidData, 0);
            NidData currNid;
            currNid = subtree.resolve(new PathData(parName+":NAME"), 0);
            parName = subtree.evaluateData(currNid, idx).getString();
            subtree.setDefault(prevDefNid, 0);
        }catch(Exception exc)
        {
            JOptionPane.showMessageDialog(null, "Error getting Component Name in DeviceParameters: " + exc);
            parName = "";
        }
        return parName;
    }
    protected NidData getComponentNidAt(NidData nidData, int idx)
    {
        String parName;
        NidData prevDefNid;
        String paramName = getParameterName();
        if(idx < 10)
            parName = paramName+"_00"+(idx+1);
        else if(idx < 100)
            parName =  paramName+"_0"+(idx+1);
        else
            parName = paramName+"_" + (idx+1);
        try {
            prevDefNid = subtree.getDefault(0);
            subtree.setDefault(nidData, 0);
            NidData currNid;
            currNid = subtree.resolve(new PathData(parName), 0);
            subtree.setDefault(prevDefNid, 0);
            return currNid;
        }catch(Exception exc)
        {
             JOptionPane.showMessageDialog(null, "Error getting Component Nid in DeviceParameters: " + exc);
            return null;
        }
    }
    protected int getNumComponents(NidData nidData)
    {
        try {
            NidData prevDefNid = subtree.getDefault(0);
            subtree.setDefault(nidData, 0);
            NidData currNid;
            currNid = subtree.resolve(new PathData(":NUM_ACTIVE"), 0);
            int numComponents = subtree.evaluateData(currNid, 0).getInt();
            subtree.setDefault(prevDefNid, 0);
            return numComponents;
         }catch(Exception exc)
        {
             JOptionPane.showMessageDialog(null, "Error getting Num Components in DeviceParameters: " + exc);
            return 0;
        }
    }

    protected void addParameter(JPanel jp, NidData nidData)
     {
        try {
            subtree.setDefault(nidData, 0);
            NidData currNid;
            currNid = subtree.resolve(new PathData(":DESCRIPTION"), 0);
            String description = subtree.evaluateData(currNid, 0).getString();
            currNid = subtree.resolve(new PathData(":TYPE"), 0);
            String typeStr = subtree.evaluateData(currNid, 0).getString();
            currNid = subtree.resolve(new PathData(":DIMS"), 0);
            int[]dims = subtree.evaluateData(currNid, 0).getIntArray();
            currNid = subtree.resolve(new PathData(":DATA"), 0);
            if(dims[0] == 0) //Scalar
            {
                DeviceField currField = new DeviceField();
                currField.setSubtree(subtree);
                currField.setBaseNid(currNid.getInt());
                currField.setOffsetNid(0);
                currField.setLabelString(description);
                JPanel jp1 = new JPanel();
                jp1.add(currField);
                jp.add(jp1);
                currField.configure(currNid.getInt());
                parameters.addElement(currField);
            }
            else //Array or Matrix, use DeviceTable
            {
                DeviceTable currField = new DeviceTable();
                currField.setSubtree(subtree);
                currField.setBaseNid(currNid.getInt());
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
                currField.configure(currNid.getInt());
                parameters.addElement(currField);
            }
        }catch(Exception exc)
        {
            System.err.println("Error in DeviceParameters.addParam: " + exc);
        }
    }
    protected void resetComponent(NidData nidData)
    {
        for(int i = 0; i < parameters.size(); i++)
        {
            ((DeviceField)parameters.elementAt(i)).reset();
        }
    }
    protected void applyComponent(NidData nidData)
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






/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
import javax.swing.*;
import java.awt.*;
import java.util.*;
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
            NidData prevDefNid = subtree.getDefault(0);
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






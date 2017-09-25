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
public abstract class DeviceMultiComponent extends DeviceComponent
{
    NidData compNids[];
    protected String baseName = ".PARAMETERS";
    //return null when no more components
    public void setBaseName(String baseName){this.baseName = baseName;}
    public String getBaseName(){return baseName; }
    protected abstract String getComponentNameAt(NidData nidData, int idx);
    protected abstract NidData getComponentNidAt(NidData nidData, int idx);
    protected abstract int getNumComponents(NidData nidData);
    protected abstract void addParameter(JPanel jp, NidData nidData);
    protected abstract void resetComponent(NidData nidData);
    protected abstract void applyComponent(NidData nidData);
    protected String getNameSeparator() {return "/";}
    public void configure(int baseNid)
    {
        try {
            NidData prevDefNid = subtree.getDefault(0);
            subtree.setDefault(new NidData(baseNid), 0);
            nidData = subtree.resolve(new PathData(baseName), 0);
            subtree.setDefault(prevDefNid, 0);
        }catch(Exception exc)
        {
            JOptionPane.showMessageDialog(null, "Cannot resolve base nid: "+baseName);
            return;
        }

        this.baseNid = baseNid;
        baseNidData = new NidData(baseNid);
        int numComponents = getNumComponents(nidData);
        String compNames[] = new String[numComponents];
        compNids = new NidData[numComponents];
        for(int i = 0; i < numComponents; i++)
        {
            compNames[i] = getComponentNameAt(nidData, i);
            compNids[i] = getComponentNidAt(nidData, i);
        }
        Hashtable compHash = new Hashtable();
        String separator = getNameSeparator();
        for(int i = 0; i < numComponents; i++)
        {
            if(compNames[i] == null) continue;
            StringTokenizer st = new StringTokenizer(compNames[i], separator);
            String firstPart = st.nextToken();
            if(!st.hasMoreTokens())
                firstPart = "Default";
            Vector nidsV = (Vector)compHash.get(firstPart);
            if(nidsV == null)
            {
                nidsV = new Vector();
                compHash.put(firstPart, nidsV);
            }
            if(compNids != null)
                nidsV.addElement(compNids[i]);
        }
        setLayout(new BorderLayout());
        JTabbedPane tabP = new JTabbedPane();
        add(tabP, "Center");
        Enumeration groups = compHash.keys();
        while(groups.hasMoreElements())
        {
            String currName = (String)groups.nextElement();
            JPanel jp = new JPanel();
            tabP.add(currName, new JScrollPane(jp));
            Vector currParams = (Vector)compHash.get(currName);
            int nParams = currParams.size();
            jp.setLayout(new GridLayout(nParams, 1));
            for(int i = 0; i < nParams; i++)
                addParameter(jp, (NidData)currParams.elementAt(i));
        }
    }


    public void reset()
    {
        for(int i = 0; i < compNids.length; i++)
            resetComponent(compNids[i]);
    }

    public void apply() throws Exception
    {
        for(int i = 0; i < compNids.length; i++)
            applyComponent(compNids[i]);
    }

    public void apply(int currBaseNid) throws Exception
    {
        apply();
    }

    protected  void initializeData(Data data, boolean is_on){}
    protected  void displayData(Data data, boolean is_on){}
    protected  Data getData(){return null;}
    protected  boolean getState(){return false;}
}









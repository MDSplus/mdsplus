import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.StringTokenizer;
import java.util.Vector;

import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;

import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Nid;
public abstract class DeviceMultiComponent extends DeviceComponent
{
    Nid compNids[];
    protected String baseName = ".PARAMETERS";
    //return null when no more components
    public void setBaseName(String baseName){this.baseName = baseName;}
    public String getBaseName(){return baseName; }
    protected abstract String getComponentNameAt(Nid nidData, int idx);
    protected abstract Nid getComponentNidAt(Nid nidData, int idx);
    protected abstract int getNumComponents(Nid nidData);
    protected abstract void addParameter(JPanel jp, Nid nidData);
    protected abstract void resetComponent(Nid nidData);
    protected abstract void applyComponent(Nid nidData);
    protected String getNameSeparator() {return "/";}
    public void configure(int baseNid)
    {
	try {
	    int olddef = subtree.getDefault();
	    subtree.setDefault(baseNid);
	    nidData = subtree.getNode(baseName);
	    subtree.setDefault(olddef);
	}catch(Exception exc)
	{
	    JOptionPane.showMessageDialog(null, "Cannot resolve base nid: "+baseName);
	    return;
	}

	this.baseNid = baseNid;
	baseNidData = new Nid(baseNid, subtree);
	int numComponents = getNumComponents(nidData);
	String compNames[] = new String[numComponents];
	compNids = new Nid[numComponents];
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
	        addParameter(jp, (Nid)currParams.elementAt(i));
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

    protected  void initializeData(Descriptor<?> data, boolean is_on){}
    protected  void displayData(Descriptor<?> data, boolean is_on){}
    protected  Descriptor<?> getData(){return null;}
    protected  boolean getState(){return false;}
}









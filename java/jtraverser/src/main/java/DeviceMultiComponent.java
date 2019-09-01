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









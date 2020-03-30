import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;

import mds.data.descriptor_s.Nid;
public  class  DeviceWaveParameters extends DeviceParameters
{
     public DeviceWaveParameters(){}
     protected String getParameterName() {return "WAVE";}
     protected void addParameter(JPanel jp, Nid nid)
     {
	try {
	    subtree.setDefault(nid.getNidNumber());
	    Nid currNid;
	    currNid = subtree.getNode(":DESCRIPTION");
	    String description = currNid.getData().toString();
	    Nid currXNid = subtree.getNode(":X");
	    DeviceField currXField = new DeviceField();
	    currXField.setSubtree(subtree);
	    currXField.setBaseNid(currXNid.getNidNumber());
	    currXField.setOffsetNid(0);
	    currXField.setLabelString("X:");
	    currXField.setNumCols(30);
	    parameters.add(currXField);
	    Nid currYNid = subtree.getNode(":Y");
	    DeviceField currYField = new DeviceField();
	    currYField.setSubtree(subtree);
	    currYField.setBaseNid(currYNid.getNidNumber());
	    currYField.setOffsetNid(0);
	    currYField.setLabelString("Y:");
	    currYField.setNumCols(30);
	    parameters.add(currYField);
	    JPanel jp1 = new JPanel();
	    jp1.add(new JLabel(description));
	    jp1.add(currXField);
	    jp1.add(currYField);
	    jp.add(jp1);
	    currXField.configure(currXNid.getNidNumber());
	    currYField.configure(currYNid.getNidNumber());
	 }catch(Exception exc)
	{
	    JOptionPane.showMessageDialog(null, "Error in DeviceWaveParameters.addParam: " + exc);
	}
    }
}






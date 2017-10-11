import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
public  class  DeviceWaveParameters extends DeviceParameters
{
     public DeviceWaveParameters(){}
     protected String getParameterName() {return "WAVE";}
     protected void addParameter(JPanel jp, NidData nidData)
     {
        try {
            subtree.setDefault(nidData, 0);
            NidData currNid;
            currNid = subtree.resolve(new PathData(":DESCRIPTION"), 0);
            String description = subtree.evaluateData(currNid, 0).getString();
            NidData currXNid = subtree.resolve(new PathData(":X"), 0);
            DeviceField currXField = new DeviceField();
            currXField.setSubtree(subtree);
            currXField.setBaseNid(currXNid.getInt());
            currXField.setOffsetNid(0);
            currXField.setLabelString("X:");
            currXField.setNumCols(30);
            parameters.add(currXField);
            NidData currYNid = subtree.resolve(new PathData(":Y"), 0);
            DeviceField currYField = new DeviceField();
            currYField.setSubtree(subtree);
            currYField.setBaseNid(currYNid.getInt());
            currYField.setOffsetNid(0);
            currYField.setLabelString("Y:");
            currYField.setNumCols(30);
            parameters.add(currYField);
            JPanel jp1 = new JPanel();
            jp1.add(new JLabel(description));
            jp1.add(currXField);
            jp1.add(currYField);
            jp.add(jp1);
            currXField.configure(currXNid.getInt());
            currYField.configure(currYNid.getInt());
         }catch(Exception exc)
        {
            JOptionPane.showMessageDialog(null, "Error in DeviceWaveParameters.addParam: " + exc);
        }
    }
}






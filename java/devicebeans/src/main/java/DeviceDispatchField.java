import java.awt.GridLayout;

import javax.swing.JCheckBox;
import javax.swing.JPanel;

public class DeviceDispatchField extends DeviceComponent
{

    protected DeviceLabeledExprEditor ident, phase, when, completion;
    protected String action;
    protected String dispatch;
    protected JCheckBox state;

    protected void initializeData(String data, boolean is_on)
    {
        action = data;
        dispatch = "DISPATCH_OF("+data+")";
	setLayout(new GridLayout(4,1));
        String server;
        try {
            server = subtree.getString("IDENT_OF("+dispatch+")");
            if(server != null && server.trim().length() > 0)
                server = "\""+server+"\"";
        }catch(Exception exc){server = "";}
	add(ident = new DeviceLabeledExprEditor("Ident:         ", new DeviceExprEditor(server, true)));
        String phaseStr;
        try {
            phaseStr = subtree.getString("PHASE_OF("+dispatch+")");
            if(phaseStr != null && phaseStr.trim().length() > 0)
                phaseStr = "\""+phaseStr+"\"";
        }catch(Exception exc){phaseStr = "";}
 	add(phase = new DeviceLabeledExprEditor("Phase:        ", new DeviceExprEditor(phaseStr, true)));
        String completionStr;
        try {
            completionStr = subtree.getString("COMPLETION_OF("+dispatch+")");
            if(completionStr != null && completionStr.trim().length() > 0)
                completionStr = "\""+completionStr+"\"";
        }catch(Exception exc){completionStr = "";}

	add(completion = new DeviceLabeledExprEditor("Completion:", new DeviceExprEditor(completionStr, true)));
	JPanel jp = new JPanel();
        try {
            int sequence = subtree.getInt("WHEN_OF("+dispatch+")");
            jp.add(when = new DeviceLabeledExprEditor("Sequence:  ", new DeviceExprEditor(""+sequence, false, 1, 6)));
        }catch(Exception exc)
        {
            String whenStr;
            try {
                whenStr = subtree.execute("WHEN_OF("+dispatch+")");
             }catch(Exception exc1){whenStr = "";}
             jp.add(when = new DeviceLabeledExprEditor("When:  ", new DeviceExprEditor(whenStr, false, 1, 6)));

        }
        jp.add(state = new JCheckBox("Is On", is_on));
	add(jp);
    }

    protected void displayData(String data, boolean is_on)
    {
        try {
            dispatch = subtree.evaluate("DISPATCH_OF("+data+")");
        }catch(Exception exc)
        {
           ident.setData("");
           phase.setData("");
           completion.setData("");
           when.setData("");
           return;
        }
        String server;
        try {
            server = subtree.getString("IDENT_OF("+dispatch+")");
        }catch(Exception exc){server = "";}
	ident.setData("\""+server+"\"");
        String phaseStr;
        try {
            phaseStr = subtree.getString("PHASE_OF("+dispatch+")");
        }catch(Exception exc){phaseStr = "";}
 	phase.setData("\""+phaseStr+"\"");
        String completionStr;
        try {
            completionStr = subtree.getString("COMPLETION_OF("+dispatch+")");
        }catch(Exception exc){completionStr = "";}

	completion.setData("\""+completionStr+"\"");
        try {
            int sequence = subtree.getInt("WHEN_OF("+dispatch+")");
            when.setData(""+sequence);
         }catch(Exception exc)
        {
            String whenStr;
            try {
                whenStr = subtree.evaluate("WHEN_OF("+dispatch+")");
             }catch(Exception exc1){whenStr = "";}
             when.setData(whenStr);
 
        }
	state.setSelected(is_on);
    }

    protected String getData()
    {
        int type;
        try
        {
            subtree.getInt(when.getData());
            type = 2; //Sequential
        }catch(Exception exc) {type = 3; }
        try {
            String retAction = "BUILD_ACTION(BUILD_DISPATCH("+type+","+ident.getData()+","+phase.getData()+","+
                when.getData()+","+completion.getData()+"),"+subtree.evaluate("TASK_OF("+action+")")+")";
            return retAction;
        }catch(Exception exc){return null;}
     }

    protected boolean getState()
    {
	return state.isSelected();
    }
    public void setEnabled(boolean state)
    {
    }
    /*Allow writing only if model */
    protected boolean isDataChanged()
    {
	try
	{
	    if (subtree.getShot() == -1)
	        return true;
	    else
	        return false;
	}
	catch (Exception exc)
	{
	    return false;
	}
    }
}

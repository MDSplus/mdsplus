import java.awt.GridLayout;

import javax.swing.JCheckBox;
import javax.swing.JPanel;

import jtraverser.editor.ExprEditor;
import mds.MdsException;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Action;
import mds.data.descriptor_r.Dispatch;

public class DeviceDispatchField extends DeviceComponent
{

    protected ExprEditor ident, phase, when, completion;
    protected Action action;
    protected Dispatch dispatch;
    protected JCheckBox state;

    protected void initializeData(Descriptor<?> data, boolean is_on)
    {
	if (!(data instanceof Action))
	{
	    System.out.println("\nError: DeviceDispatchField used for non action data");
	    return;
	}
	action = (Action)data;
	dispatch = (Dispatch)action.getDispatch();
	if(dispatch == null) return;
	setLayout(new GridLayout(4,1));
	add(ident = ExprEditor.addLabel("Ident:         ", new ExprEditor(dispatch.getIdent(), true, subtree)));
	add(phase =  ExprEditor.addLabel("Phase:        ", new ExprEditor(dispatch.getPhase(), true, subtree)));
	add(completion =  ExprEditor.addLabel("Completion:", new ExprEditor(dispatch.getCompletion(), true, subtree)));
	JPanel jp = new JPanel();
	jp.add(when =  ExprEditor.addLabel("Sequence:  ", new ExprEditor(dispatch.getWhen(), false, subtree)));
	jp.add(state = new JCheckBox("Is On", is_on));
	add(jp);
    }

    protected void displayData(Descriptor<?> data, boolean is_on)
    {
	if(ident == null) return;
	if (!(data instanceof Action))
	{
	    System.out.println("\nError: DeviceDispatchField used for non action data");
	    return;
	}
	Dispatch dispatch = (Dispatch)((Action)data).getDispatch();
	ident.setData(dispatch.getIdent());
	phase.setData(dispatch.getPhase());
	when.setData(dispatch.getWhen());
	completion.setData(dispatch.getCompletion());
	state.setSelected(is_on);
    }

    protected Descriptor<?> getData()
    {
	if(dispatch == null) return null;
	try {
		return new Action(new Dispatch(dispatch.getType(), ident.getData(),
		    phase.getData(), when.getData(), completion.getData()), action.getTask(),
		    action.getErrorLogs(), action.getCompletionMessage(), action.getPerformance());
	} catch (MdsException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
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
	    if (subtree.shot == -1)
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

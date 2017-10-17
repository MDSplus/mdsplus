import java.awt.GridLayout;

import javax.swing.JCheckBox;
import javax.swing.JPanel;

public class DeviceDispatchField extends DeviceComponent
{

    protected LabeledExprEditor ident, phase, when, completion;
    protected ActionData action;
    protected DispatchData dispatch;
    protected JCheckBox state;

    protected void initializeData(Data data, boolean is_on)
    {
        if (!(data instanceof ActionData))
        {
            System.out.println("\nError: DeviceDispatchField used for non action data");
            return;
        }
        action = (ActionData)data;
        dispatch = (DispatchData)((ActionData)data).getDispatch();
        if(dispatch == null) return;
        setLayout(new GridLayout(4,1));
        add(ident = new LabeledExprEditor("Ident:         ", new ExprEditor(dispatch.getIdent(), true)));
        add(phase = new LabeledExprEditor("Phase:        ", new ExprEditor(dispatch.getPhase(), true)));
        add(completion = new LabeledExprEditor("Completion:", new ExprEditor(dispatch.getCompletion(), true)));
        JPanel jp = new JPanel();
        jp.add(when = new LabeledExprEditor("Sequence:  ", new ExprEditor(dispatch.getWhen(), false, 1, 6)));
        jp.add(state = new JCheckBox("Is On", is_on));
        add(jp);
    }

    protected void displayData(Data data, boolean is_on)
    {
        if(ident == null) return;
        if (!(data instanceof ActionData))
        {
            System.out.println("\nError: DeviceDispatchField used for non action data");
            return;
        }
        DispatchData dispatch = (DispatchData)((ActionData)data).getDispatch();
        ident.setData(dispatch.getIdent());
        phase.setData(dispatch.getPhase());
        when.setData(dispatch.getWhen());
        completion.setData(dispatch.getCompletion());
        state.setSelected(is_on);
    }

    protected Data getData()
    {
        if(dispatch == null) return null;
        return new ActionData(new DispatchData(dispatch.getType(), ident.getData(),
            phase.getData(), when.getData(), completion.getData()), action.getTask(),
            action.getErrorlogs(), action.getCompletionMessage(), action.getPerformance());
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

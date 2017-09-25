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
import javax.swing.event.*;
import java.awt.*;

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

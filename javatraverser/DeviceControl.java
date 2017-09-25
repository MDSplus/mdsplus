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
import java.awt.event.*;
import java.awt.*;

public abstract class DeviceControl extends JButton
{
    DeviceSetup deviceSetup = null;
    protected String [] checkExpressions, checkMessages;

    DeviceControl()
    {
        addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
                discoverDevice();
                if(deviceSetup != null)
                    doOperation(deviceSetup);
            }
        });
    }

    protected void discoverDevice()
    {
        Container curr_container;
        Component curr_component = this;
        do {
            curr_container = curr_component.getParent();
            curr_component = curr_container;
        }while ((curr_container != null) && !(curr_container instanceof DeviceSetup));
        if(curr_container != null)
        {
            deviceSetup = (DeviceSetup) curr_container;
         }
    }

    public void setCheckExpressions(String [] checkExpressions)
    {
        this.checkExpressions = checkExpressions;
    }
    public String [] getCheckExpressions() {return checkExpressions; }
    public void setCheckMessages(String [] checkMessages)
    {
        this.checkMessages = checkMessages;
    }
    public String [] getCheckMessages() {return checkMessages; }
    protected boolean check()
    {
        if(deviceSetup == null)
            discoverDevice();
        if(deviceSetup != null && checkExpressions != null && checkMessages != null)
            return deviceSetup.check(checkExpressions, checkMessages);
        return true;
    }

    void setReadOnly(boolean readOnly){}

    protected abstract void doOperation(DeviceSetup deviceSetup);
}


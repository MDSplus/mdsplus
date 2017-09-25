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
import java.beans.*;
import java.awt.*;
import java.lang.reflect.*;
import java.awt.event.*;

public class DeviceWaveCustomizer extends DeviceCustomizer implements Customizer
{
    DeviceWave bean = null;
    Object obj;
    PropertyChangeSupport listeners = new PropertyChangeSupport(this);
    TextField identifier, updateIdentifier, updateExpression;

    Choice nids;
    Button doneButton;
    Checkbox minXVisible;
    Checkbox maxXVisible;
    Checkbox minYVisible;
    Checkbox maxYVisible;

    public DeviceWaveCustomizer()
    {
    }
    public void setObject(Object o)
    {

        bean = (DeviceWave)o;

        setLayout(new BorderLayout());
        Panel jp = new Panel();
        jp.setLayout(new GridLayout(3, 1));
        Panel jp1 = new Panel();
        jp1.add(new Label("Opt. identifier: "));
        jp1.add(identifier = new TextField(bean.getIdentifier(), 20));
        jp1.add(new Label("Offset nid: "));
        jp1.add(nids = new Choice());

        String names[] = getDeviceFields();

        if(names != null)
        for(int i = 0; i < names.length; i++)
            nids.addItem(names[i]);
        int offsetNid = bean.getOffsetNid();
        if(offsetNid > 0) offsetNid--;
        nids.select(offsetNid);
        jp.add(jp1);
        jp1 = new Panel();
        jp1.add(minXVisible = new Checkbox("Min X Visible: ", bean.getMinXVisible()));
        jp1.add(maxXVisible = new Checkbox("Max X Visible: ", bean.getMaxXVisible()));
        jp1 = new Panel();
        jp1.add(minYVisible = new Checkbox("Min Y Visible: ", bean.getMinYVisible()));
        jp1.add(maxYVisible = new Checkbox("Max Y Visible: ", bean.getMaxYVisible()));
        jp.add(jp1);

        jp1 = new Panel();
        jp1.add(new Label("Update id: "));
        jp1.add(updateIdentifier = new TextField(bean.getUpdateIdentifier(), 10));
        jp1.add(new Label("Update expr: "));
        jp1.add(updateExpression = new TextField(bean.getUpdateExpression(), 30));
        jp.add(jp1);

        add(jp, "Center");
        jp = new Panel();
        jp.add(doneButton = new Button("Apply"));
        doneButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
                boolean oldMinXVisible = bean.getMinXVisible();
                bean.setMinXVisible(minXVisible.getState());
                listeners.firePropertyChange("minXVisible", oldMinXVisible, bean.getMinXVisible());
                boolean oldMaxXVisible = bean.getMaxXVisible();
                bean.setMaxXVisible(maxXVisible.getState());
                listeners.firePropertyChange("maxXVisible", oldMaxXVisible, bean.getMaxXVisible());
                boolean oldMinYVisible = bean.getMinYVisible();
                bean.setMinYVisible(minYVisible.getState());
                listeners.firePropertyChange("minYVisible", oldMinYVisible, bean.getMinYVisible());
                boolean oldMaxYVisible = bean.getMaxYVisible();
                bean.setMaxYVisible(maxYVisible.getState());
                listeners.firePropertyChange("maxYVisible", oldMaxYVisible, bean.getMaxYVisible());
                int oldOffsetNid = bean.getOffsetNid();
                bean.setOffsetNid(nids.getSelectedIndex() + 1);
                listeners.firePropertyChange("offsetNid", oldOffsetNid, bean.getOffsetNid());
                String oldIdentifier = bean.getIdentifier();
                bean.setIdentifier(identifier.getText());
                listeners.firePropertyChange("identifier", oldIdentifier, bean.getIdentifier());
                String oldUpdateIdentifier = bean.getIdentifier();
                bean.setUpdateIdentifier(updateIdentifier.getText().trim());
                listeners.firePropertyChange("updateIdentifier", oldUpdateIdentifier, bean.getUpdateIdentifier());
                String oldUpdateExpression = bean.getUpdateExpression();
                bean.setUpdateExpression(updateExpression.getText().trim());
                listeners.firePropertyChange("updateExpression", oldUpdateExpression, bean.getUpdateExpression());
            }
        });
        add(jp,"South");
    }

    public void addPropertyChangeListener(PropertyChangeListener l)
    {
        listeners.addPropertyChangeListener(l);
    }

    public void removePropertyChangeListener(PropertyChangeListener l)
    {
        listeners.removePropertyChangeListener(l);
    }
  }


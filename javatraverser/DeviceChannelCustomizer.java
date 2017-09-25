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

public class DeviceChannelCustomizer extends DeviceCustomizer implements Customizer
{
    DeviceChannel bean = null;
    Object obj;
    PropertyChangeSupport listeners = new PropertyChangeSupport(this);
    TextField labelString, lines, columns, updateId, showVal;
    Choice nids;
    Button doneButton;
    Checkbox showBorder, inSameLine, showState;

    public DeviceChannelCustomizer()
    {
    }
    public void setObject(Object o)
    {
        bean = (DeviceChannel)o;

        setLayout(new BorderLayout());
        Panel jp = new Panel();
        jp.setLayout(new GridLayout(4, 1));
        Panel jp1 = new Panel();
        jp1.add(new Label("Label: "));
        jp1.add(labelString = new TextField(30));
        labelString.setText(bean.getLabelString());
        jp1.add(showState = new Checkbox("Show state: ", bean.getShowState()));
        jp.add(jp1);
        jp1 = new Panel();
        jp1.add(showBorder = new Checkbox("Border visible: ", bean.getBorderVisible()));
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
        jp1.add(new Label("Lines: "));
        jp1.add(lines = new TextField(4));
        lines.setText((new Integer(bean.getLines())).toString());
        jp1.add(new Label("Columns: "));
        jp1.add(columns = new TextField(4));
        columns.setText((new Integer(bean.getColumns())).toString());
        jp1.add(inSameLine = new Checkbox("Same line: ", bean.getInSameLine()));
        jp.add(jp1);
        jp1 = new Panel();
        jp1.add(new Label("Show Id: "));
        jp1.add(updateId = new TextField(8));
        updateId.setText(bean.getUpdateIdentifier());
        jp1.add(new Label("Show value: "));
        jp1.add(showVal = new TextField(8));
        showVal.setText(bean.getShowVal());
        jp.add(jp1);

        add(jp, "Center");
        jp = new Panel();
        jp.add(doneButton = new Button("Apply"));
        doneButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
                String oldLabelString = bean.getLabelString();
                bean.setLabelString(labelString.getText());
                listeners.firePropertyChange("labelString", oldLabelString, bean.getLabelString());
                boolean oldBorderVisible = bean.getBorderVisible();
                bean.setBorderVisible(showBorder.getState());
                listeners.firePropertyChange("borderVisible", oldBorderVisible, bean.getBorderVisible());
                boolean oldShowState = bean.getShowState();
                bean.setShowState(showState.getState());
                listeners.firePropertyChange("showState", oldShowState, bean.getShowState());
                boolean oldInSameLine = bean.getInSameLine();
                bean.setInSameLine(inSameLine.getState());
                listeners.firePropertyChange("inSameLine", oldInSameLine, bean.getInSameLine());
                int oldOffsetNid = bean.getOffsetNid();
                bean.setOffsetNid(nids.getSelectedIndex() + 1);
                listeners.firePropertyChange("offsetNid", oldOffsetNid, bean.getOffsetNid());
                int oldLines = bean.getLines();
                bean.setLines((new Integer(lines.getText())).intValue());
                listeners.firePropertyChange("lines", oldLines, bean.getLines());
                int oldColumns = bean.getColumns();
                bean.setColumns((new Integer(columns.getText())).intValue());
                listeners.firePropertyChange("columns", oldColumns, bean.getColumns());
                String oldUpdateIdentifier = bean.getUpdateIdentifier();
                bean.setUpdateIdentifier(updateId.getText());
                listeners.firePropertyChange("updateIdentifier", oldUpdateIdentifier, bean.getUpdateIdentifier());
                String oldShowVal = bean.getShowVal();
                bean.setShowVal(showVal.getText());
                listeners.firePropertyChange("showVal", oldShowVal, bean.getShowVal());
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


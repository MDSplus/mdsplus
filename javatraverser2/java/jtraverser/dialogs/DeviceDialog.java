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
package jtraverser.dialogs;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.WindowConstants;
import jtraverser.Node;
import jtraverser.editor.SegmentEditor;
import jtraverser.editor.usage.ActionEditor;
import jtraverser.editor.usage.AnyEditor;
import jtraverser.editor.usage.AxisEditor;
import jtraverser.editor.usage.DispatchEditor;
import jtraverser.editor.usage.NumericEditor;
import jtraverser.editor.usage.SignalEditor;
import jtraverser.editor.usage.TaskEditor;
import jtraverser.editor.usage.TextEditor;
import jtraverser.editor.usage.WindowEditor;
import mds.MdsException;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.NODE;

@SuppressWarnings("serial")
public final class DeviceDialog extends JDialog{
    public static final DeviceDialog open(final Node node, final boolean editable) {
        final DeviceDialog dialog = new DeviceDialog(node, editable);
        Dialogs.setLocation(dialog);
        dialog.setVisible(true);
        return dialog;
    }
    private final Node    node;
    private final boolean editable;
    private final JButton ok_b, apply_b, reset_b, cancel_b;

    private DeviceDialog(final Node node, final boolean editable){
        super(JOptionPane.getRootFrame());
        this.node = node;
        this.editable = editable;
        this.setLayout(new BorderLayout());
        final JPanel jp = new JPanel();
        this.add(jp, BorderLayout.PAGE_END);
        if(this.editable){
            this.ok_b = new JButton("Ok");
            this.ok_b.addActionListener(new ActionListener(){
                @Override
                public void actionPerformed(final ActionEvent e) {
                    DeviceDialog.this.ok();
                }
            });
            jp.add(this.ok_b);
            this.apply_b = new JButton("Apply");
            this.apply_b.addActionListener(new ActionListener(){
                @Override
                public void actionPerformed(final ActionEvent e) {
                    DeviceDialog.this.apply();
                }
            });
            jp.add(this.apply_b);
            this.reset_b = new JButton("Reset");
            this.reset_b.addActionListener(new ActionListener(){
                @Override
                public void actionPerformed(final ActionEvent e) {
                    DeviceDialog.this.reset();
                }
            });
            jp.add(this.reset_b);
            this.addKeyListener(new KeyAdapter(){
                @Override
                public void keyTyped(final KeyEvent e) {
                    if(e.getKeyCode() == KeyEvent.VK_ENTER) DeviceDialog.this.ok();
                }
            });
        }else this.ok_b = this.apply_b = this.reset_b = null;
        this.cancel_b = new JButton("Cancel");
        this.cancel_b.addActionListener(new ActionListener(){
            @Override
            public void actionPerformed(final ActionEvent e) {
                DeviceDialog.this.cancel();
            }
        });
        this.cancel_b.setSelected(true);
        jp.add(this.cancel_b);
    }

    protected void apply() {
        // TODO Auto-generated method stub
    }

    protected void cancel() {
        // TODO Auto-generated method stub
    }

    protected void ok() {
        // TODO Auto-generated method stub
    }

    private final Component pickEditor(final NodeInfo node) {
        Component edit;
        //if(node.isSegmented()) edit = new SegmentEditor(node, this.editable, this);
        else{
            Descriptor<?> data = null;
            try{
                data = this.node.getData();
            }catch(final MdsException e){
                MdsException.stderr("DataDialog.getData", e);
                e.printStackTrace();
            }
            switch(node.getUsage()){
                case NODE.USAGE_SIGNAL:
                    edit = new SignalEditor(data, this.editable, this);
                    break;
                case NODE.USAGE_ACTION:
                    edit = new ActionEditor(data, this.editable, this, node.getStatus(), node.treeview.isModel());
                    break;
                case NODE.USAGE_DISPATCH:
                    edit = new DispatchEditor(data, this.editable, this);
                    break;
                case NODE.USAGE_TASK:
                    edit = new TaskEditor(data, this.editable, this);
                    break;
                case NODE.USAGE_WINDOW:
                    edit = new WindowEditor(data, this.editable, this);
                    break;
                case NODE.USAGE_NUMERIC:
                    edit = new NumericEditor(data, this.editable, this);
                    break;
                case NODE.USAGE_AXIS:
                    edit = new AxisEditor(data, this.editable, this);
                    break;
                case NODE.USAGE_TEXT:
                    edit = new TextEditor(data, this.editable, this);
                    break;
                case NODE.USAGE_COMPOUND_DATA:
                case NODE.USAGE_DEVICE:
                default:
                    edit = new AnyEditor(data, this.editable, this);
            }
        }
        this.add(this.edit, BorderLayout.CENTER);
        if(node.isOn()) this.onoff.setText("Node is On   ");
        else this.onoff.setText("Node is Off  ");
        if(this.editable) this.setTitle("Modify data of " + node.getFullPath());
        else this.setTitle("Display data of " + node.getFullPath());
        this.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
        this.pack();
    }

    protected void reset() {
        // TODO Auto-generated method stub
    }
}
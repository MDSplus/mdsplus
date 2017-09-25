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

import java.awt.Color;
import javax.swing.JCheckBox;
import javax.swing.tree.DefaultMutableTreeNode;
import jtraverser.Node;
import jtraverser.TreeManager;
import mds.MdsException;
import mds.data.TREE;
import mds.data.descriptor_a.Int32Array;
import mds.data.descriptor_r.Action;
import mds.data.descriptor_s.NODE;
import mds.data.descriptor_s.Nid;

@SuppressWarnings("serial")
public class ActionList extends CheckBoxList{
    public ActionList(final TreeManager treeman){
        super(treeman, "Action Nodes in Tree");
    }

    @Override
    protected void apply() {
        for(int i = 0; i < this.checkboxes.size(); i++){
            final JCheckBox cb = this.checkboxes.getElementAt(i);
            if(cb.isSelected() != ((Boolean)cb.getClientProperty(CheckBoxList.PROP_OLD)).booleanValue()){
                final Nid nid = (Nid)cb.getClientProperty(CheckBoxList.PROP_NID);
                final DefaultMutableTreeNode treenode = this.treeview.findPath((String)cb.getClientProperty(CheckBoxList.PROP_FULLPATH));
                try{
                    nid.setOn(cb.isSelected());
                    if(treenode != null) ((Node)treenode.getUserObject()).setOnUnchecked();
                }catch(final MdsException me){
                    MdsException.stderr("ActionList.apply", me);
                }
            }
        }
        this.treeview.reportChange();
        this.update();
    }

    @Override
    public final void update() {
        super.update();
        if(this.checkboxes.size() > 0){
            final int[] nid = new int[this.checkboxes.size()];
            for(int i = 0; i < this.checkboxes.size(); i++)
                nid[i] = ((Nid)this.checkboxes.getElementAt(i).getClientProperty(CheckBoxList.PROP_NID)).getNidNumber();
            byte[] isoff;
            int[] status;
            try{
                final TREE tree = this.treeview.getTree();
                isoff = tree.mds.getByteArray(this.treeview.getTree().ctx, "_n=$;OR(GETNCI(_n,'PARENT_STATE'),GETNCI(_n,'STATE'))", new Int32Array(nid));
                status = tree.mds.getIntegerArray(this.treeview.getTree().ctx, "GETNCI($,'STATUS')", new Int32Array(nid));
                for(int i = 0; i < this.checkboxes.size(); i++){
                    final JCheckBox cb = this.checkboxes.getElementAt(i);
                    cb.setSelected(isoff[i] == 0);
                    cb.putClientProperty(CheckBoxList.PROP_OLD, Boolean.valueOf(isoff[i] == 0));
                    if(this.treeview.isModel()) continue;
                    cb.setToolTipText(Action.getStatusMsg(status[i]));
                    cb.setForeground((status[i] & 1) == 0 ? Color.RED : Color.BLACK);
                }
            }catch(final MdsException e){
                MdsException.stderr("ActionList.update", e);
            }
        }
        this.repaint();
    }

    @Override
    protected final void updatelist() {
        this.updatelist(NODE.USAGE_ACTION);
    }
}

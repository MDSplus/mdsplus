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

import javax.swing.JCheckBox;
import javax.swing.tree.DefaultMutableTreeNode;
import jtraverser.Node;
import jtraverser.TreeManager;
import jtraverser.TreeView;
import mds.MdsException;
import mds.data.descriptor_a.Int32Array;
import mds.data.descriptor_s.NODE;
import mds.data.descriptor_s.Nid;

@SuppressWarnings("serial")
public class SubTreeList extends CheckBoxList{
    public SubTreeList(final TreeManager treeman){
        super(treeman, "SubTrees include in Pulse");
    }

    @Override
    protected void apply() {
        final TreeView tree = SubTreeList.this.treeman.getCurrentTreeView();
        for(int i = 0; i < this.checkboxes.size(); i++){
            final JCheckBox cb = this.checkboxes.getElementAt(i);
            if(cb.isSelected() != ((Boolean)cb.getClientProperty(CheckBoxList.PROP_OLD)).booleanValue()){
                final Nid nid = (Nid)cb.getClientProperty(CheckBoxList.PROP_NID);
                final DefaultMutableTreeNode treenode = tree.findPath((String)cb.getClientProperty(CheckBoxList.PROP_FULLPATH));
                try{
                    if(cb.isSelected()) nid.setFlags(NODE.Flags.INCLUDE_IN_PULSE);
                    else nid.clearFlags(NODE.Flags.INCLUDE_IN_PULSE);
                    if(treenode != null) ((Node)treenode.getUserObject()).readFlags();
                }catch(final MdsException me){
                    MdsException.stderr("SubTreeList.apply", me);
                }
            }
        }
        tree.reportChange();
        this.update();
    }

    @Override
    public final void update() {
        super.update();
        if(this.treeview.getTree().mds.isLowLatency()){
            final int[] nid = new int[this.checkboxes.size()];
            for(int i = 0; i < this.checkboxes.size(); i++)
                nid[i] = ((Nid)this.checkboxes.getElementAt(i).getClientProperty(CheckBoxList.PROP_NID)).getNidNumber();
            try{
                final int[] flag = this.treeview.getTree().mds.getIntegerArray(this.treeview.getTree().ctx, "GETNCI($,'GET_FLAGS')", new Int32Array(nid));
                for(int i = 0; i < this.checkboxes.size(); i++)
                    this.checkboxes.getElementAt(i).setSelected((flag[i] & NODE.Flags.INCLUDE_IN_PULSE) != 0);
            }catch(final MdsException e){
                MdsException.stderr("SubTreeList.update", e);
            }
        }else for(int i = 0; i < this.checkboxes.size(); i++){
            final JCheckBox cb = this.checkboxes.getElementAt(i);
            final Nid nid = (Nid)cb.getClientProperty(CheckBoxList.PROP_NID);
            try{
                cb.setSelected((nid.getNciFlags() & NODE.Flags.INCLUDE_IN_PULSE) != 0);
            }catch(final MdsException e){/**/}
        }
        for(int i = 0; i < this.checkboxes.size(); i++)
            this.checkboxes.getElementAt(i).putClientProperty(CheckBoxList.PROP_OLD, Boolean.valueOf(this.checkboxes.getElementAt(i).isSelected()));
        this.repaint();
    }

    @Override
    protected final void updatelist() {
        this.updatelist(NODE.USAGE_SUBTREE);
    }
}
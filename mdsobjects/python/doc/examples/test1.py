# 
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

import os
from MDSobjects.tree import Tree
def test(quiet=False):
    """Exercise all of the readonly Tree and TreeNode methods.

    This test function will open trees from the Alcator C-Mod experiment.
    Environment variables are set for the C-Mod tree paths.

    This function will accept one boolean argument. If passed True the print
    output will be suppressed.
    """
    def Print(obj):
        ans=str(obj)
        if not quiet:
            print ans
    
    trees=('cmod','electrons','mhd','analysis','magnetics','xtomo','efit01','spectroscopy','video_daq','edge','engineering','hybrid','pcs','dpcs','particles','rf','dnb')
    for i in range(len(trees)):
	os.putenv(trees[i]+"_path","alcdata-archives.psfc.mit.edu::/cmod/trees/archives/~i~h/~g~f/~e~d/~t;alcdata-models.psfc.mit.edu::/cmod/trees/models/~t")
    Print("Beginning tree operations\n\n\n")
    pulse=Tree('cmod',1080326005,'Readonly')
    Print(pulse)
    model=Tree('cmod',-1,'Readonly')
    Print(model)
    ip=pulse.getNode('\\ip')
    Print(ip)
    pulse.setDefault(ip)
    Print(pulse.getDefault())
    Print(model.getDefault())
    members=model.getNodeWild(':*')
    Print(map(lambda i:str(members[i]),range(len(members))))
    children=model.getNodeWild('.*')
    Print(map(lambda i:str(children[i]),range(len(children))))
    Print("\n\n\nBeginning treenode operations\n\n\n")
    Print(ip.record)
    Print(ip.data())
    Print(ip.brother)
    Print(ip.parent.child)
    Print(ip.parent.parent.children_nids)
    Print(ip.mclass)
    Print(ip.class_str)
    Print(ip.compressible)
    Print(ip.compress_on_put)
    Print(ip.conglomerate_elt)
    Print(ip.conglomerate_nids)
    Print(ip.data_in_nci)
    Print(ip.parent.descendants)
    Print(ip.depth)
    Print(ip.disabled)
    Print(ip.do_not_compress)
    Print(ip.dtype)
    Print(ip.dtype_str)
    Print(ip.essential)
    Print(ip.fullpath)
    Print(pulse.getNode('\\top').include_in_pulse)
    Print(ip.is_child)
    Print(ip.is_member)
    Print(ip.length)
    Print(ip.parent.member)
    Print(ip.parent.member_nids)
    Print(ip.minpath)
    Print(ip.nid_number)
    Print(ip.nid_reference)
    Print(ip.node_name)
    Print(ip.no_write_model)
    Print(ip.no_write_shot)
    Print(ip.parent.parent.number_of_children)
    Print(ip.parent.parent.number_of_descendants)
    Print(ip.number_of_elts)
    Print(ip.parent.number_of_members)
    Print(ip.on)
    Print(ip.original_part_name)
    Print(ip.owner_id)
    Print(ip.parent)
    Print(ip.parent_disabled)
    Print(ip.path)
    Print(ip.path_reference)
    Print(ip.record)
    Print(ip.rfa)
    Print(ip.rlength)
    Print(ip.segmented)
    Print(ip.setup_information)
    Print(ip.status)
    Print(ip.tags)
    Print(ip.time_inserted.date)
    Print(ip.usage)
    Print(ip.versions)
    Print(ip.write_once)
    Print(ip.getClass())
    Print(ip.isCompressOnPut())
    Print(ip.getConglomerateElt())
    Print(ip.getNumElts())
    Print(ip.getConglomerateNodes())
    Print(ip.getOriginalPartName())
    Print(ip.getDtype())
    Print(ip.isEssential())
    Print(ip.getFullPath())
    Print(ip.getMinPath())
    Print(ip.getPath())
    Print(ip.getNodeName())
    Print(ip.isIncludeInPulse())
    Print(ip.getDepth())
    Print(ip.isChild())
    Print(ip.parent.getChild())
    Print(ip.parent.getNumChildren())
    Print(ip.parent.getChildren())
    Print(ip.isMember())
    Print(ip.getMember())
    Print(ip.parent.getMembers())
    Print(ip.parent.getNumDescendants())
    Print(ip.parent.getDescendants())
    Print(ip.getParent())
    Print(ip.getBrother())
    Print(ip.getLength())
    Print(ip.getCompressedLength())
    Print(ip.getNid())
    Print(ip.isNoWriteModel())
    Print(ip.isNoWriteShot())
    Print(ip.isWriteOnce())
    Print(ip.isOn())
    Print(ip.getOwnerId())
    Print(ip.getData())
    Print(ip.getTimeInserted().date)
    Print(ip.isSetup())
    Print(ip.getStatus())
    Print(ip.getUsage())
    Print(ip.getTags())
    Print(ip.containsVersions())
    Print(ip.isSegmented())
    Print(ip.getNumSegments())

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

from MDSplus import Tree,DATA,tcl
import os,sys
def doDir(self):
    if len(self.path_parts) > 2:
        tree = self.openTree(self.path_parts[1],self.path_parts[2])
    else: return (("500 BAD REQUEST", [('Content-type','text/plain')], "Missing expt and/or shot"))
    req_uri= self.environ["REQUEST_URI"]
    post   = self.environ["PATH_INFO"]
    root   = req_uri[:(-len(post))]
    pre1da = '/'.join([root,'1darray']+self.path_parts[1:3])
    topstr = "\\%s::TOP"%(tree.tree,)
    if len(self.path_parts)>3 and len(self.path_parts[3])>0:
       node = tree.getNode(":".join(self.path_parts[3:]))
       pretop = '/'.join([root,'dir']+self.path_parts[1:3])
       body = [
           '<a href="%s"> %s</a>'%(pretop,topstr),
           '<a href="%s"> up</a>'%os.path.dirname(req_uri)
       ]
    else:
       node = tree.top
       body = [topstr]
    predir = '%s/'%self.path_parts[-1] if req_uri[-1] != "/" else ""     
    title  = node.fullpath
    for subnode in node.descendants:
        nn_hi = subnode.node_name
        nn_lo = nn_hi.lower()
        record = '<a href="%s?expr=DECOMPILE(`%s)"> RECORD</a>'%(pre1da,subnode.path) if subnode.length>0 else "RECORD"
        link   = '<a href="%s%s"> %s</a>'%(predir,nn_lo,nn_hi) if subnode.number_of_descendants>0 else nn_hi
        body.append('%s %s'%(record,link))
    body = '<br/>'.join(body)
    output = '<!DOCTYPE html>\n<title>%s</title>\n<html>\n<body>\n%s\n</body>\n</html>'%(title,body)
    return ('200 OK', [('Content-type','text/html')], output)

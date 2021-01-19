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

from MDSplus import Tree, DATA, tcl
import os
import sys

example = "/dir/%s/-1/" % os.environ.get("EXPT", "main")


def toplink(self, root, tree, args):
    node = tree.top
    path = node.fullpath
    href = '/'.join([root, 'dir']+self.path_parts[1:3])
    parts = [self.link(href+args, path)]
    lenp = len(path)
    for i in range(3, len(self.path_parts)):
        node = node.getNode(self.path_parts[i])
        text = node.fullpath[lenp:]
        lenp += len(text)
        if len(self.path_parts) <= i+1 or len(self.path_parts[i+1]) == 0:
            parts.append(text)
            break
        href = '%s/%s' % (href, self.path_parts[i])
        parts.append(self.link(href+args, text))
    return ''.join(parts)


def doDir(self):
    if len(self.path_parts) > 2:
        tree = self.openTree(self.path_parts[1], self.path_parts[2])
    else:
        raise self.Exception("Missing expt and/or shot")
    args = "?%s" % ("&".join(["=".join([k, v[-1]])
                              for k, v in self.args.items()]),) if len(self.args) > 0 else ""
    req_uri = self.getReqURI()
    post = self.environ["PATH_INFO"]
    root = req_uri[:(-len(post))]
    predo = '/'.join([root, '%s']+self.path_parts[1:3])
    if len(self.path_parts) > 3 and len(self.path_parts[3]) > 0:
        node = tree.getNode(":".join(self.path_parts[3:]))
        title = node.fullpath
        top = toplink(self, root, tree, args)
    else:
        node = tree.top
        title = node.fullpath
        top = title
    predir = '%s/' % self.path_parts[-1] if req_uri[-1] != "/" else ""
    rows = []
    bit = self.args.get("bit", ["8"])[-1]
    for subnode in node.descendants:
        nn_hi = subnode.node_name
        nn_lo = nn_hi.lower()
        record = self.link('%s?expr=DECOMPILE(`GETNCI(%d,&quot;RECORD&quot;))' % (
            predo % '1darray', subnode.nid), 'RECORD') if subnode.length > 0 else '&lt;NODATA&gt;'
        data = self.link('%s?expr=DECOMPILE(`DATA(GETNCI(%d,&quot;RECORD&quot;)))' % (
            predo % '1darray', subnode.nid), 'DATA($)') if subnode.length > 0 else '&lt;NODATA&gt;'
        image = self.link('%s?expr=%s&idx=0&bit=%s' % (
            predo % 'image', subnode.fullpath, bit), 'IMAGE($)') if subnode.length > 0 else '&lt;NODATA&gt;'
        plot = self.link('%s?expr=%s' % (predo % 'plot', subnode.fullpath),
                         'PLOT($)') if subnode.length > 0 else '&lt;NODATA&gt;'
        link = self.link(predir+nn_lo+args,
                         nn_hi) if subnode.number_of_descendants > 0 else nn_hi
        rows.append((link, subnode.usage, record, data, plot, image))
    body = '%s<hr>%s' % (top, self.table_frame(
        ('name', 'usage', 'record', 'data', 'plot', 'image'), rows))
    output = self.html_frame(title, body)
    return ('200 OK', [('Content-type', 'text/html')], output)

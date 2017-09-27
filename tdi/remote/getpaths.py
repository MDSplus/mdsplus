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

def getpaths(tree):
    """Server side support of getpaths(host,tree) to fetch tree path definitions from data
       server."""
    from MDSplus import Tree,Data,StringArray
    import os
    import socket

    def fixpath(path):
        plist=path.split(';')
        if len(plist) > 1:
            for i in range(len(plist)):
                plist[i]=fixpath(plist[i])
            path=';'.join(plist)
        else:
            parts=path.split(':')
            if len(parts) == 1:
                path="%s::%s" % (socket.gethostname(),path)
            else:
                parts[0]=socket.gethostbyaddr(parts[0])[0]
                path=':'.join(parts)
        return path

    trees=[]
    paths=[]
    try:
        t=Tree(tree,-1)
        treenames=t.findTags('top')
        for treename in treenames:
            try:
                path=os.environ[treename[1:-5].lower()+'_path']
                trees.append(treename[1:-5])
                paths.append(fixpath(path))
            except:
                pass
    except:
        pass
    ans={'tree':StringArray(trees),'path':StringArray(paths)}
    return Data.makeData(ans).serialize()

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

from MDSplus import TdiCompile, TreeNode
import os
import sys
import numpy

example = '/image/%s/-1?expr=ADD(ZERO([100,100],0WU),2000WU)&bit=12' % os.environ.get(
    'EXPT', 'main')


def doImage(self):
    if len(self.path_parts) > 2:
        tree = self.openTree(self.path_parts[1], self.path_parts[2])
        _tdi = tree.tdiCompile
    else:
        _tdi = TdiCompile
    expr = self.args['expr'][-1]
    obj = _tdi(expr)
    idx = int(self.args['idx'][-1]) if 'idx' in self.args else 0
    if isinstance(obj, TreeNode) and obj.getNumSegments() > 0:
        d = obj.getSegment(idx)
        isseg = True
    else:
        d = obj.evaluate()
        isseg = False
    try:
        im = d.getImage()
    except:
        from PIL import Image
        import io
        raw = d.data()
        if 'bit' in self.args:
            bit = 8-int(self.args['bit'][-1])
            if bit != 0:
                if raw.itemsize == 1:
                    raw = raw.astype('uint16')
                if bit > 0:
                    raw = (((raw+1) << (bit))-1).astype('uint8')
                elif bit < 0:
                    raw = (((raw-1) >> (-bit))+1).astype('uint8')
        else:
            raw.astype("uint8")
        if raw.ndim > 2 and ((not isseg) or raw.shape[0]):
            raw = raw[0] if isseg else raw[idx]
        if raw.ndim == 2:
            img = Image.new("L", raw.T.shape, "gray")
        elif raw.ndim == 3:
            img = Image.new("RGB", raw.T.shape[:2])
            raw = numpy.rollaxis(raw, 0, 3)
        else:
            raise
        fmt = self.args['format'][-1].lower() if 'format' in self.args else 'jpeg'
        img.frombytes(raw.tostring())
        stream = io.BytesIO()
        img.save(stream, format=fmt.upper())
        return ('200 OK', [('Content-type', 'image/%s' % fmt)], stream.getvalue())
    else:
        if im.format == "MPEG":
            response_headers = [('Content-type', 'video/mpeg'),
                                ('Content-Disposition', 'inline; filename="%s.mpeg"' % (expr,))]
        else:  # covers gif, jpeg, and png
            fmt = im.format.lower()
            response_headers = [('Content-type', 'image/%s' % fmt),
                                ('Content-Disposition', 'inline; filename="%s.%s"' % (expr, fmt))]
    output = str(d.data().data)
    status = '200 OK'
    return (status, response_headers, output)

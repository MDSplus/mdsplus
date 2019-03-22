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

from MDSplus import TdiCompile,TreeNode
import sys,numpy

def doImage(self):
    if len(self.path_parts) > 2:
        tree = self.openTree(self.path_parts[1],self.path_parts[2])
        _tdi = tree.tdiCompile
    else:
        _tdi = TdiCompile
    expr=self.args['expr'][-1]
    try:
        obj = _tdi(expr)
    except:
        import traceback
        raise Exception("Error evaluating expression: '%s', error: %s" % (expr,traceback.format_exc()))
    try:
        if 'idx' in self.args and isinstance(obj,TreeNode) and obj.getNumSegments()>0:
            i = int(self.args['idx'][-1])
            d = obj.getSegment(i)
        else:
            d = obj.evaluate()
        try:
            im = d.getImage()
        except:
            from PIL import Image
            import io
            raw = d.data()
            if 'bit' in self.args:
                bit = int(self.args['bit'][-1])
                raw.astype("uint32")
                raw = (raw>>(raw.itemsize*8-bit)).astype('uint8')
            else:
                raw.astype("uint8")
            if raw.ndim==3 and raw.shape[0] == 1:
                raw = raw[0]
            if raw.ndim==2:
                img = Image.new("L",raw.T.shape,"gray")
            elif raw.ndim==3:
                img = Image.new("RGB",raw.T.shape[:2])
                raw = numpy.rollaxis(raw,0,3)
            else: raise
            img.frombytes(raw.tostring())
            stream = io.BytesIO()
            img.save(stream, format='PNG')
            return ('200 OK', [('Content-type','image/png')], stream.getvalue())
        else:
            if im.format == "MPEG":
                response_headers=[('Content-type','video/mpeg'),('Content-Disposition','inline; filename="%s.mpeg"' % (expr,))]
            else: # covers gif, jpeg, and png
                fmt = im.format.lower()
                response_headers=[('Content-type','image/%s'%fmt),('Content-Disposition','inline; filename="%s.%s"' % (expr,fmt))]
        output=str(d.data().data)
    except:
        import traceback
        raise Exception("Error getting image: '%s', error: %s" % (str(obj),traceback.format_exc()))
    status = '200 OK'
    return (status, response_headers, output)

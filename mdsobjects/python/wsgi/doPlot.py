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

from MDSplus import tdi,version,String,Descriptor_xd
import os,sys,ctypes,numpy,struct

example = '/plot?expr=make_signal(sin((0:1:.01)*$2pi),*,0:1:.01)'

_MdsMisc = version.load_library('MdsMisc')
def getXYSignal(expr,num):
    _MdsMisc.GetXYSignal.restype = ctypes.POINTER(Descriptor_xd._structure_class)
    y,x = '`(__misc__=(%s;);)'%expr,'`DIM_OF(__misc__)'
    cnum = ctypes.c_int32(num)
    cmin = ctypes.c_float(-float('inf'))
    cmax = ctypes.c_float( float('inf'))
    xd   = _MdsMisc.GetXYSignal(ctypes.c_char_p(y),ctypes.c_char_p(x),ctypes.byref(cmin),ctypes.byref(cmax),ctypes.byref(cnum))
    d    = Descriptor_xd(xd.contents).value
    if isinstance(d,String): raise Exception(d.data())
    bt  = d.data().tostring()
    off = 0
    fmt = '!fib'
    res,length,typ = struct.unpack_from(fmt,bt,off)
    off += struct.calcsize(fmt)
    fmt = '!'+('f'*length)
    y = numpy.array(struct.unpack_from(fmt,bt,off),'float32')
    off += struct.calcsize(fmt)
    if   typ == 1: # int64
        typ = 'int64'
        fmt = '!'+('q'*length)
    elif typ == 2: # float64
        typ = 'float64'
        fmt = '!'+('d'*length)
    else:          # float32
        typ = 'float32'
        #fmt = '!'+('f'*length)
    x = numpy.array(struct.unpack_from(fmt,bt,off),typ)
    title = xlabel = ylabel = ""
    off += struct.calcsize(fmt)
    fmt = '!I'
    llen = struct.unpack_from(fmt,bt,off)[0]
    off += struct.calcsize(fmt)+llen
    title = bt[off-llen:off]
    llen = struct.unpack_from(fmt,bt,off)[0]
    off += struct.calcsize(fmt)+llen
    ylabel = bt[off-llen:off]
    llen = struct.unpack_from(fmt,bt,off)[0]
    off += struct.calcsize(fmt)+llen
    xlabel = bt[off-llen:off]
    return length,x,y,title,xlabel,ylabel

def gridScaling(min_in,max_in,divisions,span):
    from math import log10,pow,fabs,ceil,floor,modf
    resdivlog = log10(fabs(max_in-min_in))
    if resdivlog < 0:
        res = int(pow(10,ceil(resdivlog) - 4))
    else:
        res = int(pow(10,floor(resdivlog) - 4))
    divlog = log10(fabs(max_in - min_in) / divisions)
    fractpart,wholepart = modf(divlog)
    intinc = pow(10,fabs(fractpart))
    if intinc < 1.3333333:
        intinc=1
    elif intinc < 2.857:
        intinc=2
    else:
        intinc=10
    if divlog < 0:
        val_inc = pow(10,-log10(intinc) + wholepart)
    else:
        val_inc = pow(10,log10(intinc) + wholepart)
    grid = ceil(min_in/val_inc) * val_inc
    divs_out = int(floor((max_in - grid + val_inc) / val_inc))
    if max_in > min_in:
        first_pix = span * ((grid - min_in) / (max_in - min_in))
        pix_inc = span * val_inc / (max_in - min_in);
        first_val = (first_pix * ((max_in - min_in)/span)) + min_in
    else:
        divs_out = 2
        first_pix = 0
        pix_inc = span
        val_inc = 0
    return res,(first_pix, pix_inc),(first_val,val_inc),divs_out       

def doPlot(self):
    viewbox = (0,0,10000,10000);
    expr = self.args['expr'][-1]
    if len(self.path_parts) > 2:
        tdi('treeopen("%s",%s)'%tuple(self.path_parts[1:3]))
    try:
        length,x,y,title,xlabel,ylabel = getXYSignal(expr,viewbox[2])
    finally:
        if len(self.path_parts) > 2:
            tdi('treeclose("%s",%s)'%tuple(self.path_parts[1:3]))
    response_headers = [
        ('Cache-Control','no-store, no-cache, must-revalidate'),
        ('Pragma','no-cache'),
        ('XDTYPE',x.__class__.__name__),
        ('YDTYPE',y.__class__.__name__),
        ('XLENGTH',str(length)),
        ('YLENGTH',str(length)),
        ('Content-type','text/xml'),
    ]
    if len(self.path_parts) > 2:
        response_headers.append(('TREE',self.path_parts[1].upper()))
        response_headers.append(('SHOT',self.path_parts[2]))
    output  = '<?xml version="1.0" standalone="no"?>\n'
    output += '<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">\n'
    output += '<svg viewBox="%d %d %d %d" xmlns="http://www.w3.org/2000/svg" version="1.1">\n' % viewbox
    output += '<title>Plot of %s</title>\n' % (expr,)
    output += '<path fill="none" stroke="black" stroke-width="30" d="M\n'
    xmin,  xmax   = float(min(x)),float(max(x))
    ymin,  ymax   = float(min(y)),float(max(y))
    xmin_s,ymin_s = float(viewbox[0]),float(viewbox[1])
    xmax_s,ymax_s = float(viewbox[2]),float(viewbox[3])
    # scale
    x =  (x-xmin)*((xmax_s-xmin_s-1200)/(xmax-xmin))+1000
    y =  (ymax-y)*((ymax_s-ymin_s-1200)/(ymax-ymin))+200
    for i in range(len(x)): output += "%d %d\n" % (int(x[i]),int(y[i]))
    output += '"/>\n'
    res,pix,val,div = gridScaling(xmin,xmax,5,viewbox[2]-viewbox[0]-1200)
    for i in range(div):
        output += '<text text-anchor="middle" x="%d" y="%d" font-size="300">%g</text>\n' % (
            pix[0]+i*pix[1]+1000, viewbox[2]-200, val[0]+i*val[1])
        output += '<path fill="none" stroke="black" stroke-width="5" stroke-dasharray="200,100,50,50,50,100" d="M%d %d %d %d"/>\n' % ( 
            pix[0]+i*pix[1]+1000, viewbox[2],
            pix[0]+i*pix[1]+1000, viewbox[0])   
    res,pix,val,div = gridScaling(ymin,ymax,5,viewbox[3]-viewbox[1]-1200)
    for i in range(div):
        output += '<text text-anchor="left" x="%d" y="%d" font-size="300">%g</text>\n' % (
            100, viewbox[3]-(pix[0]+i*pix[1]+1000), val[0]+i*val[1])
        output += '<path fill="none" stroke="black" stroke-width="5" stroke-dasharray="200,100,50,50,50,100" d="M%d %d %d %d"/>\n' % (
            viewbox[1], viewbox[3]-(pix[0]+i*pix[1]+1000),
            viewbox[3], viewbox[3]-(pix[0]+i*pix[1]+1000) )
    output += '</svg>\n'
    status = '200 OK'
    return (status, response_headers, output)

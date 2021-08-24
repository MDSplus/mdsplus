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

from MDSplus import Data, TdiCompile, version, String, DescriptorXD
import os
import sys
import ctypes
import numpy
import struct

example = '/plot?expr=make_param(make_signal(make_with_units(sin((0:1:.01)*$2pi),"ylabel"),,make_with_units(0:1:.01,"xlabel")),"title",)'


def getXYSignal(obj, num):
    sig = obj.getXYSignal(num=num)
    dim = sig.dim_of()
    try:
        title = str(sig.help)
    except:
        title = None
    try:
        ylabel = str(sig.units)
    except:
        ylabel = None
    try:
        xlabel = str(dim.units)
    except:
        xlabel = None
    return dim.data(), sig.data(), title, ylabel, xlabel


def gridScaling(min_in, max_in, divisions, span):
    from math import log10, pow, fabs, ceil, floor, modf
    resdivlog = log10(max(fabs(max_in), fabs(min_in)))
    if resdivlog < 0:
        res = int(ceil(resdivlog)) + 5
    else:
        res = int(floor(resdivlog)) + 5
    divlog = log10(fabs(max_in-min_in) / divisions)
    fractpart, wholepart = modf(divlog)
    intinc = pow(10, fabs(fractpart))
    if intinc < 1.3333333:
        intinc = 1
    elif intinc < 2.857:
        intinc = 2
    else:
        intinc = 10
    if divlog < 0:
        val_inc = pow(10, -log10(intinc) + wholepart)
    else:
        val_inc = pow(10, log10(intinc) + wholepart)
    grid = ceil(min_in/val_inc) * val_inc
    divs_out = int(floor((max_in - grid + val_inc) / val_inc))
    if max_in > min_in:
        first_pix = span * ((grid - min_in) / (max_in - min_in))
        pix_inc = span * val_inc / (max_in - min_in)
        first_val = numpy.round(
            first_pix * ((max_in - min_in)/span) + min_in, res)
        val_inc = numpy.round(val_inc, res)
    else:
        divs_out = 2
        first_pix = 0
        pix_inc = span
        val_inc = 0
        first_val = max_in
    return (first_pix, pix_inc), (first_val, val_inc), divs_out


def doPlot(self):
    expr = self.args['expr'][-1]
    if len(self.path_parts) > 2:
        tree = self.openTree(self.path_parts[1], self.path_parts[2])
        obj = tree.tdiCompile(expr)
    else:
        tree = None
        obj = TdiCompile(expr)
    viewbox = (0, 0, 10000, 10000)    # x0,y0,x1,y1
    x, y, title, ylabel, xlabel = getXYSignal(obj, viewbox[2])
    margin = [1000, 1000, 200, 1000]  # x0,y0,x1,y1
    if title is None:
        margin[1] = 200
    response_headers = [
        ('Cache-Control', 'no-store, no-cache, must-revalidate'),
        ('Pragma', 'no-cache'),
        ('XDTYPE', x.__class__.__name__),
        ('YDTYPE', y.__class__.__name__),
        ('XLENGTH', str(x.shape[0])),
        ('YLENGTH', str(y.shape[0])),
        ('Content-type', 'text/xml'),
    ]
    if tree is not None:
        response_headers.append(('TREE', tree.tree))
        response_headers.append(('SHOT', str(tree.shot)))
    output = '<?xml version="1.0" standalone="no"?>\n'
    output += '<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">\n'
    output += '<svg viewBox="%d %d %d %d" xmlns="http://www.w3.org/2000/svg" version="1.1">\n' % viewbox
    output += '<title>Plot of %s</title>\n' % (expr,)
    if title is not None:
        output += '<text text-anchor="middle" x="%d" y="%d" font-size="400">%s</text>\n' % (
            (viewbox[2]-viewbox[0])/2, 500, title)
    if xlabel is not None:
        output += '<text text-anchor="middle" x="%d" y="%d" font-size="300">%s</text>\n' % (
            (viewbox[2]-viewbox[0])/2, viewbox[3]-100, xlabel)
    if ylabel is not None:
        output += '<text transform="rotate(-90)" style="text-anchor:middle;" x="%d" y="%d" font-size="300">%s</text>\n' % (
            (viewbox[1]-viewbox[3])/2, 100, ylabel)
    output += '<path fill="none" stroke="black" stroke-width="30" d="M\n'
    xmin, xmax = float(min(x)), float(max(x))
    ymin, ymax = float(min(y)), float(max(y))
    xvspan, xmspan = viewbox[2]-viewbox[0], margin[2]+margin[0]
    yvspan, ymspan = viewbox[3]-viewbox[1], margin[3]+margin[1]
    # scale
    x = (x-xmin)*((xvspan-xmspan)/(xmax-xmin))+margin[0]
    y = (ymax-y)*((yvspan-ymspan)/(ymax-ymin))+margin[1]
    for i in range(len(x)):
        output += "%d %d\n" % (int(x[i]), int(y[i]))
    output += '"/>\n'
    pix, val, div = gridScaling(xmin, xmax, 5, xvspan-xmspan)
    for i in range(div):
        output += '<text text-anchor="middle" x="%d" y="%d" font-size="300">%.5g</text>\n' % (
            pix[0]+i*pix[1]+margin[0], viewbox[2]-margin[2]-200, val[0]+i*val[1])
        output += '<path fill="none" stroke="black" stroke-width="5" stroke-dasharray="200,100,50,50,50,100" d="M%d %d %d %d"/>\n' % (
            pix[0]+i*pix[1]+margin[0], viewbox[2],
            pix[0]+i*pix[1]+margin[0], viewbox[0])
    pix, val, div = gridScaling(ymin, ymax, 5, yvspan-ymspan)
    for i in range(div):
        output += '<text text-anchor="left" x="%d" y="%d" font-size="300">%.5g</text>\n' % (
            300, viewbox[3]-(pix[0]+i*pix[1]+margin[3]), val[0]+i*val[1])
        output += '<path fill="none" stroke="black" stroke-width="5" stroke-dasharray="200,100,50,50,50,100" d="M%d %d %d %d"/>\n' % (
            viewbox[1], viewbox[3]-(pix[0]+i*pix[1]+margin[3]),
            viewbox[3], viewbox[3]-(pix[0]+i*pix[1]+margin[3]))
    output += '</svg>\n'
    status = '200 OK'
    return (status, response_headers, output)

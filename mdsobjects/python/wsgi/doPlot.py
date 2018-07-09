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

from MDSplus import Data,makeData
import sys

def doPlot(self):

    viewbox=[0,0,10000,10000];
    def scale(xmin,ymin,xmax,ymax,xmin_s,ymin_s,xmax_s,ymax_s,x,y):
        px=(x-xmin)*(xmax_s-xmin_s)/(xmax-xmin)
        py=(y-ymin)*(ymax_s-ymin_s)/(ymax-ymin)
        return " %d %d" % (int(round(px)),int(round(py)))

    def gridScaling(min_in,max_in,divisions,span):
        from math import log10,pow,fabs,ceil,floor,modf
        resdivlog = log10(fabs(max_in-min_in))
        if resdivlog < 0:
            res = pow(10,ceil(resdivlog) - 4)
        else:
            res = pow(10,floor(resdivlog) - 4)
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
        grid=ceil(min_in/val_inc) * val_inc
        divs_out = floor((max_in - grid + val_inc) / val_inc)
        if max_in > min_in:
            first_pix = span * ((grid - min_in) / (max_in - min_in))
            pix_inc = span * val_inc / (max_in - min_in);
            first_val = (first_pix * ((max_in - min_in)/span)) + min_in
        else:
            divs_out = 2
            first_pix = 0
            pix_inc = span
            val_inc = 0
        return {"divisions":divs_out,
                "first_pixel":first_pix, "pixel_increment":pix_inc, 
                "first_value":first_val, "value_increment":val_inc,
                "resolution":res}
          
    if len(self.path_parts) > 2:
        self.openTree(self.path_parts[1],self.path_parts[2])
    expr=self.args['expr'][-1]
    try:
        sig=Data.execute(expr)
        y=makeData(sig.data()).data()
        x=makeData(sig.dim_of().data()).data()
    except Exception:
        raise Exception("Error evaluating expression: '%s', error: %s" % (expr,sys.get_info()))
    response_headers=list()
    response_headers.append(('Cache-Control','no-store, no-cache, must-revalidate'))
    response_headers.append(('Pragma','no-cache'))
    response_headers.append(('XDTYPE',x.__class__.__name__))
    response_headers.append(('YDTYPE',y.__class__.__name__))
    response_headers.append(('XLENGTH',str(len(x))))
    response_headers.append(('YLENGTH',str(len(y))))
    response_headers.append(('Content-type','text/xml'))
    if self.tree is not None:
        response_headers.append(('TREE',self.tree))
        response_headers.append(('SHOT',self.shot))
        
    output="""<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg viewBox="%d %d %d %d"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
""" % (viewbox[0],viewbox[1],viewbox[2],viewbox[3])
    output += "<title>Plot of %s </title>" % (expr,)
    output += """
  <path fill="none" stroke="black" stroke-width="30" d="M """
    xmin=float(min(x))
    xmax=float(max(x))
    ymin=float(min(y))
    ymax=float(max(y))
    xmin_s=float(viewbox[0])
    ymin_s=float(viewbox[1])
    xmax_s=float(viewbox[2])
    ymax_s=float(viewbox[3])
    for i in range(len(x)):
        output += scale(xmin-(xmax-xmin)*.1,
                        ymin-(ymax-ymin)*.1,
                        xmax+(xmax-xmin)*.1,
                        ymax+(ymax-ymin)*.1,xmin_s,ymin_s,xmax_s,ymax_s,x[i],y[i])
    output +=""" " />
"""
    scaling=gridScaling(xmin,xmax,5,viewbox[2]-viewbox[0])
    for i in range(scaling['divisions']):
        output +="""<text text-anchor="middle" x="%d" y="%d" font-size="300" >%g</text>
<path fill="none" stroke="black" stroke-width="5" stroke-dasharray="200,100,50,50,50,100" d="M%d %d %d %d" />
""" % (scaling["first_pixel"]+i*scaling["pixel_increment"],
       viewbox[2]-200,
       round(scaling['first_value']+i*scaling["value_increment"],scaling["resolution"]),
       scaling["first_pixel"]+i*scaling["pixel_increment"],
       viewbox[2],
       scaling["first_pixel"]+i*scaling["pixel_increment"],
       viewbox[0])
        
    scaling=gridScaling(ymin,ymax,5,viewbox[3]-viewbox[1])
    for i in range(scaling['divisions']):
        output +="""<text text-anchor="left" x="%d" y="%d" font-size="300" >%g</text>
<path fill="none" stroke="black" stroke-width="5" stroke-dasharray="200,100,50,50,50,100" d="M%d %d %d %d" />
""" % (100,
       scaling["first_pixel"]+i*scaling["pixel_increment"],
       round(scaling['first_value']+i*scaling["value_increment"],scaling["resolution"]),
       0,
       scaling["first_pixel"]+i*scaling["pixel_increment"],
       10000,
       scaling["first_pixel"]+i*scaling["pixel_increment"])
    output +="""</svg>
"""
    status = '200 OK'
    return (status, response_headers, output)

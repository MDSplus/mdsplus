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

from MDSplus import makeData,Data,Tree,Float64Array
import sys

def doFrame(self):
    def getStringExp(self,name,response_headers):
        if name in self.args:
            try:
                response_headers.append((name,str(Data.execute(self.args[name][-1]).data())))
            except Exception:
                response_headers.append((name,str(sys.exc_info())))

    response_headers = list()
    response_headers.append(('Cache-Control','no-store, no-cache, must-revalidate'))
    response_headers.append(('Pragma','no-cache'))
    response_headers.append(('Content-Type','application/octet-stream'))

    if 'tree' in self.args:
        Tree.usePrivateCtx()
        try:
            t = Tree(self.args['tree'][-1],int(self.args['shot'][-1].split(',')[0]))
        except:
            response_headers.append(('ERROR','Error opening tree'))

    for name in ('title','xlabel','ylabel'):
        getStringExp(self,name,response_headers)

    if 'frame_idx' in self.args:
        frame_idx = self.args['frame_idx'][-1]
    else:
        frame_idx = '0'

    expr = self.args['y'][-1]
    try:
        sig = Data.execute('GetSegment(' + expr + ',' + frame_idx + ')')
        frame_data = makeData(sig.data())
    except Exception:
        response_headers.append(('ERROR','Error evaluating expression: "%s", error: %s' % (expr,sys.exc_info())))

    if 'init' in self.args:
        if 'x' in self.args:
            expr = self.args['x'][-1]
            try:
                times = Data.execute(expr)
                times = makeData(times.data())
            except Exception:
                response_headers.append(('ERROR','Error evaluating expression: "%s", error: %s' % (expr,sys.exc_info())))
        else:
            try:
                #times = Data.execute('dim_of(' + expr + ')')
                times = list()
                numSegments = Data.execute('GetNumSegments(' + expr + ')').data()
                for i in range(0, numSegments):
                    times.append(Data.execute('GetSegmentLimits(' + expr + ',' + str(i) + ')').data()[0])
                times = Float64Array(times)
            except Exception:
                response_headers.append(('ERROR','Error getting x axis of: "%s", error: %s' % (expr,sys.exc_info())))
    
    response_headers.append(('FRAME_WIDTH',str(sig.getShape()[0])))
    response_headers.append(('FRAME_HEIGHT',str(sig.getShape()[1])))
    response_headers.append(('FRAME_BYTES_PER_PIXEL',str(frame_data.data().itemsize)))
    response_headers.append(('FRAME_LENGTH',str(len(frame_data))))

    output = str(frame_data.data().data)

    if 'init' in self.args:
        response_headers.append(('TIMES_DATATYPE',times.__class__.__name__))
        response_headers.append(('TIMES_LENGTH',str(len(times))))
        output = output + str(times.data().data)

    status = '200 OK'
    return (status, response_headers, output)

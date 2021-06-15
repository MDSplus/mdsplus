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

from MDSplus import DATA, Float64Array, tdi
import sys

example = '/frame?tree=expt&shot=123&y=SIGNAL:NODE&x=0.0&frame_idx=0'


def doFrame(self):
    def getStringExp(self, name, response_headers, _tdi):
        if name in self.args:
            try:
                response_headers.append(
                    (name, str(_tdi(self.args[name][-1]).data())))
            except Exception as e:
                response_headers.append((name, "ERROR: %s" % (e,)))

    response_headers = list()
    response_headers.append(
        ('Cache-Control', 'no-store, no-cache, must-revalidate'))
    response_headers.append(('Pragma', 'no-cache'))
    response_headers.append(('Content-Type', 'application/octet-stream'))

    if 'tree' in self.args:
        tree = self.openTree(
            self.args['tree'][-1], self.args['shot'][-1].split(',')[0])
        _tdi = tree.tdiExecute
    else:
        tree = None
        _tdi = tdi
    for name in ('title', 'xlabel', 'ylabel'):
        getStringExp(self, name, response_headers, _tdi)

    if 'frame_idx' in self.args:
        frame_idx = self.args['frame_idx'][-1]
    else:
        frame_idx = '0'

    expr = self.args['y'][-1]

    sig = _tdi('GetSegment(' + expr + ',' + frame_idx + ')')
    frame_data = DATA(sig).evaluate()

    response_headers.append(('FRAME_WIDTH', str(sig.getShape()[0])))
    response_headers.append(('FRAME_HEIGHT', str(sig.getShape()[1])))
    response_headers.append(
        ('FRAME_BYTES_PER_PIXEL', str(frame_data.data().itemsize)))
    response_headers.append(('FRAME_LENGTH', str(len(frame_data))))

    output = str(frame_data.data().data)

    if 'init' in self.args:
        if 'x' in self.args:
            expr = self.args['x'][-1]
            times = DATA(_tdi(expr)).evaulate()
        else:
            times = list()
            numSegments = _tdi('GetNumSegments(' + expr + ')').data()
            for i in range(0, numSegments):
                times.append(
                    _tdi('GetSegmentLimits(' + expr + ',' + str(i) + ')').data()[0])
            times = Float64Array(times)
        response_headers.append(('TIMES_DATATYPE', times.__class__.__name__))
        response_headers.append(('TIMES_LENGTH', str(len(times))))
        output = output + str(times.data().data)

    status = '200 OK'
    return (status, response_headers, output)

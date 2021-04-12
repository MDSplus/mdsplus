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

MARTE_GENERIC = __import__('MARTE_GENERIC', globals())


class MARTE_BREAKDOWN(MARTE_GENERIC.MARTE_GENERIC):
    params = [
        ('breakdownTrigIdx', 1),
        ('breakdownDeadTime', 10e-3),
        ('breakdownRiseTime', 20e-3),
        ('breakdownDeadOut1', 0),
        ('breakdownDeadOut2', 0),
        ('breakdownDeadOut3', 0),
        ('breakdownDeadOut4', 0),
    ]
    parts = MARTE_GENERIC.MARTE_GENERIC.parts + [
        {'path': '.PARAMS', 'type': 'structure'},
        {'path': '.PARAMS:NUM_ACTIVE',
            'type': 'numeric', 'value': len(params)},
    ]
    for i, par in enumerate(params):
        parts.extend([
            {'path': '.PARAMS:PAR_%03d' % (i+1), 'type': 'structure'},
            {'path': '.PARAMS:PAR_%03d:DESCRIPTION' % (i+1), 'type': 'text'},
            {'path': '.PARAMS:PAR_%03d:NAME' % (
                i+1), 'type': 'text', 'value': par[0]},
            {'path': '.PARAMS:PAR_%03d:TYPE' % (i+1), 'type': 'text'},
            {'path': '.PARAMS:PAR_%03d:DIMS' % (i+1), 'type': 'numeric'},
            {'path': '.PARAMS:PAR_%03d:DATA' % (
                i+1), 'type': 'numeric', 'value': par[1]},
        ])
    for i in range(i+1, 256):
        parts.extend([
            {'path': '.PARAMS:PAR_%03d' % (i+1), 'type': 'structure'},
            {'path': '.PARAMS:PAR_%03d:DESCRIPTION' % (i+1), 'type': 'text'},
            {'path': '.PARAMS:PAR_%03d:NAME' % (i+1), 'type': 'text'},
            {'path': '.PARAMS:PAR_%03d:TYPE' % (i+1), 'type': 'text'},
            {'path': '.PARAMS:PAR_%03d:DIMS' % (i+1), 'type': 'numeric'},
            {'path': '.PARAMS:PAR_%03d:DATA' % (i+1), 'type': 'numeric'},
        ])
    parts.append({'path': '.WAVE_PARAMS', 'type': 'structure'})
    parts.append({'path': '.WAVE_PARAMS:NUM_ACTIVE',
                  'type': 'numeric', 'value': 8})
    params = ['wave1', 'wave2', 'wave3', 'wave4',
              'riseWave1', 'riseWave2', 'riseWave3', 'riseWave4']
    for i, par in enumerate(params):
        parts.extend([
            {'path': '.WAVE_PARAMS:WAVE_%03d' % (i+1), 'type': 'structure'},
            {'path': '.WAVE_PARAMS:WAVE_%03d:DESCRIPTION' % (
                i+1), 'type': 'text'},
            {'path': '.WAVE_PARAMS:WAVE_%03d:NAME' % (
                i+1), 'type': 'text', 'value': par},
            {'path': '.WAVE_PARAMS:WAVE_%03d:X' % (
                i+1), 'type': 'numeric', 'valueExpr': 'Float32Array([0.,1.])'},
            {'path': '.WAVE_PARAMS:WAVE_%03d:Y' % (
                i+1), 'type': 'numeric', 'valueExpr': 'Float32Array([0.,0.])'},
        ])
    for i in range(i+1, 64):
        parts.extend([
            {'path': '.WAVE_PARAMS:WAVE_%03d' % (i+1), 'type': 'structure'},
            {'path': '.WAVE_PARAMS:WAVE_%03d:DESCRIPTION' % (
                i+1), 'type': 'text'},
            {'path': '.WAVE_PARAMS:WAVE_%03d:NAME' % (i+1), 'type': 'text'},
            {'path': '.WAVE_PARAMS:WAVE_%03d:X' % (i+1), 'type': 'numeric'},
            {'path': '.WAVE_PARAMS:WAVE_%03d:Y' % (i+1), 'type': 'numeric'},
        ])
    del(i, params, par)

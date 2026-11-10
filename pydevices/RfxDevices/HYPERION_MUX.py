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

"""Passive MDSplus model for up to ten simultaneous FBG wavelengths.

The optical topology may be a sensor chain or a optical switch mux.  The
HYPERION acquisition device reads the interrogator, matches detected peaks to
the nominal wavelengths stored here and writes the resulting signals.  This
model intentionally defines no actions.
"""

from MDSplus import Data, Device


class HYPERION_MUX(Device):
    """Configuration and storage for up to ten simultaneous FBG sensors."""

    parts = [
        {'path': ':NAME', 'type': 'text'},
        {'path': ':COMMENT', 'type': 'text'},
        {'path': ':TIME0', 'type': 'numeric', 'value': 0},
    ]

    for i in range(1, 11):
        parts.extend([
            {'path': '.INPUT_%02d' % i, 'type': 'structure'},
            {'path': '.INPUT_%02d:NOMINAL_WL' % i, 'type': 'numeric', 'value': 0.0},
            {'path': '.INPUT_%02d:PEAK' % i, 'type': 'signal'},
            {'path': '.INPUT_%02d:PEAK_RTIME' % i, 'type': 'numeric', 'valueExpr': 'Data.compile("pvResample($1,,,,$2)", head.input_%02d_peak, head.time0)' % (i)},
        ])
    del i

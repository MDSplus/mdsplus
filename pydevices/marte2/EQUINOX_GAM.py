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

MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('EquinoxGAM', MC.MARTE2_COMPONENT.MODE_GAM)
class EQUINOX_GAM(MC.MARTE2_COMPONENT):
    inputs = [
        {'name': 'ip', 'type': 'float64', 'dimensions': 0, 'parameters':[]},
        {'name': 'btvac0', 'type': 'float64', 'dimensions': 0, 'parameters':[]},
        {'name': 'psi', 'type': 'float64', 'dimensions': [216], 'parameters':[]},
        {'name': 'br', 'type': 'float64', 'dimensions': [216], 'parameters':[]},
        {'name': 'bz', 'type': 'float64', 'dimensions': [216], 'parameters':[]},
        {'name': 'neint_fir', 'type': 'float64', 'dimensions': [8], 'parameters':[]},
        {'name': 'far_fir', 'type': 'float64', 'dimensions': [8], 'parameters':[]},
    ]
    outputs = [
        {'name': 'necalc', 'type': 'float64', 'dimensions': [8], 'parameters':[]},
        {'name': 'necalc_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'neerr', 'type': 'float64', 'dimensions': [8], 'parameters':[]},
        {'name': 'neerr_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'ne_dofs', 'type': 'float64', 'dimensions': [21], 'parameters':[]},
        {'name': 'ne_dofs_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'anenow', 'type': 'float64', 'dimensions': [168], 'parameters':[]},
        {'name': 'anenow_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'flag_config', 'type': 'int32', 'dimensions': 0, 'parameters':[]},
        {'name': 'flag_config_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'flag_quality', 'type': 'int32', 'dimensions': 0, 'parameters':[]},
        {'name': 'flag_quality_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'flag_quality2', 'type': 'int32', 'dimensions': 0, 'parameters':[]},
        {'name': 'flag_quality2_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'flag_valid', 'type': 'int32', 'dimensions': 0, 'parameters':[]},
        {'name': 'flag_valid_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'flag_valid_axis', 'type': 'int32', 'dimensions': 0, 'parameters':[]},
        {'name': 'flag_valid_axis_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'psin', 'type': 'float64', 'dimensions': [2263], 'parameters':[]},
        {'name': 'psin_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'b0', 'type': 'float64', 'dimensions': 0, 'parameters':[]},
        {'name': 'b0_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'ip', 'type': 'float64', 'dimensions': 0, 'parameters':[]},
        {'name': 'ip_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'li', 'type': 'float64', 'dimensions': 0, 'parameters':[]},
        {'name': 'li_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'phip', 'type': 'float64', 'dimensions': 0, 'parameters':[]},
        {'name': 'phip_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'rax', 'type': 'float64', 'dimensions': 0, 'parameters':[]},
        {'name': 'rax_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'tot_volume', 'type': 'float64', 'dimensions': 0, 'parameters':[]},
        {'name': 'tot_volume_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'wp', 'type': 'float64', 'dimensions': 0, 'parameters':[]},
        {'name': 'wp_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'wpol', 'type': 'float64', 'dimensions': 0, 'parameters':[]},
        {'name': 'wpol_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'wtor', 'type': 'float64', 'dimensions': 0, 'parameters':[]},
        {'name': 'wtor_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'zax', 'type': 'float64', 'dimensions': 0, 'parameters':[]},
        {'name': 'zax_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'area', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'area_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'bp2', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'bp2_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'delta', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'delta_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'epsilon', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'epsilon_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'f', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'f_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'ffprime', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'ffprime_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'h', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'h_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'iota', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'iota_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'ir', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'ir_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'ir2', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'ir2_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'jphior', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'jphior_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'kappa', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'kappa_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'minrad', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'minrad_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'muoip', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'muoip_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'ne', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'ne_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'phi', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'phi_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'psin1d', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'psin1d_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'psi1d', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'psi1d_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'pprime', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'pprime_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'pressure', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'pressure_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'q', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'q_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'r2bp2', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'r2bp2_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'rbp', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'rbp_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'rgeom', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'rgeom_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'rhotorn', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'rhotorn_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'volume', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'volume_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
        {'name': 'zgeom', 'type': 'float64', 'dimensions': [17], 'parameters':[]},
        {'name': 'zgeom_flag', 'type': 'int32', 'dimensions': 0,'parameters':[]},
    ]
    parameters = [
        {'name':'Parameter1', 'type': 'float32'},
        {'name':'Parameter2', 'type': 'string'},
    ]
    parts = []

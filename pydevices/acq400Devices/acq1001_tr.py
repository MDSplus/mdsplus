#!/usr/bin/env python
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


try:
    acq400_base = __import__('acq400_base', globals(), level=1)
except:
    acq400_base = __import__('acq400_base', globals())


class _ACQ1001_TR(acq400_base._ACQ400_TR_BASE):
    """
    D-Tacq ACQ1001 transient support.
    """


class_ch_dict = acq400_base.create_classes(
    _ACQ1001_TR, "ACQ1001_TR",
    list(_ACQ1001_TR.base_parts),
    acq400_base.ACQ1001_CHANNEL_CHOICES
)
globals().update(class_ch_dict)

if __name__ == '__main__':
    acq400_base.print_generated_classes(class_ch_dict)
del(class_ch_dict)

# public classes created in this module
# ACQ1001_TR_8
# ACQ1001_TR_16
# ACQ1001_TR_24
# ACQ1001_TR_32
# ACQ1001_TR_40
# ACQ1001_TR_48
# ACQ1001_TR_64

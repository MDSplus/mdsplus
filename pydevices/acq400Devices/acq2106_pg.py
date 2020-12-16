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
    ACQ2106_WRPG = __import__('acq2106_WRPG', globals(), level=1)
except:
    ACQ2106_WRPG = __import__('acq2106_WRPG', globals())


class _ACQ2106_PG(ACQ2106_WRPG.ACQ2106_WRPG):
    """
    D-Tacq ACQ2106 PG support.
    """


class_ch_dict = ACQ2106_WRPG.create_classes(
    _ACQ2106_PG, "ACQ2106_PG",
    list(_ACQ2106_PG.base_parts),
    ACQ2106_WRPG.ACQ2106_CHANNEL_CHOICES
)
globals().update(class_ch_dict)

if __name__ == '__main__':
    ACQ2106_WRPG.print_generated_classes(class_ch_dict)
del(class_ch_dict)

# public classes created in this module
# ACQ2106_PG_4
# ACQ2106_PG_32


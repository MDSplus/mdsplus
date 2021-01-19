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

import MDSplus


def pyfun(fun, module=None, *args):
    MDSplus.DEALLOCATE('public _py_exception')
    fun = MDSplus.Data(fun).data()
    module = MDSplus.Data(module).data()

    def getfun(fun):
        fun = str(fun)
        if module is not None:
            return __import__(str(module), globals(), locals(), [fun]).__dict__[fun]
        builtins = __builtins__ if isinstance(
            __builtins__, dict) else __builtins__.__dict__
        if fun in builtins:
            return builtins[fun]
        try:
            return MDSplus.__dict__[fun]
        except:
            raise MDSplus.TdiUNKNOWN_VAR(fun)
    fun = getfun(fun)
    args = tuple(map(MDSplus.Data.evaluate, args))
    try:
        return fun(*args)
    except Exception as exc:
        import traceback
        traceback.print_exc()
        MDSplus.String(str(exc)).setTdiVar("_py_exception")

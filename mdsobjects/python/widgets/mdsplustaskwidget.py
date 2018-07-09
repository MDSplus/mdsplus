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

from gtk import VBox
from mdspluserrormsg import MDSplusErrorMsg
from mdsplusexpressionwidget import MDSplusExpressionWidget
from mdsplusroutinewidget import MDSplusRoutineWidget
from mdsplusmethodwidget import MDSplusMethodWidget
from mdsplusdtypeselwidget import MDSplusDtypeSelWidget


class MDSplusTaskWidget(MDSplusDtypeSelWidget,VBox):

    def __init__(self):
        super(MDSplusTaskWidget,self).__init__(homogeneous=False,spacing=10)
        menu=self.dtype_menu(("Method","Routine"))
        self.method=MDSplusMethodWidget()
        self.routine=MDSplusRoutineWidget()
        self.routine.set_no_show_all(True)
        self.expression=MDSplusExpressionWidget()
        self.expression.set_no_show_all(True)
        self.widgets=(self.method,self.routine,self.expression)
        self.pack_start(menu,False,False,0)
        self.pack_start(self.method,True,True,0)
        self.pack_start(self.routine,True,True,0)
        self.pack_start(self.expression,True,True,0)

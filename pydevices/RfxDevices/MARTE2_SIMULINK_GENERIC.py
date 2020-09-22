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


@MC.BUILDER('SimulinkWrapperGAM', MC.MARTE2_COMPONENT.MODE_GAM)
class MARTE2_SIMULINK_GENERIC(MC.MARTE2_COMPONENT):
    inputs = [
        {'name': 'In1', 'type': 'int32', 'dimensions': 0, 'parameters':{}},
        {'name': 'In2', 'type': 'int32', 'dimensions': 0, 'parameters':{}},
        {'name': 'In3', 'type': 'int32', 'dimensions': 0, 'parameters':{}},
        {'name': 'In4', 'type': 'int32', 'dimensions': 0, 'parameters':{}},
        {'name': 'In5', 'type': 'int32', 'dimensions': 0, 'parameters':{}},
        {'name': 'In6', 'type': 'int32', 'dimensions': 0, 'parameters':{}},
        {'name': 'In7', 'type': 'int32', 'dimensions': 0, 'parameters':{}},
        {'name': 'In8', 'type': 'int32', 'dimensions': 0, 'parameters':{}},
    ]
    outputs = [
        {'name': 'Out1', 'type': 'int32', 'dimensions': -1, 'parameters':{}},
        {'name': 'Out2', 'type': 'int32', 'dimensions': -1, 'parameters':{}},
        {'name': 'Out3', 'type': 'int32', 'dimensions': -1, 'parameters':{}},
        {'name': 'Out4', 'type': 'int32', 'dimensions': -1, 'parameters':{}},
        {'name': 'Out5', 'type': 'int32', 'dimensions': -1, 'parameters':{}},
        {'name': 'Out6', 'type': 'int32', 'dimensions': -1, 'parameters':{}},
        {'name': 'Out7', 'type': 'int32', 'dimensions': -1, 'parameters':{}},
        {'name': 'Out8', 'type': 'int32', 'dimensions': -1, 'parameters':{}},
    ]
    parameters = [
        {'name':'Verbosity', 'type': 'int32', 'value': 2},
        {'name':'Library', 'type': 'string'},
        {'name':'SymbolPrefix', 'type': 'string'},
        {'name':'Parameter1', 'type': 'int32'},
        {'name':'Parameter2', 'type': 'int32'},
        {'name':'Parameter3', 'type': 'int32'},
        {'name':'Parameter4', 'type': 'int32'},
        {'name':'Parameter5', 'type': 'int32'},
        {'name':'Parameter6', 'type': 'int32'},
        {'name':'Parameter7', 'type': 'int32'},
        {'name':'Parameter8', 'type': 'int32'},
     ]
    parts = []
    
    def prepareMarteInfo(self):
      self.getNode('parameters.par_2:value').putData(self.getNode('parameters.par_3:value').data() + '.so')   #By convention, the library name is assumed to have the same name of prefix and in $LD_LIBRARY_PATH 
      for i in range(8):  #Append 'Parameters.' in from to every parameter name for defined parameters (i.e. with non empty value) 
        try:
          parVal = self.getNode('parameters.par_'+str(4+i)+':value').data()
          #if code arrives here, append 'Paratemeters.' in front if not already done
          parName = self.getNode('parameters.par_'+str(4+i)+':name').data()
          if parName[:11] != 'Parameters.':
            parName = 'Parameters.'+parName
            self.getNode('parameters.par_'+str(4+i)+':name').putData(parName)
        except:
          pass

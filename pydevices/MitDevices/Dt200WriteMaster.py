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

def Dt200WriteMaster(board, cmd, rsh):
  print "Dt200WriteMaster(%d, %s, %d)" % (board, cmd, rsh)
  if rsh :
    file = '/dev/acq200/acq200.%d.rsh' % board
  else :
    file = '/dev/acq32/acq32.%d.m%d' % (board, board)
  try:
    f = open(file, 'rw+')
  except Exception,e:
    print "Error opening D-Tacq device %s\n\t%s" % (file ,str(e),)
    raise e
  try:
    try:  
      f.write(cmd+'\n')
    except Exception,e:
      print "Error writing %s to D-Tacq device %s\n\t%s" % (cmd, file ,str(e),)
      raise e
    try:
      ans = f.readlines()
    except Exception,e:
      print "Error reading answer to %s from D-Tacq device %s\n\t%s" % (cmd, file ,str(e),)
      raise e
  finally:
    f.close()
  print "\t%s" % ans
  return(ans)

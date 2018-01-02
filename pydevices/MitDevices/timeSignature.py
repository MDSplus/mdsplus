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

from MDSplus import *
import numpy

def timeSignatureAnalyze(y, start) :
    """
    JAS  1/27/11
    From TWF translation of IDL
    MIT - PSFC
    
    Function timebase
    A routine to analyze the digitized signal of the timebase
    verifier and extract the real times for the samples at the
    beginnings of the time signatures
    
    Arguments
        y - digitized waveform
        start - time of start of signature.

    Returns
       (times, starts)
        starts - returned list of sample numbers
        times - returned list of times previous argument
     """

    """
    first round the data up and down so that it is all 0 or max value
    """
    max=y.max()
    yytest=y > max/2
    yy=y*0
    yy.put(yytest.nonzero()[0],max)
    """
    find all the times at wiich the data rises or falls
    """
    transitions = yy[1:]-yy[:-1]
    rising = (transitions > 1).nonzero()[0]
    falling = (transitions < -1).nonzero()[0]

    """
    get the times between all of the rising edgets
    """
    bit_width = rising[1:]-rising[:-1]
    max_bitwidth = bit_width.max()
    min_bitwidth = bit_width.min()
    """
    get the times of the beginnings  of the patterns,  These are found
    by wider spaces in the list of rising edges
    """
    starts = rising[(bit_width > max_bitwidth-5).nonzero()[0]+1]
    times = numpy.float64(numpy.array(starts*0))
    rising = rising[(rising>=starts[0]).nonzero()]
    falling = falling[(falling>starts[0]).nonzero()]
    """
    get all of the pulse widths and classifiers for
    narrow - 0 
    wide - 1
    """
    num=numpy.array((len(falling),len(rising))).min()
    widths = falling[:num-1]-rising[:num-1]
    zero_width = widths.min()+5
    one_width = widths.max()-5
    """
    for each pattern
    for each bit in the pattern
    if it is wide
    set this bit in the answer
    scale the answer by the rate the patterns are generated
    """
    for i in range(len(starts)-1):
       ans = numpy.int32(0)
       for bit in range(19):
         if (falling[i*19+bit] - rising[i*19+bit] > one_width):
           ans = ans | numpy.int32(1) << (18-bit)
       times[i] = numpy.float64(ans)*20E-3+start
    return (times[0:-1], starts[0:-1])

def timeSignature(parent) :
    '''
      timeSignature
        Routine returns a Dim after analyzing a recorded timing signature
        waveeform.
      Assumes that under the parent node which is specified as an argument
      the following set of nodes exists and are filled in:

      T_SIG_BASE (axis) - place to store the answer   
     :*T_SIG_CHAN (signal) - points to channel with time signal 
     :T_SIG_DESC (text) - describes these nodes  
      *T_SIG_GATES (numeric) - optional gates for the timing signal 4 values
     :*T_SIG_START (numeric) - start of the timing signature signal 
     :T_SIG_TIMES (signal) - place to store the times and indexes of the time signals

      * used by this routine

      Arguments
        parent - parent node to find above members under.

      Returns
        Dimension
    '''    
    y = data(parent.getNode('t_sig_chan').record)
    try:
        start = data( parent.getNode('t_sig_start').record)
    except:
        start = -3.0
    try:
        gates = data( parent.getNode('t_sig_gates').record)
    except:
        gates = None
    if gates and gates.length==4 :
        gate_times = gates.record
        (times1,idxs1) = timeSigAnal(y[gate_times[0]:gate_times[1]], start)
        (times2, idxs2) = timeSigAnal(y[gate_times[2]:gate_times[3]], start)
        times = (times1, times2)
        idxs = (idxs1, idxs2)
    else:
        (times, idxs) = timeSignatureAnalyze(y, start)

    '''
       Make a dim that has the same size as t_sig_chan
       dt = (times[-1] - times[0])/(samples[-1]-samples[0])
       first_idx = 0
       last_idx = len(y)-1
       time_of_samp_0 = times[0]-(idxs[0]*dt)
       answer = build_dim(build_window(first_idx, 0, time_of_samp_0), * : * : dt)
    '''
    last_idx = len(y)-1
    dt = (times[-1]-times[0]) / (idxs[-1] - idxs[0])
    time_of_zero = times[0] - dt*idxs[0]
    return (Dimension(Window(0, last_idx, time_of_zero), Range( None, None, dt)), Signal(idxs, None, times))
    


    
        

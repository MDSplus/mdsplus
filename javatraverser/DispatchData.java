/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
//package jTraverser;

public class DispatchData extends CompoundData
{
    public static Data getData() {return new DispatchData(); }
    public DispatchData() {dtype = DTYPE_DISPATCH; }
    public DispatchData(int type, Data ident, Data phase, Data when, Data completion)
    {
	dtype = DTYPE_DISPATCH;
	opcode = type;
	descs = new Data[4];
	descs[0] = ident;
	descs[1] = phase;
	descs[2] = when;
	descs[3] = completion;
    }
    public final int getType() {return  opcode; }
    public final Data getIdent() {return descs[0]; }
    public final Data getPhase() {return descs[1]; }
    public final Data getWhen()	 {return descs[2]; }
    public final Data getCompletion() {return descs[3]; }

    public static final int SCHED_NONE = 0x0;
    public static final int SCHED_ASYNCH = 1;//0x1000000;
    public static final int SCHED_SEQ = 2;//0x2000000;
    public static final int SCHED_COND = 3;//0x3000000;
 
}
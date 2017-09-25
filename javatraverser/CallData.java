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

public class CallData extends CompoundData
{
    int type;
    public static Data getData() {return new CallData(); }
    public CallData() {dtype = DTYPE_CALL; }
    public CallData(int type, Data image, Data routine, Data[] arguments)
    {
	int ndescs;
	dtype = DTYPE_CALL;
	this.type = type;
	if(arguments != null)
	    ndescs = 2 + arguments.length;
	else
	    ndescs = 2;
	descs = new Data[ndescs];
	descs[0] = image;
	descs[1] = routine;
	for(int i = 2; i < ndescs; i++)
	    descs[i] = arguments[i-2];
    }
    
    public final int getType() {return type; }
    public final Data getImage() {return descs[0]; }
    public final Data getRoutine() {return descs[1]; }
    public final Data [] getArguments()
    {
	Data ris [] = new Data[descs.length - 2];
	for(int i = 0; i < descs.length - 2; i++)
	    ris[i] = descs[i+2];
	return ris;
    }
}
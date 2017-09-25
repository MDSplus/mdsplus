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

public class ProcedureData extends CompoundData
{
    public static Data getData() {return new ProcedureData(); }
    public ProcedureData() {dtype = DTYPE_PROCEDURE; }
    public ProcedureData(Data timeout, Data language, Data procedure, 
	Data [] arguments)
    {
	int ndescs;
	dtype = DTYPE_PROCEDURE;
	if(arguments != null)
	    ndescs = 3 + arguments.length;
	else
	    ndescs = 3;
	descs = new Data[ndescs];
	descs[0] = timeout;
	descs[1] = language;
	descs[2] = procedure;
	for(int i = 3; i < ndescs; i++)
	    descs[i] = arguments[i-3];
    }
    
     public final Data getTimeout() {return descs[0]; }
     public final Data getLanguage(){return descs[1]; }
     public final Data getProcedure() {return descs[2]; }
     public final Data[] getArguments() 
     {
	Data []ris = new Data[descs.length-3];
	for(int i = 0; i < descs.length-3; i++)
	    ris[i] = descs[3+i];
	return ris;
     }
    }
	    	 

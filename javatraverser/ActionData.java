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

public class ActionData extends CompoundData
{
    public static Data getData()
    {
	return new ActionData();
    }
    
    public ActionData() {dtype = DTYPE_ACTION;}
    public ActionData(Data dispatch, Data task, Data errorlogs, 
	Data completion_message, Data performance)
    {
	dtype = DTYPE_ACTION;
	descs = new Data[5];
	descs[0] = dispatch;
	descs[1] = task;
	descs[2] = errorlogs;
	descs[3] = completion_message;
	descs[4] = performance;
    }
    public final Data getDispatch() {return descs[0]; }
    public final Data getTask()	    {return descs[1]; }
    public final Data getErrorlogs() 
    {
	if(descs.length >= 3)
	    return descs[2];
	else
	    return null;
    }
    public final Data getCompletionMessage()
    {
	if(descs.length >= 4)
	    return descs[3];
	else
	    return null;
    }
        
    public final Data getPerformance() 
    {
	if(descs.length >= 5)
	    return descs[4];
	else
	    return null;
     }
}
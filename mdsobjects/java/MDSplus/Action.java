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
package MDSplus;

/**
 * Class for DTYPE_ACTION
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.21
 */
public class Action extends Compound 
{
	public Action(Data dispatch, Data task, Data errorLog, Data completionMessage, Data performance,
                Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            descs = new Data[]{dispatch, task, errorLog, completionMessage, performance};
            opcode = 0;
	}
	public Action(Data dispatch, Data task, Data errorLog, Data completionMessage, Data performance)
        {
            this(dispatch, task, errorLog, completionMessage, performance, null, null, null, null);
	}
        public Action(Data help, Data units, Data error, Data validation)
        {
             super(help, units, error, validation);
            clazz = CLASS_R;
            dtype = DTYPE_ACTION;
            descs = new Data[5];
            opcode = 0;
        }
        public Action()
        {
            this(null, null, null, null);
        }
        
        public static Action getData(Data help, Data units, Data error, Data validation)
        {
            return new Action(help, units, error, validation);
        }
        
        public Data getDispatch()
        {
            return descs[0];
	}

	public Data getTask()
        {
            return descs[1];
	}

	public Data getErrorLog()
        {
            return descs[2];
	}

	public Data getCompletionMessage()
        {
	    return descs[3];
	}

	public Data getPerformance()
        {
	    return descs[3];
	}

	/**
	 * 
	 * @param data
	 */
	public void setDispatch(Data data)
        {
            descs[0] = data;
	}

	/**
	 * 
	 * @param data
	 */
	public void setTask(Data data)
        {
            descs[1] = data;
	}

	/**
	 * 
	 * @param data
	 */
	public void setErrorLog(Data data)
        {
            descs[2] = data;
	}

	/**
	 * 
	 * @param data
	 */
	public void setCompletionMessage(Data data)
        {
            descs[3] = data;
	}

	/**
	 * 
	 * @param data
	 */
	public void setPerformance(Data data)
        {
            descs[4] = data;
	}

}
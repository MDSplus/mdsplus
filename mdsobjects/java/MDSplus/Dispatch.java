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
 * Class for DTYPE_DISPATCH
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.36
 */
public class Dispatch extends Compound 
{
	public Dispatch(Data ident, Data phase, Data when, Data completion,
                Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            clazz = CLASS_R;
            dtype = DTYPE_DISPATCH;
            descs = new Data[]{ident, phase, when, completion};
	}
	public Dispatch(Data ident, Data phase, Data when, Data completion)
        {
            this(ident, phase, when, completion, null, null, null, null);
	}
        public Dispatch()
        {
            this(null, null, null, null);
        }
        
        public static Dispatch getData(Data help, Data units, Data error, Data validation)
        {
            return new Dispatch(null, null, null, null, help, units, error, validation);
        }

	public Data getIdent()
        {
            return descs[0];
	}

	public Data getPhase()
        {
            return descs[1];
	}

	public Data getWhen()
        {
            return descs[2];
	}

	public Data getCompletion()
        {
            return descs[3];
	}

	/**
	 * 
	 * @param data
	 */
	public void setIdent(Data data)
        {
            descs[0] = data;
	}

	/**
	 * 
	 * @param data
	 */
	public void setPhase(Data data)
        {
            descs[1] = data;
	}

	/**
	 * 
	 * @param data
	 */
	public void setWhen(Data data)
        {
            descs[2] = data;
	}

	/**
	 * 
	 * @param data
	 */
	public void setCompletion(Data data)
        {
            descs[3] = data;
	}

}
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
 * Class for DTYPE_METHODS
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.39
 */
public class Method extends Compound 
{
	public Method(Data timeout, Data method, Data object, Data[] args,
                Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            clazz = CLASS_R;
            dtype = DTYPE_METHOD;
            descs = new Data[3+args.length];
            descs[0] = timeout;
            descs[1] = method;
            descs[2] = object;
            for(int i = 0; i < args.length; i++)
                descs[3+i] = args[i];
	}
	public Method(Data timeout, Data method, Data object, Data[] args)
        {
            this(timeout, method, object, args, null, null, null, null);
	}
        public Method(Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            clazz = CLASS_R;
            dtype = DTYPE_METHOD;
            descs = new Data[3];
        }
       
        public static Method getData(Data help, Data units, Data error, Data validation)
        {
            return new Method(help, units, error, validation);
        }

	public Data getMethod()
        {
            return descs[1];
	}

	public Data getObject()
        {
            return descs[2];
	}

	public Data[] getArguments()
        {
            Data []args = new Data[descs.length - 3];
            for(int i = 0; i < args.length; i++)
                args[i] = descs[3+i];
            return args;
	}

	public Data getTimeout()
        {
            return descs[0];
	}

	/**
	 * 
	 * @param idx
	 */
	public Data getArgumentAt(int idx)
        {
            return descs[3+idx];
	}

	/**
	 * 
	 * @param data
	 */
	public void setMethod(Data data)
        {
            descs[1] = data;
	}

	/**
	 * 
	 * @param data
	 */
	public void setObject(Data data)
        {
            descs[2] = data;
	}

	/**
	 * 
	 * @param data
	 */
	public void setArguments(Data[] args)
        {
            resizeDescs(3+args.length);
            for(int i = 0; i < args.length; i++)
                descs[3+i] = args[i];
	}

	/**
	 * 
	 * @param data
	 */
	public void setTimeout(Data data)
        {
            descs[0] = data;
	}

	/**
	 * 
	 * @param arg
	 * @param idx
	 */
	public void setArgumentAt(Data arg, int idx)
        {
            resizeDescs(3+idx+1);
            descs[3+idx] = arg;
        }
}
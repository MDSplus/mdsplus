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
 * Class for DTYPE_FUNCTION
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.37
 */
public class Function extends Compound 
{

	public Function(int opcode, Data [] args,
                Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            this.opcode = opcode;
            clazz = CLASS_R;
            dtype = DTYPE_FUNCTION;
            descs = new Data[args.length];
            for(int i = 0; i < args.length; i++)
                descs[i] = args[i];
	}
	public Function(int opcode, Data [] args)
        {
            this(opcode, args, null, null, null, null);
        }

        public Function(Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            clazz = CLASS_R;
            dtype = DTYPE_FUNCTION;
            descs = new Data[0];
            opcode = 0;
        }
        public static Function getData(Data help, Data units, Data error, Data validation)
        {
            return new Function(help, units, error, validation);
        }

	public Data [] getArguments()
        {
            return descs;
	}

	/**
	 * 
	 * @param idx
	 */
	public Data getArgumentAt(int idx)
        {
            return descs[idx];
	}


	/**
	 * 
	 * @param data
	 */
	public void setArguments(Data[] args)
        {
            descs = new Data[args.length];
            for(int i = 0; i < args.length; i++)
                descs[i] = args[i];
	}

	/**
	 * 
	 * @param idx
	 * @param arg
	 */
	public void setArgumentAt(int idx, Data arg)
        {
            descs[idx] = arg;
	}

}
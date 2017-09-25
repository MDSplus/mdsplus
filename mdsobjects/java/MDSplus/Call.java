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
 * Class for DTYPE_CALL
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.33
 */
public class Call extends Compound 
{
	/**
	 * Data type argument, used to specify the return type of the called routine
	 */
	public Call(int retType, Data image, Data routine, Data [] args,
                Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            opcode = retType;
            clazz = CLASS_R;
            dtype = DTYPE_CALL;
            descs = new Data[2 + args.length];
            descs[0] = image;
            descs[1] = routine;
            for(int i = 0; i < args.length; i++)
                descs[2+i] = args[i];
	}
	public Call(int retType, Data image, Data routine, Data [] args)
        {
            this(retType, image, routine, args, null, null, null, null);
        }

        public Call(Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            clazz = CLASS_R;
            dtype = DTYPE_CALL;
            descs = new Data[0];
            opcode = 0;
        }
        public static Call getData(Data help, Data units, Data error, Data validation)
        {
            return new Call(help, units, error, validation);
        }

	public Data getImage()
        {
		return descs[0];
	}

	public Data getRoutine()
        {
		return descs[1];
        }

	public Data[] getArguments()
        {
		Data[] args = new Data[descs.length - 2];
                for(int i = 0; i < descs.length - 2; i++)
                    args[i] = descs[2+i];
                return args;
	}

	/**
	 * 
	 * @param idx
	 */
	public Data getArgumentAt(int idx)
        {
            return descs[2+idx];
	}

	/**
	 * 
	 * @param data
	 */
	public void setImage(Data data)
        {
            descs[0] = data;
	}

	/**
	 * 
	 * @param data
	 */
	public void setRoutine(Data data)
        {
            descs[1] = data;
	}

	/**
	 * 
	 * @param data
	 */
	public void setArguments(Data[] args)
        {
            resizeDescs(2 + args.length);
            Data newDescs[] = new Data[2 + args.length];
            newDescs[0] = descs[0];
            newDescs[1] = descs[1];
            for(int i = 0; i < args.length; i++)
                 newDescs[2+i] = args[i];   
	}

	/**
	 * 
	 * @param idx
	 * @param data
	 */
	public void setArgumentAt(int idx, Data data)
        {
            resizeDescs(2 + idx + 1);
            descs[2+idx] = data;
	}

	public int getRetType()
        {
            return opcode;
	}

	/**
	 * 
	 * @param dtype
	 */
	public void setRetType(int dtype)
        {
            opcode = dtype;
	}
}
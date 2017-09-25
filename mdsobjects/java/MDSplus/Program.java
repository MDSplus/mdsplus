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
 * Class for DTYPE_PROGRAM
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.41
 */
public class Program extends Compound 
{
	public Program(Data timeout, Data program,
                Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            clazz = CLASS_R;
            dtype = DTYPE_PROGRAM;
            descs = new Data[]{timeout, program};
	}
	public Program(Data timeout, Data program)
        {
            this(timeout, program, null, null, null, null);
	}
 	public Program(Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            clazz = CLASS_R;
            dtype = DTYPE_PROGRAM;
            descs = new Data[2];
	}
        public Program()
        {
            this(null, null, null, null);
        }
        
        public static Program getData(Data help, Data units, Data error, Data validation)
        {
            return new Program(help, units, error, validation);
        }

	public Data getProgram()
        {
            return descs[1];
	}

	public Data getTimeout()
        {
            return descs[0];
	}

	/**
	 * 
	 * @param data
	 */
	public void setProgram(Data data)
        {
            descs[1] = data;
	}

	/**
	 * 
	 * @param data
	 */
	public void setTimeout(Data data)
        {
            descs[0] = data;
	}

}
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
 * Class for DTYPE_WINDOW
 * @author manduchi
 * @version 1.0
 * @updated 03-ott-2008 12.23.45
 */
public class Window extends Compound 
{
	public Window(Data startidx, Data endidx, Data timeat0,
                Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            clazz = CLASS_R;
            dtype = DTYPE_WINDOW;
            descs = new Data[]{startidx, endidx, timeat0};
	}
	public Window(Data startidx, Data endidx, Data timeat0)
        {
            this(startidx, endidx, timeat0, null, null, null, null);
	}
 	public Window(Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            clazz = CLASS_R;
            dtype = DTYPE_WINDOW;
            descs = new Data[3];
	}
        public Window()
        {
            this(null, null, null, null);
        }
        
        public static Window getData(Data help, Data units, Data error, Data validation)
        {
            return new Window(help, units, error, validation);
        }

	public Data getStartIdx()
        {
            return descs[0];
	}

	public Data getEndIdx()
        {
            return descs[1];
	}

	public Data getTimeAt0()
        {
            return descs[2];
	}

	/**
	 * 
	 * @param data
	 */
	public void setStartIdx(Data data)
        {
            descs[0] = data;
	}

	/**
	 * 
	 * @param data
	 */
	public void setEndIdx(Data data)
        {
            descs[1] = data;
	}

	/**
	 * 
	 * @param data
	 */
	public void setTimeAt0(Data data)
        {
            descs[2] = data;
	}
}

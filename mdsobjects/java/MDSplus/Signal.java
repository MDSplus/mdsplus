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
 * Data for DTYPE_SIGNAL
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.42
 */
public class Signal extends Compound 
{
	public Signal(Data data, Data raw, Data[] dimensions,
                Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            clazz = CLASS_R;
            dtype = DTYPE_SIGNAL;
            descs = new Data[2+dimensions.length];
            descs[0] = data;
            descs[1] = raw;
            for(int i = 0; i < dimensions.length; i++)
                descs[2+i] = dimensions[i];
	}
	public Signal(Data data, Data raw, Data[] dimensions)
        {
            this(data, raw, dimensions, null, null, null, null);
	}
	public Signal(Data data, Data raw, Data dimension)
        {
            this(data, raw, new Data[]{dimension}, null, null, null, null);
	}
        public Signal(Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            clazz = CLASS_R;
            dtype = DTYPE_SIGNAL;
            descs = new Data[2];
        }
       
        public static Signal getData(Data help, Data units, Data error, Data validation)
        {
            return new Signal(help, units, error, validation);
        }


	/**
	 * 
	 * @param raw
	 */
	public void setData(Data data)
        {
            descs[0] = data;
	}

	public Data getData()
        {
            return descs[0];
	}

	/**
	 * 
	 * @param raw
	 */
	public void setRaw(Data raw)
        {
            descs[1] = raw;
	}

	public Data getRaw()
        {
            return descs[1];
	}

	/**
	 * 
	 * @param data
	 */
	public Data[] getDimensions()
        {
            Data []dimensions = new Data[descs.length - 2];
            for(int i = 0; i < dimensions.length; i++)
                dimensions[i] = descs[2+i];
            return dimensions;
	}

	/**
	 * 
	 * @param idx
	 */
	public Data getDimensionAt(int idx)
        {
            return descs[2+idx];
	}

	/**
	 * 
	 * @param data
	 */
	public void setDimensions(Data[] dims)
        {
            resizeDescs(2+dims.length);
            for(int i = 0; i < dims.length; i++)
                descs[2+i] = dims[i];
	}

	/**
	 * 
	 * @param arg
	 * @param idx
	 */
	public void setDimensionAt(Data dim, int idx)
        {
            resizeDescs(2+idx+1);
            descs[2+idx] = dim;
        }
}
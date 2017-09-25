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
 * Compound is the common supreclass for all CLASS_R types.
 * Its fields contain all the required information (Descriptor array, keyword).
 * Its getter/setter methods allow to read/replace descriptors, based on their
 * index.
 * Derived classes will only define methods with appropriate names for
 * reading/writing descriptors (i.e. Data objects).
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.34
 */
public class Compound extends Data {

	/**
	 * Opcode, used only by some derived classes.
	 */
        int opcode;
	/**
	 * The data (descriptor) array.
	 */
	Data[] descs;

	public Compound(Data help, Data units, Data error, Data validation)
        {
           super(help, units, error, validation);
	}

	public int getNumDescs()
        {
		return descs.length;
	}

	/**
	 * 
	 * @param idx
	 */
	public Data getDescAt(int idx)
        {   
            return descs[idx];
	}

	/**
	 * 
	 * @param idx
	 */
	public void setDescAt(int idx, Data desc)
        {
            descs[idx] = desc;

	}

	public Data[] getDescs()
        {
		return descs;
	}

	/**
	 * 
	 * @param dscs
	 */
	public void setDescs(Data[] dscs)
        {
            this.descs = dscs;
	}
        
        protected void resizeDescs(int newDim)
        {
            if(descs == null)
            {
                descs = new Data[newDim];
            }
            else
            {
                if(newDim <= descs.length) return;
                Data newDescs[] = new Data[newDim];
                for(int i = 0; i  < descs.length; i++)
                    newDescs[i] = descs[i];
                descs = newDescs;
            }
        }

}
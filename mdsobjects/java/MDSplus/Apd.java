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
 * Class for CLASS_APD, DTYPE_DSC
 * @author manduchi
 * @version 1.0
 * @updated 03-ott-2008 12.23.37
 */
public class Apd extends Data {

	protected Data[] descs;
        protected int nDescs = 0;
        final static int CHUNK_SIZE = 64;
        public Apd()
        {
            this(new Data[0]);
        }
        public Apd(Data [] descs)
        {
            this(descs, null, null, null, null);
        }
	public Apd(Data [] descs, Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            clazz = CLASS_APD;
            dtype = DTYPE_DSC;
            this.descs = descs;
            nDescs = descs.length;
	}

        public static Data getData(Data [] descs, Data help, Data units, Data error, Data validation)
        {
            return new Apd(descs, help, units, error, validation);
        }
        
        
	public Data[] getDescs()
        {
            Data []retDescs = new Data[nDescs];
            for(int i = 0; i < nDescs; i++)
                retDescs[i] = descs[i];
            return retDescs;
	}

	/**
	 * 
	 * @param descs
	 */
	public void setDescs(Data[] inDescs)
        {
            descs = new Data[inDescs.length];
            for(int i = 0; i < inDescs.length; i++)
                descs[i] = inDescs[i];
            nDescs = descs.length;
        }
	/**
	 * 
	 * @param idx
	 */
	public Data getDescAt(int idx)
        {
            return descs[idx];
	}

        protected void resizeDescs(int newDim)
        {
            if(descs == null)
            {
                descs = new Data[CHUNK_SIZE];
            }
            else
            {
                if(newDim <= descs.length) return;
                Data newDescs[] = new Data[(newDim/CHUNK_SIZE)*CHUNK_SIZE + CHUNK_SIZE];
                for(int i = 0; i  < nDescs; i++)
                    newDescs[i] = descs[i];
                descs = newDescs;
            }
        }
	/**
	 * 
	 * @param idx
	 * @param dsc
	 */
	public void setDescAt(int idx, Data dsc)
        {
            resizeDescs(idx+1);
            descs[idx] = dsc;
        }

}

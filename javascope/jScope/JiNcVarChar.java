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
package jScope;

/* $Id$ */
import java.io.*;
import java.util.*;

class JiNcVarChar extends JiNcVarImp 
{
	public JiNcVarChar(RandomAccessData in, JiNcVar parent, long offset)
	  {
	  super(in, parent, offset);
	  }  
	public Object read(JiDim[] dims) throws IOException
	  {
	  return readChar(dims);
	  }  
	public char[] readChar(JiDim[] dims) throws IOException
	  {
	  char[] rval = null;
	  mParent.validateDims(dims);
	  JiSlabIterator itr = new JiSlabIterator((JiNcSource)mParent.getSource(), mParent, dims);
	  int size = itr.size();
	  rval = new char[size];
	  JiSlab slab;
	  int counter = 0;
	  while((slab = itr.next()) != null){
	      byte[] bytes = new byte[slab.mSize * sizeof()];
	      mRFile.seek(mOffset + slab.mOffset);
	      mRFile.readFully(bytes);
	      for (int i=0; i < slab.mSize; ++i){
		  rval[counter++] = (char)bytes[i];
	      }
	  }
	  return rval;
	  }  
	public int sizeof()
	  {
	  return 1;
	  }  
}

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

public abstract class JiVarImpl implements JiVar 
{
	protected JiDataSource mSource;	// Source object for this variable
	protected String mName;
	protected int mType = 0;

	public JiVarImpl(JiDataSource s, String name)
	  {
	  mName = name;
	  mSource = s;
	  }  
	/**
	 * Get a attribute named 'name'
	 *
	 * @exception IOException
	 * @return the attribute named 'name'
	 */
	public abstract JiVar getAtt(String name) throws IOException;
	/**
	 * Get all the attributes for this source
	 *
	 * @return Vector containing attributes
	 */
	public abstract JiVar[] getAtts();
	/**
	 * Get a dimension named 'name'
	 *
	 * @exception IOException
	 * @return the dimension named 'name'
	 */
	public abstract JiDim getDim(String name) throws IOException;
	/**
	 * Get all the dimensions for this source
	 *
	 * @return Vector containing dimensions
	 */
	public abstract JiDim[] getDims();
	public String getName()
	  {
	  return mName;
	  }  
	public JiDataSource getSource()
	  {
	  return mSource;
	  }  
	public int getType()
	  {
	  return mType;
	  }  
	public String getTypeString()
	  {
	  switch(mType){
	    case Byte:
	      return "byte";
	    case Char:
	      return "char";
	    case Short:
	      return "short";
	    case Int:
	      return "int";
	    case Float:
	      return "float";
	    case Double:
	      return "double";
	    default:
	      return "unknown";
	  }
	  }  
	public abstract Object read(JiDim[] dim)
	  throws IOException;  
	public abstract byte[] readByte(JiDim[] dim)
	  throws IOException;  
	public abstract char[] readChar(JiDim[] dim)
	  throws IOException;  
	public abstract double[] readDouble(JiDim[] dim)
	  throws IOException;  
	public abstract float[] readFloat(JiDim[] dim)
	  throws IOException;  
	public abstract int[] readInt(JiDim[] dim)
	  throws IOException;  
	public abstract short[] readShort(JiDim[] dim)
	  throws IOException;  
}

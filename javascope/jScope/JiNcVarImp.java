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

abstract class JiNcVarImp 
{
	protected JiNcVar mParent;
	protected RandomAccessData mRFile;
	protected long mOffset;
	public JiNcVarImp(RandomAccessData in, JiNcVar parent, long offset)
	  {
	  mParent = parent;
	  mRFile = in;
	  mOffset = offset;
	  }  
	public static void convertDoubles(byte[] bytes, double[] doubles)
	  throws IOException
	  {
	  if (bytes.length % 8 != 0)
	    throw new IOException();
	  int length = bytes.length / 8;

	  
	  // Lack of unsigned types make this a real pain...
	  int count = 0;
	  for (int i=0; i < bytes.length; i+=8){
		  long result =  (long)(bytes[i] & 0xff);
		  result <<= 8;
		  result |=  (long)(bytes[i+1] & 0xff);
		  result <<= 8;
		  result |=  (long)(bytes[i+2] & 0xff);
		  result <<= 8;
		  result |=  (long)(bytes[i+3] & 0xff);
		  result <<= 8;
		  result |=  (long)(bytes[i+4] & 0xff);
		  result <<= 8;
		  result |=  (long)(bytes[i+5] & 0xff);
		  result <<= 8;
		  result |=  (long)(bytes[i+6] & 0xff);
		  result <<= 8;
		  result |=  (long)(bytes[i+7] & 0xff);
		  
/*		  
	      int val1 = (((int)bytes[i] & 0xff) << 24) +
		(((int)bytes[i+1] & 0xff) << 16) +
		  (((int)bytes[i+2] & 0xff) << 8) +
		   ((int)bytes[i+3] & 0xff);
	      int val2 = (((int)bytes[i+4] & 0xff) << 24) +
		(((int)bytes[i+5] & 0xff) << 16) +
		  (((int)bytes[i+6] & 0xff) << 8) +
		    ((int)bytes[i+7] & 0xff);
	      long result = (val2 << 32L) + (val1 & 0xFFFFFFFFL);
*/	      
	      doubles[count++] = Double.longBitsToDouble(result);
	  }
	  }  
	public static void convertFloats(byte[] bytes, float[] floats)
	  throws IOException
	  {
	  if (bytes.length % 4 != 0)
	    throw new IOException();
	  int length = bytes.length / 4;

	  // Lack of unsigned types make this a real pain...
	  int count = 0;
//	  for (int i=0; i < bytes.length; i+=4){
//	      int val = (((int)bytes[i] & 0xff) << 24) +
//		(((int)bytes[i+1] & 0xff) << 16) +
//		  (((int)bytes[i+2] & 0xff) << 8) +
//		   ((int)bytes[i+3] & 0xff);
//	      floats[count++] = Float.intBitsToFloat(val);
//	  }
      int ix = 0;
      while (ix < bytes.length)
      {
        int val = (((int)bytes[ix++] & 0xff) << 24) +
                  (((int)bytes[ix++] & 0xff) << 16) +
                  (((int)bytes[ix++] & 0xff) << 8) +
                  ((int)bytes[ix++] & 0xff);
        floats[count++] = Float.intBitsToFloat(val);
       }
	}  

	public static void convertInts(byte[] bytes, int[] ints)
	  throws IOException
	  {
	  if (bytes.length % 4 != 0)
	    throw new IOException();
	  int length = bytes.length / 4;

	  // Lack of unsigned types make this a real pain...
	  int count = 0;
	  for (int i=0; i < bytes.length; i+=4){
	      ints[count++] = (((int)bytes[i] & 0xff) << 24) +
		(((int)bytes[i+1] & 0xff) << 16) +
		  (((int)bytes[i+2] & 0xff) << 8) +
		   ((int)bytes[i+3] & 0xff);
	  }
	  }  
	public static void convertShorts(byte[] bytes, short[] shorts)
	  throws IOException
	  {
	  if (bytes.length % 2 != 0)
	    throw new IOException();
	  int length = bytes.length / 2;

	  // Lack of unsigned types make this a real pain...
	  int count = 0;
	  for (int i=0; i < bytes.length; i+=4){
	      shorts[count++] = (short)((((int)bytes[i] & 0xff) << 8) +
		((int)bytes[i+1] & 0xff));
	  }
	  }  
	public abstract Object read(JiDim[] dims) throws IOException;
	public byte[] readByte(JiDim[] dims) throws IOException
	  {
	  return null;
	  }  
	public char[] readChar(JiDim[] dims) throws IOException
	  {
	  return null;
	  }  
	public double[] readDouble(JiDim[] dims) throws IOException
	  {
	  return null;
	  }  
	public float[] readFloat(JiDim[] dims) throws IOException
	  {
	  return null;
	  }  
	public int[] readInt(JiDim[] dims) throws IOException
	  {
	  return null;
	  }  
	public short[] readShort(JiDim[] dims) throws IOException
	  {
	  return null;
	  }  
	public abstract int sizeof();
}

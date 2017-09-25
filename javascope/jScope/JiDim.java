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

public class JiDim 
{
	public int mStart, mCount, mStride;
	public String mName;

	public JiDim(String name, int start, int count)
	  {
	  mName = name;
	  mStart = start;
	  mCount = count;
	  mStride = 1;
	  }  
	public JiDim(String name, int start, int count, int stride)
	  {
	  mName = name;
	  mStart = start;
	  mCount = count;
	  mStride = stride;
	  }  
	protected Object clone()
	  {
	  return new JiDim(mName, mStart, mCount, mStride);
	  }  
	public JiDim copy()
	  {
	  return (JiDim)clone();
	  }  
	public JiVar getCoordVar() throws IOException
	  {
	  throw new IOException("JiDim::getCoordVar() : not supported");
	  }  
	public String getName()
	  {
	  return mName;
	  }  
	public String toString()
	  {
	  return "(" + mName + "," + mStart + "," + mCount + "," + mStride + ")";
	  }  
}

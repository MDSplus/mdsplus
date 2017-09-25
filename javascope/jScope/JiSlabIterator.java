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

// Counter for hyperslab access

class JiSlabIterator 
{
	private int[] mCounter;	// Current slice
	private int[] mProducts, mSizes; // Product matrix, sizes of dimensions
	private int mOffset, mSize = 1;  
	private boolean mIsRecord = false;
	private boolean mFinished = false;
	private static boolean DEBUG = false;
	public JiSlabIterator(JiNcSource source, JiNcVar var, JiDim[] sliceDims)
	  {
	  JiDim[] varDims = var.getDims();
	  mIsRecord = var.isRecord();
	  mCounter = new int[sliceDims.length];
	  mProducts = new int[sliceDims.length];
	  mSizes = new int[sliceDims.length];
	  int[] dimOffsets = new int[sliceDims.length];
	  
	  int value = 1;
	  int j = sliceDims.length-1;
	  for (int i=0; i < sliceDims.length; i++, j--){
	      mProducts[i] = value * var.sizeof();
	      mCounter[i] = 0;
	      mSizes[i] = sliceDims[j].mCount;
	      dimOffsets[i] = sliceDims[j].mStart;
	      value *= varDims[j].mCount;
	  }
	  if (mIsRecord){
	      mProducts[sliceDims.length-1] = source.getRecordSize();
	  }
	  for (int i=0; i < sliceDims.length; i++)
	    mSize *= mSizes[i];
	  mOffset = innerProduct(mProducts, dimOffsets);
	  }  
	private int getOffset() throws IOException
	  {
	  int rval = innerProduct(mProducts, mCounter);
//	  System.out.println("Offset = " + rval);
	  rval += mOffset;
	  return rval;
	  }  
	private int innerProduct(int [] v1, int [] v2)
	  {
	  int rval = 0;
	  for (int i=0; i < v1.length; i++){
	      rval += v1[i]*v2[i];
	  }
	  return rval;
	  }  
	public JiSlab next() throws IOException
	  {
	  JiSlab rval = null;
	  if (!mFinished){
	      if (mIsRecord && mSizes.length == 1)
		rval = new JiSlab(getOffset(), 1);
	      else
		rval = new JiSlab(getOffset(), mSizes[0]);
	      if (!mIsRecord && mCounter.length == 1){
		  mFinished = true;
	      } else {		// Advance counter and ripple carry if needed
		  int length = mSizes.length;
		  int i;
		  if (mIsRecord && length == 1)
		    i = 0;
		  else
		    i = 1;
		  for (; i < length; ++i){
		      if (DEBUG){
			  System.out.print("mCounter = ");
			  for (int j = length-1; j >= 0; --j){
			      System.out.print(mCounter[j]);
			  }
			  System.out.println();
		      }
		      
		      ++mCounter[i];
		      mCounter[i] %= mSizes[i];
		      if (mCounter[i] != 0)
			break;
		  }
		  if (i == length)
		    mFinished = true;
	      }
	  }
	  return rval;
	  }  
	public int size()
	  {
	  return mSize;
	  }  
}

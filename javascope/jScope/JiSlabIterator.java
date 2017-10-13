package jScope;

/* $Id$ */
import java.io.IOException;

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

package jScope;

/* $Id$ */
import java.io.*;

class JiNcVarDouble extends JiNcVarImp 
{
	public JiNcVarDouble(RandomAccessData in, JiNcVar parent, long offset)
	  {
	  super(in, parent, offset);
	  }  
	public Object read(JiDim[] dims) throws IOException
	  {
	  return readDouble(dims);
	  }  
	public double[] readDouble(JiDim[] dims) throws IOException
	  {
	  double[] rval = null;
	  mParent.validateDims(dims);
	  JiSlabIterator itr = new JiSlabIterator((JiNcSource)mParent.getSource(), mParent, dims);
	  int size = itr.size();
	  rval = new double[size];
	  JiSlab slab;
	  int counter = 0;
	  while((slab = itr.next()) != null){
	      byte[] bytes = new byte[slab.mSize * sizeof()];
	      double[] doubles = new double[slab.mSize];
	      mRFile.seek(mOffset + slab.mOffset);
	      mRFile.readFully(bytes);
	      convertDoubles(bytes,doubles);
	      for (int i=0; i < slab.mSize; ++i){
		  rval[counter++] = doubles[i];
	      }
	  }
	  return rval;
	  }  
	public int sizeof()
	  {
	  return 8;
	  }  
}

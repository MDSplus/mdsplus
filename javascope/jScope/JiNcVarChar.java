package jScope;

/* $Id$ */
import java.io.IOException;

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

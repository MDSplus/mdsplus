package jScope;

/* $Id$ */
import java.io.IOException;

class JiNcVarByte extends JiNcVarImp 
{
	public JiNcVarByte(RandomAccessData in, JiNcVar parent, long offset)
	  {
	  super(in, parent, offset);
	  }  
	public Object read(JiDim[] dims) throws IOException
	  {
	  return readByte(dims);
	  }  
	public byte[] readByte(JiDim[] dims) throws IOException
	  {
	  byte[] rval = null;
	  mParent.validateDims(dims);
	  JiSlabIterator itr = new JiSlabIterator((JiNcSource)mParent.getSource(), mParent, dims);
	  int size = itr.size();
	  rval = new byte[size];
	  JiSlab slab;
	  int counter = 0;
	  while((slab = itr.next()) != null){
	      byte[] bytes = new byte[slab.mSize * sizeof()];
	      mRFile.seek(mOffset + slab.mOffset);
	      mRFile.readFully(bytes);
	      for (int i=0; i < slab.mSize; ++i){
		  rval[counter++] = bytes[i];
	      }
	  }
	  return rval;
	  }  
	public int sizeof()
	  {
	  return 1;
	  }  
}

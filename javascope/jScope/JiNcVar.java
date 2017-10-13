package jScope;

/* $Id$ */
import java.io.IOException;

public class JiNcVar extends JiVarImpl 
{
	private JiVar[] mAtts = null;	// Attributes for this variable
	private JiDim[] mDims;	// Array of dimensions for variable
	protected int mNcType;	// Two types -- our own type and netCDF type
	private JiNcVarImp mFactory = null; // Factory for variable implementation
	private JiSlabIterator mIterator = null;
	private boolean mIsRecord;

	public static final int NcByte = 1;
	public static final int NcChar = 2;
	public static final int NcShort = 3;
	public static final int NcLong = 4;
	public static final int NcFloat = 5;
	public static final int NcDouble = 6;

	public JiNcVar(JiNcSource s, String name, JiDim[] dims,
		   long offset, int netCDFtype, boolean isRecord) throws IOException
	  {
	  super(s, name);
	  mIsRecord = isRecord;
	  mDims = dims;

	  mNcType = netCDFtype;

	  switch(netCDFtype){
	    case NcByte:
	      mFactory = new JiNcVarByte(s.getInput(), this, offset);
	      mType = Byte;
	      break;
	    case NcChar:
	      mFactory = new JiNcVarChar(s.getInput(), this, offset);
	      mType = Char;
	      break;
	    case NcShort:
	      mFactory = new JiNcVarShort(s.getInput(), this, offset);
	      mType = Short;
	      break;
	    case NcLong:	// netCDF long is same as Java int
	      mFactory = new JiNcVarInt(s.getInput(), this, offset);
	      mType = Int;
	      break;
	    case NcFloat:
	      mFactory = new JiNcVarFloat(s.getInput(), this, offset);
	      mType = Float;
	      break;
	    case NcDouble:
	      mFactory = new JiNcVarDouble(s.getInput(), this, offset);
	      mType = Double;
	      break;
	    default:
	      throw new IOException("Bad variable type");
	  }

	  }  
	public void addAtts(JiVar[] atts)
	  {
	  mAtts = atts;
	  }  
	/**
	 * Get a attribute named 'name'
	 * 
	 * @param name attribute name
	 * @return the attribute named 'name'
	 * @exception IOException
	 * @exception java.io.IOException
	 */
	public JiVar getAtt(String name) throws IOException
	  {
	  JiVar rval = null;
	  for (int i=0; i < mAtts.length; ++i){
	      if (name.equals(mAtts[i].getName())){
		  rval = mAtts[i];
		  break;
	      }
	  }
	  return rval;
	  }  
	/**
	 * Get all the attributes for this variable
	 *
	 * @return Vector containing attributes
	 */
	public JiVar[] getAtts()
	  {
	  JiVar[] rvar = new JiVar[mAtts.length];
	  for (int i=0; i < mAtts.length; ++i){
	      rvar[i] = mAtts[i];
	  }
	  return rvar;
	  }  
	/**
	 * Get a dimension named 'name'
	 * 
	 * @param name string name
	 * @return the dimension named 'name'
	 * @exception IOException
	 * @exception java.io.IOException
	 */
	public JiDim getDim(String name) throws IOException
	  {
	  JiDim rval = null;
	  for (int i=0; i < mDims.length; ++i){
	      if (name.equals(mDims[i].mName)){
		  rval = (JiDim)mDims[i].clone();
		  break;
	      }
	  }
	  return rval;
	  }  
	/**
	 * Get all the dimensions for this source
	 *
	 * @return Vector containing dimensions
	 */
	public JiDim[] getDims()
	  {
	  JiDim[] rdim = new JiDim[mDims.length];
	  for (int i=0; i < mDims.length; ++i){
	      rdim[i] = (JiDim)mDims[i].clone();
	  }
	  return rdim;
	  }  
	public boolean isRecord()
	  {
	  return mIsRecord;
	  }  
	public Object read(JiDim[] dims) throws IOException
	  {
	  return mFactory.read(dims);
	  }  
	public byte[] readByte(JiDim[] dims) throws IOException
	  {
	  return mFactory.readByte(dims);
	  }  
	public char[] readChar(JiDim[] dims) throws IOException
	  {
	  return mFactory.readChar(dims);
	  }  
	public double[] readDouble(JiDim[] dims) throws IOException
	  {
	  return mFactory.readDouble(dims);
	  }  
	public float[] readFloat(JiDim[] dims) throws IOException
	  {
	  return mFactory.readFloat(dims);
	  }  
	public int[] readInt(JiDim[] dims) throws IOException
	  {
	  return mFactory.readInt(dims);
	  }  
	public short[] readShort(JiDim[] dims) throws IOException
	  {
	  return mFactory.readShort(dims);
	  }  
	public int size()
	  {
	  // mIterator init Must be deferred until all of header is read
	  if (mIterator == null)
	    mIterator = new JiSlabIterator((JiNcSource)mSource, this, mDims);
	  return mIterator.size() * sizeof();
	  }  
	public int sizeof()
	  {
	  return mFactory.sizeof();
	  }  
	public void validateDims(JiDim[] dims) throws IOException
	  {
	  if (dims.length != mDims.length)
	    throw new IOException("Dimension lengths don't match");
	  for (int i=0; i < dims.length; ++i){
	      JiDim cdim = dims[i];
	      JiDim mdim = mDims[i];
	      int cend = cdim.mStart + cdim.mCount;
	      int mend = mdim.mStart + mdim.mCount;
	      if (cdim.mStart < mdim.mStart || cdim.mStart >= mend ||
		  cend < mdim.mStart || cend > mend){
		  String message = "Dimensions " + cdim + " " + mdim +
		    " don't match";
		  throw new IOException(message);
	      }
	  }
	  }  
}

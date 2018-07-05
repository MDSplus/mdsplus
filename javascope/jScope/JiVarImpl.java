package jScope;

/* $Id$ */
import java.io.IOException;

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

package jScope;

/* $Id$ */
import java.io.IOException;

public interface JiVar 
{
	public static final int Byte = 1;
	public static final int Char = 2;
	public static final int Short = 3;
	public static final int Int = 4;
	public static final int Float = 5;
	public static final int Double = 6;

	/**
	 * Get a attribute named 'name'
	 *
	 * @exception IOException
	 * @return the attribute named 'name'
	 */
	public JiVar getAtt(String name) throws IOException;
	/**
	 * Get all the attributes for this source
	 *
	 * @return Vector containing attributes
	 */
	public JiVar[] getAtts();
	/**
	 * Get a dimension named 'name'
	 *
	 * @exception IOException
	 * @return the dimension named 'name'
	 */
	public JiDim getDim(String name) throws IOException;
	/**
	 * Get all the dimensions for this source
	 *
	 * @return Vector containing dimensions
	 */
	public JiDim[] getDims();
	public String getName();
	public JiDataSource getSource();
	public int getType();
	public String getTypeString();
	public Object read(JiDim[] dim)
	  throws IOException;  
	public byte[] readByte(JiDim[] dim)
	  throws IOException;  
	public char[] readChar(JiDim[] dim)
	  throws IOException;  
	public double[] readDouble(JiDim[] dim)
	  throws IOException;  
	public float[] readFloat(JiDim[] dim)
	  throws IOException;  
	public int[] readInt(JiDim[] dim)
	  throws IOException;  
	public short[] readShort(JiDim[] dim)
	  throws IOException;  
}

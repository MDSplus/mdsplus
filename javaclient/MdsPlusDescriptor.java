package MdsPlus;
/*
 *
 * MdsPlusDescriptor
 *
 */

/** This class is a generic descriptor for data and is used when communicating
* with a remote MdsPlus server.
*/

public class MdsPlusDescriptor extends Object
{
	static public final byte DTYPE_UCHAR = 2;
	static public final byte DTYPE_USHORT = 3;
	static public final byte DTYPE_UINT = 4;
	static public final byte DTYPE_CHAR = 6;
	static public final byte DTYPE_SHORT = 7;
	static public final byte DTYPE_INT = 8;
	static public final byte DTYPE_FLOAT = 10;
	static public final byte DTYPE_DOUBLE = 11;
	static public final byte DTYPE_COMPLEX = 12;
	static public final byte DTYPE_CSTRING = 14;
	static final byte DTYPE_EVENT = 99;
	/** data type of resultant data. <BR>
	* A value of 2 or 6 represents byte data. <BR>
	* A value of 3 or 7 represents short data. <BR>
	* A value of 4 or 8 represents int data. <BR>
	* A value of 10 represents float data. <BR>
	* A value of 11 represents double data.<BR>
	*/
	public byte dtype;
	/** dimensions of data array.
	* The system supports data with up to 8 dimensions. An array with dims[0]=10 and dims[1]=5 represents
	* an 10 by 5 array.
	*/
	public int  dims[];
	/** return status of expression evaluation. Low bit set is successful. Normally an MdsPlusException
	* is thrown if the low bit is not set.
	*/
	public int  status;
	/** data returned from remote host. Use the provided casting methods to obtain the
	data in the appropriate format.
	*/
	public byte data[];

	/** constructs an empty MdsPlusDescriptor
	*/
	public MdsPlusDescriptor() {
		dims = new int[0];
		data = new byte[0];
	}

	/** constructs an MdsPlusDescriptor from a byte.
	*/
	public MdsPlusDescriptor(byte b) {
		dtype = DTYPE_CHAR;
		dims = new int[0];
		data = new byte[1];
		data[0] = b;
	}

	/** constructs an MdsPlusDescriptor from a short.
	*/
	public MdsPlusDescriptor(short s) {
		java.io.ByteArrayOutputStream buffer = new java.io.ByteArrayOutputStream();
		java.io.DataOutputStream out = new java.io.DataOutputStream(buffer);
		dtype = DTYPE_SHORT;
		try {out.writeShort((int)s);} catch (java.io.IOException e){}
		dims = new int[0];
		data = buffer.toByteArray();
	}

	/** constructs an MdsPlusDescriptor from an int.
	*/
	public MdsPlusDescriptor(int s) {
		java.io.ByteArrayOutputStream buffer = new java.io.ByteArrayOutputStream();
		java.io.DataOutputStream out = new java.io.DataOutputStream(buffer);
		dtype = DTYPE_INT;
		try {out.writeInt(s);} catch (java.io.IOException e){}
		dims = new int[0];
		data = buffer.toByteArray();
	}

	/** constructs an MdsPlusDescriptor from a float.
	*/

	public MdsPlusDescriptor(float s) {
		java.io.ByteArrayOutputStream buffer = new java.io.ByteArrayOutputStream();
		java.io.DataOutputStream out = new java.io.DataOutputStream(buffer);
		dtype = DTYPE_FLOAT;
		try {out.writeFloat(s);} catch (java.io.IOException e){}
		dims = new int[0];
		data = buffer.toByteArray();
	}

	/** constructs an MdsPlusDescriptor from a double.
	*/
	public MdsPlusDescriptor(double s) {
		java.io.ByteArrayOutputStream buffer = new java.io.ByteArrayOutputStream();
		java.io.DataOutputStream out = new java.io.DataOutputStream(buffer);
		dtype = DTYPE_DOUBLE;
		try {out.writeDouble(s);} catch (java.io.IOException e){}
		dims = new int[0];
		data = buffer.toByteArray();
	}

	/** constructs an MdsPlusDescriptor from a string.
	*/
	public MdsPlusDescriptor(String s) {
		dtype = DTYPE_CSTRING;
//		data = new byte[s.length()];
		dims = new int[0];
	        data = s.getBytes();
//		s.getBytes(0,s.length(),data,0);
	}

	/** constructs an MdsPlusDescriptor from a byte array.
	*/
	public MdsPlusDescriptor(byte b[]) {
		dtype = DTYPE_CHAR;
		dims = new int[1];
		dims[0] = b.length;
		data = b;
	}

	/** constructs an MdsPlusDescriptor from a short array.
	*/
	public MdsPlusDescriptor(short s[]) {
		java.io.ByteArrayOutputStream buffer = new java.io.ByteArrayOutputStream();
		java.io.DataOutputStream out = new java.io.DataOutputStream(buffer);
		dtype = DTYPE_SHORT;
		dims = new int[1];
		dims[0] = s.length;
		int i;
		for (i=0;i<s.length;i++) {try {out.writeShort((int)s[i]);} catch (java.io.IOException e){}}
		data = buffer.toByteArray();
	}

	/** constructs an MdsPlusDescriptor from an int array.
	*/
	public MdsPlusDescriptor(int s[]) {
		java.io.ByteArrayOutputStream buffer = new java.io.ByteArrayOutputStream();
		java.io.DataOutputStream out = new java.io.DataOutputStream(buffer);
		dtype = DTYPE_INT;
		dims = new int[1];
		dims[0] = s.length;
		int i;
		for (i=0;i<s.length;i++) {try {out.writeInt(s[i]);} catch (java.io.IOException e){}}
		data = buffer.toByteArray();
	}

	/** constructs an MdsPlusDescriptor from a float array.
	*/
	public MdsPlusDescriptor(float s[]) {
		java.io.ByteArrayOutputStream buffer = new java.io.ByteArrayOutputStream();
		java.io.DataOutputStream out = new java.io.DataOutputStream(buffer);
		dtype = DTYPE_FLOAT;
		dims = new int[1];
		dims[0] = s.length;
		int i;
		for (i=0;i<s.length;i++) {try {out.writeFloat(s[i]);} catch (java.io.IOException e){}}
		data = buffer.toByteArray();
	}

	/** constructs an MdsPlusDescriptor from a double array.
	*/
	public MdsPlusDescriptor(double s[]) {
		java.io.ByteArrayOutputStream buffer = new java.io.ByteArrayOutputStream();
		java.io.DataOutputStream out = new java.io.DataOutputStream(buffer);
		dtype = DTYPE_DOUBLE;
		dims = new int[1];
		dims[0] = s.length;
		int i;
		for (i=0;i<s.length;i++) {try {out.writeDouble(s[i]);} catch (java.io.IOException e){}}
		data = buffer.toByteArray();
	}

	private byte[] FlipBytes(int size)
	{
		int samples = data.length/size;
		byte ans[] = new byte[data.length];
		for (int index=0; index<samples; index++)
			for (int idx=0;idx<size;idx++)
				ans[index*size+idx] = data[(index+1)*size-idx-1];
		return ans;
	}

	/** returns the data as an array of bytes.
	* @return an array of bytes.
	* @exception IOException error converting data to bytes.
	* @exception MdsPlusException invalid data dtype for this conversion.
	*/
	public byte[] Byte() throws java.io.IOException,MdsPlusException
	{
		byte ans[] = new byte[0];
		switch (dtype)
		{
		case DTYPE_CHAR:
		case DTYPE_UCHAR:
			ans = data;
			break;
		case DTYPE_SHORT:
		case DTYPE_USHORT:
			short sans[] = Short();
			ans = new byte[sans.length];
			for (int i=0;i<ans.length;i++) ans[i] = (byte)sans[i];
			break;
		case DTYPE_INT:
		case DTYPE_UINT:
			int ians[] = Int();
			ans = new byte[ians.length];
			for (int i=0;i<ans.length;i++) ans[i] = (byte)ians[i];
			break;
		case DTYPE_FLOAT:
			float fans[] = Float();
			ans = new byte[fans.length];
			for (int i=0;i<ans.length;i++) ans[i] = (byte)fans[i];
			break;
		case DTYPE_DOUBLE:
			double dans[] = Double();
			ans = new byte[dans.length];
			for (int i=0;i<ans.length;i++) ans[i] = (byte)dans[i];
			break;
		default:
			throw new MdsPlusException("Error converting value to int");
		}
		return ans;
	}


	/** returns the data as an array of shorts.
	* @return an array of shorts.
	* @exception IOException error converting data to shorts.
	* @exception MdsPlusException invalid data dtype for this conversion.
	*/
	public short[] Short() throws java.io.IOException,MdsPlusException
	{
		short ans[] = new short[0];
		switch (dtype)
		{
		case DTYPE_CHAR:
		case DTYPE_UCHAR:
			byte bans[] = Byte();
			ans = new short[bans.length];
			for (int i=0;i<ans.length;i++) ans[i] = (short)bans[i];
			break;
		case DTYPE_SHORT:
		case DTYPE_USHORT:
		    ans = new short[data.length/2];
			int samples = data.length/2;
			byte ndata[] = FlipBytes(2);
			java.io.ByteArrayInputStream bais = new java.io.ByteArrayInputStream(ndata);
		    java.io.DataInputStream dis = new java.io.DataInputStream(bais);
			for (int index=0;index<samples;index++)
				ans[index] = dis.readShort();
			break;
		case DTYPE_INT:
		case DTYPE_UINT:
			int ians[] = Int();
			ans = new short[ians.length];
			for (int i=0;i<ans.length;i++) ans[i] = (short)ians[i];
			break;
		case DTYPE_FLOAT:
			float fans[] = Float();
			ans = new short[fans.length];
			for (int i=0;i<ans.length;i++) ans[i] = (short)fans[i];
			break;
		case DTYPE_DOUBLE:
			double dans[] = Double();
			ans = new short[dans.length];
			for (int i=0;i<ans.length;i++) ans[i] = (short)dans[i];
			break;
		default:
			throw new MdsPlusException("Error converting value to int");
		}
		return ans;
	}

	/** returns the data as an array of ints.
	* @return an array of ints.
	* @exception IOException error converting data to int.
	* @exception MdsPlusException invalid data dtype for this conversion.
	*/
	public int[] Int() throws java.io.IOException,MdsPlusException
	{
		int ans[] = new int[0];
		switch (dtype)
		{
		case DTYPE_CHAR:
		case DTYPE_UCHAR:
			byte bans[] = Byte();
			ans = new int[bans.length];
			for (int i=0;i<ans.length;i++) ans[i] = (int)bans[i];
			break;
		case DTYPE_SHORT:
		case DTYPE_USHORT:
			short sans[] = Short();
			ans = new int[sans.length];
			for (int i=0;i<ans.length;i++) ans[i] = (int)sans[i];
			break;
		case DTYPE_INT:
		case DTYPE_UINT:
		    ans = new int[data.length/4];
			int samples = data.length/4;
			byte ndata[] = FlipBytes(4);
			java.io.ByteArrayInputStream bais = new java.io.ByteArrayInputStream(ndata);
		    java.io.DataInputStream dis = new java.io.DataInputStream(bais);
			for (int index=0;index<samples;index++)
				ans[index] = dis.readInt();
			break;
		case DTYPE_FLOAT:
			float fans[] = Float();
			ans = new int[fans.length];
			for (int i=0;i<ans.length;i++) ans[i] = (int)fans[i];
			break;
		case DTYPE_DOUBLE:
			double dans[] = Double();
			ans = new int[dans.length];
			for (int i=0;i<ans.length;i++) ans[i] = (int)dans[i];
			break;
		default:
			throw new MdsPlusException("Error converting value to int");
		}
		return ans;
	}

	/** returns the data as an array of floats.
	* @return an array of floats.
	* @exception IOException error converting data to float.
	* @exception MdsPlusException invalid data dtype for this conversion.
	*/
	public float[] Float() throws java.io.IOException,MdsPlusException
	{
		float ans[] = new float[0];
		switch (dtype)
		{
		case DTYPE_CHAR:
		case DTYPE_UCHAR:
			byte bans[] = Byte();
			ans = new float[bans.length];
			for (int i=0;i<ans.length;i++) ans[i] = (float)bans[i];
			break;

		case DTYPE_SHORT:
		case DTYPE_USHORT:
			short sans[] = Short();
			ans = new float[sans.length];
			for (int i=0;i<ans.length;i++) ans[i] = (float)sans[i];
			break;
		case DTYPE_INT:
		case DTYPE_UINT:
			int ians[] = Int();
			ans = new float[ians.length];
			for (int i=0;i<ans.length;i++) ans[i] = (float)ians[i];
			break;
		case DTYPE_FLOAT:
		    ans = new float[data.length/4];
			int samples = data.length/4;
			byte ndata[] = FlipBytes(4);
			java.io.ByteArrayInputStream bais = new java.io.ByteArrayInputStream(ndata);
		    java.io.DataInputStream dis = new java.io.DataInputStream(bais);
			for (int index=0;index<samples;index++)
				ans[index] = dis.readFloat();
			break;
		case DTYPE_DOUBLE:
			double dans[] = Double();
			ans = new float[dans.length];
			for (int i=0;i<ans.length;i++) ans[i] = (float)dans[i];
			break;
		default:
			throw new MdsPlusException("Error converting value to float");
		}
		return ans;
	}
	/** returns the data as an array of doubles.
	* @return an array of doubles.
	* @exception IOException error converting data to float.
	* @exception MdsPlusException invalid data dtype for this conversion.
	*/
	public double[] Double() throws java.io.IOException,MdsPlusException
	{
		double ans[] = new double[0];
		switch (dtype)
		{
		case DTYPE_CHAR:
		case DTYPE_UCHAR:
			byte bans[] = Byte();
			ans = new double[bans.length];
			for (int i=0;i<ans.length;i++) ans[i] = (double)bans[i];
			break;

		case DTYPE_SHORT:
		case DTYPE_USHORT:
			short sans[] = Short();
			ans = new double[sans.length];
			for (int i=0;i<ans.length;i++) ans[i] = (double)sans[i];
			break;
		case DTYPE_INT:
		case DTYPE_UINT:
			int ians[] = Int();
			ans = new double[ians.length];
			for (int i=0;i<ans.length;i++) ans[i] = (double)ians[i];
			break;
		case DTYPE_FLOAT:
			float fans[] = Float();
			ans = new double[fans.length];
			for (int i=0;i<ans.length;i++) ans[i] = (double)fans[i];
			break;
		case DTYPE_DOUBLE:
		    ans = new double[data.length/8];
			int samples = data.length/8;
			byte ndata[] = FlipBytes(8);
			java.io.ByteArrayInputStream bais = new java.io.ByteArrayInputStream(ndata);
		    java.io.DataInputStream dis = new java.io.DataInputStream(bais);
			for (int index=0;index<samples;index++)
				ans[index] = dis.readDouble();
			break;
		default:
			throw new MdsPlusException("Error converting value to double");
		}
		return ans;
	}

	/** returns the error string of low bit of status is zero.
	*/
	public String GetErrorString() {
		String ans = null;
		if ((dtype == DTYPE_CSTRING) && ((status & 1)==0) && (data.length > 0))
//			ans = new String(data,0x00);
	                ans = new String(data);
		return ans;
	}

	/** returns the data as a String.
	*/
	public String String() {
		String ans = null;
		if ((dtype == DTYPE_CSTRING) && ((status & 1)==1) && (data.length > 0))
//			ans = new String(data,0x00);
	                ans = new String(data);
		return ans;
	}



}


package mds.data;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.descriptor.Descriptor;

public interface DATA<T extends Object>
{
	public static final MdsException dataerror = new MdsException("DATA: Cannot convert to data.");
	public static final MdsException tdierror = new MdsException("Error evaluating data.");
	public static final MdsException notimplemented = new MdsException("Not yet implemented.");

	/**
	 * returns the absolute value of the data
	 *
	 * @return Descriptor
	 */
	public Descriptor<?> abs() throws MdsException;

	public Descriptor<?> add(Descriptor<?> X, Descriptor<?> Y) throws MdsException;

	/**
	 * returns the data cast to byte
	 *
	 * @return Int8, Int8Array, Missing
	 */
	public Descriptor<?> bytes() throws MdsException;

	/**
	 * returns the data cast to byte_unsigned
	 *
	 * @return Uint8, Uint8Array, Missing
	 */
	public Descriptor<?> byteu() throws MdsException;

	/**
	 * returns the dclass of the data
	 *
	 * @return byte
	 */
	public byte dclass();

	/**
	 * returns the decompiled string of the data
	 *
	 * @return String
	 */
	public String decompile();

	public StringBuilder decompile(int prec, StringBuilder pout, int mode);

	/**
	 * returns the data cast to dfloat
	 *
	 * @return Float64, Float64Array, Missing
	 */
	public Descriptor<?> dfloat() throws MdsException;

	public Descriptor<?> divide(Descriptor<?> X, Descriptor<?> Y) throws MdsException;

	/**
	 * returns the dtype of the data
	 *
	 * @return byte
	 */
	public DTYPE dtype();

	/**
	 * returns the data cast to ffloat
	 *
	 * @return Float32, Float32Array, Missing
	 */
	public Descriptor<?> ffloat() throws MdsException;

	/**
	 * returns the data cast to fsfloat
	 *
	 * @return Float32, Float32Array, Missing
	 */
	public Descriptor<?> fsfloat() throws MdsException;

	/**
	 * returns the data cast to ftfloat
	 *
	 * @return Float64, Float64Array, Missing
	 */
	public Descriptor<?> ftfloat() throws MdsException;

	/**
	 * returns a read-only ByteBuffer of the data
	 *
	 * @return ByteBuffer
	 */
	public ByteBuffer getBuffer();

	/**
	 * returns the rank of the data to determine the return dtype of a binary
	 * operation
	 *
	 * @return byte: 0bEACCBBBS Error,Array,Class,Bits,Signed
	 */
	public byte getRank();

	/**
	 * returns the shape of the data, i.e. its dimensions
	 *
	 * @return int[]
	 */
	public int[] getShape();

	/**
	 * returns the data cast to gfloat
	 *
	 * @return Float64, Float64Array, Missing
	 */
	public Descriptor<?> gfloat() throws MdsException;

	/**
	 * returns the bitwise negated value of data
	 *
	 * @return INTEGER_UNSIGNED, INTEGER_UNSIGNEDArray, Missing
	 */
	public Descriptor<?> inot() throws MdsException;

	/**
	 * returns the length of the data (element wise)
	 *
	 * @return int
	 */
	public int length() throws MdsException;

	/**
	 * returns the data cast to long
	 *
	 * @return Int32, Int32Array, Missing
	 */
	public Descriptor<?> longs() throws MdsException;

	/**
	 * returns the data cast to long_unsigned
	 *
	 * @return Uint32, Uint32Array, Missing
	 */
	public Descriptor<?> longu() throws MdsException;

	public Descriptor<?> multiply(Descriptor<?> X, Descriptor<?> Y) throws MdsException;

	/**
	 * returns the length of the data (element wise)
	 *
	 * @return Descriptor
	 */
	public Descriptor<?> neg() throws MdsException;

	/**
	 * returns the negated logical value of the data
	 *
	 * @return Uint8, Uint8Array, Missing
	 */
	public Descriptor<?> not() throws MdsException;

	/**
	 * returns the data cast to octaword
	 *
	 * @return Int128, Int128Array, Missing
	 */
	public Descriptor<?> octawords() throws MdsException;

	/**
	 * returns the data cast to octaword_unsigned
	 *
	 * @return Uint128, Uint128Array, Missing
	 */
	public Descriptor<?> octawordu() throws MdsException;

	public Descriptor<?> power(Descriptor<?> X, Descriptor<?> Y) throws MdsException;

	/**
	 * returns the data cast to quadword
	 *
	 * @return Int64, Int64Array, Missing
	 */
	public Descriptor<?> quadwords() throws MdsException;

	/**
	 * returns the data cast to quadword_unsigned
	 *
	 * @return Uint64, Uint64Array, Missing
	 */
	public Descriptor<?> quadwordu() throws MdsException;

	public ByteBuffer serialize();

	public Descriptor<?> shiftleft(Descriptor<?> X) throws MdsException;

	public Descriptor<?> shiftright(Descriptor<?> X) throws MdsException;

	public Descriptor<?> subtract(Descriptor<?> X, Descriptor<?> Y) throws MdsException;

	/**
	 * returns the data as text
	 *
	 * @return CString, CStringArray, Missing
	 * @throws MdsException
	 */
	public Descriptor<?> text() throws MdsException;

	public BigInteger toBigInteger();

	public BigInteger[] toBigIntegerArray();

	public byte toByte();

	public byte[] toByteArray();

	public Descriptor<T> toDescriptor();

	public double toDouble();

	public double[] toDoubleArray();

	public float toFloat();

	public float[] toFloatArray();

	public int toInt();

	public int[] toIntArray();

	public long toLong();

	public long[] toLongArray();

	public short toShort();

	public short[] toShortArray();

	public String[] toStringArray();

	/**
	 * returns the data cast to word
	 *
	 * @return Int16, Int16Array, Missing
	 */
	public Descriptor<?> words() throws MdsException;

	/**
	 * returns the data cast to word_unsigned
	 *
	 * @return Uint16, Uint16Array, Missing
	 */
	public Descriptor<?> wordu() throws MdsException;
}

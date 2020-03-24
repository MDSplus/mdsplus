//package jTraverser;

public class ByteArray extends ArrayData
{
    byte datum[];
    public static Data getData(byte datum[], boolean unsigned)
    {
	return new ByteArray(datum, unsigned);
    }
    public ByteArray(byte datum[], boolean unsigned)
    {
	length = datum.length;
	if(unsigned)
	    dtype = DTYPE_BU;
	else
	    dtype = DTYPE_B;
	this.datum = new byte[datum.length];
	for(int i = 0; i < datum.length; i++)
	    this.datum[i] = datum[i];
    }
    public ByteArray(byte datum[]) { this(datum, false); }
    public int [] getIntArray()
    {
	int ris [] = new int[datum.length];
	for(int i = 0; i < datum.length; i++)
	    ris[i] = (int)datum[i];
	return ris;
    }
    public float [] getFloatArray()
    {
	float ris [] = new float[datum.length];
	for(int i = 0; i < datum.length; i++)
	    ris[i] = (float)datum[i];
	return ris;
    }
    public byte[] getByteArray(){return datum;}
}
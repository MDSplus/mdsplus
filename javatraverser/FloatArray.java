//package jTraverser;

public class FloatArray extends ArrayData
{
    float datum[];
    int flags = DTYPE_FLOAT;
    public static Data getData(float datum[], int flags)
    {
	    return new FloatArray(datum, flags);
    }
    public FloatArray(float datum[]){this(datum, DTYPE_FLOAT);}
    public FloatArray(float datum[], int flags)
    {
	    length = datum.length;
	    dtype = DTYPE_FLOAT;
	    this.datum = new float[datum.length];
	    for(int i = 0; i < datum.length; i++)
	        this.datum[i] = datum[i];
	    this.flags = flags;
    }
    public int [] getIntArray()
    {
	int ris [] = new int[datum.length];
	for(int i = 0; i < datum.length; i++)
	    ris[i] = (int)datum[i];
	return ris;
    }
    public float [] getFloatArray() {return datum; }
    public int getFlags(){return flags;}
}
//package jTraverser;

public class FloatArray extends ArrayData
{
    float datum[];
    public static Data getData(float datum[])
    {
	return new FloatArray(datum);
    }
    public FloatArray(float datum[])
    {
	length = datum.length;
	dtype = DTYPE_FLOAT;
	this.datum = new float[datum.length];
	for(int i = 0; i < datum.length; i++)
	    this.datum[i] = datum[i];
    }
    public int [] getIntArray() 
    {
	int ris [] = new int[datum.length];
	for(int i = 0; i < datum.length; i++)
	    ris[i] = (int)datum[i];
	return ris;
    }
    public float [] getFloatArray() {return datum; }
}
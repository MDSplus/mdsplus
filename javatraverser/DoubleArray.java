//package jTraverser;

public class DoubleArray extends ArrayData
{
    double datum[];
    public static Data getData(double datum[])
    {
	return new DoubleArray(datum);
    }
    public DoubleArray(double datum[])
    {
	length = datum.length;
	dtype = DTYPE_DOUBLE;
	this.datum = new double[datum.length];
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
    public float [] getFloatArray()
    {
	float ris [] = new float[datum.length];
	for(int i = 0; i < datum.length; i++)
	    ris[i] = (float)datum[i];
	return ris;
    }
}
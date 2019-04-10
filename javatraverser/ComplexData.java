//package jTraverser;

public class ComplexData extends AtomicData
{
    double re, im;
    int flags = DTYPE_FTC;
    public static Data getData(double re, double im, int flags)
    {
	    return new ComplexData(re, im, flags);
    }
    public ComplexData(double re, double im) {this(re, im, DTYPE_FTC);}
    public ComplexData(double re, double im, int flags)
    {
	    dtype = DTYPE_FTC;
	    this.re = re;
	    this.im = im;
	    this.flags = flags;
    }
}

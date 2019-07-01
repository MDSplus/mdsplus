//package jTraverser;
/** Abstract class Data defines the general concept of data. At this level only a list of
DataListeners is maintained, as well as methods to notify a data change to the element of this list.
*/
import java.util.*;
import java.io.*;

public abstract class Data implements Serializable
{
    static boolean library_loaded = false;
/*    static {
	try {
	    System.loadLibrary("JavaMds");
	    }catch(Exception e) {System.out.println("Cannot load library " + e); }
    }*/
    byte dclass, dtype;
    Vector data_listeners = new Vector();
    public Data()
    {
	if(!library_loaded)
	{
	    library_loaded = true;
	    try {
//              System.loadLibrary("MdsShr");
//              System.loadLibrary("MdsIpShr");
//              System.loadLibrary("TreeShr");
//              System.loadLibrary("TdiShr");
	      System.loadLibrary("JavaMds");
		}catch(Exception exc)
	        {
	          jTraverser.stderr("Error loading library", exc);
	          exc.printStackTrace();
	        }
	    }
    }
    native static public Data fromExpr(String text_descr);
    native public String toString();
    native static public int evaluate(String expression);


    public void AddDataListener(DataListener dl)
    {
	data_listeners.addElement(dl);
    }
    public void FireDataEvent()
    {
	for(int i = 0; i < data_listeners.size(); i++)
	    ((DataListener)(data_listeners.elementAt(i))).DataChanged(this);
    }
    public int getInt() throws IllegalDataException
    {
	throw new IllegalDataException("Conversion to int not supported", this);
    }
    public float getFloat() throws IllegalDataException
    {
	throw new IllegalDataException("Conversion to float not supported", this);
    }
    public double getDouble() throws IllegalDataException
    {
	throw new IllegalDataException("Conversion to double not supported", this);
    }
    public int[] getIntArray() throws IllegalDataException
    {
	throw new IllegalDataException("Conversion to int array not supported", this);
    }
    public float[] getFloatArray() throws IllegalDataException
    {
	throw new IllegalDataException("Conversion to float array not supported", this);
    }
    public double[] getDoubleArray() throws IllegalDataException
    {
	throw new IllegalDataException("Conversion to double array not supported", this);
    }
    public String getString() throws IllegalDataException
    {
	throw new IllegalDataException("Conversion to String not supported", this);
    }
    public String []getStringArray() throws IllegalDataException
    {
	throw new IllegalDataException("Conversion to String array not supported", this);
    }
    abstract boolean isAtomic();


    public static final byte DTYPE_BU = 2;
    public static final byte DTYPE_WU = 3;
    public static final byte DTYPE_LU = 4;
    public static final byte DTYPE_QU = 5;
    public static final byte DTYPE_OU = 25;
    public static final byte DTYPE_B = 6;
    public static final byte DTYPE_W = 7;
    public static final byte DTYPE_L = 8;
    public static final byte DTYPE_Q = 9;
    public static final byte DTYPE_O = 26;
    public static final byte DTYPE_FLOAT = 52;
    public static final byte DTYPE_DOUBLE = 53;
    public static final byte DTYPE_FSC = 54;
    public static final byte DTYPE_FTC = 55;
    public static final byte DTYPE_F = 10;
    public static final byte DTYPE_D = 11;
    public static final byte DTYPE_G = 27;
    public static final byte DTYPE_H = 28;
    public static final byte DTYPE_T = 14;
    public static final byte DTYPE_POINTER = 51;
    public static final byte DTYPE_DSC = 24;
    public static final byte DTYPE_IDENT = -65; //191
    public static final byte DTYPE_NID = -64; //192
    public static final byte DTYPE_PATH = -63; //193
    public static final byte DTYPE_PARAM = -62; //194
    public static final byte DTYPE_SIGNAL = -61; //195
    public static final byte DTYPE_DIMENSION = -60; //196
    public static final byte DTYPE_WINDOW = -59; //197
    public static final byte DTYPE_SLOPE = -58; //198
    public static final byte DTYPE_FUNCTION = -57; //199
    public static final byte DTYPE_CONGLOM = -56; //200
    public static final byte DTYPE_RANGE = -55; //201
    public static final byte DTYPE_ACTION = -54; //202
    public static final byte DTYPE_DISPATCH = -53; //203
    public static final byte DTYPE_PROGRAM = -52; //204
    public static final byte DTYPE_ROUTINE = -51; //205
    public static final byte DTYPE_PROCEDURE = -50; //206
    public static final byte DTYPE_METHOD = -49; //207
    public static final byte DTYPE_DEPENDENCY = -48; //208
    public static final byte DTYPE_CONDITION = -47; //209
    public static final byte DTYPE_EVENT = -46; //210
    public static final byte DTYPE_WITH_UNITS = -45; //211
    public static final byte DTYPE_CALL = -44; //212
    public static final byte DTYPE_WITH_ERROR = -43; //213
    public static final byte DTYPE_LIST = -42;//214
    public static final byte DTYPE_TUPLE = -41;//215
    public static final byte DTYPE_DICTIONARY = -40;//216

    public static final byte CLASS_S = 1;
    public static final byte CLASS_D = 2;
    public static final byte CLASS_A = 4;
    public static final byte CLASS_R = -62; //194
    public static final byte CLASS_APD = -60; //196
}




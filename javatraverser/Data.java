//package jTraverser;
/** Abstract class Data defines the general concept of data. At this level only a list of 
DataListeners is maintained, as well as methods to notify a data change to the element of this list.
*/
import java.util.*;

abstract class Data 
{
    static {
        try {
	    System.loadLibrary("JavaMds");
	    }catch(Exception e) {System.out.println("Cannot load library " + e); }
    }
    int dclass;
    int dtype;
    Vector data_listeners = new Vector();
    public Data() {}
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
    public int[] getIntArray() throws IllegalDataException		    
    {
	throw new IllegalDataException("Conversion to int array not supported", this); 
    }
    public float[] getFloatArray() throws IllegalDataException	  
    {
	throw new IllegalDataException("Conversion to float array not supported", this); 
    }
    public String getString() throws IllegalDataException		    
    {
	throw new IllegalDataException("Conversion to String not supported", this); 
    }
    abstract boolean isAtomic();


    public static final int DTYPE_BU = 2;
    public static final int DTYPE_WU = 3;
    public static final int DTYPE_LU = 4;
    public static final int DTYPE_QU = 5;
    public static final int DTYPE_OU = 25;
    public static final int DTYPE_B = 6;
    public static final int DTYPE_W = 7;
    public static final int DTYPE_L = 8;
    public static final int DTYPE_Q = 9;
    public static final int DTYPE_O = 26;
    public static final int DTYPE_FLOAT = 52;
    public static final int DTYPE_DOUBLE = 53;
    public static final int DTYPE_T = 14;
    public static final int DTYPE_IDENT = 191;
    public static final int DTYPE_NID = 192;
    public static final int DTYPE_PATH = 193;
    public static final int DTYPE_PARAM = 194;
    public static final int DTYPE_SIGNAL = 195;
    public static final int DTYPE_DIMENSION = 196;
    public static final int DTYPE_WINDOW = 197;
    public static final int DTYPE_FUNCTION = 199;
    public static final int DTYPE_CONGLOM = 200;
    public static final int DTYPE_RANGE = 201;
    public static final int DTYPE_ACTION = 202;
    public static final int DTYPE_DISPATCH = 203;
    public static final int DTYPE_PROGRAM = 204;
    public static final int DTYPE_ROUTINE = 205;
    public static final int DTYPE_PROCEDURE = 206;
    public static final int DTYPE_METHOD = 207;
    public static final int DTYPE_DEPENDENCY = 208;
    public static final int DTYPE_CONDITION = 209;
    public static final int DTYPE_EVENT = 210;
    public static final int DTYPE_WITH_UNITS = 211;
    public static final int DTYPE_CALL = 212;
    public static final int DTYPE_WITH_ERROR = 213;
    
    
    public static final int CLASS_S = 1;
    public static final int CLASS_D = 2;
    public static final int CLASS_A = 4;
    public static final int CLASS_R = 194;
    public static final int CLASS_APD = 196;
}
    
	
	
    
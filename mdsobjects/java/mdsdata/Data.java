package mdsdata;

public class Data
{
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
    public static final int DTYPE_SLOPE = 198;
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
    public static final int DTYPE_DSC =	24;


    public static final int CLASS_S = 1;
    public static final int CLASS_D = 2;
    public static final int CLASS_A = 4;
    public static final int CLASS_R = 194;
    public static final int CLASS_APD = 196;
    static boolean jniLoaded = false;

    static
    {
        try {
            System.loadLibrary("mdsobjects");
	}catch(Throwable e)
        {
            System.out.println("Error Loading library: "+e);
            e.printStackTrace();
        }
    }
    
    
    int clazz;
    int dtype;
    
    public int getClazz() { return clazz;}
    public int getDType() {return dtype;}
    public boolean isArray()
    {
        return clazz == CLASS_A;
    }
    
    public boolean isScalar()
    {
        return clazz == CLASS_S;
    }
    
    native public String decompile();
    public String toString()
    {
        return decompile();
    }
    public static native Data compileWithArgs(String expression, Data args[]);
    
    public static Data compile(String expr)
    {
        return compileWithArgs(expr, new Data[0]);
    }
    
    public static Data compile(String expr, Data args[])
    {
        return compileWithArgs(expr, args);
    }
    
    public native Data evaluate();
    
    public byte getByte() throws DataException
    {
        throw new DataException(clazz, dtype, "getByte not suppored by this class");
    }
    public short getShort() throws DataException
    {
        throw new DataException(clazz, dtype, "getShort not suppored by this class");
    }
    public int getInt() throws DataException
    {
        throw new DataException(clazz, dtype, "getInt not suppored by this class");
    }
    public long getLong() throws DataException
    {
        throw new DataException(clazz, dtype, "getInt not suppored by this class");
    }
    public float getFloat() throws DataException
    {
        throw new DataException(clazz, dtype, "getFloat not suppored by this class");
    }
    public double getDouble() throws DataException
    {
        throw new DataException(clazz, dtype, "getDouble not suppored by this class");
    }
    public String getString() throws DataException
    {
        throw new DataException(clazz, dtype, "getString not suppored by this class");
    }
     public int[] getDims() throws DataException
    {
        throw new DataException(clazz, dtype, "getDims not suppored by this class");
    }
    public byte[] getByteArray() throws DataException
    {
        throw new DataException(clazz, dtype, "getByteArray not suppored by this class");
    }
    public short[] getShortArray() throws DataException
    {
        throw new DataException(clazz, dtype, "getShortArray not suppored by this class");
    }
    public int[] getIntArray() throws DataException
    {
        throw new DataException(clazz, dtype, "getIntArray not suppored by this class");
    }
    public long[] getLongArray() throws DataException
    {
        throw new DataException(clazz, dtype, "getLongArray not suppored by this class");
    }
    public float[] getFloatArray() throws DataException
    {
        throw new DataException(clazz, dtype, "getFloatArray not suppored by this class");
    }
    public double[] getDoubleArray() throws DataException
    {
        throw new DataException(clazz, dtype, "getDoubleArray not suppored by this class");
    }
  
}
        
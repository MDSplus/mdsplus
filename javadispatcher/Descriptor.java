import java.io.*;

class Descriptor 
{
    public    static final byte MAX_DIM                 = 8;
    public    static final byte DTYPE_BYTE              = 2;
    public    static final byte DTYPE_USHORT            = 3;
    public    static final byte DTYPE_ULONG             = 4;
    public    static final byte DTYPE_ULONGLONG         = 5;
    public    static final byte DTYPE_CHAR              = 6;
    public    static final byte DTYPE_SHORT             = 7;
    public    static final byte DTYPE_LONG              = 8;
    public    static final byte DTYPE_LONGLONG          = 9;
    public    static final byte DTYPE_FLOAT             = 10;
    public    static final byte DTYPE_DOUBLE            = 11;
    public    static final byte DTYPE_COMPLEX           = 12;
    public    static final byte DTYPE_COMPLEX_DOUBLE    = 13;
    public    static final byte DTYPE_CSTRING           = 14;
    public    static final byte DTYPE_EVENT             = 99;
    public    static final byte DTYPE_EVENT_NOTIFY      = 99;



    public    byte   dtype;
    public    float  float_data[];
    public	  int    int_data[];
    public    byte   byte_data[];
    public	  String strdata;
    public	  String error = null;
    public    int    dims[];
    public    int    status;
    
    public Descriptor()
    {
        super();
    }
    
    public Descriptor(String error)
    {
        this.error = error;
    }
    
    public Descriptor(byte dtype, int dims[], byte byte_data[])
    {
        this.dtype = dtype;
        this.dims = dims;
        this.byte_data = byte_data;
    }
    
    public int getStatus() {return status;}
    public int getInt() {return int_data[0]; }
    static byte[] dataToByteArray(Object o)
    {
        byte b[] = null;
        try
        {
        ByteArrayOutputStream dosb = new ByteArrayOutputStream();
        DataOutputStream dos = new DataOutputStream(dosb);
        
        
        if(o instanceof Short)
        {
            short d = ((Short)o).shortValue();
            dos.writeShort(d);
            b = dosb.toByteArray();
        }
        
        if(o instanceof Integer)
        {
            int d = ((Integer)o).intValue();
            dos.writeInt(d);
            b = dosb.toByteArray();
        }
        
        if(o instanceof Float)
        {
            float d = ((Float)o).floatValue();
            dos.writeFloat(d);
            b = dosb.toByteArray();
        }

        if(o instanceof Double)
        {
            double d = ((Double)o).doubleValue();
            dos.writeDouble(d);
            b = dosb.toByteArray();
        }
        
        dos.close();
        return b;
        
        } catch (IOException e){};
        
        return null;
    }
    
    static void main(String arg[])
    {
        byte b[] = dataToByteArray(new Short((short)100));
        System.out.println("  "+b.length);
        b = dataToByteArray(new Float(100));
        System.out.println("  "+b.length);
        b = dataToByteArray(new Double(100));
        System.out.println("  "+b.length);
    }
}

class Descriptor {
public    static final byte MAX_DIM = 8;
public    static final byte DTYPE_CSTRING = 14;
public    static final byte JAVA_CLIENT = 3;
public    static final byte DTYPE_CHAR = 6;
public    static final byte DTYPE_SHORT = 7;
public    static final byte DTYPE_LONG = 8;
public    static final byte DTYPE_FLOAT = 10;

public    byte dtype;
public    double data[];
public	  String strdata;
public	  String error;

public Descriptor()
{
    data = null;
    strdata = null;
    error = null;
}    

}


package MDSplus;

/**
 * Class Data represents the ancestor of every MDSplus data.
 * It defines and implements the generic methods for retrieving data as native
 * types. These methods can be implemented at this generic level since thay are
 * internally based of TDIfunctionality.
 * 
 * Private methods of data will convert the data object instance into a MDSplus
 * descriptor and viceversa: this mechanism allows the usage of TDI. An important
 * fact about Data classes is that no data management code is provided: the
 * classes only priovide a mere interface to the underlying TDI functionality.
 * 
 * As data evaluation is routinely performed, a private field of Data will hold
 * the cached copy of the latest data() result. This cached copy will be returned
 * directly (instead of passing through TdiData()) only if all the components
 * forming this data hierarchy did not change
 * 
 * Private  method mayHaveChanged() defaults to false for most scalar classes
 * except, for example, Nid or Ident.  For composite classes, mayHaveChanged()
 * will return false in most cases only if mayHaveChanges() reutrns false for all
 * the linked data instances are immutable. For some CompoundData classes such as
 * FunctionData whose opcode refers to user provided routines, mayHaveChanged()
 * returns always true.
 * @author manduchi
 * @version 1.0
 * @updated 03-ott-2008 12.23.34
 */
public class Data {
    //Common definitions
    public static final int DTYPE_BU = 2; 
    public static final int DTYPE_WU = 3;
    public static final int DTYPE_LU = 4; 
    public static final int  DTYPE_QU = 5; 
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
    public static final int DTYPE_LIST = 214;
    public static final int DTYPE_TUPLE = 215;
    public static final int DTYPE_DICTIONARY = 216;

    public static final int DTYPE_DSC = 24;		

    public static final int TreeNEGATE_CONDITION = 7;
    public static final int TreeIGNORE_UNDEFINED = 8;
    public static final int TreeIGNORE_STATUS = 9;
    public static final int TreeDEPENDENCY_AND	= 10;
    public static final int TreeDEPENDENCY_OR = 11;


    public static final int CLASS_S = 1; 
    public static final int CLASS_D = 2; 
    public static final int CLASS_A = 4; 
    public static final int CLASS_R = 194; 
    public static final int CLASS_APD = 196; 

    
    
    
	int clazz, dtype;
	Data help, units, error, validation;
        //dataCache contains a chahed copy of the evaluated data. When data has not changed 
        // and is immutable (i.e. it does not refer to something external which may have changed,
        // like a function call) a clone of the cache is returned by method data()
	Data dataCache;
	boolean changed = true;
        //Tree context information. Possibly set by TreeNode.getData() and used in compile(), execute() and data()
        Tree ctxTree = null;
	boolean isImmutable() {return false;}
        boolean hasChanged() {return !isImmutable() || changed;}
        public Data(){}
	public Data(Data help, Data units, Data error, Data validation)
        {
            this.help = help;
            this.units = units;
            this.error = error;
            this.validation = validation;
        }

        public double[]toDouble()throws Exception {return getDoubleArray();}
        public void setCtxTree(Tree ctxTree)
        {
            this.ctxTree = ctxTree;
        }
        
        static {
            try {
              int loaded = 0;
              try {
                java.lang.String value = System.getenv("JavaMdsLib");
                if (value == null) {
                  value = System.getProperty("JavaMdsLib");
                }
                if (value != null) {
                  System.load(value);
                  loaded = 1;
                }
              } catch (Throwable e) {
              }
              if (loaded == 0) {
                System.loadLibrary("JavaMds");
              }
	    }catch(Throwable e)
            {
                  System.out.println("Error loading library javamds: "+e);
                  e.printStackTrace();
            }
	}


	/**
	 * Static method (routine in C++) which compiles the expression (via TdiCompile())
	 * and returns the object instance correspondind to the compiled expression.
	 * 
	 * @param expr
	 * @param args
	 */
	public static Data compile(java.lang.String expr)
        {
            return compile(expr, new Data[0]);
        }
	public static native Data compile(java.lang.String expr, Data[] args);
        public Data executeWithContext(java.lang.String expr, Data[] args)
        {
            if(ctxTree != null && ctxTree.isOpen())
            {
                 try {
                    Data data;
                    Tree currTree = Tree.getActiveTree();
                    Tree.setActiveTree(ctxTree);
                    data = execute(expr, args);
                    Tree.setActiveTree(ctxTree);
                    return data;
                }catch(Exception exc){return execute(expr, args); }
            }
            else
              return  execute(expr, args); 
        }
	public static native Data execute(java.lang.String expr, Data[] args);

	/**
	 * Return the result of TDI decompile(this)
	 */
	public native java.lang.String decompile();

        public java.lang.String toString()
        {
            return decompile();
        }
        
	/**
	 * Method data exports TDI data() functionality, i.e. returns a native type
	 * (scalar or array). If isImmutable() returns true and a cached instance is
	 * avavilable, this is returned instead.
	 */
	public Data data()
        {
            if(hasChanged())
            {
                //Set the right context is any assiotated with this Data instance
                if(ctxTree != null && ctxTree.isOpen())
                {
                    try {
                        Tree currTree = Tree.getActiveTree();
                        Tree.setActiveTree(ctxTree);
                        dataCache = dataData();
                        Tree.setActiveTree(currTree);
                    }catch(Exception exc){}
                }
                else
                    dataCache = dataData();
                changed = false;
            }
            return dataCache.cloneData();
	}

        native Data cloneData();
        native Data dataData();
        
	/**
	 * Return the result of TDI evaluate(this).
	 */
	public String evaluate(){
		return evaluateData();
	}

        native String evaluateData();
        
        native byte[] serialize();
        static native Data deserialize(byte[] serialized);
        
	/**
	 * Convert this data into a byte. Implemented at this class level by returning TDI
	 * data(BYTE(this)). If data() fails or the returned class is not scalar,
	 * generated an exception.
	 */
	public  byte getByte() throws MdsException
        {
            Data data = executeWithContext("BYTE($1)", new Data[]{this});
            if(!(data instanceof Scalar))
                throw new MdsException("Cannot convert Data to byte");
            return data.getByte();
        }

	/**
	 * Convert this data into a short. Implemented at this class level by returning TDI
	 * data(WORD(this)).If data() fails or the returned class is not scalar, generated
	 * an exception.
	 */
	public  short getShort() throws MdsException
        {
            Data data = executeWithContext("WORD($1)", new Data[]{this});
            if(!(data instanceof Scalar))
                throw new MdsException("Cannot convert Data to byte");
            return data.getShort();
        }

	/**
	 * Convert this data into a int. Implemented at this class level by returning TDI
	 * data(LONG(this)).If data() fails or the returned class is not scalar, generated
	 * an exception.
	 */
	public  int getInt() throws MdsException
        {
            Data data = executeWithContext("LONG($1)", new Data[]{this});
            if(!(data instanceof Scalar))
                throw new MdsException("Cannot convert Data to int");
            return data.getInt();
        }


	/**
	 * Convert this data into a long. Implemented at this class level by returning TDI
	 * data(QUADWORD(this)).If data() fails or the returned class is not scalar,
	 * generated an exception.
	 */
	public  long getLong() throws MdsException
        {
            Data data = executeWithContext("QUADWORD($1)", new Data[]{this});
            if(!(data instanceof Scalar))
                throw new MdsException("Cannot convert Data to long");
            return data.getLong();
        }

	/**
	 * Convert this data into a float. Implemented at this class level by returning TDI
	 * data(F_FLOAT(this)).If data() fails or the returned class is not scalar,
	 * generated an exception.
	 */
	public  float getFloat()throws MdsException
        {
            Data data = executeWithContext("FLOAT($1)", new Data[]{this});
            if(!(data instanceof Scalar))
                throw new MdsException("Cannot convert Data to float");
            return data.getFloat();
        }

	/**
	 * Convert this data into a double. Implemented at this class level by returning TDI
	 * data(FT_FLOAT(this)). If data() fails or the returned class is not scalar,
	 * generated an exception.
	 */
	public  double getDouble()throws MdsException
        {
            Data data = executeWithContext("FT_FLOAT($1)", new Data[]{this});
            if(!(data instanceof Scalar))
                throw new MdsException("Cannot convert Data to double");
            return data.getDouble();
        }

	/**
	 * Get the array dimensions as an integer array. It is implemented at this class
	 * level by computing TDI expression SHAPE(this). If shape fails an exception is
	 * generated.
	 */
	public  int[] getShape() throws MdsException
        {
            Data data = executeWithContext("SHAPE($1)", new Data[]{this});
            if(!(data instanceof Array))
                throw new MdsException("Cannot get data shape");
            return data.getIntArray();
            
        }

	/**
	 * Convert this data into a byte array. Implemented at this class level by
	 * returning TDI data(BYTE(this)). If data() fails or the returned class is not
	 * array, generates an exception. In Java and C++ will return a 1 dimensional
	 * array using row-first ordering if a multidimensional array. 
	 */
	public  byte[] getByteArray() throws MdsException
        {
            Data data = executeWithContext("BYTE($1)", new Data[]{this});
            if(!(data instanceof Scalar) && !(data instanceof Array))
                throw new MdsException("Cannot convert Data to byte array");
            return data.getByteArray();
        }


	/**
	 * Convert this data into a short array. Implemented at this class level by
	 * returning TDI data(WORD(this)). If data() fails or the returned class is not
	 * array, generates an exception. In Java and C++ will return a 1 dimensional
	 * array using row-first ordering if a multidimensional array. 
	 */
	public  short[] getShortArray()throws MdsException
        {
            Data data = executeWithContext("WORD($1)", new Data[]{this});
            if(!(data instanceof Scalar) && !(data instanceof Array))
                throw new MdsException("Cannot convert Data to short array");
            return data.getShortArray();
        }


	/**
	 * Convert this data into a int array. Implemented at this class level by
	 * returning TDI data (LONG(this)). If data() fails or the returned class is not
	 * array, generates an exception. In Java and C++ will return a 1 dimensional
	 * array using row-first ordering if a multidimensional array. 
	 */
	public  int[] getIntArray()throws MdsException
        {
            Data data = executeWithContext("LONG($1)", new Data[]{this});
            if(!(data instanceof Scalar) && !(data instanceof Array))
                throw new MdsException("Cannot convert Data to int array");
            return data.getIntArray();
        }

	/**
	 * Convert this data into a long array. Implemented at this class level by
	 * returning TDI data(QUADWORD(this)). If data() fails or the returned class is
	 * not array, generates an exception. In Java and C++ will return a 1 dimensional
	 * array using row-first ordering if a multidimensional array. 
	 */
	public  long[] getLongArray()throws MdsException
        {
            Data data = executeWithContext("QUADWORD($1)", new Data[]{this});
            if(!(data instanceof Scalar) && !(data instanceof Array))
                throw new MdsException("Cannot convert Data to long array");
            return data.getLongArray();
        }

	/**
	 * Convert this data into a float array. Implemented at this class level by
	 * returning TDI data(QUADWORD(this)). If data() fails or the returned class is
	 * not array, generates an exception. In Java and C++ will return a 1 dimensional
	 * array using row-first ordering if a multidimensional array. 
	 */
	public  float[] getFloatArray()throws MdsException
        {
            Data data = executeWithContext("FLOAT($1)", new Data[]{this});
            if(!(data instanceof Scalar) && !(data instanceof Array))
                throw new MdsException("Cannot convert Data to float array");
            return data.getFloatArray();
        }

	/**
	 * Convert this data into a long array. Implemented at this class level by
	 * returning TDI data(QUADWORD(this)). If data() fails or the returned class is
	 * not array, generates an exception. In Java and C++ will return a 1 dimensional
	 * array using row-first ordering if a multidimensional array. 
	 */
	public  double[] getDoubleArray()throws MdsException
        {
            Data data = executeWithContext("FT_FLOAT($1)", new Data[]{this});
            if(!(data instanceof Scalar) && !(data instanceof Array))
                throw new MdsException("Cannot convert Data to double array");
            return data.getDoubleArray();
        }

	/**
	 * Convert this data into a STRING. Implemented at this class level by returning
	 * TDI data((this)). If data() fails or the returned class is not string,
	 * generates an exception.
	 */
	public  java.lang.String getString() throws MdsException
        {
            Data data = data();
            if(!(data instanceof String))
                throw new MdsException("Cannot convert Data to string");
            return data.getString();
                
        }
	public  java.lang.String[] getStringArray()throws MdsException
        {
            Data data = data();
            if(!(data instanceof StringArray))
                throw new MdsException("Cannot convert Data to string array");
            return data.getStringArray();
        }


        public int getSize()
        {
            Data data = data();
            return data.getSize();
        }
        
        public int getSizeInBytes()
        {
            Data data = data();
            return data.getSizeInBytes();
        }
        
        
	/**
	 * Return units field. EmptyData is returned if no units
	 * defined.
	 */
	public Data getUnits()
        {
            return units;
	}

	/**
	 * Returns help). Returns EmptyData if no help field
	 * defined.
	 */
	public Data getHelp()
        {
            return help;
	}

	/**
	 * Get the error field. Returns EmptyData if no error defined.
	 */
	public Data getError()
        {
            return error;
	}
	/**
	 * Get the error field. Returns EmptyData if no error defined.
	 */
	public Data getValidation()
        {
            return validation;
	}

	/**
	 * 
	 * @param units    Set the Units field for this Data instance.
	 */
	public void setUnits(Data units)
        {
            this.units = units;
	}

	/**
	 * Set the Help  field for this Data instance.
	 * 
	 * @param help
	 */
	public void setHelp(Data help)
        {
            this.help = help;
	}

	/**
	 * Set the Error field for this Data instance.
	 * 
	 * @param error
	 */
	public void setError(Data error)
        {
            this.error = error;
	}

	public void setValidation(Data validation)
        {
            this.validation = validation;
	}

        public Data getDimensionAt(int idx)
        {
            return executeWithContext("DIM_OF($)", new Data[]{this});
        }
        public void plot()
        {
            Scope scope = new Scope("", 100, 100, 200, 200);
            try {
                scope.plot(getDimensionAt(0), this, 1, 1, "black");
            }catch(Exception exc){System.err.println("Cannot plot data: " + exc);}
        }
        
        public static Data toData(byte d) {return new Int8(d);}
        public static Data toData(short d) {return new Int16(d);}
        public static Data toData(int d) {return new Int32(d);}
        public static Data toData(long d) {return new Int64(d);}
        public static Data toData(float d) {return new Float32(d);}
        public static Data toData(double d) {return new Float64(d);}
        public static Data toData(java.lang.String d) {return new MDSplus.String(d);}
        
        public static Data toData(byte d[]) {return new Int8Array(d);}
        public static Data toData(short d[]) {return new Int16Array(d);}
        public static Data toData(int d[]) {return new Int32Array(d);}
        public static Data toData(long d[]) {return new Int64Array(d);}
        public static Data toData(float d[]) {return new Float32Array(d);}
        public static Data toData(double d[]) {return new Float64Array(d);}
        public static Data toData(java.lang.String d[]) {return new StringArray(d);}
        //MDSplus time support
        static native java.lang.String convertToDate(long time);
        static native long getTime();
}

package jScope;

/* $Id$ */
import java.util.Vector;

public class MdsDataClient extends MdsConnection
{
    /**
     * This class allows to read the data from an mdsip data server.
     *
     * @param provider String to define the mdsip data server to use: host_address[:port]
     * @exception MdsIOException if an I/0 error occurs
     */
    public MdsDataClient(String provider) throws MdsIOException
    {
        super(provider);
        if(ConnectToMds(false) == 0)
           throw new MdsIOException(error);
    }

    /**
     * This class allows to read the data from an mdsip data server.
     *
     * @param provider String to define the mdsip data server to use: host_address[:port]
     * @param user String to define the mdsip user name
     * @exception MdsIOException if an I/0 error occurs
     */
    public MdsDataClient(String provider, String user) throws MdsIOException
    {
        super(provider);
        setUser(user);
        if(ConnectToMds(false) == 0)
            throw new MdsIOException(error);
    }

	    /**
     * Open an MdsPlus experiment in read only access mode
     *
     * @param experiment Experiment name
     * @param shot Shot number
     * @exception MdsIOException if an I/0 error occurs
     */
    public void open(String experiment, int shot) throws MdsIOException
    {
		open(experiment, shot, 1);
	}

    /**
     * Open an MdsPlus experiment
     *
     * @param experiment Experiment name
     * @param shot Shot number
    * @param readOnly  access mode 1 for read only	
     * @exception MdsIOException if an I/0 error occurs
     */
    public void open(String experiment, int shot, int readOnly) throws MdsIOException
    {
        if(!connected)
            throw new MdsIOException("Mds data client not connected to "+provider);

        Descriptor descr = MdsValue("JavaOpen(\""+experiment+"\"," + shot + ","+ readOnly +")");
	    if(!(descr.dtype != Descriptor.DTYPE_CSTRING
		   && descr.dtype == Descriptor.DTYPE_LONG && descr.int_data != null
		   && descr.int_data.length > 0 && (descr.int_data[0]%2 == 1)))
		{
	        if(error != null)
		        throw new MdsIOException("Cannot open experiment " + experiment + " shot "+
		                            shot + " : "+ error);
	        else
		       throw new MdsIOException("Cannot open experiment " + experiment + " shot "+ shot);
		}
    }

    /**
     * Close currently open experiment
     */
    public void close()
    {
        if(connected)
            DisconnectFromMds();
    }

    /**
     * Evaluate an MdsPlus expression which return a bidimensional float array
     *
     * @param expr expression to evaluate
     * @return bidimensional float array returned by expression evaluation
     * @exception MdsIOException if an I/0 or an expression evaluation error occurs
     */
    public float[][] getFloatMatrix(String expr) throws MdsIOException
    {

        Descriptor desc = MdsValue("shape("+expr+")");

        float out[][] = null;
        int row = 0, col = 0;

        switch(desc.dtype)
        {
	        case Descriptor.DTYPE_FLOAT:
	            throw new MdsIOException("Evaluated expression not a matrix");
	        case Descriptor.DTYPE_LONG:
	            if(desc.int_data.length != 2)
	                throw new MdsIOException("Can be read only bi-dimensional matrix");
                col = desc.int_data[0];
                row = desc.int_data[1];
	            break;
	        case Descriptor.DTYPE_BYTE:
	            throw new MdsIOException("Evaluated expression not a matrix");
	        case Descriptor.DTYPE_CSTRING:
	            if((desc.status & 1) == 0)
	                throw new MdsIOException(desc.error);
	        default:
	            throw new MdsIOException("Evaluated expression not a matrix");
        }

        desc = MdsValue(expr);

        switch(desc.dtype)
        {
	        case Descriptor.DTYPE_FLOAT:
	            out = new float[row][col];
	            for(int i = 0, k = 0; i < row; i++)
	                for(int j = 0; j < col; j++)
		                out[i][j] = desc.float_data[k++];
		        return out;
	        case Descriptor.DTYPE_LONG:
	            out = new float[row][col];
	            for(int i = 0, k = 0; i < row; i++)
	                for(int j = 0; j < col; j++)
		                out[i][j] = (float)desc.int_data[k++];
		        return out;
	        case Descriptor.DTYPE_DOUBLE:
	            out = new float[row][col];
	            for(int i = 0, k = 0; i < row; i++)
	                for(int j = 0; j < col; j++)
		                out[i][j] = (float)desc.double_data[k++];
		        return out;
	        case Descriptor.DTYPE_BYTE:
	            throw new MdsIOException("Cannot convert a string to float array");
	        case Descriptor.DTYPE_CSTRING:
	            if((desc.status & 1) == 0)
	                throw new MdsIOException(desc.error);
	        default:
	            throw new MdsIOException("Data type code "+ desc.dtype+" unsupported");
         }
    }


    /**
     * Evaluate an MdsPlus expression which return a bidimensional double array
     *
     * @param expr expression to evaluate
     * @return bidimensional float array returned by expression evaluation
     * @exception MdsIOException if an I/0 or an expression evaluation error occurs
     */
    public double[][] getDoubleMatrix(String expr) throws MdsIOException
    {

        Descriptor desc = MdsValue("shape("+expr+")");

        double out[][] = null;
        int row = 0, col = 0;

        switch(desc.dtype)
        {
	        case Descriptor.DTYPE_FLOAT:
	            throw new MdsIOException("Evaluated expression not a matrix");
	        case Descriptor.DTYPE_LONG:
	            if(desc.int_data.length != 2)
	                throw new MdsIOException("Can be read only bi-dimensional matrix");
                col = desc.int_data[0];
                row = desc.int_data[1];
	            break;
	        case Descriptor.DTYPE_BYTE:
	            throw new MdsIOException("Evaluated expression not a matrix");
	        case Descriptor.DTYPE_CSTRING:
	            if((desc.status & 1) == 0)
	                throw new MdsIOException(desc.error);
	        default:
	            throw new MdsIOException("Evaluated expression not a matrix");
        }

        desc = MdsValue(expr);

        switch(desc.dtype)
        {
	        case Descriptor.DTYPE_FLOAT:
	            out = new double[row][col];
	            for(int i = 0, k = 0; i < row; i++)
	                for(int j = 0; j < col; j++)
		                out[i][j] = (double)desc.float_data[k++];
		        return out;
	        case Descriptor.DTYPE_LONG:
	            out = new double[row][col];
	            for(int i = 0, k = 0; i < row; i++)
	                for(int j = 0; j < col; j++)
		                out[i][j] = (double)desc.int_data[k++];
		        return out;
	        case Descriptor.DTYPE_DOUBLE:
	            out = new double[row][col];
	            for(int i = 0, k = 0; i < row; i++)
	                for(int j = 0; j < col; j++)
		                out[i][j] = desc.double_data[k++];
		        return out;
	        case Descriptor.DTYPE_BYTE:
	            throw new MdsIOException("Cannot convert a string to float array");
	        case Descriptor.DTYPE_CSTRING:
	            if((desc.status & 1) == 0)
	                throw new MdsIOException(desc.error);
	        default:
	            throw new MdsIOException("Data type code "+ desc.dtype+" unsupported");
         }
    }


    /**
     * Evaluate an MdsPlus expression which return a float array
     *
     * @param expr expression to evaluate
     * @return float array value returned by the expression evaluation
     * @exception MdsIOException if an I/0 or an expression evaluation error occurs
     */
    public float[] getFloatArray(String expr) throws MdsIOException
    {
        Descriptor desc = MdsValue(expr);
        float out[] = null;
        float out_data[] = null;

        switch(desc.dtype)
        {
	        case Descriptor.DTYPE_FLOAT:
	            out = desc.float_data;
	            break;
	        case Descriptor.DTYPE_LONG:
	            out_data = new float[desc.int_data.length];
	            for(int i = 0; i < desc.int_data.length; i++)
		            out_data[i] = (float)desc.int_data[i];
		        out = out_data;
	            break;
	        case Descriptor.DTYPE_DOUBLE:
	            out_data = new float[desc.double_data.length];
	            for(int i = 0; i < desc.double_data.length; i++)
		            out_data[i] = (float)desc.double_data[i];
		        out = out_data;
	            break;
	        case Descriptor.DTYPE_BYTE:
	            throw new MdsIOException("Cannot convert a string to float array");
	        case Descriptor.DTYPE_CSTRING:
	            if((desc.status & 1) == 0)
	                throw new MdsIOException(desc.error);
	        default:
	            throw new MdsIOException("Data type code "+ desc.dtype+" unsupported");
         }

         return out;
    }

    /**
     * Evaluate an MdsPlus expression which return a byte array
     *
     * @param expr expression to evaluate
     * @return float array value returned by the expression evaluation
     * @exception MdsIOException if an I/0 or an expression evaluation error occurs
     */
    public byte[] getByteArray(String expr) throws MdsIOException
    {
        Descriptor desc = MdsValue(expr);
        byte out[] = null;
        byte out_data[] = null;

        switch(desc.dtype)
        {
	        case Descriptor.DTYPE_FLOAT:
	            throw new MdsIOException("Cannot convert a string to byte array");
	        case Descriptor.DTYPE_LONG:
	            throw new MdsIOException("Cannot convert a string to byte array");
	        case Descriptor.DTYPE_DOUBLE:
	            throw new MdsIOException("Cannot convert a string to byte array");
	        case Descriptor.DTYPE_BYTE:
	        case Descriptor.DTYPE_UBYTE:
	            out_data = new byte[desc.byte_data.length];
	            for(int i = 0; i < desc.byte_data.length; i++)
		            out_data[i] = desc.byte_data[i];
		        out = out_data;
                break;
	        case Descriptor.DTYPE_CSTRING:
	            if((desc.status & 1) == 0)
	                throw new MdsIOException(desc.error);
	            out = desc.strdata.getBytes();
	        default:
	            throw new MdsIOException("Data type code "+ desc.dtype+" unsupported");
         }

         return out;
    }

    /**
     * Evaluate an MdsPlus expression which return a int array
     *
     * @param expr expression to evaluate
     * @return float array value returned by the expression evaluation
     * @exception MdsIOException if an I/0 or an expression evaluation error occurs
     */
    public int[] getIntArray(String expr) throws MdsIOException
    {
        Descriptor desc = MdsValue(expr);
        int out_data[] = null;

        switch(desc.dtype)
        {
                case Descriptor.DTYPE_FLOAT:
                    throw new MdsIOException("Cannot convert a float to int array");
                case Descriptor.DTYPE_LONG:
                case Descriptor.DTYPE_ULONG:
                    return desc.int_data;
                case Descriptor.DTYPE_DOUBLE:
                    throw new MdsIOException("Cannot convert a double to int array");
                case Descriptor.DTYPE_UBYTE:
                case Descriptor.DTYPE_BYTE:
                    out_data = new int[desc.byte_data.length];
                    for(int i = 0; i < desc.byte_data.length; i++)
                            out_data[i] = (int)desc.byte_data[i];
                    return out_data;
                case Descriptor.DTYPE_CSTRING:
                    if((desc.status & 1) == 0)
                        throw new MdsIOException(desc.error);
                 default:
                    throw new MdsIOException("Data type code "+ desc.dtype+" unsupported");
         }
    }

    /**
     * Evaluate an MdsPlus expression which return a double array
     *
     * @param expr expression to evaluate
     * @return float array value returned by the expression evaluation
     * @exception MdsIOException if an I/0 or an expression evaluation error occurs
     */
    public double[] getDoubleArray(String expr) throws MdsIOException
    {
        Descriptor desc = MdsValue(expr);
        double out[] = null;
        double out_data[] = null;

        switch(desc.dtype)
        {
	        case Descriptor.DTYPE_FLOAT:
	            out_data = new double[desc.float_data.length];
	            for(int i = 0; i < desc.float_data.length; i++)
		            out_data[i] = (double)desc.float_data[i];
		        out = out_data;
	            break;
	        case Descriptor.DTYPE_LONG:
	            out_data = new double[desc.int_data.length];
	            for(int i = 0; i < desc.int_data.length; i++)
		            out_data[i] = (double)desc.int_data[i];
		        out = out_data;
	            break;
	        case Descriptor.DTYPE_DOUBLE:
	            out = desc.double_data;
	            break;
	        case Descriptor.DTYPE_BYTE:
	            throw new MdsIOException("Cannot convert a string to float array");
	        case Descriptor.DTYPE_CSTRING:
	            if((desc.status & 1) == 0)
	                throw new MdsIOException(desc.error);
	        default:
	            throw new MdsIOException("Data type code "+ desc.dtype+" unsupported");
         }

         return out;
    }


    /**
     * Evaluate an MdsPlus expression which return a float value
     *
     * @param expr expression to evaluate
     * @return float value returned by the expression evaluation
     * @exception MdsIOException if an I/0 or an expression evaluation error occurs
     */
    public float getFloat(String expr) throws MdsIOException
    {

	    Descriptor desc = MdsValue(expr);
	    switch (desc.dtype)
	    {
		    case Descriptor.DTYPE_FLOAT:
		        return desc.float_data[0];
		    case Descriptor.DTYPE_LONG:
		        return (float)desc.int_data[0];
	        case Descriptor.DTYPE_DOUBLE:
	            return (float)desc.double_data[0];
		    case Descriptor.DTYPE_BYTE:
		        throw new MdsIOException("Cannot convert a string to float");
		    case Descriptor.DTYPE_CSTRING:
		        if((desc.status & 1) == 0)
		            throw new MdsIOException(desc.error);
	        default:
	            throw new MdsIOException("Data type code "+ desc.dtype+" unsupported");
	    }
    }

    /**
     * Evaluate an MdsPlus expression which return a double value
     *
     * @param expr expression to evaluate
     * @return float value returned by the expression evaluation
     * @exception MdsIOException if an I/0 or an expression evaluation error occurs
     */
    public double getDouble(String expr) throws MdsIOException
    {

	    Descriptor desc = MdsValue(expr);
	    switch (desc.dtype)
	    {
		    case Descriptor.DTYPE_FLOAT:
		        return (double)desc.float_data[0];
		    case Descriptor.DTYPE_LONG:
		        return (double)desc.int_data[0];
	            case Descriptor.DTYPE_DOUBLE:
	            return desc.double_data[0];
		    case Descriptor.DTYPE_BYTE:
		        throw new MdsIOException("Cannot convert a string to float");
		    case Descriptor.DTYPE_CSTRING:
		        if((desc.status & 1) == 0)
		            throw new MdsIOException(desc.error);
	        default:
	            throw new MdsIOException("Data type code "+ desc.dtype+" unsupported");
	    }
    }


    /**
     * Evaluate an MdsPlus expression which return an integer value
     *
     * @param expr expression to evaluate
     * @return integer value returned by the expression evaluation
     * @exception MdsIOException if an I/0 or an expression evaluation error occurs
     */
    public int getInt(String expr) throws MdsIOException
    {

	    Descriptor desc = MdsValue(expr);
	    switch (desc.dtype)
	    {
                    case Descriptor.DTYPE_UBYTE:
                    case Descriptor.DTYPE_BYTE:
                        return (int)desc.byte_data[0];
		    case Descriptor.DTYPE_FLOAT:
		        return (int)desc.float_data[0];
                    case Descriptor.DTYPE_SHORT:
                    case Descriptor.DTYPE_USHORT:
                        return (int)desc.short_data[0];
		    case Descriptor.DTYPE_LONG:
		        return desc.int_data[0];
		    case Descriptor.DTYPE_CSTRING:
		        if((desc.status & 1) == 0)
		            throw new MdsIOException(desc.error);
	        default:
	            throw new MdsIOException("Data type code "+ desc.dtype+" unsupported");
	    }
    }


    /**
     * Evaluate an MdsPlus expression which return an short value
     *
     * @param expr expression to evaluate
     * @return integer value returned by the expression evaluation
     * @exception MdsIOException if an I/0 or an expression evaluation error occurs
     */
    public short getShort(String expr) throws MdsIOException {

      Descriptor desc = MdsValue(expr);
      switch (desc.dtype) {
        case Descriptor.DTYPE_UBYTE:
        case Descriptor.DTYPE_BYTE:
          return (short) desc.byte_data[0];
        case Descriptor.DTYPE_SHORT:
          return desc.short_data[0];
        case Descriptor.DTYPE_FLOAT:
          throw new MdsIOException("Cannot convert a float to short");
        case Descriptor.DTYPE_LONG:
          throw new MdsIOException("Cannot convert a int to short");
        case Descriptor.DTYPE_CSTRING:
          if ( (desc.status & 1) == 0)
            throw new MdsIOException(desc.error);
        default:
          throw new MdsIOException("Data type code " + desc.dtype +
                                   " unsupported");
      }
    }

    /**
     * Evaluate an MdsPlus expression which return a short array
     *
     * @param expr expression to evaluate
     * @return float array value returned by the expression evaluation
     * @exception MdsIOException if an I/0 or an expression evaluation error occurs
     */
    public short[] getShortArray(String expr) throws MdsIOException
    {
        Descriptor desc = MdsValue(expr);
        short out_data[] = null;

        switch(desc.dtype)
        {
                case Descriptor.DTYPE_FLOAT:
                    throw new MdsIOException("Cannot convert a float to short array");
                case Descriptor.DTYPE_LONG:
                case Descriptor.DTYPE_ULONG:
                    throw new MdsIOException("Cannot convert a long to short array");
                case Descriptor.DTYPE_DOUBLE:
                    throw new MdsIOException("Cannot convert a double to short array");
                case Descriptor.DTYPE_UBYTE:
                case Descriptor.DTYPE_BYTE:
                    out_data = new short[desc.byte_data.length];
                    for(int i = 0; i < desc.byte_data.length; i++)
                            out_data[i] = (short)desc.byte_data[i];
                    return out_data;
                case Descriptor.DTYPE_SHORT:
                case Descriptor.DTYPE_USHORT:
                   return desc.short_data;
                case Descriptor.DTYPE_CSTRING:
                    if((desc.status & 1) == 0)
                        throw new MdsIOException(desc.error);
                 default:
                    throw new MdsIOException("Data type code "+ desc.dtype+" unsupported");
         }
    }



    /**
     * Evaluate an MdsPlus expression which return an long value
     *
     * @param expr expression to evaluate
     * @return integer value returned by the expression evaluation
     * @exception MdsIOException if an I/0 or an expression evaluation error occurs
     */
    public long getLong(String expr) throws MdsIOException
    {

	    Descriptor desc = MdsValue(expr);
	    switch (desc.dtype)
	    {
		    case Descriptor.DTYPE_FLOAT:
		        return (long)desc.float_data[0];
		    case Descriptor.DTYPE_LONG:
		        return (long)desc.int_data[0];
		    case Descriptor.DTYPE_ULONG:
		        return desc.long_data[0];
		    case Descriptor.DTYPE_BYTE:
		        throw new MdsIOException("Cannot convert a string to float");
		    case Descriptor.DTYPE_CSTRING:
		        if((desc.status & 1) == 0)
		            throw new MdsIOException(desc.error);
	        default:
	            throw new MdsIOException("Data type code "+ desc.dtype+" unsupported");
	    }
    }



    /**
     * Evaluate an MdsPlus expression which return a string value
     *
     * @param expr expression to evaluate
     * @return string value returned by the expression evaluation
     * @exception MdsIOException if an I/0 or an expression evaluation error occurs
     */
    public String getString(String expr) throws MdsIOException
    {
	    Descriptor desc = MdsValue(expr);
	    switch(desc.dtype)
	    {
                case Descriptor.DTYPE_UBYTE:
	        case Descriptor.DTYPE_BYTE:
		        return new String(desc.byte_data);
                case Descriptor.DTYPE_LONG:
		        throw new MdsIOException("Cannot convert a integer to string");
	        case Descriptor.DTYPE_FLOAT:
		        throw new MdsIOException("Cannot convert a float to string");
	        case Descriptor.DTYPE_CSTRING:
	            if((desc.status & 1) == 1)
		            return desc.strdata;
	            else
		            throw new MdsIOException(desc.error);
	        default:
	            throw new MdsIOException("Data type code "+ desc.dtype+" unsupported");
	    }
    }

    public Object evaluate(String expr, Vector<Descriptor> args) throws MdsIOException
    {
	    Descriptor desc = MdsValue(expr, args);

	    switch (desc.dtype)
	    {
		    case Descriptor.DTYPE_FLOAT:
		        return Float.valueOf(desc.float_data[0]);
		    case Descriptor.DTYPE_LONG:
		        return Integer.valueOf(desc.int_data[0]);
	            case Descriptor.DTYPE_DOUBLE:
	                return Double.valueOf(desc.double_data[0]);
		    case Descriptor.DTYPE_ULONG:
		        return Long.valueOf(desc.long_data[0]);
		    case Descriptor.DTYPE_BYTE:
		        return Character.valueOf((char)desc.byte_data[0]);
		    case Descriptor.DTYPE_CSTRING:
		        if((desc.status & 1) == 0)
		            throw new MdsIOException(desc.error);
		        return new String( desc.strdata );
	        default:
	            throw new MdsIOException("Data type code "+ desc.dtype+" unsupported");
	    }

    }

    public static void main(String arg[])
    {
        MdsDataClient mdc = null;
        try
        {
            mdc = new MdsDataClient("150.178.3.33");
            System.out.println("OK connessione");

           // float data_f[] = mdc.getFloatArray("sin(0:6.28:0.1)");
           //mdc.open("PR98_TFTR", 102257);
           // System.out.println("OK apertura pulse");
           // float data[][] = mdc.getFloatMatrix(":matrix");
           // double data_2[][] = mdc.getDoubleMatrix("[[1.,2.,3.],[4.,5.,6.],[7.,8.,9.],[10.,11.,12.]]");
           // double data_y[] = mdc.getDoubleArray(".ONED:WTOT");
           // float data_x[] = mdc.getFloatArray("DIM_OF(\\PR98_TFTR::TOP.ONED:WTOT)");
           // for(int i = 0; i < data_x.length; i++)
           //     System.out.println("  "+data_x[i]+"  "+data_y[i]);
           // String s = mdc.getString("\\PR98_TFTR::TOP.COMMENTS:INSTITUTION");
           //  System.out.println(s);
            mdc.close();
            System.exit(1);
        } catch (Exception exc) {
            System.out.println(""+exc);
            if(mdc != null)
                mdc.close();
        }
    }

}

public class MdsDataClient
{
    private MdsConnection mds;
    private String  experiment;
    private int     shot;
    
    
    /**
     * This class allows to read the data from an mdsip data server.
     * 
     * @param provider String to define the mdsip data server to use: host_address[:port]
     * @exception MdsIOException if an I/0 error occurs
     */
    MdsDataClient(String provider) throws MdsIOException
    {
        mds = new MdsConnection(provider);
        if(mds.ConnectToMds(false) == 0)
            throw new MdsIOException(mds.error);
    }

    /**
     * This class allows to read the data from an mdsip data server.
     * 
     * @param provider String to define the mdsip data server to use: host_address[:port]
     * @param user String to define the mdsip user name
     * @exception MdsIOException if an I/0 error occurs
     */
    MdsDataClient(String provider, String user) throws MdsIOException
    {
        mds = new MdsConnection(provider);
        mds.setUser(user);
        if(mds.ConnectToMds(false) == 0)
            throw new MdsIOException(mds.error);
    }

    
    /**
     * Open an MdsPlus experiment
     * 
     * @param experiment Experiment name
     * @param shot Shot number
     * @exception MdsIOException if an I/0 error occurs
     */
    public void open(String experiment, int shot) throws MdsIOException
    {
        if(!mds.connected)
            throw new MdsIOException("Mds data client not connected to "+mds.provider);
        
        this.experiment = experiment;
        this.shot = shot;
	    Descriptor descr = mds.MdsValue("JavaOpen(\""+experiment+"\"," + shot +")");
	    if(!(descr.dtype != Descriptor.DTYPE_CSTRING
		   && descr.dtype == Descriptor.DTYPE_LONG && descr.int_data != null 
		   && descr.int_data.length > 0 && (descr.int_data[0]%2 == 1)))
		{
	        if(mds.error != null)
		        throw new MdsIOException("Cannot open experiment " + experiment + " shot "+ 
		                            shot + " : "+ mds.error);
	        else
		       throw new MdsIOException("Cannot open experiment " + experiment + " shot "+ shot);	    
		}
    }

    /**
     * Close currently open experiment
     */
    public void close()
    {
        if(mds.connected)
            mds.DisconnectFromMds();    
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

        Descriptor desc = mds.MdsValue("shape("+expr+")");
        
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
	        case Descriptor.DTYPE_CHAR:
	            throw new MdsIOException("Evaluated expression not a matrix");
	        case Descriptor.DTYPE_CSTRING:
	            if((desc.status & 1) == 0)
	                throw new MdsIOException(desc.error);
	        default:
	            throw new MdsIOException("Evaluated expression not a matrix");
        }	
        
        desc = mds.MdsValue(expr);
        
        switch(desc.dtype)  
        {
	        case Descriptor.DTYPE_FLOAT:
	            out = new float[row][col];
	            for(int i = 0, k = 0; i < row; i++)
	                for(int j = 0; j < col; j++)
		                out[i][j] = (float)desc.float_data[k++];
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
	        case Descriptor.DTYPE_CHAR:
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

        Descriptor desc = mds.MdsValue("shape("+expr+")");
        
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
	        case Descriptor.DTYPE_CHAR:
	            throw new MdsIOException("Evaluated expression not a matrix");
	        case Descriptor.DTYPE_CSTRING:
	            if((desc.status & 1) == 0)
	                throw new MdsIOException(desc.error);
	        default:
	            throw new MdsIOException("Evaluated expression not a matrix");
        }	
        
        desc = mds.MdsValue(expr);
        
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
	        case Descriptor.DTYPE_CHAR:
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
        Descriptor desc = mds.MdsValue(expr);
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
	        case Descriptor.DTYPE_CHAR:
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
     * Evaluate an MdsPlus expression which return a double array
     * 
     * @param expr expression to evaluate
     * @return float array value returned by the expression evaluation
     * @exception MdsIOException if an I/0 or an expression evaluation error occurs
     */
    public double[] getDoubleArray(String expr) throws MdsIOException
    {
        Descriptor desc = mds.MdsValue(expr);
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
	        case Descriptor.DTYPE_CHAR:
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
        
	    Descriptor desc = mds.MdsValue(expr);
	    switch (desc.dtype) 
	    {
		    case Descriptor.DTYPE_FLOAT:
		        return desc.float_data[0];
		    case Descriptor.DTYPE_LONG:
		        return (float)desc.int_data[0];
	        case Descriptor.DTYPE_DOUBLE: 
	            return (float)desc.double_data[0];
		    case Descriptor.DTYPE_CHAR:
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
        
	    Descriptor desc = mds.MdsValue(expr);
	    switch (desc.dtype) 
	    {
		    case Descriptor.DTYPE_FLOAT:
		        return (double)desc.float_data[0];
		    case Descriptor.DTYPE_LONG:
		        return (double)desc.int_data[0];
	        case Descriptor.DTYPE_DOUBLE: 
	            return desc.double_data[0];
		    case Descriptor.DTYPE_CHAR:
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

	    Descriptor desc = mds.MdsValue(expr);
	    switch (desc.dtype) 
	    {
		    case Descriptor.DTYPE_FLOAT:
		        return (int)desc.float_data[0];
		    case Descriptor.DTYPE_LONG:
		        return desc.int_data[0];
		    case Descriptor.DTYPE_CHAR:
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
	    Descriptor desc = mds.MdsValue(expr);
	    switch(desc.dtype)  
	    {
	        case Descriptor.DTYPE_CHAR:
		        return desc.strdata;
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
    
    static void main(String arg[])
    {
        MdsDataClient mdc = null;
        try
        {
            mdc = new MdsDataClient("tokamak-profiledb.ukaea.org.uk", "pippo");
            System.out.println("OK connessione");
           // float data_f[] = mdc.getFloatArray("sin(0:6.28:0.1)");
            mdc.open("PR98_TFTR", 102257);
            System.out.println("OK apertura pulse");
           // float data[][] = mdc.getFloatMatrix(":matrix");
            double data_2[][] = mdc.getDoubleMatrix("[[1.,2.,3.],[4.,5.,6.],[7.,8.,9.],[10.,11.,12.]]");
            double data_y[] = mdc.getDoubleArray(".ONED:WTOT");
            float data_x[] = mdc.getFloatArray("DIM_OF(\\PR98_TFTR::TOP.ONED:WTOT)");
            for(int i = 0; i < data_x.length; i++)
                System.out.println("  "+data_x[i]+"  "+data_y[i]);
            String s = mdc.getString("\\PR98_TFTR::TOP.COMMENTS:INSTITUTION");
             System.out.println(s);
            mdc.close();
            System.exit(1);
        } catch (MdsIOException exc) {
            System.out.println(""+exc);
            if(mdc != null)
                mdc.close();
        }
    }
    
}
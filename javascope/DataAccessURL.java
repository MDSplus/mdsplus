import java.util.Vector;
import java.io.IOException;

class DataAccessURL  
{
    
    static Vector data_access_vector = new Vector();
        
    static public void addProtocol(DataAccess data_access)
    {
        data_access_vector.addElement(data_access);
    }

    static public Signal getSignal(String url) throws IOException
    {
        return getSignal(url, null, null);
    }

    static public Signal getSignal(String url, String passwd) throws IOException
    {
        return getSignal(url, null, passwd);
    }
    
    static public DataAccess getDataAccess(String url)
    {
        DataAccess da = null;
        
        for(int i = 0 ; i < data_access_vector.size(); da = null, i++)
        {
            da = (DataAccess)data_access_vector.elementAt(i);
            if(da.supports(url))
                break;
        }
        return da;
    }
    
    static public Signal getSignal(String url, String name, String passwd) throws IOException
    {
        DataAccess da = null;
        
        if((da = getDataAccess(url)) != null)
        {
            da.setPassword(passwd);
            Signal s = da.getSignal(url);
            if(s == null && da.getError() == null)
                throw(new IOException("Incorrect password or read signal error"));
            
            if(da.getError() == null)
            {
                if(name == null)
                    name = s.getName();
                    
                if(name == null)
                    name = da.getSignal()+" "+da.getShot();
                else
                    name = name+" "+da.getShot();                
                s.setName(name);
                return s;
            }
            else
            {
                throw(new IOException(da.getError()));
            }
        }
        throw(new IOException("Protocol not recognized"));
    }    


    static public Frames getImages(String url, Frames f) throws IOException
    {
        return getImages(url, null, null, f);
    }

    static public Frames getImages(String url, String passwd, Frames f) throws IOException
    {
        return getImages(url, null, passwd, f);
    }

    static public Frames getImages(String url, String name, String passwd, Frames f) throws IOException
    {
        DataAccess da = null;
        
        if((da = getDataAccess(url)) != null)
        {
            da.setPassword(passwd);
            f = da.getImages(url, f);
            if(f == null && da.getError() == null)
                throw(new IOException("Incorrect password or read images error"));
            
            if(da.getError() == null)
            {
                return f;
            }
            else
            {
                throw(new IOException(da.getError()));
            }
        }
        throw(new IOException("Protocol not recognized"));
    }
    
    
    static public void close()
    {
        DataAccess da = null;
        for(int i = 0 ; i < data_access_vector.size(); i++)
        {
            da = (DataAccess)data_access_vector.elementAt(i);
            if(da != null)
                da.close();
        }
    }


}
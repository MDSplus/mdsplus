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
  
    static public Signal getSignal(String url, String name, String passwd) throws IOException
    {
        DataAccess da = null;
        
        for(int i = 0 ; i < data_access_vector.size(); da = null, i++)
        {
            da = (DataAccess)data_access_vector.elementAt(i);
            if(da.supports(url))
                break;
        }
        if(da != null)
        {
           da.setPassword(passwd);
           
           float y[] = da.getY(url);
           float x[] = da.getX(url);
            
            if(x == null || y == null)
                throw(new IOException("Incorrect password or read signal error"));
            
            Signal s = new Signal(x, y);
            if(da.getError() == null)
            {
                if(name == null)
                {
                    name = da.getSignal()+" "+da.getShot();
                } else
                    name = name+" "+da.getShot();                
                s.setName(name);
                return s;
            }
            else
                throw(new IOException(da.getError()));
        }
        throw(new IOException("Protocol not recognized"));
    }
    
    
}
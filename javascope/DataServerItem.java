public class DataServerItem
{
    String data_server;
    String browse_class;
    String browse_url;
    
    public DataServerItem()
    {
        this(null, null, null);
    }
    
    public DataServerItem(String data_server, String browse_class, String browse_utl)
    {
        this.data_server = data_server;
        this.browse_class = browse_class;
        this.browse_url = browse_url;
    }
            
    public String toString() {return data_server;}
    
        
    public boolean equals(DataServerItem dsi)
    {
        return data_server.equals(dsi.data_server);
    }

    public boolean equals(String data_server)
    {
        return this.data_server.equals(data_server);
    }
        
}

package jScope;

/* $Id$ */
public class DataServerItem
{
    String name;
    String argument;
    String user;
    String class_name;
    String browse_class;
    String browse_url;
    String tunnel_port;


    public DataServerItem(String user)
    {
        this(null, null, user, null, null, null, null);
    }

    public DataServerItem()
    {
        this(null, null, null, null, null, null, null);
    }

    public DataServerItem(String name, String argument, String user,
                          String class_name, String browse_class,
                          String browse_url, String tunnel_port)
    {
        this.name = name;
        this.argument = argument;
        this.user = user;
        this.class_name = class_name;
        this.browse_class = browse_class;
        this.browse_url = browse_url;
        this.tunnel_port = tunnel_port;
    }

    public String toString() {return name;}


    public boolean equals(DataServerItem dsi)
    {
        try
        {
            boolean out = name.equals(dsi.name);
                         //&&
                         // argument.equals(dsi.argument) &&
                         // class_name.equals(dsi.class_name) &&
                         // browse_class.equals(dsi.browse_class);
            return out;
        }
        catch (Exception exc)
        {
            return false;
        }
    }

    public boolean equals(String name)
    {
        return this.name.equals(name);
    }
    public void print()
    {
        System.out.println("name: " + this.name);
        System.out.println("argument: " + this.argument);
        System.out.println("user: " + this.user);
        System.out.println("class_name: " + this.class_name);
        System.out.println("browse_class: " + this.name);
        System.out.println("tunnel_port: " + this.tunnel_port);
    }
}

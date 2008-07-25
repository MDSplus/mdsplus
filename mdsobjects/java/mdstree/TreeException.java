package mdstree;

public class TreeException extends Exception
{
    String message;
    public TreeException(String message) 
    {
        this.message = message;
    }
    
    public String toString()
    {
        return message;
    }
    
}

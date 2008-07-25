package mdsdata;

public class SyntaxException extends Exception
{
    String message;
    public SyntaxException()
    {
        this.message = "TDI Syntax error";
    }
    public String toString(){return message;}
}
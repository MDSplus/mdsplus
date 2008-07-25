package mdsdata;

public class DataException extends Exception
{
    int clazz = 0, dtype = 0;
    String message;
    public DataException(int clazz, int dtype, String message)
    {
        this.clazz = clazz;
        this.dtype = dtype;
        this.message = message;
    }
    public DataException(String message)
    {
        this.message = message;
    }
    public String toString()
    {
        if(clazz != 0)
            return message + " Class = "+clazz + " Dtype = " + dtype;
        else
            return message;
    }
}
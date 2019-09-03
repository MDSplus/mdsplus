//package jTraverser;
import java.util.*;
class DatabaseException
    extends Exception
{
    int status = 0;
    static String getMsg(String message)
    {
	StringTokenizer st = new StringTokenizer(message, ":");
	String statusStr = "";
	if(st.countTokens() > 0)
	    statusStr = st.nextToken();
	return message.substring(statusStr.length() + 1);

    }
    private void setStatus(String message)
    {
	try {
	    StringTokenizer st = new StringTokenizer(message, ":");
	    status = Integer.parseInt(st.nextToken());
	}catch(Exception exc){status = 0;}
    }

    public DatabaseException(String message)
    {
	super(getMsg(message));
	setStatus(message);
    }

    public int getStatus(){return status;}
}
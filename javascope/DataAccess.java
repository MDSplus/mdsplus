import java.net.*;
import java.io.IOException;


public interface DataAccess
{
    float [] getX(String url) throws IOException;
    float [] getY(String url) throws IOException;
    Signal getSignal(String url);
    boolean supports(String url);
    String getError();
    String getSignal();
    String getShot();
    void setPassword(String encoded_credentials);
}
import java.net.*;

public interface DataAccess
{
    float [] getX(String url);
    float [] getY(String url);
    boolean supports(String url);
    String getError();
    String getSignal();
    String getShot();
    void setPassword(String encoded_credentials);
}
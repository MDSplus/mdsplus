import java.net.*;
import java.io.IOException;


public interface DataAccess
{
    float [] getX(String url) throws IOException;
    float [] getY(String url) throws IOException;
    Signal getSignal(String url) throws IOException;
    Frames getImages(String url, Frames f) throws IOException;
    boolean supports(String url);
    String getError();
    String getSignal();
    String getShot();
    String getExperiment();
    DataProvider getDataProvider();
    public String setProvider(String url) throws IOException;
    void setPassword(String encoded_credentials);
    void close();
}
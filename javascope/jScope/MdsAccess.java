package jScope;

/* $Id$ */
import jScope.DataAccess;
import java.util.*;
import java.io.IOException;

public class MdsAccess implements DataAccess
{
    String ip_addr = null;
    String shot_str = null;
    String signal = null;
    String experiment = null;
    MdsDataProvider np = null;
    String error = null;
    String encoded_credentials = null;
	
	String prevUrl = null;

	 public static void main(String[] paramArrayOfString)
  {
    MdsAccess localMdsAccess = new MdsAccess();

    //String str = "mds://raserver.igi.cnr.it:8123/rfx~inProgress/29417/\\DICO28::TOP.SIGNALS.DMIR_A:NE_5";
    String str = "mds://raserver.igi.cnr.it:8123/rfx~inProgress/29417/\\DICO28::COS_CO2_5";

    boolean bool = localMdsAccess.supports(str);
    try
    {
      Signal localSignal = localMdsAccess.getSignal(str);

      float[] arrayOfFloat = localMdsAccess.getX(str);
    }
    catch (IOException localIOException)
    {
    }
  }
    public static void mainOld(String args[])
    {
        MdsAccess access = new MdsAccess();
//      String url = "mds:://150.178.3.80/a/14000/\\emra_it";
//        String url1 = "mds://150.178.34.39/a/19321/\\emra_it";
//      String url = "mds://150.178.34.39/RFX/19321/\\DSTC::VIS_VIDEO_0";
//        String  url = "mds://150.178.34.39/rfx/19321/FramesInterleave(\\dstc::vis_video_1)";
//      String  url = "mds://raserver.igi.cnr.it:8123/rfx~active/29417/getenv(\"dico28_path\")";
        String  url = "mds://raserver.igi.cnr.it:8123/rfx/29417/getenv(\"dico28_path\")";

        boolean supports = access.supports(url);
        try
        {
//          Signal s = access.getSignal(url);
            FrameData fd = access.getFrameData(url);
            //s = access.getSignal(url);
            float x [] = access.getX(url);
            //float y [] = access.getY(url);
         }
        catch (IOException e) {}
    }

    public boolean supports(String url)
    {
        StringTokenizer st = new StringTokenizer(url, ":");
        if(st.countTokens() < 2) return false;
        return st.nextToken().equals("mds");
    }

    
    public void setProvider(String url) throws IOException
    {
        if ((url != null) && (this.prevUrl != null) && (this.prevUrl.equals(url))) {
          return;
        }

        StringTokenizer st = new StringTokenizer(url, ":");
        String urlPath = st.nextToken();
        urlPath = st.nextToken("");
        urlPath = urlPath.substring(2);

        StringTokenizer st1= new StringTokenizer(urlPath, "/");
        if (st1.countTokens() < 4) {
          return;
        }
        String ipAddress = st1.nextToken();
        if (ipAddress == null) return;

        if ((this.ip_addr == null) || (!this.ip_addr.equals(ipAddress)))
        {
          this.np = new MdsDataProvider(ipAddress);
          this.ip_addr = ipAddress;
        }

        String region = null;
        this.experiment = st1.nextToken();
        StringTokenizer localStringTokenizer3 = new StringTokenizer(this.experiment, "~");
        if (localStringTokenizer3.countTokens() == 2)
        {
          this.experiment = localStringTokenizer3.nextToken();
          region = localStringTokenizer3.nextToken();
        }
        if ((this.experiment != null) && (!this.experiment.equals("")))
        {
          this.shot_str = st1.nextToken();
          int shot = Integer.parseInt(this.shot_str);
          if (region != null)
          {
            int out[] = np.GetIntArray("treeSetSource('" + this.experiment + "','" + region + "')");
            if( out == null )
                return;
          }

          this.np.Update(this.experiment, shot, true);
        }
        this.signal = st1.nextToken();
        this.prevUrl = url;
    }

    public void setProviderOld(String url) throws IOException
    {
        StringTokenizer st1 = new StringTokenizer(url, ":");
        String content = st1.nextToken();
        content = st1.nextToken("");
        content = content.substring(2);
        StringTokenizer st2 = new StringTokenizer(content, "/");
        if(st2.countTokens() < 4) //ip addr/exp/shot/signal
            return;
        String addr = st2.nextToken();
        if(addr == null) return;
        if(ip_addr == null || !ip_addr.equals(addr))
        {
            np = new MdsDataProvider(addr);
            /*
            if(encoded_credentials == null ||( ip_addr != null && !ip_addr.equals(addr)))
            {
                encoded_credentials = new String();
                np.InquireCredentials(null, encoded_credentials);
            }
            */
            ip_addr = addr;
        }
        experiment = st2.nextToken();
        if(experiment != null && !experiment.equals(""))
        {
            //String shot_str = st2.nextToken();
            shot_str = st2.nextToken();
            int shot = Integer.parseInt(shot_str);
            np.Update(experiment, shot);
        }
        signal = st2.nextToken();
    }

    public String getShot()
    {
        return shot_str;
    }

    public String getSignalName()
    {
        return signal;
    }

    public String getExperiment()
    {
        return experiment;
    }

    public DataProvider getDataProvider()
    {
        return np;
    }

    public void close()
    {
        if(np != null)
            np.Dispose();
        np = null;
        ip_addr = null;
    }

    public float [] getX(String url) throws IOException
    {
        setProvider(url);
        if(signal == null) return null;
        return np.GetFloatArray("DIM_OF("+signal+")");
    }

    public float [] getY(String url) throws IOException
    {
        setProvider(url);
        if(signal == null) return null;
        return np.GetFloatArray(signal);
    }

	public String getExpression(String paramString) throws IOException
   {
    System.out.println("Expr URL = " + paramString);
    setProvider(paramString);
    if (this.signal == null) return null;
    return this.np.GetStringValue(this.signal);
   }

    public Signal getSignal(String url) throws IOException
    {
        Signal s = null;
        error = null;

         float y[] = getY(url);
        float x[] = getX(url);

		System.out.println("URL = " + url);

        if(x == null || y == null)
        {
            error = np.ErrorString();
            return null;
        }
        s = new Signal(x, y);
        return s;
    }

    public FrameData getFrameData(String url) throws IOException
    {
        setProvider(url);
        return np.GetFrameData(signal, null, (float)-1E8, (float)1E8);
    }

    public void setPassword(String encoded_credentials)
    {
        this.encoded_credentials = encoded_credentials;
    }


    public String getError()
    {
        if(np == null)
           return("Cannot create MdsDataProvider");
        if(error != null)
            return error;
        return np.ErrorString();
    }
}


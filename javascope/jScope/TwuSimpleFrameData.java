package jScope;

// -------------------------------------------------------------------------------------------------
// TwuSimpleFrameData
// A "simple" implementation of "FrameData" for signals from a TEC Web-Umbrella (TWU) server.
//
// $Id$
// 
// -------------------------------------------------------------------------------------------------
import java.awt.Dimension;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.util.StringTokenizer;

class TwuSimpleFrameData
    implements FrameData
{
    String in_x, in_y;
    float time_max, time_min;
    int mode = -1;
    int first_frame_idx = -1;
    byte buf[];
    private int st_idx = -1, end_idx = -1;
    private int n_frames = 0;
    private float times[] = null;
    private TwuDataProvider provider = null;

    public TwuSimpleFrameData (TwuDataProvider dp,
                               String in_y, String in_x, float time_min, float time_max)
        throws IOException
    {
        int i;
        float t;
        float all_times[] = null;

        this.provider = dp;
        
        this.in_y = in_y;
        this.in_x = in_x;
        this.time_min = time_min;
        this.time_max = time_max;

        /* Da modificare per multi frame */
        if(in_x == null || in_x.length() == 0)
          all_times = new float[352/3];
        else
          all_times = provider.GetFloatArray(in_x);

        if(all_times == null)
            throw(new IOException("Frame time evaluation error"));

        for(i = 0; i < all_times.length; i++)
          all_times[i] = (float)(-0.1 + 0.06 * i);

        for(i = 0; i < all_times.length; i++)
        {
            t = all_times[i];
            if(t > time_max)
              break;
            if(t >= time_min)
            {
                if(st_idx == -1) st_idx = i;
            }
        }
        end_idx = i;

        if(st_idx == -1)
          throw(new IOException("No frames found between "+time_min+ " - "+time_max));

        n_frames = end_idx - st_idx;
        times = new float[n_frames];
        int j = 0;
        for(i = st_idx; i < end_idx; i++)
          times[j++] = all_times[i];
    }

    public int GetFrameType()
        throws IOException
    {
        if(mode != -1)
          return mode;
        int i;
        for(i = 0; i < n_frames; i++)
        {
            buf = GetFrameAt(i);
            if(buf != null)
              break;
        }
        first_frame_idx = i;
        mode = Frames.DecodeImageType(buf);
        return mode;
    }

    public int GetNumFrames()
    {
        return n_frames;
    }

    public Dimension GetFrameDimension()
    {
        return null;
    }

    public float[] GetFrameTimes()
    {
        return times;
    }

    public byte[] GetFrameAt(int idx) throws IOException
    {
        if(idx == first_frame_idx && buf != null)
          return buf;

        // b_img = MdsDataProvider.this.GetFrameAt(in_y, st_idx+idx);
        // Da modificare per leggere i frames

        idx *= 3;

        ConnectionEvent ce = new ConnectionEvent(this, "Loading Image "+idx, 0, 0);
        provider.DispatchConnectionEvent(ce);

        StringTokenizer st = new StringTokenizer(in_y, "/", true);
        String str = new String();

        int nt = st.countTokens();
        for(int i = 0; i < nt - 1; i++)
          str = str + st.nextToken();

        String img_name = "00000"+idx;
        img_name = img_name.substring(img_name.length()-6, img_name.length());
        str = str + img_name + ".jpg";

        URL url = new URL(str);
        URLConnection url_con = url.openConnection();
        int size = url_con.getContentLength();

        /* Sometimes size < 0 and an exception is thrown */
        /* Taliercio 27/02/2003 */
        byte b_img[] = null;
        if(size > 0)
        {
            int offset = 0, num_read = 0;

            //byte b_img[] = new byte[size];
            b_img = new byte[size];
            InputStream is = url_con.getInputStream();

            while(size > 0 && num_read != -1)
            {
                num_read = is.read(b_img, offset, size);
                size -= num_read;
                offset += num_read;
            }
        }
        return b_img;
    }
}


// -------------------------------------------------------------------------------------------------
// End of: $Id$
// -------------------------------------------------------------------------------------------------

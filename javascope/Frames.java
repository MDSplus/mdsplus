import java.awt.*;
import java.io.*;
import java.awt.image.*;
import java.util.Vector;

class Frames extends Canvas {
    
    Vector frame = new Vector();
    Vector frame_time = new Vector();
    Rectangle zoom_rect = null;
    Rectangle view_rect = null;
    int curr_frame_idx = -1;
    MediaTracker tracker;
    Dimension d;
    
    Frames()
    {
        super();
        tracker = new MediaTracker(this);
    }
    
    
    protected void finalize()
    {
        frame.removeAllElements();
        frame_time.removeAllElements();
        tracker = null;
    }
     
    public int getNumFrame()
    {
        return frame.size();
    }


    //Find start of frame byte ','= 0x2c is needed to skip
    //extension block start whith '!'=0x21 byte
    //Extension block = !<code><dim><b[]>...<0/dim>[<dim byte><0/dim>]
    private int findStartOfFrame(byte buf[], int start_idx)
    {
        int k;
        for(k = start_idx; k < buf.length && buf[k] != 0x2c; k++)
        {
            if(buf[k] == 0x21)
            {
                //set index on extension block dimension index
                k += 2; 
                //Skip extension block data 
                while(k < buf.length && buf[k] != 0) 
                   k += buf[k] + 1; 
            }
        }
        if(k >= buf.length)
            return -1;
        else
            return k;
    }


    private int findExtensionBlock(byte[] buf, byte b[], int start_idx)
    {        
        for(int i = start_idx, j = 0; i < buf.length; i++)
        {
            if(buf[i] == b[j])
            {
                if(j == b.length - 1)                     
                    // set index on start of estension Block 
                    return i - b.length - 2; 
                j++;                         
            } else {
                if(j != 0) 
                {
                    i--;
                    j = 0;
                }
            }
        }
        
        return -1;
    }
    
    //find start index of next frame and time of previus frame
    private int findNextFrame(byte buf[], int start_idx, float f_t[], int f_idx)
    {
        //MGIFFRAME 4d 47 49 46 46 52 41 4d 45
        byte frame[] = {(byte)0x4d, 
                           (byte)0x47, (byte)0x49, (byte)0x46, (byte)0x46, 
                           (byte)0x52, (byte)0x41, (byte)0x4D, (byte)0x45};
                           
        start_idx = findExtensionBlock(buf, frame, start_idx);
        if(start_idx != -1) {
            int j = start_idx + 13;
            int    ch1 = (buf[j+3] & 0xff) << 24;
            int    ch2 = (buf[j+2] & 0xff) << 16;
            int    ch3 = (buf[j+1] & 0xff) << 8;
            int    ch4 = (buf[j+0] & 0xff) << 0;    
            int    time_ms = (int)((ch1) + (ch2) + (ch3) + (ch4));
            f_t[f_idx] = time_ms/1000.F;
            start_idx = findStartOfFrame(buf, start_idx);
        }
        return start_idx;
    }

    //Find   MGIFHEADER = 4d 4d 47 49 46 48 45 41 44 45 52
    //extension block if not found image isn't a multi frame gif
    private int getNumFrame(byte buf[], int start_idx)
    {
        byte head[] = {(byte)0x4d, 
                           (byte)0x47, (byte)0x49, (byte)0x46, (byte)0x48, 
                           (byte)0x45, (byte)0x41, (byte)0x44, (byte)0x45, 
                           (byte)0x52};
         
        int head_idx = findExtensionBlock(buf, head, start_idx);
        if(head_idx != -1)
        {
            return  buf[head_idx+18];
        }
        return 0;                   
    }

    private int getLong(byte[] b, int i)
    {
        int ch0 = (b[i++] & 0xff);
        int ch1 = (b[i++] & 0xff) << 8;
        return  (ch1) + (ch0);
    }

    public boolean AddMultiFrame(byte[] buf, WaveSetup controller, float timemin, float timemax)
    {
        
        String gif_signature = new String(buf, 0, 6);
        int end_header;
        
        
        if(gif_signature.equals("GIF87a"))
        {
            int color_map_end_idx = (int)Math.pow(2.0, (double)(((buf[10]) &  0x07) + 1));
            int start_idx = (color_map_end_idx + 1) * 3 + 10;

            end_header = start_idx;

            int n_frame = getNumFrame(buf, end_header); 
            if(n_frame == 0)
                return false; //Not a multi frame image
            
            float f_time[] = new float[n_frame];
            
            start_idx = findStartOfFrame(buf, end_header);
           
            int frame_st[] = new int[n_frame + 1];
            int frame_idx = 0;
            while(start_idx != -1)
            {
                frame_st[frame_idx] = start_idx;
                start_idx = findNextFrame(buf, start_idx + 1, f_time, frame_idx);
                frame_idx++;
                if(f_time[frame_idx - 1] > timemax)
                    break;
                controller.DisplayFrameLoad("Extracting frame "+ frame_idx+"/"+(frame_st.length - 1));
            }
            
            frame_st[frame_idx] = buf.length;
            
            int size;
            for(int i = 0; i < frame_st.length - 1; i++)
            {
                if(f_time[i] < timemin)
                    continue;
                if(f_time[i] > timemax)
                    break;
                controller.DisplayFrameLoad("Add frames "+ (i+1)+"/"+ (frame_st.length - 1));
                                
                size = frame_st[i + 1] - frame_st[i];

                ByteArrayOutputStream b_out = new ByteArrayOutputStream(size + end_header + 1);
                b_out.write(buf, 0, end_header);
                b_out.write(buf, frame_st[i], size);
                byte b[] = b_out.toByteArray();
                b[b.length - 1] = 0x00; // Gif terminator
                AddFrame(b, f_time[i]);
 
            }
            
            return true;
        }
        
        return false;
    }


    public boolean AddFrame(byte[] buf, float t)
    {
        Image img;
        
        img = getToolkit().createImage(buf);
        if(img != null)
        {
            AddFrame(img, t);
            tracker.addImage(img, 0);
            return true;
        } else
            return false;
        
    }    

    public void WaitLoadFrame() throws InterruptedException
    {
        tracker.waitForID(0);
    }    

    
    public boolean AddFrame(Object f, float t)
    {
        frame.addElement(f);
        frame_time.addElement(new Float(t));
        
        return true;
    }

    public Object GetFrameAtTime(float t)
    {
        int idx = GetFrameIdxAtTime(t);
        
        if(idx != -1)
            return GetFrame(idx);
        else
            return null;
        
    }
    
    public float GetFrameTime()
    {
        float t_out = 0;
        if(curr_frame_idx != -1 && frame_time.size() != 0)
        {
            t_out = ((Float)frame_time.elementAt(curr_frame_idx)).floatValue();
        }
        
        return t_out;
    }
    
    public Point GetFramePoint(Point p, Dimension d)
    {
        Point p_out = new Point(0, 0);
        
        if(curr_frame_idx != -1 && frame.size() != 0)
        {
            Dimension dim ;
            
            if(zoom_rect == null)
            {
                dim = GetFrameDim(curr_frame_idx);
            } else {
                dim = new Dimension(zoom_rect.width, zoom_rect.height);
                p_out.x = zoom_rect.x;
                p_out.y = zoom_rect.y;
            }

            double ratio_x = (double)dim.width/ d.width;
            double ratio_y = (double)dim.height/ d.height;
            
            p_out.x += ratio_x * p.x;
            p_out.y += ratio_y * p.y;
        }
        
        return p_out;
    }
    
    public float GetTime(int frame_idx)
    {
        if(frame_idx > frame.size() - 1 ) return (float)0.0;
        return ((Float)frame_time.elementAt(frame_idx)).floatValue();
    }
    
    public int GetFrameIdxAtTime(float t)
    {
        int idx = 0;

        if(frame.size() < 0)
            return -1;

        
        if(frame.size() == 1)
            return 0;
  
        if(t > ((Float)frame_time.elementAt(frame.size()-1)).floatValue())
            return frame.size()-1;

        for(int i = 0; i < frame.size() - 1; i++)
        {
                
            if( t >= ((Float)frame_time.elementAt(i)).floatValue() && 
                t < ((Float)frame_time.elementAt(i + 1)).floatValue())
            {
                idx = i;
                break;
            }  
        }
        
        return idx;
                
    }
    
    protected Dimension GetFrameDim(int idx)
    {
       return  new Dimension( ((Image)frame.elementAt(idx)).getWidth(this),
                              ((Image)frame.elementAt(idx)).getHeight(this));
    }
    
    public void SetZoomRegion(int idx, Dimension d, Rectangle r)
    {
        if(idx > frame.size() - 1 || frame.elementAt(idx) == null ) 
            return;

        Dimension dim;
        
        if(zoom_rect == null)
        {
            zoom_rect = new Rectangle(0, 0, 0, 0);
            dim = GetFrameDim(idx);
        } else {
            dim = new Dimension (zoom_rect.width, zoom_rect.height);
        }

        double ratio_x = (double)dim.width/ d.width;
        double ratio_y = (double)dim.height/ d.height;
        
            
        zoom_rect.width = (int)(ratio_x * r.width + 0.5);
        zoom_rect.height = (int)(ratio_y * r.height + 0.5);
        zoom_rect.x += ratio_x * r.x + 0.5;
        zoom_rect.y += ratio_y * r.y + 0.5;
                
        curr_frame_idx = idx;
    }

    public Rectangle GetZoomRect()
    {
        return zoom_rect;
    }

    public void SetViewRect(int start_x, int start_y, int end_x, int end_y)
    {
        view_rect = null;
        if(start_x == -1 && start_y == -1 && end_x == -1 && end_y == -1)
            return;
        if(getNumFrame() == 0)
            return;

        Dimension  dim = GetFrameDim(0);
            
        if(start_x < 0)
            start_x = 0;
        if(start_y < 0)
            start_y = 0;
        if(end_x == -1 || end_x > dim.width)
            end_x = dim.width;
        if(end_y == -1 || end_y > dim.height)
            end_y = dim.height;

        if(start_x < end_x && start_y < end_y)
        {
            view_rect = new Rectangle(start_x, start_y, end_x-start_x, end_y-start_y);
            zoom_rect = view_rect;
        }                           
    }

    public void Resize()
    {
       zoom_rect = null;
    }
    
    public int GetFrameIdx()
    {
       return curr_frame_idx;
    }
    
    public Object GetFrame(int idx, Dimension d)
    {       
        return GetFrame(idx);
    }

    public Object GetFrame(int idx)
    {
        int count;
        if(idx >= frame.size())
            idx = frame.size() - 1;
        if(idx < 0 || frame.elementAt(idx) == null) return null;
        curr_frame_idx = idx;
        return frame.elementAt(idx);
    }
}
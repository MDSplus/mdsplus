import java.awt.*;
import java.io.*;
import java.awt.image.*;
import java.util.*;

class Frames extends Canvas {
    
    Vector frame = new Vector();
    Vector frame_time = new Vector();
    Rectangle zoom_rect = null;
    Rectangle view_rect = null;
    int curr_frame_idx = -1;
    MediaTracker tracker;
    Dimension d;
    ColorModel c_model = null;
    
    Frames()
    {
        super();
        tracker = new MediaTracker(this);
        byte rgb[] = new byte[256], b = 0;
        for(int i = 0; i < 256; i++, b++)
            rgb[i] = b;                   
        c_model = new IndexColorModel(8, 256, rgb, rgb, rgb);
   }
    
    Frames(Frames frames)
    {
        this();
        Image img;
        
        frame.removeAllElements();
        frame_time.removeAllElements();
        int num_frame = frames.getNumFrame();
        for(int i = 0; i < num_frame; i++)
        {
            img = getToolkit().createImage(((Image)frames.GetFrame(i)).getSource());
            this.AddFrame(img, frames.GetTime(i));
            tracker.addImage(img, 0);
        }
        if(frames.zoom_rect != null)
            zoom_rect = new Rectangle(frames.zoom_rect);
        if(frames.view_rect != null)
            view_rect = new Rectangle(frames.view_rect);
        curr_frame_idx = frames.curr_frame_idx;
        try
        {
            WaitLoadFrame();
        } catch (InterruptedException e) {}
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

    
    public boolean AddMultiFrame(byte[] buf, /* WaveSetup controller,*/ float timemin, float timemax)
    {

        try
        {
            MemoryImageSource source;
            ByteArrayInputStream b = new ByteArrayInputStream(buf);
            DataInputStream d = new DataInputStream(b);
            
            int width = d.readInt();
            int height = d.readInt();
            int img_size = height*width;
            int n_frame = d.readInt();
            float f_time[] = new float[n_frame];
            for(int i = 0; i < n_frame; i++)
                f_time[i] = d.readFloat();
            
            byte b_img[];
            Image img;
            for(int i = 0; i < n_frame; i++)
            {
                //controller.DisplayFrameLoad("Extracting frame "+ i+"/"+n_frame);
                if(f_time[i] < timemin || f_time[i] > timemax)
                {
                    d.skip(img_size);
                    continue;
                }
                b_img = new byte[img_size];
                d.read(b_img);
                
                source = new MemoryImageSource(width, height, c_model, b_img, 0, width);
                img = createImage(source);
                AddFrame(img, f_time[i]);
            }
            d.close();
            
        } catch(IOException e)
        {
            System.out.println("Errore "+e);
            return false;
        }
        return true;
    }


    public boolean AddFrame(Image img, float t)
    {
        if(img != null)
        {
            AddFrame((Object)img, t);
            tracker.addImage(img, 0);
            return true;
        } else
            return false;
    }

    public boolean AddFrame(byte[] buf, float t)
    {
        Image img;
        if(buf == null) return false;
        img = getToolkit().createImage(buf);
        return AddFrame(img, t);
        
    }    

    public boolean AddFrame(Object f, float t)
    {
        frame.addElement(f);
        frame_time.addElement(new Float(t));        
        return true;
    }

    public void WaitLoadFrame() throws InterruptedException
    {
        tracker.waitForID(0);
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
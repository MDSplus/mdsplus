import java.awt.*;
import java.awt.image.*;
import java.util.Vector;

class Frames extends Canvas {
    
    Vector frame = new Vector();
    Vector frame_time = new Vector();
    Rectangle zoom_rect = null;
    Rectangle view_rect = null;
    int curr_frame_idx = -1;
    MediaTracker tracker;
    
    
    Frames()
    {
        super();
        tracker = new MediaTracker(this);
    }
    
     
    public int getNumFrame()
    {
        return frame.size();
    }

    public void AddFrame(byte[] buf, float t)
    {
        Image img;
        
        img = getToolkit().createImage(buf);
        AddFrame(img, t);
        tracker.addImage(img, 0);        
    }    

    public void WaitLoadFrame() throws InterruptedException
    {
        tracker.waitForID(0);
    }    

    
    public void AddFrame(Image f, float t)
    {
        frame.addElement(f);
        frame_time.addElement(new Float(t));
    }

    public Image GetFrameAtTime(float t)
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
            Dimension dim = new Dimension();
            
            if(zoom_rect == null)
            {
                dim.width = ((Image)frame.elementAt(curr_frame_idx)).getWidth(this);
                dim.height = ((Image)frame.elementAt(curr_frame_idx)).getHeight(this);
            } else {
                dim.width = zoom_rect.width - zoom_rect.x;
                dim.height = zoom_rect.height - zoom_rect.y;
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
        int idx = -1;
        
        if(frame.size() == 1)
            return 0;

        for(int i = 0; i < frame.size() - 1; i++)
        {
                
            if( t >= ((Float)frame_time.elementAt(i)).floatValue() && 
                t < ((Float)frame_time.elementAt(i + 1)).floatValue())
                idx = i;
/*            
            if( t > ((Float)frame_time.elementAt(i)).floatValue() && 
                t < ((Float)frame_time.elementAt(i + 1)).floatValue())
            {
                if(Math.abs(t - ((Float)frame_time.elementAt(i)).floatValue()) > 
                    Math.abs(t - ((Float)frame_time.elementAt(i + 1)).floatValue()))
                {
                    idx = i+1;
                } else {
                    idx = i;
                }
                break;
            }
 */
        }
        
        if(idx != curr_frame_idx)
            return idx;
        else
            return -1;
    }
    
    
    public Image GetZoomFrame(int idx, Dimension d, Rectangle r)
    {
        if(idx > frame.size() - 1 || frame.elementAt(idx) == null ) return null;


        Dimension dim = new Dimension();
        
        if(zoom_rect == null)
        {
            zoom_rect = new Rectangle(0, 0, 0, 0);
            dim.width = ((Image)frame.elementAt(idx)).getWidth(this);
            dim.height = ((Image)frame.elementAt(idx)).getHeight(this);
        } else {
            dim.width = zoom_rect.width - zoom_rect.x;
            dim.height = zoom_rect.height - zoom_rect.y;
        }

        double ratio_x = (double)dim.width/ d.width;
        double ratio_y = (double)dim.height/ d.height;
        
            
        zoom_rect.width = zoom_rect.x + (int)(ratio_x * r.width);
        zoom_rect.height = zoom_rect.y + (int)(ratio_y * r.height);
        zoom_rect.x += ratio_x * r.x;
        zoom_rect.y += ratio_y * r.y;
        
        
        curr_frame_idx = idx;
        return GetFrame(idx);        
    }

    public Rectangle GetZoomRect()
    {
        return zoom_rect;
    }

    public void SetViewRect(int start_x, int start_y, int end_x, int end_y)
    {
        
        if(start_x == -1 && start_y == -1 && end_x == -1 && end_y == -1)
            return;
        if(getNumFrame() == 0)
            return;
                    
        if(start_x == -1)
            start_x = 0;
        if(start_y == -1)
            start_y = 0;
        if(end_x == -1)
            start_x = ((Image)frame.elementAt(0)).getWidth(this);
        if(end_y == -1)
            start_y = ((Image)frame.elementAt(0)).getHeight(this);
        
        view_rect = new Rectangle(start_x, start_y, end_x, end_y);
        zoom_rect = view_rect;
    }

    public void Resize()
    {
       zoom_rect = null;
    }
    
    public int GetFrameIdx()
    {
       return curr_frame_idx;
    }

    public Image GetFrame(int idx)
    {
        int count;
        
        if(idx >= frame.size())
            idx = frame.size() - 1;
        if(idx < 0 || frame.elementAt(idx) == null) return null;
        curr_frame_idx = idx;
        return (Image)frame.elementAt(idx);
    }

}
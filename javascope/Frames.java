import java.awt.*;
import java.awt.image.*;
import java.util.Vector;

class Frames extends Canvas {
    
    Vector frame = new Vector();
    Vector frame_time = new Vector();
    Rectangle zoom_rect = null;
    Rectangle prev_rect = null;
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
        
        prev_rect = new Rectangle(zoom_rect);
        
        curr_frame_idx = idx;
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
        
        view_rect = new Rectangle(start_x, start_y, start_x - end_x, start_y - end_y);
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
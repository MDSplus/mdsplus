import java.awt.*;
import java.io.*;
import java.awt.image.*;
import java.util.*;

class Frames extends Canvas {
    
    static final int ROI = 20;
    Vector frame = new Vector();
    Vector frame_time = new Vector();
    Rectangle zoom_rect = null;
    Rectangle view_rect = null;
    int curr_frame_idx = -1;
    MediaTracker tracker;
    Dimension d;
    ColorModel c_model = null;
    protected boolean aspect_ratio = true;
    protected int curr_grab_frame = -1;
    protected int[] pixel_array;
    protected int img_width;
    protected int img_height;
    protected int[] frames_pixel_array;
    protected Rectangle frames_pixel_roi;
    protected int x_measure_pixel = 0, y_measure_pixel = 0;
    protected float ft[] = null;
    protected Point sel_point = null;
    protected boolean horizontal_flip = false;
    protected boolean vertical_flip = false;
    protected int color_idx;
    
    Frames()
    {
        super();
        tracker = new MediaTracker(this);
    }
    
    Frames(Frames frames)
    {
        this();
        Image img;
        
        if(frame.size() != 0)
            frame.removeAllElements();
        if(frame_time.size() != 0)
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

    static int DecodeImageType(byte buf[])
    {
        String s = new String(buf, 0, 20);
        if(s.indexOf("GIF") == 0)
            return FrameData.AWT_IMAGE;
        if(s.indexOf("JFIF") == 6)
            return FrameData.AWT_IMAGE;

        return FrameData.JAI_IMAGE;
    }
    
    public void CreateColorModel(int mode)
    {
        int pixel_size;
        switch(mode)
        {
            case FrameData.BITMAP_IMAGE_8 : pixel_size = 8; break;
            case FrameData.BITMAP_IMAGE_16 : pixel_size = 16; break;
            case FrameData.BITMAP_IMAGE_32 : pixel_size = 32; break;
            default: return;
        }
        byte rgb[] = new byte[256], b = 0;
        for(int i = 0; i < 256; i++, b++)
            rgb[i] = b;                   
        c_model = new IndexColorModel(pixel_size, 256, rgb, rgb, rgb);
    }
    
    protected void finalize()
    {
        if(frame.size() != 0)
            frame.removeAllElements();
        if(frame_time.size() != 0)
            frame_time.removeAllElements();
        tracker = null;
    }
     
    public void SetFrameData(FrameData fd) throws IOException
    {
        int n_frames = fd.GetNumFrames();
        float t[] = fd.GetFrameTimes();
        byte[] buf;
        
        CreateColorModel(fd.GetFrameType());       
        for(int i = 0; i < n_frames; i++)
        {
            buf = fd.GetFrameAt(i);
            if(buf == null)
                continue;
            switch(fd.GetFrameType())
            {
                case FrameData.BITMAP_IMAGE_8 :
                    FlipFrame(buf, fd.GetFrameDimension());
                    AddBITMAPImage(buf, fd.GetFrameDimension(), t[i]);
                break;
                case FrameData.AWT_IMAGE :
//                    AddAWTImage(fd.GetFrameAt(i), t[i]);
                    AddAWTImage(buf, t[i]);
                break;
                case FrameData.JAI_IMAGE :
//                    AddJAIImage(fd.GetFrameAt(i), t[i]);
                    AddJAIImage(buf, t[i]);
                break;
            }
        }
    }

    public boolean AddBITMAPImage(byte[] buf, Dimension d, float t)
    {
        Image img;
        MemoryImageSource source;
        if(buf == null || d == null) return false;
        source = new MemoryImageSource(d.width, d.height, c_model, buf, 0, d.width);
        img = createImage(source);
        return AddFrame(img, t);        
    }        

    public boolean AddAWTImage(byte[] buf, float t)
    {
        Image img;
        if(buf == null) return false;
        img = getToolkit().createImage(buf);
        return AddFrame(img, t);        
    }    

    public boolean AddJAIImage(byte[] buf, float t)
    {
        return false;
    }    
    
    public void SetColorIdx(int color_idx)
    {
        this.color_idx = color_idx;
    }

    public int GetColorIdx()
    {
        return color_idx;
    }

     
    public int getNumFrame()
    {
        return frame.size();
    }

    public boolean getAspectRatio()
    {
        return aspect_ratio;
    }
    
    public void setAspectRatio(boolean aspect_ratio)
    {
        this.aspect_ratio = aspect_ratio;
    }
    
    public void FlipFrame(byte buf[], Dimension d)
    {
        if(!vertical_flip && !horizontal_flip)
            return;

        int img_size = d.height*d.width;
        byte tmp[] = new byte[img_size];
        int i, j , k , ofs;
        
        int h = vertical_flip ? d.height - 1: 0;
        int w = horizontal_flip ? d.width - 1: 0;
                
        for(j = 0; j < d.width; j++)
            for(k = 0; k < d.height; k++)
                tmp[(Math.abs(h - k) * d.width) +  Math.abs(w - j)] = buf[(k * d.width) + j];
        System.arraycopy(tmp, 0, buf, 0, img_size);
    }
    
    public void flipFrames(byte buf[])
    {
        if(!vertical_flip && !horizontal_flip)
            return;
        
        try
        {
        ByteArrayInputStream b = new ByteArrayInputStream(buf);
        DataInputStream d = new DataInputStream(b);

            
        int width = d.readInt();
        int height = d.readInt();
        int img_size = height*width;
        int n_frame = d.readInt();
   
        d.close();
        
        byte tmp[] = new byte[img_size];
        int i, j , k , ofs;
        
        int h = vertical_flip ? height - 1: 0;
        int w = horizontal_flip ? width - 1: 0;
        
        ofs = 12 + 4 * n_frame;
        
        for(i = 0; i < n_frame; i++)
        {
            for(j = 0; j < width; j++)
                for(k = 0; k < height; k++)
                   tmp[(Math.abs(h - k) * width) +  Math.abs(w - j)] = buf[ofs + (k * width) + j];
            System.arraycopy(tmp, 0, buf, ofs, img_size);
            ofs += img_size; 
        }
        } catch (IOException e) {}
    }
    
    
    public boolean AddMultiFrame(byte[] buf, float timemin, float timemax)
    {

        try
        {
            flipFrames(buf);
            
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
            
        } 
        catch(IOException e)
        {
            System.out.println("Errore "+e);
            return false;
        }
        return true;
    }


    public boolean AddFrame(byte[] buf, float t)
    {
        Image img;
        if(buf == null) return false;
        img = getToolkit().createImage(buf);
        return AddFrame(img, t);
        
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

    protected int[] getPixelArray(int idx, int x, int y, int img_w, int img_h)
    {
       Image img = (Image)frame.elementAt(idx);
       if(img_w == -1 && img_h == -1)
       {
            img_width = img_w = img.getWidth(this); 
            img_height = img_h = img.getHeight(this);
       }
       int pixel_array[] = new int[img_w * img_h]; 
       PixelGrabber grabber = new PixelGrabber(img, x, y, img_w, img_h, pixel_array,0,img_w);//idth);    
       try 
       { 
          grabber.grabPixels(); 
       } catch(InterruptedException ie) { 
            System.err.println("Pixel array not completed"); 
            return null; 
       }
       return pixel_array;
    }
    
    protected void grabFrame()
    {
        if(curr_frame_idx != curr_grab_frame)
        {
            if( (pixel_array = getPixelArray(curr_frame_idx, 0, 0, -1, -1)) != null)
                curr_grab_frame = curr_frame_idx;
        }
    }

    public boolean isInImage(int idx, int x, int y)
    {
        Dimension d = this.GetFrameDim(idx);
        Rectangle r = new Rectangle(0,0,d.width,d.height);
        return r.contains(x,y);        
    }

    public void setHorizontalFlip(boolean horizontal_flip) {this.horizontal_flip = horizontal_flip;}
    public void setVerticalFlip(boolean vertical_flip) {this.vertical_flip = vertical_flip;}
    public boolean getHorizontalFlip() {return horizontal_flip;}
    public boolean getVerticalFlip() {return vertical_flip;}

    public int getPixel(int idx, int x, int y)
    {        
        if(!isInImage(idx, x, y))
            return -1;
        
        if(idx != curr_grab_frame)
        {
            if( (pixel_array = getPixelArray(idx, 0, 0, -1, -1)) != null)
                curr_grab_frame = idx;
        }
        return pixel_array[(y * img_width) + x];
    }
    
    public int getStartPixelX()
    {
       if(zoom_rect != null)
          return zoom_rect.x;
       else 
          return 0;
    }

    public int getStartPixelY()
    {
       if(zoom_rect != null)
          return zoom_rect.y;
       else 
          return 0;
    }

    public int[] getPixelsLine(int st_x, int st_y, int end_x, int end_y)
    {
        int n_point = (int) (Math.sqrt( Math.pow((double)(st_x - end_x), 2.0) + Math.pow((double)(st_y - end_y), 2.0)) + 0.5);
        int e_x, s_x, x, y;
        int pixels_line[] = {pixel_array[(st_y * img_width) + st_x],
                             pixel_array[(st_y * img_width) + st_x]};
                        
        grabFrame();
        if(n_point < 2)
            return pixels_line;
        
        pixels_line = new int[n_point];
        
        for(int i = 0; i < n_point; i++)
        {
            //y = (int)(st_y + (i - st_x) * ((double)end_y - st_y)/(end_x - st_x));
            x = (int)( st_x + (double)i * (end_x - st_x)/n_point);
            y = (int)( st_y + (double)i * (end_y - st_y)/n_point);
            pixels_line[i] = pixel_array[(y * img_width) + x]; 
        }
        return pixels_line;
    }
    
    public int[] getPixelsX(int y)
    {
        int pixels_x[] = null;
        int st, end;

        grabFrame();
        
        if(pixel_array != null && y < img_height)
        {
           if(zoom_rect != null)
           {
             st = zoom_rect.x;
             end = zoom_rect.x + zoom_rect.width;
           } else {
             st = 0;
             end = img_width;
           }
           pixels_x = new int[end - st];
           for(int i = st, j = 0; i < end; i++, j++)
           {
              pixels_x[j] = pixel_array[(y * img_width) + i]; 
           }
        }
        return pixels_x;
    }

    public int[] getPixelsY(int x)
    {
        int pixels_y[] = null;
        int st, end;

        grabFrame();
        
        if(pixel_array != null && x < img_width)
        {
           if(zoom_rect != null)
           {
             st = zoom_rect.y;
             end = zoom_rect.y + zoom_rect.height;
           } else {
             st = 0;
             end = img_height;
           }
           pixels_y = new int[end - st];
           for(int i = st, j = 0; i < end; i++, j++)
           {
              pixels_y[j] = pixel_array[(i * img_width) + x]; 
           }
        }
        return pixels_y;
    }

    public int[] getPixelsSignal(int x, int y)
    {
        int pixels_signal[] = null;
        if(frames_pixel_array == null || !frames_pixel_roi.contains(x, y))
        {
            frames_pixel_roi = new Rectangle();
            if(zoom_rect == null)
            {
                zoom_rect = new Rectangle(0, 0, img_width, img_height);
            }
            frames_pixel_roi.x = (x - ROI >= zoom_rect.x ? x - ROI : zoom_rect.x);
            frames_pixel_roi.y = (y - ROI >= zoom_rect.y ? y - ROI : zoom_rect.y);
            frames_pixel_roi.width = ( frames_pixel_roi.x + 2 * ROI <= zoom_rect.x + zoom_rect.width ? 2 * ROI : zoom_rect.width - (frames_pixel_roi.x - zoom_rect.x));
            frames_pixel_roi.height = ( frames_pixel_roi.y + 2 * ROI <= zoom_rect.y + zoom_rect.height ? 2 * ROI : zoom_rect.height - (frames_pixel_roi.y - zoom_rect.y));

            frames_pixel_array = new int[frames_pixel_roi.width * frames_pixel_roi.height * getNumFrame()];
            int f_array[];
            for(int i = 0; i < getNumFrame(); i++)
            {
                f_array = getPixelArray(i, frames_pixel_roi.x, frames_pixel_roi.y, frames_pixel_roi.width, frames_pixel_roi.height);
                System.arraycopy(f_array, 0, frames_pixel_array, f_array.length * i, f_array.length);
            }             
        }
        
        if(frames_pixel_array != null)
        {
            x -= frames_pixel_roi.x;
            y -= frames_pixel_roi.y;
            int size = frames_pixel_roi.width * frames_pixel_roi.height;
            pixels_signal = new int[getNumFrame()];
            for (int i = 0; i < getNumFrame(); i++)
            {
                pixels_signal[i] = frames_pixel_array[size * i + (y * frames_pixel_roi.width) + x];
            }
        }
        
        return pixels_signal;
    }

    public Object GetFrameAtTime(float t)
    {
        int idx = GetFrameIdxAtTime(t);
        
        if(idx != -1)
            return GetFrame(idx);
        else
            return null;
        
    }
    
    public float[] getFramesTime()
    {
        if(frame_time == null || frame_time.size() == 0)
            return null;
            
        if(ft == null)
        {
            //float frames_time[] = new float[frame_time.size()];
            ft = new float[frame_time.size()];
            for(int i = 0; i < frame_time.size(); i++)
            {
                ft[i] = ((Float)frame_time.elementAt(i)).floatValue();
            }
        }
        return ft;
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
    
    //return point position in the show frame
    public Point getImagePoint(Point p, Dimension d)
    {
        Point p_out = new Point(0, 0);
        Dimension fr_dim = getFrameSize(curr_frame_idx, d);
        
        if(curr_frame_idx != -1 && frame.size() != 0)
        {
            Dimension view_dim;
            Dimension dim;
            
            if(zoom_rect == null)
            {
                view_dim = GetFrameDim(curr_frame_idx);
                dim = view_dim;
            } else {
                dim = new Dimension(zoom_rect.width, zoom_rect.height);
                view_dim = new Dimension(zoom_rect.x+zoom_rect.width, zoom_rect.y+zoom_rect.height);
                p.x -= zoom_rect.x;
                p.y -= zoom_rect.y;
            }

            double ratio_x = (double)fr_dim.width/ dim.width;
            double ratio_y = (double)fr_dim.height/ dim.height;
            
            p_out.x = (int) (ratio_x * p.x + ratio_x/2);
           
           
            if(p_out.x > (dim.width-1)*ratio_x)
            {
                p_out.x = 0;
                p_out.y = 0;
            } 
            else
            {
                                        
                p_out.y = (int) (ratio_y * p.y + ratio_y/2);
                    
                if(p_out.y > (dim.height-1)*ratio_y)
                {
                    p_out.x = 0;
                    p_out.y = 0;
                }
            }
        }
        return p_out;
    }
    
    public void setFramePoint(Point sel_point, Dimension d)
    {
        this.sel_point = getFramePoint(new Point(sel_point.x, sel_point.y), d);
    }

    public Point getFramePoint(Dimension d)
    {
        if(sel_point != null)
           return getImagePoint(new Point(sel_point.x, sel_point.y), d);
        else
           return new Point(0, 0);
    }


    //return pixel position in the frame
    //Argument point is fit to frame dimension
    public Point getFramePoint(Point p, Dimension d)
    {
        Point p_out = new Point(0, 0);
        
        if(curr_frame_idx != -1 && frame.size() != 0)
        {
            Dimension view_dim;
            Dimension dim;
            Dimension fr_dim = getFrameSize(curr_frame_idx, d);
            
            if(zoom_rect == null)
            {
                view_dim = GetFrameDim(curr_frame_idx);
                dim = view_dim;
            } else {
                dim = new Dimension(zoom_rect.width, zoom_rect.height);
                view_dim = new Dimension(zoom_rect.x+zoom_rect.width, zoom_rect.y+zoom_rect.height);
                p_out.x = zoom_rect.x;
                p_out.y = zoom_rect.y;
            }

            double ratio_x = (double)dim.width/ fr_dim.width;
            double ratio_y = (double)dim.height/ fr_dim.height;
            
            p_out.x += ratio_x * p.x;
            if(p_out.x > view_dim.width-1)
            {
                p_out.x = view_dim.width-1;
                p.x = fr_dim.width;
            }
            p_out.y += ratio_y * p.y;
            if(p_out.y > view_dim.height-1)
            {
                p_out.y = view_dim.height-1;
                p.y = fr_dim.height;
            }
        }
        
        return p_out;
    }

    public boolean contain(Point p, Dimension d)
    {
        Dimension fr_dim = getFrameSize(curr_frame_idx, d);
        if(p.x > fr_dim.width || p.y > fr_dim.height)
            return false;
        return true;
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
    
    /*returm frame image pixel dimension*/
    public Dimension getFrameSize(int idx, Dimension d)
    {
        int width, height;
        //Border image pixel
        Dimension dim_b = new Dimension(d.width-1,d.height-1);

        width = dim_b.width;
        height = dim_b.height;
        if(getAspectRatio())
        {
            Dimension dim = GetFrameDim(idx);
            int w = dim.width;
            int h = dim.height;
            if(zoom_rect != null)
            {
                w = zoom_rect.width;
                h = zoom_rect.height;
            }
            double ratio = (double)w/h;
            width = (int)(ratio * d.height);
            if(width > d.width)
            {
                width = d.width;
                height = (int)(d.width/ratio);
            }
         }
         return new Dimension(width, height);
     }
    
    public void setMeasurePoint(int x_pixel, int y_pixel, Dimension d)
    {
        Point mp = getFramePoint(new Point(x_pixel, y_pixel), d);
        x_measure_pixel = mp.x;
        y_measure_pixel = mp.y;
    }
    
    public Point getMeasurePoint(Dimension d)
    {
        return getImagePoint(new Point(x_measure_pixel, y_measure_pixel), d);
    }
    
    public void SetZoomRegion(int idx, Dimension d, Rectangle r)
    {
        if(idx > frame.size() - 1 || frame.elementAt(idx) == null ) 
            return;

        Dimension dim;
        Dimension fr_dim = getFrameSize(idx, d);
        
        if(zoom_rect == null)
        {
            zoom_rect = new Rectangle(0, 0, 0, 0);
            dim = GetFrameDim(idx);
        } else {
            dim = new Dimension (zoom_rect.width, zoom_rect.height);
        }

        double ratio_x = (double)dim.width/ fr_dim.width;
        double ratio_y = (double)dim.height/ fr_dim.height;
        
            
        zoom_rect.width = (int)(ratio_x * r.width + 0.5);
        zoom_rect.height = (int)(ratio_y * r.height + 0.5);
        zoom_rect.x += ratio_x * r.x + 0.5;
        zoom_rect.y += ratio_y * r.y + 0.5;
        
        if(zoom_rect.width == 0)
            zoom_rect.width = 1;
        if(zoom_rect.height == 0)
            zoom_rect.height = 1;
                
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
    
    public int getNextFrameIdx()
    {
        if(curr_frame_idx + 1 == getNumFrame())
            return curr_frame_idx;
        else
            return curr_frame_idx + 1;
        
    }

    public int getLastFrameIdx()
    {
        if(curr_frame_idx - 1 < 0)
            return 0;
        else
            return curr_frame_idx - 1;
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

import java.awt.*;
import java.util.*;
import java.awt.image.*;
import java.awt.image.renderable.ParameterBlock;
import java.io.IOException;
import javax.media.jai.widget.ImageCanvas;
import javax.media.jai.Interpolation;
import javax.media.jai.JAI;
import javax.media.jai.RenderedOp;
import javax.media.jai.PlanarImage;
import javax.media.jai.TiledImage;
import com.sun.media.jai.codec.ByteArraySeekableStream;
import javax.media.jai.widget.ScrollingImagePanel;


class FrameJAI extends Frames
{
    RenderingHints renderHints = null;
//    boolean perform_zoom = false;
    float x_ratio = 0.1F;
    float y_ratio = 0.1F;
    
    
    FrameJAI()
    {
       super();
       renderHints =  new RenderingHints(RenderingHints.KEY_RENDERING, 
                                         RenderingHints.VALUE_RENDER_SPEED);

    }
    
    public boolean AddFrame(byte[] buf, float t) 
    {
        try
        {
            ByteArraySeekableStream stream = new ByteArraySeekableStream(buf);
        
        /* Create an operator to decode the image file. */
            RenderedOp image1 = JAI.create("stream", stream);
            AddFrame(image1, t);
            return true;
        } catch (IOException e) {
            return false;
        }
    }
    
    protected Dimension GetFrameDim(int idx)
    {
       return  new Dimension( ((RenderedOp)frame.elementAt(idx)).getWidth(),
                              ((RenderedOp)frame.elementAt(idx)).getHeight());
    }
    
    public Object GetFrame(int idx, Dimension d)
    {
        RenderedImage buimg = null;
        
        ParameterBlock params, params1;
        RenderedImage image_op = null;
        float tras_x = 0.0F, tras_y = 0.0F;

        int count;
        if(idx >= frame.size())
            idx = frame.size() - 1;
        if(idx < 0 || frame.elementAt(idx) == null) return null;
        curr_frame_idx = idx;

        RenderedImage image1  = (RenderedImage)frame.elementAt(idx);
        if(zoom_rect != null)
        {
           params = new ParameterBlock();
           params.addSource(image1);
           params.add((float)zoom_rect.x);        
           params.add((float)zoom_rect.y);         
           params.add((float)zoom_rect.width);         
           params.add((float)zoom_rect.height);         

           image_op = JAI.create("crop", params);
                    
           tras_x = -(float)zoom_rect.x;
           tras_y = -(float)zoom_rect.y;
                                        
         } else {
            image_op = image1;
         }
         
         Interpolation interp = Interpolation.getInstance(
                                        Interpolation.INTERP_NEAREST);
         params1 = new ParameterBlock();
                
         x_ratio = ((float)d.width)/image_op.getWidth();
         y_ratio = ((float)d.height)/image_op.getHeight();
         params1.addSource(image_op);

         params1.add(x_ratio);          // x scale factor
         params1.add(y_ratio);          // y scale factor
         params1.add(tras_x * x_ratio); // x translate
         params1.add(tras_y * y_ratio); // y translate
         params1.add(interp);           // interpolation method

         /* Create an operator to scale image1. */
         buimg = JAI.create("scale", params1);
           
         return buimg;

    }
        
}
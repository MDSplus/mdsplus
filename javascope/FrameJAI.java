import java.awt.*;
import java.util.*;
import java.awt.image.*;
import java.awt.image.renderable.ParameterBlock;
import java.io.IOException;
import javax.media.jai.*;
import javax.media.jai.operator.TransposeDescriptor;

import com.sun.media.jai.codec.ByteArraySeekableStream;


class FrameJAI extends Frames
{
    RenderingHints renderHints = null;
    float x_ratio = 0.1F;
    float y_ratio = 0.1F;
    
    
    FrameJAI()
    {
       super();
       renderHints =  new RenderingHints(RenderingHints.KEY_RENDERING, 
                                         RenderingHints.VALUE_RENDER_SPEED);

    }
    
    public void AddJAIImage(byte[] buf, float t) throws IOException
    {
        ByteArraySeekableStream stream = new ByteArraySeekableStream(buf);
        
    /* Create an operator to decode the image file. */
        RenderedOp image1 = JAI.create("stream", stream);
        if(getHorizontalFlip())
        {
            RenderedOp image2 = JAI.create("transpose", image1, TransposeDescriptor.FLIP_HORIZONTAL);
            image1 = image2;
        }
        if(this.getVerticalFlip())
        {
            RenderedOp image2 = JAI.create("transpose", image1, TransposeDescriptor.FLIP_VERTICAL);
            image1 = image2;
        }
        AddFrame(image1, t);
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

         Dimension dim = this.getFrameSize(idx, d);

                
         x_ratio = ((float)dim.width)/image_op.getWidth();
         y_ratio = ((float)dim.height)/image_op.getHeight();
         params1.addSource(image_op);

         params1.add(x_ratio);          // x scale factor
         params1.add(y_ratio);          // y scale factor
         params1.add(tras_x * x_ratio); // x translate
         params1.add(tras_y * y_ratio); // y translate
    //     params1.add(interp);           // interpolation method

         /* Create an operator to scale image1. */
         buimg = JAI.create("scale", params1);
           
         return buimg;

    }
        
    
    protected int[] getPixelArray(int idx, int x, int y, int img_w, int img_h)
    {
       int pixel_array[];// = new int[img_width * img_height]; 
       RenderedImage r_img = (RenderedImage)frame.elementAt(idx);
       if(img_w == -1 && img_h == -1)
       {
            img_width = img_w = r_img.getWidth(); 
            img_height = img_h = r_img.getHeight();
       }
       Raster raster = r_img.getData();
       try 
       { 
         pixel_array = raster.getPixels(x, y, img_w, img_h, (int[])null);
       } catch(Exception exc) { 
            System.out.println("Pixel array not completed"); 
            return null; 
       }
       return pixel_array;
    }
}
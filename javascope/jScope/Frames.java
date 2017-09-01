package jScope;

/* $Id$ */
import java.awt.*;
import java.io.*;
import java.awt.image.*;
import java.awt.color.*;
import java.util.*;
import javax.imageio.ImageIO;

class Frames extends Canvas
{
    static final int ROI = 20;

    Vector<Float> frame_time = new Vector<>();
    Rectangle zoom_rect = null;
    Rectangle view_rect = null;
    private int curr_frame_idx = -1;
  
    protected boolean aspect_ratio = true;
    protected int curr_grab_frame = -1;
    protected int[] pixel_array;
    protected float[] values_array;
    protected int img_width;
    protected int img_height;
    protected int[] frames_pixel_array;
    protected float[] frames_value_array;
    protected Rectangle frames_pixel_roi;
    protected int x_measure_pixel = 0, y_measure_pixel = 0;
    protected float ft[] = null;
    protected Point sel_point = null;
    protected boolean horizontal_flip = false;
    protected boolean vertical_flip = false;
    protected int color_idx;
  //protected int[] frame_type;


    // Frame data cache management class
    static class FrameDescriptor
    {
        byte []buffer;
        Image image;
        Image updatedImage;
        int updateCount;
        FrameDescriptor(byte [] buffer, Image image, Image updatedImage, int updateCount)
        {
            this.buffer = buffer;
            this.image = image;
            this.updatedImage = updatedImage;
            this.updateCount = updateCount;
        }
    }
    class FrameCache
    {
        FrameData fd;
        Hashtable<Integer, FrameDescriptor> recentFrames;
        int bitShift;
        boolean bitClip;
        ColorMap colorMap;
        int frameType;
        int pixelSize;
        int bytesPerPixel;
        Dimension frameDim;
        int numFrames;
        MediaTracker tracker;
        Vector<Integer> recentIdxV= new Vector<Integer>();
        int updateCount = 0;
        static final int MAX_CACHE_MEM = 50000000;

        public FrameCache()
        {
            recentFrames = new Hashtable<>();
            bitShift = 0;
            bitClip = false;
            colorMap = new ColorMap();
            tracker = new MediaTracker(Frames.this);
        }
        FrameCache(FrameCache fc)
        {
            fd = fc.fd;
            bitShift = 0;
            bitClip = false;
            colorMap = new ColorMap();
            recentFrames = new Hashtable<>();
            Enumeration fds = fc.recentFrames.keys();
            while(fds.hasMoreElements())
            {
                Integer idx = (Integer)fds.nextElement();
                FrameDescriptor fDescr = (FrameDescriptor)fc.recentFrames.get(idx);
                recentFrames.put(idx, fDescr);
            }

        }
        void setFrameData(FrameData fd)
        {
            this.fd = fd;
            try {
                numFrames = fd.GetNumFrames();
            }catch(Exception exc){numFrames = 0;}
        }
        public void shiftImagePixel(int bitShift, boolean bitClip)
        {
            this.bitShift = bitShift;
            this.bitClip = bitClip;
            updateCount++;
         }
        void loadFrame(int idx) throws Exception
        {
            frameType = fd.GetFrameType();
            frameDim = fd.GetFrameDimension();
            numFrames = fd.GetNumFrames();
            byte []buf = fd.GetFrameAt(idx);
            if(buf == null)
                return;
            Image img;
            float[] values = null;
            DataBuffer db;
            WritableRaster raster;
            switch(frameType)
            {
                case FrameData.BITMAP_IMAGE_8  :
                    pixelSize = 8;
                    bytesPerPixel = 1;
                    //FlipFrame(buf, frameDim, 1);
                    ColorModel colorModel = colorMap.getIndexColorModel(8);
                    db = new DataBufferByte(buf, buf.length);
                    raster = Raster.createInterleavedRaster(db,
                        frameDim.width,  frameDim.height, frameDim.width, 1, new int[]{0}, null);
                    img = new BufferedImage(colorModel, raster, false, null);
                    break;
                case FrameData.BITMAP_IMAGE_16 :
                {
                    boolean right;
                    pixelSize = 16;
                    bytesPerPixel = 2;
                    colorModel = colorMap.getIndexColorModel(16);
                    if(colorMap.bitShift < 0)
                        right = true;
                    else
                        right = false;
                    
                    bitShift = colorMap.bitShift;

                    //FlipFrame(buf, frameDim, 2);
                    int n_pix = frameDim.width * frameDim.height;
                    short buf_out[] = new short[n_pix];
                    values = new float[n_pix];
                    ByteArrayInputStream b = new ByteArrayInputStream(buf);
                    DataInputStream din = new DataInputStream(b);
                    if(right)
                    {
                        if(colorMap.bitClip)
                            for (int j = 0; j < n_pix; j++)
                            {
                                values[j] = 0xffff & din.readShort();
                                int val = ( (int)values[j] >> bitShift );
                                buf_out[j] = (short)(val > 255 ? 255 : val);
                            }
                        else
                            for (int j = 0; j < n_pix; j++)
                            {
                                values[j] = 0xffff & din.readShort();
                                buf_out[j] = (short)((int)values[j] >> bitShift );
                            }
                    }
                    else
                    {
                        if(colorMap.bitClip)
                            for (int j = 0; j < n_pix; j++)
                            {
                                values[j] = 0xffff & din.readShort();
                                int val = ( (int)values[j] << bitShift );
                                buf_out[j] = (short)(val > 255 ? 255 : val);
                            }
                        else
                            for (int j = 0; j < n_pix; j++)
                            {
                                values[j] = 0xffff & din.readShort();
                                buf_out[j] = (short)((int)values[j] << bitShift );
                            }
                    }
                    db = new DataBufferUShort(buf_out, buf.length);
                    raster = Raster.createInterleavedRaster(db,
                        frameDim.width,  frameDim.height, frameDim.width, 1, new int[]{0}, null);
                    img = new BufferedImage(colorModel, raster, false, null);
                    break;
                }
                case FrameData.BITMAP_IMAGE_32 :
                {
                    pixelSize = 32;
                    bytesPerPixel = 4;
                    colorModel = colorMap.getIndexColorModel(16);
                    //FlipFrame(buf, frameDim, 4);

                    int n_pix = frameDim.width*frameDim.height;
                    float buf_out[] = new float[n_pix];
                    ByteArrayInputStream b = new ByteArrayInputStream(buf);
                    DataInputStream din = new DataInputStream(b);
                    float max = Float.MIN_VALUE;
                    float min = Float.MAX_VALUE;
                    for(int j = 0; j < n_pix; j++)
                    {
                       buf_out[j] = din.readFloat();
                       if(buf_out[j] > max) max = buf_out[j];
                       if(buf_out[j] < min) min = buf_out[j];
                    }
                    values = buf_out;
                    short buf_out1[] = new short[n_pix];
                    for(int j = 0; j < n_pix; j++)
                    {
                       buf_out1[j] = (short)(255 * (buf_out[j] - min)/(max - min));
                    }
                    db = new DataBufferUShort(buf_out1, buf.length);
                    raster = Raster.createInterleavedRaster(db,
                        frameDim.width,  frameDim.height, frameDim.width, 1, new int[]{0}, null);
                    img = new BufferedImage(colorModel, raster, false, null);
                    break;
                }
                case FrameData.AWT_IMAGE :
                    pixelSize = 0; //pixelSize == 0 means that the image is not a bitmap (jpg, gif...)
                    img = ImageIO.read(new ByteArrayInputStream(buf));
                    break;
                default:
                    return;
            }
            tracker = new MediaTracker(Frames.this);
            tracker.addImage(img, idx);
            recentFrames.put(new Integer(idx), new FrameDescriptor(buf, img, img, 0));
            try {
                tracker.waitForID(idx);
            }catch(Exception exc){}
            int maxStoreFrames = MAX_CACHE_MEM/buf.length;
            recentIdxV.insertElementAt(new Integer(idx), 0);
            if(maxStoreFrames < 1) maxStoreFrames = 1;
            if(recentIdxV.size()> maxStoreFrames)
            {
                Object delIdx = recentIdxV.elementAt(maxStoreFrames);
                recentFrames.remove(delIdx);
                recentIdxV.removeElementAt(maxStoreFrames);
            }
        }
        Image getImageAt(int idx) throws IOException
        {
            FrameDescriptor fDesc = (FrameDescriptor)recentFrames.get(new Integer(idx));
            if(fDesc == null)
            {
                try {
                    loadFrame(idx);
                }catch(Exception exc)
                {
                    System.out.println("Error Loading frame at " + idx);
                    return null;
                }
                fDesc = (FrameDescriptor)recentFrames.get(new Integer(idx));
            }
            if(fDesc == null) return null;
            if(fDesc.updateCount == updateCount) //fDesc.updatedImage  is still ok
                return fDesc.updatedImage;
            //Othewise it is necessary to update it
            Image img ;
            if(pixelSize > 0)
            {
                ColorModel colorModel = colorMap.getIndexColorModel( (pixelSize < 32 ? pixelSize : 16) );
                img = new BufferedImage(colorModel, ((BufferedImage)fDesc.image).getRaster(), false, null);
                if(bitShift != 0)
                {
                    BufferedImage bi = (BufferedImage)img;
                    ByteArrayInputStream b = new ByteArrayInputStream(fDesc.buffer);
                    DataInputStream din = new DataInputStream(b);

                    WritableRaster wr = bi.getRaster();
                    DataBuffer db = wr.getDataBuffer();
                    int nPixels = db.getSize()/bytesPerPixel;
                    if(nPixels != frameDim.width * frameDim.height)
                    {
                        System.out.println("INTERNAL ERRROR: Inconsistend frame dimension when getting frame");
                        return null;
                    }

                    int val;
                    int absBitShift = Math.abs(bitShift);
                    for(int j = 0; j < nPixels; j++)
                    {
                        if(frameType == FrameData.BITMAP_IMAGE_8)
                            val = din.readByte();
                        else if(frameType == FrameData.BITMAP_IMAGE_16)
                            val = din.readShort();
                        else
                            val = din.readInt();

                        if(bitShift > 0)
                        {
                            val = val << bitShift ;
                            if(bitClip)
                                db.setElem(j, val > 255 ? 255 : val);
                            else
                                db.setElem(j, val);
                        }
                        else
                        {
                             val = val >> absBitShift ;
                           if(bitClip)
                                db.setElem(j, val > 255 ? 255 : val);
                            else
                                db.setElem(j, val);
                        }
                    }
                }
            }
            else //Non bitmap image (j[pg,  gif...)
                img = fDesc.image;
            tracker = new MediaTracker(Frames.this);
            tracker.addImage(img, idx);
            try {
                tracker.waitForID(idx);
            }catch(Exception exc){}
            recentFrames.put(new Integer(idx), new FrameDescriptor(fDesc.buffer, fDesc.image, img, updateCount));
            return img;
        }
        byte [] getBufferAt(int idx)
        {
            FrameDescriptor fDescr = (FrameDescriptor)recentFrames.get(new Integer(idx));
            if(fDescr == null)
            {
                try {
                    loadFrame(idx);
                }catch(Exception exc)
                {
                    System.out.println("Error Loading frame at " + idx);
                    return null;
                }
            }
            fDescr = (FrameDescriptor)recentFrames.get(new Integer(idx));
            if(fDescr == null) return null;
            return fDescr.buffer;

        }
        float[] getValuesAt(int idx)
        {
            FrameDescriptor fDescr = (FrameDescriptor)recentFrames.get(new Integer(idx));
            if(fDescr == null)
            {
                try {
                    loadFrame(idx);
                }catch(Exception exc)
                {
                    System.out.println("Error Loading frame at " + idx);
                    return null;
                }
            }
            fDescr = (FrameDescriptor)recentFrames.get(new Integer(idx));
            if(fDescr == null) return null;
            byte []buf = fDescr.buffer;
            int n_pix = frameDim.width * frameDim.height;
            float values[] = new float[n_pix];
            ByteArrayInputStream b;
            DataInputStream din;
            switch(frameType)
            {
                case FrameData.BITMAP_IMAGE_8  :
//                  System.out.println("INTERNAL ERROR frame values requested for BITMAP_IMAGE_8");
//                  return null;
                    b = new ByteArrayInputStream(buf);
                    din = new DataInputStream(b);
                    try {
                        for (int j = 0; j < n_pix; j++)
                            values[j] = din.readByte();
                    }catch(IOException exc)
                    {
                        System.out.println("INTERNAL ERROR Getting Frame values: " + exc);
                        return null;
                    }
                    break;
                case FrameData.BITMAP_IMAGE_16 :
                {
                    b = new ByteArrayInputStream(buf);
                    din = new DataInputStream(b);
                    try {
                        for (int j = 0; j < n_pix; j++)
                            values[j] = 0xffff & din.readShort();
                    }catch(IOException exc)
                    {
                        System.out.println("INTERNAL ERROR Getting Frame values: " + exc);
                        return null;
                    }
                    break;
                }
                case FrameData.BITMAP_IMAGE_32 :
                {
                    b = new ByteArrayInputStream(buf);
                    din = new DataInputStream(b);
                    try {
                        for(int j = 0; j < n_pix; j++)
                            values[j] = din.readFloat();
                     }catch(IOException exc)
                    {
                        System.out.println("INTERNAL ERROR Getting Frame values: " + exc);
                        return null;
                    }
                    break;
                }
                default:
                   System.out.println("INTERNAL ERROR frame values requested for unexpected type: "+ frameType);
                   return null;
            }
            return values;
        }
        int getFrameType() {return frameType;}
        void setColorMap(ColorMap colorMap)
        {
            this.colorMap = colorMap;
            updateCount++;
        }
        ColorMap getColorMap() {return colorMap;}
        int getNumFrames() { return numFrames;}
        Dimension getFrameDimension() {return frameDim;}
    } //End class FrameCache
    FrameCache cache;




    Frames()
    {
        super();
        img_width = -1;
        img_height = -1;
        cache = new FrameCache();
    }

    Frames(Frames frames)
    {
        this();
        Image img;

        cache = new FrameCache(frames.cache);

        if(frame_time.size() != 0)
            frame_time.removeAllElements();

        float buf_values[] = null;
        if(frames.zoom_rect != null)
            zoom_rect = new Rectangle(frames.zoom_rect);
        if(frames.view_rect != null)
            view_rect = new Rectangle(frames.view_rect);
        curr_frame_idx = frames.curr_frame_idx;
    }

    public int getFrameType()
    {
        return cache.getFrameType();
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


    public void setColorMap(ColorMap colorMap)
    {
      if(colorMap == null) return;
      cache.setColorMap(colorMap);
    }

    public ColorMap getColorMap()
    {
      return cache.getColorMap();
    }

 /*   public int getPixelSize(int idx)
    {
        if(getNumFrame() == 0) return 8;
        switch(frame_type[idx])
        {
            case FrameData.BITMAP_IMAGE_8 : return 8;
            case FrameData.BITMAP_IMAGE_16 : return 16;
            case FrameData.BITMAP_IMAGE_32 : return 16;
            default: return 8;
        }
    }

    protected void finalize()
    {
        if(frame.size() != 0)
            frame.removeAllElements();
        if(frame_time.size() != 0)
            frame_time.removeAllElements();
        tracker = null;
    }
*/
    public void SetFrameData(FrameData fd) throws Exception
    {
        cache.setFrameData(fd);
        curr_frame_idx = 0;
        float t[] = fd.GetFrameTimes();
        for(int i = 0; i < t.length; i++)
            frame_time.addElement(new Float(t[i]));
/*      int n_frames = fd.GetNumFrames();
        float t[] = fd.GetFrameTimes();
        byte[] buf;
        ColorModel colorModel;
        boolean right = false;
        
        frame_type = new int[n_frames];

        if(colorMap.bitShift < 0)
            right = true;
        
        int bitShift = Math.abs(colorMap.bitShift);

        for(int i = 0; i < n_frames; i++)
        {
            buf = fd.GetFrameAt(i);
            if(buf == null)
                continue;
            switch((frame_type[i] = fd.GetFrameType()))
            {
                case FrameData.BITMAP_IMAGE_8  :
                    FlipFrame(buf, fd.GetFrameDimension(), 1);
                    colorModel = colorMap.getIndexColorModel(8);
                    AddBITMAPImage(buf, colorModel, fd.GetFrameDimension(), t[i]);
                break;
                case FrameData.BITMAP_IMAGE_16 :
                {
                    colorModel = colorMap.getIndexColorModel(16);
                    Dimension d = fd.GetFrameDimension();
                    FlipFrame(buf, d, 2);
                                      
                  int n_pix = d.width * d.height;
                  short buf_out[] = new short[n_pix];
                  float values[] = new float[n_pix];
                  ByteArrayInputStream b = new ByteArrayInputStream(buf);
                  DataInputStream din = new DataInputStream(b);
            
                    if(right)
                    {    
                        if(colorMap.bitClip)
                            for (int j = 0; j < n_pix; j++)
                            {
                                values[j] = 0xffff & din.readShort();
                                int val = ( (int)values[j] >> bitShift );
                                buf_out[j] = (short)(val > 255 ? 255 : val);
                            }
                        else
                            for (int j = 0; j < n_pix; j++)
                            {
                                values[j] = 0xffff & din.readShort();
                                buf_out[j] = (short)((int)values[j] >> bitShift );
                            }
                    }
                    else
                    {
                        if(colorMap.bitClip)
                            for (int j = 0; j < n_pix; j++)
                            {
                                values[j] = 0xffff & din.readShort();
                                int val = ( (int)values[j] << bitShift );
                                buf_out[j] = (short)(val > 255 ? 255 : val);
                            }
                        else
                            for (int j = 0; j < n_pix; j++)
                            {
                                values[j] = 0xffff & din.readShort();
                                buf_out[j] = (short)((int)values[j] << bitShift );
                            }
                    }                        
                  
                    frame_values.addElement(values);
                    AddBITMAPImage(buf_out, colorModel, d, t[i]);

                }
                break;
                case FrameData.BITMAP_IMAGE_32 :
                {
                    colorModel = colorMap.getIndexColorModel(16);
                    Dimension d = fd.GetFrameDimension();
                    FlipFrame(buf, d, 4);

                    int n_pix = d.width*d.height;
                    float buf_out[] = new float[n_pix];
                    ByteArrayInputStream b = new ByteArrayInputStream(buf);
                    DataInputStream din = new DataInputStream(b);
                    float max = Float.MIN_VALUE;
                    float min = Float.MAX_VALUE;
                    for(int j = 0; j < n_pix; j++)
                    {
                       buf_out[j] = din.readFloat();
                       if(buf_out[j] > max) max = buf_out[j];
                       if(buf_out[j] < min) min = buf_out[j];
                    }
                    frame_values.addElement(buf_out);
                    short buf_out1[] = new short[n_pix];
                    for(int j = 0; j < n_pix; j++)
                    {
                       buf_out1[j] = (short)(255 * (buf_out[j] - min)/(max - min));
                    }

                    AddBITMAPImage(buf_out1, colorModel, d, t[i]);
                }
                break;
                case FrameData.AWT_IMAGE :
                    AddAWTImage(buf, t[i]);
                break;
                case FrameData.JAI_IMAGE :
                    AddJAIImage(buf, t[i]);
                break;
            }
        }

 */
    }

    public void applyColorModel(ColorMap colorMap)
    {
        if(colorMap == null) return;

        cache.setColorMap(colorMap);

 /*
        BufferedImage img = null;
        Vector newFrame = new Vector();
        ColorModel colorModel;

               
        for(int i = 0; i < frame.size(); i++)
        {
            colorModel = colorMap.getIndexColorModel(getPixelSize(i));
            img = (BufferedImage)frame.elementAt(i);
            img = new BufferedImage(colorModel, img.getRaster(), false, null);
            newFrame.add(img);
        }
        frame.removeAllElements();
        frame = newFrame;
*/    }

/*    public boolean AddBITMAPImage(byte[] buf, ColorModel colorModel, Dimension d, float t)
    {
        Image img;
        MemoryImageSource source;
        if(buf == null || d == null) return false;
        DataBuffer db = new DataBufferByte(buf, buf.length);
        WritableRaster raster = Raster.createInterleavedRaster(db,
                    d.width,  d.height, d.width, 1, new int[]{0}, null);
        img = new BufferedImage(colorModel, raster, false, null);

        return AddFrame(img, t);
    }


    public boolean AddBITMAPImage(short[] buf, ColorModel colorModel, Dimension d, float t)
    {
        BufferedImage img;
        MemoryImageSource source;
        if(buf == null || d == null) return false;

        DataBuffer db = new DataBufferUShort(buf, buf.length);
        WritableRaster raster = Raster.createInterleavedRaster(db,
                    d.width,  d.height, d.width, 1, new int[]{0}, null);
        
        img = new BufferedImage(colorModel, raster, false, null);

        
        return AddFrame(img, t);
    }


    public boolean AddBITMAPImage(int[] buf,  ColorModel colorModel, Dimension d, float t)
    {
        Image img;
        MemoryImageSource source;
        if(buf == null || d == null) return false;
       DataBuffer db = new DataBufferInt(buf, buf.length);
        WritableRaster raster = Raster.createInterleavedRaster(db,
                   d.width,  d.height, d.width, 1, new int[]{0}, null);
        img = new BufferedImage(colorModel, raster, false, null);

        return AddFrame(img, t);
    }
*/
    public void shiftImagePixel(int bitShift, boolean bitClip)
    {
        BufferedImage bi;
        float values[] = null; 
        boolean right = false;
 
        cache.shiftImagePixel(bitShift, bitClip);
    }
/*
        if(bitShift < 0)
            right = true;
        
          bitShift = Math.abs(bitShift);

          for( int i = 0; i < frame.size() ; i++)
          {
                if(this.frame_type[i] != FrameData.BITMAP_IMAGE_16)
                    continue;

                bi = (BufferedImage)frame.elementAt(i);
                values = (float[])frame_values.elementAt(i);
                WritableRaster wr = bi.getRaster();
                DataBuffer db = wr.getDataBuffer();
                int numBank = db.getNumBanks();
                int n_pix = db.getSize();
                
                if(right)
                {    
                    if(bitClip)
                        for (int j = 0; j < n_pix; j++)
                        {
                            int val = ( (int)values[j] >> bitShift );
                            db.setElem(j, val > 255 ? 255 : val);
                        }
                    else
                        for (int j = 0; j < n_pix; j++)
                            db.setElem(j, ((int)values[j] >> bitShift ));
                }
                else
                {
                    if(bitClip)
                        for (int j = 0; j < n_pix; j++)
                        {
                            int val = ( (int)values[j] << bitShift );
                            db.setElem(j, val > 255 ? 255 : val);
                        }
                    else
                        for (int j = 0; j < n_pix; j++)
                            db.setElem(j, ((int)values[j] << bitShift ));
                    
                }                        
          }
    }

   
    
    public boolean AddAWTImage(byte[] buf, float t)
    {
        Image img;
        if(buf == null) return false;
        //img = getToolkit().createImage(buf);
        img = Toolkit.getDefaultToolkit().createImage(buf);
        return AddFrame(img, t);
    }

    public void AddJAIImage(byte[] buf, float t) throws IOException
    {
    }
*/

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
        return cache.getNumFrames();
 //        return frame.size();
    }

    public boolean getAspectRatio()
    {
        return aspect_ratio;
    }

    public void setAspectRatio(boolean aspect_ratio)
    {
        this.aspect_ratio = aspect_ratio;
    }

    public void FlipFrame(byte buf[], Dimension d, int num_byte_pixel)
    {
        if(!vertical_flip && !horizontal_flip)
            return;

        int img_size = d.height*d.width * num_byte_pixel;
        byte tmp[] = new byte[img_size];
        int i, j , k , l, ofs;

        int h = vertical_flip ? d.height - 1: 0;
        int w = horizontal_flip ? d.width - 1: 0;

        for(j = 0; j < d.width; j++)
            for(k = 0; k < d.height; k++)
                for(l = 0; l < num_byte_pixel; l++)
                    tmp[ ( (Math.abs(h - k) * d.width) +  Math.abs(w - j) ) * num_byte_pixel  + l] =
                    buf[((k * d.width) + j ) * num_byte_pixel + l];
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
    protected int[] getPixelArray(int idx, int x, int y, int img_w, int img_h)
    {
        Image img;
        try {
            img =  cache.getImageAt(idx);
        }catch(Exception exc)
        {
            System.out.println("INTERNAL ERROR in Frame.getPixelArrat: " + exc);
            return  null;
        }
       if(img_w == -1 && img_h == -1)
       {
            img_width = img_w = img.getWidth(this);
            img_height = img_h = img.getHeight(this);
       }
       int pixel_array[] = new int[img_w * img_h];
       PixelGrabber grabber = new PixelGrabber(img, x, y, img_w, img_h, pixel_array,0,img_w);
       try
       {
          grabber.grabPixels();
       } catch(InterruptedException ie) {
            System.err.println("Pixel array not completed");
            return null;
       }
        return pixel_array;
    }

    protected float[] getValueArray(int idx, int x, int y, int img_w, int img_h)
    {
        float values[];
        Image img;
        try {
            values = cache.getValuesAt(idx);
            img = cache.getImageAt(idx);
        }catch(Exception exc){return null;}

        if(img_w == -1 && img_h == -1)
       {
            img_width = img.getWidth(this);
            img_height = img.getHeight(this);
            return values;
       }
        float values_array[] = new float[img_w * img_h];
       int k = 0;
//       for(int j = y; j < img_h; j++)
//            for(int i = x; i < img_w; i++)
      for(int j = y; j < y+img_h; j++)
            for(int i = x; i < x+img_w; i++)
                values_array[k++] = values[j * img_width + i];

       return values_array;
    }


    protected void grabFrame()
    {
        if(curr_frame_idx != curr_grab_frame || pixel_array == null)
        {
            if( (pixel_array = getPixelArray(curr_frame_idx, 0, 0, -1, -1)) != null)
            {
                values_array = getValueArray(curr_frame_idx, 0, 0, -1, -1);
                curr_grab_frame = curr_frame_idx;
            }
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

    private Point getImageBufferPoint( int x, int y )
    {
        Point p = new Point();

        p.x = x;
        p.y = y;        
        if(this.horizontal_flip)
            p.y = this.img_height - y - 1; 
        if(this.vertical_flip)
            p.x = this.img_width - x - 1; 
        return p;
    }
    
    
    public int getPixel(int idx, int x, int y)
    {
        if(!isInImage(idx, x, y))
            return -1;
        //curr_grab_frame = idx;
        byte[] imgBuf = cache.getBufferAt(idx);

         
        Point p = getImageBufferPoint(x,y);
        
        try {
            if(imgBuf != null)
                return (int)imgBuf[(p.y * img_width) + p.x];
        }catch(Exception exc) {return -1;}
        return -1;
    }

    public float getPointValue(int idx, int x, int y)
    {
        if(!isInImage(idx, x, y))
            return -1;

        //curr_grab_frame = idx;
        values_array = cache.getValuesAt(idx);
 
        Point p = getImageBufferPoint(x,y);
 
        return values_array[(p.y * img_width) + p.x];
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
        Point p;
        int n_point = (int) (Math.sqrt( Math.pow((double)(st_x - end_x), 2.0) + Math.pow((double)(st_y - end_y), 2.0)) + 0.5);
        int e_x, s_x, x, y;
        int pixels_line[] = {pixel_array[(st_y * img_width) + st_x], pixel_array[(st_y * img_width) + st_x]};

        grabFrame();
        if(n_point < 2)
        {
            pixels_line = new int[2];
            p = getImageBufferPoint(st_x,st_y);
            pixels_line[0] = pixels_line[1] = pixel_array[(p.y * img_width) + p.x];
           
            return pixels_line;
        }
        
        pixels_line = new int[n_point];

        for(int i = 0; i < n_point; i++)
        {
            x = (int)( st_x + (double)i * (end_x - st_x)/n_point);
            y = (int)( st_y + (double)i * (end_y - st_y)/n_point);
            p = getImageBufferPoint(x,y);
            pixels_line[i] = pixel_array[(p.y * img_width) + p.x];
        }
        return pixels_line;
    }

    public float[] getValuesLine(int st_x, int st_y, int end_x, int end_y)
    {
        Point p;
        int n_point = (int) (Math.sqrt( Math.pow((double)(st_x - end_x), 2.0) + Math.pow((double)(st_y - end_y), 2.0)) + 0.5);
        int e_x, s_x, x, y;
        //float values_line[] = {values_array[(st_y * img_width) + st_x], values_array[(st_y * img_width) + st_x]};
        float values_line[]; 
            
        grabFrame();
        if(n_point < 2)
        {
            values_line = new float[2];
            p = getImageBufferPoint(st_x,st_y);
            values_line[0] = values_line[1] = values_array[(p.y * img_width) + p.x];
            return values_line;
        }
        
        values_line = new float[n_point];

        for(int i = 0; i < n_point; i++)
        {
            x = (int)( st_x + (double)i * (end_x - st_x)/n_point);
            y = (int)( st_y + (double)i * (end_y - st_y)/n_point);
            p = getImageBufferPoint(x,y);
            values_line[i] = values_array[(p.y * img_width) + p.x];
        }
        return values_line;
    }

    public int[] getPixelsX(int y)
    {
        Point p;
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
              p = getImageBufferPoint(i,y);
              //pixels_x[j] = pixel_array[(y * img_width) + i];
              pixels_x[j] = pixel_array[(p.y * img_width) + p.x];
           }
        }
        return pixels_x;
    }

    public float[] getValuesX(int y)
    {
        Point p;
        float values_x[] = null;
        int st, end;

        grabFrame();

        if(values_array != null && y < img_height)
        {
           if(zoom_rect != null)
           {
             st = zoom_rect.x;
             end = zoom_rect.x + zoom_rect.width;
           } else {
             st = 0;
             end = img_width;
           }
           values_x = new float[end - st];
           for(int i = st, j = 0; i < end; i++, j++)
           {
              p = getImageBufferPoint(i,y);
              //values_x[j] = values_array[(y * img_width) + i];
              values_x[j] = values_array[(p.y * img_width) + p.x];
           }
        }
        return values_x;
    }


    public int[] getPixelsY(int x)
    {
        Point p;
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
              p = getImageBufferPoint(x,i);               
              //pixels_y[j] = pixel_array[(i * img_width) + x];
              pixels_y[j] = pixel_array[(p.y * img_width) + p.x];
           }
        }
        return pixels_y;
    }

    public float[] getValuesY(int x)
    {
        Point p;
        float values_y[] = null;
        int st, end;

        grabFrame();

        if(values_array != null && x < img_width)
        {
           if(zoom_rect != null)
           {
             st = zoom_rect.y;
             end = zoom_rect.y + zoom_rect.height;
           } else {
             st = 0;
             end = img_height;
           }
           values_y = new float[end - st];
           for(int i = st, j = 0; i < end; i++, j++)
           {
              p = getImageBufferPoint(x,i);               
              //values_y[j] = values_array[(i * img_width) + x];
              values_y[j] = values_array[(p.y * img_width) + p.x];
           }
        }
        return values_y;
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

    public float[] getValuesSignal(int x, int y)
    {
        float values_signal[] = null;
        if(frames_value_array == null || !frames_pixel_roi.contains(x, y))
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

            frames_value_array = new float[frames_pixel_roi.width * frames_pixel_roi.height * getNumFrame()];
            float f_array[];
            for(int i = 0; i < getNumFrame(); i++)
            {
                f_array = getValueArray(i, frames_pixel_roi.x, frames_pixel_roi.y, frames_pixel_roi.width, frames_pixel_roi.height);
                System.arraycopy(f_array, 0, frames_value_array, f_array.length * i, f_array.length);
            }
        }

        if(frames_value_array != null)
        {
            x -= frames_pixel_roi.x;
            y -= frames_pixel_roi.y;
            int size = frames_pixel_roi.width * frames_pixel_roi.height;
            values_signal = new float[getNumFrame()];
            for (int i = 0; i < getNumFrame(); i++)
            {
                values_signal[i] = frames_value_array[size * i + (y * frames_pixel_roi.width) + x];
            }
        }

        return values_signal;
    }


    public float[] getFramesTime()
    {
        if(frame_time == null || frame_time.size() == 0)
            return null;

        if(ft == null)
        {
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

    //return point position in the frame shows
    public Point getImagePoint(Point p, Dimension d)
    {
        Point p_out = new Point(0, 0);

        if(curr_frame_idx != -1 && cache.getNumFrames() != 0)
        {
            Dimension fr_dim = getFrameSize(curr_frame_idx, d);

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
        if(p.x < 0) p.x = 0;
        if(p.y < 0) p.y = 0;


        if(curr_frame_idx != -1 && cache.getNumFrames() != 0)
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
        if(frame_idx > cache.getNumFrames() - 1 || frame_idx < 0) return (float)0.0;
        return ((Float)frame_time.elementAt(frame_idx)).floatValue();
    }

    public int GetFrameIdxAtTime(float t)
    {
        int idx = -1;
        float dt;
        int numFrames = cache.getNumFrames();
        if(numFrames <= 0)
            return -1;


        if(numFrames == 1)
           dt = 1;
        else
           dt = ( (Float) frame_time.elementAt(1)).floatValue() -
                ( (Float) frame_time.elementAt(0)).floatValue();

        if(t >= ((Float)frame_time.elementAt(numFrames-1)).floatValue() + dt)
            return -1;

        if(t >= ((Float)frame_time.elementAt(numFrames-1)).floatValue() )
            return  numFrames-1;

        for(int i = 0; i < numFrames - 1; i++)
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
        return cache.getFrameDimension();
/*       return  new Dimension( ((Image)frame.elementAt(idx)).getWidth(this),
                              ((Image)frame.elementAt(idx)).getHeight(this));
*/    }

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
	/*
	Temporary fix, in order to avoid modification image if it is resized,
	must be investigate
	*/
        return new Dimension(width, height);
//      return GetFrameDim(idx);
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
        int numFrames = cache.getNumFrames();
        if(idx > numFrames - 1)// || frame.elementAt(idx) == null )
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
            return curr_frame_idx += 1;

    }

    public int getLastFrameIdx()
    {
        if(curr_frame_idx - 1 < 0)
            return 0;
        else
            return curr_frame_idx -= 1;
    }

    public int GetFrameIdx()
    {
       return curr_frame_idx;
    }

    public Image GetFrame(int idx, Dimension d)
    {
        return GetFrame(idx);
    }

    public Image GetFrame(int idx)
    {


        if(idx < 0) return null;
        int numFrames = cache.getNumFrames();
        if(idx >= numFrames)
            idx = numFrames - 1;
        //if(idx < 0)// || frame.elementAt(idx) == null) return null;
        curr_frame_idx = idx;
        Image img;
        try {
            img = cache.getImageAt(idx);
        }catch(Exception exc){img = null;}
        if(img_width == -1)
        {
            img_width = img.getWidth(this);
            img_height = img.getHeight(this);
        }

        //curr_grab_frame = curr_frame_idx;
        return img;
    }
}

package mds.wave;

/* $Id$ */
import java.awt.Canvas;
import java.awt.Dimension;
import java.awt.Image;
import java.awt.MediaTracker;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.DataBuffer;
import java.awt.image.DataBufferByte;
import java.awt.image.DataBufferUShort;
import java.awt.image.PixelGrabber;
import java.awt.image.Raster;
import java.awt.image.WritableRaster;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import javax.imageio.ImageIO;

public class Frames extends Canvas
{
	class FrameCache
	{
		static final int MAX_CACHE_MEM = 50000000;
		FrameData fd;
		Hashtable<Integer, FrameDescriptor> recentFrames;
		int bitShift;
		boolean bitClip;
		ColorMap colorMap;
		private float max;
		private float min;
		int frameType;
		int pixelSize;
		int bytesPerPixel;
		Dimension frameDim;
		int numFrames;
		MediaTracker tracker;
		Vector<Integer> recentIdxV = new Vector<>();
		int updateCount = 0;

		public FrameCache()
		{
			recentFrames = new Hashtable<>();
			bitShift = 0;
			bitClip = false;
			min = Float.MIN_VALUE;
			max = Float.MAX_VALUE;
			colorMap = new ColorMap();
			tracker = new MediaTracker(Frames.this);
		}

		FrameCache(FrameCache fc)
		{
			fd = fc.fd;
			bitShift = 0;
			bitClip = false;
			min = Float.MIN_VALUE;
			max = Float.MAX_VALUE;
			colorMap = new ColorMap();
			recentFrames = new Hashtable<>();
			final Enumeration<Integer> fds = fc.recentFrames.keys();
			while (fds.hasMoreElements())
			{
				final Integer idx = fds.nextElement();
				final FrameDescriptor fDescr = fc.recentFrames.get(idx);
				recentFrames.put(idx, fDescr);
			}
		}

		byte[] getBufferAt(int idx)
		{
			error = "";
			FrameDescriptor fDescr = recentFrames.get(new Integer(idx));
			if (fDescr == null)
			{
				try
				{
					loadFrame(idx);
				}
				catch (final Exception exc)
				{
					if (error.length() == 0)
						error = "Error Loading frame at " + idx;
					System.out.println(error);
					return null;
				}
			}
			fDescr = recentFrames.get(new Integer(idx));
			if (fDescr == null)
				return null;
			return fDescr.buffer;
		}

		ColorMap getColorMap()
		{ return colorMap; }

		Dimension getFrameDimension()
		{ return frameDim; }

		int getFrameType()
		{ return frameType; }

		Image getImageAt(int idx) throws IOException
		{
			error = "";
			FrameDescriptor fDesc = recentFrames.get(new Integer(idx));
			if (fDesc == null)
			{
				try
				{
					loadFrame(idx);
				}
				catch (final Exception exc)
				{
					if (error.length() == 0)
						error = "Error Loading frame at " + idx;
					System.out.println(error);
					return null;
				}
				fDesc = recentFrames.get(new Integer(idx));
			}
			if (fDesc == null)
				return null;
			if (fDesc.updateCount == updateCount) // fDesc.updatedImage is still ok
				return fDesc.updatedImage;
			// Othewise it is necessary to update it
			Image img;
			if (pixelSize > 0)
			{
                            try {
                         	//final ColorModel colorModel = colorMap.getIndexColorModel((pixelSize < 32 ? pixelSize : 16));
				final ColorModel colorModel = colorMap.getIndexColorModel(16);
				img = new BufferedImage(colorModel, ((BufferedImage) fDesc.image).getRaster(), false, null);
				final BufferedImage bi = (BufferedImage) img;
				final ByteArrayInputStream b = new ByteArrayInputStream(fDesc.buffer);
				final DataInputStream din = new DataInputStream(b);
				final WritableRaster wr = bi.getRaster();
                        	final DataBuffer db = wr.getDataBuffer();
				// int nPixels = db.getSize()/bytesPerPixel; ??? ces
				final int nPixels = db.getSize();
				if (nPixels != frameDim.width * frameDim.height)
				{
					error = "INTERNAL ERRROR: Inconsistend frame dimension when getting frame";
					System.out.println("INTERNAL ERRROR: Inconsistend frame dimension when getting frame");
					return null;
				}
 				float val;
                        	for (int j = 0; j < nPixels; j++)
				{
					if (frameType == FrameData.BITMAP_IMAGE_8 ||frameType == FrameData.BITMAP_IMAGE_U8)
                                        {
                                            byte bval = din.readByte();
                                            if(bval < 0)
                                                val = 256 + (bval & 0x7F);
                                            else
                                                val = bval;
                                        }
          				//else if (frameType == FrameData.BITMAP_IMAGE_U8)
					//	val = 0xFF & din.readByte();
					else if (frameType == FrameData.BITMAP_IMAGE_16)
						val = din.readShort();
					else if (frameType == FrameData.BITMAP_IMAGE_U16)
						val = 0xFFFF & din.readShort();
					else if (frameType == FrameData.BITMAP_IMAGE_F32)
						val = din.readFloat();
					else
						val = din.readInt();
					if (val < min)
						db.setElem(j, 0);
					else if (val > max)
						db.setElem(j, 255);
					else
						db.setElem(j, (int) ((val - min) * 255 / (max - min)));
				}
                           }catch(Exception exc){System.out.println(exc); img=null;}
			}
			else // Non bitmap image (j[pg, gif...)
				img = fDesc.image;
                        
                      	tracker = new MediaTracker(Frames.this);
			tracker.addImage(img, idx);
                    	try
			{
				tracker.waitForID(idx);
			}
			catch (final Exception exc)
			{}
			recentFrames.put(new Integer(idx), new FrameDescriptor(fDesc.buffer, fDesc.image, img, updateCount));
			return img;
		}

		int getNumFrames()
		{ return numFrames; }

		float[] getValuesAt(int idx)
		{
			error = "";
			FrameDescriptor fDescr = recentFrames.get(new Integer(idx));
			if (fDescr == null)
			{
				try
				{
					loadFrame(idx);
				}
				catch (final Exception exc)
				{
					if (error.length() == 0)
						error = "Error Loading frame at " + idx;
					System.out.println(error);
					return null;
				}
			}
			fDescr = recentFrames.get(new Integer(idx));
			if (fDescr == null)
				return null;
			final byte[] buf = fDescr.buffer;
			final int n_pix = frameDim.width * frameDim.height;
			final float values[] = new float[n_pix];
			ByteArrayInputStream b;
			DataInputStream din;
			b = new ByteArrayInputStream(buf);
			din = new DataInputStream(b);
			switch (frameType)
			{
			case FrameData.BITMAP_IMAGE_U8:
			case FrameData.BITMAP_IMAGE_8:
//                  System.out.println("INTERNAL ERROR frame values requested for BITMAP_IMAGE_8");
//                  return null;
				b = new ByteArrayInputStream(buf);
				din = new DataInputStream(b);
				try
				{
					for (int j = 0; j < n_pix; j++)
                                        {
                                            byte bval = din.readByte();
                                             if(bval < 0)
                                                values[j] = 256 + (bval & 0x7F);
                                            else
                                                values[j] = bval;

                                        }
				}
				catch (final IOException exc)
				{
					error = "INTERNAL ERROR Getting Frame values: " + exc;
					System.out.println("INTERNAL ERROR Getting Frame values: " + exc);
					return null;
				}
				break;
			case FrameData.BITMAP_IMAGE_U16:
			case FrameData.BITMAP_IMAGE_16:
			{
				b = new ByteArrayInputStream(buf);
				din = new DataInputStream(b);
				try
				{
					for (int j = 0; j < n_pix; j++)
						if (frameType == FrameData.BITMAP_IMAGE_U16)
							values[j] = 0xffff & din.readShort();
						else
							values[j] = din.readShort();
				}
				catch (final IOException exc)
				{
					error = "INTERNAL ERROR Getting Frame values: " + exc;
					System.out.println("INTERNAL ERROR Getting Frame values: " + exc);
					return null;
				}
				break;
			}
			case FrameData.BITMAP_IMAGE_U32:
			case FrameData.BITMAP_IMAGE_32:
			{
				try
				{
					for (int j = 0; j < n_pix; j++)
						values[j] = din.readInt();
				}
				catch (final IOException exc)
				{
					error = "INTERNAL ERROR Getting Frame values: " + exc;
					System.out.println("INTERNAL ERROR Getting Frame values: " + exc);
					return null;
				}
				break;
			}
			case FrameData.BITMAP_IMAGE_F32:
			{
				try
				{
					for (int j = 0; j < n_pix; j++)
						values[j] = din.readFloat();
				}
				catch (final IOException exc)
				{
					error = "INTERNAL ERROR Getting Frame values: " + exc;
					System.out.println("INTERNAL ERROR Getting Frame values: " + exc);
					return null;
				}
				break;
			}
			case FrameData.AWT_IMAGE:
				break;
			default:
				error = "INTERNAL ERROR frame values requested for unexpected type: " + frameType;
				System.out.println("INTERNAL ERROR frame values requested for unexpected type: " + frameType);
				return null;
			}
			return values;
		}

		void loadFrame(int idx) throws Exception
		{
			ColorModel colorModel;
			frameType = fd.getFrameType();
			frameDim = fd.getFrameDimension();
			numFrames = fd.getNumFrames();
			final byte[] buf = fd.getFrameAt(idx);
			if (buf == null)
				return;
			Image img;
			DataBuffer db;
			WritableRaster raster;
			switch (frameType)
			{
			case FrameData.BITMAP_IMAGE_U8:
			case FrameData.BITMAP_IMAGE_8:
			{
				pixelSize = 8;
				bytesPerPixel = 1;
				//colorModel = colorMap.getIndexColorModel(8);
				colorModel = colorMap.getIndexColorModel(16);
				final int n_pix = frameDim.width * frameDim.height;
				db = new DataBufferUShort(n_pix);
				short val;
				for (int j = 0; j < n_pix; j++)
				{
                                        if(buf[j] < 0) //Signed/Unsigned issue
                                            val = (short)(256+(buf[j] & (byte)0x7F));
                                        else
                                            val = buf[j];
                                        if (val < min)
						db.setElem(j, (byte) 0);
					else if (val > max)
						db.setElem(j, (byte) 255);
					else
						db.setElem(j, (byte) ((val - min) * 512 / (max - min)));
					 //System.out.println(""+j+" val "+val+" db " + ( db.getElem(j) ) );
					db.setElem(j, val);
				}

                                raster = Raster.createInterleavedRaster(db, frameDim.width, frameDim.height, frameDim.width, 1,
						new int[]
						{ 0 }, null);
                 		img = new BufferedImage(colorModel, raster, false, null);
                                break;
			}
			case FrameData.BITMAP_IMAGE_U16:
			case FrameData.BITMAP_IMAGE_16:
			{
				pixelSize = 16;
				bytesPerPixel = 2;
				colorModel = colorMap.getIndexColorModel(16);
				if (colorMap.bitShift < 0)
				{}
				else
				{}
				bitShift = colorMap.bitShift;
				// FlipFrame(buf, frameDim, 2);
				final int n_pix = frameDim.width * frameDim.height;
				db = new DataBufferUShort(n_pix);
				final ByteArrayInputStream b = new ByteArrayInputStream(buf);
				final DataInputStream din = new DataInputStream(b);
				int val;
				for (int j = 0; j < n_pix; j++)
				{
					if (frameType == FrameData.BITMAP_IMAGE_16)
						val = din.readShort();
					else
						val = 0xFFFF & din.readShort();
					if (val < min)
						db.setElem(j, 0);
					else if (val > max)
						db.setElem(j, 255);
					else
						db.setElem(j, (int) ((val - min) * 255 / (max - min)));
				}
				raster = Raster.createInterleavedRaster(db, frameDim.width, frameDim.height, frameDim.width, 1,
						new int[]
						{ 0 }, null);
				img = new BufferedImage(colorModel, raster, false, null);
				break;
			}
			case FrameData.BITMAP_IMAGE_U32:
			case FrameData.BITMAP_IMAGE_32:
			{
				pixelSize = 32;
				bytesPerPixel = 4;
				colorModel = colorMap.getIndexColorModel(16);
				final int n_pix = frameDim.width * frameDim.height;
				final ByteArrayInputStream b = new ByteArrayInputStream(buf);
				final DataInputStream din = new DataInputStream(b);
				db = new DataBufferUShort(n_pix);
				long val;
				for (int j = 0; j < n_pix; j++)
				{
					if (frameType == FrameData.BITMAP_IMAGE_32)
						val = din.readInt();
					else
						val = 0xFFFFFFFF & din.readInt();
					if (val < min)
						db.setElem(j, 0);
					else if (val > max)
						db.setElem(j, 255);
					else
						db.setElem(j, (int) ((val - min) * 255 / (max - min)));
				}
				// db = new DataBufferUShort(buf_out, buf.length);
				raster = Raster.createInterleavedRaster(db, frameDim.width, frameDim.height, frameDim.width, 1,
						new int[]
						{ 0 }, null);
				img = new BufferedImage(colorModel, raster, false, null);
				break;
			}
			case FrameData.BITMAP_IMAGE_F32:
			{
				pixelSize = 32;
				bytesPerPixel = 4;
				colorModel = colorMap.getIndexColorModel(16);
				// FlipFrame(buf, frameDim, 4);
				final int n_pix = frameDim.width * frameDim.height;
				final ByteArrayInputStream b = new ByteArrayInputStream(buf);
				final DataInputStream din = new DataInputStream(b);
				db = new DataBufferUShort(n_pix);
				float val;
				for (int j = 0; j < n_pix; j++)
				{
					val = din.readFloat();
					if (val < min)
						db.setElem(j, 0);
					else if (val > max)
						db.setElem(j, 255);
					else
						db.setElem(j, (int) ((val - min) * 255 / (max - min)));
				}
				/*
				 * values = buf_out; short buf_out1[] = new short[n_pix]; for(int j = 0; j <
				 * n_pix; j++) { buf_out1[j] = (short)(255 * (buf_out[j] - min)/(max - min)); }
				 * db = new DataBufferUShort(buf_out1, buf.length);
				 */
				raster = Raster.createInterleavedRaster(db, frameDim.width, frameDim.height, frameDim.width, 1,
						new int[]
						{ 0 }, null);
				img = new BufferedImage(colorModel, raster, false, null);
				break;
			}
			case FrameData.AWT_IMAGE:
				// img = Toolkit.getDefaultToolkit().createImage(buf);
				pixelSize = 0; // pixelSize == 0 means that the image is not a bitmap (jpg, gif...)
				img = ImageIO.read(new ByteArrayInputStream(buf));
				break;
			default:
				error = "Frame data type (MDSplus code " + frameType + ") is not yet supported";
				throw (new Exception(error));
			}
			tracker = new MediaTracker(Frames.this);
			tracker.addImage(img, idx);
			recentFrames.put(new Integer(idx), new FrameDescriptor(buf, img, img, 0));
			try
			{
				tracker.waitForID(idx);
			}
			catch (final Exception exc)
			{}
			int maxStoreFrames = MAX_CACHE_MEM / buf.length;
			recentIdxV.insertElementAt(new Integer(idx), 0);
			if (maxStoreFrames < 1)
				maxStoreFrames = 1;
			if (recentIdxV.size() > maxStoreFrames)
			{
				final Object delIdx = recentIdxV.elementAt(maxStoreFrames);
				recentFrames.remove(delIdx);
				recentIdxV.removeElementAt(maxStoreFrames);
			}
		}

		void setColorMap(ColorMap colorMap)
		{
			this.colorMap = colorMap;
			this.min = colorMap.getMin();
			this.max = colorMap.getMax();
			updateCount++;
		}

		void setFrameData(FrameData fd)
		{
			this.fd = fd;
			try
			{
				numFrames = fd.getNumFrames();
			}
			catch (final Exception exc)
			{
				numFrames = 0;
			}
		}

		public void setMinMax(float min, float max)
		{
			this.min = min;
			this.max = max;
			updateCount++;
		}

		public void shiftImagePixel(int bitShift, boolean bitClip)
		{
			this.bitShift = bitShift;
			this.bitClip = bitClip;
			updateCount++;
		}
	} // End class FrameCache
	// Frame data cache management class
	static class FrameDescriptor
	{
		byte[] buffer;
		Image image;
		Image updatedImage;
		int updateCount;

		FrameDescriptor(byte[] buffer, Image image, Image updatedImage, int updateCount)
		{
			this.buffer = buffer;
			this.image = image;
			this.updatedImage = updatedImage;
			this.updateCount = updateCount;
		}
	}
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	static final int ROI = 20;
	public static int DecodeImageType(byte buf[])
	{
		final String s = new String(buf, 0, 20);
		if (s.indexOf("GIF") == 0)
			return FrameData.AWT_IMAGE;
		if (s.indexOf("JFIF") == 6)
			return FrameData.AWT_IMAGE;
		return FrameData.JAI_IMAGE;
	}
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

	protected String error;
	// protected int[] frame_type;

	FrameCache cache;

	public Frames()
	{
		super();
		img_width = -1;
		img_height = -1;
		cache = new FrameCache();
	}

	Frames(Frames frames)
	{
		this();
		cache = new FrameCache(frames.cache);
		if (frame_time.size() != 0)
			frame_time.removeAllElements();
		if (frames.zoom_rect != null)
			zoom_rect = new Rectangle(frames.zoom_rect);
		if (frames.view_rect != null)
			view_rect = new Rectangle(frames.view_rect);
		curr_frame_idx = frames.curr_frame_idx;
	}

	public void applyColorModel(ColorMap colorMap)
	{
		if (colorMap == null)
			return;
		cache.setColorMap(colorMap);
		/*
		 * BufferedImage img = null; Vector newFrame = new Vector(); ColorModel
		 * colorModel;
		 *
		 *
		 * for(int i = 0; i < frame.size(); i++) { colorModel =
		 * colorMap.getIndexColorModel(getPixelSize(i)); img =
		 * (BufferedImage)frame.elementAt(i); img = new BufferedImage(colorModel,
		 * img.getRaster(), false, null); newFrame.add(img); }
		 * frame.removeAllElements(); frame = newFrame;
		 */ }

	public boolean contain(Point p, Dimension d)
	{
		final Dimension fr_dim = getFrameSize(curr_frame_idx, d);
		if (p.x > fr_dim.width || p.y > fr_dim.height)
			return false;
		return true;
	}

	public void FlipFrame(byte buf[], Dimension d, int num_byte_pixel)
	{
		if (!vertical_flip && !horizontal_flip)
			return;
		final int img_size = d.height * d.width * num_byte_pixel;
		final byte tmp[] = new byte[img_size];
		int j, k, l;
		final int h = vertical_flip ? d.height - 1 : 0;
		final int w = horizontal_flip ? d.width - 1 : 0;
		for (j = 0; j < d.width; j++)
			for (k = 0; k < d.height; k++)
				for (l = 0; l < num_byte_pixel; l++)
					tmp[((Math.abs(h - k) * d.width) + Math.abs(w - j)) * num_byte_pixel
							+ l] = buf[((k * d.width) + j) * num_byte_pixel + l];
		System.arraycopy(tmp, 0, buf, 0, img_size);
	}

	public void flipFrames(byte buf[])
	{
		if (!vertical_flip && !horizontal_flip)
			return;
		try
		{
			final ByteArrayInputStream b = new ByteArrayInputStream(buf);
			final DataInputStream d = new DataInputStream(b);
			final int width = d.readInt();
			final int height = d.readInt();
			final int img_size = height * width;
			final int n_frame = d.readInt();
			d.close();
			final byte tmp[] = new byte[img_size];
			int i, j, k, ofs;
			final int h = vertical_flip ? height - 1 : 0;
			final int w = horizontal_flip ? width - 1 : 0;
			ofs = 12 + 4 * n_frame;
			for (i = 0; i < n_frame; i++)
			{
				for (j = 0; j < width; j++)
					for (k = 0; k < height; k++)
						tmp[(Math.abs(h - k) * width) + Math.abs(w - j)] = buf[ofs + (k * width) + j];
				System.arraycopy(tmp, 0, buf, ofs, img_size);
				ofs += img_size;
			}
		}
		catch (final IOException e)
		{}
	}

	public boolean getAspectRatio()
	{ return aspect_ratio; }

	public int GetColorIdx()
	{
		return color_idx;
	}

	public ColorMap getColorMap()
	{ return cache.getColorMap(); }

	String getError()
	{ return error; }

	public Image GetFrame(int idx)
	{
		if (idx < 0)
			return null;
		final int numFrames = cache.getNumFrames();
		if (idx >= numFrames)
			idx = numFrames - 1;
		// if(idx < 0)// || frame.elementAt(idx) == null) return null;
		curr_frame_idx = idx;
		Image img;
		try
		{
			img = cache.getImageAt(idx);
		}
		catch (final Exception exc)
		{
			img = null;
		}
		if (img != null && img_width == -1)
		{
			img_width = img.getWidth(this);
			img_height = img.getHeight(this);
		}
		// curr_grab_frame = curr_frame_idx;
		return img;
	}

	public Image GetFrame(int idx, Dimension d)
	{
		return GetFrame(idx);
	}

	protected Dimension GetFrameDim(int idx)
	{
		return cache.getFrameDimension();
		/*
		 * return new Dimension( ((Image)frame.elementAt(idx)).getWidth(this),
		 * ((Image)frame.elementAt(idx)).getHeight(this));
		 */ }

	public int GetFrameIdx()
	{
		return curr_frame_idx;
	}

	public int GetFrameIdxAtTime(float t)
	{
		int idx = -1;
		float dt;
		final int numFrames = cache.getNumFrames();
		if (numFrames <= 0)
			return -1;
		if (numFrames == 1)
			dt = 1;
		else
			dt = frame_time.elementAt(1).floatValue() - frame_time.elementAt(0).floatValue();
		if (t >= frame_time.elementAt(numFrames - 1).floatValue() + dt)
			return -1;
		if (t >= frame_time.elementAt(numFrames - 1).floatValue())
			return numFrames - 1;
		for (int i = 0; i < numFrames - 1; i++)
		{
			if (t >= frame_time.elementAt(i).floatValue() && t < frame_time.elementAt(i + 1).floatValue())
			{
				idx = i;
				break;
			}
		}
		return idx;
	}

	public Point getFramePoint(Dimension d)
	{
		if (sel_point != null)
			return getImagePoint(new Point(sel_point.x, sel_point.y), d);
		else
			return new Point(0, 0);
	}

	// return pixel position in the frame
	// Argument point is fit to frame dimension
	public Point getFramePoint(Point p, Dimension d)
	{
		final Point p_out = new Point(0, 0);
		if (p.x < 0)
			p.x = 0;
		if (p.y < 0)
			p.y = 0;
		if (curr_frame_idx != -1 && cache.getNumFrames() != 0)
		{
			Dimension view_dim;
			Dimension dim;
			final Dimension fr_dim = getFrameSize(curr_frame_idx, d);
			if (zoom_rect == null)
			{
				view_dim = GetFrameDim(curr_frame_idx);
				dim = view_dim;
			}
			else
			{
				dim = new Dimension(zoom_rect.width, zoom_rect.height);
				view_dim = new Dimension(zoom_rect.x + zoom_rect.width, zoom_rect.y + zoom_rect.height);
				p_out.x = zoom_rect.x;
				p_out.y = zoom_rect.y;
			}
			final double ratio_x = (double) dim.width / fr_dim.width;
			final double ratio_y = (double) dim.height / fr_dim.height;
			p_out.x += ratio_x * p.x;
			if (p_out.x > view_dim.width - 1)
			{
				p_out.x = view_dim.width - 1;
				p.x = fr_dim.width;
			}
			p_out.y += ratio_y * p.y;
			if (p_out.y > view_dim.height - 1)
			{
				p_out.y = view_dim.height - 1;
				p.y = fr_dim.height;
			}
		}
		return p_out;
	}

	/* returm frame image pixel dimension */
	public Dimension getFrameSize(int idx, Dimension d)
	{
		int width, height;
		// Border image pixel
		final Dimension dim_b = new Dimension(d.width - 1, d.height - 1);
		width = dim_b.width;
		height = dim_b.height;
		if (getAspectRatio())
		{
			final Dimension dim = GetFrameDim(idx);
			int w = dim.width;
			int h = dim.height;
			if (zoom_rect != null)
			{
				w = zoom_rect.width;
				h = zoom_rect.height;
			}
			final double ratio = (double) w / h;
			width = (int) (ratio * d.height);
			if (width > d.width)
			{
				width = d.width;
				height = (int) (d.width / ratio);
			}
		}
		/*
		 * Temporary fix, in order to avoid modification image if it is resized, must be
		 * investigate
		 */
		return new Dimension(width, height);
//      return GetFrameDim(idx);
	}

	public float[] getFramesTime()
	{
		if (frame_time == null || frame_time.size() == 0)
			return null;
		if (ft == null)
		{
			ft = new float[frame_time.size()];
			for (int i = 0; i < frame_time.size(); i++)
			{
				ft[i] = frame_time.elementAt(i).floatValue();
			}
		}
		return ft;
	}

	public float GetFrameTime()
	{
		float t_out = 0;
		if (curr_frame_idx != -1 && frame_time != null && !frame_time.isEmpty())
		{
			t_out = frame_time.elementAt(curr_frame_idx).floatValue();
		}
		return t_out;
	}

	public int getFrameType()
	{ return cache.getFrameType(); }

	public boolean getHorizontalFlip()
	{ return horizontal_flip; }

	private Point getImageBufferPoint(int x, int y)
	{
		final Point p = new Point();
		p.x = x;
		p.y = y;
		if (this.horizontal_flip)
			p.y = this.img_height - y - 1;
		if (this.vertical_flip)
			p.x = this.img_width - x - 1;
		return p;
	}

	// return point position in the frame shows
	public Point getImagePoint(Point p, Dimension d)
	{
		final Point p_out = new Point(0, 0);
		if (curr_frame_idx != -1 && cache.getNumFrames() != 0)
		{
			final Dimension fr_dim = getFrameSize(curr_frame_idx, d);
			Dimension view_dim;
			Dimension dim;
			if (zoom_rect == null)
			{
				view_dim = GetFrameDim(curr_frame_idx);
				dim = view_dim;
			}
			else
			{
				dim = new Dimension(zoom_rect.width, zoom_rect.height);
				view_dim = new Dimension(zoom_rect.x + zoom_rect.width, zoom_rect.y + zoom_rect.height);
				p.x -= zoom_rect.x;
				p.y -= zoom_rect.y;
			}
			final double ratio_x = (double) fr_dim.width / dim.width;
			final double ratio_y = (double) fr_dim.height / dim.height;
			p_out.x = (int) (ratio_x * p.x + ratio_x / 2);
			if (p_out.x > (dim.width - 1) * ratio_x)
			{
				p_out.x = 0;
				p_out.y = 0;
			}
			else
			{
				p_out.y = (int) (ratio_y * p.y + ratio_y / 2);
				if (p_out.y > (dim.height - 1) * ratio_y)
				{
					p_out.x = 0;
					p_out.y = 0;
				}
			}
		}
		return p_out;
	}

	public int getLastFrameIdx()
	{
		if (curr_frame_idx - 1 < 0)
			return 0;
		else
			return curr_frame_idx -= 1;
	}

	public Point getMeasurePoint(Dimension d)
	{
		return getImagePoint(new Point(x_measure_pixel, y_measure_pixel), d);
	}

	public int getNextFrameIdx()
	{
		if (curr_frame_idx + 1 == getNumFrame())
			return curr_frame_idx;
		else
			return curr_frame_idx += 1;
	}

	public int getNumFrame()
	{
		return cache.getNumFrames();
		// return frame.size();
	}

	public int getPixel(int idx, int x, int y)
	{
		if (!isInImage(idx, x, y))
			return -1;
		// byte[] imgBuf = cache.getBufferAt(idx);
		try
		{
			final BufferedImage img = (BufferedImage) cache.getImageAt(idx);
			final Point p = getImageBufferPoint(x, y);
			if (img != null)
				return img.getRGB(p.y, p.x);
		}
		catch (final Exception exc)
		{}
		return -1;
		/*
		 * try { if(imgBuf != null) return (int)imgBuf[(p.y * img_width) + p.x];
		 * }catch(Exception exc) {return -1;} return -1;
		 */
	}

	protected int[] getPixelArray(int idx, int x, int y, int img_w, int img_h)
	{
		error = "";
		Image img;
		try
		{
			img = cache.getImageAt(idx);
		}
		catch (final Exception exc)
		{
			error = "INTERNAL ERROR in Frame.getPixelArrat: " + exc;
			System.out.println("INTERNAL ERROR in Frame.getPixelArrat: " + exc);
			return null;
		}
		if (img_w == -1 && img_h == -1)
		{
			img_width = img_w = img.getWidth(this);
			img_height = img_h = img.getHeight(this);
		}
		final int pixel_array[] = new int[img_w * img_h];
		final PixelGrabber grabber = new PixelGrabber(img, x, y, img_w, img_h, pixel_array, 0, img_w);
		try
		{
			grabber.grabPixels();
		}
		catch (final InterruptedException ie)
		{
			error = "Pixel array not completed";
			System.err.println("Pixel array not completed");
			return null;
		}
		return pixel_array;
	}

	public int[] getPixelsLine(int st_x, int st_y, int end_x, int end_y)
	{
		Point p;
		final int n_point = (int) (Math.sqrt(Math.pow(st_x - end_x, 2.0) + Math.pow(st_y - end_y, 2.0)) + 0.5);
		int x, y;
		int pixels_line[] =
		{ pixel_array[(st_y * img_width) + st_x], pixel_array[(st_y * img_width) + st_x] };
		grabFrame();
		if (n_point < 2)
		{
			pixels_line = new int[2];
			p = getImageBufferPoint(st_x, st_y);
			pixels_line[0] = pixels_line[1] = pixel_array[(p.y * img_width) + p.x];
			return pixels_line;
		}
		pixels_line = new int[n_point];
		for (int i = 0; i < n_point; i++)
		{
			x = (int) (st_x + (double) i * (end_x - st_x) / n_point);
			y = (int) (st_y + (double) i * (end_y - st_y) / n_point);
			p = getImageBufferPoint(x, y);
			pixels_line[i] = pixel_array[(p.y * img_width) + p.x];
		}
		return pixels_line;
	}

	public int[] getPixelsSignal(int x, int y)
	{
		int pixels_signal[] = null;
		if (frames_pixel_array == null || !frames_pixel_roi.contains(x, y))
		{
			frames_pixel_roi = new Rectangle();
			if (zoom_rect == null)
			{
				zoom_rect = new Rectangle(0, 0, img_width, img_height);
			}
			frames_pixel_roi.x = (x - ROI >= zoom_rect.x ? x - ROI : zoom_rect.x);
			frames_pixel_roi.y = (y - ROI >= zoom_rect.y ? y - ROI : zoom_rect.y);
			frames_pixel_roi.width = (frames_pixel_roi.x + 2 * ROI <= zoom_rect.x + zoom_rect.width ? 2 * ROI
					: zoom_rect.width - (frames_pixel_roi.x - zoom_rect.x));
			frames_pixel_roi.height = (frames_pixel_roi.y + 2 * ROI <= zoom_rect.y + zoom_rect.height ? 2 * ROI
					: zoom_rect.height - (frames_pixel_roi.y - zoom_rect.y));
			frames_pixel_array = new int[frames_pixel_roi.width * frames_pixel_roi.height * getNumFrame()];
			int f_array[];
			for (int i = 0; i < getNumFrame(); i++)
			{
				f_array = getPixelArray(i, frames_pixel_roi.x, frames_pixel_roi.y, frames_pixel_roi.width,
						frames_pixel_roi.height);
				System.arraycopy(f_array, 0, frames_pixel_array, f_array.length * i, f_array.length);
			}
		}
		if (frames_pixel_array != null)
		{
			x -= frames_pixel_roi.x;
			y -= frames_pixel_roi.y;
			final int size = frames_pixel_roi.width * frames_pixel_roi.height;
			pixels_signal = new int[getNumFrame()];
			for (int i = 0; i < getNumFrame(); i++)
			{
				pixels_signal[i] = frames_pixel_array[size * i + (y * frames_pixel_roi.width) + x];
			}
		}
		return pixels_signal;
	}

	public int[] getPixelsX(int y)
	{
		Point p;
		int pixels_x[] = null;
		int st, end;
		grabFrame();
		if (pixel_array != null && y < img_height)
		{
			if (zoom_rect != null)
			{
				st = zoom_rect.x;
				end = zoom_rect.x + zoom_rect.width;
			}
			else
			{
				st = 0;
				end = img_width;
			}
			pixels_x = new int[end - st];
			for (int i = st, j = 0; i < end; i++, j++)
			{
				p = getImageBufferPoint(i, y);
				// pixels_x[j] = pixel_array[(y * img_width) + i];
				pixels_x[j] = pixel_array[(p.y * img_width) + p.x];
			}
		}
		return pixels_x;
	}

	public int[] getPixelsY(int x)
	{
		Point p;
		int pixels_y[] = null;
		int st, end;
		grabFrame();
		if (pixel_array != null && x < img_width)
		{
			if (zoom_rect != null)
			{
				st = zoom_rect.y;
				end = zoom_rect.y + zoom_rect.height;
			}
			else
			{
				st = 0;
				end = img_height;
			}
			pixels_y = new int[end - st];
			for (int i = st, j = 0; i < end; i++, j++)
			{
				p = getImageBufferPoint(x, i);
				// pixels_y[j] = pixel_array[(i * img_width) + x];
				pixels_y[j] = pixel_array[(p.y * img_width) + p.x];
			}
		}
		return pixels_y;
	}

	public float getPointValue(int idx, int x, int y)
	{
		if (!isInImage(idx, x, y))
			return -1;
		// curr_grab_frame = idx;
		values_array = cache.getValuesAt(idx);
		final Point p = getImageBufferPoint(x, y);
		if (values_array != null)
			return values_array[(p.y * img_width) + p.x];
		return (float) 0.0;
	}

	public int getStartPixelX()
	{
		if (zoom_rect != null)
			return zoom_rect.x;
		else
			return 0;
	}

	public int getStartPixelY()
	{
		if (zoom_rect != null)
			return zoom_rect.y;
		else
			return 0;
	}

	public float GetTime(int frame_idx)
	{
		if (frame_idx > cache.getNumFrames() - 1 || frame_idx < 0)
			return (float) 0.0;
		return frame_time.elementAt(frame_idx).floatValue();
	}

	protected float[] getValueArray(int idx, int x, int y, int img_w, int img_h)
	{
		float values[];
		Image img;
		try
		{
			values = cache.getValuesAt(idx);
			img = cache.getImageAt(idx);
		}
		catch (final Exception exc)
		{
			return null;
		}
		if (img_w == -1 && img_h == -1)
		{
			img_width = img_w = img.getWidth(this);
			img_height = img_h = img.getHeight(this);
			return values;
		}
		final float values_array[] = new float[img_w * img_h];
		int k = 0;
//       for(int j = y; j < img_h; j++)
//            for(int i = x; i < img_w; i++)
		for (int j = y; j < y + img_h; j++)
			for (int i = x; i < x + img_w; i++)
				values_array[k++] = values[j * img_width + i];
		return values_array;
	}

	public float[] getValuesLine(int st_x, int st_y, int end_x, int end_y)
	{
		Point p;
		final int n_point = (int) (Math.sqrt(Math.pow(st_x - end_x, 2.0) + Math.pow(st_y - end_y, 2.0)) + 0.5);
		int x, y;
		// float values_line[] = {values_array[(st_y * img_width) + st_x],
		// values_array[(st_y * img_width) + st_x]};
		float values_line[];
		grabFrame();
		if (n_point < 2)
		{
			values_line = new float[2];
			p = getImageBufferPoint(st_x, st_y);
			values_line[0] = values_line[1] = values_array[(p.y * img_width) + p.x];
			return values_line;
		}
		values_line = new float[n_point];
		for (int i = 0; i < n_point; i++)
		{
			x = (int) (st_x + (double) i * (end_x - st_x) / n_point);
			y = (int) (st_y + (double) i * (end_y - st_y) / n_point);
			p = getImageBufferPoint(x, y);
			values_line[i] = values_array[(p.y * img_width) + p.x];
		}
		return values_line;
	}

	public float[] getValuesSignal(int x, int y)
	{
		float values_signal[] = null;
		if (frames_value_array == null || !frames_pixel_roi.contains(x, y))
		{
			frames_pixel_roi = new Rectangle();
			if (zoom_rect == null)
			{
				zoom_rect = new Rectangle(0, 0, img_width, img_height);
			}
			frames_pixel_roi.x = (x - ROI >= zoom_rect.x ? x - ROI : zoom_rect.x);
			frames_pixel_roi.y = (y - ROI >= zoom_rect.y ? y - ROI : zoom_rect.y);
			frames_pixel_roi.width = (frames_pixel_roi.x + 2 * ROI <= zoom_rect.x + zoom_rect.width ? 2 * ROI
					: zoom_rect.width - (frames_pixel_roi.x - zoom_rect.x));
			frames_pixel_roi.height = (frames_pixel_roi.y + 2 * ROI <= zoom_rect.y + zoom_rect.height ? 2 * ROI
					: zoom_rect.height - (frames_pixel_roi.y - zoom_rect.y));
			frames_value_array = new float[frames_pixel_roi.width * frames_pixel_roi.height * getNumFrame()];
			float f_array[];
			for (int i = 0; i < getNumFrame(); i++)
			{
				f_array = getValueArray(i, frames_pixel_roi.x, frames_pixel_roi.y, frames_pixel_roi.width,
						frames_pixel_roi.height);
				System.arraycopy(f_array, 0, frames_value_array, f_array.length * i, f_array.length);
			}
		}
		if (frames_value_array != null)
		{
			x -= frames_pixel_roi.x;
			y -= frames_pixel_roi.y;
			final int size = frames_pixel_roi.width * frames_pixel_roi.height;
			values_signal = new float[getNumFrame()];
			for (int i = 0; i < getNumFrame(); i++)
			{
				values_signal[i] = frames_value_array[size * i + (y * frames_pixel_roi.width) + x];
			}
		}
		return values_signal;
	}

	public float[] getValuesX(int y)
	{
		Point p;
		float values_x[] = null;
		int st, end;
		grabFrame();
		if (values_array != null && y < img_height)
		{
			if (zoom_rect != null)
			{
				st = zoom_rect.x;
				end = zoom_rect.x + zoom_rect.width;
			}
			else
			{
				st = 0;
				end = img_width;
			}
			values_x = new float[end - st];
			for (int i = st, j = 0; i < end; i++, j++)
			{
				p = getImageBufferPoint(i, y);
				// values_x[j] = values_array[(y * img_width) + i];
				values_x[j] = values_array[(p.y * img_width) + p.x];
			}
		}
		return values_x;
	}

	public float[] getValuesY(int x)
	{
		Point p;
		float values_y[] = null;
		int st, end;
		grabFrame();
		if (values_array != null && x < img_width)
		{
			if (zoom_rect != null)
			{
				st = zoom_rect.y;
				end = zoom_rect.y + zoom_rect.height;
			}
			else
			{
				st = 0;
				end = img_height;
			}
			values_y = new float[end - st];
			for (int i = st, j = 0; i < end; i++, j++)
			{
				p = getImageBufferPoint(x, i);
				// values_y[j] = values_array[(i * img_width) + x];
				values_y[j] = values_array[(p.y * img_width) + p.x];
			}
		}
		return values_y;
	}

	public boolean getVerticalFlip()
	{ return vertical_flip; }

	public Rectangle GetZoomRect()
	{
		return zoom_rect;
	}

	protected void grabFrame()
	{
		if (curr_frame_idx != curr_grab_frame || pixel_array == null)
		{
			if ((pixel_array = getPixelArray(curr_frame_idx, 0, 0, -1, -1)) != null)
			{
				values_array = getValueArray(curr_frame_idx, 0, 0, -1, -1);
				curr_grab_frame = curr_frame_idx;
			}
		}
	}

	public boolean isInImage(int idx, int x, int y)
	{
		final Dimension d = this.GetFrameDim(idx);
		final Rectangle r = new Rectangle(0, 0, d.width, d.height);
		return r.contains(x, y);
	}

	public void Resize()
	{
		zoom_rect = null;
	}

	public void setAspectRatio(boolean aspect_ratio)
	{ this.aspect_ratio = aspect_ratio; }

	/*
	 *
	 * public boolean AddAWTImage(byte[] buf, float t) { Image img; if(buf == null)
	 * return false; //img = getToolkit().createImage(buf); img =
	 * Toolkit.getDefaultToolkit().createImage(buf); return AddFrame(img, t); }
	 *
	 * public void AddJAIImage(byte[] buf, float t) throws IOException { }
	 */
	public void SetColorIdx(int color_idx)
	{
		this.color_idx = color_idx;
	}

	public void setColorMap(ColorMap colorMap)
	{
		if (colorMap == null)
			return;
		cache.setColorMap(colorMap);
	}

	/*
	 * public int getPixelSize(int idx) { if(getNumFrame() == 0) return 8;
	 * switch(frame_type[idx]) { case FrameData.BITMAP_IMAGE_8 : return 8; case
	 * FrameData.BITMAP_IMAGE_16 : return 16; case FrameData.BITMAP_IMAGE_32 :
	 * return 16; default: return 8; } }
	 *
	 * protected void finalize() { if(frame.size() != 0) frame.removeAllElements();
	 * if(frame_time.size() != 0) frame_time.removeAllElements(); tracker = null; }
	 */
	public void SetFrameData(FrameData fd) throws Exception
	{
		cache.setFrameData(fd);
		curr_frame_idx = 0;
		final float t[] = fd.getFrameTimes();
		for (int i = 0; i < t.length; i++)
			frame_time.addElement(new Float(t[i]));
	}

	public void setFramePoint(Point sel_point, Dimension d)
	{
		this.sel_point = getFramePoint(new Point(sel_point.x, sel_point.y), d);
	}

	public void setHorizontalFlip(boolean horizontal_flip)
	{ this.horizontal_flip = horizontal_flip; }

	public void setMeasurePoint(int x_pixel, int y_pixel, Dimension d)
	{
		final Point mp = getFramePoint(new Point(x_pixel, y_pixel), d);
		x_measure_pixel = mp.x;
		y_measure_pixel = mp.y;
	}

	public void setMinMax(float min, float max)
	{
		cache.setMinMax(min, max);
	}

	public void setVerticalFlip(boolean vertical_flip)
	{ this.vertical_flip = vertical_flip; }

	public void SetViewRect(int start_x, int start_y, int end_x, int end_y)
	{
		view_rect = null;
		if (start_x == -1 && start_y == -1 && end_x == -1 && end_y == -1)
			return;
		if (getNumFrame() == 0)
			return;
		final Dimension dim = GetFrameDim(0);
		if (start_x < 0)
			start_x = 0;
		if (start_y < 0)
			start_y = 0;
		if (end_x == -1 || end_x > dim.width)
			end_x = dim.width;
		if (end_y == -1 || end_y > dim.height)
			end_y = dim.height;
		if (start_x < end_x && start_y < end_y)
		{
			view_rect = new Rectangle(start_x, start_y, end_x - start_x, end_y - start_y);
			zoom_rect = view_rect;
		}
	}

	public void SetZoomRegion(int idx, Dimension d, Rectangle r)
	{
		final int numFrames = cache.getNumFrames();
		if (idx > numFrames - 1)// || frame.elementAt(idx) == null )
			return;
		Dimension dim;
		final Dimension fr_dim = getFrameSize(idx, d);
		if (zoom_rect == null)
		{
			zoom_rect = new Rectangle(0, 0, 0, 0);
			dim = GetFrameDim(idx);
		}
		else
		{
			dim = new Dimension(zoom_rect.width, zoom_rect.height);
		}
		final double ratio_x = (double) dim.width / fr_dim.width;
		final double ratio_y = (double) dim.height / fr_dim.height;
		zoom_rect.width = (int) (ratio_x * r.width + 0.5);
		zoom_rect.height = (int) (ratio_y * r.height + 0.5);
		zoom_rect.x += ratio_x * r.x + 0.5;
		zoom_rect.y += ratio_y * r.y + 0.5;
		if (zoom_rect.width == 0)
			zoom_rect.width = 1;
		if (zoom_rect.height == 0)
			zoom_rect.height = 1;
		curr_frame_idx = idx;
	}

	/*
	 * public boolean AddBITMAPImage(byte[] buf, ColorModel colorModel, Dimension d,
	 * float t) { Image img; MemoryImageSource source; if(buf == null || d == null)
	 * return false; DataBuffer db = new DataBufferByte(buf, buf.length);
	 * WritableRaster raster = Raster.createInterleavedRaster(db, d.width, d.height,
	 * d.width, 1, new int[]{0}, null); img = new BufferedImage(colorModel, raster,
	 * false, null);
	 *
	 * return AddFrame(img, t); }
	 *
	 *
	 * public boolean AddBITMAPImage(short[] buf, ColorModel colorModel, Dimension
	 * d, float t) { BufferedImage img; MemoryImageSource source; if(buf == null ||
	 * d == null) return false;
	 *
	 * DataBuffer db = new DataBufferUShort(buf, buf.length); WritableRaster raster
	 * = Raster.createInterleavedRaster(db, d.width, d.height, d.width, 1, new
	 * int[]{0}, null);
	 *
	 * img = new BufferedImage(colorModel, raster, false, null);
	 *
	 *
	 * return AddFrame(img, t); }
	 *
	 *
	 * public boolean AddBITMAPImage(int[] buf, ColorModel colorModel, Dimension d,
	 * float t) { Image img; MemoryImageSource source; if(buf == null || d == null)
	 * return false; DataBuffer db = new DataBufferInt(buf, buf.length);
	 * WritableRaster raster = Raster.createInterleavedRaster(db, d.width, d.height,
	 * d.width, 1, new int[]{0}, null); img = new BufferedImage(colorModel, raster,
	 * false, null);
	 *
	 * return AddFrame(img, t); }
	 */
	public void shiftImagePixel(int bitShift, boolean bitClip)
	{
		cache.shiftImagePixel(bitShift, bitClip);
	}
}

package mds.provider;

import java.awt.Dimension;
import java.awt.image.BufferedImage;
import java.io.*;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.*;

import javax.imageio.ImageIO;
import javax.swing.JFrame;
import javax.swing.JOptionPane;

import mds.connection.*;
import mds.provider.mds.SshTunneling;
import mds.wave.*;

public class MdsDataProvider implements DataProvider
{
	public interface AsyncDataSource
	{
		void addDataListener(WaveDataListener l);

		void startGeneration(String expression);
	}

	static class RealArray
	{
		double doubleArray[] = null;
		float floatArray[] = null;
		long longArray[] = null;
		boolean isDouble;
		boolean isLong;

		RealArray(double[] doubleArray)
		{
			this.doubleArray = doubleArray;
			isDouble = true;
			isLong = false;
		}

		RealArray(float[] floatArray)
		{
			this.floatArray = floatArray;
			isDouble = false;
			isLong = false;
		}

		RealArray(long[] longArray)
		{
			this.longArray = longArray;
			for (int i = 0; i < longArray.length; i++)
				longArray[i] = Waveform.convertFromSpecificTime(longArray[i]);
			isDouble = false;
			isLong = true;
		}

		double[] getDoubleArray()
		{
			if (isLong)
				return null;
			if (!isDouble && floatArray != null && doubleArray == null)
			{
				doubleArray = new double[floatArray.length];
				for (int i = 0; i < floatArray.length; i++)
					doubleArray[i] = floatArray[i];
			}
			return doubleArray;
		}

		float[] getFloatArray()
		{
			if (isLong)
				return null;
			if (isDouble && floatArray == null && doubleArray != null)
			{
				floatArray = new float[doubleArray.length];
				for (int i = 0; i < doubleArray.length; i++)
					floatArray[i] = (float) doubleArray[i];
			}
			return floatArray;
		}

		long[] getLongArray()
		{
			if (isDouble)
				return null;
			return longArray;
		}

		boolean isDouble()
		{ return isDouble; }

		boolean isLong()
		{ return isLong; }
	}
	class SegmentedFrameData implements FrameData
	{
		String inX, inY;
		float timeMax, timeMin;
		int framesPerSegment;
		int numSegments;
		int startSegment, endSegment, actSegments;
		int mode;
		Dimension dim;
		float times[];
		int bytesPerPixel;

		public SegmentedFrameData(String inY, String inX, float timeMin, float timeMax, int numSegments)
				throws IOException
		{
			// Find out frames per segment and frame min and max based on time min and time
			// max
			this.inX = inX;
			this.inY = inY;
			this.timeMin = timeMin;
			this.timeMax = timeMax;
			this.numSegments = numSegments;
			startSegment = -1;
			final float startTimes[] = new float[numSegments];
			// Get segment window corresponding to the passed time window
			for (int i = 0; i < numSegments; i++)
			{
				final float limits[] = GetFloatArray("GetSegmentLimits(" + inY + "," + i + ")");
				startTimes[i] = limits[0];
				if (limits[1] > timeMin)
				{
					startSegment = i;
					break;
				}
			}
			if (startSegment == -1)
				throw new IOException("Frames outside defined time window");
//Check first if endTime is greated than the end of the last segment, to avoid rolling over all segments
			float endLimits[] = GetFloatArray("GetSegmentLimits(" + inY + "," + (numSegments - 1) + ")");
//Throw away spurious frames at the end
			while (endLimits == null || endLimits.length != 2)
			{
				numSegments--;
				if (numSegments == 0)
					break;
				endLimits = GetFloatArray("GetSegmentLimits(" + inY + "," + (numSegments - 1) + ")");
			}
			if (numSegments > 100 && endLimits[0] < timeMax)
			{
				endSegment = numSegments - 1;
				for (int i = startSegment; i < numSegments; i++)
					startTimes[i] = startTimes[0] + i * (endLimits[0] - startTimes[0]) / numSegments;
			}
			else
			{
				for (endSegment = startSegment; endSegment < numSegments; endSegment++)
				{
					try
					{
						final float limits[] = GetFloatArray("GetSegmentLimits(" + inY + "," + endSegment + ")");
						startTimes[endSegment] = limits[0];
						if (limits[0] > timeMax)
							break;
					}
					catch (final Exception exc)
					{
						break;
					}
				}
			}
			actSegments = endSegment - startSegment;
//Get Frame Dimension and frames per segment
			final int dims[] = getIntArray("shape(GetSegment(" + inY + ", 0))");
			if (dims.length != 3 && dims.length != 1) // The former refers to usual raster image, the latter to AWT
														// images (jpg, gif...)
				throw new IOException("Invalid number of segment dimensions: " + dims.length);
			if (dims.length == 3)
			{
				dim = new Dimension(dims[0], dims[1]);
				framesPerSegment = dims[2];
//Get Frame element length in bytes
				final int len[] = getIntArray("len(GetSegment(" + inY + ", 0))");
				bytesPerPixel = len[0];
				switch (len[0])
				{
				case 1:
					mode = BITMAP_IMAGE_8;
					break;
				case 2:
					mode = BITMAP_IMAGE_16;
					break;
				case 4:
					mode = BITMAP_IMAGE_32;
					break;
				default:
					throw new IOException("Unexpected length for frame data: " + len[0]);
				}
			}
			else // The degment contains a 1D char buffer, i.e. the binary format of the image
					// (jpg,gif, ...O)
			{
				framesPerSegment = 1;
				mode = AWT_IMAGE;
				bytesPerPixel = 1;
				final byte[] firstSegment = GetByteArray("GetSegment(" + inY + ",0)");
				final BufferedImage img = ImageIO.read(new ByteArrayInputStream(firstSegment));
				dim = new Dimension(img.getWidth(), img.getHeight());
			}
//Get Frame times
			if (framesPerSegment == 1) // We assume in this case that start time is the same of the frame time
			{
				times = new float[actSegments];
				for (int i = 0; i < actSegments; i++)
					times[i] = startTimes[startSegment + i];
			}
			else // Get segment times. We assume that the same number of frames is contained in
					// every segment
			{
				times = new float[actSegments * framesPerSegment];
				for (int i = 0; i < actSegments; i++)
				{
					final float segTimes[] = GetFloatArray("dim_of(GetSegment(" + inY + "," + i + "))");
					if (segTimes.length != framesPerSegment)
						throw new IOException("Inconsistent definition of time in frame + " + i + ": read "
								+ segTimes.length + " times, expected " + framesPerSegment);
					for (int j = 0; j < framesPerSegment; j++)
						times[i * framesPerSegment + j] = segTimes[j];
				}
			}
		}

		@Override
		public byte[] getFrameAt(int idx) throws IOException
		{
			if (debug)
				System.out.println("GetFrameAt " + idx);
			// System.out.println("GET FRAME AT " + idx);
			final int segmentIdx = startSegment + idx / framesPerSegment;
			final int segmentOffset = (idx % framesPerSegment) * dim.width * dim.height * bytesPerPixel;
			final byte[] segment = GetByteArray("GetSegment(" + inY + "," + segmentIdx + ")");
			if (framesPerSegment == 1)
				return segment;
			final byte[] outFrame = new byte[dim.width * dim.height * bytesPerPixel];
			System.arraycopy(segment, segmentOffset, outFrame, 0, dim.width * dim.height * bytesPerPixel);
			return outFrame;
		}

		@Override
		public Dimension getFrameDimension()
		{ return dim; }

		@Override
		public float[] getFrameTimes()
		{ return times; }

		@Override
		public int getFrameType() throws IOException
		{ return mode; }

		@Override
		public int getNumFrames()
		{ return actSegments * framesPerSegment; }
	}
	class SimpleFrameData implements FrameData
	{
		String in_x, in_y;
		float time_max, time_min;
		int mode = -1;
		int pixel_size;
		int first_frame_idx = -1;
		byte buf[];
		String error;
		private int st_idx = -1, end_idx = -1;
		private int n_frames = 0;
		private float times[] = null;
		private Dimension dim = null;
		private int header_size = 0;

		public SimpleFrameData(String in_y, String in_x, float time_min, float time_max) throws Exception
		{
			int i;
			float t;
			float all_times[] = null;
			this.in_y = in_y;
			this.in_x = in_x;
			this.time_min = time_min;
			this.time_max = time_max;
			buf = GetAllFrames(in_y);
			if (buf != null)
			{
				final ByteArrayInputStream b = new ByteArrayInputStream(buf);
				final DataInputStream d = new DataInputStream(b);
				pixel_size = d.readInt();
				final int width = d.readInt();
				final int height = d.readInt();
				final int n_frame = d.readInt();
				dim = new Dimension(width, height);
				if (in_x == null || in_x.length() == 0)
				{
					all_times = new float[n_frame];
					for (i = 0; i < n_frame; i++)
					{
						all_times[i] = d.readFloat();
					}
				}
				else
				{
					// all_times = MdsDataProvider.this.GetWaveData(in_x).GetFloatData();
					all_times = MdsDataProvider.this.getWaveData(in_x, 0, 0, 0).getData(MAX_PIXELS).y;
				}
				header_size = 16 + 4 * n_frame;
				switch (pixel_size)
				{
				case 8:
					mode = BITMAP_IMAGE_8;
					break;
				case 16:
					mode = BITMAP_IMAGE_16;
					break;
				case 32:
					mode = BITMAP_IMAGE_32;
					break;
				}
			}
			else
			{
				final String mframe_error = getError();
				if (in_x == null || in_x.length() == 0)
					all_times = MdsDataProvider.this.GetFrameTimes(in_y);
				else
					// all_times = MdsDataProvider.this.GetWaveData(in_x).GetFloatData();
					all_times = MdsDataProvider.this.getWaveData(in_x, 0, 0, 0).getData(MAX_PIXELS).y;
				if (all_times == null)
				{
					if (mframe_error != null)
						error = " Pulse file or image file not found\nRead pulse file error\n" + mframe_error
								+ "\nFrame times read error";
					else
						error = " Image file not found ";
					if (getError() != null)
						error = error + "\n" + getError();
					throw (new IOException(error));
				}
			}
			for (i = 0; i < all_times.length; i++)
			{
				t = all_times[i];
				if (t > time_max)
					break;
				if (t >= time_min)
				{
					if (st_idx == -1)
						st_idx = i;
				}
			}
			end_idx = i;
			if (st_idx == -1)
				throw (new IOException("No frames found between " + time_min + " - " + time_max));
			n_frames = end_idx - st_idx;
			times = new float[n_frames];
			int j = 0;
			for (i = st_idx; i < end_idx; i++)
				times[j++] = all_times[i];
		}

		@Override
		public byte[] getFrameAt(int idx) throws IOException
		{
			if (debug)
				System.out.println("GetFrameAt " + idx);
			byte[] b_img = null;
			if (mode == BITMAP_IMAGE_8 || mode == BITMAP_IMAGE_16 || mode == BITMAP_IMAGE_32)
			{
				if (buf == null)
					throw (new IOException("Frames not loaded"));
				final ByteArrayInputStream b = new ByteArrayInputStream(buf);
				final DataInputStream d = new DataInputStream(b);
				if (buf == null)
					throw (new IOException("Frames dimension not evaluated"));
				final int img_size = dim.width * dim.height * pixel_size / 8;
				d.skip(header_size + (st_idx + idx) * img_size);
				if (d.available() < img_size)
					return null;
				b_img = new byte[img_size];
				d.readFully(b_img);
				return b_img;
			}
			else
			{
				// we = new WaveformEvent(wave, "Loading frame "+idx+"/"+n_frames);
				// wave.dispatchWaveformEvent(we);
				if (idx == first_frame_idx && buf != null)
					return buf;
				b_img = MdsDataProvider.this.GetFrameAt(in_y, st_idx + idx);
				return b_img;
			}
		}

		@Override
		public Dimension getFrameDimension()
		{ return dim; }

		@Override
		public float[] getFrameTimes()
		{ return times; }

		@Override
		public int getFrameType() throws IOException
		{
			if (mode != -1)
				return mode;
			int i;
			for (i = 0; i < n_frames; i++)
			{
				buf = getFrameAt(i);
				if (buf != null)
					break;
			}
			first_frame_idx = i;
			mode = Frames.DecodeImageType(buf);
			return mode;
		}

		@Override
		public int getNumFrames()
		{ return n_frames; }
	}
	//////////////////////////////////////// GAB JULY 2014
	class SimpleWaveData implements WaveData
	{
		static final int SEGMENTED_YES = 1, SEGMENTED_NO = 2, SEGMENTED_UNKNOWN = 3;
		static final int UNKNOWN = -1;
		String in_x, in_y;
		boolean _jscope_set = false;
		int numDimensions = UNKNOWN;
		int segmentMode = SEGMENTED_UNKNOWN;
		String segmentNodeName;
		int v_idx;
		boolean isXLong = false;
		String title = null;
		String xLabel = null;
		String yLabel = null;
		String defaultNode = null;
		boolean titleEvaluated = false;
		boolean xLabelEvaluated = false;
		boolean yLabelEvaluated = false;
		String wd_experiment;
		long wd_shot;
		AsyncDataSource asynchSource = null;

		private long x2DLong[];

		// Async update management
		Vector<WaveDataListener> waveDataListenersV = new Vector<>();

		public SimpleWaveData(String in_y, String experiment, long shot, String defaultNode)
		{
			this.wd_experiment = experiment;
			this.wd_shot = shot;
			this.defaultNode = defaultNode;
			if (checkForAsynchRequest(in_y))
			{
				this.in_y = "[]";
				this.in_x = "[]";
			}
			else
			{
				this.in_y = in_y;
			}
			v_idx = var_idx;
			var_idx += 2;
			if (segmentMode == SEGMENTED_UNKNOWN)
			{
				final String fixedY = duplicateBackslashes(in_y);
				try
				{
					final String segExpr = "long(MdsMisc->IsSegmented(\"" + fixedY + "\"))";
					final int[] retData = getIntArray(segExpr);
					if (debug)
						System.out.println(segExpr + " " + retData[0]);
					if (retData[0] != 0)
					{
						segmentMode = SEGMENTED_YES;
						segmentNodeName = getStringValue("MdsMisc->GetPathOf:DSC(" + retData[0] + ")");
						if (debug)
							System.out.println("Segmented Node: " + segmentNodeName);
					}
					else
						segmentMode = SEGMENTED_NO;
				}
				catch (final IOException exc)
				{
					error = null;
					segmentMode = SEGMENTED_UNKNOWN;
				}
			}
		}

		public SimpleWaveData(String in_y, String in_x, String experiment, long shot, String defaultNode)
		{
			this.wd_experiment = experiment;
			this.wd_shot = shot;
			this.defaultNode = defaultNode;
			if (checkForAsynchRequest(in_y))
			{
				this.in_y = "[]";
				this.in_x = "[]";
			}
			else
			{
				this.in_y = in_y;
				this.in_x = in_x;
			}
			v_idx = var_idx;
			var_idx += 2;
			if (segmentMode == SEGMENTED_UNKNOWN)
			{
                            final String fixedY = duplicateBackslashes(in_y);
                            try
                            {
				final String segExpr = "long(MdsMisc->IsSegmented(\"" + fixedY + "\"))";
				final int[] retData = getIntArray(segExpr);
				if (retData[0] != 0)
				{
                                    segmentNodeName = getStringValue("MdsMisc->GetPathOf:DSC(" + retData[0] + ")");
                                    if (debug)
					System.out.println("Segmented Node: " + segmentNodeName);
                                    segmentMode = SEGMENTED_YES;
				}
				else
                                    segmentMode = SEGMENTED_NO;
				}
				catch (final Exception exc)
				{
					error = null;
					segmentMode = SEGMENTED_UNKNOWN;
				}
			}
		}

		@Override
		public void addWaveDataListener(WaveDataListener listener)
		{
			waveDataListenersV.addElement(listener);
			if (asynchSource != null)
				asynchSource.addDataListener(listener);
		}

		// Check if the passed Y expression specifies also an asynchronous part
		// (separated by the patern &&&)
		// in case get an implemenation of AsynchDataSource
		boolean checkForAsynchRequest(String expression)
		{
			if (expression.startsWith("ASYNCH::"))
			{
				asynchSource = getAsynchSource();
				if (asynchSource != null)
				{
					asynchSource.startGeneration(expression.substring("ASYNCH::".length()));
				}
				return true;
			}
			return false;
		}

		public String duplicateBackslashes(String inStr)
		{
			final StringBuffer outStr = new StringBuffer();
			for (int i = 0; i < inStr.length(); i++)
			{
				if (inStr.charAt(i) == '\\')
				{
					outStr.append('\\');
				}
				outStr.append(inStr.charAt(i));
			}
			return outStr.toString();
		}

		@Override
		public XYData getData(double xmin, double xmax, int numPoints) throws IOException
		{
			return getData(xmin, xmax, numPoints, false);
		}

		public XYData getData(double xmin, double xmax, int numPoints, boolean isLong) throws IOException
		{
			String xExpr, yExpr;
			XYData res = null;
			if (debug)
				System.out.println("GET DATA " + in_y + "   Start: " + xmin + "   End: " + xmax);
			if (!CheckOpen(this.wd_experiment, this.wd_shot, this.defaultNode))
				return null;
			if (segmentMode == SEGMENTED_UNKNOWN)
			{
                            final String fixedY = duplicateBackslashes(in_y);
                            try
                            {
				final String segExpr = "long(MdsMisc->IsSegmented(\"" + fixedY + "\"))";
				final int[] retData = getIntArray(segExpr);
				if (debug)
                                    System.out.println(segExpr + " " + retData[0]);
				if (retData[0] != 0)
				{
                                    segmentMode = SEGMENTED_YES;
                                    segmentNodeName = getStringValue("MdsMisc->GetPathOf:DSC(" + retData[0] + ")");
                                    if (debug)
					System.out.println("Segmented Node: " + segmentNodeName);
				}
				else
                                    segmentMode = SEGMENTED_NO;
				}
				catch (final Exception exc)
				{
					segmentMode = SEGMENTED_NO;
				}
			}
			if (segmentMode == SEGMENTED_NO) // Store in TDI variable only non segmented data
			{
				yExpr = in_y;
				_jscope_set = false;
				if (in_x == null)
					xExpr = "DIM_OF( " + in_y + " );";
				else
					xExpr = in_x;
			}
			else
			{
				if (in_x == null)
				{
					yExpr = in_y;
					xExpr = "DIM_OF( " + in_y + " );";
				}
				else
				{
					yExpr = in_y;
					xExpr = in_x;
				}
			}
			final Vector<Descriptor> args = new Vector<>();
			args.addElement(new Descriptor(null, yExpr));
			if (in_x == null)
				args.addElement(new Descriptor(null, ""));
			else
				args.addElement(new Descriptor(null, xExpr));
			if (isLong)
			{
				args.addElement(new Descriptor(null, new long[]
				{ (xmin <= -Double.MAX_VALUE) ? Long.MIN_VALUE : (long) xmin }));
				args.addElement(new Descriptor(null, new long[]
				{ (xmax >= Double.MAX_VALUE) ? Long.MAX_VALUE : (long) xmax }));
			}
			else
			{
				args.addElement(new Descriptor(null, new float[]
				{ (float) xmin }));
				args.addElement(new Descriptor(null, new float[]
				{ (float) xmax }));
			}
			args.addElement(new Descriptor(null, new int[]
			{ numPoints }));
			byte[] retData;
			int nSamples;
			try
			{
				// If the requeated number of points is Integer.MAX_VALUE, force the old way of
				// getting data
				if (numPoints == Integer.MAX_VALUE)
					throw new Exception("Use Old Method for getting data");
				if (isLong)
//                      retData = GetByteArray(setTimeContext+" MdsMisc->GetXYSignalLongTimes:DSC", args);
					retData = GetByteArray(" MdsMisc->GetXYSignalLongTimes:DSC", args);
				else
//                      retData = GetByteArray(setTimeContext+" MdsMisc->GetXYSignal:DSC", args);
					retData = GetByteArray(" MdsMisc->GetXYSignal:DSC", args);
				/*
				 * Decode data: Format: -retResolution(float) ----Gabriele Feb 2019 NEW: if
				 * retResolution == 0 then the following int is the number of bytes of the error
				 * message, followied by the error message itself. -number of samples (minumum
				 * between X and Y) -type of X xamples (byte: long(1), double(2) or float(3)) -y
				 * samples -x Samples
				 */
				final ByteArrayInputStream bis = new ByteArrayInputStream(retData);
				final DataInputStream dis = new DataInputStream(bis);
				float fRes;
				double dRes;
				fRes = dis.readFloat();
				// Check if an error was returned
				if (fRes == 0)
				{
					nSamples = dis.readInt();
					final byte[] errorBuf = new byte[nSamples];
					dis.readFully(errorBuf);
					throw new Exception(new String(errorBuf));
				}
				if (debug)
					System.out.println("********************RET RESOLUTION: " + fRes);
				if (fRes >= 1E10)
					dRes = Double.MAX_VALUE;
				else
					dRes = fRes;
				nSamples = dis.readInt();
				if (nSamples <= 0)
				{
					error = "No Samples returned";
					return null;
				}
				final byte type = dis.readByte();
				final float y[] = new float[nSamples];
				for (int i = 0; i < nSamples; i++)
				{
					y[i] = dis.readFloat();
				}
				if (type == 1) // Long X (i.e. absolute times
				{
					final long[] longX = new long[nSamples];
					for (int i = 0; i < nSamples; i++)
						longX[i] = dis.readLong();
					isXLong = true;
					res = new XYData(longX, y, dRes);
				}
				else if (type == 2) // double X
				{
					final double[] x = new double[nSamples];
					for (int i = 0; i < nSamples; i++)
						x[i] = dis.readDouble();
					res = new XYData(x, y, dRes);
				}
				else // float X
				{
					final double[] x = new double[nSamples];
					for (int i = 0; i < nSamples; i++)
						x[i] = dis.readFloat();
					res = new XYData(x, y, dRes);
				}
				// Get title, xLabel and yLabel
				final int titleLen = dis.readInt();
				if (titleLen > 0)
				{
					final byte[] titleBuf = new byte[titleLen];
					dis.readFully(titleBuf);
					title = new String(titleBuf);
				}
				final int xLabelLen = dis.readInt();
				if (xLabelLen > 0)
				{
					final byte[] xLabelBuf = new byte[xLabelLen];
					dis.readFully(xLabelBuf);
					xLabel = new String(xLabelBuf);
				}
				final int yLabelLen = dis.readInt();
				if (yLabelLen > 0)
				{
					final byte[] yLabelBuf = new byte[yLabelLen];
					dis.readFully(yLabelBuf);
					yLabel = new String(yLabelBuf);
				}
				titleEvaluated = xLabelEvaluated = yLabelEvaluated = true;
				if (type == 1)
					isLong = true;
				/*
				 * } catch(Exception exc) { System.out.println("Error Reading data: "+exc);
				 * nSamples = 0; }
				 */ // Got resampled signal, if it is segmented and jScope.refreshPeriod > 0,
					// enqueue a new request
				return res;
			}
			catch (final Exception exc)
			{
				// System.out.println("MdsMisc->GetXYSignal Failed: "+exc); It means that
				// MdsMisc->GetXYSignal() is likely not available on the server
			}
			// If execution arrives here probably MdsMisc->GetXYSignal() is not available on
			// the server, so use the traditional approach
//            float y[] = GetFloatArray("SetTimeContext(*,*,*); ("+yExpr+");");
			final float y[] = GetFloatArray("(" + yExpr + ")");
			final RealArray xReal = GetRealArray("(" + xExpr + ";)");
			if (y == null || xReal == null)
				return null;
			if (xReal.isLong())
			{
				isXLong = true;
				return new XYData(xReal.getLongArray(), y, 1E12);
			}
			else
			{
				isXLong = false;
				return new XYData(xReal.getDoubleArray(), y, 1E12);
			}
		}

		@Override
		public XYData getData(int numPoints) throws IOException
		{
			return getData(-Double.MAX_VALUE, Double.MAX_VALUE, numPoints);
		}

		@Override
		public XYData getData(long xmin, long xmax, int numPoints) throws IOException
		{
			return getData(xmin, xmax, numPoints, true);
		}

		@Override
		public void getDataAsync(double lowerBound, double upperBound, int numPoints)
		{
			if (debug)
				System.out.println("***GET DATA ASYNCH " + lowerBound + "  " + upperBound + "  " + numPoints);
			updateWorker.updateInfo(lowerBound, upperBound, numPoints, waveDataListenersV, this, isXLong);
		}

		@Override
		public int getNumDimension() throws IOException
		{
			if (numDimensions != UNKNOWN)
				return numDimensions;
			String expr;
			if (_jscope_set)
				expr = "shape(_jscope_" + v_idx + ")";
			else
			{
				if (segmentMode == SEGMENTED_YES)
					expr = "shape(GetSegment(" + segmentNodeName + ",0))";
//	            expr = "shape(GetSegment(" + in_y +",0))";
				else
				{
					_jscope_set = true;
					expr = "( _jscope_" + v_idx + " = (" + in_y + ";), shape(_jscope_" + v_idx + "))";
				}
			}
			error = null;
			final int shape[] = getNumDimensions(expr);
			if (error != null || shape == null)
			{
				_jscope_set = false;
				error = null;
				return 1;
			}
			numDimensions = shape.length;
			return shape.length;
		}

		@Override
		public String GetTitle() throws IOException
		{
			String expr;
			if (!titleEvaluated)
			{
				titleEvaluated = true;
				if (_jscope_set)
				{
					expr = "help_of(_jscope_" + v_idx + ")";
					title = getStringValue(expr);
				}
				else
				{
					if (segmentMode == SEGMENTED_YES)
					{
						expr = "help_of(" + in_y + ")";
					}
					else
					{
						_jscope_set = true;
						expr = "( _jscope_" + v_idx + " = (" + in_y + "), help_of(_jscope_" + v_idx + "))";
					}
					title = getStringValue(expr);
				}
			}
			return title;
		}

		public float[] getX_X2D()
		{
			final String in = "DIM_OF( " + in_x + ", 0);";
			try
			{
				return GetFloatArray(in);
			}
			catch (final Exception exc)
			{
				return null;
			}
		}

		public float[] getX_Y2D()
		{
			final String in = "DIM_OF( " + in_x + ", 1);";
			try
			{
				return GetFloatArray(in);
			}
			catch (final Exception exc)
			{
				return null;
			}
		}
		// End

		// Cesare Mar 2015
		public float[] getX_Z()
		{
			try
			{
				return GetFloatArray(in_x);
			}
			catch (final Exception exc)
			{
				return null;
			}
		}

		@Override
		public double[] getX2D()
		{
			final String in = "DIM_OF(" + in_y + ", 0);";
			try
			{
				final RealArray realArray = GetRealArray(in);
				if (realArray.isLong())
				{
					this.isXLong = true;
					x2DLong = realArray.getLongArray();
					return null;
				}
				else
				{
					x2DLong = null;
					return realArray.getDoubleArray();
				}
				// return GetFloatArray(in);
			}
			catch (final Exception exc)
			{
				return null;
			}
		}

		@Override
		public long[] getX2DLong()
		{ return x2DLong; }

		@Override
		public String GetXLabel() throws IOException
		{
			if (!xLabelEvaluated)
			{
				xLabelEvaluated = true;
				if (in_x == null || in_x.length() == 0)
				{
					String expr;
					if (_jscope_set)
					{
						expr = "Units(dim_of(_jscope_" + v_idx + "))";
						xLabel = getStringValue(expr);
					}
					else
					{
						if (segmentMode == SEGMENTED_YES)
						{
//	                    expr = "Units(dim_of(GetSegment(" + in_y + ", 0)))";
							expr = "Units(dim_of(GetSegment(" + segmentNodeName + ", 0)))";
							xLabel = getStringValue(expr);
						}
						else
						{
							_jscope_set = true;
							expr = "( _jscope_" + v_idx + " = (" + in_y + "), Units(dim_of(_jscope_" + v_idx + ")))";
							xLabel = getStringValue(expr);
						}
					}
				}
				else
				{
                                    if (segmentMode == SEGMENTED_YES)
                                    {
					xLabel = ""; //Shall be set afterwards
                                    }
                                    else
                                        xLabel = getStringValue("Units(" + in_x + ")");
				}
			}
			return xLabel;
		}

		@Override
		public float[] getY2D()
		{
			final String in = "DIM_OF( " + in_y + ", 1);";
			try
			{
				return GetFloatArray(in);
			}
			catch (final Exception exc)
			{
				return null;
			}
		}

		@Override
		public String GetYLabel() throws IOException
		{
			String expr;
			if (!yLabelEvaluated)
			{
				yLabelEvaluated = true;
				if (getNumDimension() > 1)
				{
					if (segmentMode == SEGMENTED_YES)
					{
						expr = "Units(dim_of(GetSegment(" + segmentNodeName + ", 1)))";
						yLabel = getStringValue(expr);
					}
					else
					{
						if (_jscope_set)
						{
							expr = "Units(dim_of(_jscope_" + v_idx + ", 1))";
							yLabel = getStringValue(expr);
						}
						else
						{
							_jscope_set = true;
							expr = "( _jscope_" + v_idx + " = (" + in_y + "), Units(dim_of(_jscope_" + v_idx + ", 1)))";
							yLabel = getStringValue(expr);
						}
					}
				}
				return yLabel;
			}
			if (segmentMode == SEGMENTED_YES)
			{
				expr = "Units(dim_of(GetSegment(" + segmentNodeName + ", 0)))";
				yLabel = getStringValue(expr);
			}
			else
			{
				if (_jscope_set)
				{
					expr = "Units(_jscope_" + v_idx + ")";
					yLabel = getStringValue(expr);
				}
				else
				{
					_jscope_set = true;
					expr = "( _jscope_" + v_idx + " = (" + in_y + "), Units(_jscope_" + v_idx + "))";
					yLabel = getStringValue(expr);
				}
			}
			return yLabel;
		}

		@Override
		public float[] getZ()
		{
			try
			{
				return GetFloatArray(in_y);
			}
			catch (final Exception exc)
			{
				return null;
			}
		}

		@Override
		public String GetZLabel() throws IOException
		{
			String expr;
			if (_jscope_set)
				expr = "Units(_jscope_" + v_idx + ")";
			else
			{
				_jscope_set = true;
				expr = "( _jscope_" + v_idx + " = (" + in_y + "), Units(_jscope_" + v_idx + "))";
			}
			final String out = getStringValue(expr);
			if (out == null)
				_jscope_set = false;
			return out;
		}

		// public double[] getXLimits(){System.out.println("BADABUM!!"); return null;}
		// public long []getXLong(){System.out.println("BADABUM!!"); return null;}
		@Override
		public boolean isXLong()
		{ return isXLong; }

		@Override
		public void removeWaveDataListener(WaveDataListener listener)
		{
			waveDataListenersV.remove(listener);
		}

		@Override
		public boolean supportsStreaming()
		{
			return segmentMode == SEGMENTED_YES;
		}
	} // END Inner Class SimpleWaveData
	// Inner class UpdateWorker handler asynchronous requests for getting (portions
	// of) data
	class UpdateWorker extends Thread
	{
		class UpdateDescriptor
		{
			double updateLowerBound;
			double updateUpperBound;
			int updatePoints;
			Vector<WaveDataListener> waveDataListenersV;
			SimpleWaveData simpleWaveData;
			boolean isXLong;
			long updateTime;

			UpdateDescriptor(double updateLowerBound, double updateUpperBound, int updatePoints,
					Vector<WaveDataListener> waveDataListenersV, SimpleWaveData simpleWaveData, boolean isXLong,
					long updateTime)
			{
				this.updateLowerBound = updateLowerBound;
				this.updateUpperBound = updateUpperBound;
				this.updatePoints = updatePoints;
				this.waveDataListenersV = waveDataListenersV;
				this.simpleWaveData = simpleWaveData;
				this.isXLong = isXLong;
				this.updateTime = updateTime;
			}
		}

		boolean enabled = true;
		Vector<UpdateDescriptor> requestsV = new Vector<>();

		boolean stopWorker = false;

		synchronized void enableAsyncUpdate(boolean enabled)
		{
			this.enabled = enabled;
			if (enabled)
				notify();
		}

		synchronized void intUpdateInfo(double updateLowerBound, double updateUpperBound, int updatePoints,
				Vector<WaveDataListener> waveDataListenersV, SimpleWaveData simpleWaveData, boolean isXLong,
				long updateTime)
		{
			requestsV.add(new UpdateDescriptor(updateLowerBound, updateUpperBound, updatePoints, waveDataListenersV,
					simpleWaveData, isXLong, updateTime));
			notify();
		}

		@Override
		public void run()
		{
			this.setName("UpdateWorker");
			while (true)
			{
				synchronized (this)
				{
					try
					{
						wait();
						if (stopWorker)
							return;
					}
					catch (final InterruptedException exc)
					{}
				}
				if (!enabled)
					continue;
				while (requestsV.size() > 0)
				{
					if (!enabled)
						break;
					// Take most recent request
					final UpdateDescriptor currUpdate = requestsV.elementAt(requestsV.size() - 1);
					try
					{
						requestsV.removeElementAt(requestsV.size() - 1);
						final XYData currData = currUpdate.simpleWaveData.getData(currUpdate.updateLowerBound,
								currUpdate.updateUpperBound, currUpdate.updatePoints, currUpdate.isXLong);
						if (debug)
							System.out.println("UPDATE Lower Bound: " + currUpdate.updateLowerBound + "   Upper bound: "
									+ currUpdate.updateUpperBound + "  Resolution: "
									+ ((currData == null) ? "None" : ("" + currData.resolution)));
						if (currData == null || currData.nSamples == 0)
							continue;
						for (int j = 0; j < currUpdate.waveDataListenersV.size(); j++)
						{
							if (currUpdate.isXLong)
								currUpdate.waveDataListenersV.elementAt(j).dataRegionUpdated(currData.xLong, currData.y,
										currData.resolution);
							else
								currUpdate.waveDataListenersV.elementAt(j).dataRegionUpdated(currData.x, currData.y,
										currData.resolution);
						}
					}
					catch (final Exception exc)
					{
						final Date d = new Date();
						System.out.println(d + " Error in asynchUpdate: " + exc);
					}
				}
			}
		}

		synchronized void stopUpdateWorker()
		{
			stopWorker = true;
			notify();
		}

		void updateInfo(double updateLowerBound, double updateUpperBound, int updatePoints,
				Vector<WaveDataListener> waveDataListenersV, SimpleWaveData simpleWaveData, boolean isXLong)
		{
			// intUpdateInfo(updateLowerBound, updateUpperBound, updatePoints,
			// waveDataListenersV, simpleWaveData, isXLong,
			// Calendar.getInstance().getTimeInMillis());
			intUpdateInfo(updateLowerBound, updateUpperBound, updatePoints, waveDataListenersV, simpleWaveData, isXLong,
					-1);
		}
	} // End Inner class UpdateWorker
	static final long RESAMPLE_TRESHOLD = 1000000000;
	static final int MAX_PIXELS = 20000;
	static boolean debug = false;
	protected String provider;
	protected String experiment;
	private String prev_default_node = null;
	private String default_node = null;
	private String environment_vars;
	protected long shot;
	protected boolean open, connected;
	protected final MdsConnection mds;
	protected String error;

	protected boolean use_compression = false;

	protected int var_idx = 0;

	private boolean is_tunneling = false;

	private String tunnel_provider = "127.0.0.1:8000";

	private SshTunneling ssh_tunneling = null;

	UpdateWorker updateWorker;

	public MdsDataProvider()
	{
		experiment = null;
		shot = 0;
		open = connected = false;
		mds = getConnection();
		error = null;
		// updateWorker = new UpdateWorker();
		// updateWorker.start();
	}

	public MdsDataProvider(String provider)
	{
		setProvider(provider);
		experiment = null;
		shot = 0;
		open = connected = false;
//        mds = new MdsConnection(this.provider);
		mds = getConnection(this.provider);
		error = null;
		// updateWorker = new UpdateWorker();
		// updateWorker.start();
	}

	public MdsDataProvider(String exp, int s)
	{
		experiment = exp;
		shot = 0;
		open = connected = false;
		// mds = new MdsConnection();
		mds = getConnection();
		error = null;
		// updateWorker = new UpdateWorker();
		// updateWorker.start();
	}

	@Override
	public synchronized void addConnectionListener(ConnectionListener l)
	{
		if (mds != null)
			mds.addConnectionListener(l);
	}

	@Override
	public synchronized void addUpdateEventListener(UpdateEventListener l, String event_name) throws IOException
	{
		if (event_name == null || event_name.trim().length() == 0)
			return;
		CheckConnection();
		mds.MdsSetEvent(l, event_name);
	}

	protected synchronized void CheckConnection() throws IOException
	{
		if (!connected)
		{
			if (mds.ConnectToMds(use_compression) == 0)
			{
				if (mds.error != null)
					throw new IOException(mds.error);
				else
					throw new IOException("Could not get IO for " + provider);
			}
			else
			{
				connected = true;
				updateWorker = new UpdateWorker();
				updateWorker.start();
			}
		}
	}

	protected synchronized boolean CheckOpen() throws IOException
	{
		return CheckOpen(this.experiment, this.shot, null);
	}

	protected synchronized boolean CheckOpen(String experiment, long shot, String defaultNode) throws IOException
	{
		int status;
		if (!connected)
		{
			status = mds.ConnectToMds(use_compression);
			if (status == 0)
			{
				if (mds.error != null)
					throw new IOException("Cannot connect to data server : " + mds.error);
				else
					error = "Cannot connect to data server";
				return false;
			}
			connected = true;
			updateWorker = new UpdateWorker();
			updateWorker.start();
		}
		if (!open && experiment != null || this.shot != shot
				|| experiment != null && !experiment.equalsIgnoreCase(this.experiment))
		{
			// System.out.println("\n-->\nOpen tree "+experiment+ " shot "+ shot
			// +"\n<--\n");
			final Descriptor descr = mds.MdsValue("JavaOpen(\"" + experiment + "\"," + shot + ")");
			if (descr.dtype != Descriptor.DTYPE_CSTRING && descr.dtype == Descriptor.DTYPE_LONG
					&& descr.int_data != null && descr.int_data.length > 0 && (descr.int_data[0] % 2 == 1))
			{
				open = true;
				this.shot = shot;
				this.experiment = experiment;
				final Descriptor descr1 = mds.MdsValue("setenv(\'MDSPLUS_DEFAULT_RESAMPLE_MODE=MinMax\')");
				switch (descr1.dtype)
				{
				case Descriptor.DTYPE_CSTRING:
					if ((descr1.status & 1) == 0)
					{
						error = descr1.error;
						return false;
					}
				}
				if (environment_vars != null && environment_vars.length() > 0)
				{
					this.SetEnvironmentSpecific(environment_vars);
					if (error != null)
					{
						error = "Public variable evaluation error " + experiment + " shot " + shot + " : " + error;
						return false;
					}
				}
			}
			else
			{
				if (mds.error != null)
					error = "Cannot open experiment " + experiment + " shot " + shot + " : " + mds.error;
				else
					error = "Cannot open experiment " + experiment + " shot " + shot;
				return false;
			}
		}
		if (open)
		{
			if (defaultNode != null && (prev_default_node == null || !defaultNode.equals(prev_default_node)))
			{
				Descriptor descr;
				if (default_node.trim().charAt(0) == '\\')
					descr = mds.MdsValue("TreeSetDefault(\"\\" + defaultNode + "\")");
				else
					descr = mds.MdsValue("TreeSetDefault(\"\\\\" + defaultNode + "\")");
				prev_default_node = defaultNode;
				if ((descr.int_data[0] & 1) == 0)
				{
					mds.MdsValue("TreeSetDefault(\"\\\\::TOP\")");
					prev_default_node = null;
				}
			}
			else if (defaultNode == null && prev_default_node != null)
			{
				mds.MdsValue("TreeSetDefault(\"\\\\::TOP\")");
				prev_default_node = null;
			}
		}
		return true;
	}

	@Override
	public synchronized void close()
	{
		if (is_tunneling && ssh_tunneling != null)
			ssh_tunneling.close();
		if (connected)
		{
			connected = false;
			mds.DisconnectFromMds();
			final ConnectionEvent ce = new ConnectionEvent(this, ConnectionEvent.LOST_CONNECTION,
					"Lost connection from : " + provider);
			mds.dispatchConnectionEvent(ce);
		}
		if (updateWorker != null && updateWorker.isAlive())
		{
			updateWorker.stopUpdateWorker();
		}
	}

	protected void dispatchConnectionEvent(ConnectionEvent e)
	{
		if (mds != null)
			mds.dispatchConnectionEvent(e);
	}

	public void enableAsyncUpdate(boolean enable)
	{
		updateWorker.enableAsyncUpdate(enable);
	}

	@Override
	protected void finalize()
	{
		if (open)
			mds.MdsValue("JavaClose(\"" + experiment + "\"," + shot + ")");
		if (connected)
			mds.DisconnectFromMds();
	}

	public synchronized byte[] GetAllFrames(String in_frame) throws IOException
	{
		float time[];
		int shape[];
		int pixel_size = 8;
		int num_time = 0;
		if (!CheckOpen())
			return null;
		String in;
		in = "DIM_OF( _jScope_img = (" + in_frame + "), 2)";
		time = GetFloatArray(in);
		if (time == null || ((time.length >= 2) && (time[1] == 1.0)))
		{
			// in = "DIM_OF(" + in_frame + ")";
			in = "DIM_OF( _jScope_img )";
			time = GetFloatArray(in);
			if (time == null)
				return null;
		}
//      in = "eshape(data(" + in_frame + "))";
		in = "eshape(data( _jScope_img ))";
		shape = getIntArray(in);
		if (shape == null)
			return null;
		// in = in_frame;
		in = "_jScope_img";
		final byte[] img_buf = GetByteArray(in);
		if (img_buf == null)
			return null;
		if (shape.length == 3)
		{
			num_time = shape[2];
			pixel_size = img_buf.length / (shape[0] * shape[1] * shape[2]) * 8;
		}
		else
		{
			if (shape.length == 2)
			{
				num_time = 1;
				pixel_size = img_buf.length / (shape[0] * shape[1]) * 8;
			}
			else if (shape.length == 1)
			{
				throw (new IOException("The evaluated signal is not an image"));
			}
		}
		final ByteArrayOutputStream b = new ByteArrayOutputStream();
		try (DataOutputStream d = new DataOutputStream(b))
		{
			d.writeInt(pixel_size);
			d.writeInt(shape[0]);
			d.writeInt(shape[1]);
			d.writeInt(num_time);
			for (int i = 0; i < num_time; i++)
				d.writeFloat(time[i]);
			d.write(img_buf);
			return b.toByteArray();
		}
	}

	// To be overridden by any DataProvider implementation with added dynamic
	// generation
	public AsyncDataSource getAsynchSource()
	{ return null; }

	public synchronized byte[] GetByteArray(String in) throws IOException
	{
		return GetByteArray(in, null);
	}

	@SuppressWarnings("deprecation")
	public synchronized byte[] GetByteArray(String in, Vector<Descriptor> args) throws IOException
	{
		if (debug)
			System.out.println("GetByteArray " + in);
		byte out_byte[] = null;
		final ByteArrayOutputStream dosb = new ByteArrayOutputStream();
		final DataOutputStream dos = new DataOutputStream(dosb);
		if (!CheckOpen())
			return null;
		Descriptor desc;
		if (args == null)
			desc = mds.MdsValue(in);
		else
			desc = mds.MdsValue(in, args);
		switch (desc.dtype)
		{
		case Descriptor.DTYPE_FLOAT:
			for (final float element : desc.float_data)
				dos.writeFloat(element);
			out_byte = dosb.toByteArray();
			return out_byte;
		case Descriptor.DTYPE_USHORT:
		case Descriptor.DTYPE_SHORT: // bdb hacked this to try to make profile dialog read true data values, not
										// normalised
			for (final short element : desc.short_data)
				dos.writeShort(element);
			out_byte = dosb.toByteArray();
			return out_byte;
		case Descriptor.DTYPE_LONG:
			for (final int element : desc.int_data)
				dos.writeInt(element);
			out_byte = dosb.toByteArray();
			return out_byte;
		case Descriptor.DTYPE_UBYTE:
		case Descriptor.DTYPE_BYTE:
			return desc.byte_data;
		case Descriptor.DTYPE_CSTRING:
			if ((desc.status & 1) == 0)
			{
				error = desc.error;
				throw new IOException(error);
			}
		}
		throw new IOException(error);
	}

	protected MdsConnection getConnection()
	{ return new MdsConnection(); }

	protected MdsConnection getConnection(String arg)
	{
		return new MdsConnection(arg);
	}

	public double[] GetDoubleArray(String in) throws IOException
	{
		if (debug)
			System.out.println("GetDoubleArray " + in);
		final RealArray realArray = GetRealArray(in);
		if (realArray == null)
			return null;
		return realArray.getDoubleArray();
	}

	@Override
	public synchronized String getError()
	{ return error; }

	protected String GetExperimentName(String in_frame)
	{
		String exp;
		if (experiment == null)
		{
			if (in_frame.indexOf(".") == -1)
				exp = in_frame;
			else
				exp = in_frame.substring(0, in_frame.indexOf("."));
		}
		else
			exp = experiment;
		return exp;
	}

	@Override
	public synchronized double getFloat(String in, int row, int col, int index) throws IOException
	{
		error = null;
		if (debug)
			System.out.println("GetFloat " + in);
		// First check Whether this is a date
		try
		{
			final Calendar cal = Calendar.getInstance();
			// cal.setTimeZone(TimeZone.getTimeZone("GMT+00"));
			final DateFormat df = new SimpleDateFormat("d-MMM-yyyy HH:mm Z");
//            DateFormat df = new SimpleDateFormat("d-MMM-yyyy HH:mm");
			final Date date = df.parse(in + " GMT");
			// Date date = df.parse(in);
			cal.setTime(date);
			final long javaTime = cal.getTime().getTime();
			return javaTime;
		}
		catch (final Exception exc)
		{
			try
			{
				return GetNow(in);
			}
			catch (final Exception exc1)
			{}
		} // If exception occurs this is not a date, try NOW condtruction
		if (NotYetNumber(in))
		{
			if (!CheckOpen())
				return 0;
			Descriptor desc;
			if (row != -1)
				desc = mds.MdsValue(
						"( _ROW = " + row + "; _COLUMN = " + col + "; _INDEX = " + index + "; " + in + " ; )");
			else
				desc = mds.MdsValue(in);
			if (desc.error != null)
				error = desc.error;
			switch (desc.dtype)
			{
			case Descriptor.DTYPE_DOUBLE:
				return desc.double_data[0];
			case Descriptor.DTYPE_FLOAT:
				return desc.float_data[0];
			case Descriptor.DTYPE_LONG:
				return desc.int_data[0];
			case Descriptor.DTYPE_BYTE:
				return desc.byte_data[0];
			case Descriptor.DTYPE_UBYTE:
				short currShort = 0;
				currShort |= (desc.byte_data[0] & 0x00FF);
				return currShort;
			case Descriptor.DTYPE_CSTRING:
				if ((desc.status & 1) == 0)
				{
					error = desc.error;
					throw (new IOException(error));
				}
				return 0;
			}
		}
		else
			return Float.parseFloat(in);
		return 0;
	}

	public float[] GetFloatArray(String in) throws IOException
	{
		if (debug)
			System.out.println("GetFloatArray " + in);
		final RealArray realArray = GetRealArray(in);
		if (realArray == null)
			return null;
		return realArray.getFloatArray();
	}

	public byte[] GetFrameAt(String in_frame, int frame_idx) throws IOException
	{
		if (debug)
			System.out.println("GetFrameAt " + in_frame + "  " + frame_idx);
		final String exp = GetExperimentName(in_frame);
		final String in = "JavaGetFrameAt(\"" + exp + "\",\" " + in_frame + "\"," + shot + ", " + frame_idx + " )";
		// if(!CheckOpen())
		// return null;
		return GetByteArray(in);
	}

	@Override
	public FrameData getFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
	{
		int[] numSegments = null;
		try
		{
			numSegments = getIntArray("GetNumSegments(" + in_y + ")");
		}
		catch (final Exception exc)
		{
			error = null;
		}
		if (numSegments != null && numSegments[0] > 0)
			return new SegmentedFrameData(in_y, in_x, time_min, time_max, numSegments[0]);
		else
		{
			try
			{
				return (new SimpleFrameData(in_y, in_x, time_min, time_max));
			}
			catch (final Exception exc)
			{
				return null;
			}
		}
	}

	public synchronized float[] GetFrameTimes(String in_frame)
	{
		final String exp = GetExperimentName(in_frame);
		final String in = "JavaGetFrameTimes(\"" + exp + "\",\"" + in_frame + "\"," + shot + " )";
		// if(!CheckOpen())
		// return null;
		final Descriptor desc = mds.MdsValue(in);
		switch (desc.dtype)
		{
		case Descriptor.DTYPE_FLOAT:
			return desc.float_data;
		case Descriptor.DTYPE_LONG:
			final float[] out_data = new float[desc.int_data.length];
			for (int i = 0; i < desc.int_data.length; i++)
				out_data[i] = desc.int_data[i];
			return out_data;
		case Descriptor.DTYPE_BYTE:
			error = "Cannot convert byte array to float array";
			return null;
		case Descriptor.DTYPE_CSTRING:
			if ((desc.status & 1) == 0)
				error = desc.error;
			return null;
		}
		return null;
	}

	public int[] getIntArray(String in) throws IOException
	{ // public because its used by MdsAccess
		if (debug)
			System.out.println("GetIntArray " + in);
		if (in == null)
			return null;
		if (!CheckOpen())
			throw new IOException("Tree not open");
		return getIntegerArray(in);
	}

	private synchronized int[] getIntegerArray(String in) throws IOException
	{
		if (debug)
			System.out.println("GetIntegerArray " + in);
		int out_data[];
		final Descriptor desc = mds.MdsValue(in);
		switch (desc.dtype)
		{
		case Descriptor.DTYPE_LONG:
			return desc.int_data;
		case Descriptor.DTYPE_FLOAT:
			out_data = new int[desc.float_data.length];
			for (int i = 0; i < desc.float_data.length; i++)
				out_data[i] = (int) (desc.float_data[i] + 0.5);
			return out_data;
		case Descriptor.DTYPE_BYTE:
		case Descriptor.DTYPE_UBYTE:
			out_data = new int[desc.byte_data.length];
			for (int i = 0; i < desc.byte_data.length; i++)
				out_data[i] = (int) (desc.byte_data[i] + 0.5);
			return out_data;
		case Descriptor.DTYPE_CSTRING:
			if ((desc.status & 1) == 0)
				error = desc.error;
			throw new IOException(error);
		default:
			error = "Data type code : " + desc.dtype + " not yet supported ";
		}
		throw new IOException(error);
	}

	double GetNow(String in) throws Exception
	{
		boolean isPlus = true;
		int hours = 0, minutes = 0, seconds = 0;
		String currStr = in.trim().toUpperCase();
		if (!currStr.startsWith("NOW"))
			throw new Exception();
		currStr = currStr.substring(3).trim();
		if (currStr.length() > 0) // Not only NOW
		{
			if (currStr.startsWith("+"))
				isPlus = true;
			else if (currStr.startsWith("-"))
				isPlus = false;
			else
				throw new Exception();
			currStr = currStr.substring(1).trim();
			final StringTokenizer st = new StringTokenizer(currStr, ":", true);
			String currTok = st.nextToken();
			if (currTok.equals(":"))
				hours = 0;
			else
			{
				hours = Integer.parseInt(currTok);
				currTok = st.nextToken();
			}
			if (!currTok.equals(":"))
				throw new Exception();
			currTok = st.nextToken();
			if (currTok.equals(":"))
				minutes = 0;
			else
			{
				minutes = Integer.parseInt(currTok);
				currTok = st.nextToken();
			}
			if (!currTok.equals(":"))
				throw new Exception();
			if (st.hasMoreTokens())
			{
				seconds = Integer.parseInt(st.nextToken());
			}
		}
		if (!isPlus)
		{
			hours = -hours;
			minutes = -minutes;
			seconds = -seconds;
		}
		final Calendar cal = Calendar.getInstance();
		// cal.setTimeZone(TimeZone.getTimeZone("GMT+00"));
		cal.setTime(new Date());
		cal.add(Calendar.HOUR, hours);
		cal.add(Calendar.MINUTE, minutes);
		cal.add(Calendar.SECOND, seconds);
		final long javaTime = cal.getTime().getTime();
		return javaTime;
	}

	protected int[] getNumDimensions(String in_y) throws IOException
	{
		final int[] fullDims = getIntArray(in_y);
		if (fullDims == null)
			return null;
		if (fullDims.length == 1)
			return fullDims;
		// count dimensions == 1
		int numDimensions = 0;
		for (final int fullDim : fullDims)
		{
			if (fullDim != 1)
				numDimensions++;
		}
		final int[] retDims = new int[numDimensions];
		int j = 0;
		for (final int fullDim : fullDims)
		{
			if (fullDim != 1)
				retDims[j++] = fullDim;
		}
		return retDims;
	}

	protected String getProvider()
	{ return provider; }

	public synchronized RealArray GetRealArray(String in) throws IOException
	{
		if (debug)
			System.out.println("GetRealArray " + in);
		RealArray out;
		final ConnectionEvent e = new ConnectionEvent(this, 1, 0);
		dispatchConnectionEvent(e);
		if (!CheckOpen())
			return null;
		final Descriptor desc = mds.MdsValue(in);
		out = null;
		switch (desc.dtype)
		{
		case Descriptor.DTYPE_FLOAT:
			out = new RealArray(desc.float_data);
			break;
		case Descriptor.DTYPE_DOUBLE:
			out = new RealArray(desc.double_data);
			break;
		case Descriptor.DTYPE_LONG:
		{
			final float[] outF = new float[desc.int_data.length];
			for (int i = 0; i < desc.int_data.length; i++)
				outF[i] = desc.int_data[i];
			out = new RealArray(outF);
		}
		case Descriptor.DTYPE_ULONG:
		{
			final float[] outF = new float[desc.int_data.length];
			for (int i = 0; i < desc.int_data.length; i++)
			{
				long currLong = 0;
				currLong |= ((long) desc.int_data[i] & 0x00000000FFFFFFFF);
				outF[i] = currLong;
			}
			out = new RealArray(outF);
		}
			break;
		case Descriptor.DTYPE_USHORT:
		{
			final float[] outF = new float[desc.int_data.length];
			for (int i = 0; i < desc.int_data.length; i++)
			{
				int currInt = 0;
				currInt |= (desc.short_data[i] & 0x0000FFFF);
				outF[i] = currInt;
			}
			out = new RealArray(outF);
		}
			break;
		case Descriptor.DTYPE_SHORT:
		{
			final float[] outF = new float[desc.short_data.length];
			for (int i = 0; i < desc.short_data.length; i++)
				outF[i] = desc.short_data[i];
			out = new RealArray(outF);
		}
			break;
		case Descriptor.DTYPE_BYTE:
		{
			final float[] outF = new float[desc.byte_data.length];
			for (int i = 0; i < desc.byte_data.length; i++)
				outF[i] = desc.byte_data[i];
			out = new RealArray(outF);
		}
			break;
		case Descriptor.DTYPE_UBYTE:
		{
			final float[] outF = new float[desc.byte_data.length];
			for (int i = 0; i < desc.byte_data.length; i++)
			{
				short currShort = 0;
				currShort |= (desc.byte_data[i] & 0x00FF);
				outF[i] = currShort;
			}
			out = new RealArray(outF);
		}
			break;
		case Descriptor.DTYPE_ULONGLONG:
		case Descriptor.DTYPE_LONGLONG:
		{
			out = new RealArray(desc.long_data);
		}
			break;
		case Descriptor.DTYPE_CSTRING:
			if ((desc.status & 1) == 0)
			{
				error = desc.error;
				throw new IOException(error);
			}
		default:
			error = "Data type code : " + desc.dtype + " not yet supported ";
		}
		return out;
	}

	@Override
	public long[] getShots(String in, String experiment) throws IOException
	{
		if (debug)
			System.out.println("GetShots " + in + "  " + experiment);
		// To shot evaluation don't execute check
		// if a pulse file is open
		CheckConnection();
		synchronized (this)
		{
			long out_data[];
			final Descriptor desc = mds.MdsValue(in);
			switch (desc.dtype)
			{
			case Descriptor.DTYPE_ULONGLONG:
			case Descriptor.DTYPE_LONGLONG:
				return desc.long_data;
			case Descriptor.DTYPE_LONG:
				out_data = new long[desc.int_data.length];
				for (int i = 0; i < desc.int_data.length; i++)
				{
					out_data[i] = (desc.int_data[i]);
				}
				return out_data;
			case Descriptor.DTYPE_FLOAT:
				out_data = new long[desc.float_data.length];
				for (int i = 0; i < desc.float_data.length; i++)
					out_data[i] = (long) (desc.float_data[i] + 0.5);
				return out_data;
			case Descriptor.DTYPE_BYTE:
			case Descriptor.DTYPE_UBYTE:
				out_data = new long[desc.byte_data.length];
				for (int i = 0; i < desc.byte_data.length; i++)
					out_data[i] = (long) (desc.byte_data[i] + 0.5);
				return out_data;
			case Descriptor.DTYPE_CSTRING:
				if ((desc.status & 1) == 0)
					error = desc.error;
				throw new IOException(error);
			default:
				error = "Data type code : " + desc.dtype + " not yet supported ";
			}
			throw new IOException(error);
		}
	}

	@Override
	public synchronized String getString(String _in, int row, int col, int index) throws IOException
	{
		if (_in == null)
			return null;
		String in;
		if (row != -1)
			in = "( _ROW = " + row + "; _COLUMN = " + col + "; _INDEX = " + index + "; " + _in + " ; )";
		else
			in = _in;
		error = null;
		if (NotYetString(in))
		{
			if (!CheckOpen())
				return null;
			final Descriptor desc = mds.MdsValue(in);
			switch (desc.dtype)
			{
			case Descriptor.DTYPE_BYTE:
			case Descriptor.DTYPE_UBYTE:
				return new String(desc.byte_data);
			case Descriptor.DTYPE_FLOAT:
				error = "Cannot convert a float to string";
				throw new IOException(error);
			case Descriptor.DTYPE_CSTRING:
				if ((desc.status & 1) == 1)
					return desc.strdata;
				else
					return (error = desc.error);
			}
			if (desc.error == null)
				return "Undefined error";
			return (error = desc.error);
		}
		else
			return new String(in.getBytes(), 1, in.length() - 2);
	}

	public String getStringValue(String expr) throws IOException
	{ // public as used by MdsAccess
		if (debug)
			System.out.println("getStringValue " + expr);
		String out = getString(expr, -1, -1, -1);
		if (out == null || out.length() == 0 || error != null)
		{
			error = null;
			return null;
		}
		if (out.indexOf(0) > 0)
			out = out.substring(0, out.indexOf(0));
		return out;
	}

	@Override
	public WaveData getWaveData(String in, int row, int col, int index)
	{
		return new SimpleWaveData("( _ROW = " + row + "; _COLUMN = " + col + "; _INDEX = " + index + "; " + in + " ; )",
				experiment, shot, default_node);
	}

	@Override
	public WaveData getWaveData(String in_y, String in_x, int col, int row, int index)
	{
		return new SimpleWaveData(
				"( _ROW = " + row + "; _COLUMN = " + col + "; _INDEX = " + index + "; " + in_y + " ; )", in_x,
				experiment, shot, default_node);
	}

	@Override
	public int inquireCredentials(JFrame f, DataServerItem server_item)
	{
		mds.setUser(server_item.getUser());
		is_tunneling = false;
		final String tunnelPort = server_item.getTunnelPort();
		if (tunnelPort != null && tunnelPort.length() != 0)
		{
			final StringTokenizer st = new StringTokenizer(server_item.getArgument(), ":");
			String ip;
			ip = st.nextToken();
			final int rport;
			if (st.hasMoreTokens())
				rport = Integer.valueOf(st.nextToken());
			else
				rport = MdsConnection.DEFAULT_PORT;
			is_tunneling = true;
			try
			{
				final int lport = Integer.valueOf(server_item.getTunnelPort());
				ssh_tunneling = new SshTunneling(server_item.getUser(), ip, lport, rport);
				tunnel_provider = "127.0.0.1:" + server_item.getTunnelPort();
			}
			catch (final Throwable exc)
			{
				if (exc instanceof NoClassDefFoundError)
					JOptionPane.showMessageDialog(f,
							"The MindTerm.jar library is required for ssh tunneling.You can download it from \nhttp://www.appgate.com/mindterm/download.php\n"
									+ exc,
							"alert", JOptionPane.ERROR_MESSAGE);
				return DataProvider.LOGIN_ERROR;
			}
		}
		return DataProvider.LOGIN_OK;
	}

	@Override
	public boolean isBusy()
	{ return this.mds != null && this.mds.isBusy(); }

	protected boolean NotYetNumber(String in)
	{
		try
		{
			Float.parseFloat(in);
			return false;
		}
		catch (final NumberFormatException e)
		{
			return true;
		}
	}

	protected boolean NotYetString(String in)
	{
		int i;
		if (in.charAt(0) == '\"')
		{
			for (i = 1; i < in.length()
					&& (in.charAt(i) != '\"' || (i > 0 && in.charAt(i) == '\"' && in.charAt(i - 1) == '\\')); i++);
			if (i == (in.length() - 1))
				return false;
		}
		return true;
	}

	@Override
	public synchronized void removeConnectionListener(ConnectionListener l)
	{
		if (mds != null)
			mds.removeConnectionListener(l);
	}

	@Override
	public synchronized void removeUpdateEventListener(UpdateEventListener l, String event_name) throws IOException
	{
		if (event_name == null || event_name.trim().length() == 0)
			return;
		CheckConnection();
		mds.MdsRemoveEvent(l, event_name);
	}

	protected void resetPrevious()
	{}// Used by subclass MdsDataProviderStream to close previous connections

	@Override
	public void setArgument(String arg) throws IOException
	{
		setProvider(arg);
		mds.setProvider(provider);
	}

	public void SetCompression(boolean state)
	{
		if (connected)
			close();
		use_compression = state;
	}

	@Override
	public synchronized void setEnvironment(String in) throws IOException
	{
		if (in == null || in.length() == 0)
			return;
		final Properties pr = new Properties();
		pr.load(new ByteArrayInputStream(in.getBytes()));
		String def_node = pr.getProperty("__default_node");
		if (def_node != null)
		{
			def_node = def_node.trim();
			if (!(default_node != null && def_node.equals(default_node))
					|| (def_node.length() == 0 && default_node != null))
			{
				default_node = (def_node.length() == 0) ? null : def_node;
			}
			return;
		}
		if (in.indexOf("pulseSetVer") >= 0)
		{
			open = false;
		}
		if (environment_vars == null || !environment_vars.equalsIgnoreCase(in))
		{
			open = false;
			environment_vars = in;
		}
	}

	void SetEnvironmentSpecific(String in)
	{
		final Descriptor desc = mds.MdsValue(in);
		switch (desc.dtype)
		{
		case Descriptor.DTYPE_CSTRING:
			if ((desc.status & 1) == 0)
				error = desc.error;
		}
	}

	private void setProvider(String arg)
	{
		if (is_tunneling)
			provider = tunnel_provider;
		else
			provider = arg;
	}

	public boolean SupportsCompression()
	{
		return true;
	}

	@Override
	public boolean supportsTunneling()
	{
		return true;
	}

	@Override
	public synchronized void update(String experiment, long shot)
	{
		Update(experiment, shot, false);
	}

	public synchronized void Update(String experiment, long shot, boolean resetExperiment)
	{
		this.error = null;
		this.var_idx = 0;
		if (resetExperiment)
		{
			this.experiment = null;
		}
		if ((shot != this.shot) || (shot == 0L) || (this.experiment == null) || (this.experiment.length() == 0)
				|| (!this.experiment.equalsIgnoreCase(experiment)))
		{
			this.experiment = ((experiment != null) && (experiment.trim().length() > 0) ? experiment : null);
			this.shot = shot;
			this.open = false;
			resetPrevious();
		}
	}
}

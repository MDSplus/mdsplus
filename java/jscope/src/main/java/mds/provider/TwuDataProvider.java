package mds.provider;

import java.awt.Dimension;
// -------------------------------------------------------------------------------------------------
// TwuDataProvider
// An implementation of "DataProvider" for signals from a TEC Web-Umbrella (TWU) server.
//
// The first versions of this class (cvs revisions 1.x) were designed and written
// by Gabriele Manduchi and with some minor hacks by Jon Krom.
// Marco van de Giessen <A.P.M.vandeGiessen@phys.uu.nl> did some major surgery on
// this class (starting revision line 2.x) mainly in order to ensure that zooming
// works in more situations. (See also the cvs log.)
//
// Most of this revision 2.x work has now, from 2.21 onwards, been moved into separate classes.
// No new major revision number was started; has little use in CVS.
//
//
// -------------------------------------------------------------------------------------------------
import java.io.*;
import java.net.URL;
import java.net.URLConnection;
import java.util.*;

import javax.swing.JFrame;

import mds.connection.*;
import mds.wave.*;

public class TwuDataProvider implements DataProvider
{
	static private class TwuFetchOptions
	{
		private int start = 0;
		private int step = 1;
		private int total = -1;

		public TwuFetchOptions()
		{}

		public TwuFetchOptions(int sta, int ste, int tot)
		{
			start = sta;
			step = ste;
			total = tot;
		}

		public void clip(final int twupLengthTotal)
		{
			final int length = twupLengthTotal;
			if ((length <= 0) || (length <= start))
			{
				start = 0;
				step = 1;
				total = 0;
				return;
			}
			if (start < 0)
				start = 0;
			if (step < 1)
				step = 1;
			if (total < 0)
				total = length;
			final int requestedEnd = start + (total - 1) * step;
			int overshoot = requestedEnd - (length - 1);
			if (overshoot > 0)
			{
				overshoot %= step;
				if (overshoot > 0)
					overshoot -= step;
			}
			final int realEnd = (length - 1) + overshoot;
			// got a valid range now :
			total = (realEnd - start) / step + 1;
		}

		public boolean equalsForBulkData(TwuFetchOptions opt)
		{
			return (start == opt.start && step == opt.step && total == opt.total);
		}

		public int getStart()
		{ return start; }

		public int getStep()
		{ return step; }

		public int getTotal()
		{ return total; }

		@Override
		public String toString()
		{
			return "TWUFetchOptions(" + start + ", " + step + ", " + total + ")";
		}
	}

	static private class TwuNameServices
	{
		static private final String default_experiment = "textor";
		static private final String default_provider_url = "ipptwu.ipp.kfa-juelich.de";

		static private boolean catersFor(DataProvider dp)
		{
			return (dp instanceof TwuDataProvider);
		}

		static private String GetSignalPath(String internalSignalURL, long shot)
		{
			if (IsFullURL(internalSignalURL))
				return internalSignalURL;
			else
			{
				// Hashed_URLs
				// Check if signal path is in the format
				// //url_server_address/experiment/shotGroup/#####/signal_path
				if (isHashedURL(internalSignalURL))
					return hashed2shot(internalSignalURL, shot);
				// If not, then it is of the old jScope internal format
				// url_server_address//group/signal_path
				// (Continue handling them; they could come out of .jscp files)
				String p_url = GetURLserver(internalSignalURL);
				if (p_url == null)
					p_url = default_provider_url;
				else
					internalSignalURL = internalSignalURL.substring(internalSignalURL.indexOf("//") + 2,
							internalSignalURL.length());
				final StringTokenizer st = new StringTokenizer(internalSignalURL, "/");
				String full_url = "http://" + p_url + "/" + probableExperiment(null) + "/" + st.nextToken() + "/"
						+ shot;
				while (st.hasMoreTokens())
					full_url += "/" + st.nextToken();
				return full_url;
			}
		}

		static private String GetURLserver(String in)
		{
			// Find the servername, if it follows the (early) jScope internal
			// convention that it is encoded before the double slash.
			int idx;
			String out = null;
			if ((idx = in.indexOf("//")) != -1)
				out = in.substring(0, idx);
			return out;
		}

		static private String hashed2shot(String hashedURL, long shot)
		{
			if (hashedURL == null)
				return hashedURL;
			final int hashfield = hashedURL.indexOf("#");
			if (hashfield == -1)
				return hashedURL;
			final String full_url = hashedURL.substring(0, hashfield) + shot
					+ hashedURL.substring(hashedURL.lastIndexOf("#") + 1);
			return full_url;
		}

		static private boolean IsFullURL(String in)
		{
			in = in.toLowerCase();
			return (in.startsWith("http://") || in.startsWith("//")) && in.indexOf("#") == -1;
		}

		public static boolean isHashedURL(String in)
		{
			in = in.toLowerCase();
			return in.startsWith("//") && in.indexOf("#") != -1;
		}

		static private String probableExperiment(WaveInterface wi)
		{
			if (wi != null)
			{
				if (wi.experiment != null)
					return wi.experiment;
				if (wi.dp != null)
				{
					if (TwuNameServices.catersFor(wi.dp))
					{
						final TwuDataProvider twudp = (TwuDataProvider) wi.dp;
						final String twudp_exp = twudp.getExperiment();
						if (twudp_exp != null)
							return twudp_exp;
					}
				}
			}
			return default_experiment;
		}
	}

	static private class TwuProperties
	{
		protected Properties signalProps = new Properties();
		private URL signalURL = null;
		private boolean textRead = false;

		private TwuProperties(String SigURL)
		{
			this(SigURL, null);
		}

		private TwuProperties(String SigURL, String user_agent)
		{
			final String actual_user_agent = (user_agent != null) ? user_agent
					: "TWUProperties.java for jScope ($Revision$)";
			signalProps = new Properties();
			if (SigURL == null)
				return;
			final String fullURL = CompleteURL(SigURL);
			try
			{
				signalURL = new URL(fullURL);
				final URLConnection con = signalURL.openConnection();
				con.setRequestProperty("User-Agent", actual_user_agent);
				final String mime_type = con.getContentType();
				// Assume (like browsers) that missing mime-type indicates text/html.
				if (mime_type == null || mime_type.indexOf("text") >= 0)
				{
					signalProps.load(con.getInputStream());
					textRead = true;
				}
			}
			catch (final Exception e)
			{
				signalURL = null;
				/* No need for further action; textRead will be left false. */
			}
		}

		// Allow, like many browsers do, URLs without protocol name (the
		// 'http:' bit). Assume in these cases that we want 'http:'
		// This allows. (at least in principle) other protocols.
		private String CompleteURL(String in)
		{
			if (in.indexOf("://") >= 0)
				return in;
			return "http:" + in;
		}

		private boolean Decrementing()
		{
			final String equidistant = signalProps.getProperty("Equidistant");
			return (equidistant != null) && equidistant.equalsIgnoreCase("decrementing");
		}

		private int Dimensions()
		{
			final String dimstr = signalProps.getProperty("Dimensions");
			final Integer dim = Integer.valueOf(dimstr == null ? "0" : dimstr);
			return dim.intValue();
		}

		private boolean Equidistant()
		{
			return (Incrementing() || Decrementing()) && signalProps.getProperty("Signal.Minimum") != null
					&& signalProps.getProperty("Signal.Maximum") != null;
		}

		private String FQAbscissa0Name()
		{
			final String abs = signalProps.getProperty("Abscissa.URL.0");
			return ((abs == null || abs.equalsIgnoreCase("None")) ? null : abs);
		}

		private String FQBulkName()
		{
			return signalProps.getProperty("Bulkfile.URL");
		}

		private String getProperty(String keyword)
		{
			return signalProps.getProperty(keyword);
		}

		private boolean hasAbscissa0()
		{
			final String abscissa = signalProps.getProperty("Abscissa.URL.0");
			return ((abscissa == null) ? false : (!abscissa.equalsIgnoreCase("None")));
		}

		private boolean Incrementing()
		{
			final String equidistant = signalProps.getProperty("Equidistant");
			return (equidistant != null) && equidistant.equalsIgnoreCase("incrementing");
		}

		// Dim0Length = getProperty("Length.dimension.0")
		// Dim1Length = getProperty("Length.dimension.1")
		private int LengthTotal()
		{
			final String ltstr = signalProps.getProperty("Length.total");
			final Integer lt = Integer.valueOf(ltstr == null ? "0" : ltstr);
			return lt.intValue();
		}

		private double Maximum()
		{
			final String max = signalProps.getProperty("Signal.Maximum");
			if (max != null)
				return Double.parseDouble(max);
			return Double.NaN;
		}

		private double Minimum()
		{
			final String min = signalProps.getProperty("Signal.Minimum");
			if (min != null)
				return Double.parseDouble(min);
			return Double.NaN;
		}

		private String Title()
		{
			String title = signalProps.getProperty("Title");
			if (title == null)
				title = signalProps.getProperty("SignalName");
			return title;
		}

		@Override
		public String toString()
		{
			return signalURL == null ? "" : signalURL.toString();
		}

		// FQSignalName = getProperty("SignalURL");
		// SignalName = getProperty("FullSignalName");
		private String Units()
		{
			final String unitstr = signalProps.getProperty("Unit");
			return ((unitstr == null || unitstr.equalsIgnoreCase("None")) ? "" : unitstr);
		}

		protected boolean valid()
		{
			// Should handle this pseudo-numerically
			final String version = signalProps.getProperty("TWU.properties.version");
			return textRead && version != null && (version.equals("0.7") || version.equals("0.8"));
		}
	}

	static private class TwuPropertiesFake extends TwuProperties
	{
		private TwuPropertiesFake(int len)
		{
			super(null);
			signalProps.put("TWU.properties.version", "0.7");
			signalProps.put("Dimensions", "1");
			signalProps.put("Length.total", String.valueOf(len));
			signalProps.put("Length.dimension.0", String.valueOf(len));
			signalProps.put("Equidistant", "Incrementing");
			signalProps.put("Signal.Minimum", "0.0");
			signalProps.put("Signal.Maximum", String.valueOf((double) (len - 1)));
		}

		@Override
		protected boolean valid()
		{
			return true;
		}
	}

	static private class TwuSignal
	{
		private URL bulkURL = null;
		private boolean error = false;
		private boolean finished = false;
		private BufferedReader instream = null;
		private int sampleCount = 0;
		private int samples2Read = 0;
		private TwuProperties twup = null;
		private float ydata[] = null;

		private TwuSignal(TwuProperties SigURL, int firstSample, int step, int maxSamples)
		{
			boolean success = false;
			twup = SigURL;
			if (maxSamples <= 0)
				maxSamples = twup.LengthTotal();
			samples2Read = maxSamples;
			ydata = new float[samples2Read];
			finished = false;
			sampleCount = 0;
			if (!SigURL.valid())
			{
				finished = true;
				return;
			}
			if (twup.Equidistant())
				success = tryToConstruct(firstSample, step, maxSamples);
			if (!success)
				prepareToRead(firstSample, step, maxSamples);
		}

		private int getActualSampleCount()
		{ return sampleCount; }

		private float[] getBulkData()
		{
			/*
			 * Several users of this class do not use the getActualSampleCount() method, but
			 * rely on getBulkData().length to see how many samples are available. Although
			 * this seems reasonable, from the caller's point of view, it is wrong in a
			 * number of borderline situations.
			 *
			 * This could be resolved by using a vector, but that would be slower and the
			 * clientcodes depent on arrays. Since the error conditions do not occur, very
			 * often, it seems better to resolve it by creating a new array in those few
			 * cases when the getActualSampleCount() is less then the array size.
			 */
			if (sampleCount < ydata.length)
			{
				final float[] newydata = new float[sampleCount];
				if (sampleCount > 0)
					System.arraycopy(ydata, 0, newydata, 0, sampleCount);
				ydata = null; // Attempt to trigger garbage collection.
				ydata = newydata;
			}
			return ydata;
		}

		private void prepareToRead(int firstSample, int step, int maxSamples)
		{
			try
			{
				error = false;
				final StringBuffer bulk = new StringBuffer(twup.FQBulkName() + "?start=" + firstSample);
				if (step > 0)
					bulk.append("&step=" + step);
				if (maxSamples > 0)
					bulk.append("&total=" + maxSamples);
				bulkURL = new URL(bulk.toString());
				final URLConnection con = bulkURL.openConnection();
				con.setRequestProperty("User-Agent", "TWUSignal.java for jScope ($Revision$)");
				// It seems to be more efficient, for the type of data we have in the
				// bulk files, to close the connection after the server has send all
				// the data. In that way HTTP/1.1 servers will not "chunk" the data.
				// This chunking doubled the amounts to transfer and the de-chunking
				// on the client side took significant effort.
				con.setRequestProperty("Connection", "close");
				con.connect();
				instream = new BufferedReader(new InputStreamReader(con.getInputStream()));
			}
			catch (final Exception e)
			{
				System.out.println("TWUSignal.prepareToRead :" + e);
				error = true;
			}
		}

		private boolean tryToConstruct(int firstSample, int step, int maxSamples)
		{
			final double min = twup.Minimum();
			final double max = twup.Maximum();
			final double last;
			final double first;
			/* See sanitizeAQ() in the WUServer family */
			if (firstSample < 0)
				firstSample = 0;
			if (step < 1)
				step = 1;
			if (maxSamples < 0)
				maxSamples = 0;
			if (twup.Incrementing())
			{
				first = min;
				last = max;
			}
			else if (twup.Decrementing())
			{
				first = max;
				last = min;
			}
			else
				return false;
			final long lentotal = twup.LengthTotal();
			final long stillToGo = lentotal - firstSample;
			final long stepsToGo = stillToGo < 1 ? 0 : 1 + (stillToGo - 1) / step;
			final long toReturn = stepsToGo < maxSamples ? stepsToGo : maxSamples;
			final double span = last - first;
			final long segments = lentotal - 1;
			final double delta = segments == 0 ? 0 : span / segments;
			final double stepXdelta = step * delta;
			final double firstValue = firstSample * delta + first;
			int ix = 0;
			while (ix < toReturn) // or: (ix < maxSamples ) ???
			{
				ydata[ix] = (float) (ix * stepXdelta + firstValue);
				/*
				 * The following limiting tests, and looping until (ix<maxSamples) were
				 * required, in some early versions of jScope; probably as an artefact of the
				 * problem discussed below, at getBulkData().
				 */
				if (ydata[ix] > max)
					ydata[ix] = (float) max;
				else if (ydata[ix] < min)
					ydata[ix] = (float) min;
				++ix;
			}
			sampleCount = ix;
			finished = true;
			return true;
		}

		private void tryToRead(final int samples2Try)
		{
			int thisAttempt = 0;
			try
			{
				String s = null;
				while ((samples2Try > thisAttempt++) && (samples2Read > sampleCount)
						&& ((s = instream.readLine()) != null))
				{
					final Float F = Float.valueOf(s);
					ydata[sampleCount++] = F.floatValue();
				}
				finished = (sampleCount >= samples2Read || s == null);
				if (finished)
				{
					// boolean premature_eof = (s==null);
					// We should handle this, if it is a real problem.
					try
					{
						instream.close();
					}
					catch (final Exception e)
					{}
					if (sampleCount < samples2Read)
					{
						// Fill-up required
						if (sampleCount == 0)
							ydata[sampleCount++] = 0.0F;
						while (sampleCount < samples2Read)
						{
							ydata[sampleCount] = ydata[sampleCount - 1];
							sampleCount++;
						}
					}
				}
			}
			catch (final Exception e)
			{
				System.out.println("TWUSignal.tryToRead :" + e);
				error = true;
			}
		}
	}

	private class TwuSimpleFrameData implements FrameData
	{
		byte buf[];
		int first_frame_idx = -1;
		String in_y;
		int mode = -1;
		private int n_frames = 0;
		private TwuDataProvider provider = null;
		private int st_idx = -1, end_idx = -1;
		private float times[] = null;

		private TwuSimpleFrameData(TwuDataProvider dp, String in_y, String in_x, float time_min, float time_max)
				throws IOException
		{
			int i;
			float t;
			float all_times[] = null;
			this.provider = dp;
			this.in_y = in_y;
			if (in_x == null || in_x.length() == 0)
				all_times = new float[352 / 3];
			else
				all_times = provider.GetFloatArray(in_x);
			if (all_times == null)
				throw (new IOException("Frame time evaluation error"));
			for (i = 0; i < all_times.length; i++)
				all_times[i] = (float) (-0.1 + 0.06 * i);
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
			if (idx == first_frame_idx && buf != null)
				return buf;
			// b_img = MdsDataProvider.this.GetFrameAt(in_y, st_idx+idx);
			// Da modificare per leggere i frames
			idx *= 3;
			final ConnectionEvent ce = new ConnectionEvent(this, "Loading Image " + idx, 0, 0);
			provider.dispatchConnectionEvent(ce);
			final StringTokenizer st = new StringTokenizer(in_y, "/", true);
			String str = new String();
			final int nt = st.countTokens();
			for (int i = 0; i < nt - 1; i++)
				str = str + st.nextToken();
			String img_name = "00000" + idx;
			img_name = img_name.substring(img_name.length() - 6, img_name.length());
			str = str + img_name + ".jpg";
			final URL url = new URL(str);
			final URLConnection url_con = url.openConnection();
			int size = url_con.getContentLength();
			/* Sometimes size < 0 and an exception is thrown */
			/* Taliercio 27/02/2003 */
			byte b_img[] = null;
			if (size > 0)
			{
				int offset = 0, num_read = 0;
				// byte b_img[] = new byte[size];
				b_img = new byte[size];
				final InputStream is = url_con.getInputStream();
				while (size > 0 && num_read != -1)
				{
					num_read = is.read(b_img, offset, size);
					size -= num_read;
					offset += num_read;
				}
			}
			return b_img;
		}

		@Override
		public Dimension getFrameDimension()
		{ return null; }

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

	static private class TwuSingleSignal
	{
		static private void checkForError(TwuSingleSignal sig) throws Exception
		{
			if (sig != null && sig.error())
				throw ((Exception) sig.errorSource.fillInStackTrace());
		}

		static private int FindIndex(final float target, final TwuSingleSignal xsig, final int start, final int step,
				final int maxpts, final int upperlimit)
		{
			// This is an iterative routine : it 'zooms in' on (a subsampled part of the)
			// abscissa data, until it finds the closest index. It looks between indices
			// start and start+(step*maxpts), subsamples at most maxpts at a time =>
			// next stepsize will be ceil (step/maxpts) ....
			//
			final float[] data = xsig.doFetch(new TwuFetchOptions(start, step, maxpts + 1));
			final int newnum = data.length;
			final int ix = findIndexInSubset(data, target);
			final int bestGuess = start + ix * step;
			if (step > 1)
			{
				// Continue search with smaller step.
				int newstep = (int) Math.ceil(step / ((float) maxpts));
				if (newstep < 1)
					newstep = 1;
				return FindIndex(target, xsig, bestGuess, newstep, maxpts, upperlimit);
			}
			if (ix >= newnum - 1)
				return bestGuess > upperlimit ? upperlimit : bestGuess;
			final boolean closer = (Math.abs(data[ix] - target) <= Math.abs(data[ix + 1] - target));
			return closer ? bestGuess : bestGuess + 1;
		}

		static private int findIndexInSubset(final float[] subsetData, final float target)
		{
			if (subsetData == null)
				return 0;
			final int len = subsetData.length;
			if (len < 2)
				return 0;
			final boolean up = subsetData[1] > subsetData[0];
			int ix = 0;
			if (up)
			{
				while (ix < len && subsetData[ix] <= target)
					ix++;
			}
			else
			{
				while (ix < len && subsetData[ix] >= target)
					ix++;
			}
			if (ix != 0)
				ix--; // correct the overshoot from the 'break'.
			return ix;
		}

		private float[] data = null;
		private Exception errorSource = null;
		private TwuFetchOptions fetchOptions = null;
		private boolean isAbscissa = false;
		private TwuSingleSignal mainSignal = null;
		private TwuProperties properties = null;
		private TwuDataProvider provider = null;
		private long shotOfTheProperties = 0;
		private String source = null;

		public TwuSingleSignal(TwuDataProvider dp, String src)
		{
			provider = dp;
			source = src;
		}

		public TwuSingleSignal(TwuDataProvider dp, TwuSingleSignal prnt)
		{
			provider = dp;
			mainSignal = prnt;
			isAbscissa = true;
		}

		private void checkForError() throws Exception
		{
			checkForError(this);
		}

		private void createScalarData()
		{
			// an extra check to see if it really is a scalar
			if (properties != null && properties.LengthTotal() == 1)
			{
				// return an (almost) empty array so there won't be
				// an error ; also, TwuWaveData.GetTitle() will
				// return an adapted title string containing the scalar value.
				if (properties.getProperty("Signal.Minimum") != null)
					data = new float[]
					{ (float) properties.Minimum() };
				else
					data = new float[]
					{ 0.0f };
				// an empty array would cause an exception in Signal. But this works.
				return;
			}
			else
				setErrorString("No numerical data available!");
			data = null; // 'triggers' display of the error_string.
		}

		private void DispatchConnectionEvent(ConnectionEvent e)
		{
			if (provider != null)
				provider.dispatchConnectionEvent(e);
		}

		private void doClip(TwuFetchOptions opt) throws IOException
		{
			final int length = getTwuProperties(shotOfTheProperties).LengthTotal();
			opt.clip(length);
		}

		protected float[] doFetch(TwuFetchOptions opt)
		{
			ConnectionEvent ce;
			ce = makeConnectionEvent("Start Loading " + (isAbscissa ? "X" : "Y"));
			DispatchConnectionEvent(ce);
			final TwuSignal bulk = new TwuSignal(properties, opt.getStart(), opt.getStep(), opt.getTotal());
			return SimplifiedGetFloats(bulk, opt.getTotal());
		}

		public boolean error()
		{
			return errorSource != null;
		}

		private void fake_my_Properties()
		{
			final int len = mainSignal.getTwuProperties().LengthTotal();
			properties = new TwuPropertiesFake(len);
		}

		private void fetch_my_Properties(String propsurl, String XorY) throws Exception
		{
			DispatchConnectionEvent(makeConnectionEvent("Load Properties", 0, 0));
			properties = new TwuProperties(propsurl);
			DispatchConnectionEvent(makeConnectionEvent(null, 0, 0));
			if (!properties.valid())
			{
				setErrorString("No Such " + XorY + " Signal : " + propsurl);
				throwError("Error loading properties of " + XorY + " data !" + propsurl);
			}
		}

		private void fetch_X_Properties() throws Exception
		{
			checkForError(mainSignal);
			final TwuProperties yprops = mainSignal != null ? mainSignal.getTwuProperties() : null;
			final int dim = yprops != null ? yprops.Dimensions() : -1;
			if (dim > 1 || dim < 0)
				throwError("Not a 1-dimensional signal !");
			if (!yprops.hasAbscissa0())
			{
				fake_my_Properties();
				return;
			}
			final String mypropsurl = yprops.FQAbscissa0Name();
			fetch_my_Properties(mypropsurl, "X");
		}

		private void fetch_Y_Properties() throws Exception
		{
			if (source == null)
				throwError("No input signal set !");
			final String propsurl = TwuNameServices.GetSignalPath(source, shotOfTheProperties);
			fetch_my_Properties(propsurl, "Y");
		}

		private void fetchBulkData() throws Exception
		{
			if (fetchOptions == null)
				throwError("unspecified fetch options (internal error)");
			if (fetchOptions.getTotal() == -1)
				doClip(fetchOptions); // just in case ...
			if (fetchOptions.getTotal() <= 1)
			{
				createScalarData();
				return;
			}
			data = doFetch(fetchOptions);
		}

		// note that this setup only fetches the properties (and, later on, data)
		// if (and when) it is needed. it's less likely to do redundant work than
		// if I'd get the properties in the constructor.
		private void fetchProperties() throws Exception
		{
			try
			{
				// Don't remember errors and data from previous attempts
				errorSource = null;
				// error = false ;
				data = null;
				if (isAbscissa)
					fetch_X_Properties();
				else
					fetch_Y_Properties();
			}
			catch (final Exception e)
			{
				errorSource = e;
				// error = true ;
			}
			checkForError();
		}

		private TwuFetchOptions FindIndicesForXRange(final long requestedShot, final float x_start, final float x_end,
				final int n_points) throws Exception
		{
			final TwuProperties prop = this.getTwuProperties(requestedShot);
			final int len = prop.LengthTotal();
			if (prop.Dimensions() == 0 || len <= 1)
				return new TwuFetchOptions(0, 1, 1); // mainly used to pick scalars out.
			// do an iterated search to find the indices,
			// by reading parts of the abscissa values.
			final int POINTS_PER_REQUEST = 100;
			final int step = (int) Math.ceil(len / (float) POINTS_PER_REQUEST);
			final int ix_start = FindIndex(x_start, this, 0, step, POINTS_PER_REQUEST, len);
			final int ix_end = FindIndex(x_end, this, 0, step, POINTS_PER_REQUEST, len);
			final int range = ix_end - ix_start;
			final int aproxStep = range / (n_points - 1);
			final int finalStep = aproxStep < 1 ? 1 : (range / (n_points - 1));
			final int finalPoints = 1 + (int) Math.floor((float) range / (float) finalStep);
			// I want the last point (ix_end) included.
			// you should end up getting *at least* n_point points.
			return new TwuFetchOptions(ix_start, finalStep, finalPoints);
		}

		public float[] getData() throws IOException
		{
			if (data != null)
				return data;
			try
			{
				fetchBulkData();
			}
			catch (final IOException e)
			{
				throw e;
			}
			catch (final Exception e)
			{
				handleException(e);
				throw new IOException(e.toString());
			}
			return data;
		}

		// -----------------------------------------------------------------------------
		public TwuProperties getTwuProperties()
		{ return properties; }

		public TwuProperties getTwuProperties(long requestedShot) throws IOException
		{
			if (properties == null || shotOfTheProperties != requestedShot)
			{
				try
				{
					shotOfTheProperties = requestedShot;
					fetchProperties();
					// NB, this throws an exception if an error occurs
					// OR HAS occurred before. And what good did that do?
				}
				catch (final IOException e)
				{
					throw e;
				}
				catch (final Exception e)
				{
					handleException(e);
					throw new IOException(e.toString());
					// e.getMessage() might be nicer... but then you won't
					// know the original exception type at all, and
					// there's a possibility there won't be a message
					// in the exception. Then you'd have _nothing_ to go on.
				}
			}
			return properties;
		}

		private ConnectionEvent makeConnectionEvent(String info)
		{
			return new ConnectionEvent((provider != null) ? provider : (Object) this, info);
		}

		private ConnectionEvent makeConnectionEvent(String info, int total_size, int current_size)
		{
			return new ConnectionEvent((provider != null) ? provider : (Object) this, info, total_size, current_size);
		}

		public String ScalarToTitle(long requestedShot) throws Exception
		{
			final TwuProperties props = getTwuProperties(requestedShot);
			// makes sure that the properties are really fetched.
			// although they should already have been if this method is called.
			final String name = props.Title();
			if (props.LengthTotal() > 1)
				return name + " (is not a scalar)";
			final String units = props.Units();
			float min = 0.0f;
			if (props.getProperty("Signal.Minimum") != null)
				min = (float) props.Minimum();
			else
			{
				final float[] scalar = doFetch(new TwuFetchOptions());
				min = scalar[0];
			}
			return name + " = " + min + " " + units;
		}

		private void setErrorString(String errmsg)
		{
			if (provider != null)
				provider.setErrorstring(errmsg);
		}

		public void setFetchOptions(TwuFetchOptions opt) throws IOException
		{
			doClip(opt);
			if (fetchOptions != null && fetchOptions.equalsForBulkData(opt))
			{
				return;
			}
			fetchOptions = opt;
			data = null;
		}

		private float[] SimplifiedGetFloats(final TwuSignal bulk, final int n_point)
		{
			ConnectionEvent ce;
			ce = makeConnectionEvent((isAbscissa ? "Load X" : "Load Y"), 0, 0);
			DispatchConnectionEvent(ce);
			final int inc = Math.max(n_point / Waveform.MAX_POINTS, 10);
			while (!bulk.finished && !bulk.error)
			{
				bulk.tryToRead(inc);
				ce = makeConnectionEvent((isAbscissa ? "X:" : "Y:"), n_point, bulk.getActualSampleCount());
				DispatchConnectionEvent(ce);
				Thread.yield();
				// give the graphics thread a chance to update the user interface (the status
				// bar) ...
			}
			if (bulk.error)
				setErrorString("Error reading Bulk Data");
			DispatchConnectionEvent(makeConnectionEvent(null, 0, 0));
			return bulk.error ? null : bulk.getBulkData();
		}

		// -----------------------------------------------------------------------------
		private void throwError(String msg) throws Exception
		{
			// error = true ;
			errorSource = new Exception(msg);
			throw errorSource;
		}
	}

	private class TwuWaveData implements WaveData
	{
		private TwuSingleSignal abscissa_X = null;
		private String abscissa_X_name = null;
		private TwuSingleSignal mainSignal = null;
		private String mainSignal_name = null;
		private long shotOfThisData = 0;
		private String title = null;

		private TwuWaveData()
		{}

		private TwuWaveData(TwuDataProvider dp, String in_y)
		{
			init(dp, in_y, null);
		}

		private TwuWaveData(TwuDataProvider dp, String in_y, String in_x)
		{
			init(dp, in_y, in_x);
		}

		@Override
		public void addWaveDataListener(WaveDataListener listener)
		{}

		@Override
		public XYData getData(double xmin, double xmax, int numPoints) throws IOException
		{
			final double x[] = GetXDoubleData();
			final float y[] = GetFloatData();
			return new XYData(x, y, Double.MAX_VALUE);
		}

		@Override
		public XYData getData(int numPoints) throws IOException
		{
			final double x[] = GetXDoubleData();
			final float y[] = GetFloatData();
			return new XYData(x, y, Double.MAX_VALUE);
		}

		@Override
		public XYData getData(long xmin, long xmax, int numPoints) throws IOException
		{
			final double x[] = GetXDoubleData();
			final float y[] = GetFloatData();
			return new XYData(x, y, Double.MAX_VALUE);
		}

		@Override
		public void getDataAsync(double lowerBound, double upperBound, int numPoints)
		{}

		// JScope has an inconsistent way of dealing with data: GetFloatData() is used
		// to get the Y data, and *if* there's an abscissa (aka time data, aka X data)
		// this is retrieved using GetXData(). however, GetYData() is not used ?! MvdG
		// It is used! it represents the second abscissa, for a 2D signal! JGK
		private float[] GetFloatData() throws IOException
		{
			if (mainSignal == null || mainSignal.error())
				return null;
			return mainSignal.getData();
		}

		@Override
		public int getNumDimension() throws IOException
		{ return mainSignal.getTwuProperties(shotOfThisData).Dimensions(); }

		@Override
		public String GetTitle() throws IOException
		{
			// now has a special treatment for scalars ...
			if (title != null)
				return title;
			final int dim = getNumDimension();
			if (dim != 0)
				title = mainSignal.getTwuProperties(shotOfThisData).Title();
			else
			{
				try
				{
					title = mainSignal.ScalarToTitle(shotOfThisData);
				}
				catch (final IOException e)
				{
					throw e;
				}
				catch (final Exception e)
				{
					handleException(e);
					throw new IOException(e.toString());
				}
			}
			return title;
		}

		// another utility method. needed by TwuAccess (via via).
		// this is an efficient way to do it because it allows storage
		// of the properties within the TwuSingleSignal, so it won't
		// need to be retrieved time after time ...
		//
		private TwuProperties getTwuProperties() throws IOException
		{ return mainSignal.getTwuProperties(shotOfThisData); }

		@Override
		public double[] getX2D()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		@Override
		public long[] getX2DLong()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		private float[] getXData() throws IOException
		{ return abscissa_X.getData(); }

		private double[] GetXDoubleData()
		{
			return null;
		}

		@Override
		public String GetXLabel() throws IOException
		{
			return abscissa_X.getTwuProperties(shotOfThisData).Units();
		}

		@Override
		public float[] getY2D()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		private float[] getYData() throws IOException
		{
			return mainSignal.getData(); // used to be : return null; ... :o
			// Wrong !! should return Abscissa.1 data!
			// TODO: To be fixed later! JGK.
		}

		@Override
		public String GetYLabel() throws IOException
		{
			return mainSignal.getTwuProperties(shotOfThisData).Units();
		}

		@Override
		public float[] getZ()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		@Override
		public String GetZLabel() throws IOException
		{
			return null;
		}

		private void init(TwuDataProvider dp, String in_y, String in_x)
		{
			in_y = (in_y != null && in_y.trim().length() != 0) ? in_y.trim() : null;
			in_x = (in_x != null && in_x.trim().length() != 0) ? in_x.trim() : null;
			shotOfThisData = dp.shot;
			mainSignal_name = in_y;
			abscissa_X_name = in_x;
			mainSignal = new TwuSingleSignal(dp, mainSignal_name);
			if (abscissa_X_name != null)
				abscissa_X = new TwuSingleSignal(dp, abscissa_X_name);
			else
				abscissa_X = new TwuSingleSignal(dp, mainSignal);
		}

		@Override
		public boolean isXLong()
		{ return false; }

		private boolean notEqualsInputSignal(String in_y, String in_x, long requestedShot)
		{
			// this uses a simple (i.e. imperfect) comparison approach to see
			// if the WaveData for in_x, in_y has already been created ...
			if (shotOfThisData != requestedShot)
				return true;
			in_y = (in_y != null && in_y.trim().length() != 0) ? in_y.trim() : null;
			in_x = (in_x != null && in_x.trim().length() != 0) ? in_x.trim() : null;
			final boolean y_equals = (in_y == null) ? (mainSignal_name == null)
					: (mainSignal_name != null && in_y.equalsIgnoreCase(mainSignal_name));
			final boolean x_equals = (in_x == null) ? (abscissa_X_name == null)
					: (abscissa_X_name != null && in_x.equalsIgnoreCase(abscissa_X_name));
			return !(x_equals && y_equals);
		}

		@Override
		public void removeWaveDataListener(WaveDataListener listener)
		{}

		// A little utility method for the subclasses ...
		// (most fetch options, particularly settings involved with zoom range,
		// should be the same for both x and y data.)
		//
		private void setFetchOptions(TwuFetchOptions opt) throws IOException
		{
			mainSignal.setFetchOptions(opt);
			abscissa_X.setFetchOptions(opt);
		}

		private void setFullFetch()
		{
			try
			{
				setFetchOptions(new TwuFetchOptions());
			}
			catch (final IOException e)
			{}
			// same story as above, in setZoom.
		}

		private void setZoom(float xmin, float xmax, int n_points)
		{
			// this method allows reusing of this object
			// to fetch data from the same signal at a
			// different zoomrange.
			try
			{
				setFetchOptions(abscissa_X.FindIndicesForXRange(shotOfThisData, xmin, xmax, n_points));
			}
			catch (final Exception e)
			{}
			// the TwuSingleSignal already has the error flag set (?),
			// and it will throw an exception when jscope tries to
			// call GetFloatData().
		}

		@Override
		public boolean supportsStreaming()
		{
			return false;
		}
	}

	static private void handleException(Exception e)
	{
		if (Waveform.is_debug)
			e.printStackTrace(System.out);
	}

	private transient Vector<ConnectionListener> connection_listener = new Vector<>();
	private String error_string;
	private String experiment;
	private TwuWaveData lastWaveData = null;
	private long shot;

	public TwuDataProvider()
	{
		super();
	}

	public TwuDataProvider(String user_agent)
	{
		// this.user_agent = user_agent;
		// Could be used in the constructor for TwuProperties and in similar get URL
		// actions.
		// A site could used this as a possible (internal) software distribution
		// management
		// tool. In the log of a web-server you can, by checking the user_agent, see
		// which
		// machines are still running old software.
	}

	// -------------------------------------------
	// connection handling methods ....
	// -------------------------------------------
	@Override
	public synchronized void addConnectionListener(ConnectionListener l)
	{
		if (l == null)
			return;
		connection_listener.addElement(l);
	}

	@Override
	public void addUpdateEventListener(UpdateEventListener l, String event)
	{}

	@Override
	public void close()
	{}

	public void dispatchConnectionEvent(ConnectionEvent e)
	{
		if (connection_listener != null)
			for (int i = 0; i < connection_listener.size(); i++)
				connection_listener.elementAt(i).processConnectionEvent(e);
	}

	public synchronized TwuWaveData FindWaveData(String in_y, String in_x)
	{
		if (lastWaveData == null || lastWaveData.notEqualsInputSignal(in_y, in_x, shot))
		{
			lastWaveData = new TwuWaveData(this, in_y, in_x);
			try
			{
				// Ensure that the properties are fetched right away.
				lastWaveData.getTwuProperties();
			}
			catch (final IOException e)
			{
				setErrorstring("No Such Signal : " + TwuNameServices.GetSignalPath(in_y, shot));
				// throw new IOException ("No Such Signal");
			}
		}
		return lastWaveData;
	}

	@Override
	public String getError()
	{ return error_string; }

	public synchronized String getExperiment()
	{ return experiment; }

	@Override
	public double getFloat(String in, int row, int col, int index)
	{
		return Double.parseDouble(in);
	}

	// ---------------------------------------------------------------------------------------------
	public synchronized float[] GetFloatArray(String in)
	{
		resetErrorstring(null);
		if (in.startsWith("TIME:", 0))
			in = in.substring(5);
		final TwuWaveData wd = (TwuWaveData) getWaveData(in, 0, 0, 0);
		float[] data = null;
		try
		{
//            data = wd.GetFloatData() ;
			data = wd.getData(4000).y;
		}
		catch (final Exception e)
		{
			resetErrorstring(e.toString());
			data = null;
		}
		return data;
	}

	// ----------------------------------------------------
	// Methods for TwuAccess.
	// ----------------------------------------------------
	public synchronized float[] GetFloatArray(String in, boolean is_time) throws IOException
	{
		final TwuWaveData wd = (TwuWaveData) getWaveData(in, 0, 0, 0); // TwuAccess wants to get the full signal data .
		return is_time ? wd.getXData() : wd.getYData();
	}

	@Override
	public FrameData getFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
	{
		return (new TwuSimpleFrameData(this, in_y, in_x, time_min, time_max));
	}

	public synchronized WaveData GetResampledWaveData(String in, double start, double end, int n_points)
	{
		return GetResampledWaveData(in, null, start, end, n_points);
	}

	public synchronized WaveData GetResampledWaveData(String in_y, String in_x, double start, double end, int n_points)
	{
		final TwuWaveData find = FindWaveData(in_y, in_x);
		find.setZoom((float) start, (float) end, n_points);
		return find;
	}

	// -------------------------------------------------------
	// parsing of / extraction from input signal string
	// -------------------------------------------------------
	@Override
	public long[] getShots(String in, String experiment)
	{
		resetErrorstring(null);
		long[] result;
		String curr_in = in.trim();
		if (curr_in.startsWith("[", 0))
		{
			if (curr_in.endsWith("]"))
			{
				curr_in = curr_in.substring(1, curr_in.length() - 1);
				final StringTokenizer st = new StringTokenizer(curr_in, ",", false);
				result = new long[st.countTokens()];
				int i = 0;
				try
				{
					while (st.hasMoreTokens())
						result[i++] = Integer.parseInt(st.nextToken());
					return result;
				}
				catch (final Exception e)
				{}
			}
		}
		else
		{
			if (curr_in.indexOf(":") != -1)
			{
				final StringTokenizer st = new StringTokenizer(curr_in, ":");
				int start, end;
				if (st.countTokens() == 2)
				{
					try
					{
						start = Integer.parseInt(st.nextToken());
						end = Integer.parseInt(st.nextToken());
						if (end < start)
							end = start;
						result = new long[end - start + 1];
						for (int i = 0; i < end - start + 1; i++)
							result[i] = start + i;
						return result;
					}
					catch (final Exception e)
					{}
				}
			}
			else
			{
				result = new long[1];
				try
				{
					result[0] = Long.parseLong(curr_in);
					return result;
				}
				catch (final Exception e)
				{}
			}
		}
		resetErrorstring("Error parsing shot number(s)");
		return null;
	}

	public synchronized String GetSignalProperty(String prop, String in) throws IOException
	{
		final TwuWaveData wd = (TwuWaveData) getWaveData(in, 0, 0, 0);
		return wd.getTwuProperties().getProperty(prop);
	}

	@Override
	public String getString(String in, int row, int col, int index)
	{
		return in;
	}

	// ---------------------------------------------------
	@Override
	public synchronized WaveData getWaveData(String in, int row, int col, int index)
	{
		return getWaveData(in, null, 0, 0, 0);
	}

	@Override
	public synchronized WaveData getWaveData(String in_y, String in_x, int row, int col, int index)
	{
		final TwuWaveData find = FindWaveData(in_y, in_x);
		find.setFullFetch();
		return find;
	}

	@Override
	public int inquireCredentials(JFrame f, DataServerItem server_item)
	{
		return DataProvider.LOGIN_OK;
	}

	@Override
	public boolean isBusy()
	{ return false; }

	@Override
	public synchronized void removeConnectionListener(ConnectionListener l)
	{
		if (l == null)
			return;
		connection_listener.removeElement(l);
	}

	@Override
	public void removeUpdateEventListener(UpdateEventListener l, String event)
	{}

	protected synchronized void resetErrorstring(String newErrStr)
	{
		error_string = newErrStr;
	}

	@Override
	public void setArgument(String arg)
	{}

	@Override
	public void setEnvironment(String s)
	{}

	public synchronized void setErrorstring(String newErrStr)
	{
		if (error_string == null)
			error_string = newErrStr;
	}

	@Override
	public boolean supportsTunneling()
	{
		return false;
	}

	// -------------------------------------------
	// Constructor, other small stuff ...
	// -------------------------------------------
	@Override
	public synchronized void update(String experiment, long shot)
	{
		this.experiment = experiment;
		this.shot = shot;
		resetErrorstring(null);
		lastWaveData = null;
	}
}

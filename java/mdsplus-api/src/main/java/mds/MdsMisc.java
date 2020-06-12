package mds;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.nio.DoubleBuffer;
import java.nio.FloatBuffer;
import java.nio.LongBuffer;
import mds.Mds.Request;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_a.Uint8Array;
import mds.data.descriptor_s.Float32;
import mds.data.descriptor_s.Int32;
import mds.data.descriptor_s.Int64;

public class MdsMisc extends Mdsdcl
{
	public final static class DataStruct
	{
		private static String byte2String(final byte[] bytes)
		{
			try
			{
				return new String(bytes, "UTF-8");
			}
			catch (final UnsupportedEncodingException e)
			{
				return null;
			}
		}

		public final String y_label;
		public final String x_label;
		public final String title;
		public final float[] y;
		public final long[] lx;
		public final double[] dx;
		public final float[] fx;
		public final int length;
		public final float preres;
		public final byte type;
		public final double actres;

		public DataStruct(final byte[] bytes)
		{
			final ByteBuffer buf = ByteBuffer.wrap(bytes);
			this.preres = buf.getFloat();
			this.length = Math.max(buf.getInt(), 0);
			this.type = buf.get();
			this.y = new float[this.length];
			buf.asFloatBuffer().get(this.y);
			buf.position(buf.position() + Float.BYTES * this.length);
			if (this.type == 1)
			{// Long X i.e. nsec
				this.fx = null;
				this.dx = null;
				this.lx = new long[this.length];
				buf.asLongBuffer().get(this.lx);
				buf.position(buf.position() + Long.BYTES * this.length);
				this.actres = this.length == 0 ? 0 : this.length * 1e9 / (this.lx[this.length - 1] - this.lx[0]);
			}
			else if (this.type == 2)
			{// double X i.e. sec
				this.fx = null;
				this.dx = new double[this.length];
				this.lx = null;
				buf.asDoubleBuffer().get(this.dx);
				buf.position(buf.position() + Double.BYTES * this.length);
				this.actres = this.length == 0 ? 0 : this.length / (this.dx[this.length - 1] - this.dx[0]);
			}
			else
			{ // float X
				this.fx = new float[this.length];
				this.dx = null;
				this.lx = null;
				buf.asFloatBuffer().get(this.fx);
				buf.position(buf.position() + Float.BYTES * this.length);
				this.actres = this.length == 0 ? 0 : this.length / (this.fx[this.length - 1] - this.fx[0]);
			}
			if (this.length == 0)
			{
				this.title = this.x_label = this.y_label = null;
				return;
			}
			// Get title, xLabel and yLabel
			final int titleLen = buf.getInt();
			if (titleLen > 0)
			{
				final byte[] titleBuf = new byte[titleLen];
				buf.get(titleBuf);
				this.title = DataStruct.byte2String(titleBuf);
			}
			else
				this.title = null;
			final int xLabelLen = buf.getInt();
			if (xLabelLen > 0)
			{
				final byte[] xLabelBuf = new byte[xLabelLen];
				buf.get(xLabelBuf);
				this.x_label = DataStruct.byte2String(xLabelBuf);
			}
			else
				this.x_label = null;
			final int yLabelLen = buf.getInt();
			if (yLabelLen > 0)
			{
				final byte[] yLabelBuf = new byte[yLabelLen];
				buf.get(yLabelBuf);
				this.y_label = DataStruct.byte2String(yLabelBuf);
			}
			else
				this.y_label = null;
		}

		public final boolean is_double()
		{
			return this.type == 2;
		}

		public final boolean is_float()
		{
			return this.type == 3;
		}

		public final boolean is_long()
		{
			return this.type == 1;
		}

		public final double[] to_double()
		{
			if (this.is_double())
				return this.dx;
			if (this.is_long())
			{
				final double[] d = new double[this.lx.length];
				final LongBuffer lb = LongBuffer.wrap(this.lx);
				final DoubleBuffer db = DoubleBuffer.wrap(d);
				while (lb.hasRemaining())
					db.put(lb.get() / 1e9);
				return d;
			}
			if (this.is_float())
			{
				final double[] d = new double[this.fx.length];
				final FloatBuffer fb = FloatBuffer.wrap(this.fx);
				final DoubleBuffer db = DoubleBuffer.wrap(d);
				while (db.hasRemaining())
					db.put(fb.get());
				return d;
			}
			return null;
		}

		public final float[] to_float()
		{
			if (this.is_float())
				return this.fx;
			if (this.is_long())
			{
				final float[] f = new float[this.lx.length];
				final LongBuffer lb = LongBuffer.wrap(this.lx);
				final FloatBuffer fb = FloatBuffer.wrap(f);
				while (lb.hasRemaining())
					fb.put(lb.get() / 1e9f);
				return f;
			}
			if (this.is_double())
			{
				final float[] f = new float[this.dx.length];
				final DoubleBuffer db = DoubleBuffer.wrap(this.dx);
				final FloatBuffer fb = FloatBuffer.wrap(f);
				while (db.hasRemaining())
					fb.put((float) (db.get()));
				return f;
			}
			return null;
		}

		public final long[] to_long()
		{
			if (this.is_long())
				return this.lx;
			if (this.is_double())
			{
				final long[] l = new long[this.dx.length];
				final DoubleBuffer db = DoubleBuffer.wrap(this.dx);
				final LongBuffer lb = LongBuffer.wrap(l);
				while (db.hasRemaining())
					lb.put((long) (db.get() * 1e9));
				return l;
			}
			if (this.is_float())
			{
				final long[] l = new long[this.fx.length];
				final FloatBuffer fb = FloatBuffer.wrap(this.fx);
				final LongBuffer lb = LongBuffer.wrap(l);
				while (fb.hasRemaining())
					lb.put((long) (fb.get() * 1e9));
				return l;
			}
			return null;
		}
	}

	@SuppressWarnings("rawtypes")
	public static final class MiscCall<T extends Descriptor> extends Shr.LibCall<T>
	{
		public MiscCall(final Class<T> rtype, final String name)
		{
			super(0, rtype, name);
		}

		@Override
		protected final String getImage()
		{ return "MdsMisc"; }
	}

	public MdsMisc(final Mds mds)
	{
		super(mds);
	}

	private final byte[] _miscGetXYSignal(final CTX ctx, final String ydata, final String xdata, final float xmin,
			final float xmax, final int num_samples) throws MdsException
	{
		final Request<Uint8Array> request = new MiscCall<>(Uint8Array.class, "GetXYSignal:DSC")//
				.ref(Descriptor.valueOf(ydata)).ref(Descriptor.valueOf(xdata)).ref(new Float32(xmin))
				.ref(new Float32(xmax)).ref(new Int32(num_samples)).fin();
		return this.mds.getByteArray(ctx, request);
	}

	private final byte[] _miscGetXYSignalLongTimes(final CTX ctx, final String ydata, final String xdata,
			final long xmin, final long xmax, final int num_samples) throws MdsException
	{
		final Request<Uint8Array> request = new MiscCall<>(Uint8Array.class, "GetXYSignalLongTimes:DSC")//
				.ref(Descriptor.valueOf(ydata)).ref(Descriptor.valueOf(xdata)).ref(new Int64(xmin)).ref(new Int64(xmax))
				.ref(new Int32(num_samples)).fin();
		return this.mds.getByteArray(ctx, request);
	}

	public final DataStruct miscGetXYSignal(final CTX ctx, final String ydata, final String xdata, final float xmin,
			final float xmax, final int num_samples) throws MdsException
	{
		return new DataStruct(this._miscGetXYSignal(ctx, ydata, xdata, xmin, xmax, num_samples));
	}

	public final DataStruct miscGetXYSignalLongTimes(final CTX ctx, final String ydata, final String xdata,
			final long xmin, final long xmax, final int num_samples) throws MdsException
	{
		return new DataStruct(this._miscGetXYSignalLongTimes(ctx, ydata, xdata, xmin, xmax, num_samples));
	}

	public Descriptor<?> miscGetXYSignalXd(final CTX ctx, final Descriptor<?> ydata, final Descriptor<?> xdata,
			final Descriptor<?> xmin, final Descriptor<?> xmax, final int num_samples) throws MdsException
	{
		@SuppressWarnings("rawtypes")
		final Request<Descriptor> request = new MiscCall<>(Descriptor.class, "GetXYSignalXd")//
				.xd(ydata).xd(xdata).descr(xmin).descr(xmax).val(num_samples).xd("x").fin("__x");
		// request uses xd instead of descr in order to avoid evaluation in TdiCall
		return this.mds.getDescriptor(ctx, request);
	}
}

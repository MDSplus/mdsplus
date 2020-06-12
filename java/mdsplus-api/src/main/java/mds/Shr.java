package mds;

import java.util.ArrayList;
import mds.Mds.Request;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_s.Pointer;

public abstract class Shr
{
	@SuppressWarnings("rawtypes")
	public static abstract class LibCall<T extends Descriptor>
	{
		protected static final String finT = "__t=__s;MdsShr->StrFree1Dx(ref(__s));execute(\"deallocate('__*');`__t\")";
		private final StringBuilder sb;
		private final ArrayList<Descriptor<?>> args = new ArrayList<>();
		private final Class<T> rtype;
		private final int props;
		private int nargs = 0;
		private final StringBuilder xd = new StringBuilder();
		private final StringBuilder ref = new StringBuilder();
		private final StringBuilder descr = new StringBuilder();
		private final boolean free_s;

		protected LibCall(final int props, final Class<T> rtype, final String name)
		{
			this.sb = new StringBuilder(1024);
			this.rtype = rtype;
			this.props = props;
			this.free_s = name.endsWith(":T");
			this.sb.append(this.getImage()).append("->").append(name).append('(');
		}

		public final LibCall<T> addArg(final Descriptor<?> d)
		{
			this.args.add(d);
			return this;
		}

		public final LibCall<T> arg(final Object... arg)
		{
			this.sep();
			for (final Object o : arg)
				this.sb.append(o);
			this.nargs++;
			return this;
		}

		public final LibCall<T> ctx(final Pointer ctx)
		{
			this.descr.insert(0, "__c=($;);");
			this.args.add(0, ctx);
			return this.arg("val(__c)");
		}

		public final LibCall<T> ctxp(final Pointer ctxp)
		{
			this.descr.insert(0, "__c=($;);");
			this.args.add(0, ctxp);
			return this.arg("ref(__c)");
		}

		public final LibCall<T> descr(final Descriptor<?> d)
		{
			if (Descriptor.isMissing(d))
				return this.miss();
			this.args.add(d);
			return this.arg("descr(($;))");
		}

		public final LibCall<T> descr(final int val)
		{
			this.sep().append("descr(").append(val).append(')');
			this.nargs++;
			return this;
		}

		public final LibCall<T> descr(final long val)
		{
			this.sep().append("descr(").append(val).append("Q)");
			this.nargs++;
			return this;
		}

		/**
		 * Adds descr(__var) and initializes __var=init
		 */
		public final LibCall<T> descr(final String var, final String init)
		{
			this.descr.append("__").append(var).append('=').append(init).append(';');
			return this.obj("descr", "__", var);
		}

		public String expr(final String... suffix)
		{
			this.sb.append(')');
			if (this.descr.length() == 0 && this.ref.length() == 0 && this.xd.length() == 0 && !this.free_s)
			{
				for (final String str : suffix)
					this.sb.append(str);
				return this.sb.toString();
			}
			final StringBuilder out = new StringBuilder();
			if (this.xd.length() > 0)
				this.xd.append("*;");
			out.append(this.xd).append(this.ref).append(this.descr).append("__s=").append(this.sb);
			out.append(";execute(\"");
			if (this.free_s)
				out.append("MdsShr->StrFree1Dx(ref(__s));");
			out.append("deallocate('__*');as_is(`(");
			if (suffix.length == 0)
				out.append("__s");
			else
				for (final String str : suffix)
					out.append(str);
			out.append(";))\")");
			return out.toString();
		}

		public final Request<T> fin(final String... suffix)
		{
			return new Request<>(this.rtype, this.props, this.expr(suffix), this.args.toArray(new Descriptor<?>[0]));
		}

		public Request<T> finA(final String... elements)
		{
			final StringBuilder ret = new StringBuilder().append("[");
			for (final String e : elements)
				ret.append("__").append(e).append(',');
			return this.fin(ret.toString(), "*]");
		}

		public Request<T> finL(final String... elements)
		{
			final StringBuilder ret = new StringBuilder().append("List(*");
			for (final String e : elements)
				ret.append(",__").append(e);
			return this.fin(ret.toString(), ")");
		}

		public Request<T> finV(final String var)
		{
			return this.fin("__", var);
		}

		protected abstract String getImage();

		public final LibCall<T> MdsEND_ARG(final Mds mds)
		{
			return this.val(mds.MdsEND_ARG());
		}

		public final LibCall<T> miss()
		{
			this.sep().append('*');
			this.nargs++;
			return this;
		}

		final LibCall<T> obj(final String mod, final String... ex)
		{
			this.sep().append(mod).append('(');
			if (ex.length == 0)
				this.sb.append("($;)");
			else
				for (final String e : ex)
					this.sb.append(e);
			this.sb.append(')');
			this.nargs++;
			return this;
		}

		public final LibCall<T> ref(final Descriptor<?> d)
		{
			if (Descriptor.isMissing(d))
				return this.miss();
			this.args.add(d);
			return this.arg("ref(($;))");
		}

		public final LibCall<T> ref(final long val)
		{
			this.sep().append("ref(").append(val).append("Q)");
			this.nargs++;
			return this;
		}

		public LibCall<T> ref(final String var, final int init)
		{
			return this.ref(var, Integer.toString(init));
		}

		public LibCall<T> ref(final String var, final long init)
		{
			return this.ref(var, Long.toString(init) + "q");
		}

		/**
		 * Adds ref(__var) and initializes __var=init
		 */
		public final LibCall<T> ref(final String var, final String init)
		{
			this.ref.append("__").append(var).append('=').append(init).append(';');
			return this.obj("ref", "__", var);
		}

		private final StringBuilder sep()
		{
			if (this.nargs == 0)
				return this.sb;
			return this.sb.append(',');
		}

		public final LibCall<T> val(final Descriptor<?> d)
		{
			if (Descriptor.isMissing(d))
				return this.miss();
			this.args.add(d);
			return this.arg("val(($;))");
		}

		public final LibCall<T> val(final int val)
		{
			this.sep().append("val(").append(val).append(')');
			this.nargs++;
			return this;
		}

		public final LibCall<T> val(final long val)
		{
			this.sep().append("val(").append(val).append("q)");
			this.nargs++;
			return this;
		}

		public final LibCall<T> xd()
		{
			return this.obj("xd", "*");
		}

		public final LibCall<T> xd(final Descriptor<?> d)
		{
			if (Descriptor.isMissing(d))
				return this.miss();
			this.args.add(d);
			return this.arg("xd(as_is(($;)))");
		}

		public final LibCall<T> xd(final String var)
		{
			this.xd.append("__").append(var).append('=');
			return this.obj("xd", "__", var);
		}
	}

	public static interface Null
	{/* type for NA input */}

	public static final class RequestBuilder
	{
		private final StringBuilder cmds = new StringBuilder(1024);
		private final ArrayList<Descriptor<?>> argsin = new ArrayList<>();

		public final void add(final Request<?> req)
		{
			this.add(req.expr, req.args);
		}

		public final void add(final String cmd, final Descriptor<?>... argin)
		{
			if (this.cmds.length() == 0)
				this.cmds.append("List(*,");
			else
				this.cmds.append(',');
			this.cmds.append("(").append(cmd).append(";)");
			for (final Descriptor<?> arg : argin)
				this.argsin.add(arg);
		}

		public final List execute(final Mds mds) throws MdsException
		{
			return this.execute(mds, null);
		}

		public final List execute(final Mds mds, final Pointer ctx) throws MdsException
		{
			return mds.getDescriptor(ctx, this.request());
		}

		public final Request<List> request()
		{
			return new Request<>(List.class, this.cmds.append(")").toString(),
					this.argsin.toArray(new Descriptor[0]));
		}
	}

	public static class StringStatus
	{
		public final String data;
		public final int status;

		public StringStatus(final List datastatus)
		{
			this(datastatus.get(0).toString(), datastatus.get(1).toInt());
		}

		public StringStatus(final String data, final int status)
		{
			this.data = data;
			this.status = status;
		}

		@Override
		public final String toString()
		{
			return new StringBuilder(this.data.length() + 16).append(this.data).append(";").append(this.status)
					.toString();
		}
	}

	public static List getCtx(final CTX ctx, final List ans)
	{
		final Pointer nctx = (Pointer) ans.get(ans.getLength() - 1);
		ctx.getDbid().setAddress(nctx.getAddress());
		return ans;
	}

	protected final Mds mds;

	public Shr(final Mds mds)
	{
		this.mds = mds;
	}
}

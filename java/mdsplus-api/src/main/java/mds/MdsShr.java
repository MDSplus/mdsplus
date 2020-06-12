package mds;

import mds.Mds.Request;
import mds.data.descriptor.ARRAY;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;
import mds.data.descriptor.Descriptor_CA;
import mds.data.descriptor.Descriptor_R;
import mds.data.descriptor_a.EmptyArray;
import mds.data.descriptor_a.Int8Array;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_s.Int32;
import mds.data.descriptor_s.StringDsc;

public class MdsShr extends Shr
{
	@SuppressWarnings("rawtypes")
	protected static final class MdsCall<T extends Descriptor> extends Shr.LibCall<T>
	{
		public MdsCall(final Class<T> rtype, final String name)
		{
			super(0, rtype, name);
		}

		public MdsCall(final int prop, final Class<T> rtype, final String name)
		{
			super(prop, rtype, name);
		}

		@Override
		protected final String getImage()
		{ return "MdsShr"; }
	}

	public MdsShr(final Mds mds)
	{
		super(mds);
	}

	public final String getenv(final String name) throws MdsException
	{
		final StringStatus ans = this.translateLogicalXd(null, name);
		MdsException.handleStatus(ans.status);
		return ans.data;
	}

	public final ARRAY<?> mdsCompress(final Null NULL, final Descriptor_A<?> input) throws MdsException
	{
		return this.mdsCompress(null, null, null, input);
	}

	public final ARRAY<?> mdsCompress(final Null NULL, final String entry, final Descriptor_A<?> input)
			throws MdsException
	{
		return this.mdsCompress(null, null, entry, input);
	}

	public final ARRAY<?> mdsCompress(final Null NULL, final String image, final String entry,
			final Descriptor_A<?> input) throws MdsException
	{
		@SuppressWarnings("rawtypes")
		final Request<ARRAY> request = new MdsCall<>(ARRAY.class, "MdsCompress")//
				.ref(Descriptor.valueOf(image)).ref(Descriptor.valueOf(entry)).xd(input).xd("a").finV("a");
		final ARRAY<?> result = this.mds.getDescriptor(null, request);
		return (result == null || result == EmptyArray.NEW) ? input : result;
	}

	public final Descriptor<?> mdsDecompress(final Null NULL, final Descriptor_CA input) throws MdsException
	{
		return this.mdsDecompress(null, input.payload());
	}

	public final Descriptor<?> mdsDecompress(final Null NULL, final Descriptor_R<?> input) throws MdsException
	{
		@SuppressWarnings("rawtypes")
		final Request<Descriptor_A> request = new MdsCall<>(Descriptor_A.class, "MdsDecompress")//
				.xd(input).xd("a").finV("a");
		return this.mds.getDescriptor(null, request);
	}

	public final int mdsEvent(final Null NULL, final String event) throws MdsException
	{
		final Request<Int32> request = new MdsCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "MDSEvent")//
				.ref(Descriptor.valueOf(event)).val(0).val(0).fin();
		return this.mds.getDescriptor(null, request).getValue();
	}

	public final String mdsGetMsg(final Null NULL, final int status) throws MdsException
	{
		final Request<StringDsc> request = new MdsCall<>(StringDsc.class, "MdsGetMsg:T")//
				.val(status).fin();
		return this.mds.getDescriptor(null, request).toString();
	}

	public final String mdsGetMsgDsc(final Null NULL, final int status) throws MdsException
	{
		final Request<StringDsc> request = new MdsCall<>(StringDsc.class, "MdsGetMsgDsc")//
				.val(status).descr("a", "REPEAT(' ',256)").fin("TRIM(__a)");
		return this.mds.getDescriptor(null, request).toString();
	}

	protected final int mdsPutEnv(final Null NULL, final String expr) throws MdsException
	{
		final Request<Int32> request = new MdsCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "MdsPutEnv")//
				.ref(Descriptor.valueOf(expr)).fin();
		return this.mds.getDescriptor(null, request).toInt();
	}

	public final Descriptor<?> mdsSerializeDscIn(final Null NULL, final Int8Array serial) throws MdsException
	{
		@SuppressWarnings("rawtypes")
		final Request<Descriptor> request = new MdsCall<>(Descriptor.class, "MdsSerializeDscIn")//
				.ref(serial).xd("a").finV("a");
		return this.mds.getDescriptor(null, request);
	}

	public final Int8Array mdsSerializeDscOut(final Null NULL, final String expr, final Descriptor<?>... args)
			throws MdsException
	{
		@SuppressWarnings(
		{ "rawtypes", "unchecked" })
		final Request<Int8Array> request = new Request<>(Int8Array.class,
				new MdsCall(null, "MdsSerializeDscOut")//
						.obj("xd", expr).xd("a").expr("__a"),
				args);
		return this.mds.getDescriptor(null, request);
	}

	public final void setenv(final String expr) throws MdsException
	{
		MdsException.handleStatus(this.mdsPutEnv(null, expr));
	}

	public final void setenv(final String name, final String value) throws MdsException
	{
		this.setenv(String.join("=", name, value));
	}

	protected final StringStatus translateLogicalXd(final Null NULL, final String name) throws MdsException
	{
		final Request<List> request = new MdsCall<>(List.class, "TranslateLogicalXd")//
				.descr(Descriptor.valueOf(name)).xd("a").finL("a", "s");
		return new StringStatus(this.mds.getDescriptor(null, request));
	}
}

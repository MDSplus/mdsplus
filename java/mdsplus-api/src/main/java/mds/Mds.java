package mds;

import java.nio.ByteBuffer;
import java.util.*;
import java.util.regex.Pattern;

import mds.Mdsdcl.DclStatus;
import mds.data.CTX;
import mds.data.TREE;
import mds.data.descriptor.*;
import mds.data.descriptor_a.Uint8Array;
import mds.data.descriptor_s.Missing;
import mds.data.descriptor_s.StringDsc;
import mds.mdsip.MdsIp;

public abstract class Mds implements AutoCloseable
{
	public static class EventItem
	{
		public final int eventid;
		public final Vector<UpdateEventListener> listener = new Vector<>();
		public final String name;

		public EventItem(final String name, final int eventid, final UpdateEventListener l)
		{
			this.name = name;
			this.eventid = eventid;
			this.listener.addElement(l);
		}

		@Override
		public String toString()
		{
			return new String("Event name = " + this.name + " Event id = " + this.eventid);
		}
	}

	@SuppressWarnings("rawtypes")
	public static final class Request<T extends Descriptor>
	{
		public static final int PROP_DO_NOT_LIST = 1 << 1;
		public static final int PROP_USES_CTX = 1 << 0;
		public static final int PROP_ATOMIC_RESULT = 1 << 2;
		public final String expr;
		public final Descriptor<?>[] args;
		public Class<T> cls;
		public final int props;

		public Request(final Class<T> cls, final int props, final String expr, final Descriptor<?>... args)
		{
			this.cls = cls;
			this.props = props;
			this.args = args;
			if (args.length > 0 && !Mds.dollar.matcher(expr).find())
			{
				/** If no $ args specified, build argument list cmd($,$,...) **/
				final StringBuilder newexpr = new StringBuilder(expr.length() + args.length * 2 + 1).append(expr);
				for (int i = 0; i < args.length; i++)
					newexpr.append(i == 0 ? '(' : ',').append("($;)");
				this.expr = newexpr.append(')').toString();
			}
			else
				this.expr = expr;
		}

		public Request(final Class<T> cls, final String expr, final Descriptor<?>... args)
		{
			this(cls, 0, expr, args);
		}

		@Override
		final public String toString()
		{
			final String argsstr = Arrays.toString(this.args);
			final StringBuilder sb = new StringBuilder(11 + this.expr.length() + argsstr.length()).append("Execute(\"")
					.append(this.expr).append("\",");
			int len = sb.length();
			sb.append(argsstr).replace(len, len + 1, " ");
			len = sb.length();
			sb.replace(len - 1, len, ")");
			return sb.toString();
		}
	}

	public static final Pattern dollar = Pattern.compile("\\$[0-9]*(?=[^a-zA-Z]|$)");
	protected static final int MAX_NUM_EVENTS = 256;
	private static Mds active;
	private static MdsIp shared_tunnel = null;

	@SuppressWarnings(
	{ "unchecked", "rawtypes" })
	protected static <D extends Descriptor> D bufferToClass(final ByteBuffer b, final Class<D> cls) throws MdsException
	{
		if (b.capacity() == 0)
			return null;// NoData
		if (cls == null)
			throw new MdsException("bufferToClass no class specified");
		if (cls == Descriptor.class)
			return (D) Descriptor.deserialize(b);
		if (cls == Descriptor_S.class)
			return (D) Descriptor_S.deserialize(b);
		if (cls == Descriptor_A.class)
			return (D) Descriptor_A.deserialize(b);
		if (cls == ARRAY.class)
			return (D) ARRAY.deserialize(b);
		try
		{
			return cls.getConstructor(ByteBuffer.class).newInstance(b);
		}
		catch (final Exception e)
		{
			throw new MdsException(cls.getSimpleName(), e);
		}
	}

	public final static Mds getActiveMds()
	{
		return Mds.active;
	}

	public final static MdsIp getLocal()
	{
		if (Mds.shared_tunnel == null)
			Mds.shared_tunnel = new MdsIp();
		if (Mds.shared_tunnel.isReady() == null)
			return Mds.shared_tunnel;
		return null;
	}

	protected transient HashSet<TransferEventListener> translisteners = new HashSet<>();
	protected transient HashSet<ContextEventListener> ctxlisteners = new HashSet<>();
	protected transient boolean[] event_flags = new boolean[Mds.MAX_NUM_EVENTS];
	protected transient Hashtable<Integer, EventItem> hashEventId = new Hashtable<>();
	protected transient Hashtable<String, EventItem> hashEventName = new Hashtable<>();
	protected transient HashSet<String> defined_funs = new HashSet<>();
	private int mds_end_arg = 0;

	/**
	 * getDescriptor evaluates a Request and returns the deserialized Descriptor
	 * expected by the Request
	 *
	 * @param ctx
	 * @param request
	 * @return Descriptor
	 * @throws MdsException
	 */
	@SuppressWarnings("rawtypes")
	protected abstract <T extends Descriptor> T _getDescriptor(final CTX ctx, final Request<T> request)
			throws MdsException;

	public final void addContextEventListener(final ContextEventListener l)
	{
		if (l != null)
			synchronized (this.ctxlisteners)
			{
				this.ctxlisteners.add(l);
			}
	}

	synchronized private final int addEvent(final UpdateEventListener l, final String eventName)
	{
		int eventid = -1;
		EventItem eventItem;
		if (this.hashEventName.containsKey(eventName))
		{
			eventItem = this.hashEventName.get(eventName);
			if (!eventItem.listener.contains(l))
				eventItem.listener.addElement(l);
		}
		else
		{
			eventid = this.getEventId();
			eventItem = new EventItem(eventName, eventid, l);
			this.hashEventName.put(eventName, eventItem);
			this.hashEventId.put(new Integer(eventid), eventItem);
		}
		return eventid;
	}

	public final void addTransferEventListener(final TransferEventListener l)
	{
		if (l != null)
			synchronized (this.translisteners)
			{
				this.translisteners.add(l);
			}
	}

	@Override
	public void close()
	{
		// nothing to do
	}

	public final String dcl(final CTX ctx, final String dclcmd) throws MdsException
	{
		final DclStatus res = this.getAPI().mdsdcl_do_command_dsc(ctx, dclcmd);
		MdsException.handleStatus(res.status);
		final String err = res.getErrString();
		if (err != null)
			throw new Mdsdcl.DclException(err);
		return res.getOutString();
	}

	public final int deallocateAll() throws MdsException
	{
		return this.getInteger(null, "DEALLOCATE('*')");
	}

	public boolean defineFunctions(final String... funs) throws MdsException
	{
		final Collection<String> col = Arrays.asList(funs);
		if (this.defined_funs.containsAll(col))
			return false;
		this.execute(String.join(";", funs));
		this.defined_funs.addAll(col);
		return this.defined_funs.addAll(col);
	}

	private final void dispatchUpdateEvent(final EventItem eventItem)
	{
		synchronized (eventItem.listener)
		{
			for (final UpdateEventListener el : eventItem.listener)
				el.handleUpdateEvent(this, eventItem.name);
		}
	}

	protected final void dispatchUpdateEvent(final int eventid)
	{
		final Integer eventid_obj = new Integer(eventid);
		if (this.hashEventId.containsKey(eventid_obj))
			this.dispatchUpdateEvent(this.hashEventId.get(eventid_obj));
	}

	protected final void dispatchUpdateEvent(final String eventName)
	{
		if (this.hashEventName.containsKey(eventName))
			this.dispatchUpdateEvent(this.hashEventName.get(eventName));
	}

	public abstract void execute(final String expr, final Descriptor<?>... args) throws MdsException;

	public MdsApi getAPI()
	{
		return new MdsApi(this);
	}

	public final byte getByte(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException
	{
		return this.getByte(ctx, req.expr, req.args);
	}

	public final byte getByte(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getDataArray(ctx, expr, args).toByte();
	}

	public final byte getByte(final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getByte(null, expr, args);
	}

	public final byte[] getByteArray(final CTX ctx, final Request<Uint8Array> request) throws MdsException
	{
		return this.getByteArray(ctx, request.expr, request.args);
	}

	public final byte[] getByteArray(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getDataArray(ctx, expr, args).toByteArray();
	}

	public final byte[] getByteArray(final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getByteArray(null, expr, args);
	}

	private final Descriptor<?> getDataArray(final CTX ctx, final String expr, final Descriptor<?>... args)
			throws MdsException
	{
		final Descriptor<?> desc = this.getDescriptor(ctx, expr, args);
		if (desc instanceof StringDsc)
		{
			if (desc.length() < 8)
				return Missing.NEW;
			final ByteBuffer bb = desc.getBuffer();
			final int line = bb.getInt();
			final int len = bb.getInt();
			if (bb.remaining() < len)
				return Missing.NEW;
			throw new MdsException(new StringBuilder(desc.length()).append(line).append(':')
					.append(desc.toString().substring(8, 8 + len)).toString(), 0);
		}
		return desc;
	}

	@SuppressWarnings(
	{ "rawtypes", "resource" })
	public final <T extends Descriptor> T getDescriptor(final CTX ctx, final Request<T> req) throws MdsException
	{
		final Mds mds = Mds.getActiveMds();
		try
		{
			final Mds _mds = this.setActive();
			final T dsc = _mds._getDescriptor(ctx, req);
			if (ctx instanceof TREE)
				dsc.setTree((TREE) ctx);
			return dsc;
		}
		catch (final MdsException exc)
		{
			Mds.active = mds;
			throw exc;
		}
	}

	public final <T extends Descriptor<?>> T getDescriptor(final CTX ctx, final String expr, final Class<T> cls,
			final Descriptor<?>... args) throws MdsException
	{
		return this.getDescriptor(ctx, new Request<>(cls, expr, args));
	}

	@SuppressWarnings("rawtypes")
	public final Descriptor getDescriptor(final CTX ctx, final String expr, final Descriptor<?>... args)
			throws MdsException
	{
		return this.getDescriptor(ctx, new Request<>(Descriptor.class, expr, args));
	}

	public final <T extends Descriptor<?>> T getDescriptor(final String expr, final Class<T> cls,
			final Descriptor<?>... args) throws MdsException
	{
		return this.getDescriptor(null, expr, cls, args);
	}

	public final Descriptor<?> getDescriptor(final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getDescriptor(null, expr, args);
	}

	public final double getDouble(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException
	{
		return this.getDouble(ctx, req.expr, req.args);
	}

	public final double getDouble(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getDataArray(ctx, expr, args).toDouble();
	}

	public final double getDouble(final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getDouble(null, expr, args);
	}

	public final double[] getDoubleArray(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException
	{
		return this.getDoubleArray(ctx, req.expr, req.args);
	}

	public final double[] getDoubleArray(final CTX ctx, final String expr, final Descriptor<?>... args)
			throws MdsException
	{
		return this.getDataArray(ctx, expr, args).toDoubleArray();
	}

	public final double[] getDoubleArray(final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getDoubleArray(null, expr, args);
	}

	private final int getEventId()
	{
		int i;
		for (i = 0; i < Mds.MAX_NUM_EVENTS && this.event_flags[i]; i++)
			continue;
		if (i == Mds.MAX_NUM_EVENTS)
			return -1;
		this.event_flags[i] = true;
		return i;
	}

	public final float getFloat(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException
	{
		return this.getFloat(ctx, req.expr, req.args);
	}

	public final float getFloat(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getDataArray(ctx, expr, args).toFloat();
	}

	public final float getFloat(final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getFloat(null, expr, args);
	}

	public final float[] getFloatArray(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException
	{
		return this.getFloatArray(ctx, req.expr, req.args);
	}

	public final float[] getFloatArray(final CTX ctx, final String expr, final Descriptor<?>... args)
			throws MdsException
	{
		return this.getDataArray(ctx, expr, args).toFloatArray();
	}

	public final float[] getFloatArray(final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getFloatArray(null, expr, args);
	}

	public final int getInteger(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException
	{
		return this.getInteger(ctx, req.expr, req.args);
	}

	public final int getInteger(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getDataArray(ctx, expr, args).toInt();
	}

	public final int getInteger(final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getInteger(null, expr, args);
	}

	public final int[] getIntegerArray(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException
	{
		return this.getIntegerArray(ctx, req.expr, req.args);
	}

	public final int[] getIntegerArray(final CTX ctx, final String expr, final Descriptor<?>... args)
			throws MdsException
	{
		return this.getDataArray(ctx, expr, args).toIntArray();
	}

	public final int[] getIntegerArray(final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getIntegerArray(null, expr, args);
	}

	public final long getLong(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException
	{
		return this.getLong(ctx, req.expr, req.args);
	}

	public final long getLong(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getDataArray(ctx, expr, args).toLong();
	}

	public final long getLong(final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getLong(null, expr, args);
	}

	public final long[] getLongArray(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException
	{
		return this.getLongArray(ctx, req.expr, req.args);
	}

	public final long[] getLongArray(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getDataArray(ctx, expr, args).toLongArray();
	}

	public final long[] getLongArray(final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getLongArray(null, expr, args);
	}

	public final short getShort(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException
	{
		return this.getShort(ctx, req.expr, req.args);
	}

	public final short getShort(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getDataArray(ctx, expr, args).toShort();
	}

	public final short getShort(final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getShort(null, expr, args);
	}

	public final short[] getShortArray(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException
	{
		return this.getShortArray(ctx, req.expr, req.args);
	}

	public final short[] getShortArray(final CTX ctx, final String expr, final Descriptor<?>... args)
			throws MdsException
	{
		return this.getDataArray(ctx, expr, args).toShortArray();
	}

	public final short[] getShortArray(final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getShortArray(null, expr, args);
	}

	public final String getString(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException
	{
		return this.getString(ctx, req.expr, req.args);
	}

	public final String getString(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException
	{
		final Descriptor<?> desc = this.getDescriptor(ctx, expr, args);
		if (desc == null)
			return null;
		return desc.toString();
	}

	public final String getString(final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getString(null, expr, args);
	}

	public final String[] getStringArray(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException
	{
		return this.getStringArray(ctx, req.expr, req.args);
	}

	public final String[] getStringArray(final CTX ctx, final String expr, final Descriptor<?>... args)
			throws MdsException
	{
		return this.getDataArray(ctx, expr, args).toStringArray();
	}

	public final String[] getStringArray(final String expr, final Descriptor<?>... args) throws MdsException
	{
		return this.getStringArray(null, expr, args);
	}

	public final TCL getTCL()
	{
		return new TCL(this);
	}

	public final boolean isLocal()
	{
		return this == Mds.shared_tunnel;
	}

	/**
	 * returns true if the interface features a low latency This will allow more
	 * atomic operations without significant performance issues
	 *
	 * @return boolean
	 */
	public abstract boolean isLowLatency();

	/**
	 * check if interface is ready for interaction. returns null if ready or a
	 * message that describes the current state
	 *
	 * @return String
	 */
	public abstract String isReady();

	protected final int MdsEND_ARG()
	{
		if (this.mds_end_arg == 0)
			try
			{
				if (this.getDescriptor("TdiShr->TdiPi(val(0),val(1))").toLong() == 1) //
					this.mds_end_arg = 1;
				else
					this.mds_end_arg = -1;
			}
			catch (final MdsException e)
			{
				this.mds_end_arg = -1;
			}
		return this.mds_end_arg;
	}

	/**
	 * registers an Event Listener
	 *
	 * @param event
	 * @param eventid
	 */
	protected abstract void mdsSetEvent(final String event, final int eventid);

	public final void removeContextEventListener(final ContextEventListener l)
	{
		if (l == null)
			return;
		synchronized (this.ctxlisteners)
		{
			this.ctxlisteners.remove(l);
		}
	}

	synchronized public final int removeEvent(final UpdateEventListener l, final String event)
	{
		int eventid = -1;
		if (this.hashEventName.containsKey(event))
		{
			final EventItem eventItem = this.hashEventName.get(event);
			eventItem.listener.remove(l);
			if (eventItem.listener.isEmpty())
			{
				eventid = eventItem.eventid;
				this.event_flags[eventid] = false;
				this.hashEventName.remove(event);
				this.hashEventId.remove(new Integer(eventid));
			}
		}
		return eventid;
	}

	public final void removeTransferEventListener(final TransferEventListener l)
	{
		if (l == null)
			return;
		synchronized (this.translisteners)
		{
			this.translisteners.remove(l);
		}
	}

	public final Mds setActive()
	{
		Mds.active = this;
		return this;
	}

	public final void setEvent(final UpdateEventListener l, final String event)
	{
		int eventid;
		if ((eventid = this.addEvent(l, event)) == -1)
			return;
		this.mdsSetEvent(event, eventid);
	}

	public final String tcl(final CTX ctx, final String command) throws MdsException
	{
		this.dcl(ctx, "set command tcl");
		return this.dcl(ctx, command);
	}
}

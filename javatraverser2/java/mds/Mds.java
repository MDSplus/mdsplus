package mds;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Vector;
import mds.data.CTX;
import mds.data.descriptor.ARRAY;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;
import mds.data.descriptor.Descriptor_S;
import mds.data.descriptor_s.CString;
import mds.data.descriptor_s.Missing;

public abstract class Mds{
    public static class EventItem{
        public final int                         eventid;
        public final Vector<UpdateEventListener> listener = new Vector<UpdateEventListener>();
        public final String                      name;

        public EventItem(final String name, final int eventid, final UpdateEventListener l){
            this.name = name;
            this.eventid = eventid;
            this.listener.addElement(l);
        }

        @Override
        public String toString() {
            return new String("Event name = " + this.name + " Event id = " + this.eventid);
        }
    }
    @SuppressWarnings("rawtypes")
    public static final class Request<T extends Descriptor>{
        public static final int      PROP_DO_NOT_LIST   = 1 << 1;
        public static final int      PROP_USES_CTX      = 1 << 0;
        public static final int      PROP_ATOMIC_RESULT = 1 << 2;
        public final String          expr;
        public final Descriptor<?>[] args;
        public Class<T>              cls;
        public final int             props;

        public Request(final Class<T> cls, final int props, final String expr, final Descriptor<?>... args){
            this.cls = cls;
            this.props = props;
            this.args = args;
            if(args.length > 0 && expr.indexOf("$") == -1){
                /** If no $ args specified, build argument list cmd($,$,...) **/
                final StringBuilder newexpr = new StringBuilder(expr.length() + args.length * 2 + 1).append(expr);
                for(int i = 0; i < args.length; i++)
                    newexpr.append(i == 0 ? '(' : ',').append('$');
                this.expr = newexpr.append(')').toString();
            }else this.expr = expr;
        }

        public Request(final Class<T> cls, final String expr, final Descriptor<?>... args){
            this(cls, 0, expr, args);
        }

        @Override
        final public String toString() {
            final String argsstr = Arrays.toString(this.args);
            final StringBuilder sb = new StringBuilder(11 + this.expr.length() + argsstr.length()).append("Execute(\"").append(this.expr).append("\",");
            int len = sb.length();
            sb.append(argsstr).replace(len, len + 1, " ");
            len = sb.length();
            sb.replace(len - 1, len, ")");
            return sb.toString();
        }
    }
    protected static final int MAX_NUM_EVENTS = 256;
    private static Mds         active;

    @SuppressWarnings({"unchecked", "rawtypes"})
    protected static <D extends Descriptor> D bufferToClass(final ByteBuffer b, final Class<D> cls) throws MdsException {
        if(b.capacity() == 0) return null;// NoData
        if(cls == null) throw new MdsException("bufferToClass no class specified");
        if(cls == Descriptor.class) return (D)Descriptor.deserialize(b);
        if(cls == Descriptor_S.class) return (D)Descriptor_S.deserialize(b);
        if(cls == Descriptor_A.class) return (D)Descriptor_A.deserialize(b);
        if(cls == ARRAY.class) return (D)ARRAY.deserialize(b);
        try{
            return cls.getConstructor(ByteBuffer.class).newInstance(b);
        }catch(final Exception e){
            throw new MdsException(cls.getSimpleName(), e);
        }
    }

    public final static Mds getActiveMds() {
        return Mds.active;
    }
    protected transient HashSet<MdsListener>          mdslisteners  = new HashSet<MdsListener>();
    protected transient boolean[]                     event_flags   = new boolean[Mds.MAX_NUM_EVENTS];
    protected transient Hashtable<Integer, EventItem> hashEventId   = new Hashtable<Integer, EventItem>();
    protected transient Hashtable<String, EventItem>  hashEventName = new Hashtable<String, EventItem>();

    /**
     * getDescriptor
     * evaluates a Request and returns the deserialized Descriptor expected by the Request
     *
     * @param ctx
     * @param request
     * @return Descriptor
     * @throws MdsException
     */
    @SuppressWarnings("rawtypes")
    protected abstract <T extends Descriptor> T _getDescriptor(final CTX ctx, final Request<T> req) throws MdsException;

    synchronized private final int addEvent(final UpdateEventListener l, final String eventName) {
        int eventid = -1;
        EventItem eventItem;
        if(this.hashEventName.containsKey(eventName)){
            eventItem = this.hashEventName.get(eventName);
            if(!eventItem.listener.contains(l)) eventItem.listener.addElement(l);
        }else{
            eventid = this.getEventId();
            eventItem = new EventItem(eventName, eventid, l);
            this.hashEventName.put(eventName, eventItem);
            this.hashEventId.put(new Integer(eventid), eventItem);
        }
        return eventid;
    }

    synchronized public final void addMdsListener(final MdsListener l) {
        if(l != null) this.mdslisteners.add(l);
    }

    @SuppressWarnings("static-method")
    public boolean close() {
        return true;
    }

    public final int deallocateAll() throws MdsException {
        return this.getInteger(null, "DEALLOCATE('*')");
    }

    synchronized protected final void dispatchMdsEvent(final MdsEvent e) {
        if(this.mdslisteners != null) for(final MdsListener listener : this.mdslisteners)
            listener.processMdsEvent(e);
    }

    synchronized private final void dispatchUpdateEvent(final EventItem eventItem) {
        final Vector<UpdateEventListener> eventListener = eventItem.listener;
        final UpdateEvent e = new UpdateEvent(this, eventItem.name);
        for(int i = 0; i < eventListener.size(); i++)
            eventListener.elementAt(i).processUpdateEvent(e);
    }

    protected final void dispatchUpdateEvent(final int eventid) {
        final Integer eventid_obj = new Integer(eventid);
        if(this.hashEventId.containsKey(eventid_obj)) this.dispatchUpdateEvent(this.hashEventId.get(eventid_obj));
    }

    protected final void dispatchUpdateEvent(final String eventName) {
        if(this.hashEventName.containsKey(eventName)) this.dispatchUpdateEvent(this.hashEventName.get(eventName));
    }

    public final byte getByte(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException {
        return this.getByte(ctx, req.expr, req.args);
    }

    public final byte getByte(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDataArray(ctx, expr, args).toByte();
    }

    public final byte getByte(final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getByte(null, expr, args);
    }

    public final byte[] getByteArray(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException {
        return this.getByteArray(ctx, req.expr, req.args);
    }

    public final byte[] getByteArray(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDataArray(ctx, expr, args).toByteArray();
    }

    public final byte[] getByteArray(final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getByteArray(null, expr, args);
    }

    private final Descriptor<?> getDataArray(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        final Descriptor<?> desc = this.getDescriptor(ctx, expr, args);
        if(desc instanceof CString){
            if(desc.length() > 0) throw new MdsException(desc.toString(), 0);
            return Missing.NEW;
        }
        return desc;
    }

    @SuppressWarnings("rawtypes")
    public final <T extends Descriptor> T getDescriptor(final CTX ctx, final Request<T> req) throws MdsException {
        final Mds mds = Mds.getActiveMds();
        try{
            return this.setActive()._getDescriptor(ctx, req);
        }catch(final MdsException exc){
            Mds.active = mds;
            throw exc;
        }
    }

    public final <T extends Descriptor<?>> T getDescriptor(final CTX ctx, final String expr, final Class<T> cls, final Descriptor<?>... args) throws MdsException {
        return this.getDescriptor(ctx, new Request<T>(cls, expr, args));
    }

    @SuppressWarnings("rawtypes")
    public final Descriptor getDescriptor(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDescriptor(ctx, new Request<Descriptor>(Descriptor.class, expr, args));
    }

    public final <T extends Descriptor<?>> T getDescriptor(final String expr, final Class<T> cls, final Descriptor<?>... args) throws MdsException {
        return this.getDescriptor(null, expr, cls, args);
    }

    public final Descriptor<?> getDescriptor(final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDescriptor(null, expr, args);
    }

    public final double getDouble(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException {
        return this.getDouble(ctx, req.expr, req.args);
    }

    public final double getDouble(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDataArray(ctx, expr, args).toDouble();
    }

    public final double getDouble(final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDouble(null, expr, args);
    }

    public final double[] getDoubleArray(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException {
        return this.getDoubleArray(ctx, req.expr, req.args);
    }

    public final double[] getDoubleArray(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDataArray(ctx, expr, args).toDoubleArray();
    }

    public final double[] getDoubleArray(final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDoubleArray(null, expr, args);
    }

    private final int getEventId() {
        int i;
        for(i = 0; i < Mds.MAX_NUM_EVENTS && this.event_flags[i]; i++)
            continue;
        if(i == Mds.MAX_NUM_EVENTS) return -1;
        this.event_flags[i] = true;
        return i;
    }

    public final float getFloat(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException {
        return this.getFloat(ctx, req.expr, req.args);
    }

    public final float getFloat(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDataArray(ctx, expr, args).toFloat();
    }

    public final float getFloat(final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getFloat(null, expr, args);
    }

    public final float[] getFloatArray(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException {
        return this.getFloatArray(ctx, req.expr, req.args);
    }

    public final float[] getFloatArray(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDataArray(ctx, expr, args).toFloatArray();
    }

    public final float[] getFloatArray(final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getFloatArray(null, expr, args);
    }

    public final int getInteger(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException {
        return this.getInteger(ctx, req.expr, req.args);
    }

    public final int getInteger(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDataArray(ctx, expr, args).toInt();
    }

    public final int getInteger(final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getInteger(null, expr, args);
    }

    public final int[] getIntegerArray(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException {
        return this.getIntegerArray(ctx, req.expr, req.args);
    }

    public final int[] getIntegerArray(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDataArray(ctx, expr, args).toIntArray();
    }

    public final int[] getIntegerArray(final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getIntegerArray(null, expr, args);
    }

    public final long getLong(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException {
        return this.getLong(ctx, req.expr, req.args);
    }

    public final long getLong(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDataArray(ctx, expr, args).toLong();
    }

    public final long getLong(final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getLong(null, expr, args);
    }

    public final long[] getLongArray(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException {
        return this.getLongArray(ctx, req.expr, req.args);
    }

    public final long[] getLongArray(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDataArray(ctx, expr, args).toLongArray();
    }

    public final long[] getLongArray(final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getLongArray(null, expr, args);
    }

    public final short getShort(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException {
        return this.getShort(ctx, req.expr, req.args);
    }

    public final short getShort(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDataArray(ctx, expr, args).toShort();
    }

    public final short getShort(final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getShort(null, expr, args);
    }

    public final short[] getShortArray(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException {
        return this.getShortArray(ctx, req.expr, req.args);
    }

    public final short[] getShortArray(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDataArray(ctx, expr, args).toShortArray();
    }

    public final short[] getShortArray(final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getShortArray(null, expr, args);
    }

    public final String getString(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException {
        return this.getString(ctx, req.expr, req.args);
    }

    public final String getString(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        final Descriptor<?> desc = this.getDescriptor(ctx, expr, args);
        if(desc == null) return null;
        return desc.toString();
    }

    public final String getString(final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getString(null, expr, args);
    }

    public final String[] getStringArray(final CTX ctx, final Request<Descriptor<?>> req) throws MdsException {
        return this.getStringArray(ctx, req.expr, req.args);
    }

    public final String[] getStringArray(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getDataArray(ctx, expr, args).toStringArray();
    }

    public final String[] getStringArray(final String expr, final Descriptor<?>... args) throws MdsException {
        return this.getStringArray(null, expr, args);
    }

    /**
     * returns true if the interface features a low latency
     * This will allow more atomic operations without significant performance issues
     *
     * @return boolean
     */
    public abstract boolean isLowLatency();

    /**
     * check if interface is ready for interaction.
     * returns null if ready or a message that describes the current state
     *
     * @return String
     */
    public abstract String isReady();

    /**
     * registers an Event Listener
     *
     * @param event
     * @param eventid
     */
    protected abstract void mdsSetEvent(final String event, final int eventid);

    synchronized public final int removeEvent(final UpdateEventListener l, final String event) {
        int eventid = -1;
        if(this.hashEventName.containsKey(event)){
            final EventItem eventItem = this.hashEventName.get(event);
            eventItem.listener.remove(l);
            if(eventItem.listener.isEmpty()){
                eventid = eventItem.eventid;
                this.event_flags[eventid] = false;
                this.hashEventName.remove(event);
                this.hashEventId.remove(new Integer(eventid));
            }
        }
        return eventid;
    }

    synchronized public final void removeMdsListener(final MdsListener l) {
        if(l == null) return;
        this.mdslisteners.remove(l);
    }

    public final Mds setActive() {
        Mds.active = this;
        return this;
    }

    public final void setEvent(final UpdateEventListener l, final String event) {
        int eventid;
        if((eventid = this.addEvent(l, event)) == -1) return;
        this.mdsSetEvent(event, eventid);
    }

    public final String tcl(final String tclcmd) throws MdsException {
        return this.getString("_a=*;TCL($,_a);_a", new CString(tclcmd)).trim();
    }
}

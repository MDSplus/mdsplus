package mds.mdsip;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Set;
import java.util.zip.InflaterInputStream;
import debug.DEBUG;
import mds.MdsEvent;
import mds.MdsException;
import mds.MdsListener;
import mds.data.DTYPE;
import mds.data.descriptor.ARRAY;
import mds.data.descriptor.Descriptor;

public final class Message extends Object{
    public static final int       _mlenI               = 0;
    public static final int       _statI               = 4;
    public static final int       _lenS                = 8;
    public static final int       _nargsB              = 10;
    public static final int       _idxB                = 11;
    public static final int       _midB                = 12;
    public static final int       _typB                = 13;
    public static final int       _clntB               = 14;
    public static final int       _dmctB               = 15;
    public static final int       _dmsI                = 16;
    public static final byte      BIG_ENDIAN_MASK      = (byte)0x80;
    private static final byte     COMPRESSED           = (byte)0x20;
    protected static final String EVENTASTREQUEST      = "---EVENTAST---REQUEST---";
    protected static final String EVENTCANREQUEST      = "---EVENTCAN---REQUEST---";
    public static final int       HEADER_SIZE          = 48;
    public static final byte      JAVA_CLIENT          = (byte)(Message.JAVA_CLIENT_LITTLE | Message.BIG_ENDIAN_MASK);// | Message.SWAP_ENDIAN_ON_SERVER_MASK);
    private static final byte     JAVA_CLIENT_LITTLE   = (byte)0x3;
    // private static final byte SENDCAPABILITIES = (byte)0xF;
    private static final int      SUPPORTS_COMPRESSION = 0x8000;

    // private static final byte SWAP_ENDIAN_ON_SERVER_MASK = (byte)0x40;
    private static final synchronized void dispatchMdsEvent(final Set<MdsListener> mdslisteners, final MdsEvent e) {
        if(mdslisteners != null) for(final MdsListener listener : mdslisteners)
            listener.processMdsEvent(e);
    }

    protected static boolean isRoprand(final byte arr[], final int idx) {
        return(arr[idx] == 0 && arr[idx + 1] == 0 && arr[idx + 2] == -128 && arr[idx + 3] == 0);
    }

    private static final byte[] readBuf(int bytes_to_read, final InputStream dis, final Set<MdsListener> mdslisteners) throws IOException {
        final byte[] buf = new byte[bytes_to_read];
        int read_bytes = 0, curr_offset = 0;
        final boolean send = (bytes_to_read > 2000);
        if(send) Message.dispatchMdsEvent(mdslisteners, new MdsEvent(dis, buf.length, curr_offset));
        try{
            while(bytes_to_read > 0){
                read_bytes = dis.read(buf, curr_offset, bytes_to_read);
                if(read_bytes == -1) throw new SocketException("connection lost");
                curr_offset += read_bytes;
                bytes_to_read -= read_bytes;
                if(send) Message.dispatchMdsEvent(mdslisteners, new MdsEvent(dis, buf.length, curr_offset));
            }
        }finally{
            if(send) Message.dispatchMdsEvent(mdslisteners, new MdsEvent(dis));
        }
        return buf;
    }

    protected static final byte[] readCompressedBuf(final InputStream dis, final ByteOrder byteOrder, final Set<MdsListener> listeners) throws IOException {
        final byte[] unflattened_length = new byte[4];
        dis.read(unflattened_length);
        final int bytes_to_read = ByteBuffer.wrap(unflattened_length).order(byteOrder).getInt() - Message.HEADER_SIZE;
        final InflaterInputStream zis = new InflaterInputStream(dis);
        final byte[] buf = Message.readBuf(bytes_to_read, zis, listeners);
        zis.skip(1); // EOF
        return buf;
    }

    public final static Message receive(final InputStream dis, final Set<MdsListener> mdslisteners) throws IOException {
        final ByteBuffer header = ByteBuffer.wrap(Message.readBuf(Message.HEADER_SIZE, dis, null));
        final byte c_type = header.get(Message._clntB);
        if((c_type & Message.BIG_ENDIAN_MASK) == 0) header.order(ByteOrder.LITTLE_ENDIAN);
        final int msglen = header.getInt(Message._mlenI);
        final ByteBuffer body;
        if(msglen > Message.HEADER_SIZE){
            if((c_type & Message.COMPRESSED) != 0) body = ByteBuffer.wrap(Message.readCompressedBuf(dis, header.order(), mdslisteners));
            else body = ByteBuffer.wrap(Message.readBuf(msglen - Message.HEADER_SIZE, dis, mdslisteners));
        }else body = ByteBuffer.allocate(0);
        body.order(Descriptor.BYTEORDER);
        final Message msg = new Message(header, body);
        if(DEBUG.N) System.err.println(msg.message_id + "< " + msg.toString());
        return msg;
    }
    public final ByteBuffer body;
    private final byte      client_type;
    private final byte      descr_idx;
    private final int       dims[];
    protected final byte    dtype;
    public final ByteBuffer header;
    protected final short   length;
    protected final byte    message_id;
    protected final int     msglen;
    private final byte      nargs;
    private final byte      ndims;
    protected int           status;

    public Message(final byte mid){
        this('\0', mid);
        this.verify();
    }

    public Message(final byte descr_idx, final byte dtype, final byte nargs, final int dims[], final byte body[], final byte mid){
        this(descr_idx, dtype, nargs, dims, (dims == null) ? (byte)0 : (byte)dims.length, body, Message.JAVA_CLIENT, 0, mid);
    }

    public Message(final byte descr_idx, final byte dtype, final byte nargs, final int dims[], final byte ndims, final byte body[], final byte client_type, final int status, final byte mid){
        final int body_size = body == null ? 0 : body.length;
        this.msglen = Message.HEADER_SIZE + body_size;
        this.status = status;
        this.message_id = mid;
        this.length = Descriptor.getDataSize(dtype, body_size);
        this.client_type = client_type;
        this.nargs = nargs;
        this.descr_idx = descr_idx;
        this.ndims = (ndims > ARRAY.MAX_DIM) ? ARRAY.MAX_DIM : ndims;
        if(dims == null || dims.length != ARRAY.MAX_DIM){
            this.dims = new int[ARRAY.MAX_DIM];
            if(dims != null) System.arraycopy(dims, 0, this.dims, 0, this.ndims < dims.length ? this.ndims : dims.length);
        }else this.dims = dims;
        this.dtype = dtype;
        this.body = ByteBuffer.wrap(body);
        if(this.isLittleEndian()) this.body.order(ByteOrder.LITTLE_ENDIAN);
        this.header = this.getHeader();
    }

    public Message(final byte descr_idx, final byte dtype, final byte nargs, final int dims[], final byte ndims, final ByteBuffer body, final int status, final byte mid){
        final int body_size = body == null ? 0 : body.limit();
        this.msglen = Message.HEADER_SIZE + body_size;
        this.status = status;
        this.message_id = mid;
        this.length = Descriptor.getDataSize(dtype, body_size);
        this.nargs = nargs;
        this.descr_idx = descr_idx;
        this.ndims = (ndims > ARRAY.MAX_DIM) ? ARRAY.MAX_DIM : ndims;
        if(dims == null || dims.length != ARRAY.MAX_DIM){
            this.dims = new int[ARRAY.MAX_DIM];
            if(dims != null) System.arraycopy(dims, 0, this.dims, 0, this.ndims < dims.length ? this.ndims : dims.length);
        }else this.dims = dims;
        this.dtype = dtype;
        this.body = body;
        this.client_type = (body == null || body.order() == ByteOrder.BIG_ENDIAN) ? Message.JAVA_CLIENT : Message.JAVA_CLIENT_LITTLE;
        this.header = this.getHeader();
    }

    public Message(final byte descr_idx, final byte dtype, final byte nargs, final int dims[], final ByteBuffer body, final byte mid){
        this(descr_idx, dtype, nargs, dims, (dims == null) ? (byte)0 : (byte)dims.length, body, 0, mid);
    }

    public Message(final ByteBuffer header, final ByteBuffer body){
        this.msglen = header.getInt();
        this.status = header.getInt();
        this.length = header.getShort();
        this.nargs = header.get();
        this.descr_idx = header.get();
        this.message_id = header.get();
        this.dtype = header.get();
        this.client_type = (byte)(header.get() & 0x1F);
        this.ndims = header.get();
        this.dims = new int[ARRAY.MAX_DIM];
        for(int i = 0; i < ARRAY.MAX_DIM; i++)
            this.dims[i] = header.getInt();
        this.header = header;
        this.body = body;
    }

    protected Message(final char c, final byte mid){
        this(String.valueOf(c), mid);
    }

    protected Message(final String s, final byte mid){
        this((byte)0, DTYPE.T, (byte)1, null, s.getBytes(), mid);
    }

    public final byte[] asByteArray() {
        final ByteBuffer b = this.body.duplicate().order(this.body.order());
        return b.array();
    }

    protected final double[] asDoubleArray() {
        final ByteBuffer b = this.body.duplicate().order(this.body.order());
        final double[] out = new double[b.position(0).remaining() / Double.BYTES];
        b.asDoubleBuffer().get(out);
        return out;
    }

    protected final float asFloat(final byte bytes[]) {
        return ByteBuffer.wrap(bytes).order(this.isLittleEndian() ? ByteOrder.LITTLE_ENDIAN : ByteOrder.BIG_ENDIAN).getFloat();
    }

    protected final float[] asFloatArray() {
        final ByteBuffer b = this.body.duplicate().order(this.body.order());
        final float out[] = new float[b.position(0).remaining() / Float.BYTES];
        b.asFloatBuffer().get(out);
        return out;
    }

    protected final int asInt(final byte bytes[]) {
        return ByteBuffer.wrap(bytes).order(this.isLittleEndian() ? ByteOrder.LITTLE_ENDIAN : ByteOrder.BIG_ENDIAN).getInt();
    }

    public final int[] asIntArray() {
        final ByteBuffer b = this.body.duplicate().order(this.body.order());
        final int out[] = new int[b.position(0).remaining() / Integer.BYTES];
        b.asIntBuffer().get(out);
        return out;
    }

    public final long[] asLongArray() {
        final ByteBuffer b = this.body.duplicate().order(this.body.order());
        final long out[] = new long[b.position(0).remaining() / Long.BYTES];
        b.asLongBuffer().get(out);
        return out;
    }

    public final short asShort(final byte bytes[]) {
        return ByteBuffer.wrap(bytes).order(this.isLittleEndian() ? ByteOrder.LITTLE_ENDIAN : ByteOrder.BIG_ENDIAN).getShort();
    }

    public final short[] asShortArray() {
        final ByteBuffer b = this.body.duplicate().order(this.body.order());
        final short out[] = new short[b.position(0).remaining() / Short.BYTES];
        b.asShortBuffer().get(out);
        return out;
    }

    public final String asString() {
        return new String(this.body.array());
    }

    public final long[] asUIntArray() {
        final ByteBuffer b = this.body.duplicate().order(this.body.order());
        final long out[] = new long[b.position(0).remaining() / Integer.BYTES];
        for(int i = 0; i < out.length; i++)
            out[i] = b.getInt() & 0xFFFFFFFFl;
        return out;
    }

    public final int[] asUShortArray() {
        final ByteBuffer b = this.body.duplicate().order(this.body.order());
        final int out[] = new int[b.position(0).remaining() / Short.BYTES];
        for(int i = 0; i < out.length; i++)
            out[i] = b.getShort() & 0xFFFF;
        return out;
    }

    private final ByteBuffer getHeader() {
        final ByteBuffer new_header = ByteBuffer.allocate(Message.HEADER_SIZE).order(this.body.order());
        new_header.putInt(this.msglen);
        new_header.putInt(this.status);
        new_header.putShort(this.length);
        new_header.put(this.getNargs());
        new_header.put(this.descr_idx);
        new_header.put(this.message_id);
        new_header.put(this.dtype);
        new_header.put(this.client_type);
        new_header.put(this.ndims);
        for(final int dm : this.dims)
            new_header.putInt(dm);
        return new_header;
    }

    public final byte getNargs() {
        return this.nargs;
    }

    public final boolean isLittleEndian() {
        return((this.client_type & Message.BIG_ENDIAN_MASK) == 0);
    }

    public final void send(final OutputStream dos) throws MdsException {
        try{
            if(DEBUG.N) System.err.println(this.message_id + "> " + this.toString());
            synchronized(dos){
                dos.write(this.header.array());
                final ByteBuffer buf = this.body.slice();
                while(buf.hasRemaining())
                    dos.write(buf.get());
                dos.flush();
            }
        }catch(final IOException e){
            throw new MdsException(e.getMessage());
        }
    }

    @Override
    public final String toString() {
        try{
            return Descriptor.readMessage(this).toString();
        }catch(final Exception e){
            return DTYPE.getName(this.dtype) + "(" + this.length + "Bytes)";
        }
    }

    protected final void useCompression(final boolean use_cmp) {
        this.status = (use_cmp ? Message.SUPPORTS_COMPRESSION | 5 : 0);
        this.header.putInt(4, this.status);
    }

    public final void verify() {
        this.status |= 1;
    }
}

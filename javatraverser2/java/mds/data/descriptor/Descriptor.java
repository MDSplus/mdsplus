package mds.data.descriptor;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import mds.Mds;
import mds.MdsException;
import mds.TdiShr;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.TREE;
import mds.data.descriptor_a.Int8Array;
import mds.data.descriptor_s.CString;
import mds.data.descriptor_s.Missing;
import mds.data.descriptor_s.NUMBER;
import mds.mdsip.Message;
import mds.mdslib.MdsLib;

/** DSC (24) **/
public abstract class Descriptor<T>{
    static protected class FLAG{
        public static final boolean and(final FLAG flag, final boolean in) {
            if(flag != null) flag.flag = flag.flag && in;
            return in;
        }

        public static final boolean or(final FLAG flag, final boolean in) {
            if(flag != null) flag.flag = flag.flag || in;
            return in;
        }

        public static final boolean set(final FLAG flag, final boolean in) {
            if(flag != null) flag.flag = in;
            return in;
        }

        public static final boolean xor(final FLAG flag, final boolean in) {
            if(flag != null) flag.flag = flag.flag ^ in;
            return in;
        }
        public boolean flag = true;

        public FLAG(){}
    }
    protected static final int    _lenS     = 0;
    protected static final int    _typB     = 2;
    protected static final int    _clsB     = 3;
    protected static final int    _ptrI     = 4;
    public static final ByteOrder BYTEORDER = Descriptor.BYTEORDER;
    public static final short     BYTES     = 8;
    protected static final int    DECO_NRM  = 0;
    protected static final int    DECO_STR  = 1;
    protected static final int    DECO_STRX = Descriptor.DECO_X | Descriptor.DECO_STR;
    protected static final int    DECO_X    = 2;
    public static final boolean   atomic    = false;
    protected static final byte   P_ARG     = 88;
    protected static final byte   P_STMT    = 96;
    protected static final byte   P_SUBS    = 0;
    static protected MdsLib       mdslib    = new MdsLib();

    private static final ByteBuffer Buffer(final byte[] buf, final boolean swap_little) {
        return Descriptor.Buffer(buf, swap_little ? ByteOrder.LITTLE_ENDIAN : ByteOrder.BIG_ENDIAN);
    }

    private static final ByteBuffer Buffer(final byte[] buf, final ByteOrder bo) {
        return ByteBuffer.wrap(buf).asReadOnlyBuffer().order(bo);
    }

    /** null safe decompile of the given Descriptor **/
    public static final String deco(final Descriptor<?> t) {
        return t == null ? "*" : t.decompile();
    }

    /** Returns the Descriptor deserialized from the given byte[] with native byte order (Descriptor.BYTEORDER) **/
    public static final Descriptor<?> deserialize(final byte[] buf) throws MdsException {
        if(buf == null) return null;
        return Descriptor.deserialize(Descriptor.Buffer(buf, Descriptor.BYTEORDER));
    }

    /** Returns the Descriptor deserialized from the given byte[] with byte order **/
    public static final Descriptor<?> deserialize(final byte[] buf, final boolean swap) throws MdsException {
        if(buf == null) return null;
        return Descriptor.deserialize(Descriptor.Buffer(buf, swap));
    }

    /** Returns the Descriptor deserialized from the given ByteBuffer **/
    public static Descriptor<?> deserialize(final ByteBuffer bi) throws MdsException {
        if(!bi.hasRemaining()) return Missing.NEW;
        final ByteBuffer b = bi.slice().order(bi.order());
        switch(b.get(Descriptor._clsB)){
            case Descriptor_A.CLASS:
                return Descriptor_A.deserialize(b);
            case Descriptor_APD.CLASS:
                return Descriptor_APD.deserialize(b);
            case Descriptor_CA.CLASS:
                return Descriptor_CA.deserialize(b);
            case Descriptor_D.CLASS:
                return Descriptor_S.deserialize(b);
            case Descriptor_R.CLASS:
                return Descriptor_R.deserialize(b);
            case Descriptor_S.CLASS:
                return Descriptor_S.deserialize(b);
            case Descriptor_XD.CLASS:
                return Descriptor_XS.deserialize(b);
            case Descriptor_XS.CLASS:
                return Descriptor_XS.deserialize(b);
        }
        throw new MdsException(String.format("Unsupported class %s", Descriptor.getDClassName(b.get(Descriptor._clsB))), 0);
    }

    /** null safe sloppy decompile of the given Descriptor **/
    public static final String Dsc2String(final Descriptor<?> t) {
        return t == null ? "*" : t.toString();
    }

    protected final static DATA<?>[] getDATAs(final Descriptor<?>... args) throws MdsException {
        final DATA<?>[] data_args = new DATA[args.length];
        for(int i = 0; i < args.length; i++)
            data_args[i] = args[i].getData();
        return data_args;
    }

    /** Returns the element length of the given dtype **/
    public static final short getDataSize(final byte dtype, final int length) {
        switch(dtype){
            default:
            case DTYPE.T:
                return (short)length;
            case DTYPE.BU:
            case DTYPE.B:
                return 1;
            case DTYPE.WU:
            case DTYPE.W:
                return 2;
            case DTYPE.NID:
            case DTYPE.LU:
            case DTYPE.L:
            case DTYPE.F:
            case DTYPE.FS:
                return 4;
            case DTYPE.Q:
            case DTYPE.QU:
            case DTYPE.FC:
            case DTYPE.FSC:
            case DTYPE.D:
            case DTYPE.FT:
            case DTYPE.G:
                return 8;
            case DTYPE.O:
            case DTYPE.OU:
            case DTYPE.DC:
            case DTYPE.FTC:
            case DTYPE.GC:
                return 16;
        }
    }

    /** Returns the name of the given dclass **/
    public static final String getDClassName(final byte dclass) {
        switch(dclass){
            case Descriptor_S.CLASS:
                return "CLASS_S";
            case Descriptor_D.CLASS:
                return "CLASS_D";
            case Descriptor_R.CLASS:
                return "CLASS_R";
            case Descriptor_A.CLASS:
                return "CLASS_A";
            case Descriptor_XS.CLASS:
                return "CLASS_XS";
            case Descriptor_XD.CLASS:
                return "CLASS_XD";
            case Descriptor_CA.CLASS:
                return "CLASS_CA";
            case Descriptor_APD.CLASS:
                return "CLASS_APD";
            default:
                return "CLASS_" + (dclass & 0xFF);
        }
    }

    public static final Descriptor<?> getLocal(final FLAG local, final Descriptor<?> dsc) {
        if(dsc == null || dsc.isLocal()) return dsc;
        final Descriptor<?> ldsc = dsc.getLocal(local);
        return ldsc.setLocal();
    }

    protected final static Descriptor<?>[] getLocals(final FLAG local, final Descriptor<?>... args) {
        final Descriptor<?>[] local_args = new Descriptor<?>[args.length];
        for(int i = 0; i < args.length; i++)
            local_args[i] = Descriptor.getLocal(local, args[i]);
        return local_args;
    }

    public static final boolean isLocal(final Descriptor<?> dsc) {
        if(dsc == null) return true;
        return dsc.isLocal();
    }

    public static final boolean isMissing(final Descriptor<?> dsc) {
        return dsc == null || dsc == Missing.NEW;
    }

    public static final Descriptor<?> NEW(final Object obj) throws MdsException {
        if(obj == null) return Missing.NEW;
        if(obj instanceof String) return new CString((String)obj);
        if(obj instanceof Number) return NUMBER.NEW((Number)obj);
        throw new MdsException("Conversion form " + obj.getClass().getName() + " not yet implemented.");
    }

    /** Returns Descriptor contained in Message **/
    public static Descriptor<?> readMessage(final Message msg) throws MdsException {
        if(msg.header.get(Message._typB) == DTYPE.T) return new CString(msg.body.array());
        return Descriptor_A.readMessage(msg);
    }
    protected TREE             tree;
    protected Mds              mds;
    protected final ByteBuffer b, p;
    protected Descriptor<?>    VALUE;
    private boolean            islocal = false;

    public Descriptor(final ByteBuffer b){
        this.tree = TREE.getActiveTree();
        this.mds = this.tree == null ? Mds.getActiveMds() : this.tree.mds;
        this.b = b.slice().order(b.order());
        this.p = ((ByteBuffer)this.b.duplicate().position(this.pointer() == 0 ? this.b.limit() : this.pointer())).slice().order(this.b.order());
    }

    public Descriptor(final short length, final byte dtype, final byte dclass, final ByteBuffer data, final int pointer, int size){
        this.tree = TREE.getActiveTree();
        this.mds = this.tree == null ? Mds.getActiveMds() : this.tree.mds;
        size += pointer;
        if(data != null) size += data.limit();
        this.b = ByteBuffer.allocate(size).order(Descriptor.BYTEORDER);
        this.b.putShort(Descriptor._lenS, length);
        this.b.put(Descriptor._typB, dtype);
        this.b.put(Descriptor._clsB, dclass);
        if(data == null){
            this.b.putInt(Descriptor._ptrI, 0);
        }else{
            this.b.putInt(Descriptor._ptrI, pointer);
            ((ByteBuffer)this.b.position(pointer)).put((ByteBuffer)data.duplicate().rewind()).rewind();
        }
        this.p = ((ByteBuffer)this.b.duplicate().position(this.pointer() == 0 ? this.b.limit() : this.pointer())).slice().order(this.b.order());
    }

    /** (3,b) descriptor class code **/
    public final byte dclass() {
        return this.b.get(Descriptor._clsB);
    }

    /** Returns the decompiled string **/
    public final String decompile() {
        return this.decompile(Descriptor.P_STMT, new StringBuilder(1024), Descriptor.DECO_NRM).toString();
    }

    /** Core method for decompiling (dummy) **/
    public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        pout.append("<Descriptor(");
        pout.append(this.length() & 0xFFFF).append(',');
        pout.append(this.dtype() & 0xFF).append(',');
        pout.append(this.dclass() & 0xFF).append(',');
        pout.append(this.pointer() & 0xFFFFFFFFl);
        return pout.append(")>");
    }

    /** Returns the decompiled string with first level indentation **/
    public final String decompileX() {
        return this.decompile(Descriptor.P_STMT, new StringBuilder(1024), Descriptor.DECO_X).toString();
    }

    /** (2,b) data type code **/
    public final byte dtype() {
        return this.b.get(Descriptor._typB);
    }

    @Override
    public boolean equals(final Object obj) {
        if(this.getClass() != obj.getClass()) return false;
        final Descriptor<?> that = (Descriptor<?>)obj;
        if(this.length() != that.length()) return false;
        if(this.dclass() != that.dclass()) return false;
        if(this.dtype() != that.dtype()) return false;
        if(this.pointer() != that.pointer()) return false;
        return true;
    }

    /** Evaluates Descriptor remotely and returns result Descriptor **/
    public Descriptor<?> evaluate() {
        if(this instanceof DATA) return this;
        try{
            if(this.isLocal()) return this.evaluate_lib();
            return new TdiShr(this.mds).tdiEvaluate(this);
        }catch(final MdsException e){
            return Missing.NEW;
        }
    }

    public Descriptor<?> evaluate_lib() throws MdsException {
        if(this instanceof DATA) return this;
        if(MdsLib.lib_loaded == null) return Descriptor.mdslib.getDescriptor(this.tree, "EVALUATE($)", this.getLocal());
        return new TdiShr(this.mds).tdiEvaluate(this);
    }

    /** Returns the value<T> of the body directed to by pointer **/
    public abstract T getAtomic();

    /** Returns the value as raw ByteBuffer **/
    public ByteBuffer getBuffer() {
        return this.p.asReadOnlyBuffer().order(this.p.order());
    }

    /**
     * Returns the data of the Descriptor, i.e. DATA($THIS)
     *
     * @throws MdsException
     **/
    public DATA<?> getData() throws MdsException {
        if(this instanceof DATA) return (DATA<?>)this;
        if(MdsLib.lib_loaded == null) return (DATA<?>)Descriptor.mdslib.getDescriptor(this.tree, "DATA($)", this.getLocal());
        return this.getData_();
    }

    @SuppressWarnings("static-method")
    protected DATA<?> getData_() throws MdsException {
        throw DATA.dataerror;
    }

    public Descriptor_A<?> getDataA() throws MdsException {
        return (Descriptor_A<?>)this.getData().toDescriptor();
    }

    public Descriptor<?> getDataD() throws MdsException {
        return this.getData().toDescriptor();
    }

    public Descriptor_S<?> getDataS() throws MdsException {
        return (Descriptor_S<?>)this.getData().toDescriptor();
    }

    /** Returns the dclass name of the Descriptor **/
    public final String getDClassName() {
        return Descriptor.getDClassName(this.dclass());
    }

    /** Returns the value cast to Descriptor **/
    public final Descriptor<?> getDescriptor() throws MdsException {
        return Descriptor.deserialize(this.getBuffer());
    }

    /** Returns the dtype name of the Descriptor **/
    public String getDTypeName() {
        return DTYPE.getName(this.dtype());
    }

    final public Descriptor<?> getLocal() {
        if(this.isLocal()) return this;
        return this.getLocal(null);
    }

    /**
     * Returns the local version of the Descriptor, i.e. it will download all required data
     *
     * @throws MdsException
     **/
    public Descriptor<?> getLocal(final FLAG local) {
        if(this.isLocal()) return this;
        final Descriptor<?> ldsc = this.getLocal_(local);
        return ldsc.setLocal();
    }

    public Descriptor<?> getLocal_(final FLAG local) {
        FLAG.set(local, false);
        return this.evaluate().getLocal();
    }

    /** Returns the shape of the Descriptor, i.e. SHAPE($THIS) **/
    public abstract int[] getShape();

    /** Returns the total size of the backing buffer in bytes **/
    public final int getSize() {
        return this.b.limit();
    }

    @Override
    public int hashCode() {
        return super.hashCode();
    }

    @SuppressWarnings("static-method")
    public boolean isAtomic() {
        return Descriptor.atomic;
    }

    public boolean isLocal() {
        return this.islocal;
    }

    /** (0,s) specific length typically a 16-bit (unsigned) length **/
    public final int length() {
        return this.b.getShort(Descriptor._lenS) & 0xFFFF;
    }

    /** (4,i) address of first byte of data element **/
    public final int pointer() {
        return this.b.getInt(Descriptor._ptrI);
    }

    /** Returns serialized byte stream as ByteBuffer **/
    public final ByteBuffer serialize() {
        return this.b.asReadOnlyBuffer().order(this.b.order());
    }

    /** Returns serialized byte stream as byte[] **/
    public final byte[] serializeArray() {
        if(!this.b.isReadOnly() && this.b.arrayOffset() == 0) return this.b.array();
        return this.serializeArray_copy();
    }

    /** Returns serialized byte stream as byte[] **/
    public final byte[] serializeArray_copy() {
        final ByteBuffer bs = this.serialize();
        return ByteBuffer.allocate(bs.limit()).put(bs).array();
    }

    /** Returns serialized byte stream as Descriptor **/
    public final Int8Array serializeDsc() {
        return new Int8Array(this.serializeArray());
    }

    public Descriptor<?> setLocal() {
        this.islocal = true;
        return this;
    }

    public Descriptor<?> setMds(final Mds mds) {
        this.mds = mds;
        return this;
    }

    public Descriptor<?> setTree(final TREE tree) {
        this.tree = tree;
        if(tree != null) this.mds = tree.mds;
        return this;
    }

    /** Returns value as BigInteger **/
    public BigInteger toBigInteger() {
        return this.toBigIntegerArray()[0];
    }

    /** Returns value as BigInteger[] **/
    public abstract BigInteger[] toBigIntegerArray();

    /** Returns value as byte **/
    public byte toByte() {
        return this.toByteArray()[0];
    }

    /** Returns value as byte[] **/
    public abstract byte[] toByteArray();

    public final Descriptor<T> toDescriptor() {
        return this;
    }

    /** Returns value as double **/
    public double toDouble() {
        return this.toDoubleArray()[0];
    }

    /** Returns value as double[] **/
    public abstract double[] toDoubleArray();

    /** Returns value as float **/
    public float toFloat() {
        return this.toFloatArray()[0];
    }

    /** Returns value as float[] **/
    public abstract float[] toFloatArray();

    /** Returns value as int **/
    public int toInt() {
        return this.toIntArray()[0];
    }

    /** Returns value as int[] **/
    public abstract int[] toIntArray();

    /** Returns value as long **/
    public long toLong() {
        return this.toLongArray()[0];
    }

    /** return value as long[] **/
    public abstract long[] toLongArray();

    /**
     * Returns the MdsIp Message representing this Descriptor
     *
     * @param mid
     * @throws MdsException
     **/
    public Message toMessage(final byte descr_idx, final byte n_args, final byte mid) throws MdsException {
        final DATA<?> data = this.getData();
        return new Message(descr_idx, data.dtype(), n_args, data.getShape(), data.getBuffer(), mid);
    }

    /** Returns value as short **/
    public short toShort() {
        return this.toShortArray()[0];
    }

    /** Returns value as short[] **/
    public abstract short[] toShortArray();

    @Override
    /** Returns a sloppy decompiled string **/
    public String toString() {
        try{
            return this.decompile(Descriptor.P_STMT, new StringBuilder(1024), Descriptor.DECO_STR).toString();
        }catch(final Exception e){
            e.printStackTrace();
            return Descriptor.getDClassName(this.dclass()) + " " + this.getDTypeName();
        }
    }

    /** Returns value as String[] **/
    public abstract String[] toStringArray();

    /** Returns a sloppy decompiled string with first level indentation **/
    public final String toStringX() {
        return this.decompile(Descriptor.P_STMT, new StringBuilder(1024), Descriptor.DECO_STRX).toString();
    }

    public Descriptor<?> VALUE() {
        return this.VALUE.VALUE();
    }

    public final Descriptor<?> VALUE(final Descriptor<?> value) {
        this.VALUE = value;
        return this;
    }
}

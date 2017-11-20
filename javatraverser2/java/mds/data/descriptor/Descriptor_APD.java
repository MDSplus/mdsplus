package mds.data.descriptor;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DTYPE;
import mds.data.descriptor_apd.Dictionary;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_s.Missing;

/** Array of Descriptor (-60 : 196) **/
public class Descriptor_APD extends Descriptor_A<Descriptor<?>>{
    @SuppressWarnings("hiding")
    public static final byte   CLASS = -60;
    public static final String name  = "APD";

    public static Descriptor_APD deserialize(final ByteBuffer b) throws MdsException {
        switch(b.get(Descriptor._typB)){
            case DTYPE.DICTIONARY:
                return new Dictionary(b);
            case DTYPE.LIST:
                return new List(b);
            default:
                return new Descriptor_APD(b);
        }
    }

    private static ByteBuffer makeBuffer(final byte dtype, final Descriptor<?>[] descs, final int[] shape) {
        int offset = shape.length > 1 ? ARRAY._dmsIa : ARRAY._a0I;
        offset += shape.length * Integer.BYTES;
        final int pointer = offset;
        final int arsize = descs.length * Integer.BYTES;
        offset += arsize;
        final int[] dscptrs = new int[descs.length];
        for(int i = 0; i < descs.length; i++){
            dscptrs[i] = offset;
            offset += descs[i].getSize();
        }
        final ByteBuffer b = ByteBuffer.allocate(offset).order(Descriptor.BYTEORDER);
        b.putInt(Descriptor._lenS, (short)4);
        b.putInt(Descriptor._typB, dtype);
        b.putInt(Descriptor._clsB, Descriptor_APD.CLASS);
        b.putInt(Descriptor._ptrI, pointer);
        if(shape.length > 1){
            b.put(ARRAY._dmctB, (byte)shape.length);
            b.putInt(ARRAY._a0I, pointer).position(ARRAY._dmsIa);
            b.asIntBuffer().put(shape);
            b.position(0);
        }else{
            b.put(ARRAY._dmctB, (byte)1);
        }
        b.putInt(ARRAY._aszI, arsize);
        b.position(pointer);
        b.asIntBuffer().put(dscptrs);
        for(int i = 0; i < descs.length; i++){
            b.position(dscptrs[i]);
            b.put(descs[i].b);
        }
        b.rewind();
        return b;
    }

    public Descriptor_APD(final byte dtype, final Descriptor<?>[] descs, final int... shape){
        super(Descriptor_APD.makeBuffer(dtype, descs, shape));
    }

    protected Descriptor_APD(final ByteBuffer b){
        super(b);
    }

    @Override
    protected final StringBuilder decompile(final StringBuilder pout, final Descriptor<?> t) {
        return t == null ? pout.append("*") : t.decompile(Descriptor.P_STMT, pout, Descriptor.DECO_NRM);
    }

    @Override
    public final String getDTypeName() {
        return DTYPE.getName(this.dtype());
    }

    @Override
    public final Descriptor<?> getElement(final ByteBuffer b_ptr) {
        final int dscptr = b_ptr.getInt();
        if(dscptr == 0) return Missing.NEW;
        final int max = this.getLength() * Integer.BYTES;
        int pos = b_ptr.position();
        for(; pos < max && b_ptr.getInt(pos) == 0; pos += Integer.BYTES){/*cond*/}
        final int next = pos < max ? b_ptr.getInt(pos) : this.b.limit();
        try{
            return Descriptor.deserialize(((ByteBuffer)this.b.duplicate().position(dscptr).limit(next)).slice().order(this.b.order())).setTree(this.tree).VALUE(this);
        }catch(final MdsException e){
            e.printStackTrace();
            return Missing.NEW;
        }
    }

    @Override
    public Descriptor<?> getElement(final int i) {
        return this.getElement((ByteBuffer)this.p.duplicate().order(this.p.order()).position(i * Integer.BYTES));
    }

    @Override
    public final Descriptor_APD getLocal_(final FLAG local) {
        final FLAG mylocal = new FLAG();
        final Descriptor<?>[] apd = new Descriptor<?>[this.arsize() / this.length()];
        for(int i = 0; i < apd.length; i++)
            apd[i] = this.getScalar(i).getLocal(mylocal);
        if(FLAG.and(local, mylocal.flag)) return (Descriptor_APD)this.setLocal();
        return (Descriptor_APD)new Descriptor_APD(this.dtype(), apd, this.getShape()).setLocal();
    }

    @Override
    public Descriptor<?> getScalar(final int idx) {
        return this.getElement(idx);
    }

    @Override
    protected final String getSuffix() {
        return "";
    }

    @Override
    protected final Descriptor<?>[] initArray(final int size) {
        return new Descriptor<?>[size];
    }

    @Override
    public boolean isAtomic() {
        return false;
    }

    @Override
    protected final void setElement(final ByteBuffer b, final Descriptor<?> value) {/*TODO:stub*/}

    @Override
    protected void setElement(final int i, final Descriptor<?> value) {/*TODO:stub*/}

    @Override
    public final BigInteger toBigInteger(final Descriptor<?> t) {
        return t.toBigInteger();
    }

    public final BigInteger[] toBigIntegerArray(final int idx) {
        return this.getElement(idx).toBigIntegerArray();
    }

    @Override
    public final byte toByte(final Descriptor<?> t) {
        return t.toByte();
    }

    public final byte[] toByteArray(final int idx) {
        return this.getElement(idx).toByteArray();
    }

    @Override
    public final double toDouble(final Descriptor<?> t) {
        return t.toDouble();
    }

    public final double[] toDoubleArray(final int idx) {
        return this.getElement(idx).toDoubleArray();
    }

    @Override
    public final float toFloat(final Descriptor<?> t) {
        return t.toFloat();
    }

    public final float[] toFloatArray(final int idx) {
        return this.getElement(idx).toFloatArray();
    }

    @Override
    public final int toInt(final Descriptor<?> t) {
        return t.toInt();
    }

    public final int[] toIntArray(final int idx) {
        return this.getElement(idx).toIntArray();
    }

    @Override
    public final long toLong(final Descriptor<?> t) {
        return t.toLong();
    }

    public final long[] toLongArray(final int idx) {
        return this.getElement(idx).toLongArray();
    }

    @Override
    public final short toShort(final Descriptor<?> t) {
        return t.toShort();
    }

    public final short[] toShortArray(final int idx) {
        return this.getElement(idx).toShortArray();
    }

    @Override
    public final String toString(final Descriptor<?> t) {
        return Descriptor.Dsc2String(t);
    }

    public final String[] toStringArray(final int idx) {
        return this.getElement(idx).toStringArray();
    }
}
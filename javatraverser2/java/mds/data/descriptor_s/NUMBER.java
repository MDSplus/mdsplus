package mds.data.descriptor_s;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_S;
import mds.data.descriptor_r.function.CONST;

public abstract class NUMBER<T extends Number>extends Descriptor_S<T> implements DATA<T>{
    public static final BigInteger max128 = BigInteger.ONE.shiftLeft(128);

    public static final NUMBER<? extends Number> NEW(final Number value) {
        return NUMBER.NEW(value, true);
    }

    public static final NUMBER<? extends Number> NEW(final Number value, final boolean signed) {
        if(value instanceof Integer) return new Int32(value.intValue());
        if(value instanceof Long) return new Int64(value.longValue());
        if(value instanceof Float) return new Float32(value.floatValue());
        if(value instanceof Double) return new Float64(value.doubleValue());
        if(value instanceof Short) return new Int16(value.shortValue());
        if(value instanceof Byte) return new Int8(value.byteValue());
        if(value instanceof BigInteger) return new Int128((BigInteger)value);
        return new Float32(value.floatValue());// default
    }

    public NUMBER(final byte dtype, final ByteBuffer b){
        super(dtype, b);
    }

    protected NUMBER(final ByteBuffer b){
        super(b);
    }

    @Override
    public abstract NUMBER<?> abs();

    @Override
    public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return pout.append(this.getAtomic()).append(this.getSuffix());
    }

    @Override
    public final DATA<T> getData_() throws MdsException {
        return this;
    }

    @Override
    public Descriptor<?> getLocal_(final FLAG local) {
        return this;
    }

    @Override
    public final byte getRank() {
        return (byte)(this.getRankClass() | (this.getRankBits() - 1));
    }

    protected abstract byte getRankBits();

    protected abstract byte getRankClass();

    protected final String getSuffix() {
        return DTYPE.getSuffix(this.dtype());
    }

    @Override
    public abstract NUMBER<?> inot();

    @Override
    public final boolean isLocal() {
        return true;
    }

    @Override
    public abstract NUMBER<?> neg();

    protected final NUMBER<?> newType(final double value) {
        try{
            return this.getClass().getConstructor(double.class).newInstance(Double.valueOf(value));
        }catch(final Exception e){
            return CONST.ROPRAND;
        }
    }

    protected final NUMBER<?> newType(final int value) {
        try{
            return this.getClass().getConstructor(int.class).newInstance(Integer.valueOf(value));
        }catch(final Exception e){
            return CONST.ROPRAND;
        }
    }

    public abstract T parse(final String in);

    @Override
    public NUMBER<?> power(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        return this.newType(Math.pow(X.toDouble(), Y.toDouble()));
    }

    @Override
    public CString text() {// 4,8,12,20,36
        return new CString((int)((this.length() - 1) * 8 * .30103 + 6.8), this.getAtomic().toString());
    }

    @Override
    public final byte toByte() {
        return this.getAtomic().byteValue();
    }

    @Override
    public final byte[] toByteArray() {
        return new byte[]{this.toByte()};
    }

    @Override
    public final double toDouble() {
        return this.getAtomic().doubleValue();
    }

    @Override
    public final double[] toDoubleArray() {
        return new double[]{this.toDouble()};
    }

    @Override
    public final float toFloat() {
        return this.getAtomic().floatValue();
    }

    @Override
    public final float[] toFloatArray() {
        return new float[]{this.toFloat()};
    }

    @Override
    public final int toInt() {
        return this.getAtomic().intValue();
    }

    @Override
    public final int[] toIntArray() {
        return new int[]{this.toInt()};
    }

    @Override
    public final long toLong() {
        return this.getAtomic().longValue();
    }

    @Override
    public final long[] toLongArray() {
        return new long[]{this.toLong()};
    }

    @Override
    public final short toShort() {
        return this.getAtomic().shortValue();
    }

    @Override
    public final short[] toShortArray() {
        return new short[]{this.toShort()};
    }

    @Override
    public final String[] toStringArray() {
        return new String[]{this.toString()};
    }
}

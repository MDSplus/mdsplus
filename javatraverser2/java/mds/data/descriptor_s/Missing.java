package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.data.DATA;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_S;

public final class Missing extends Descriptor_S<Object> implements DATA<Object>{
    public static final Missing NEW = new Missing();

    private Missing(){
        super(ByteBuffer.wrap(new byte[]{4, 0, 0, 1, 8, 0, 0, 0, 0, 0, 0, 0}).order(Descriptor.BYTEORDER));
    }

    @Override
    public final Missing abs() {
        return this;
    }

    @Override
    public Descriptor<?> add(final Descriptor<?> X, final Descriptor<?> Y) {
        return this;
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return pout.append('*');
    }

    @Override
    public Descriptor<?> divide(final Descriptor<?> X, final Descriptor<?> Y) {
        return this;
    }

    @Override
    public final Object getAtomic() {
        return null;
    }

    @Override
    public final DATA<Object> getData_() {
        return this;
    }

    @Override
    public Descriptor<?> getLocal_(final FLAG local) {
        return this;
    }

    @Override
    public final byte getRank() {
        return -1;
    }

    @Override
    public final Missing inot() {
        return this;
    }

    @Override
    public final boolean isLocal() {
        return true;
    }

    @Override
    public Descriptor<?> multiply(final Descriptor<?> X, final Descriptor<?> Y) {
        return this;
    }

    @Override
    public final Missing neg() {
        return this;
    }

    @Override
    public Descriptor<?> power(final Descriptor<?> X, final Descriptor<?> Y) {
        return this;
    }

    @Override
    public Descriptor<?> shiftleft(final Descriptor<?> X) {
        return this;
    }

    @Override
    public Descriptor<?> shiftright(final Descriptor<?> X) {
        return this;
    }

    @Override
    public Descriptor<?> subtract(final Descriptor<?> X, final Descriptor<?> Y) {
        return this;
    }

    @Override
    public final CString text() {
        return new CString("");
    }

    @Override
    public final byte[] toByteArray() {
        return new byte[0];
    }

    @Override
    public final double[] toDoubleArray() {
        return new double[0];
    }

    @Override
    public final float[] toFloatArray() {
        return new float[0];
    }

    @Override
    public final int[] toIntArray() {
        return new int[0];
    }

    @Override
    public final long[] toLongArray() {
        return new long[0];
    }

    @Override
    public final short[] toShortArray() {
        return new short[0];
    }

    @Override
    public final String toString() {
        return null;
    }
}

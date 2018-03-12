package mds.data.descriptor_a;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Uint128;

public final class Uint128Array extends INTEGER_UNSIGNEDArray<BigInteger>{
    public Uint128Array(final BigInteger... values){
        super(DTYPE.OU, values);
    }

    public Uint128Array(final byte dtype, final ByteBuffer data, final int shape[]){
        super(DTYPE.OU, data, shape);
    }

    public Uint128Array(final ByteBuffer b){
        super(b);
    }

    public Uint128Array(final int shape[], final BigInteger... values){
        super(DTYPE.OU, values, shape);
    }

    public Uint128Array(final int shape[], final long... values){
        super(DTYPE.OU, INTEGERArray.long2BigInt(true, values), shape);
    }

    @Override
    public final Uint128Array abs() {
        return this;
    }

    @Override
    public ByteBuffer buildBuffer(final ByteBuffer buf, final int value) {
        return null;
    }

    @Override
    public final BigInteger getElement(final ByteBuffer b_in) {
        return Uint128.getBigInteger(b_in);
    }

    @Override
    public final BigInteger getElement(final int idx) {
        return Uint128.getBigInteger(this.p, idx);
    }

    @Override
    protected final byte getRankBits() {
        return 16;
    }

    @Override
    public Uint128 getScalar(final int idx) {
        return new Uint128(this.getElement(idx));
    }

    @Override
    protected final BigInteger[] initArray(final int size) {
        return new BigInteger[size];
    }

    @Override
    public final Uint128Array inot() {
        final BigInteger[] values = this.getAtomic();
        for(int i = 0; i < values.length; i++)
            values[i] = values[i].xor(Uint128.FFx16);
        return new Uint128Array(values);
    }

    @Override
    public final Int128Array neg() {
        final BigInteger[] values = this.getAtomic();
        for(int i = 0; i < values.length; i++)
            values[i] = values[i].negate();
        return new Int128Array(values);
    }

    @Override
    public final Int128Array octawords() {
        return new Int128Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.O));
    }

    @Override
    public final Uint128Array octawordu() {
        return this;
    }

    @Override
    public final BigInteger parse(final String in) {
        return new BigInteger(in).abs().setBit(128);
    }

    @Override
    protected final void setElement(final ByteBuffer b, final BigInteger value) {
        Uint128.putBigInteger(b, value);
    }

    @Override
    protected final void setElement(final int i, final BigInteger value) {
        Uint128.putBigInteger(this.b, i, value);
    }
}

package mds.data.descriptor_s;

import java.math.BigInteger;
import java.nio.ByteBuffer;

public abstract class INTEGER_UNSIGNED<T extends Number>extends INTEGER<T>{
    public INTEGER_UNSIGNED(final byte ou, final BigInteger value){
        super(ou, value);
    }

    public INTEGER_UNSIGNED(final byte bu, final byte value){
        super(bu, value);
    }

    public INTEGER_UNSIGNED(final byte lu, final int value){
        super(lu, value);
    }

    public INTEGER_UNSIGNED(final byte qu, final long value){
        super(qu, value);
    }

    public INTEGER_UNSIGNED(final byte wu, final short value){
        super(wu, value);
    }

    protected INTEGER_UNSIGNED(final ByteBuffer b){
        super(b);
    }

    @Override
    public final INTEGER_UNSIGNED<?> abs() {
        return this;
    }

    @Override
    protected final byte getRankClass() {
        return 0x00;
    }
}

package mds.data.descriptor_a;

import java.math.BigInteger;
import java.nio.ByteBuffer;

public abstract class INTEGER_UNSIGNEDArray<T extends Number>extends INTEGERArray<T>{
    protected INTEGER_UNSIGNEDArray(final byte dtype, final BigInteger[] values, final int... shape){
        super(dtype, values, shape);
    }

    protected INTEGER_UNSIGNEDArray(final byte dtype, final byte[] values, final int... shape){
        super(dtype, values, shape);
    }

    public INTEGER_UNSIGNEDArray(final byte dtype, final ByteBuffer data, final int[] shape){
        super(dtype, data, shape);
    }

    protected INTEGER_UNSIGNEDArray(final byte dtype, final int[] values, final int... shape){
        super(dtype, values, shape);
    }

    protected INTEGER_UNSIGNEDArray(final byte dtype, final long[] values, final int... shape){
        super(dtype, values, shape);
    }

    protected INTEGER_UNSIGNEDArray(final byte dtype, final short[] values, final int... shape){
        super(dtype, values, shape);
    }

    protected INTEGER_UNSIGNEDArray(final ByteBuffer b){
        super(b);
    }

    @Override
    protected final byte getRankClass() {
        return 0x00;
    }
}

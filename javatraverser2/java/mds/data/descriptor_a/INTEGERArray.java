/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
package mds.data.descriptor_a;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.function.BINARY.Add.long_add;
import mds.data.descriptor_r.function.BINARY.Divide.long_divide;
import mds.data.descriptor_r.function.BINARY.Multiply.long_multiply;
import mds.data.descriptor_r.function.BINARY.Subtract.long_subtract;
import mds.data.descriptor_s.NUMBER;

public abstract class INTEGERArray<T extends Number>extends NUMBERArray<T>{
    protected static final BigInteger[] long2BigInt(final boolean unsigned, final long... in) {
        final BigInteger[] out = new BigInteger[in.length];
        for(int i = 0; i < in.length; i++)
            out[i] = BigInteger.valueOf(in[i]);
        return out;
    }

    private static final ByteBuffer toByteBuffer(final BigInteger[] values) {
        final ByteBuffer b = ByteBuffer.allocate(values.length * 16).order(Descriptor.BYTEORDER);
        for(final BigInteger value : values)
            b.put(value.or(NUMBER.max128).toByteArray(), 0, 16);
        return b;
    }

    private static final ByteBuffer toByteBuffer(final int[] values) {
        final ByteBuffer b = ByteBuffer.allocate(values.length * Integer.BYTES).order(Descriptor.BYTEORDER);
        b.asIntBuffer().put(values);
        return b;
    }

    private static final ByteBuffer toByteBuffer(final long[] values) {
        final ByteBuffer b = ByteBuffer.allocate(values.length * Long.BYTES).order(Descriptor.BYTEORDER);
        b.asLongBuffer().put(values);
        return b;
    }

    private static final ByteBuffer toByteBuffer(final short[] values) {
        final ByteBuffer b = ByteBuffer.allocate(values.length * Short.BYTES).order(Descriptor.BYTEORDER);
        b.asShortBuffer().put(values);
        return b;
    }

    protected INTEGERArray(final byte dtype, final BigInteger[] values, final int... shape){
        super(dtype, INTEGERArray.toByteBuffer(values), shape.length == 0 ? new int[]{values.length} : shape);
    }

    protected INTEGERArray(final byte dtype, final byte[] values, final int... shape){
        super(dtype, ByteBuffer.wrap(values).order(Descriptor.BYTEORDER), shape.length == 0 ? new int[]{values.length} : shape);
    }

    public INTEGERArray(final byte dtype, final ByteBuffer data, final int[] shape){
        super(dtype, data, shape);
    }

    protected INTEGERArray(final byte dtype, final int[] values, final int... shape){
        super(dtype, INTEGERArray.toByteBuffer(values), shape.length == 0 ? new int[]{values.length} : shape);
    }

    protected INTEGERArray(final byte dtype, final long[] values, final int... shape){
        super(dtype, INTEGERArray.toByteBuffer(values), shape.length == 0 ? new int[]{values.length} : shape);
    }

    protected INTEGERArray(final byte dtype, final short[] values, final int... shape){
        super(dtype, INTEGERArray.toByteBuffer(values), shape.length == 0 ? new int[]{values.length} : shape);
    }

    protected INTEGERArray(final ByteBuffer b){
        super(b);
    }

    @Override
    public Descriptor<?> add(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        return this.long_binary(X, Y, new long_add());
    }

    @Override
    public ByteBuffer buildBuffer(final ByteBuffer buf, final double value) {
        return this.buildBuffer(buf, (int)value);
    }

    public abstract ByteBuffer buildBuffer(final ByteBuffer buf, final int value);

    @Override
    public Descriptor<?> divide(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        return this.long_binary(X, Y, new long_divide());
    }

    @Override
    protected byte getRankClass() {
        return 0x10;
    }

    @Override
    public Descriptor<?> multiply(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        return this.long_binary(X, Y, new long_multiply());
    }

    @Override
    public Descriptor<?> subtract(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        return this.long_binary(X, Y, new long_subtract());
    }
}

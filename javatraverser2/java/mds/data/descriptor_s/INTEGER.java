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
package mds.data.descriptor_s;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.data.descriptor.Descriptor;

public abstract class INTEGER<T extends Number>extends NUMBER<T>{
    private static ByteBuffer toByteBuffer(final BigInteger value) {
        final byte[] ba = value.or(NUMBER.max128).toByteArray();
        final ByteBuffer buf = ByteBuffer.allocate(16).order(Descriptor.BYTEORDER);
        for(int i = 17; i-- > 1;)
            buf.put(ba[i]);
        return buf;
    }

    private static final ByteBuffer toByteBuffer(final byte value) {
        return ByteBuffer.allocate(Byte.BYTES).order(Descriptor.BYTEORDER).put(0, value);
    }

    static final ByteBuffer toByteBuffer(final int value) {
        return ByteBuffer.allocate(Integer.BYTES).order(Descriptor.BYTEORDER).putInt(0, value);
    }

    private static final ByteBuffer toByteBuffer(final long value) {
        return ByteBuffer.allocate(Long.BYTES).order(Descriptor.BYTEORDER).putLong(0, value);
    }

    static final ByteBuffer toByteBuffer(final short value) {
        return ByteBuffer.allocate(Short.BYTES).order(Descriptor.BYTEORDER).putShort(0, value);
    }

    protected INTEGER(final byte dtype, final BigInteger value){
        super(dtype, INTEGER.toByteBuffer(value));
    }

    protected INTEGER(final byte dtype, final byte value){
        super(dtype, INTEGER.toByteBuffer(value));
    }

    protected INTEGER(final byte dtype, final int value){
        super(dtype, INTEGER.toByteBuffer(value));
    }

    protected INTEGER(final byte dtype, final long value){
        super(dtype, INTEGER.toByteBuffer(value));
    }

    protected INTEGER(final byte dtype, final short value){
        super(dtype, INTEGER.toByteBuffer(value));
    }

    protected INTEGER(final ByteBuffer b){
        super(b);
    }

    @Override
    public NUMBER<?> add(final Descriptor<?> X, final Descriptor<?> Y) {
        return this.newType(X.toInt() + Y.toInt());
    }

    @Override
    public NUMBER<?> divide(final Descriptor<?> X, final Descriptor<?> Y) {
        return this.newType(X.toInt() / Y.toInt());
    }

    @Override
    protected byte getRankClass() {
        return 0x10;
    }

    @Override
    public NUMBER<?> multiply(final Descriptor<?> X, final Descriptor<?> Y) {
        return this.newType(X.toInt() * Y.toInt());
    }

    @Override
    public INTEGER<?> shiftleft(final Descriptor<?> X) {
        return (INTEGER<?>)this.newType(this.toInt() << X.toInt());
    }

    @Override
    public INTEGER<?> shiftright(final Descriptor<?> X) {
        return (INTEGER<?>)this.newType(this.toInt() >> X.toInt());
    }

    @Override
    public NUMBER<?> subtract(final Descriptor<?> X, final Descriptor<?> Y) {
        return this.newType(X.toInt() - Y.toInt());
    }
}

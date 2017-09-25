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
import java.nio.ByteOrder;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;

public final class Int128 extends INTEGER<BigInteger>{
    public static BigInteger getBigInteger(final ByteBuffer b) {
        final byte[] buf = new byte[16];
        if(b.order() == ByteOrder.BIG_ENDIAN) b.get(buf);
        else for(int i = 16; i-- > 0;)
            buf[i] = b.get();
        return new BigInteger(buf);
    }

    public static BigInteger getBigInteger(final ByteBuffer b, final int idx) {
        return Int128.getBigInteger((ByteBuffer)b.duplicate().order(b.order()).position(16 * idx));
    }

    public static void putBigInteger(final ByteBuffer b, final BigInteger value) {
        final byte[] buf = new byte[16], bbuf = value.toByteArray();
        System.arraycopy(bbuf, 0, buf, 0, bbuf.length > 16 ? 16 : bbuf.length);
        if(b.order() == ByteOrder.BIG_ENDIAN) b.put(buf);
        else for(int i = 16; i-- > 0;)
            b.put(buf[i]);
    }

    public static void putBigInteger(final ByteBuffer b, final int idx, final BigInteger value) {
        Int128.putBigInteger((ByteBuffer)b.duplicate().order(b.order()).position(16 * idx), value);
    }

    public Int128(){
        this(0);
    }

    public Int128(final BigInteger value){
        super(DTYPE.O, value);
    }

    public Int128(final ByteBuffer b){
        super(b);
    }

    public Int128(final long value){
        this(BigInteger.valueOf(value));
    }

    @Override
    public final Uint128 abs() {
        return new Uint128(this.getValue().abs());
    }

    @Override
    public final Int128 add(final Descriptor<?> X, final Descriptor<?> Y) {
        return new Int128(X.toBigInteger().add(Y.toBigInteger()));
    }

    @Override
    public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return pout.append(String.format("0X%x", this.getAtomic())).append(this.getSuffix());
    }

    @Override
    public final Int128 divide(final Descriptor<?> X, final Descriptor<?> Y) {
        return new Int128(X.toBigInteger().divide(Y.toBigInteger()));
    }

    @Override
    public final BigInteger getAtomic() {
        return Int128.getBigInteger(this.p, 0);
    }

    @Override
    protected final byte getRankBits() {
        return 16;
    }

    public final BigInteger getValue() {
        return this.getAtomic();
    }

    @Override
    public final Uint128 inot() {
        return new Uint128(this.getValue().xor(Uint128.FFx16));
    }

    @Override
    public Int128 multiply(final Descriptor<?> X, final Descriptor<?> Y) {
        return new Int128(X.toBigInteger().multiply(Y.toBigInteger()));
    }

    @Override
    public final Int128 neg() {
        return new Int128(this.getValue().negate());
    }

    @Override
    public final Int128 octawords() {
        return this;
    }

    @Override
    public final Uint128 octawordu() {
        return new Uint128(this.getValue());
    }

    @Override
    public final BigInteger parse(final String in) {
        return new BigInteger(in).setBit(128);
    }

    @Override
    public Int128 power(final Descriptor<?> X, final Descriptor<?> Y) {
        return new Int128(X.toBigInteger().pow(Y.toInt()));
    }

    public final void setValue(final BigInteger value) {
        ((ByteBuffer)this.b.duplicate().position(this.pointer())).put(value.setBit(127).toByteArray());
    }

    @Override
    public Int128 shiftleft(final Descriptor<?> X) {
        return new Int128(this.getAtomic().shiftLeft(X.toInt()));
    }

    @Override
    public Int128 shiftright(final Descriptor<?> X) {
        return new Int128(this.getAtomic().shiftRight(X.toInt()));
    }

    @Override
    public Int128 subtract(final Descriptor<?> X, final Descriptor<?> Y) {
        return new Int128(X.toBigInteger().subtract(Y.toBigInteger()));
    }

    @Override
    public final CString text() {
        return new CString(String.format("0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", this.getAtomic().toByteArray()));
    }

    @Override
    public final BigInteger toBigInteger() {
        return this.getAtomic();
    }
}

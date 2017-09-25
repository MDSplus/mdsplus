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
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Uint64.ULong;

@SuppressWarnings("serial")
public final class Uint64 extends INTEGER_UNSIGNED<ULong>{
    public static final class ULong extends Number{
        private static final BigInteger max = BigInteger.ONE.shiftLeft(64);

        public static ULong decode(final String in) {
            return new ULong(new BigInteger(in).longValue());
        }

        public static final ULong fromBuffer(final ByteBuffer b) {
            return new ULong(b.getLong());
        }

        public static final ULong fromBuffer(final ByteBuffer b, final int idx) {
            return new ULong(b.getLong(idx * Long.BYTES));
        }
        private final long value;

        public ULong(final long value){
            this.value = value;
        }

        @Override
        public final double doubleValue() {
            if(this.value >= 0) return this.value;
            return BigInteger.valueOf(this.value).add(ULong.max).doubleValue();
        }

        @Override
        public final float floatValue() {
            if(this.value >= 0) return this.value;
            return BigInteger.valueOf(this.value).add(ULong.max).floatValue();
        }

        @Override
        public final int intValue() {
            return (int)this.value;
        }

        @Override
        public final long longValue() {
            return this.value;
        }

        @Override
        public final String toString() {
            return Long.toUnsignedString(this.value);
        }
    }

    public Uint64(){
        this(0);
    }

    public Uint64(final ByteBuffer b){
        super(b);
    }

    public Uint64(final double value){
        this((long)value);
    }

    public Uint64(final int value){
        this(value & 0xFFFFFFFFl);
    }

    public Uint64(final long value){
        super(DTYPE.QU, value);
    }

    public Uint64(final ULong value){
        this(value.value);
    }

    @Override
    public Uint64 add(final Descriptor<?> X, final Descriptor<?> Y) {
        return new Uint64(X.toLong() + Y.toLong());
    }

    @Override
    public Uint64 divide(final Descriptor<?> X, final Descriptor<?> Y) {
        return new Uint64(X.toLong() / Y.toLong());
    }

    @Override
    public final ULong getAtomic() {
        return ULong.fromBuffer(this.p, 0);
    }

    @Override
    protected final byte getRankBits() {
        return 8;
    }

    @Override
    public final Uint64 inot() {
        return new Uint64(~this.getAtomic().value);
    }

    @Override
    public Uint64 multiply(final Descriptor<?> X, final Descriptor<?> Y) {
        return new Uint64(X.toLong() * Y.toLong());
    }

    @Override
    public final Int64 neg() {
        return new Int64(-this.getAtomic().value);
    }

    @Override
    public final ULong parse(final String in) {
        return ULong.decode(in);
    }

    @Override
    public Uint64 quadwordu() {
        return this;
    }

    public final void setValue(final long value) {
        this.b.putLong(this.pointer(), value);
    }

    @Override
    public Uint64 shiftleft(final Descriptor<?> X) {
        return new Uint64(this.toLong() << X.toInt());
    }

    @Override
    public Uint64 shiftright(final Descriptor<?> X) {
        return new Uint64(this.toLong() >> X.toInt());
    }

    @Override
    public Uint64 subtract(final Descriptor<?> X, final Descriptor<?> Y) {
        return new Uint64(X.toLong() - Y.toLong());
    }
}

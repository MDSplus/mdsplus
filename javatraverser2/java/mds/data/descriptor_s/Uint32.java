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

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor_s.Uint32.UInteger;

@SuppressWarnings("serial")
public final class Uint32 extends INTEGER_UNSIGNED<UInteger>{
    public static final class UInteger extends Number{
        public static UInteger decode(final String in) {
            return new UInteger(Long.decode(in).intValue());
        }

        public static final UInteger fromBuffer(final ByteBuffer b) {
            return new UInteger(b.getInt());
        }

        public static final UInteger fromBuffer(final ByteBuffer b, final int idx) {
            return new UInteger(b.getInt(idx * Integer.BYTES));
        }
        private final int value;

        public UInteger(final int value){
            this.value = value;
        }

        @Override
        public final double doubleValue() {
            return this.value & 0xFFFFFFFFl;
        }

        @Override
        public final float floatValue() {
            return this.value & 0xFFFFFFFFl;
        }

        @Override
        public final int intValue() {
            return this.value;
        }

        @Override
        public final long longValue() {
            return this.value & 0xFFFFFFFFl;
        }

        @Override
        public final String toString() {
            return Long.toString(Integer.toUnsignedLong(this.value));
        }
    }

    public Uint32(){
        this(0);
    }

    public Uint32(final ByteBuffer b){
        super(b);
    }

    public Uint32(final double value){
        this((int)value);
    }

    public Uint32(final int value){
        super(DTYPE.LU, value);
    }

    public Uint32(final UInteger value){
        this(value.value);
    }

    @Override
    public final UInteger getAtomic() {
        return UInteger.fromBuffer(this.p, 0);
    }

    @Override
    protected final byte getRankBits() {
        return 4;
    }

    @Override
    public final Uint32 inot() {
        return new Uint32(~this.getAtomic().value);
    }

    @Override
    public final Uint32 longu() {
        return this;
    }

    @Override
    public final Int32 neg() {
        return new Int32(-this.getAtomic().value);
    }

    @Override
    public final UInteger parse(final String in) {
        return UInteger.decode(in);
    }

    public final void setValue(final int value) {
        this.b.putInt(this.pointer(), value);
    }
}

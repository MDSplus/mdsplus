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
import mds.data.descriptor_s.Uint8.UByte;

@SuppressWarnings("serial")
public final class Uint8 extends INTEGER_UNSIGNED<UByte>{
    public static final class UByte extends Number{
        public static UByte decode(final String in) {
            return new UByte(Short.decode(in).byteValue());
        }

        public static final UByte fromBuffer(final ByteBuffer b) {
            return new UByte(b.get());
        }

        public static final UByte fromBuffer(final ByteBuffer b, final int idx) {
            return new UByte(b.get(idx * Byte.BYTES));
        }
        private final byte value;

        public UByte(final byte value){
            this.value = value;
        }

        @Override
        public final double doubleValue() {
            return this.value & 0xFF;
        }

        @Override
        public final float floatValue() {
            return this.value & 0xFF;
        }

        @Override
        public final int intValue() {
            return this.value & 0xFF;
        }

        @Override
        public final long longValue() {
            return this.value & 0xFFl;
        }

        @Override
        public final String toString() {
            return Integer.toString(this.value & 0xFF);
        }
    }

    public Uint8(){
        this(0);
    }

    public Uint8(final byte value){
        super(DTYPE.BU, value);
    }

    public Uint8(final ByteBuffer b){
        super(b);
    }

    public Uint8(final double value){
        this((int)value);
    }

    public Uint8(final int value){
        this((byte)(value & 0xFF));
    }

    public Uint8(final UByte value){
        this(value.value);
    }

    @Override
    public final Uint8 byteu() {
        return this;
    }

    @Override
    public final UByte getAtomic() {
        return UByte.fromBuffer(this.p, 0);
    }

    @Override
    protected final byte getRankBits() {
        return 1;
    }

    @Override
    public final Uint8 inot() {
        return new Uint8(~this.getAtomic().value);
    }

    @Override
    public final Int8 neg() {
        return new Int8(-this.getAtomic().value);
    }

    @Override
    public final UByte parse(final String in) {
        return UByte.decode(in);
    }

    public final void setValue(final byte value) {
        this.b.put(this.pointer(), value);
    }
}
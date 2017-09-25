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
import mds.data.descriptor_s.Uint16.UShort;

@SuppressWarnings("serial")
public final class Uint16 extends INTEGER_UNSIGNED<UShort>{
    public static final class UShort extends Number{
        public static UShort decode(final String in) {
            return new UShort(Integer.decode(in).shortValue());
        }

        public static final UShort fromBuffer(final ByteBuffer b) {
            return new UShort(b.getShort());
        }

        public static final UShort fromBuffer(final ByteBuffer b, final int idx) {
            return new UShort(b.getShort(idx * Short.BYTES));
        }
        private final short value;

        public UShort(final short value){
            this.value = value;
        }

        @Override
        public final double doubleValue() {
            return this.value & 0xFFFF;
        }

        @Override
        public final float floatValue() {
            return this.value & 0xFFFF;
        }

        @Override
        public final int intValue() {
            return this.value & 0xFFFF;
        }

        @Override
        public final long longValue() {
            return this.value & 0xFFFFl;
        }

        @Override
        public final String toString() {
            return Integer.toString(this.value & 0xFFFF);
        }
    }

    public Uint16(){
        this(0);
    }

    public Uint16(final ByteBuffer b){
        super(b);
    }

    public Uint16(final double value){
        this((int)value);
    }

    public Uint16(final int value){
        this((short)(value & 0xFFFF));
    }

    public Uint16(final short value){
        super(DTYPE.WU, value);
    }

    public Uint16(final UShort value){
        this(value.value);
    }

    @Override
    public final UShort getAtomic() {
        return UShort.fromBuffer(this.p, 0);
    }

    @Override
    protected final byte getRankBits() {
        return 2;
    }

    public final UShort getValue() {
        return this.getAtomic();
    }

    @Override
    public final Uint16 inot() {
        return new Uint16(~this.getAtomic().value);
    }

    @Override
    public final Int16 neg() {
        return new Int16(-this.getAtomic().value);
    }

    @Override
    public final UShort parse(final String in) {
        return UShort.decode(in);
    }

    public final void setValue(final short value) {
        this.b.putShort(this.pointer(), value);
    }

    @Override
    public final Uint16 wordu() {
        return this;
    }
}

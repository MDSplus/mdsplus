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

public final class Int8 extends INTEGER<Byte>{
    public Int8(){
        this(0);
    }

    public Int8(final byte value){
        super(DTYPE.B, value);
    }

    public Int8(final ByteBuffer b){
        super(b);
    }

    public Int8(final double value){
        this((byte)value);
    }

    public Int8(final int value){
        this((byte)value);
    }

    @Override
    public final Uint8 abs() {
        return new Uint8((byte)(java.lang.Math.abs(this.getPrimitive()) & 0xFF));
    }

    @Override
    public final Int8 bytes() {
        return this;
    }

    @Override
    public final Byte getAtomic() {
        return new Byte(this.p.get(0));
    }

    public final byte getPrimitive() {
        return this.p.get(0);
    }

    @Override
    protected final byte getRankBits() {
        return 1;
    }

    @Override
    public final Uint8 inot() {
        return new Uint8((byte)(~this.getPrimitive() & 0xFF));
    }

    @Override
    public final Int8 neg() {
        return new Int8((byte)(-this.getPrimitive() & 0xFF));
    }

    @Override
    public final Byte parse(final String in) {
        return Byte.decode(in);
    }

    public final void setValue(final byte value) {
        this.b.put(this.pointer(), value);
    }
}

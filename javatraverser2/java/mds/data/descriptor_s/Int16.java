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

public final class Int16 extends INTEGER<Short>{
    public Int16(){
        this(0);
    }

    public Int16(final ByteBuffer b){
        super(b);
    }

    public Int16(final double value){
        this((short)value);
    }

    public Int16(final int value){
        this((short)value);
    }

    public Int16(final short value){
        super(DTYPE.W, value);
    }

    @Override
    public final Uint16 abs() {
        return new Uint16(java.lang.Math.abs(this.getValue()));
    }

    @Override
    public final Short getAtomic() {
        return new Short(this.p.getShort(0));
    }

    @Override
    protected final byte getRankBits() {
        return 2;
    }

    public final short getValue() {
        return this.getBuffer().getShort(0);
    }

    @Override
    public final Uint16 inot() {
        return new Uint16(~this.getValue());
    }

    @Override
    public final Int16 neg() {
        return new Int16(-this.getValue());
    }

    @Override
    public final Short parse(final String in) {
        return Short.decode(in);
    }

    public final void setValue(final short value) {
        this.b.putShort(this.pointer(), value);
    }

    @Override
    public final Int16 words() {
        return this;
    }
}

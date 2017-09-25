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

public class Int32 extends INTEGER<Integer>{
    public Int32(){
        this(0);
    }

    protected Int32(final byte dtype, final int value){
        super(dtype, value);
    }

    public Int32(final ByteBuffer b){
        super(b);
    }

    public Int32(final double value){
        this((int)value);
    }

    public Int32(final int value){
        super(DTYPE.L, value);
    }

    @Override
    public final Uint32 abs() {
        return new Uint32(Math.abs(this.getValue()));
    }

    @Override
    public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return pout.append(this.getAtomic());
    }

    @Override
    public final Integer getAtomic() {
        return new Integer(this.p.getInt(0));
    }

    @Override
    protected final byte getRankBits() {
        return 4;
    }

    public final int getValue() {
        return this.getBuffer().getInt(0);
    }

    @Override
    public Uint32 inot() {
        return new Uint32(~this.getValue());
    }

    @Override
    public final Int32 longs() {
        return this;
    }

    @Override
    public final Int32 neg() {
        return new Int32(-this.getValue());
    }

    @Override
    public final Integer parse(final String in) {
        return Integer.decode(in);
    }

    public final void setValue(final int value) {
        this.b.putInt(this.pointer(), value);
    }
}

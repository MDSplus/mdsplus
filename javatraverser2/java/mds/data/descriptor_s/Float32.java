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

public final class Float32 extends FLOAT<Float>{
    public static final Float32 F(final float value) {
        return new Float32(DTYPE.F, value);
    }

    public static final Float32 FS(final float value) {
        return new Float32(DTYPE.FS, value);
    }

    public Float32(){
        this(0);
    }

    Float32(final byte dtype, final float value){
        super(dtype, value);
    }

    public Float32(final ByteBuffer b){
        super(b);
    }

    public Float32(final float value){
        this(DTYPE.FLOAT, value);
    }

    @Override
    public final Float32 abs() {
        return new Float32(this.dtype(), Math.abs(this.getValue()));
    }

    @Override
    public boolean equals(final Object obj) {
        if(!super.equals(obj)) return false;
        final float tid = this.toFloat(), tad = ((Float32)obj).toFloat();
        if(tid == 0) return tad == 0;
        final float ttt = tad / tid * tad;
        return (ttt / tid - 2 * tad / tid + 1) / (ttt + tid) < 1e-13;
    }

    @Override
    public final Float32 ffloat() {
        if(this.dtype() == DTYPE.F) return this;
        return Float32.F(this.getValue());
    }

    @Override
    public final Float32 fsfloat() {
        if(this.dtype() == DTYPE.FS) return this;
        return Float32.FS(this.getValue());
    }

    @Override
    public final Float getAtomic() {
        return new Float(this.p.getFloat(0));
    }

    @Override
    protected final byte getRankBits() {
        return 4;
    }

    public final float getValue() {
        return this.getBuffer().getFloat(0);
    }

    @Override
    public final Uint32 inot() {
        return new Uint32(~this.getBuffer().getInt());
    }

    @Override
    public final Float32 neg() {
        return new Float32(this.dtype(), -this.getValue());
    }

    @Override
    public final Float parse(final String in) {
        return Float.valueOf(in);
    }

    public final void setValue(final float value) {
        this.b.putFloat(this.pointer(), value);
    }

    @Override
    public final CString text() {
        return new CString(String.format("%16.5E", this.getAtomic()).replace("E", this.getSuffix()));
    }
}

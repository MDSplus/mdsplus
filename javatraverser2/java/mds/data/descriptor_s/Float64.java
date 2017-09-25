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

public final class Float64 extends FLOAT<Double>{
    public static final Float64 D(final double value) {
        return new Float64(DTYPE.D, value);
    }

    public static final Float64 FT(final double value) {
        return new Float64(DTYPE.FT, value);
    }

    public static final Float64 G(final double value) {
        return new Float64(DTYPE.G, value);
    }

    public Float64(){
        this(0);
    }

    protected Float64(final byte dtype, final double value){
        super(dtype, value);
    }

    public Float64(final ByteBuffer b){
        super(b);
    }

    public Float64(final double value){
        this(DTYPE.DOUBLE, value);
    }

    @Override
    public final Float64 abs() {
        return new Float64(this.dtype(), Math.abs(this.getValue()));
    }

    @Override
    public final Float64 dfloat() {
        if(this.dtype() == DTYPE.D) return this;
        return Float64.D(this.getValue());
    }

    @Override
    public boolean equals(final Object obj) {
        if(!super.equals(obj)) return false;
        final double tid = this.toDouble(), tad = ((Float64)obj).toDouble();
        if(tid == 0) return tad == 0;
        final double ttt = tad / tid * tad;
        return (ttt / tid - 2 * tad / tid + 1) / (ttt + tid) < 1e-13;
    }

    @Override
    public final Float64 ftfloat() {
        if(this.dtype() == DTYPE.FT) return this;
        return Float64.FT(this.getValue());
    }

    @Override
    public final Double getAtomic() {
        return new Double(this.p.getDouble(0));
    }

    @Override
    protected final byte getRankBits() {
        return 8;
    }

    public final double getValue() {
        return this.getBuffer().getDouble(0);
    }

    @Override
    public final Float64 gfloat() {
        if(this.dtype() == DTYPE.G) return this;
        return Float64.G(this.getValue());
    }

    @Override
    public final Uint64 inot() {
        return new Uint64(~this.getBuffer().getLong());
    }

    @Override
    public final Float64 neg() {
        return new Float64(this.dtype(), -this.getValue());
    }

    @Override
    public final Double parse(final String in) {
        return Double.valueOf(in);
    }

    public final void setValue(final double value) {
        this.b.putDouble(this.pointer(), value);
    }

    @Override
    public final CString text() {
        return new CString(String.format("%32.15E", this.getAtomic()).replace("E", this.getSuffix()));
    }
}

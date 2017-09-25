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

public final class Complex64 extends COMPLEX<Double>{
    @SuppressWarnings("serial")
    public static final class ComplexDouble extends Complex<Double>{
        public static final ComplexDouble decode(final String in) {
            final String[] parts = in.split(",", 2);
            return new ComplexDouble(Double.valueOf(parts[0]), Double.valueOf(parts[1]));
        }

        public static final ComplexDouble fromBuffer(final ByteBuffer b) {
            return new ComplexDouble(b.getDouble(), b.getDouble());
        }

        public static final ComplexDouble fromBuffer(final ByteBuffer b, final int idx) {
            return new ComplexDouble(b.getDouble(idx), b.getDouble(idx + Double.BYTES));
        }

        public ComplexDouble(final double real, final double imag){
            this(new Double(real), new Double(imag));
        }

        public ComplexDouble(final Double real, final Double imag){
            super(real, imag);
        }
    }

    public static void putComplexDouble(final ByteBuffer b, final Complex<Double> value) {
        b.putDouble(value.real.doubleValue());
        b.putDouble(value.real.doubleValue());
    }

    public Complex64(){
        this(0, 0);
    }

    public Complex64(final ByteBuffer b){
        super(b);
    }

    public Complex64(final Complex<Double> value){
        this(value.real, value.imag);
    }

    public Complex64(final double real, final double imag){
        super(DTYPE.COMPLEX_DOUBLE, real, imag);
    }

    public Complex64(final Double real, final Double imag){
        this(real.doubleValue(), imag.doubleValue());
    }

    public Complex64(final double[] value){
        this(value[0], value[1]);
    }

    @Override
    public final Float64 abs() {
        return new Float64(Math.sqrt(Math.pow(this.getAtomic().real.doubleValue(), 2) + Math.pow(this.getAtomic().imag.doubleValue(), 2)));
    }

    @Override
    public final ComplexDouble getAtomic() {
        return ComplexDouble.fromBuffer(this.p, 0);
    }

    @Override
    protected final byte getRankBits() {
        return 0x70;
    }

    @Override
    public final Uint64 inot() {
        return new Uint64(~this.getBuffer().getLong());
    }

    @Override
    public final Complex64 neg() {
        return new Complex64(-this.getReal().doubleValue(), -this.getImag().doubleValue());
    }

    @Override
    public final ComplexDouble parse(final String in) {
        return ComplexDouble.decode(in);
    }

    public final void setImag(final double imag) {
        this.b.putDouble(this.pointer() + Double.BYTES, imag);
    }

    public final void setReal(final double real) {
        this.b.putDouble(this.pointer(), real);
    }

    public final void setValue(final double real, final double imag) {
        this.setReal(real);
        this.setImag(imag);
    }

    public final void setValue(final mds.data.descriptor_s.COMPLEX.Complex<Double> value) {
        this.setValue(value.real.doubleValue(), value.imag.doubleValue());
    }
}

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
import mds.MdsException;
import mds.data.DATA;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.COMPLEX.Complex;

@SuppressWarnings("serial")
public abstract class COMPLEX<T extends Number>extends NUMBER<Complex<T>>{
    public static class Complex<T extends Number>extends Number{
        public final T imag;
        public final T real;

        public Complex(final T real, final T imag){
            this.real = real;
            this.imag = imag;
        }

        @Override
        public double doubleValue() {
            return this.real.doubleValue();
        }

        @Override
        public float floatValue() {
            return this.real.floatValue();
        }

        @Override
        public int intValue() {
            return this.real.intValue();
        }

        @Override
        public long longValue() {
            return this.real.longValue();
        }
    }

    public static final <T extends Number> StringBuilder decompile(final StringBuilder pout, final Complex<T> complex, final byte dtype, final int mode) {
        final byte realdtype = (byte)(dtype - 2);
        pout.append("Cmplx(");
        pout.append(FLOAT.decompile(complex.real, realdtype, mode)).append(',');
        pout.append(FLOAT.decompile(complex.imag, realdtype, mode)).append(')');
        return pout;
    }

    private static ByteBuffer toByteBuffer(final Complex<?> value) {
        if(value.real instanceof Double) return COMPLEX.toByteBuffer(value.real.doubleValue(), value.imag.doubleValue());
        return COMPLEX.toByteBuffer(value.real.floatValue(), value.imag.floatValue());
    }

    private static final ByteBuffer toByteBuffer(final double real, final double imag) {
        return ByteBuffer.allocate(Double.BYTES * 2).order(Descriptor.BYTEORDER).putDouble(0, real).putDouble(Double.BYTES, imag);
    }

    private static final ByteBuffer toByteBuffer(final float real, final float imag) {
        return ByteBuffer.allocate(Float.BYTES * 2).order(Descriptor.BYTEORDER).putFloat(0, real).putFloat(Float.BYTES, imag);
    }

    @SuppressWarnings({"unchecked", "rawtypes"})
    private final static Complex<Number> toComplex(final Descriptor<?> X) {
        return (X instanceof COMPLEX) ? ((COMPLEX<Number>)X).getAtomic() : new Complex(Double.valueOf(X.toDouble()), Double.valueOf(0.));
    }

    protected COMPLEX(final byte dtype, final Complex<?> value){
        super(dtype, COMPLEX.toByteBuffer(value));
    }

    public COMPLEX(final byte dtype, final double real, final double imag){
        super(dtype, COMPLEX.toByteBuffer(real, imag));
    }

    public COMPLEX(final byte dtype, final float real, final float imag){
        super(dtype, COMPLEX.toByteBuffer(real, imag));
    }

    protected COMPLEX(final ByteBuffer b){
        super(b);
    }

    @Override
    public final COMPLEX<?> add(final Descriptor<?> X, final Descriptor<?> Y) {
        final Complex<Number> x = COMPLEX.toComplex(X), y = COMPLEX.toComplex(Y);
        return this.newType(x.real.doubleValue() + y.real.doubleValue(), x.imag.doubleValue() + y.imag.doubleValue());
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return COMPLEX.decompile(pout, this.getAtomic(), this.dtype(), mode);
    }

    @Override
    public final COMPLEX<?> divide(final Descriptor<?> X, final Descriptor<?> Y) {
        final Complex<Number> x = COMPLEX.toComplex(X), y = COMPLEX.toComplex(Y);
        final double xr = x.real.doubleValue(), xi = x.imag.doubleValue();
        final double yr = y.real.doubleValue(), yi = y.imag.doubleValue();
        final double d = yr * yr + yi * yi;
        return this.newType((xr * yr + xi * yi) / d, (xi * yr - xr * yi) / d);
    }

    public final T getImag() {
        return this.getAtomic().imag;
    }

    @Override
    protected byte getRankClass() {
        return 8;
    }

    public final T getReal() {
        return this.getAtomic().real;
    }

    @Override
    public final COMPLEX<?> multiply(final Descriptor<?> X, final Descriptor<?> Y) {
        final Complex<Number> x = COMPLEX.toComplex(X), y = COMPLEX.toComplex(Y);
        return this.newType(x.real.doubleValue() * y.real.doubleValue() - x.imag.doubleValue() * y.imag.doubleValue(), x.real.doubleValue() * y.imag.doubleValue() + x.imag.doubleValue() * y.real.doubleValue());
    }

    private final COMPLEX<?> newType(final double real, final double imag) {
        return (this instanceof Complex32) ? new Complex32(real, imag) : new Complex64(real, imag);
    }

    @Override
    public final COMPLEX<?> power(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        throw DATA.notimplemented;
    }

    @Override
    public COMPLEX<?> shiftleft(final Descriptor<?> X) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public COMPLEX<?> shiftright(final Descriptor<?> X) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final COMPLEX<?> subtract(final Descriptor<?> X, final Descriptor<?> Y) {
        final Complex<Number> x = COMPLEX.toComplex(X), y = COMPLEX.toComplex(Y);
        return this.newType(x.real.doubleValue() - y.real.doubleValue(), x.imag.doubleValue() - y.imag.doubleValue());
    }
}

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
package mds.data.descriptor_a;

import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import mds.MdsException;
import mds.data.DTYPE;
import mds.data.descriptor_s.COMPLEX.Complex;
import mds.data.descriptor_s.Complex32;
import mds.data.descriptor_s.Complex32.ComplexFloat;

public final class Complex32Array extends COMPLEXArray<Float>{
    public Complex32Array(){
        super(DTYPE.COMPLEX_FLOAT, new float[0]);
    }

    public Complex32Array(final byte dtype, final ByteBuffer data, final int[] shape){
        super(DTYPE.COMPLEX_FLOAT, data, shape);
    }

    public Complex32Array(final ByteBuffer b){
        super(b);
    }

    public Complex32Array(final float... values){
        super(DTYPE.COMPLEX_FLOAT, values, new int[]{values.length / 2});
    }

    public Complex32Array(final float[]... values){
        super(DTYPE.COMPLEX_FLOAT, values);
    }

    public Complex32Array(final int[] dims, final float... values){
        super(DTYPE.COMPLEX_FLOAT, values, dims);
    }

    public Complex32Array(final int[] dims, final float[]... values){
        super(DTYPE.COMPLEX_FLOAT, values, dims);
    }

    @Override
    public Float32Array abs() {
        final float[] values = new float[this.arsize() / this.length()];
        for(int i = 0; i < values.length; i++)
            values[i] = (float)Math.sqrt(Math.pow(this.getImag(i).doubleValue(), 2) + Math.pow(this.getImag(i).doubleValue(), 2));
        return new Float32Array(this.getShape(), values);
    }

    @Override
    public final ByteBuffer buildBuffer(final ByteBuffer buf, final double value) {
        return buf.putFloat((float)value);
    }

    @SuppressWarnings("static-method")
    public final ByteBuffer buildBuffer(final ByteBuffer buf, final double real, final double imag) {
        return buf.putFloat((float)real).putFloat((float)imag);
    }

    @Override
    public final ComplexFloat getElement(final ByteBuffer b_in) {
        return new ComplexFloat(b_in.getFloat(), b_in.getFloat());
    }

    @Override
    public ComplexFloat getElement(final int i) {
        return new ComplexFloat(this.p.getFloat(i * Float.BYTES), this.p.getFloat(i * Float.BYTES + Float.BYTES));
    }

    @Override
    protected final byte getRankBits() {
        return 4;
    }

    @Override
    public Complex32 getScalar(final int idx) {
        return new Complex32(this.getElement(idx));
    }

    @Override
    protected final ComplexFloat[] initArray(final int size) {
        return new ComplexFloat[size];
    }

    @Override
    public Uint32Array inot() {
        final IntBuffer ib = this.getBuffer().asIntBuffer();
        final int[] values = new int[this.arsize() / this.length()];
        for(int i = 0; i < values.length; i++)
            values[i] = ~ib.get(i * 2);
        return new Uint32Array(this.getShape(), values);
    }

    @Override
    public final Complex32Array neg() throws MdsException {
        final FloatBuffer fb = this.getBuffer().asFloatBuffer();
        final float[] values = new float[this.arsize() / this.length() * 2];
        for(int i = 0; i < values.length; i++)
            values[i] = -fb.get();
        return new Complex32Array(this.getShape(), values);
    }

    @Override
    public final ComplexFloat parse(final String in) {
        return ComplexFloat.decode(in);
    }

    @Override
    protected final void setElement(final ByteBuffer b, final Complex<Float> value) {
        b.putFloat(value.real.floatValue());
        b.putFloat(value.real.floatValue());
    }

    @Override
    protected void setElement(final int i, final Complex<Float> value) {
        this.p.putFloat(i * Float.BYTES, value.real.floatValue());
        this.p.putFloat(i * Float.BYTES + Float.BYTES, value.imag.floatValue());
    }
}

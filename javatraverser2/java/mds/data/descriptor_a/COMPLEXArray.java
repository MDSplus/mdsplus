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
import java.nio.DoubleBuffer;
import java.nio.FloatBuffer;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.COMPLEX;
import mds.data.descriptor_s.COMPLEX.Complex;

public abstract class COMPLEXArray<T extends Number>extends NUMBERArray<Complex<T>>{
    private static final ByteBuffer toByteBuffer(final Complex<Number>[] values) {
        if(values.length == 0) return ByteBuffer.allocate(0);
        final ByteBuffer b;
        if(values[0].real instanceof Double){
            b = ByteBuffer.allocate(values.length * Double.BYTES * 2).order(Descriptor.BYTEORDER);
            for(final Complex<Number> value : values)
                b.putDouble(value.real.doubleValue()).putDouble(value.imag.doubleValue());
        }else{
            b = ByteBuffer.allocate(values.length * Float.BYTES * 2).order(Descriptor.BYTEORDER);
            for(final Complex<Number> value : values)
                b.putFloat(value.real.floatValue()).putDouble(value.imag.doubleValue());
        }
        return b;
    }

    private static final ByteBuffer toByteBuffer(final double[][] values) {
        final ByteBuffer b = ByteBuffer.allocate(values.length * Float.BYTES * 2).order(Descriptor.BYTEORDER);
        final DoubleBuffer db = b.asDoubleBuffer();
        for(final double[] value : values)
            db.put(value[0]).put(value[1]);
        return b;
    }

    private static final ByteBuffer toByteBuffer(final float[][] values) {
        final ByteBuffer b = ByteBuffer.allocate(values.length * Float.BYTES * 2).order(Descriptor.BYTEORDER);
        final FloatBuffer fb = b.asFloatBuffer();
        for(final float[] value : values)
            fb.put(value[0]).put(value[1]);
        return b;
    }

    public COMPLEXArray(final byte dtype, final ByteBuffer data, final int[] shape){
        super(dtype, data, shape);
    }

    protected COMPLEXArray(final byte dtype, final Complex<Number>[] values, final int... shape){
        super(dtype, COMPLEXArray.toByteBuffer(values), shape.length == 0 ? new int[]{values.length} : shape);
    }

    protected COMPLEXArray(final byte dtype, final double[] values, final int... shape){
        super(dtype, FLOATArray.toByteBuffer(values), shape.length == 0 ? new int[]{values.length / 2} : shape);
    }

    protected COMPLEXArray(final byte dtype, final double[][] values, final int... shape){
        super(dtype, COMPLEXArray.toByteBuffer(values), shape.length == 0 ? new int[]{values.length} : shape);
    }

    protected COMPLEXArray(final byte dtype, final float[] values, final int... shape){
        super(dtype, FLOATArray.toByteBuffer(values), shape.length == 0 ? new int[]{values.length / 2} : shape);
    }

    protected COMPLEXArray(final byte dtype, final float[][] values, final int... shape){
        super(dtype, COMPLEXArray.toByteBuffer(values), shape.length == 0 ? new int[]{values.length} : shape);
    }

    protected COMPLEXArray(final ByteBuffer b){
        super(b);
    }

    @Override
    protected final StringBuilder decompile(final StringBuilder pout, final Complex<T> t) {
        return COMPLEX.decompile(pout, t, this.dtype(), Descriptor.DECO_NRM);
    }

    public final T getImag(final int idx) {
        return this.getElement(idx).imag;
    }

    @Override
    protected final byte getRankClass() {
        return 0x70;
    }

    public final T getReal(final int idx) {
        return this.getElement(idx).real;
    }
}

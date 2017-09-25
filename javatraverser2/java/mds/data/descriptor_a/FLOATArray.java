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
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.FLOAT;

public abstract class FLOATArray<T extends Number>extends NUMBERArray<T>{
    static final ByteBuffer toByteBuffer(final double[] values) {
        final ByteBuffer b = ByteBuffer.allocate(values.length * Double.BYTES).order(Descriptor.BYTEORDER);
        b.asDoubleBuffer().put(values);
        return b;
    }

    static final ByteBuffer toByteBuffer(final float[] values) {
        final ByteBuffer b = ByteBuffer.allocate(values.length * Float.BYTES).order(Descriptor.BYTEORDER);
        b.asFloatBuffer().put(values);
        return b;
    }

    public FLOATArray(final byte dtype, final ByteBuffer data, final int[] shape){
        super(dtype, data, shape);
    }

    protected FLOATArray(final byte dtype, final double[] values, final int... shape){
        super(dtype, FLOATArray.toByteBuffer(values), shape.length == 0 ? new int[]{values.length} : shape);
    }

    protected FLOATArray(final byte dtype, final float[] values, final int... shape){
        super(dtype, FLOATArray.toByteBuffer(values), shape.length == 0 ? new int[]{values.length} : shape);
    }

    protected FLOATArray(final ByteBuffer b){
        super(b);
    }

    @Override
    public final StringBuilder decompile(final StringBuilder pout, final T value) {
        return pout.append(FLOAT.decompile(value, this.dtype(), Descriptor.DECO_NRM));
    }

    @Override
    protected final byte getRankClass() {
        return 0x30;
    }

    @Override
    public final String toString(final T value) {
        return FLOAT.decompile(value, this.dtype(), Descriptor.DECO_STR);
    }
}

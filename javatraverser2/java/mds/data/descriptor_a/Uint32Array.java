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
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Uint32;
import mds.data.descriptor_s.Uint32.UInteger;

public final class Uint32Array extends INTEGER_UNSIGNEDArray<UInteger>{
    public Uint32Array(final byte dtype, final ByteBuffer data, final int shape[]){
        super(DTYPE.LU, data, shape);
    }

    public Uint32Array(final ByteBuffer b){
        super(b);
    }

    public Uint32Array(final int... values){
        super(DTYPE.LU, values);
    }

    public Uint32Array(final int shape[], final int... values){
        super(DTYPE.LU, values, shape);
    }

    @Override
    public Uint32Array abs() {
        return this;
    }

    @Override
    public final ByteBuffer buildBuffer(final ByteBuffer buf, final int value) {
        return buf.putInt(value);
    }

    @Override
    protected final boolean format() {
        return true;
    }

    @Override
    public final UInteger getElement(final ByteBuffer b_in) {
        return UInteger.fromBuffer(b_in);
    }

    @Override
    public final UInteger getElement(final int idx) {
        return UInteger.fromBuffer(this.p, idx);
    }

    @Override
    protected final byte getRankBits() {
        return 4;
    }

    @Override
    public Uint32 getScalar(final int idx) {
        return new Uint32(this.getElement(idx));
    }

    @Override
    protected final UInteger[] initArray(final int size) {
        return new UInteger[size];
    }

    @Override
    public final Uint32Array inot() {
        final int[] values = this.toArray();
        for(int i = 0; i < values.length; i++)
            values[i] = ~values[i];
        return new Uint32Array(this.getShape(), values);
    }

    @Override
    public final Int32Array longs() {
        return new Int32Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.L));
    }

    @Override
    public final Uint32Array longu() {
        return this;
    }

    @Override
    public Int32Array neg() {
        final int[] values = this.toArray();
        for(int i = 0; i < values.length; i++)
            values[i] = -values[i];
        return new Int32Array(this.getShape(), values);
    }

    @Override
    public final UInteger parse(final String in) {
        return UInteger.decode(in);
    }

    @Override
    protected final void setElement(final ByteBuffer b, final UInteger value) {
        b.putInt(value.intValue());
    }

    @Override
    protected void setElement(final int i, final UInteger value) {
        this.p.putInt(i * Integer.BYTES, value.intValue());
    }

    public final int[] toArray() {
        final int[] values = new int[this.arsize() / Integer.BYTES];
        this.getBuffer().asIntBuffer().get(values);
        return values;
    }
}

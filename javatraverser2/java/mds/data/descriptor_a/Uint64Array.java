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
import mds.data.descriptor_s.Uint64;
import mds.data.descriptor_s.Uint64.ULong;

public final class Uint64Array extends INTEGER_UNSIGNEDArray<ULong>{
    public Uint64Array(final byte dtype, final ByteBuffer data, final int shape[]){
        super(DTYPE.QU, data, shape);
    }

    public Uint64Array(final ByteBuffer b){
        super(b);
    }

    public Uint64Array(final int shape[], final long... values){
        super(DTYPE.QU, values, shape);
    }

    public Uint64Array(final long... values){
        super(DTYPE.QU, values);
    }

    @Override
    public final Uint64Array abs() {
        return this;
    }

    @Override
    public ByteBuffer buildBuffer(final ByteBuffer buf, final double value) {
        return this.buildBuffer(buf, (long)value);
    }

    @Override
    public ByteBuffer buildBuffer(final ByteBuffer buf, final int value) {
        return this.buildBuffer(buf, (long)value);
    }

    @SuppressWarnings("static-method")
    public ByteBuffer buildBuffer(final ByteBuffer buf, final long value) {
        return buf.putLong(value);
    }

    @Override
    public final ULong getElement(final ByteBuffer b_in) {
        return ULong.fromBuffer(b_in);
    }

    @Override
    public final ULong getElement(final int idx) {
        return ULong.fromBuffer(this.p, idx);
    }

    @Override
    protected final byte getRankBits() {
        return 8;
    }

    @Override
    public final Uint64 getScalar(final int idx) {
        return new Uint64(this.getElement(idx));
    }

    @Override
    protected final ULong[] initArray(final int size) {
        return new ULong[size];
    }

    @Override
    public final Uint64Array inot() {
        final long[] values = this.toArray();
        for(int i = 0; i < values.length; i++)
            values[i] = ~values[i];
        return new Uint64Array(this.getShape(), values);
    }

    @Override
    public final Uint64Array neg() {
        final long[] values = this.toArray();
        for(int i = 0; i < values.length; i++)
            values[i] = -values[i];
        return new Uint64Array(this.getShape(), values);
    }

    @Override
    public final ULong parse(final String in) {
        return ULong.decode(in);
    }

    @Override
    public final Int64Array quadwords() {
        return new Int64Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.Q));
    }

    @Override
    public final Uint64Array quadwordu() {
        return this;
    }

    @Override
    protected final void setElement(final ByteBuffer b, final ULong value) {
        b.putLong(value.longValue());
    }

    @Override
    protected void setElement(final int i, final ULong value) {
        this.p.putLong(i * Long.BYTES, value.longValue());
    }

    public final long[] toArray() {
        final long[] values = new long[this.arsize() / Long.BYTES];
        this.getBuffer().asLongBuffer().get(values);
        return values;
    }
}

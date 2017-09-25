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
import mds.data.descriptor_s.Int16;

public final class Int16Array extends INTEGERArray<Short>{
    public Int16Array(final byte dtype, final ByteBuffer data, final int shape[]){
        super(DTYPE.W, data, shape);
    }

    public Int16Array(final ByteBuffer b){
        super(b);
    }

    public Int16Array(final int shape[], final short... values){
        super(DTYPE.W, values, shape);
    }

    public Int16Array(final short... values){
        super(DTYPE.W, values);
    }

    @Override
    public Uint16Array abs() {
        final short[] values = this.toArray();
        for(int i = 0; i < values.length; i++)
            if(values[i] < 0) values[i] = (short)(-values[i] & 0xFFFF);
        return new Uint16Array(values);
    }

    @Override
    public final ByteBuffer buildBuffer(final ByteBuffer buf, final int value) {
        return buf.putShort((short)value);
    }

    @Override
    protected final boolean format() {
        return true;
    }

    @Override
    public final Short getElement(final ByteBuffer b_in) {
        return new Short(b_in.getShort());
    }

    @Override
    public final Short getElement(final int i) {
        return new Short(this.p.getShort(i * Short.BYTES));
    }

    @Override
    protected final byte getRankBits() {
        return 2;
    }

    @Override
    public Int16 getScalar(final int idx) {
        return new Int16(this.getElement(idx).shortValue());
    }

    @Override
    protected final Short[] initArray(final int size) {
        return new Short[size];
    }

    @Override
    public final Uint16Array inot() {
        final short[] values = this.toArray();
        for(int i = 0; i < values.length; i++)
            values[i] = (short)(~values[i] & 0xFFFF);
        return new Uint16Array(values);
    }

    @Override
    public Int16Array neg() {
        final short[] values = this.toArray();
        for(int i = 0; i < values.length; i++)
            values[i] = (short)(-values[i] & 0xFFFF);
        return new Int16Array(values);
    }

    @Override
    public final Short parse(final String in) {
        return Short.decode(in);
    }

    @Override
    protected final void setElement(final ByteBuffer b, final Short value) {
        b.putShort(value.shortValue());
    }

    @Override
    protected void setElement(final int i, final Short value) {
        this.p.putShort(i * Short.BYTES, value.shortValue());
    }

    public final short[] toArray() {
        final short[] values = new short[this.arsize() / Short.BYTES];
        this.getBuffer().asShortBuffer().get(values);
        return values;
    }

    @Override
    public final Int16Array words() {
        return this;
    }

    @Override
    public final Uint16Array wordu() {
        return new Uint16Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.WU));
    }
}

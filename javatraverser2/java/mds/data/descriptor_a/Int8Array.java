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
import mds.MdsException;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Int8;

public final class Int8Array extends INTEGERArray<Byte>{
    public Int8Array(final byte... values){
        super(DTYPE.B, values);
    }

    public Int8Array(final byte dtype, final ByteBuffer data, final int shape[]){
        super(DTYPE.B, data, shape);
    }

    public Int8Array(final ByteBuffer b){
        super(b);
    }

    public Int8Array(final int shape[], final byte... values){
        super(DTYPE.B, values, shape);
    }

    @Override
    public Uint8Array abs() throws MdsException {
        final byte[] values = this.serializeArray_copy();
        for(int i = 0; i < values.length; i++)
            if(values[i] < 0) values[i] = (byte)(-values[i] & 0xFF);
        return new Uint8Array(this.getShape(), values);
    }

    @Override
    public final ByteBuffer buildBuffer(final ByteBuffer buf, final int value) {
        return buf.put((byte)value);
    }

    @Override
    public final Int8Array bytes() {
        return this;
    }

    @Override
    public final Uint8Array byteu() {
        return new Uint8Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.BU));
    }

    public final Descriptor<?> deserialize() throws MdsException {
        return Descriptor.deserialize(this.getBuffer());
    }

    @Override
    protected final boolean format() {
        return true;
    }

    @Override
    public final Byte getElement(final ByteBuffer b_in) {
        return new Byte(b_in.get());
    }

    @Override
    public Byte getElement(final int i) {
        return new Byte(this.p.get(i));
    }

    @Override
    protected final byte getRankBits() {
        return 1;
    }

    @Override
    public Int8 getScalar(final int idx) {
        return new Int8(this.getElement(idx).byteValue());
    }

    @Override
    protected final Byte[] initArray(final int size) {
        return new Byte[size];
    }

    @Override
    public final Uint8Array inot() throws MdsException {
        final byte[] values = this.toArray();
        for(int i = 0; i < values.length; i++)
            values[i] = (byte)(~values[i] & 0xFF);
        return new Uint8Array(this.getShape(), values);
    }

    @Override
    public Int8Array neg() throws MdsException {
        final byte[] values = this.serializeArray_copy();
        for(int i = 0; i < values.length; i++)
            values[i] = (byte)(-values[i] & 0xFF);
        return new Int8Array(this.getShape(), values);
    }

    @Override
    public final Byte parse(final String in) {
        return Byte.decode(in);
    }

    @Override
    protected final void setElement(final ByteBuffer b, final Byte value) {
        b.put(value.byteValue());
    }

    @Override
    protected final void setElement(final int i, final Byte value) {
        this.p.put(i, value.byteValue());
    }

    public final byte[] toArray() {
        final byte[] values = new byte[this.arsize() / Byte.BYTES];
        this.getBuffer().get(values);
        return values;
    }
}

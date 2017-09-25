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
import mds.data.descriptor_s.Uint8;
import mds.data.descriptor_s.Uint8.UByte;

public final class Uint8Array extends INTEGER_UNSIGNEDArray<UByte>{
    public Uint8Array(final byte... values){
        super(DTYPE.BU, values);
    }

    public Uint8Array(final byte dtype, final ByteBuffer data, final int shape[]){
        super(DTYPE.BU, data, shape);
    }

    public Uint8Array(final ByteBuffer b){
        super(b);
    }

    public Uint8Array(final int shape[], final byte... values){
        super(DTYPE.BU, values, shape);
    }

    @Override
    public final Uint8Array abs() {
        return this;
    }

    @Override
    public final ByteBuffer buildBuffer(final ByteBuffer buf, final int value) {
        return buf.put((byte)value);
    }

    @Override
    public final Int8Array bytes() {
        return new Int8Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.B));
    }

    @Override
    public final Uint8Array byteu() {
        return this;
    }

    @Override
    protected final boolean format() {
        return true;
    }

    @Override
    public final UByte getElement(final ByteBuffer b_in) {
        return UByte.fromBuffer(b_in);
    }

    @Override
    public UByte getElement(final int i) {
        return UByte.fromBuffer(this.p, i);
    }

    @Override
    protected final byte getRankBits() {
        return 1;
    }

    @Override
    public final Uint8 getScalar(final int idx) {
        return new Uint8(this.getElement(idx));
    }

    @Override
    protected final UByte[] initArray(final int size) {
        return new UByte[size];
    }

    @Override
    public final Uint8Array inot() {
        final byte[] values = this.toArray();
        for(int i = 0; i < values.length; i++)
            values[i] = (byte)(~values[i] & 0xFF);
        return new Uint8Array(this.getShape(), values);
    }

    @Override
    public final Int8Array neg() {
        final byte[] values = this.toArray();
        for(int i = 0; i < values.length; i++)
            values[i] = (byte)(-values[i] & 0xFF);
        return new Int8Array(this.getShape(), values);
    }

    @Override
    public final UByte parse(final String in) {
        return UByte.decode(in);
    }

    @Override
    protected final void setElement(final ByteBuffer b, final UByte value) {
        b.put(value.byteValue());
    }

    @Override
    protected final void setElement(final int i, final UByte value) {
        this.p.put(i, value.byteValue());
    }

    public final byte[] toArray() {
        final byte[] values = new byte[this.arsize() / Byte.BYTES];
        this.getBuffer().get(values);
        return values;
    }
}

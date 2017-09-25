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

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Int128;
import mds.data.descriptor_s.Uint128;

public final class Int128Array extends INTEGERArray<BigInteger>{
    public Int128Array(final BigInteger... values){
        super(DTYPE.O, values);
    }

    public Int128Array(final byte dtype, final ByteBuffer data, final int shape[]){
        super(DTYPE.O, data, shape);
    }

    public Int128Array(final ByteBuffer b){
        super(b);
    }

    public Int128Array(final int shape[], final BigInteger... values){
        super(DTYPE.O, values, shape);
    }

    public Int128Array(final int shape[], final long... values){
        super(DTYPE.O, INTEGERArray.long2BigInt(false, values), shape);
    }

    @Override
    public final Uint128Array abs() {
        final BigInteger[] values = this.getAtomic();
        for(int i = 0; i < values.length; i++)
            values[i] = values[i].abs();
        return new Uint128Array(values);
    }

    @Override
    public ByteBuffer buildBuffer(final ByteBuffer buf, final int value) {
        return null;
    }

    @Override
    protected StringBuilder decompile(final StringBuilder pout, final BigInteger value) {
        return pout.append(String.format("0X%x", value)).append(this.getSuffix());
    }

    @Override
    public final BigInteger getElement(final ByteBuffer b_in) {
        return Int128.getBigInteger(b_in);
    }

    @Override
    public final BigInteger getElement(final int i) {
        return Int128.getBigInteger(this.p, i);
    }

    @Override
    protected final byte getRankBits() {
        return 16;
    }

    @Override
    public Int128 getScalar(final int idx) {
        return new Int128(this.getElement(idx));
    }

    @Override
    protected final BigInteger[] initArray(final int size) {
        return new BigInteger[size];
    }

    @Override
    public final Uint128Array inot() {
        final BigInteger[] values = this.getAtomic();
        for(int i = 0; i < values.length; i++)
            values[i] = values[i].xor(Uint128.FFx16);
        return new Uint128Array(values);
    }

    @Override
    public final Int128Array neg() {
        final BigInteger[] values = this.getAtomic();
        for(int i = 0; i < values.length; i++)
            values[i] = values[i].negate();
        return new Int128Array(values);
    }

    @Override
    public final Int128Array octawords() {
        return this;
    }

    @Override
    public final Uint128Array octawordu() {
        return new Uint128Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.OU));
    }

    @Override
    public final BigInteger parse(final String in) {
        return new BigInteger(in).setBit(128);
    }

    @Override
    protected final void setElement(final ByteBuffer b, final BigInteger value) {
        Int128.putBigInteger(b, value);
    }

    @Override
    protected final void setElement(final int i, final BigInteger value) {
        Int128.putBigInteger(this.b, i, value);
    }
}

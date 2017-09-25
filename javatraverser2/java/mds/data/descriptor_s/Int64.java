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
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;

public final class Int64 extends INTEGER<Long>{
    public Int64(){
        this(0);
    }

    public Int64(final ByteBuffer b){
        super(b);
    }

    public Int64(final double value){
        this((long)value);
    }

    public Int64(final int value){
        this((long)value);
    }

    public Int64(final long value){
        super(DTYPE.Q, value);
    }

    @Override
    public final Uint64 abs() {
        return new Uint64(Math.abs(this.getPrimitive()));
    }

    @Override
    public Int64 add(final Descriptor<?> X, final Descriptor<?> Y) {
        return new Int64(X.toLong() + Y.toLong());
    }

    @Override
    public Int64 divide(final Descriptor<?> X, final Descriptor<?> Y) {
        return new Int64(X.toLong() / Y.toLong());
    }

    @Override
    public final Long getAtomic() {
        return new Long(this.p.getLong(0));
    }

    public final long getPrimitive() {
        return this.p.getLong(0);
    }

    @Override
    protected final byte getRankBits() {
        return 8;
    }

    @Override
    public Uint64 inot() {
        return new Uint64(~this.getPrimitive());
    }

    @Override
    public Int64 multiply(final Descriptor<?> X, final Descriptor<?> Y) {
        return new Int64(X.toLong() * Y.toLong());
    }

    @Override
    public final Int64 neg() {
        return new Int64(-this.getPrimitive());
    }

    @Override
    public final Long parse(final String in) {
        return Long.decode(in);
    }

    @Override
    public Int64 quadwords() {
        return this;
    }

    public final void setValue(final long value) {
        this.b.putLong(this.pointer(), value);
    }

    @Override
    public Int64 shiftleft(final Descriptor<?> X) {
        return new Int64(this.toLong() << X.toInt());
    }

    @Override
    public Int64 shiftright(final Descriptor<?> X) {
        return new Int64(this.toLong() >> X.toInt());
    }

    @Override
    public Int64 subtract(final Descriptor<?> X, final Descriptor<?> Y) {
        return new Int64(X.toLong() - Y.toLong());
    }
}
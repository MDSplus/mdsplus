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
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_S;
import mds.data.descriptor_s.Uint16.UShort;

public final class Pointer extends Descriptor_S<Number> implements DATA<Number>{
    public static final UShort fromBuffer(final ByteBuffer b, final int idx) {
        return new UShort(b.getShort(idx));
    }

    public static final Pointer NULL() {
        return new Pointer(ByteBuffer.wrap(new byte[]{8, 0, DTYPE.POINTER, Descriptor_S.CLASS, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}).order(Descriptor.BYTEORDER));
    }

    public Pointer(final ByteBuffer b){
        super(b);
    }

    public Pointer(final int value){
        super(DTYPE.POINTER, ByteBuffer.allocate(Integer.BYTES).order(Descriptor.BYTEORDER).putInt(value));
    }

    public Pointer(final long value){
        super(DTYPE.POINTER, ByteBuffer.allocate(Long.BYTES).order(Descriptor.BYTEORDER).putLong(value));
    }

    @Override
    public final Descriptor<?> abs() throws MdsException {
        throw DATA.dataerror;
    }

    @Override
    public Descriptor<?> add(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        pout.append(this.getDTypeName());
        if(this.isNull()) return pout.append("(0)");
        return pout.append("(0x").append(this.length() == 4 ? Integer.toHexString(this.toInt()) : Long.toHexString(this.toLong())).append(')');
    }

    @Override
    public Descriptor<?> divide(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        throw DATA.tdierror;
    }

    public final long getAddress() {
        if(this.length() == 4) return this.getAtomic().longValue() & 0xFFFFFFFFl;
        return this.getAtomic().longValue();
    }

    public final int getAddress32() {
        return this.getAtomic().intValue();
    }

    @Override
    public final Number getAtomic() {
        if(this.length() == 4) return new Integer(this.p.getInt(0));
        return new Long(this.p.getLong(0));
    }

    @Override
    public final DATA<Number> getData_() throws MdsException {
        return this;
    }

    @Override
    public Descriptor<?> getLocal_() {
        return this;
    }

    @Override
    public final byte getRank() {
        return -1;
    }

    @Override
    public final Descriptor<?> inot() throws MdsException {
        throw DATA.dataerror;
    }

    public final boolean isNull() {
        if(this.length() == 4) return this.b.getInt(Descriptor._ptrI + Integer.BYTES) == 0;
        return this.b.getLong(Descriptor._ptrI + Integer.BYTES) == 0l;
    }

    @Override
    public Descriptor<?> multiply(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final Descriptor<?> neg() throws MdsException {
        return new Uint8(this.isNull() ? (byte)1 : (byte)0);
    }

    @Override
    public NUMBER<?> power(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        throw DATA.tdierror;
    }

    public final void setAddress(final ByteBuffer val) {
        if(val.capacity() == 8) this.setAddress(val.getLong(0));
        else this.setAddress(val.getInt(0));
    }

    public final void setAddress(final int val) {
        if(this.length() == 8) this.b.putLong(this.pointer(), val);
        else this.b.putInt(this.pointer(), val);
    }

    public final void setAddress(final long val) {
        if(this.length() == 8) this.b.putLong(this.pointer(), val);
        else if((val & 0xFFFFFFFFl) == val) this.b.putInt(this.pointer(), (int)val);
        else((ByteBuffer)this.b.duplicate().position(this.pointer())).put(new byte[this.length()]);
    }

    public final void setAddress(final Number value) {
        if(value instanceof Integer) this.setAddress(value.intValue());
        else if(value instanceof Long) this.setAddress(value.longValue());
    }

    @Override
    public Descriptor<?> shiftleft(final Descriptor<?> X) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public Descriptor<?> shiftright(final Descriptor<?> X) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public Descriptor<?> subtract(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final Missing text() throws MdsException {
        throw DATA.tdierror;
    }
}
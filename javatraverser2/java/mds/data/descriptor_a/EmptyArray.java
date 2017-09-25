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
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;

public final class EmptyArray extends Descriptor_A<Object>{
    public static final EmptyArray NEW = new EmptyArray();

    public EmptyArray(){
        super(DTYPE.MISSING, ByteBuffer.allocate(0).order(Descriptor.BYTEORDER));
    }

    public EmptyArray(final ByteBuffer b){
        super(b);
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return pout.append("[]");
    }

    @Override
    protected DATA<?> getData_() throws MdsException {
        return new Uint8Array(new byte[]{});
    }

    @Override
    public final Object getElement(final ByteBuffer b_in) {
        return null;
    }

    @Override
    public Object getElement(final int i) {
        return null;
    }

    @Override
    public Descriptor<?> getScalar(final int idx) {
        return null;
    }

    @Override
    protected final String getSuffix() {
        return "";
    }

    @Override
    protected final Object[] initArray(final int size) {
        return new Object[0];
    }

    @Override
    protected final void setElement(final ByteBuffer b, final Object value) {/*stub*/}

    @Override
    protected void setElement(final int i, final Object value) {/*stub*/}

    @Override
    public BigInteger toBigInteger(final Object t) {
        return null;
    }

    @Override
    public final byte toByte(final Object t) {
        return 0;
    }

    @Override
    public final double toDouble(final Object t) {
        return 0.;
    }

    @Override
    public final float toFloat(final Object t) {
        return 0.f;
    }

    @Override
    public final int toInt(final Object t) {
        return 0;
    }

    @Override
    public final long toLong(final Object t) {
        return 0l;
    }

    @Override
    public final short toShort(final Object t) {
        return 0;
    }

    @Override
    protected final String toString(final Object t) {
        return "*";
    }
}

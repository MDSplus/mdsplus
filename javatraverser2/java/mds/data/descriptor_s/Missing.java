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
import mds.data.DATA;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_S;

public final class Missing extends Descriptor_S<Object> implements DATA<Object>{
    public static final Missing NEW = new Missing();

    private Missing(){
        super(ByteBuffer.wrap(new byte[]{4, 0, 0, 1, 8, 0, 0, 0, 0, 0, 0, 0}).order(Descriptor.BYTEORDER));
    }

    @Override
    public final Missing abs() {
        return this;
    }

    @Override
    public Descriptor<?> add(final Descriptor<?> X, final Descriptor<?> Y) {
        return this;
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return pout.append('*');
    }

    @Override
    public Descriptor<?> divide(final Descriptor<?> X, final Descriptor<?> Y) {
        return this;
    }

    @Override
    public final Object getAtomic() {
        return null;
    }

    @Override
    public final DATA<Object> getData_() {
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
    public final Missing inot() {
        return this;
    }

    @Override
    public final boolean isLocal() {
        return true;
    }

    @Override
    public Descriptor<?> multiply(final Descriptor<?> X, final Descriptor<?> Y) {
        return this;
    }

    @Override
    public final Missing neg() {
        return this;
    }

    @Override
    public Descriptor<?> power(final Descriptor<?> X, final Descriptor<?> Y) {
        return this;
    }

    @Override
    public Descriptor<?> shiftleft(final Descriptor<?> X) {
        return this;
    }

    @Override
    public Descriptor<?> shiftright(final Descriptor<?> X) {
        return this;
    }

    @Override
    public Descriptor<?> subtract(final Descriptor<?> X, final Descriptor<?> Y) {
        return this;
    }

    @Override
    public final CString text() {
        return new CString("");
    }

    @Override
    public final byte[] toByteArray() {
        return new byte[0];
    }

    @Override
    public final double[] toDoubleArray() {
        return new double[0];
    }

    @Override
    public final float[] toFloatArray() {
        return new float[0];
    }

    @Override
    public final int[] toIntArray() {
        return new int[0];
    }

    @Override
    public final long[] toLongArray() {
        return new long[0];
    }

    @Override
    public final short[] toShortArray() {
        return new short[0];
    }

    @Override
    public final String toString() {
        return null;
    }
}

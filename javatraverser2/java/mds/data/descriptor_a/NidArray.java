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
import mds.data.descriptor_s.Nid;
import mds.mdsip.Message;

public final class NidArray extends Descriptor_A<Nid>{
    private static final ByteBuffer getByteBuffer(final Nid... nids) {
        final ByteBuffer bb = ByteBuffer.allocate(nids.length * Integer.BYTES).order(Descriptor.BYTEORDER);
        for(final Nid nid : nids)
            bb.putInt(nid.getNidNumber());
        return (ByteBuffer)bb.rewind();
    }

    public NidArray(){
        super(DTYPE.NID, ByteBuffer.allocate(0));
    }

    public NidArray(final ByteBuffer b){
        super(b);
    }

    public NidArray(final Nid... nids){
        super(DTYPE.NID, NidArray.getByteBuffer(nids), nids.length);
        this.setTree(nids[0].getTree());
    }

    @Override
    protected DATA<?> getData_() throws MdsException {
        return this.evaluate().getData();
    }

    @Override
    public final Nid getElement(final ByteBuffer b_in) {
        return new Nid(b_in.getInt(), this.tree);
    }

    @Override
    public final Nid getElement(final int i) {
        return new Nid(this.p.getInt(i * Integer.BYTES), this.tree);
    }

    @Override
    public final Nid getScalar(final int idx) {
        return this.getElement(idx);
    }

    @Override
    protected final String getSuffix() {
        return "";
    }

    @Override
    protected final Nid[] initArray(final int size) {
        return new Nid[size];
    }

    @Override
    protected final void setElement(final ByteBuffer b, final Nid value) {
        b.putInt(value.getNidNumber());
    }

    @Override
    protected final void setElement(final int i, final Nid value) {
        this.p.putInt(i * Integer.BYTES, value.getNidNumber());
    }

    public final Nid[] toArray() {
        return this.getAtomic(0, this.getLength());
    }

    @Override
    public final BigInteger toBigInteger(final Nid t) {
        return t.toBigInteger();
    }

    @Override
    public final byte toByte(final Nid t) {
        return t.toByte();
    }

    @Override
    public final double toDouble(final Nid t) {
        return t.toDouble();
    }

    @Override
    public final float toFloat(final Nid t) {
        return t.toFloat();
    }

    @Override
    public final int toInt(final Nid t) {
        return t.toInt();
    }

    @Override
    public final long toLong(final Nid t) {
        return t.toLong();
    }

    @Override
    public final Message toMessage(final byte descr_idx, final byte n_args, final byte mid) throws MdsException {
        return new Message(descr_idx, DTYPE.L, n_args, this.getShape(), this.getBuffer(), mid);
    }

    @Override
    public final short toShort(final Nid t) {
        return t.toShort();
    }

    @Override
    protected final String toString(final Nid t) {
        return t.toString();
    }
}

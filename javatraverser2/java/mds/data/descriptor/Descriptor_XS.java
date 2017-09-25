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
package mds.data.descriptor;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.TREE;
import mds.data.descriptor_s.Missing;
import mds.mdsip.Message;

/** XS (-63 : 193) **/
public class Descriptor_XS extends Descriptor<Descriptor<?>>{
    public static final short _l_lenI = 8;
    @SuppressWarnings("hiding")
    public static final int   BYTES   = 12;
    public static final byte  CLASS   = -63;

    public static final Descriptor_XS deserialize(final ByteBuffer bb) throws MdsException {
        return new Descriptor_XS(bb);
    }

    protected static final ByteBuffer getPointer(final ByteBuffer bi) {
        ByteBuffer bo = bi.duplicate().order(bi.order());
        bo.position(bi.getInt(Descriptor._ptrI));
        bo = bo.slice().order(bi.order());
        bo.limit(bi.getInt(Descriptor_XS._l_lenI));
        return bo;
    }

    public Descriptor_XS(final ByteBuffer b){
        super(b);
        this.b.limit(this.pointer() + this.payload().getSize());
    }

    public Descriptor_XS(final Descriptor<?> value){
        super(Descriptor_XS._l_lenI, DTYPE.DSC, Descriptor_XS.CLASS, value.b, Descriptor_XS.BYTES, 0);
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return (this.pointer() == 0) ? pout.append("*") : this.payload().decompile(prec, pout, mode);
    }

    @Override
    public Descriptor<?> getAtomic() {
        return this.payload();
    }

    @Override
    protected DATA<?> getData_() throws MdsException {
        return this.pointer() == 0 ? Missing.NEW : this.payload().getData();
    }

    @Override
    public Descriptor_XS getLocal_() {
        return (Descriptor_XS)new Descriptor_XS(this.payload().getLocal()).setLocal();
    }

    @Override
    public int[] getShape() {
        return new int[0];
    }

    public final int l_length() {
        return this.b.getInt(Descriptor_XS._l_lenI);
    }

    public final Descriptor<?> payload() {
        try{
            return Descriptor.deserialize(this.b).setTree(this.tree);
        }catch(final MdsException e){
            e.printStackTrace();
            return Missing.NEW;
        }
    }

    @Override
    public final Descriptor<?> setTree(final TREE tree) {
        super.setTree(tree);
        return this;
    }

    @Override
    public final BigInteger[] toBigIntegerArray() {
        return this.pointer() == 0 ? null : this.payload().toBigIntegerArray();
    }

    @Override
    public final byte[] toByteArray() {
        return this.pointer() == 0 ? null : this.payload().toByteArray();
    }

    @Override
    public final double[] toDoubleArray() {
        return this.pointer() == 0 ? null : this.payload().toDoubleArray();
    }

    @Override
    public final float[] toFloatArray() {
        return this.pointer() == 0 ? null : this.payload().toFloatArray();
    }

    @Override
    public final int[] toIntArray() {
        return this.pointer() == 0 ? null : this.payload().toIntArray();
    }

    @Override
    public final long[] toLongArray() {
        return this.pointer() == 0 ? null : this.payload().toLongArray();
    }

    @Override
    public final Message toMessage(final byte descr_idx, final byte n_args, final byte msgid) throws MdsException {
        return this.pointer() == 0 ? null : this.payload().toMessage(descr_idx, n_args, msgid);
    }

    @Override
    public short[] toShortArray() {
        return this.pointer() == 0 ? null : this.payload().toShortArray();
    }

    @Override
    public final String[] toStringArray() {
        return this.pointer() == 0 ? null : this.payload().toStringArray();
    }
}

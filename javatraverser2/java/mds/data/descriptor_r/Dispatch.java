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
package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

public final class Dispatch extends Descriptor_R<Byte>{
    public static final byte SCHED_ASYNC = 1;
    public static final byte SCHED_COND  = 3;
    public static final byte SCHED_NONE  = 0;
    public static final byte SCHED_SEQ   = 2;

    public Dispatch(final byte type, final Descriptor<?> ident, final Descriptor<?> phase, final Descriptor<?> when, final Descriptor<?> completion){
        super(DTYPE.DISPATCH, ByteBuffer.allocate(Byte.BYTES).order(Descriptor.BYTEORDER).put(0, type), ident, phase, when, completion);
    }

    public Dispatch(final ByteBuffer b){
        super(b);
    }

    public Dispatch(final Descriptor<?>... arguments){
        super(DTYPE.DISPATCH, null, arguments);
    }

    @Override
    public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return Descriptor_R.decompile_build(this, prec, pout, mode);
    }

    @Override
    public final Byte getAtomic() {
        switch(this.length()){
            case 1:
                return Byte.valueOf(this.p.get(0));
            case 2:
                return Byte.valueOf((byte)this.p.getShort(0));
            case 4:
                return Byte.valueOf((byte)this.p.getInt(0));
            default:
                return Byte.valueOf((byte)0);
        }
    }

    public final Descriptor<?> getCompletion() {
        return this.getDescriptor(3);
    }

    public final Descriptor<?> getIdent() {
        return this.getDescriptor(0);
    }

    public final Descriptor<?> getPhase() {
        return this.getDescriptor(1);
    }

    public final byte getType() {
        return this.getAtomic().byteValue();
    }

    public final Descriptor<?> getWhen() {
        return this.getDescriptor(2);
    }
}

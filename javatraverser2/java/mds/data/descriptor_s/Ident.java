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
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_S;

public final class Ident extends Descriptor_S<String>{
    public Ident(final ByteBuffer b){
        super(b);
    }

    public final Descriptor<?> dec() throws MdsException {
        return this.mds.getDescriptor("`--$", this);
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return pout.append(this.getAtomic());
    }

    @Override
    public final Descriptor<?> evaluate() {
        try{
            return this.mds.getDescriptor(this.tree.ctx, "`$", this);
        }catch(final MdsException e){
            System.err.println(e);
            return Missing.NEW;
        }
    }

    @Override
    public final String getAtomic() {
        final byte[] buf = new byte[this.length()];
        this.getBuffer().get(buf);
        return new String(buf);
    }

    @Override
    public DATA<?> getData_() throws MdsException {
        return this.evaluate().getData();
    }

    public final String getName() {
        return this.getAtomic();
    }

    public final Descriptor<?> inc() throws MdsException {
        return this.mds.getDescriptor("`++$", this);
    }
}

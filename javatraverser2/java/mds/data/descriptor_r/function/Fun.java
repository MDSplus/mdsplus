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
package mds.data.descriptor_r.function;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.OPC;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Function;
import mds.data.descriptor_s.Missing;

public class Fun extends Function{
    public Fun(final ByteBuffer b){
        super(b);
    }

    public Fun(final Descriptor<?> a, final Descriptor<?> b){
        super(OPC.OpcFun, a, b);
    }

    public Fun(final Descriptor<?>[] args){
        super(OPC.OpcFun, args);
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        if(prec < Descriptor.P_STMT) pout.append('(');
        pout.append("Fun ");
        final Descriptor<?> ptr = this.getDescriptor(0);
        if(ptr.dtype() == DTYPE.T) pout.append(ptr.toString());
        else ptr.decompile(Descriptor.P_SUBS, pout, mode & ~Descriptor.DECO_X);
        this.addArguments(2, " (", ") ", pout, mode);
        Function.addCompoundStatement(1, this, 1, pout, mode);
        if(prec < Descriptor.P_STMT) pout.append(')');
        return pout;
    }

    @Override
    public final Descriptor<?> evaluate() {
        try{
            final DATA<?>[] args = Descriptor.getDATA(this.getArguments());
            return BINARY.getCommon(args).add(args[0].toDescriptor(), args[1].toDescriptor());
        }catch(final MdsException e){
            System.err.println(e.getMessage());
            return Missing.NEW;
        }
    }
}

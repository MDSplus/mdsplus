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
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;
import mds.data.descriptor_s.CString;

public final class Param extends Descriptor_R<Number> implements PARAMETER{
    public Param(final ByteBuffer b){
        super(b);
    }

    public Param(final Descriptor<?>... arguments){
        super(DTYPE.PARAM, null, arguments);
    }

    public Param(final Descriptor<?> value, final Descriptor<?> help, final Descriptor<?> valid){
        super(DTYPE.PARAM, null, value, help, valid);
    }

    public Param(final Descriptor<?> value, final String help, final Descriptor<?> valid){
        this(value, new CString(help), valid);
    }

    @Override
    public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return Descriptor_R.decompile_build(this, prec, pout, mode);
    }

    @Override
    public final DATA<?> getData_() throws MdsException {
        return this.getValue().getData();
    }

    public final Descriptor<?> getHelp() {
        return this.getDescriptor(1);
    }

    @Override
    public final Param getLocal_() {
        return (Param)new Param(this.getValue().getLocal(), this.getHelp().getLocal(), this.getValidation().getLocal()).setLocal();
    }

    @Override
    public final int[] getShape() {
        return this.getValue().getShape();
    }

    public final Descriptor<?> getValidation() {
        return this.getDescriptor(2);
    }

    @Override
    public final Descriptor<?> getValue() {
        return this.getDescriptor(0);
    }
}

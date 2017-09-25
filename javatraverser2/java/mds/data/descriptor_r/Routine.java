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

public final class Routine extends Descriptor_R<Number>{
    public Routine(final ByteBuffer b){
        super(b);
    }

    public Routine(final Descriptor<?>... arguments){
        super(DTYPE.ROUTINE, null, arguments);
    }

    public Routine(final Descriptor<?> time_out, final Descriptor<?> image, final Descriptor<?> routine){
        super(DTYPE.ROUTINE, null, time_out, image, routine);
    }

    public Routine(final Descriptor<?> time_out, final Descriptor<?> image, final Descriptor<?> routine, final Descriptor<?>... args){
        super(DTYPE.ROUTINE, null, args, time_out, image, routine);
    }

    @Override
    public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return Descriptor_R.decompile_build(this, prec, pout, mode);
    }

    public final Descriptor<?> getArgument(final int idx) {
        return this.getDescriptor(3 + idx);
    }

    public final Descriptor<?>[] getArguments() {
        final Descriptor<?>[] desc = new Descriptor[this.ndesc() - 3];
        for(int i = 0; i < desc.length; i++)
            desc[i] = this.getDescriptor(i + 3);
        return desc;
    }

    public final Descriptor<?> getImage() {
        return this.getDescriptor(1);
    }

    public final Descriptor<?> getRoutine() {
        return this.getDescriptor(2);
    }

    public final Descriptor<?> getTimeOut() {
        return this.getDescriptor(0);
    }
}

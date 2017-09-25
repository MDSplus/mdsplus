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
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

public final class Action extends Descriptor_R<Number>{
    private static final String status0 = "<html>Status: 0x00000000 , 0<br>The Action has not yet been dispatched or returned and unknown error</html>";

    public static final String getStatusMsg(final int status) {
        if(status == 0) return Action.status0;
        return new StringBuilder(256).append("<html>Status: 0x").append(String.format("%08X", new Long(status))).append(" , ").append(status).append("<br>").append(MdsException.getMdsMessage(status)).append("</html>").toString();
    }

    public Action(final ByteBuffer b){
        super(b);
    }

    public Action(final Descriptor<?>... arguments){
        super(DTYPE.ACTION, null, arguments);
    }

    public Action(final Descriptor<?> dispatch, final Descriptor<?> task, final Descriptor<?> errorlogs, final Descriptor<?> completion_message, final Descriptor<?> performance){
        super(DTYPE.ACTION, null, dispatch, task, errorlogs, completion_message, performance);
    }

    @Override
    public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return Descriptor_R.decompile_build(this, prec, pout, mode);
    }

    public final Descriptor<?> getCompletionMessage() {
        return this.getDescriptor(3);
    }

    public final Descriptor<?> getDispatch() {
        return this.getDescriptor(0);
    }

    public final Descriptor<?> getErrorLogs() {
        return this.getDescriptor(2);
    }

    public final Descriptor<?> getPerformance() {
        return this.getDescriptor(4);
    }

    public final Descriptor<?> getTask() {
        return this.getDescriptor(1);
    }
}

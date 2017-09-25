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
import mds.data.descriptor_a.Float32Array;
import mds.data.descriptor_s.Missing;
import mds.data.descriptor_s.Uint8;

public final class Dim extends Descriptor_R<Number>{
    public Dim(final ByteBuffer b){
        super(b);
    }

    public Dim(final Descriptor<?>... arguments){
        super(DTYPE.DIMENSION, null, arguments);
    }

    public Dim(final Descriptor<?> window, final Descriptor<?> axis){
        super(DTYPE.DIMENSION, null, window, axis);
    }

    @Override
    public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return Descriptor_R.decompile_build(this, prec, pout, mode);
    }

    public final Descriptor<?> getAxis() {
        return this.getDescriptor(1);
    }

    @Override
    public final DATA<?> getData_() throws MdsException {
        float begin = Float.NEGATIVE_INFINITY, ending = Float.POSITIVE_INFINITY, origin = 0, delta = 1;
        if(this.getAxis() instanceof Range){
            if(((Range)(this.getAxis())).getDelta() != Missing.NEW) delta = ((Range)(this.getAxis())).getDelta().toFloat();
            if(((Range)(this.getAxis())).getBegin() != Missing.NEW) begin = ((Range)(this.getAxis())).getBegin().toFloat();
            if(((Range)(this.getAxis())).getEnding() != Missing.NEW) ending = ((Range)(this.getAxis())).getEnding().toFloat();
        }
        if(this.getWindow() instanceof Window){
            if(((Window)(this.getWindow())).getValueAtIdx0() != Missing.NEW) origin = ((Window)(this.getWindow())).getValueAtIdx0().toFloat();
            if(((Window)(this.getWindow())).getStartingIdx() != Missing.NEW) begin = Math.max(begin, origin + delta * ((Window)(this.getWindow())).getStartingIdx().toFloat());
            if(((Window)(this.getWindow())).getEndingIdx() != Missing.NEW) ending = Math.min(ending, origin + delta * ((Window)(this.getWindow())).getEndingIdx().toFloat());
        }
        if(begin == Float.NEGATIVE_INFINITY || ending == Float.POSITIVE_INFINITY) return new Uint8(1);
        final int elements = (int)((ending - begin) / delta);
        final ByteBuffer buf = ByteBuffer.allocate(elements * Float.SIZE).order(Descriptor.BYTEORDER);
        for(int i = 0; i < elements; i++)
            buf.putFloat(begin + i * delta);
        return new Float32Array(DTYPE.FLOAT, buf, new int[]{elements});
    }

    @Override
    public final Dim getLocal_() {
        return (Dim)new Dim(this.getWindow().getLocal(), this.getAxis().getLocal()).setLocal();
    }

    public final Descriptor<?> getWindow() {
        return this.getDescriptor(0);
    }
}

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

import java.math.BigDecimal;
import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;
import mds.data.descriptor_a.Float64Array;
import mds.data.descriptor_a.Int32Array;
import mds.data.descriptor_a.Int64Array;
import mds.data.descriptor_s.FLOAT;
import mds.data.descriptor_s.Float64;
import mds.data.descriptor_s.Int32;
import mds.data.descriptor_s.Int64;
import mds.data.descriptor_s.Missing;

public final class Range extends Descriptor_R<Number>{
    public static double[] range(final double begin, final double ending, final double delta) {
        BigDecimal strt = BigDecimal.valueOf(begin);
        final BigDecimal stop = BigDecimal.valueOf(ending);
        final BigDecimal delt = BigDecimal.valueOf(delta);
        final int n = stop.subtract(strt).divide(delt).intValue();
        final double[] array = new double[n + 1];
        for(int i = 0;; i++){
            array[i] = strt.doubleValue();
            if(i >= n) break;
            strt = strt.add(delt);
        }
        return array;
    }

    public static int[] range(int begin, final int ending, final int delta) {
        final int n = (ending - begin) / delta;
        final int[] array = new int[n + 1];
        for(int i = 0;; i++){
            array[i] = begin;
            if(i >= n) break;
            begin += delta;
        }
        return array;
    }

    public static long[] range(long begin, final long ending, final long delta) {
        final int n = (int)((ending - begin) / delta);
        final long[] array = new long[n + 1];
        for(int i = 0;; i++){
            array[i] = begin;
            if(i >= n) break;
            begin += delta;
        }
        return array;
    }

    public Range(final ByteBuffer b){
        super(b);
    }

    public Range(final Descriptor<?>... arguments){
        super(DTYPE.RANGE, null, arguments);
    }

    public Range(final Descriptor<?> begin, final Descriptor<?> ending){
        super(DTYPE.RANGE, null, begin, ending, null);
    }

    public Range(final Descriptor<?> begin, final Descriptor<?> ending, final Descriptor<?> delta){
        super(DTYPE.RANGE, null, begin, ending, delta);
    }

    public Range(final double begin, final double ending){
        this(new Float64(begin), new Float64(ending), null);
    }

    public Range(final double begin, final double ending, final double delta){
        this(new Float64(begin), new Float64(ending), new Float64(delta));
    }

    public Range(final int begin, final int ending){
        this(new Int32(begin), new Int32(ending), null);
    }

    public Range(final int begin, final int ending, final int delta){
        this(new Int32(begin), new Int32(ending), new Int32(delta));
    }

    public Range(final long begin, final long ending){
        this(new Int64(begin), new Int64(ending), null);
    }

    public Range(final long begin, final long ending, final long delta){
        this(new Int64(begin), new Int64(ending), new Int64(delta));
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        this.getBegin().decompile(prec, pout, mode).append(" : ");
        this.getEnding().decompile(prec, pout, mode);
        if(this.getDelta() != Missing.NEW) this.getDelta().decompile(prec, pout.append(" : "), mode);
        return pout;
    }

    public final Descriptor<?> getBegin() {
        return this.getDescriptor(0);
    }

    @Override
    public final DATA<?> getData_() throws MdsException {
        if(this.getBegin() == Missing.NEW || this.getEnding() == Missing.NEW) MdsException.handleStatus(MdsException.TdiBOMB);
        if(this.getBegin() instanceof FLOAT || this.getEnding() instanceof FLOAT || this.getDelta() instanceof FLOAT) return new Float64Array(Range.range(this.getBegin().toDouble(), this.getEnding().toDouble(), this.getDelta() == Missing.NEW ? 1d : this.getDelta().toDouble()));
        if(this.getBegin() instanceof Int64 || this.getEnding() instanceof Int64 || this.getDelta() instanceof Int64) return new Int64Array(Range.range(this.getBegin().toLong(), this.getEnding().toLong(), this.getDelta() == Missing.NEW ? 1l : this.getDelta().toLong()));
        return new Int32Array(Range.range(this.getBegin().toInt(), this.getEnding().toInt(), this.getDelta() == Missing.NEW ? 1 : this.getDelta().toInt()));
    }

    public final Descriptor<?> getDelta() {
        return this.getDescriptor(2);
    }

    public final Descriptor<?> getEnding() {
        return this.getDescriptor(1);
    }

    @Override
    public final Range getLocal_() {
        return (Range)new Range(this.getBegin().getLocal(), this.getEnding().getLocal(), this.getDelta().getLocal()).setLocal();
    }

    @Override
    public final int[] getShape() {
        return new int[]{(int)((this.getEnding().toFloat() - this.getBegin().toFloat()) / this.getDelta().toFloat())};
    }
}

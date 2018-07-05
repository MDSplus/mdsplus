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
        if(!Descriptor.isMissing(this.getDelta())) this.getDelta().decompile(prec, pout.append(" : "), mode);
        return pout;
    }

    public final Descriptor<?> getBegin() {
        return this.getDescriptor(0);
    }

    @Override
    public final DATA<?> getData_() throws MdsException {
        final Descriptor<?> begin = Descriptor.getLocal(null, this.getBegin());
        final Descriptor<?> end = Descriptor.getLocal(null, this.getEnding());
        final Descriptor<?> delta = Descriptor.getLocal(null, this.getDelta());
        if(Descriptor.isMissing(begin) || Descriptor.isMissing(end)) MdsException.handleStatus(MdsException.TdiBOMB);
        if(begin instanceof FLOAT || end instanceof FLOAT || delta instanceof FLOAT) return new Float64Array(Range.range(begin.toDouble(), end.toDouble(), Descriptor.isMissing(delta) ? 1d : delta.toDouble()));
        if(begin instanceof Int64 || end instanceof Int64 || delta instanceof Int64) return new Int64Array(Range.range(begin.toLong(), end.toLong(), Descriptor.isMissing(delta) ? 1l : delta.toLong()));
        return new Int32Array(Range.range(begin.toInt(), end.toInt(), Descriptor.isMissing(delta) ? 1 : delta.toInt()));
    }

    public final Descriptor<?> getDelta() {
        return this.getDescriptor(2);
    }

    public final Descriptor<?> getEnding() {
        return this.getDescriptor(1);
    }

    @Override
    public final Range getLocal_(final FLAG local) {
        final FLAG mylocal = new FLAG();
        final Descriptor<?> begin = Descriptor.getLocal(mylocal, this.getBegin());
        final Descriptor<?> end = Descriptor.getLocal(mylocal, this.getEnding());
        final Descriptor<?> delta = Descriptor.getLocal(mylocal, this.getDelta());
        if(FLAG.and(local, mylocal.flag)) return (Range)this.setLocal();
        return (Range)new Range(begin, end, delta).setLocal();
    }

    @Override
    public final int[] getShape() {
        return new int[]{(int)((this.getEnding().toFloat() - this.getBegin().toFloat()) / this.getDelta().toFloat())};
    }
}

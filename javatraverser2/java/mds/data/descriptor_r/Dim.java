package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;
import mds.data.descriptor_a.Float32Array;
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
            if(!Descriptor.isMissing(((Range)(this.getAxis())).getDelta())) delta = ((Range)(this.getAxis())).getDelta().toFloat();
            if(!Descriptor.isMissing(((Range)(this.getAxis())).getBegin())) begin = ((Range)(this.getAxis())).getBegin().toFloat();
            if(!Descriptor.isMissing(((Range)(this.getAxis())).getEnding())) ending = ((Range)(this.getAxis())).getEnding().toFloat();
        }
        if(this.getWindow() instanceof Window){
            if(!Descriptor.isMissing(((Window)(this.getWindow())).getValueAtIdx0())) origin = ((Window)(this.getWindow())).getValueAtIdx0().toFloat();
            if(!Descriptor.isMissing(((Window)(this.getWindow())).getStartingIdx())) begin = Math.max(begin, origin + delta * ((Window)(this.getWindow())).getStartingIdx().toFloat());
            if(!Descriptor.isMissing(((Window)(this.getWindow())).getEndingIdx())) ending = Math.min(ending, origin + delta * ((Window)(this.getWindow())).getEndingIdx().toFloat());
        }
        if(begin == Float.NEGATIVE_INFINITY || ending == Float.POSITIVE_INFINITY) return new Uint8(1);
        final int elements = (int)((ending - begin) / delta);
        final ByteBuffer buf = ByteBuffer.allocate(elements * Float.SIZE).order(Descriptor.BYTEORDER);
        for(int i = 0; i < elements; i++)
            buf.putFloat(begin + i * delta);
        return new Float32Array(DTYPE.FLOAT, buf, new int[]{elements});
    }

    @Override
    public final Dim getLocal_(final FLAG local) {
        final FLAG mylocal = new FLAG();
        final Descriptor<?> win = this.getWindow().getLocal(mylocal);
        final Descriptor<?> axis = this.getAxis().getLocal(mylocal);
        if(FLAG.and(local, mylocal.flag)) return this;
        return (Dim)new Dim(win, axis).setLocal();
    }

    public final Descriptor<?> getWindow() {
        return this.getDescriptor(0);
    }
}

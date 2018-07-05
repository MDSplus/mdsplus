package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

public final class Signal extends Descriptor_R<Number>{
    public Signal(final ByteBuffer b){
        super(b);
    }

    public Signal(final Descriptor<?>... arguments){
        super(DTYPE.SIGNAL, null, arguments);
    }

    @Override
    public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return Descriptor_R.decompile_build(this, prec, pout, mode);
    }

    @Override
    protected final DATA<?> getData_() throws MdsException {
        try{
            return this.getValue().getData();
        }catch(final MdsException e){
            return this.mds.getDescriptor(this.tree, "DATA($)", this).getData();
        }
    }

    public final Descriptor<?> getDimension() {
        return this.getDimension(0);
    }

    public final Descriptor<?> getDimension(final int idx) {
        return this.getDescriptor(2 + idx);
    }

    @Override
    public Signal getLocal_(final FLAG local) {
        final FLAG mylocal = new FLAG();
        final Descriptor<?>[] dscs = new Descriptor<?>[this.ndesc()];
        for(int i = 0; i < dscs.length; i++)
            dscs[i] = Descriptor.getLocal(mylocal, this.getDescriptor(i));
        if(FLAG.and(local, mylocal.flag)) return (Signal)this.setLocal();
        return (Signal)new Signal(dscs).setLocal();
    }

    public final Descriptor<?> getRaw() {
        return this.getDescriptor(1);
    }

    @Override
    public final int[] getShape() {
        return this.getValue().getShape();
    }

    public final Descriptor<?> getValue() {
        return this.getDescriptor(0);
    }

    @Override
    public final Descriptor<?> VALUE() {
        return this.getRaw();
    }
}

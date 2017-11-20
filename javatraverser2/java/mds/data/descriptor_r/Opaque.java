package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

public final class Opaque extends Descriptor_R<Number> implements PARAMETER{
    public Opaque(final ByteBuffer b){
        super(b);
    }

    public Opaque(final Descriptor<?>... arguments){
        super(DTYPE.OPAQUE, null, arguments);
    }

    public Opaque(final Descriptor<?> value, final Descriptor<?> opaque_type){
        super(DTYPE.OPAQUE, null, value);
    }

    @Override
    public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return Descriptor_R.decompile_build(this, prec, pout, mode);
    }

    @Override
    public final DATA<?> getData_() throws MdsException {
        return this.getValue().getData();
    }

    @Override
    public final Opaque getLocal_(final FLAG local) {
        final FLAG mylocal = new FLAG();
        final Descriptor<?> value = Descriptor.getLocal(mylocal, this.getValue());
        final Descriptor<?> type = Descriptor.getLocal(mylocal, this.getOpaqueType());
        if(FLAG.and(local, mylocal.flag)) return (Opaque)this.setLocal();
        return (Opaque)new Opaque(value, type).setLocal();
    }

    public final Descriptor<?> getOpaqueType() {
        return this.getDescriptor(1);
    }

    @Override
    public final int[] getShape() {
        return this.getValue().getShape();
    }

    @Override
    public final Descriptor<?> getValue() {
        return this.getDescriptor(0);
    }
}

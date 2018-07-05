package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

public final class With_Error extends Descriptor_R<Number> implements PARAMETER{
    public With_Error(final ByteBuffer b){
        super(b);
    }

    public With_Error(final Descriptor<?>... arguments){
        super(DTYPE.WITH_ERROR, null, arguments);
    }

    public With_Error(final Descriptor<?> value, final Descriptor<?> error){
        super(DTYPE.WITH_ERROR, null, value, error);
    }

    @Override
    public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return Descriptor_R.decompile_build(this, prec, pout, mode);
    }

    @Override
    public final DATA<?> getData_() throws MdsException {
        return this.getValue().getData();
    }

    public final Descriptor<?> getError() {
        return this.getDescriptor(1);
    }

    @Override
    public final With_Error getLocal_(final FLAG local) {
        final FLAG mylocal = new FLAG();
        final Descriptor<?> value = Descriptor.getLocal(mylocal, this.getValue());
        final Descriptor<?> error = Descriptor.getLocal(mylocal, this.getError());
        if(FLAG.and(local, mylocal.flag)) return (With_Error)this.setLocal();
        return (With_Error)new With_Error(value, error).setLocal();
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

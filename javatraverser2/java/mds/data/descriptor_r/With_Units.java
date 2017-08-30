package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;
import mds.data.descriptor_s.CString;

public final class With_Units extends Descriptor_R<Number> implements PARAMETER{
    public With_Units(final ByteBuffer b){
        super(b);
    }

    public With_Units(final Descriptor<?>... arguments){
        super(DTYPE.WITH_UNITS, null, arguments);
    }

    public With_Units(final Descriptor<?> value, final Descriptor<?> units){
        super(DTYPE.WITH_UNITS, null, value, units);
    }

    public With_Units(final Descriptor<?> value, final String units){
        this(value, new CString(units));
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
    public final With_Units getLocal_() {
        return (With_Units)new With_Units(this.getValue().getLocal(), this.getUnits().getLocal()).setLocal();
    }

    @Override
    public final int[] getShape() {
        return this.getValue().getShape();
    }

    public final Descriptor<?> getUnits() {
        return this.getDescriptor(1);
    }

    @Override
    public final Descriptor<?> getValue() {
        return this.getDescriptor(0);
    }
}

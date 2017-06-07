package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

public final class Window extends Descriptor_R<Number>{
    public Window(final ByteBuffer b){
        super(b);
    }

    public Window(final Descriptor<?>... arguments){
        super(DTYPE.WINDOW, null, arguments);
    }

    public Window(final Descriptor<?> startingidx, final Descriptor<?> endingidx, final Descriptor<?> valueat0){
        super(DTYPE.WINDOW, null, startingidx, endingidx, valueat0);
    }

    @Override
    public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return Descriptor_R.decompile_build(this, prec, pout, mode);
    }

    public final Descriptor<?> getEndingIdx() {
        return this.getDescriptor(1);
    }

    @Override
    public final Window getLocal_() {
        return (Window)new Window(this.getStartingIdx().getLocal(), this.getEndingIdx().getLocal(), this.getValueAtIdx0().getLocal()).setLocal();
    }

    @Override
    public final int[] getShape() {
        return new int[]{this.getEndingIdx().toInt() - this.getStartingIdx().toInt()};
    }

    public final Descriptor<?> getStartingIdx() {
        return this.getDescriptor(0);
    }

    public final Descriptor<?> getValueAtIdx0() {
        return this.getDescriptor(2);
    }
}

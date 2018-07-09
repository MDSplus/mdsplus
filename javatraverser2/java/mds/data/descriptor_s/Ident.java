package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_S;
import mds.mdsip.Message;

public final class Ident extends Descriptor_S<String>{
    public Ident(final ByteBuffer b){
        super(b);
    }

    public final Descriptor<?> dec() throws MdsException {
        return this.mds.getDescriptor("`--$", this);
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return pout.append(this.getAtomic());
    }

    @Override
    public final Descriptor<?> evaluate() {
        try{
            return this.mds.getDescriptor(this.tree, "`$", this);
        }catch(final MdsException e){
            System.err.println(e);
            return Missing.NEW;
        }
    }

    @Override
    public final String getAtomic() {
        final byte[] buf = new byte[this.length()];
        this.getBuffer().get(buf);
        return new String(buf);
    }

    @Override
    public DATA<?> getData_() throws MdsException {
        return this.evaluate().getData();
    }

    public final String getName() {
        return this.getAtomic();
    }

    public final Descriptor<?> inc() throws MdsException {
        return this.mds.getDescriptor("`++$", this);
    }

    @Override
    public Message toMessage(final byte descr_idx, final byte n_args, final byte mid) throws MdsException {
        return new Message(descr_idx, this.dtype(), n_args, this.getShape(), this.getBuffer(), mid);
    }
}

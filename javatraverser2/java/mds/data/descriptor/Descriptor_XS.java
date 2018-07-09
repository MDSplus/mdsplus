package mds.data.descriptor;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.TREE;
import mds.data.descriptor_s.Missing;
import mds.mdsip.Message;

/** XS (-63 : 193) **/
public class Descriptor_XS extends Descriptor<Descriptor<?>>{
    public static final short _l_lenI = 8;
    @SuppressWarnings("hiding")
    public static final int   BYTES   = 12;
    public static final byte  CLASS   = -63;

    public static final Descriptor_XS deserialize(final ByteBuffer bb) throws MdsException {
        return new Descriptor_XS(bb);
    }

    protected static final ByteBuffer getPointer(final ByteBuffer bi) {
        ByteBuffer bo = bi.duplicate().order(bi.order());
        bo.position(bi.getInt(Descriptor._ptrI));
        bo = bo.slice().order(bi.order());
        bo.limit(bi.getInt(Descriptor_XS._l_lenI));
        return bo;
    }

    public Descriptor_XS(final ByteBuffer b){
        super(b);
        this.b.limit(this.pointer() + this.payload().getSize());
    }

    public Descriptor_XS(final Descriptor<?> value){
        super(Descriptor_XS._l_lenI, DTYPE.DSC, Descriptor_XS.CLASS, value.b, Descriptor_XS.BYTES, 0);
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return (this.pointer() == 0) ? pout.append("*") : this.payload().decompile(prec, pout, mode);
    }

    @Override
    public Descriptor<?> getAtomic() {
        return this.payload();
    }

    @Override
    protected DATA<?> getData_() throws MdsException {
        return this.pointer() == 0 ? Missing.NEW : this.payload().getData();
    }

    @Override
    public Descriptor_XS getLocal_(final FLAG local) {
        final FLAG mylocal = new FLAG();
        final Descriptor<?> dsc = Descriptor.getLocal(mylocal, this.payload());
        if(FLAG.and(local, mylocal.flag)) return this;
        return (Descriptor_XS)new Descriptor_XS(dsc).setLocal();
    }

    @Override
    public int[] getShape() {
        return new int[0];
    }

    public final int l_length() {
        return this.b.getInt(Descriptor_XS._l_lenI);
    }

    public final Descriptor<?> payload() {
        try{
            return Descriptor.deserialize(this.b).setTree(this.tree);
        }catch(final MdsException e){
            e.printStackTrace();
            return Missing.NEW;
        }
    }

    @Override
    public final Descriptor<?> setTree(final TREE tree) {
        super.setTree(tree);
        return this;
    }

    @Override
    public final BigInteger[] toBigIntegerArray() {
        return this.pointer() == 0 ? null : this.payload().toBigIntegerArray();
    }

    @Override
    public final byte[] toByteArray() {
        return this.pointer() == 0 ? null : this.payload().toByteArray();
    }

    @Override
    public final double[] toDoubleArray() {
        return this.pointer() == 0 ? null : this.payload().toDoubleArray();
    }

    @Override
    public final float[] toFloatArray() {
        return this.pointer() == 0 ? null : this.payload().toFloatArray();
    }

    @Override
    public final int[] toIntArray() {
        return this.pointer() == 0 ? null : this.payload().toIntArray();
    }

    @Override
    public final long[] toLongArray() {
        return this.pointer() == 0 ? null : this.payload().toLongArray();
    }

    @Override
    public final Message toMessage(final byte descr_idx, final byte n_args, final byte msgid) throws MdsException {
        return this.pointer() == 0 ? null : this.payload().toMessage(descr_idx, n_args, msgid);
    }

    @Override
    public short[] toShortArray() {
        return this.pointer() == 0 ? null : this.payload().toShortArray();
    }

    @Override
    public final String[] toStringArray() {
        return this.pointer() == 0 ? null : this.payload().toStringArray();
    }
}

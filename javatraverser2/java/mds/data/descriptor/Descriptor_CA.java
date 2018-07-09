package mds.data.descriptor;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.TREE;
import mds.data.descriptor_a.EmptyArray;
import mds.data.descriptor_r.function.COMPRESSION;
import mds.mdsip.Message;
import mds.mdslib.MdsLib;

/** Compressed Array Descriptor (-61 : 195) **/
public class Descriptor_CA extends ARRAY<ByteBuffer>{
    @SuppressWarnings("hiding")
    public static final byte CLASS = -61;// 195

    public static final Descriptor_CA deserialize(final ByteBuffer b) {
        return new Descriptor_CA(b);
    }
    public Descriptor_A<?>        decmprs;
    private final Descriptor_R<?> payload;

    public Descriptor_CA(final ByteBuffer b){
        super(b);
        this.payload = null;
    }

    public Descriptor_CA(final ByteBuffer header, final Descriptor_R<?> payload){
        super(header);
        this.payload = payload;
    }

    @Override
    @SuppressWarnings("unchecked")
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        if(this.payload() == null) return this.substitute(pout);
        try{
            @SuppressWarnings("rawtypes")
            final Descriptor_A that = COMPRESSION.decompress(this, 1);
            if(that.format()) pout.append(this.getDTypeName()).append('(');
            pout.append("Set_Range(");
            if(that.dimct() == 0) pout.append("0,");
            else for(int i = 0; i < this.dimct(); i++)
                pout.append(Integer.toString(this.dims(i))).append(',');
            that.decompile(pout, that.getElement(0));
            if(that.format()) pout.append(that.getSuffix());
            pout.append(" /*** etc. ***/)");
            if(that.format()) pout.append(')');
            return pout;
        }catch(final MdsException e){
            e.printStackTrace();
            return this.payload().decompile(prec, pout, mode);
        }
    }

    @Override
    public final int dims(final int idx) {
        if(this.dimct() > 1) return this.b.getInt(ARRAY._dmsIa + idx * Integer.BYTES);
        return idx == 0 ? this.arsize() / this.length() : 0;
    }

    @Override
    public ByteBuffer getAtomic() {
        return this.getBuffer();
    }

    @Override
    public final DATA<?> getData_() throws MdsException {
        return this.pointer() == 0 ? null : this.unpack().getData();
    }

    @Override
    public Descriptor_CA getLocal_(final FLAG local) {
        return this;
    }

    @Override
    public boolean isLocal() {
        return true;
    }

    public final Descriptor_R<?> payload() {
        if(this.payload != null) return this.payload;
        if(this.pointer() == 0) return null;
        try{
            return (Descriptor_R<?>)Descriptor_R.deserialize(this.getBuffer()).setTree(this.tree);
        }catch(final MdsException e){
            return null;
        }
    }

    @Override
    public final Descriptor<?> setTree(final TREE tree) {
        super.setTree(tree);
        if(this.payload != null) this.payload.setTree(tree);
        if(this.decmprs != null) this.decmprs.setTree(tree);
        return this;
    }

    private final StringBuilder substitute(final StringBuilder pout) {
        return pout.append("ZERO(").append(this.arsize() / this.length()).append(", 0").append(DTYPE.getSuffix(this.dtype())).append(')');
    }

    @Override
    public final BigInteger[] toBigIntegerArray() {
        return this.pointer() == 0 ? null : this.unpack().toBigIntegerArray();
    }

    @Override
    public final byte[] toByteArray() {
        return this.pointer() == 0 ? null : this.unpack().toByteArray();
    }

    @Override
    public final double[] toDoubleArray() {
        return this.pointer() == 0 ? null : this.unpack().toDoubleArray();
    }

    @Override
    public final float[] toFloatArray() {
        return this.pointer() == 0 ? null : this.unpack().toFloatArray();
    }

    @Override
    public final int[] toIntArray() {
        return this.pointer() == 0 ? null : this.unpack().toIntArray();
    }

    @Override
    public final long[] toLongArray() {
        return this.pointer() == 0 ? null : this.unpack().toLongArray();
    }

    @Override
    public final Message toMessage(final byte descr_idx, final byte n_args, final byte msgid) throws MdsException {
        return this.payload.toMessage(descr_idx, n_args, msgid);
    }

    @Override
    public short[] toShortArray() {
        return this.pointer() == 0 ? null : this.unpack().toShortArray();
    }

    @Override
    public final String[] toStringArray() {
        return this.pointer() == 0 ? null : this.unpack().toStringArray();
    }

    public final Descriptor_A<?> unpack() {
        if(this.decmprs != null) return this.decmprs;
        try{
            if(this.isLocal() && MdsLib.lib_loaded == null) return (Descriptor_A<?>)Descriptor.mdslib.getDescriptor("DATA($)", this);
            return(this.decmprs = COMPRESSION.decompress(this));
        }catch(final MdsException e){
            e.printStackTrace();
            return EmptyArray.NEW;
        }
    }
}
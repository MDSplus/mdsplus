package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DTYPE;
import mds.data.TREE;
import mds.data.descriptor.Descriptor;

public final class Path extends NODE<String>{
    public static final Descriptor<?> make(final String in) {
        return in == null ? Missing.NEW : new Path(in);
    }

    public Path(final ByteBuffer b){
        super(b);
    }

    public Path(final Descriptor<?>... arguments){
        super(DTYPE.PATH, arguments[0].getBuffer());
    }

    public Path(final String path){
        super(DTYPE.PATH, ByteBuffer.wrap(path.getBytes()).order(Descriptor.BYTEORDER));
    }

    public Path(final String path, final TREE tree){
        super(DTYPE.PATH, ByteBuffer.wrap(path.getBytes()).order(Descriptor.BYTEORDER));
        this.tree = tree;
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return pout.append(this.getAtomic());
    }

    @Override
    public final String getAtomic() {
        final byte[] buf = new byte[this.length()];
        this.getBuffer().get(buf);
        return new String(buf);
    }

    @Override
    public Descriptor<?> getLocal_(final FLAG local) {
        FLAG.set(local, false);
        return this.evaluate().setLocal();
    }

    @Override
    public final int getNidNumber() throws MdsException {
        return this.tree.getNci(this, "NID_NUMBER").toInt();
    }

    @Override
    public final Path toFullPath() throws MdsException {
        return new Path(this.getNciFullPath(), this.tree);
    }

    @Override
    public final Path toMinPath() throws MdsException {
        return new Path(this.getNciMinPath(), this.tree);
    }

    @Override
    public final Nid toNid() throws MdsException {
        return new Nid(this.getNciNidNumber(), this.tree);
    }

    @Override
    public final Path toPath() throws MdsException {
        return new Path(this.getNciPath(), this.tree);
    }

    @Override
    public final String toString() {
        return this.getAtomic();
    }
}

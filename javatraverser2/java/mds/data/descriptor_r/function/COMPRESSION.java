package mds.data.descriptor_r.function;

import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import debug.DEBUG;
import mds.MdsException;
import mds.data.DTYPE;
import mds.data.OPC;
import mds.data.descriptor.ARRAY;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;
import mds.data.descriptor.Descriptor_CA;
import mds.data.descriptor_r.Function;
import mds.data.descriptor_s.Missing;

public abstract class COMPRESSION extends Function{
    public static final class Decompress extends COMPRESSION{
        public Decompress(final ByteBuffer b){
            super(b);
        }

        public Decompress(final Descriptor<?> image, final Descriptor<?> routine, final Descriptor<?> shape, final Descriptor<?> data){
            super(OPC.OpcDecompress, image, routine, shape, data);
        }

        @Override
        public Descriptor<?> evaluate() {
            if(this.pointer() != 0 && !Descriptor.isMissing(this.getArgument(0))) return Missing.NEW;
            try{
                return COMPRESSION.decompress((ARRAY<?>)this.getArgument(1), (ARRAY<?>)this.getArgument(2));
            }catch(final MdsException e){
                e.printStackTrace();
                return Missing.NEW;
            }
        }
    }
    private static final int   MAXX    = 1024;
    // private static final int MAXY = 32;
    private static final int   BITSX   = 10;
    private static final int   BITSY   = 6;
    private static final int   FIELDSX = 2;
    private static final int   FIELDSY = COMPRESSION.BITSY + COMPRESSION.BITSX;
    private static final int[] MASKS   = new int[]{0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0xff, 0x1ff, 0x3ff, 0x7ff, 0xfff, 0x1fff, 0x3fff, 0x7fff, 0xffff, 0x1ffff, 0x3ffff, 0x7ffff, 0xfffff, 0x1fffff, 0x3fffff, 0x7fffff, 0xffffff, 0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff,};
    private static final int   MASKX   = COMPRESSION.MASKS[COMPRESSION.BITSX];
    private static final int   MASKY   = COMPRESSION.MASKS[COMPRESSION.BITSY];

    public static Descriptor_A<?> compress(final Descriptor_A<?> items) {
        /** compress **/
        return items;
    }

    public static final boolean coversOpCode(final short opcode) {
        switch(opcode){
            default:
                return false;
            case OPC.OpcDecompress:
                return true;
        }
    }

    public static Descriptor_A<?> decompress(final ARRAY<?> items, final ARRAY<?> pack) throws MdsException {
        return COMPRESSION.decompress(pack, items, -1);
    }

    public static Descriptor_A<?> decompress(ARRAY<?> items, ARRAY<?> pack, int preview) throws MdsException {
        if(DEBUG.D) System.out.println("decompress: " + items.getDTypeName() + items.arsize());
        if(pack instanceof Descriptor_CA) pack = COMPRESSION.decompress((Descriptor_CA)pack);
        if(preview == -1) preview = items.getLength();
        else{
            final ByteBuffer buf = ByteBuffer.allocate(items.pointer()).order(Descriptor.BYTEORDER);
            buf.put(items.serializeArray(), 0, items.pointer()).rewind();
            buf.putInt(ARRAY._aszI, preview * items.length());
            items = Descriptor_A.deserialize(buf);
        }
        return Descriptor_A.deserialize(COMPRESSION.mdsXpand(preview, pack, items));
    }

    public static final Descriptor_A<?> decompress(final Descriptor_CA ca) throws MdsException {
        return COMPRESSION.decompress(ca, -1);
    }

    public static final Descriptor_A<?> decompress(final Descriptor_CA ca, final int preview) throws MdsException {
        return COMPRESSION.decompress(ca, (ARRAY<?>)ca.payload().getDescriptor(3), preview);
    }

    public static COMPRESSION deserialize(final ByteBuffer b) throws MdsException {
        final short opcode = b.getShort(b.getInt(Descriptor._ptrI));
        switch(opcode){
            case OPC.OpcDecompress:
                return new Decompress(b);
        }
        throw new MdsException(MdsException.TdiINV_OPC);
    }

    /**
     * mdsUnpk expects LittleEndian buffers
     *
     * @return
     **/
    private static final Buffer mdsUnpk(final int nbits, int nitems, final ByteBuffer pack, final IntBuffer items, final int[] bit) {
        pack.position((bit[0] >> 5) * Integer.BYTES);
        items.rewind();
        if(DEBUG.D) System.err.print(new StringBuilder(128).append("mdsUnpk(").append(nbits).append(", ").append(nitems).append("[..").append(pack.getInt(pack.position())).append("..], ").append(bit[0]).append(" -> ").toString());
        if(nbits == 0){// zero fill
            if(DEBUG.D) System.err.println("zero fill");
            for(int i = 0; i < nitems; i++)
                items.put(0);
            return items.rewind();
        }
        final int size = nbits >= 0 ? nbits : -nbits;
        int off = bit[0] & 31;
        final int test = 32 - size;
        bit[0] += size * nitems;
        if(test == 0){// 32-bit data
            if(DEBUG.D) System.err.println("32-bit data");
            if((off & 7) == 0){// is multiple of byte
                pack.position(pack.position() + (off >> 3));
                for(; nitems-- > 0;)
                    items.put(pack.getInt());
                return items.rewind();
            }
            int hold;
            final int ioff, masko = COMPRESSION.MASKS[ioff = 32 - off];
            for(; nitems-- > 0;){
                hold = (pack.getInt() >> off) & masko;
                hold |= pack.getInt(pack.position()) << ioff;
                items.put(hold);
            }
            return items.rewind();
        }
        final int mask = COMPRESSION.MASKS[size], full = mask + 1, max = mask >> 1;
        int ioff, masko, hold;
        if(nbits < 0){ // sign extended
            if(DEBUG.D) System.err.println("sign extended");
            for(; nitems-- > 0;){
                if(off >= test){
                    masko = COMPRESSION.MASKS[ioff = 32 - off];
                    hold = (pack.getInt() >> off) & masko;
                    hold |= (pack.getInt(pack.position()) << ioff) & mask;
                    if(hold > max) items.put(hold - full);
                    else items.put(hold);
                    off -= test;
                }else{
                    hold = (pack.getInt(pack.position()) >> off) & mask;
                    if(hold > max) items.put(hold - full);
                    else items.put(hold);
                    off += size;
                }
            }
            return items.rewind();
        }
        // if(nbits > 0){// zero extended
        if(DEBUG.D) System.err.println("zero extended");
        for(; nitems-- > 0;){
            if(off >= test){
                masko = COMPRESSION.MASKS[ioff = 32 - off];
                hold = (pack.getInt() >> off) & masko;
                hold |= (pack.getInt(pack.position()) << ioff) & mask;
                items.put(hold);
                off -= test;
            }else{
                hold = (pack.getInt(pack.position()) >> off) & mask;
                items.put(hold);
                off += size;
            }
        }
        return items.rewind();
    }

    private final static ByteBuffer mdsXpand(int nitems, final ARRAY<?> pack_dsc, final ARRAY<?> items_dsc) {
        if(DEBUG.D) System.out.println("mdsXpand: " + pack_dsc.getDTypeName() + pack_dsc.arsize());
        final int[] bit = new int[]{0};
        final ByteBuffer bpack = ByteBuffer.allocate(pack_dsc.arsize() + 4).order(Descriptor.BYTEORDER); // zero-fill the last integer
        // bpack.asIntBuffer().put(pack_dsc.getBuffer().asIntBuffer()).rewind(); // would swap ints
        bpack.put(pack_dsc.getBuffer()).rewind(); // take buffer as is
        final ByteBuffer bout = ByteBuffer.allocate(items_dsc.arsize() + items_dsc.pointer()).order(Descriptor.BYTEORDER);
        bout.put((ByteBuffer)items_dsc.serialize().limit(items_dsc.pointer())); // fill header up to body
        bout.put(Descriptor._clsB, Descriptor_A.CLASS); // override class
        final int limit = pack_dsc.arsize() * 8;
        final int dtype = items_dsc.dtype();
        int step, j = 0;
        int ye, xe, yn, xn, xhead;
        final IntBuffer n = IntBuffer.allocate(COMPRESSION.MAXX);
        final IntBuffer e = IntBuffer.allocate(COMPRESSION.MAXX);
        final IntBuffer header = IntBuffer.allocate(2);
        int old, buf = 0, mark = 0;
        if(dtype == DTYPE.T) step = Byte.BYTES;
        else if((items_dsc.length() & (Integer.BYTES - 1)) == 0) step = Integer.BYTES;
        else if((items_dsc.length() & (Short.BYTES - 1)) == 0) step = Short.BYTES;
        else step = Byte.BYTES;
        nitems *= items_dsc.length() / step;
        /********************************
         * Note the sign-extended unpacking.
         ********************************/
        final ByteBuffer fb = ByteBuffer.allocate(Integer.BYTES);
        final boolean isfloat = dtype == DTYPE.F || dtype == DTYPE.FC || dtype == DTYPE.D || dtype == DTYPE.DC || dtype == DTYPE.G || dtype == DTYPE.GC || dtype == DTYPE.H || dtype == DTYPE.HC;
        while(nitems > 0){
            if((bit[0] + 2 * COMPRESSION.FIELDSY) > limit) break;
            COMPRESSION.mdsUnpk(COMPRESSION.FIELDSY, COMPRESSION.FIELDSX, bpack, header, bit);
            xhead = j = COMPRESSION.X_OF_INT(header.get(0)) + 1;
            if(j > nitems) j = nitems;
            xn = j;
            yn = -COMPRESSION.Y_OF_INT(header.get(0));
            xe = COMPRESSION.X_OF_INT(header.get(1));
            ye = -COMPRESSION.Y_OF_INT(header.get(1)) - 1;
            if(bit[0] - ye * xe - yn * j > limit) break;
            nitems -= j;
            COMPRESSION.mdsUnpk((byte)yn, xn, bpack, n, bit);
            if(xe != 0){
                bit[0] -= yn * (xhead - j);
                COMPRESSION.mdsUnpk((byte)ye, xe, bpack, e, bit);
                mark = -1 << (-yn - 1);
            }
            /***********************************
             * Summation. Old=0 here, is new start.
             * Sign and field extend.
             * Note, signed and unsigned are same.
             ***********************************/
            old = 0;
            for(; j-- > 0;){
                buf = n.get();
                if(xe != 0 && e != null && (buf == mark)) old += e.get();
                else old += buf;
                if(step == Integer.BYTES) if(isfloat){
                    fb.putInt(0, old);
                    bout.putShort(fb.getShort(1));
                    bout.putShort(fb.getShort(0));
                }else bout.putInt(old);
                else if(step == Short.BYTES) bout.putShort((short)old);
                else bout.put((byte)old);
            }
        }
        return (ByteBuffer)bout.rewind();
    }

    private static final int X_OF_INT(final int x) {
        return (x >> COMPRESSION.BITSY) & COMPRESSION.MASKX;
    }

    private static final int Y_OF_INT(final int y) {
        return y & COMPRESSION.MASKY;
    }

    private COMPRESSION(final ByteBuffer b){
        super(b);
    }

    private COMPRESSION(final short opcode, final Descriptor<?>... args){
        super(opcode, args);
    }
}

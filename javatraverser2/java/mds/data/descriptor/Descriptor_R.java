package mds.data.descriptor;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DTYPE;
import mds.data.descriptor_r.Action;
import mds.data.descriptor_r.Call;
import mds.data.descriptor_r.Condition;
import mds.data.descriptor_r.Conglom;
import mds.data.descriptor_r.Dependency;
import mds.data.descriptor_r.Dim;
import mds.data.descriptor_r.Dispatch;
import mds.data.descriptor_r.Function;
import mds.data.descriptor_r.Method;
import mds.data.descriptor_r.Opaque;
import mds.data.descriptor_r.Param;
import mds.data.descriptor_r.Procedure;
import mds.data.descriptor_r.Program;
import mds.data.descriptor_r.Range;
import mds.data.descriptor_r.Routine;
import mds.data.descriptor_r.Signal;
import mds.data.descriptor_r.Slope;
import mds.data.descriptor_r.Window;
import mds.data.descriptor_r.With_Error;
import mds.data.descriptor_r.With_Units;
import mds.data.descriptor_s.Missing;

/** Fixed-Length (static) Descriptor (-62 : 194) **/
@SuppressWarnings("deprecation")
public abstract class Descriptor_R<T extends Number>extends Descriptor<T>{
    public static final byte _ndesc    = 8;
    public static final byte _dscoffIa = 12;
    @SuppressWarnings("hiding")
    public static final int  BYTES     = Descriptor.BYTES + 4;
    public static final byte CLASS     = -62;                 // 194

    public static StringBuilder decompile_build(final Descriptor_R<?> dsc, final int prec, final StringBuilder pout, final int mode) {
        final Number val = dsc.getAtomic();
        pout.append("Build_").append(dsc.getClass().getSimpleName()).append("(");
        if(val != null) pout.append(val.toString()).append((mode & Descriptor.DECO_X) > 0 ? "," : ", ");
        dsc.addArguments(0, "", ")", pout, mode);
        return pout;
    }

    public static Descriptor_R<?> deserialize(final ByteBuffer b) throws MdsException {
        switch(b.get(Descriptor._typB)){
            case DTYPE.ACTION:
                return new Action(b);
            case DTYPE.CALL:
                return new Call(b);
            case DTYPE.CONGLOM:
                return new Conglom(b);
            case DTYPE.DIMENSION:
                return new Dim(b);
            case DTYPE.DISPATCH:
                return new Dispatch(b);
            case DTYPE.FUNCTION:
                return Function.deserialize(b);
            case DTYPE.METHOD:
                return new Method(b);
            case DTYPE.OPAQUE:
                return new Opaque(b);
            case DTYPE.PARAM:
                return new Param(b);
            case DTYPE.RANGE:
                return new Range(b);
            case DTYPE.ROUTINE:
                return new Routine(b);
            case DTYPE.SIGNAL:
                return new Signal(b);
            case DTYPE.WINDOW:
                return new Window(b);
            case DTYPE.WITH_ERROR:
                return new With_Error(b);
            case DTYPE.WITH_UNITS:
                return new With_Units(b);
            case DTYPE.CONDITION:
                return new Condition(b);
            case DTYPE.DEPENDENCY:
                return new Dependency(b);
            case DTYPE.PROCEDURE:
                return new Procedure(b);
            case DTYPE.PROGRAM:
                return new Program(b);
            case DTYPE.SLOPE:
                return new Slope(b);
        }
        throw new MdsException(String.format("Unsupported dtype %s for class %s", DTYPE.getName(b.get(Descriptor._typB)), Descriptor.getDClassName(b.get(Descriptor._clsB))), 0);
    }

    private static final Descriptor<?>[] joinArrays(final Descriptor<?>[] args0, final Descriptor<?>[] args1) {
        if(args0 == null || args0.length == 0) return args1;
        if(args1 == null || args1.length == 0) return args0;
        final Descriptor<?>[] args = new Descriptor[args0.length + args1.length];
        System.arraycopy(args0, 0, args, 0, args0.length);
        System.arraycopy(args1, 0, args, args0.length, args1.length);
        return args;
    }

    private static final int joinSize(final Descriptor<?>... args) {
        if(args == null) return 0;
        int size = 0;
        for(final Descriptor<?> arg : args)
            if(arg != null) size += arg.getSize();
        return size;
    }

    public Descriptor_R(final byte dtype, final ByteBuffer data, final Descriptor<?>... args){
        super((short)(data == null ? 0 : data.limit()), dtype, Descriptor_R.CLASS, data, Descriptor_R._dscoffIa + (args == null ? 0 : args.length * Integer.BYTES), Descriptor_R.joinSize(args));
        int getNargs;
        this.b.put(Descriptor_R._ndesc, (byte)(getNargs = args == null ? 0 : args.length));
        if(args != null && args.length > 0){
            int offset = Descriptor_R._dscoffIa + this.length() + args.length * Integer.BYTES;
            for(int i = 0; i < getNargs; i++){
                final int pos = Descriptor_R._dscoffIa + Integer.BYTES * i;
                if(Descriptor.isMissing(args[i])) this.b.putInt(pos, 0);
                else{
                    this.b.putInt(pos, offset);
                    offset += args[i].getSize();
                }
            }
            for(int i = 0; i < getNargs; i++){
                if(this.desc_ptr(i) != 0) ((ByteBuffer)this.b.position(this.desc_ptr(i))).put(args[i].b.duplicate()).position(0);
            }
        }
    }

    protected Descriptor_R(final byte dtype, final ByteBuffer data, final Descriptor<?>[] args1, final Descriptor<?>... args0){
        this(dtype, data, Descriptor_R.joinArrays(args0, args1));
    }

    public Descriptor_R(final ByteBuffer b){
        super(b);
    }

    protected void addArguments(final int first, final String left, final String right, final StringBuilder pout, final int mode) {
        int j;
        final boolean indent = (mode & Descriptor.DECO_X) > 0;
        final String sep = indent ? ",\n\t" : ", ";
        final int last = this.getNArgs() - 1;
        if(left != null){
            pout.append(left);
            if(indent) pout.append("\n\t");
        }
        for(j = first; j <= last; j++){
            this.dscptrs(j).decompile(Descriptor.P_ARG, pout, (mode & ~Descriptor.DECO_X));
            if(j < last) pout.append(sep);
        }
        if(right != null){
            if(indent) pout.append("\n");
            pout.append(right);
        }
    }

    private final int desc_ptr(final int idx) {
        if(this.getNArgs() <= idx || idx < 0) throw new IndexOutOfBoundsException();
        return this.b.getInt(Descriptor_R._dscoffIa + idx * Integer.BYTES);
    }

    private final Descriptor<?> dscptrs(final int idx) {
        final int dscptr = this.desc_ptr(idx);
        if(dscptr == 0) return Missing.NEW;
        int next = this.getSize();
        for(int i = idx + 1; i < this.getNArgs(); i++){
            final int desc_ptr = this.desc_ptr(i);
            if(desc_ptr == 0) continue;
            next = desc_ptr;
            break;
        }
        Descriptor<?> dscptrs;
        try{
            dscptrs = Descriptor.deserialize(((ByteBuffer)this.b.duplicate().position(dscptr).limit(next)).slice().order(this.b.order())).setTree(this.tree).VALUE(this);
            return this.isLocal() ? dscptrs.setLocal() : dscptrs;
        }catch(final MdsException e){
            e.printStackTrace();
            return Missing.NEW;
        }
    }

    @Override
    public final boolean equals(final Object obj) {
        if(!super.equals(obj)) return false;
        final Descriptor_R<?> that = (Descriptor_R<?>)obj;
        if(this.getAtomic() != that.getAtomic()) return false;
        if(this.getNArgs() != that.getNArgs()) return false;
        for(int i = 0; i < this.getNArgs(); i++)
            if(!this.getDescriptor(i).equals(that.getDescriptor(i))) return false;
        return true;
    }

    @Override
    public T getAtomic() {
        return null;
    }

    public final Descriptor<?> getDescriptor(final int idx) {
        return (this.getNArgs() <= idx) ? null : this.dscptrs(idx);
    }

    public final Descriptor<?>[] getDescriptors(final int from, int upto) {
        if(upto < 0) upto = this.getNArgs();
        if(upto - from <= 0) return new Descriptor[0];
        final Descriptor<?>[] list = new Descriptor<?>[upto - from];
        for(int i = 0; i < list.length; i++)
            list[i] = this.getDescriptor(from + i);
        return list;
    }

    public final int getNArgs() {
        return this.ndesc() & 0xFF;
    }

    @Override
    public int[] getShape() {
        return new int[0];
    }

    protected final byte ndesc() {
        return this.b.get(Descriptor_R._ndesc);
    }

    @Override
    public BigInteger[] toBigIntegerArray() {
        try{
            return this.getData().toBigIntegerArray();
        }catch(final MdsException e){
            return null;
        }
    }

    @Override
    public byte[] toByteArray() {
        try{
            return this.getData().toByteArray();
        }catch(final MdsException e){
            return null;
        }
    }

    @Override
    public double[] toDoubleArray() {
        try{
            return this.getData().toDoubleArray();
        }catch(final MdsException e){
            return null;
        }
    }

    @Override
    public float[] toFloatArray() {
        try{
            return this.getData().toFloatArray();
        }catch(final MdsException e){
            return null;
        }
    }

    @Override
    public int[] toIntArray() {
        try{
            return this.getData().toIntArray();
        }catch(final MdsException e){
            return null;
        }
    }

    @Override
    public long[] toLongArray() {
        try{
            return this.getData().toLongArray();
        }catch(final MdsException e){
            return null;
        }
    }

    @Override
    public short[] toShortArray() {
        try{
            return this.getData().toShortArray();
        }catch(final MdsException e){
            return null;
        }
    }

    @Override
    public String[] toStringArray() {
        try{
            return this.getData().toStringArray();
        }catch(final MdsException e){
            return null;
        }
    }
}

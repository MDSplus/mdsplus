package mds.data.descriptor_r.function;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.OPC;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;
import mds.data.descriptor_a.Complex32Array;
import mds.data.descriptor_a.Complex64Array;
import mds.data.descriptor_a.Float32Array;
import mds.data.descriptor_a.Float64Array;
import mds.data.descriptor_a.Int128Array;
import mds.data.descriptor_a.Int16Array;
import mds.data.descriptor_a.Int32Array;
import mds.data.descriptor_a.Int64Array;
import mds.data.descriptor_a.Int8Array;
import mds.data.descriptor_a.Uint128Array;
import mds.data.descriptor_a.Uint16Array;
import mds.data.descriptor_a.Uint32Array;
import mds.data.descriptor_a.Uint64Array;
import mds.data.descriptor_a.Uint8Array;
import mds.data.descriptor_r.Function;
import mds.data.descriptor_s.CString;
import mds.data.descriptor_s.Complex32;
import mds.data.descriptor_s.Complex64;
import mds.data.descriptor_s.Float32;
import mds.data.descriptor_s.Float64;
import mds.data.descriptor_s.Int128;
import mds.data.descriptor_s.Int16;
import mds.data.descriptor_s.Int32;
import mds.data.descriptor_s.Int64;
import mds.data.descriptor_s.Int8;
import mds.data.descriptor_s.Missing;
import mds.data.descriptor_s.Uint128;
import mds.data.descriptor_s.Uint16;
import mds.data.descriptor_s.Uint32;
import mds.data.descriptor_s.Uint64;
import mds.data.descriptor_s.Uint8;
import mds.mdslib.MdsLib;

public class BINARY extends Function{
    public static class Add extends BINARY{
        static public final class double_add implements DOUBLE_METHOD{
            @Override
            public double method(final double x, final double y) {
                return x + y;
            }
        }
        static public final class long_add implements LONG_METHOD{
            @Override
            public long method(final long x, final long y) {
                return x + y;
            }
        }

        public Add(final ByteBuffer b){
            super(b);
        }

        public Add(final Descriptor<?> a, final Descriptor<?> b){
            super(OPC.OpcAdd, a, b);
        }

        @Override
        public final Descriptor<?> method(final DATA<?>[] args) throws MdsException {
            return BINARY.getCommon(args).add(args[0].toDescriptor(), args[1].toDescriptor());
        }
    }
    public static class Concat extends BINARY{
        public Concat(final ByteBuffer b){
            super(b);
        }

        public Concat(final Descriptor<?> a, final Descriptor<?> b){
            super(OPC.OpcConcat, a, b);
        }

        @Override
        public final Descriptor<?> method(final DATA<?>[] args) throws MdsException {
            final StringBuilder str = new StringBuilder(1024);
            for(final DATA<?> dsc : args)
                str.append(dsc.text());
            return new CString(str.toString());
        }
    }
    public static class Divide extends BINARY{
        static public final class double_divide implements DOUBLE_METHOD{
            @Override
            public double method(final double x, final double y) {
                return x / y;
            }
        }
        static public final class long_divide implements LONG_METHOD{
            @Override
            public long method(final long x, final long y) {
                return x / y;
            }
        }

        public Divide(final ByteBuffer b){
            super(b);
        }

        public Divide(final Descriptor<?> a, final Descriptor<?> b){
            super(OPC.OpcDivide, a, b);
        }

        @Override
        public final Descriptor<?> method(final DATA<?>[] args) throws MdsException {
            return BINARY.getCommon(args).divide(args[0].toDescriptor(), args[1].toDescriptor());
        }
    }
    public interface DOUBLE_METHOD{
        public double method(double x, double y);
    }
    public static class Equals extends BINARY{
        public Equals(final ByteBuffer b){
            super(b);
        }

        public Equals(final Descriptor<?> a, final Descriptor<?> b){
            super(OPC.OpcEquals, a, b);
        }

        @Override
        public final Descriptor<?> evaluate() {
            return this.getArgument(1).evaluate();
        }
    }
    public interface LONG_METHOD{
        public long method(long x, long y);
    }
    public static class Multiply extends BINARY{
        static public final class double_multiply implements DOUBLE_METHOD{
            @Override
            public double method(final double x, final double y) {
                return x * y;
            }
        }
        static public final class long_multiply implements LONG_METHOD{
            @Override
            public long method(final long x, final long y) {
                return x * y;
            }
        }

        public Multiply(final ByteBuffer b){
            super(b);
        }

        public Multiply(final Descriptor<?> a, final Descriptor<?> b){
            super(OPC.OpcMultiply, a, b);
        }

        @Override
        public final Descriptor<?> method(final DATA<?>[] args) throws MdsException {
            return BINARY.getCommon(args).multiply(args[0].toDescriptor(), args[1].toDescriptor());
        }
    }
    static public final class Power extends BINARY{
        static public final class double_power implements DOUBLE_METHOD{
            @Override
            public double method(final double x, final double y) {
                return Math.pow(x, y);
            }
        }

        public Power(final ByteBuffer b){
            super(b);
        }

        public Power(final Descriptor<?> a, final Descriptor<?> b){
            super(OPC.OpcPower, a, b);
        }

        @Override
        protected final Descriptor<?> method(final DATA<?>[] args) throws MdsException {
            if(args[1] instanceof Descriptor_A<?>) return Descriptor_A.deserialize(args[0].serialize()).getData().power(args[0].toDescriptor(), args[1].toDescriptor());
            return args[0].power(args[0].toDescriptor(), args[1].toDescriptor());
        }
    }
    static public final class Shift_Left extends BINARY{
        static public final class long_shiftleft implements LONG_METHOD{
            @Override
            public long method(final long x, final long y) {
                return x << y;
            }
        }

        public Shift_Left(final ByteBuffer b){
            super(b);
        }

        public Shift_Left(final Descriptor<?> a, final Descriptor<?> b){
            super(OPC.OpcShiftLeft, a, b);
        }

        @Override
        public final Descriptor<?> method(final DATA<?>[] args) throws MdsException {
            return args[0].shiftleft(args[1].toDescriptor());
        }
    }
    static public final class Shift_Right extends BINARY{
        static public final class long_shiftright implements LONG_METHOD{
            @Override
            public long method(final long x, final long y) {
                return x >> y;
            }
        }

        public Shift_Right(final ByteBuffer b){
            super(b);
        }

        public Shift_Right(final Descriptor<?> a, final Descriptor<?> b){
            super(OPC.OpcShiftRight, a, b);
        }

        @Override
        public final Descriptor<?> method(final DATA<?>[] args) throws MdsException {
            return args[0].shiftright(args[1].toDescriptor());
        }
    }
    public static class Subtract extends BINARY{
        static public final class double_subtract implements DOUBLE_METHOD{
            @Override
            public double method(final double x, final double y) {
                return x - y;
            }
        }
        static public final class long_subtract implements LONG_METHOD{
            @Override
            public long method(final long x, final long y) {
                return x - y;
            }
        }

        public Subtract(final ByteBuffer b){
            super(b);
        }

        public Subtract(final Descriptor<?> a, final Descriptor<?> b){
            super(OPC.OpcSubtract, a, b);
        }

        @Override
        public final Descriptor<?> method(final DATA<?>[] args) throws MdsException {
            return BINARY.getCommon(args).subtract(args[0].toDescriptor(), args[1].toDescriptor());
        }
    }
    private static final op_rec[] binary = new op_rec[]{ //
            new op_rec(", ", OPC.OpcComma, (byte)92, (byte)-1), // 0
            new op_rec(" = ", OPC.OpcEquals, (byte)84, (byte)1), // 1
            new op_rec(null, OPC.OpcEqualsFirst, (byte)84, (byte)1), // 2
            new op_rec(" : ", OPC.OpcDtypeRange, (byte)80, (byte)0), // 3
            new op_rec(" ? ", OPC.OpcConditional, (byte)72, (byte)1), // 4
            new op_rec(" @ ", OPC.OpcPromote, (byte)68, (byte)1), // 5
            new op_rec(" Eqv ", OPC.OpcEqv, (byte)64, (byte)-1), // 6
            new op_rec(" Neqv ", OPC.OpcNeqv, (byte)64, (byte)-1), // 7
            new op_rec(" || ", OPC.OpcOr, (byte)60, (byte)-1), // 8
            new op_rec(" Or_Not ", OPC.OpcOrNot, (byte)60, (byte)-1), // 9
            new op_rec(" Nor ", OPC.OpcNor, (byte)60, (byte)-1), // 10
            new op_rec(" Nor_Not ", OPC.OpcNorNot, (byte)60, (byte)-1), // 11
            new op_rec(" && ", OPC.OpcAnd, (byte)52, (byte)-1), // 12
            new op_rec(" And_Not ", OPC.OpcAndNot, (byte)52, (byte)-1), // 13
            new op_rec(" Nand ", OPC.OpcNand, (byte)52, (byte)-1), // 14
            new op_rec(" Nand_Not ", OPC.OpcNandNot, (byte)52, (byte)-1), // 15
            new op_rec(" == ", OPC.OpcEq, (byte)48, (byte)-1), // 16
            new op_rec(" <> ", OPC.OpcNe, (byte)48, (byte)-1), // 17
            new op_rec(" >= ", OPC.OpcGe, (byte)44, (byte)-1), // 18
            new op_rec(" > ", OPC.OpcGt, (byte)44, (byte)-1), // 19
            new op_rec(" <= ", OPC.OpcLe, (byte)44, (byte)-1), // 20
            new op_rec(" < ", OPC.OpcLt, (byte)44, (byte)-1), // 21
            new op_rec(" Is_In ", OPC.OpcIsIn, (byte)40, (byte)-1), // 22
            new op_rec(" // ", OPC.OpcConcat, (byte)32, (byte)-1), // 23
            new op_rec(" << ", OPC.OpcShiftLeft, (byte)28, (byte)-1), // 24
            new op_rec(" >> ", OPC.OpcShiftRight, (byte)28, (byte)-1), // 25
            new op_rec(" + ", OPC.OpcAdd, (byte)24, (byte)-1), // 26
            new op_rec(" - ", OPC.OpcSubtract, (byte)24, (byte)-1), // 27
            new op_rec(" * ", OPC.OpcMultiply, (byte)20, (byte)-1), // 28
            new op_rec(" / ", OPC.OpcDivide, (byte)20, (byte)-1), // 29
            new op_rec(" ^ ", OPC.OpcPower, (byte)16, (byte)1), // 30 : a ** b == a ^ b
    };

    public static final boolean coversOpCode(final short opcode) {
        switch(opcode){
            default:
                return false;
            case OPC.OpcEqualsFirst:
            case OPC.OpcPower:
            case OPC.OpcDivide:
            case OPC.OpcMultiply:
            case OPC.OpcAdd:
            case OPC.OpcSubtract:
            case OPC.OpcShiftLeft:
            case OPC.OpcShiftRight:
            case OPC.OpcConcat:
            case OPC.OpcIsIn:
            case OPC.OpcGe:
            case OPC.OpcGt:
            case OPC.OpcLe:
            case OPC.OpcLt:
            case OPC.OpcEq:
            case OPC.OpcNe:
            case OPC.OpcAnd:
            case OPC.OpcNand:
            case OPC.OpcOr:
            case OPC.OpcNor:
            case OPC.OpcEqv:
            case OPC.OpcNeqv:
            case OPC.OpcPromote:
            case OPC.OpcEquals:
            case OPC.OpcDtypeRange:
            case OPC.OpcComma:
            case OPC.OpcConditional:
                return true;
        }
    }

    public static BINARY deserialize(final ByteBuffer b) throws MdsException {
        final short opcode = b.getShort(b.getInt(Descriptor._ptrI));
        switch(opcode){
            default:
                return new BINARY(b);
            case OPC.OpcAdd:
                return new Add(b);
            case OPC.OpcDivide:
                return new Divide(b);
            case OPC.OpcConcat:
                return new Concat(b);
            case OPC.OpcEquals:
                return new Equals(b);
            case OPC.OpcMultiply:
                return new Multiply(b);
            case OPC.OpcPower:
                return new Power(b);
            case OPC.OpcShiftLeft:
                return new Shift_Left(b);
            case OPC.OpcShiftRight:
                return new Shift_Right(b);
            case OPC.OpcSubtract:
                return new Subtract(b);
        }
        // throw new MdsException(MdsException.TdiINV_OPC);
    }

    protected final static DATA<?> getCommon(final DATA<?>... args) {
        byte rank = 0;
        for(final DATA<?> arg : args){
            rank = (byte)(rank | arg.getRank());
            if(rank == -1) return Missing.NEW;
        }
        return BINARY.getDscByRank(rank);
    }

    private static DATA<?> getDscByRank(final byte rank) {
        switch(rank & 0xFF){
            case 0xFF:
                return Missing.NEW;
            case 0x00:
                return new Uint8();
            case 0x01:
                return new Uint16();
            case 0x03:
                return new Uint32();
            case 0x07:
                return new Uint64();
            case 0x0F:
                return new Uint128();
            case 0x10:
                return new Int8();
            case 0x11:
                return new Int16();
            case 0x13:
                return new Int32();
            case 0x17:
                return new Int64();
            case 0x1F:
                return new Int128();
            case 0x33:
                return new Float32();
            case 0x37:
                return new Float64();
            case 0x73:
                return new Complex32();
            case 0x77:
                return new Complex64();
            case 0x80:
                return new Uint8Array();
            case 0x81:
                return new Uint16Array();
            case 0x83:
                return new Uint32Array();
            case 0x87:
                return new Uint64Array();
            case 0x8F:
                return new Uint128Array();
            case 0x90:
                return new Int8Array();
            case 0x91:
                return new Int16Array();
            case 0x93:
                return new Int32Array();
            case 0x97:
                return new Int64Array();
            case 0x9F:
                return new Int128Array();
            case 0xB3:
                return new Float32Array();
            case 0xB7:
                return new Float64Array();
            case 0xF3:
                return new Complex32Array();
            case 0xF7:
                return new Complex64Array();
        }
        return Missing.NEW;
    }

    public BINARY(final ByteBuffer b){
        super(b);
    }

    public BINARY(final short opcode, final Descriptor<?>... args){
        super(opcode, args[0], args[1]);
    }

    public BINARY(final short opcode, final Descriptor<?> a, final Descriptor<?> b){
        super(opcode, a, b);
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        final short opcode = this.getOpCode();
        Function r_ptr;
        Descriptor<?> ptr;
        if(opcode == OPC.OpcEqualsFirst) try{
            ptr = this.getDescriptor(0);
            while(ptr != null && ptr.dtype() == DTYPE.DSC)
                ptr = ptr.getDescriptor();
            r_ptr = (Function)ptr;
            if(r_ptr == null) throw new MdsException("OpcEqualsFirst:null");
        }catch(final MdsException e){
            pout.append("/***error<").append(e).append(">***/");
            e.printStackTrace();
            return pout;
        }
        else r_ptr = this;
        final int narg = r_ptr.getNArgs();
        op_rec pop = null;
        for(final op_rec element : BINARY.binary)
            if(element.opcode == opcode){
                pop = element;
                break;
            }
        if(pop == null) return pout.append("/***error<binary opcode(").append(opcode).append(") not found>***/");
        int newone = pop.prec;
        int lorr = pop.lorr;
        if(opcode == OPC.OpcEqualsFirst){
            newone = BINARY.binary[2].prec;
            lorr = BINARY.binary[2].lorr;
        }
        if(prec <= newone) pout.append('(');
        if(opcode == OPC.OpcConditional){
            r_ptr.getDescriptor(2).decompile(newone - lorr, pout, mode & ~Descriptor.DECO_X);
            pout.append(pop.symbol);
            r_ptr.getDescriptor(0).decompile(newone, pout, mode & ~Descriptor.DECO_X);
            pout.append(" : ");
            r_ptr.getDescriptor(1).decompile(newone + lorr, pout, mode & ~Descriptor.DECO_X);
        }else{
            r_ptr.getDescriptor(0).decompile(newone - lorr, pout, mode & ~Descriptor.DECO_X);
            for(int m = 1; m < narg; m++){
                pout.append(pop.symbol);
                if(this != r_ptr) pout.append("= ");
                r_ptr.getDescriptor(m).decompile(newone + lorr, pout, mode & ~Descriptor.DECO_X);
            }
        }
        if(prec <= newone) pout.append(')');
        return pout;
    }

    @Override
    public Descriptor<?> evaluate() {
        try{
            if(MdsLib.lib_loaded == null) return Descriptor.mdslib.getDescriptor(this.tree, "EVALUATE($)", this.getLocal());
            final DATA<?>[] args = Descriptor.getDATAs(this.getArguments());
            return this.method(args);
        }catch(final MdsException e){
            System.err.println(e.getMessage());
            return Missing.NEW;
        }
    }

    @Override
    public Descriptor<?> getLocal_(final FLAG local) {
        final FLAG mylocal = new FLAG();
        final Descriptor<?> X = Descriptor.getLocal(mylocal, this.getArgument(0)), Y = Descriptor.getLocal(mylocal, this.getArgument(1));
        if(FLAG.and(local, mylocal.flag)) return this.setLocal();
        try{
            return this.getClass().getConstructor(Descriptor.class, Descriptor.class).newInstance(X, Y).setLocal();
        }catch(final Exception e){
            return new BINARY(this.getOpCode(), X, Y).setLocal();
        }
    }

    @SuppressWarnings({"static-method", "unused"})
    protected Descriptor<?> method(final DATA<?>[] args) throws MdsException {
        return Missing.NEW;
    }
}

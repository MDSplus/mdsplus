package mds.data.descriptor_r.function;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.OPC;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Function;
import mds.data.descriptor_s.Missing;

public class Fun extends Function{
    public Fun(final ByteBuffer b){
        super(b);
    }

    public Fun(final Descriptor<?> a, final Descriptor<?> b){
        super(OPC.OpcFun, a, b);
    }

    public Fun(final Descriptor<?>[] args){
        super(OPC.OpcFun, args);
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        if(prec < Descriptor.P_STMT) pout.append('(');
        pout.append("Fun ");
        final Descriptor<?> ptr = this.getDescriptor(0);
        if(ptr.dtype() == DTYPE.T) pout.append(ptr.toString());
        else ptr.decompile(Descriptor.P_SUBS, pout, mode & ~Descriptor.DECO_X);
        this.addArguments(2, " (", ") ", pout, mode);
        Function.addCompoundStatement(1, this, 1, pout, mode);
        if(prec < Descriptor.P_STMT) pout.append(')');
        return pout;
    }

    @Override
    public final Descriptor<?> evaluate() {
        try{
            final DATA<?>[] args = Descriptor.getDATAs(this.getArguments());
            return BINARY.getCommon(args).add(args[0].toDescriptor(), args[1].toDescriptor());
        }catch(final MdsException e){
            System.err.println(e.getMessage());
            return Missing.NEW;
        }
    }
}

package mds;

import java.util.ArrayList;
import mds.Mds.Request;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_S;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_s.CString;
import mds.data.descriptor_s.Int32;
import mds.data.descriptor_s.Missing;

public class TdiShr extends TreeShr{
    @SuppressWarnings("rawtypes")
    public static final class TdiCall<T extends Descriptor>extends Shr.LibCall<T>{
        public TdiCall(final Class<T> rtype, final String name){
            super(0, rtype, name);
        }

        @Override
        protected final String getImage() {
            return "TdiShr";
        }
    }

    private final static Request<List> tdiCompile(final String expr, final Descriptor<?>... args) {
        final LibCall<List> call = new TdiCall<List>(List.class, "TdiCompile").descr(new CString(expr));
        for(final Descriptor<?> arg : args)
            call.descr(arg);
        return call.xd("a").val(-1).finL("a", "s");
    }

    private final static Request<List> tdiEvaluate(final Descriptor<?> dsc) {
        return new TdiCall<List>(List.class, "TdiEvaluate").descr(dsc).xd("a").val(-1).finL("a", "s");
    }

    private final static Request<List> tdiExecute(final String expr, final Descriptor<?>... args) {
        final LibCall<List> call = new TdiCall<List>(List.class, "TdiExecute").descr(new CString(expr));
        for(final Descriptor<?> arg : args)
            call.descr(arg);
        return call.xd("a").val(-1).finL("a", "s");
    }

    public TdiShr(final Mds mds){
        super(mds);
    }

    public final int[] getShotDB(final CString expt, final Descriptor_S<?> path, final Int32 lower, final Int32 upper) throws MdsException {
        final ArrayList<Descriptor<?>> args = new ArrayList<Descriptor<?>>(4);
        final StringBuilder expr = new StringBuilder(32).append("getShotDB($");
        args.add(expt);
        if(path != null){
            args.add(path);
            expr.append(",$");
        }else expr.append(",*");
        if(lower != null){
            args.add(lower);
            expr.append(",$");
        }else expr.append(",*");
        if(upper != null){
            args.add(upper);
            expr.append(",$");
        }else expr.append(",*");
        return this.mds.getIntegerArray(expr.append(')').toString(), args.toArray(new Descriptor[args.size()]));
    }

    public final DescriptorStatus tdiCompile(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        if(expr == null || expr.isEmpty()) return new DescriptorStatus(Missing.NEW, MdsException.MDSplusSUCCESS);
        return new DescriptorStatus(this.mds.getDescriptor(ctx, TdiShr.tdiCompile(expr, args)));
    }

    public final String tdiDecompile(final CTX ctx, final Descriptor<?> dsc) throws MdsException {
        return this.tdiDecompile(ctx, "$", dsc);
    }

    public final String tdiDecompile(final CTX ctx, final String exec, final Descriptor<?>... dsc) throws MdsException {
        final DescriptorStatus result = this.tdiExecute(ctx, "Decompile(`(" + exec + ";))", dsc);
        MdsException.handleStatus(result.status);
        return result.data.toString();
    }

    public final DescriptorStatus tdiEvaluate(final CTX ctx, final Descriptor<?> dsc) throws MdsException {
        if(Descriptor.isMissing(dsc)) return new DescriptorStatus(Missing.NEW, MdsException.MDSplusSUCCESS);
        return new DescriptorStatus(this.mds.getDescriptor(ctx, TdiShr.tdiEvaluate(dsc)));
    }

    public final DescriptorStatus tdiExecute(final CTX ctx, final String expr, final Descriptor<?>... args) throws MdsException {
        if(expr == null || expr.isEmpty()) return new DescriptorStatus(Missing.NEW, MdsException.MDSplusSUCCESS);
        return new DescriptorStatus(this.mds.getDescriptor(ctx, TdiShr.tdiExecute(expr, args)));
    }
}

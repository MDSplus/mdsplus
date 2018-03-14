package mds;

import java.util.ArrayList;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_S;
import mds.data.descriptor_s.CString;
import mds.data.descriptor_s.Int32;
import mds.data.descriptor_s.Missing;

public final class TdiShr extends Shr{
    @SuppressWarnings("rawtypes")
    public static final class TdiCall<T extends Descriptor>extends Shr.LibCall<T>{
        public TdiCall(final Class<T> rtype, final String name, final int cap_in){
            super(0, rtype, name, cap_in);
        }

        public TdiCall(final Class<T> rtype, final String prefix, final int preval, final String name, final int cap_in){
            super(0, rtype, prefix, preval, name, cap_in);
        }

        public TdiCall(final Class<T> rtype, final String prefix, final long preval, final String name, final int cap_in){
            super(0, rtype, prefix, preval, name, cap_in);
        }

        public TdiCall(final Class<T> rtype, final String prefix, final String name, final int cap_in){
            super(0, rtype, prefix, name, cap_in);
        }

        @Override
        protected final String getImage() {
            return "TdiShr";
        }
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

    public final Descriptor<?> tdiCompile(final String expr) throws MdsException {
        if(expr == null || expr.isEmpty()) return Missing.NEW;
        return this.mds.getDescriptor("Compile($)", new CString(expr));
    }

    public final String tdiDecompile(final Descriptor<?> dsc) throws MdsException {
        return this.mds.getString("Decompile($)", dsc);
    }

    public final Descriptor<?> tdiEvaluate(final Descriptor<?> data) throws MdsException {
        if(Descriptor.isMissing(data)) return Missing.NEW;
        return this.mds.getDescriptor("Evaluate($)", data);
    }

    public final Descriptor<?> tdiExecute(final String expr, final Descriptor<?>... args) throws MdsException {
        if(expr == null || expr.isEmpty()) return Missing.NEW;
        return this.mds.getDescriptor(new StringBuilder(expr.length() + 13).append("Evaluate((").append(expr).append(";))").toString(), args);
    }
}

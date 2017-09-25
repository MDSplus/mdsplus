/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
        if(data == null || data == Missing.NEW) return Missing.NEW;
        return this.mds.getDescriptor("Evaluate($)", data);
    }

    public final Descriptor<?> tdiExecute(final String expr, final Descriptor<?>... args) throws MdsException {
        if(expr == null || expr.isEmpty()) return Missing.NEW;
        return this.mds.getDescriptor(new StringBuilder(expr.length() + 13).append("Evaluate((").append(expr).append(";))").toString(), args);
    }
}

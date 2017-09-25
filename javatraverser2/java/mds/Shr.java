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
import mds.Mds.Request;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_s.CString;
import mds.data.descriptor_s.Missing;
import mds.data.descriptor_s.Pointer;

public abstract class Shr{
    @SuppressWarnings("rawtypes")
    public static abstract class LibCall<T extends Descriptor>{
        protected static final String          finT  = "_t=_f;MdsShr->StrFree1Dx(ref(_f));_t";
        private final StringBuilder            sb;
        private final ArrayList<Descriptor<?>> args  = new ArrayList<Descriptor<?>>();
        private final int                      cap;
        private final Class<T>                 rtype;
        private final int                      props;
        private int                            max   = 0;
        private int                            nargs = 0;

        private LibCall(final int props, final Class<T> rtype, final int cap_in){
            this.sb = new StringBuilder(cap_in);
            this.rtype = rtype;
            this.cap = cap_in;
            this.props = props;
        }

        public LibCall(final int props, final Class<T> ret_type, final String name, final int cap_in){
            this(props, ret_type, cap_in);
            if(ret_type == CString.class){
                this.sb.append("_f=");
                this.max += 3;
            }
            this.addEntryPoint(name);
        }

        public LibCall(final int props, final Class<T> ret_type, final String prefix, final int preval, final String name, final int cap_in){
            this(props, ret_type, cap_in);
            this.sb.append(prefix).append(preval).append(";_s=");
            this.max += 4 + 11 + prefix.length();
            this.addEntryPoint(name);
        }

        public LibCall(final int props, final Class<T> ret_type, final String prefix, final long preval, final String name, final int cap_in){
            this(props, ret_type, cap_in);
            this.sb.append(prefix).append(preval).append("Q;_s=");
            this.max += 5 + 20 + prefix.length();
            this.addEntryPoint(name);
        }

        public LibCall(final int props, final Class<T> ret_type, final String prefix, final String name, final int cap_in){
            this(props, ret_type, cap_in);
            this.sb.append(prefix).append(";_s=");
            this.max += 4 + prefix.length();
            this.addEntryPoint(name);
        }

        private final void addEntryPoint(final String name) {
            this.sb.append(this.getImage()).append("->").append(name).append('(');
            this.max += 3 + this.getImage().length() + name.length();
        }

        public final LibCall<T> arg(final Object... arg) {
            this.sep();
            this.max -= this.sb.length();
            for(final Object o : arg)
                this.sb.append(o);
            this.max += this.sb.length();
            this.nargs++;
            return this;
        }

        public final LibCall<T> descr(final Descriptor<?> d) {
            if(d == null || d == Missing.NEW) return this.miss(8);
            this.args.add(d);
            return this.arg("descr($)");
        }

        public final LibCall<T> descr(final int val) {
            this.sep().append("descr(").append(val).append(')');
            this.max += 7 + 11;
            this.nargs++;
            return this;
        }

        public final LibCall<T> descr(final long val) {
            this.sep().append("descr(").append(val).append("Q)");
            this.max += 8 + 20;
            this.nargs++;
            return this;
        }

        public final LibCall<T> descr(final String... ex) {
            return this.obj("descr", ex);
        }

        public String expr() {
            this.sb.append(')');
            this.max += 1;
            if(this.max != this.cap) System.err.println(this.sb + " " + this.max + " of " + this.cap);
            return this.sb.toString();
        }

        public String expr(final String... suffix) {
            int len = 0;
            for(final String str : suffix)
                len += str.length();
            this.sb.append(");");
            for(final String str : suffix)
                this.sb.append(str);
            this.max += 2 + len;
            // TODO:DEBUG
            // if(this.max != this.cap) System.err.println(this.sb + " " + this.max + " of " + this.cap);
            return this.sb.toString();
        }

        public final Request<T> fin() {
            return new Request<T>(this.rtype, this.props, this.expr(), this.args.toArray(new Descriptor<?>[0]));
        }

        public final Request<T> fin(final String... suffix) {
            return new Request<T>(this.rtype, this.props, this.expr(suffix), this.args.toArray(new Descriptor<?>[0]));
        }

        protected abstract String getImage();

        public final LibCall<T> miss(final int amax) {
            this.sep().append('*');
            this.max += amax;
            this.nargs++;
            return this;
        }

        private final LibCall<T> obj(final String mod, final String... ex) {
            this.max -= this.sep().length();
            this.sb.append(mod).append('(');
            if(ex.length == 0) this.sb.append('$');
            else for(final String e : ex)
                this.sb.append(e);
            this.max += this.sb.append(')').length();
            this.nargs++;
            return this;
        }

        public final LibCall<T> ref(final Descriptor<?> d) {
            if(d == null || d == Missing.NEW) return this.miss(6);
            this.args.add(d);
            return this.arg("ref($)");
        }

        public final LibCall<T> ref(final long val) {
            this.sep().append("ref(").append(val).append("Q)");
            this.max += 6 + 20;
            this.nargs++;
            return this;
        }

        public final LibCall<T> ref(final String... ex) {
            return this.obj("ref", ex);
        }

        private final StringBuilder sep() {
            if(this.nargs == 0) return this.sb;
            this.max++;
            return this.sb.append(',');
        }

        public final LibCall<T> val(final Descriptor<?> d) {
            if(d == null || d == Missing.NEW) return this.miss(6);
            this.args.add(d);
            return this.arg("val($)");
        }

        public final LibCall<T> val(final int val) {
            this.sep().append("val(").append(val).append(')');
            this.max += 5 + 11;
            this.nargs++;
            return this;
        }

        public final LibCall<T> xd(final Descriptor<?> d) {
            if(d == null || d == Missing.NEW) return this.miss(12);
            this.args.add(d);
            return this.arg("xd(AS_IS($))");
        }

        public final LibCall<T> xd(final String... ex) {
            return this.obj("xd", ex);
        }
    }
    public static interface Null{/*type for NA input*/}
    public static final class RequestBuilder{
        private final StringBuilder            cmds   = new StringBuilder(1024);
        private final ArrayList<Descriptor<?>> argsin = new ArrayList<Descriptor<?>>();

        public final void add(final Request<?> req) {
            this.add(req.expr, req.args);
        }

        public final void add(final String cmd, final Descriptor<?>... argin) {
            if(this.cmds.length() == 0) this.cmds.append(List.list);
            else this.cmds.append(',');
            this.cmds.append("(").append(cmd).append(";)");
            for(final Descriptor<?> arg : argin)
                this.argsin.add(arg);
        }

        public final List execute(final Mds mds) throws MdsException {
            return this.execute(mds, null);
        }

        public final List execute(final Mds mds, final Pointer ctx) throws MdsException {
            return mds.getDescriptor(ctx, this.request());
        }

        public final Request<List> request() {
            return new Request<List>(List.class, this.cmds.append(")").toString(), this.argsin.toArray(new Descriptor[0]));
        }
    }
    protected final Mds mds;

    public Shr(final Mds mds){
        this.mds = mds;
    }
}

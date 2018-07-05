package mds;

import mds.Mds.Request;
import mds.data.descriptor.ARRAY;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;
import mds.data.descriptor.Descriptor_CA;
import mds.data.descriptor.Descriptor_R;
import mds.data.descriptor_a.EmptyArray;
import mds.data.descriptor_a.Int8Array;
import mds.data.descriptor_s.CString;
import mds.data.descriptor_s.Int32;

public final class MdsShr extends Shr{
    @SuppressWarnings("rawtypes")
    public static final class MdsCall<T extends Descriptor>extends Shr.LibCall<T>{
        public MdsCall(final Class<T> rtype, final String name, final int cap_in){
            super(0, rtype, name, cap_in);
        }

        public MdsCall(final Class<T> rtype, final String prefix, final int preval, final String name, final int cap_in){
            super(0, rtype, prefix, preval, name, cap_in);
        }

        public MdsCall(final Class<T> rtype, final String prefix, final long preval, final String name, final int cap_in){
            super(0, rtype, prefix, preval, name, cap_in);
        }

        public MdsCall(final Class<T> rtype, final String prefix, final String name, final int cap_in){
            super(0, rtype, prefix, name, cap_in);
        }

        @Override
        protected final String getImage() {
            return "MdsShr";
        }
    }

    @SuppressWarnings("rawtypes")
    public final static Request<ARRAY> mdsCompress(final String image, final String entry, final Descriptor_A<?> input) {
        return new MdsCall<ARRAY>(ARRAY.class, "_a=*", "MdsCompress", 64).ref(CString.make(image)).ref(CString.make(entry)).xd(input).xd("_a").fin("_a");
    }

    @SuppressWarnings("rawtypes")
    public final static Request<Descriptor_A> mdsDecompress(final Descriptor_R<?> input) {
        return new MdsCall<Descriptor_A>(Descriptor_A.class, "_a=*", "MdsDecompress", 67).xd(input).xd("_a").fin("_a");
    }

    public final static Request<Int32> mdsEvent(final String event) {
        return new MdsCall<Int32>(Int32.class, "MDSEvent", 58).ref(CString.make(event)).val(0).val(0).fin();
    }

    public final static Request<CString> mdsGetMsg(final int status) {
        return new MdsCall<CString>(CString.class, "MdsGetMsg:T", 77).val(status).fin(LibCall.finT);
    }

    public final static Request<CString> mdsGetMsgDsc(final int status) {
        return new MdsCall<CString>(CString.class, "_a=repeat(' ',256)", "MdsGetMsgDsc", 78).val(status).descr("_a").fin("TRIM(_a)");
    }

    public final static Request<Int32> mdsPutEnv(final String expr) {
        return new MdsCall<Int32>(Int32.class, "MdsPutEnv", 25).ref(new CString(expr)).fin();
    }

    @SuppressWarnings("rawtypes")
    public final static Request<Descriptor> mdsSerializeDscIn(final Int8Array serial) {
        return new MdsCall<Descriptor>(Descriptor.class, "_a=*", "MdsSerializeDscIn", 50).ref(serial).xd("_a").fin("_a");
    }

    @SuppressWarnings({"rawtypes", "unchecked"})
    public final static Request<Int8Array> mdsSerializeDscOut(final String expr, final Descriptor<?>... args) {
        return new Request<Int8Array>(Int8Array.class, new MdsCall(null, "_a=*", "MdsSerializeDscOut", 52 + expr.length()).xd("(", expr, ";)").xd("_a").expr("_a"), args);
    }

    public MdsShr(final Mds mds){
        super(mds);
    }

    public final ARRAY<?> mdsCompress(final Null NULL, final Descriptor_A<?> input) throws MdsException {
        return this.mdsCompress(null, null, null, input);
    }

    public final ARRAY<?> mdsCompress(final Null NULL, final String entry, final Descriptor_A<?> input) throws MdsException {
        return this.mdsCompress(null, null, entry, input);
    }

    public final ARRAY<?> mdsCompress(final Null NULL, final String image, final String entry, final Descriptor_A<?> input) throws MdsException {
        final ARRAY<?> result = this.mds.getDescriptor(null, MdsShr.mdsCompress(image, entry, input));
        return (result == null || result == EmptyArray.NEW) ? input : result;
    }

    public final Descriptor<?> mdsDecompress(final Null NULL, final Descriptor_CA input) throws MdsException {
        return this.mdsDecompress(null, input.payload());
    }

    public final Descriptor<?> mdsDecompress(final Null NULL, final Descriptor_R<?> input) throws MdsException {
        return this.mds.getDescriptor(null, MdsShr.mdsDecompress(input));
    }

    public final int mdsEvent(final Null NULL, final String event) throws MdsException {
        return this.mds.getDescriptor(null, MdsShr.mdsEvent(event)).getValue();
    }

    public final String mdsGetMsg(final Null NULL, final int status) throws MdsException {
        return this.mds.getDescriptor(null, MdsShr.mdsGetMsg(status)).toString();
    }

    public final String mdsGetMsgDsc(final Null NULL, final int status) throws MdsException {
        return this.mds.getDescriptor(null, MdsShr.mdsGetMsgDsc(status)).toString();
    }

    public final int mdsPutEnv(final Null NULL, final String expr) throws MdsException {
        return this.mds.getDescriptor(null, MdsShr.mdsPutEnv(expr)).getValue();
    }

    public final Descriptor<?> mdsSerializeDscIn(final Null NULL, final Int8Array serial) throws MdsException {
        return this.mds.getDescriptor(null, MdsShr.mdsSerializeDscIn(serial));
    }

    public final Int8Array mdsSerializeDscOut(final Null NULL, final String expr, final Descriptor<?>... args) throws MdsException {
        return this.mds.getDescriptor(null, MdsShr.mdsSerializeDscOut(expr, args));
    }

    public final int setenv(final String expr) throws MdsException {
        return this.mds.getDescriptor(null, MdsShr.mdsPutEnv(expr)).getValue();
    }

    public final int setenv(final String name, final String value) throws MdsException {
        return this.mds.getDescriptor(null, MdsShr.mdsPutEnv(name + '=' + value)).getValue();
    }
}

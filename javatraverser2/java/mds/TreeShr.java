package mds;

import mds.Mds.Request;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;
import mds.data.descriptor_a.Int32Array;
import mds.data.descriptor_a.Int64Array;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_r.Signal;
import mds.data.descriptor_s.CString;
import mds.data.descriptor_s.Int32;
import mds.data.descriptor_s.NUMBER;
import mds.data.descriptor_s.Pointer;

public final class TreeShr extends Shr{
    public static class DescriptorStatus{
        public final Descriptor<?> data;
        public final int           status;

        public DescriptorStatus(final Descriptor<?> data, final int status){
            this.data = data;
            this.status = status;
        }

        public DescriptorStatus(final List list){
            this(list.get(0), list.toInt(1));
        }

        @Override
        public final String toString() {
            return new StringBuilder().append(this.data).append(";").append(this.status).toString();
        }
    }
    public static class IntegerStatus{
        public final int data;
        public final int status;

        public IntegerStatus(final int data, final int status){
            this.data = data;
            this.status = status;
        }

        public IntegerStatus(final int[] datastatus){
            this(datastatus[0], datastatus[1]);
        }

        public IntegerStatus(final Int32Array descriptor){
            this(descriptor.toArray());
        }

        @Override
        public final String toString() {
            final StringBuilder str = new StringBuilder(32).append(this.data).append(';');
            return str.append(this.status).toString();
        }
    }
    public static class NodeRefStatus{
        public static final NodeRefStatus init = new NodeRefStatus(-1, 0, 0);
        public final int                  data;
        public final long                 ref;
        public final int                  status;

        public NodeRefStatus(final int data, final long ref, final int status){
            this.data = data;
            this.ref = ref;
            this.status = status;
        }

        public NodeRefStatus(final List list){
            this(list.toInt(0), list.toLong(1), list.toInt(2));
        }

        public final boolean ok() {
            return (this.status & 1) == 1;
        }

        @Override
        public final String toString() {
            final StringBuilder str = new StringBuilder(64).append(this.data).append(';');
            str.append(this.ref).append("l;");
            return str.append(this.status).toString();
        }
    }
    public static class SegmentInfo{
        public byte      dtype;
        public byte      dimct;
        public int[]     dims;
        public int       next_row;
        public final int status;

        public SegmentInfo(final byte dtype, final byte dimct, final int[] dims, final int next_row){
            this(dtype, dimct, dims, next_row, -1);
        }

        public SegmentInfo(final byte dtype, final byte dimct, final int[] dims, final int next_row, final int status){
            this.dtype = dtype;
            this.dimct = dimct;
            this.dims = dims;
            this.next_row = next_row;
            this.status = status;
        }

        public SegmentInfo(final int[] array){
            this.dtype = (byte)array[0];
            this.dimct = (byte)array[1];
            this.dims = new int[8];
            System.arraycopy(array, 2, this.dims, 0, 8);
            this.next_row = array[10];
            this.status = array.length > 11 ? array[11] : -1;
        }

        public SegmentInfo(final Int32Array descriptor){
            this(descriptor.toArray());
        }

        public SegmentInfo(final List list){
            this(list.toByte(0), list.toByte(1), list.toIntArray(2), list.toInt(3), list.toInt(4));
        }

        @Override
        public final String toString() {
            final StringBuilder str = new StringBuilder(128).append(this.dtype).append(';');
            str.append(this.dimct).append(';');
            for(final int dim : this.dims)
                str.append(dim).append(',');
            str.append(this.next_row).append(';');
            return str.append(this.status).toString();
        }
    }
    public static class SignalStatus{
        public final Signal data;
        public final int    status;

        public SignalStatus(final List list){
            this((Signal)list.get(0), list.toInt(1));
        }

        public SignalStatus(final Signal data, final int status){
            this.data = data;
            this.status = status;
        }

        @Override
        public final String toString() {
            return new StringBuilder().append(this.data).append(";").append(this.status).toString();
        }
    }
    public static class StringStatus{
        public final String data;
        public final int    status;

        public StringStatus(final List list){
            this.data = list.get(0).toString();
            this.status = list.get(1).toInt();
        }

        public StringStatus(final String data, final int status){
            this.data = data;
            this.status = status;
        }

        @Override
        public final String toString() {
            return new StringBuilder(32).append(this.data).append(";").append(this.status).toString();
        }
    }
    public static class TagRef{
        public static final TagRef init = new TagRef(null, 0);
        public final String        data;
        public final long          ref;

        public TagRef(final List list){
            this(list.get(0).toString(), list.get(1).toLong());
        }

        public TagRef(final String data, final long ref){
            this.data = data;
            this.ref = ref;
        }

        public final boolean ok() {
            return this.ref != 0;
        }

        @Override
        public final String toString() {
            return new StringBuilder(32).append(this.data).append(";").append(this.ref).append('l').toString();
        }
    }
    public static class TagRefStatus{
        public static final TagRefStatus init = new TagRefStatus(null, -1, 0, 0);
        public final String              data;
        public final int                 nid;
        public final long                ref;
        public final int                 status;

        public TagRefStatus(final List list){
            this(list.toString(0), list.toInt(1), list.toLong(2), list.toInt(3));
        }

        public TagRefStatus(final String data, final int nid, final long ref, final int status){
            this.data = data;
            this.nid = nid;
            this.ref = ref;
            this.status = status;
        }

        public final boolean ok() {
            return (this.status & 1) == 1;
        }

        @Override
        public final String toString() {
            final StringBuilder str = new StringBuilder(128).append(this.data).append(';');
            str.append(this.nid).append(';');
            str.append(this.ref).append("l;");
            return str.append(this.status).toString();
        }
    }
    @SuppressWarnings("rawtypes")
    public static final class TreeCall<T extends Descriptor>extends Shr.LibCall<T>{
        public TreeCall(final Class<T> rtype, final String name, final int cap_in){
            super(0, rtype, name, cap_in);
        }

        public TreeCall(final Class<T> rtype, final String prefix, final int preval, final String name, final int cap_in){
            super(0, rtype, prefix, preval, name, cap_in);
        }

        public TreeCall(final Class<T> rtype, final String prefix, final long preval, final String name, final int cap_in){
            super(0, rtype, prefix, preval, name, cap_in);
        }

        public TreeCall(final Class<T> rtype, final String prefix, final String name, final int cap_in){
            super(0, rtype, prefix, name, cap_in);
        }

        public TreeCall(final int props, final Class<T> rtype, final String prefix, final String name, final int cap_in){
            super(props, rtype, prefix, name, cap_in);
        }

        @Override
        protected final String getImage() {
            return "TreeShr";
        }
    }
    public static final int NO_STATUS = -1;

    private static Request<CString> treeAbsPath(final String relpath) {
        return new TreeCall<CString>(CString.class, "TreeAbsPath:T", 77).ref(CString.make(relpath)).fin(LibCall.finT);
    }

    public static final Request<Int32Array> treeAddConglom(final String path, final String model) {
        return new TreeCall<Int32Array>(Int32Array.class, "_a=-1", "TreeAddConglom", 62).ref(CString.make(path)).ref(CString.make(model)).ref("_a").fin("[_a,_s]");
    }

    public static final Request<Int32Array> treeAddNode(final String path, final byte usage) {
        return new TreeCall<Int32Array>(Int32Array.class, "_a=-1", "TreeAddNode", 69).ref(CString.make(path)).ref("_a").val(usage).fin("[_a,_s]");
    }

    public static final Request<Int32> treeAddTag(final int nid, final String tag) {
        return new TreeCall<Int32>(Int32.class, "TreeAddTag", 44).val(nid).ref(CString.make(tag)).fin();
    }

    public static final Request<Int32> treeBeginTimestampedSegment(final int nid, final Descriptor_A<?> initialValue, final int idx) {
        return new TreeCall<Int32>(Int32.class, "TreeBeginTimestampedSegment", 84).val(nid).xd(initialValue).val(idx).fin();
    }

    public static final Request<Int32> treeCleanDatafile(final String expt, final int shot) {
        return new TreeCall<Int32>(Int32.class, "TreeCleanDatafile", 51).ref(CString.make(expt)).val(shot).fin();
    }

    public static final Request<Int32> treeClose(final String expt, final int shot) {
        return new TreeCall<Int32>(Int32.class, "TreeClose", 43).ref(CString.make(expt)).val(shot).fin();
    }

    public static final Request<Int32> treeCompressDatafile(final String expt, final int shot) {
        return new TreeCall<Int32>(Int32.class, "TreeCompressDatafile", 54).ref(CString.make(expt)).val(shot).fin();
    }

    public static final Request<Int32> treeCreateTreeFiles(final String expt, final int newshot, final int fromshot) {
        return new TreeCall<Int32>(Int32.class, "TreeCreateTreeFiles", 70).ref(CString.make(expt)).val(newshot).val(fromshot).fin();
    }

    public static final Request<Pointer> treeCtx() {
        return new TreeCall<Pointer>(Pointer.class, "TreeCtx:P", 20).fin();
    }

    private static final Request<Pointer> treeDbid() {
        return new TreeCall<Pointer>(Pointer.class, "TreeDbid:P", 21).fin();
    }

    public static final Request<Int32> treeDeleteNodeExecute() {
        return new TreeCall<Int32>(Int32.class, "TreeDeleteNodeExecute", 32).fin();
    }

    public static final Request<Int32Array> treeDeleteNodeGetNid(final int idx) {
        return new TreeCall<Int32Array>(Int32Array.class, "_a=", idx, "TreeDeleteNodeGetNid", 64).ref("_a").fin("[_a,_s]");
    }

    public static final Request<Int32Array> treeDeleteNodeInitialize(final int nid) {
        return TreeShr.treeDeleteNodeInitialize(nid, 0, true);
    }

    public static final Request<Int32Array> treeDeleteNodeInitialize(final int nid, final int count, final boolean init) {
        return new TreeCall<Int32Array>(Int32Array.class, "_a=", count, "TreeDeleteNodeInitialize", 101).val(nid).ref("_a").val(init ? 1 : 0).fin("[_a,_s]");
    }

    public static final Request<List> treeDoMethod(final int nid, final String method, final Descriptor<?>... args) {
        final Descriptor<?>[] parms = new CString[args.length + 1];
        parms[0] = CString.make(method);
        final LibCall<List> expr = new TreeCall<List>(List.class, "TreeDoMethod", "_x=*", 1024).val(nid).ref(method);
        for(final Descriptor<?> arg : args)
            expr.ref(arg);
        return expr.val(-1).xd("_x").fin("List(*,_x,_s)");
    }

    public static final Request<Int32> treeEndConglomerate() {
        return new TreeCall<Int32>(Int32.class, "TreeEndConglomerate", 30).fin();
    }

    private static Request<List> treeFileName(final String expt, final int shot) {
        return new TreeCall<List>(List.class, "_a=*", "TreeFileName", 70).ref(CString.make(expt)).val(shot).xd("_a").fin("List(*,_a,_s)");
    }

    public static final Request<List> treeFindNodeTags_T(final int nid, final TagRef ref) {
        return new TreeCall<List>(List.class, "_a=", ref.ref, "TreeFindNodeTags:T", 107).val(nid).ref("_a").fin("_a=List(*,_s,_a);MdsShr->StrFree1Dx(ref(_s));_a");
    }

    public static final Request<List> treeFindNodeTagsDsc(final int nid, final TagRef ref) {
        return new TreeCall<List>(List.class, "_t=REPEAT(' ',255),_a=", ref.ref, "TreeFindNodeTagsDsc", 132).val(nid).ref("_a").descr("_t").fin("List(*,TRIM(_t),_a,_s)");
    }

    public static final Request<List> treeFindNodeWild(final String searchstr, final int usage_mask, final NodeRefStatus ref) {
        return new TreeCall<List>(List.class, "_a=-1;_q=", ref.ref, "TreeFindNodeWild", 115).ref(CString.make(searchstr)).ref("_a").ref("_q").val(usage_mask).fin("List(*,_a,_q,_s)");
    }

    public static final Request<List> treeFindTagWild(final String searchstr, final TagRefStatus ref) {
        return new TreeCall<List>(List.class, "_a=*;_i=-1;_q=", ref.ref, "TreeFindTagWildDsc", 114).ref(CString.make(searchstr)).ref("_i").ref("_q").xd("_a").fin("List(*,_a,_i,_q,_s)");
    }

    public static final Request<Int32> treeGetCurrentShotId(final String expt) {
        return new TreeCall<Int32>(Int32.class, "TreeGetCurrentShotId", 37).ref(CString.make(expt)).fin();
    }

    public static final Request<Int32> treeGetDatafileSize() {
        return new TreeCall<Int32>(Int32.class, "TreeGetDatafileSize", 30).fin();
    }

    public static final Request<Int32Array> treeGetDefaultNid() {
        return new TreeCall<Int32Array>(Int32Array.class, "_a=-1", "TreeGetDefaultNid", 52).ref("_a").fin("[_a,_s]");
    }

    public static final Request<CString> treeGetMinimumPath(final int nid) {
        return new TreeCall<CString>(CString.class, "TreeGetMinimumPath:T", 104).val(0).val(nid).fin(LibCall.finT);
    }

    public static final Request<Int32Array> treeGetNumSegments(final int nid) {
        return new TreeCall<Int32Array>(Int32Array.class, "_a=0", "TreeGetNumSegments", 68).val(nid).ref("_a").fin("[_a,_s]");
    }

    public static final Request<CString> treeGetPath(final int nid) {
        return new TreeCall<CString>(CString.class, "TreeGetPath:T", 80).val(nid).fin(LibCall.finT);
    }

    @SuppressWarnings("rawtypes")
    public static final Request<Descriptor> treeGetRecord(final int nid) {
        return new TreeCall<Descriptor>(Request.PROP_DO_NOT_LIST, Descriptor.class, "_a=*", "TreeGetRecord", 57).val(nid).xd("_a").fin("_a");
    }

    public static final Request<List> treeGetSegment(final int nid, final int idx) {
        return new TreeCall<List>(List.class, "_a=_t=*", "TreeGetSegment", 102).val(nid).val(idx).xd("_a").xd("_t").fin("List(*,_a,_t)");
    }

    public static final Request<List> treeGetSegmentInfo(final int nid, final int idx) {
        return new TreeCall<List>(List.class, "_a=0B;_b=0B;_d=ZERO(8,0);_i=0", "TreeGetSegmentInfo", 146).val(nid).val(idx).ref("_a").ref("_b").ref("_d").ref("_i").fin("List(*,_a,_b,_d,_i,_s)");
    }

    public final static Request<List> treeGetSegmentLimits(final int nid, final int idx) {
        return new TreeCall<List>(List.class, "_a=_b=*", "TreeGetSegmentLimits", 106).val(nid).val(idx).xd("_a").xd("_b").fin("List(*,List(*,_a,_b),_s)");
    }

    @SuppressWarnings("rawtypes")
    public static final Request<Descriptor> treeGetSegmentScale(final int nid) {
        return new TreeCall<Descriptor>(Descriptor.class, "_a=*", "TreeGetSegmentScale", 102).val(nid).xd("_a").fin("_a");
    }

    public static final Request<List> treeGetSegmentTimesXd(final int nid) {
        return new TreeCall<List>(List.class, "_a=0;_b=*;_c=*", "TreeGetSegmentTimesXd", 113).val(nid).ref("_a").xd("_b").xd("_c").fin("List(*,List(*,_a,_b,_c),_s)");
    }

    public final static Request<List> treeGetXNci(final int nid, final String name) {
        return new TreeCall<List>(List.class, "_a=*", "TreeGetXNci", 73).val(nid).ref(CString.make(name)).xd("_a").fin("List(*,_a,_s)");
    }

    public final static Request<Int32> treeIsOn(final int nid) {
        return new TreeCall<Int32>(Int32.class, "TreeIsOn", 35).val(nid).fin();
    }

    public static final Request<Int32> treeIsOpen(final Pointer ctx) {
        return new TreeCall<Int32>(Int32.class, "_TreeIsOpen", 37).val(ctx).fin();
    }

    public final static Request<Int32> treeMakeSegment(final int nid, final Descriptor<?> start, final Descriptor<?> end, final Descriptor<?> dimension, final Descriptor_A<?> values, final int idx, final int rows_filled) {
        return new TreeCall<Int32>(Int32.class, "TreeMakeSegment", 128).val(nid).xd(start).xd(end).xd(dimension).xd(values).val(idx).val(rows_filled).fin();
    }

    public final static Request<Int32> treeMakeTimestampedSegment(final int nid, final Int64Array timestamps, final Descriptor_A<?> values, final int idx, final int rows_filled) {
        return new TreeCall<Int32>(Int32.class, "TreeMakeTimestampedSegment", 107).val(nid).ref(timestamps).xd(values).val(idx).val(rows_filled).fin();
    }

    public static final Request<Int32> treeMakeTimestampedSegment(final int nid, final long[] dim, final Descriptor_A<?> values, final int idx, final int rows_filled) {
        return TreeShr.treeMakeTimestampedSegment(nid, new Int64Array(dim), values, idx, rows_filled);
    }

    public final static Request<Int32> treeOpen(final String expt, final int shot, final boolean readonly) {
        return new TreeCall<Int32>(Int32.class, "TreeOpen", 59).ref(CString.make(expt)).val(shot).val(readonly ? 1 : 0).fin();
    }

    public final static Request<Int32> treeOpenEdit(final String expt, final int shot) {
        return new TreeCall<Int32>(Int32.class, "TreeOpenEdit", 46).ref(CString.make(expt)).val(shot).fin();
    }

    public final static Request<Int32> treeOpenNew(final String expt, final int shot) {
        return new TreeCall<Int32>(Int32.class, "TreeOpenNew", 45).ref(CString.make(expt)).val(shot).fin();
    }

    public final static Request<Int32> treePutRecord(final int nid, final Descriptor<?> dsc, final int utility_update) {
        return new TreeCall<Int32>(Int32.class, "TreePutRecord", 70).val(nid).xd(dsc).val(utility_update).fin();
    }

    public final static Request<Int32> treePutRow(final int nid, final int bufsize, final long timestamp, final Descriptor_A<?> data) {
        return new TreeCall<Int32>(Int32.class, "TreePutRow", 94).val(nid).val(bufsize).ref(timestamp).xd(data).fin();
    }

    public final static Request<Int32> treePutSegment(final int nid, final int rowidx, final Descriptor_A<?> data) {
        return new TreeCall<Int32>(Int32.class, "TreePutSegment", 67).val(nid).val(rowidx).descr(data).fin();
    }

    public final static Request<Int32> treePutTimestampedSegment(final int nid, final long timestamp, final Descriptor_A<?> data) {
        return new TreeCall<Int32>(Int32.class, "TreePutTimestampedSegment", 92).val(nid).ref(timestamp).xd(data).fin();
    }

    public final static Request<Int32> treeQuitTree(final String expt, final int shot) {
        return new TreeCall<Int32>(Int32.class, "TreeQuitTree", 46).ref(CString.make(expt)).val(shot).fin();
    }

    public final static Request<Int32> treeRemoveNodesTags(final int nid) {
        return new TreeCall<Int32>(Int32.class, "TreeRemoveNodesTags", 46).val(nid).fin();
    }

    public final static Request<Int32> treeRenameNode(final int nid, final String name) {
        return new TreeCall<Int32>(Int32.class, "TreeRenameNode", 48).val(nid).ref(CString.make(name)).fin();
    }

    public final static Request<Int32> treeRestoreContext(final Pointer treectx) {
        return new TreeCall<Int32>(Int32.class, "TreeRestoreContext", 35).val(treectx).fin();
    }

    public final static Request<Pointer> treeSaveContext() {
        return new TreeCall<Pointer>(Pointer.class, "TreeSaveContext:P", 28).fin();
    }

    public final static Request<Int32> treeSetCurrentShotId(final String expt, final int shot) {
        return new TreeCall<Int32>(Int32.class, "TreeSetCurrentShotId", 54).ref(CString.make(expt)).val(shot).fin();
    }

    public final static Request<Int32Array> treeSetDefault(final String path) {
        return new TreeCall<Int32Array>(Int32Array.class, "_a=-1", "TreeSetDefault", 55).ref(CString.make(path)).ref("_a").fin("[_a,_s]");
    }

    public final static Request<Int32> treeSetDefaultNid(final int nid) {
        return new TreeCall<Int32>(Int32.class, "TreeSetDefaultNid", 44).val(nid).fin();
    }

    public final static Request<Int32> treeSetNciItm(final int nid, final boolean state, final int flags) {
        return new TreeCall<Int32>(Int32.class, "TreeSetNciItm", 74).val(nid).val(state ? 1 : 2).val(flags).fin();
    }

    public final static Request<Int32> treeSetNoSubtree(final int nid) {
        return new TreeCall<Int32>(Int32.class, "TreeSetNoSubtree", 43).val(nid).fin();
    }

    public final static Request<Int32> treeSetSegmentScale(final int nid, final Descriptor<?> scale) {
        return new TreeCall<Int32>(Int32.class, "TreeSetSegmentScale", 59).val(nid).xd(scale).fin();
    }

    public final static Request<Int32> treeSetSubtree(final int nid) {
        return new TreeCall<Int32>(Int32.class, "TreeSetSubtree", 41).val(nid).fin();
    }

    public final static Request<Int32> treeSetTimeContext(final NUMBER<?> start, final NUMBER<?> end, final NUMBER<?> delta) {
        return new TreeCall<Int32>(Int32.class, "TreeSetTimeContext", 55).descr(start).descr(end).descr(delta).fin();
    }

    public final static Request<Int32> treeSetXNci(final int nid, final String name, final Descriptor<?> value) {
        return new TreeCall<Int32>(Int32.class, "TreeSetXNci", 58).val(nid).ref(CString.make(name)).xd(value).fin();
    }

    public final static Request<Int32> treeStartConglomerate(final int size) {
        return new TreeCall<Int32>(Int32.class, "TreeStartConglomerate", 48).val(size).fin();
    }

    public static final Request<Pointer> treeSwitchDbid(final Pointer ctx) {
        return new TreeCall<Pointer>(Pointer.class, "TreeSwitchDbid:P", 33).val(ctx).fin();
    }

    public final static Request<Int32> treeTurnOff(final int nid) {
        return new TreeCall<Int32>(Int32.class, "TreeTurnOff", 38).val(nid).fin();
    }

    public final static Request<Int32> treeTurnOn(final int nid) {
        return new TreeCall<Int32>(Int32.class, "TreeTurnOn", 37).val(nid).fin();
    }

    public final static Request<Int32> treeUpdateSegment(final int nid, final Descriptor<?> start, final Descriptor<?> end, final Descriptor<?> dim, final int idx) {
        return new TreeCall<Int32>(Int32.class, "TreeUpdateSegment", 88).val(nid).descr(start).descr(end).descr(dim).val(idx).fin();
    }

    public final static Request<Int32> treeUpdateSegment(final int nid, final long start, final long end, final Descriptor<?> dim, final int idx) {
        return new TreeCall<Int32>(Int32.class, "TreeUpdateSegment", 60).val(nid).ref(start).ref(end).ref(dim).val(idx).fin();
    }

    public final static Request<Int32> treeUsePrivateCtx(final boolean state) {
        return new TreeCall<Int32>(Int32.class, "TreeUsePrivateCtx", 44).val(state ? 1 : 0).fin();
    }

    public final static Request<Int32> treeUsingPrivateCtx() {
        return new TreeCall<Int32>(Int32.class, "TreeUsingPrivateCtx", 30).fin();
    }

    public static final Request<Int32> treeVerify() {
        return new TreeCall<Int32>(Int32.class, "TreeVerify", 21).fin();
    }

    public final static Request<Int32> treeWriteTree(final String expt, final int shot) {
        return new TreeCall<Int32>(Int32.class, "TreeWriteTree", 47).ref(CString.make(expt)).val(shot).fin();
    }

    public TreeShr(final Mds mds){
        super(mds);
    }

    /**
     * returns teh absolute (full) path of the node
     *
     * @return int: string
     **/
    public final String treeAbsPath(final Pointer ctx, final String relpath) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeAbsPath(relpath)).getValue();
    }

    /**
     * adds a device node of specific model to a tree (EDIT)
     *
     * @return IntegerStatus
     **/
    public final IntegerStatus treeAddConglom(final Pointer ctx, final String path, final String model) throws MdsException {
        return new IntegerStatus(this.mds.getDescriptor(ctx, TreeShr.treeAddConglom(path, model)));
    }

    /**
     * adds a node of specific usage to a tree (EDIT)
     *
     * @return IntegerStatus
     **/
    public final IntegerStatus treeAddNode(final Pointer ctx, final String path, final byte usage) throws MdsException {
        return new IntegerStatus(this.mds.getDescriptor(ctx, TreeShr.treeAddNode(path, usage)));
    }

    /**
     * adds a tag to a node
     *
     * @return int: status
     **/
    public final int treeAddTag(final Pointer ctx, final int nid, final String tag) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeAddTag(nid, tag)).getValue();
    }

    /**
     * initiates a new time-stamped segment
     *
     * @return int: status
     **/
    public final int treeBeginTimestampedSegment(final Pointer ctx, final int nid, final Descriptor_A<?> initialValue, final int idx) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeBeginTimestampedSegment(nid, initialValue, idx)).getValue();
    }

    /**
     * cleans the data file by removing unreferenced data
     *
     * @return int: status
     **/
    public final int treeCleanDatafile(final Null NULL, final String expt, final int shot) throws MdsException {
        return this.mds.getDescriptor(null, TreeShr.treeCleanDatafile(expt, shot)).getValue();
    }

    /**
     * closes a tree
     *
     * @return int: status
     **/
    public final int treeClose(final Pointer ctx, final String expt, final int shot) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeClose(expt, shot)).getValue();
    }

    /**
     * cleans and compresses the data file of a tree
     *
     * @return int: status
     **/
    public final int treeCompressDatafile(final Null NULL, final String expt, final int shot) throws MdsException {
        return this.mds.getDescriptor(null, TreeShr.treeCompressDatafile(expt, shot)).getValue();
    }

    /**
     * creates a copy of a tree with a new shot number
     *
     * @return int: status
     **/
    public final int treeCreateTreeFiles(final Null NULL, final String expt, final int newshot, final int fromshot) throws MdsException {
        return this.mds.getDescriptor(null, TreeShr.treeCreateTreeFiles(expt, newshot, fromshot)).getValue();
    }

    /**
     * returns the current tree context
     *
     * @return Pointer: context
     **/
    public final Pointer treeCtx(final Pointer ctx) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeCtx());
    }

    /**
     * returns the dbid pointer of the current context
     *
     * @return Pointer: current dbid
     **/
    public Pointer treeDbid(final Pointer ctx) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeDbid());
    }

    /**
     * executes the initiated delete operation
     *
     * @return int: status
     **/
    public final int treeDeleteNodeExecute(final Pointer ctx) throws MdsException {
        this.mds.getDescriptor(ctx, TreeShr.treeDeleteNodeExecute()).getValue();
        return MdsException.TreeNORMAL;// TODO: change once mdsplus returns status for treeDeleteNodeExecute
    }

    /** @return IntegerStatus: nid-number of the next node in the list of nodes to be deleted **/
    public final IntegerStatus treeDeleteNodeGetNid(final Pointer ctx, final int idx) throws MdsException {
        return new IntegerStatus(this.mds.getDescriptor(ctx, TreeShr.treeDeleteNodeGetNid(idx)));
    }

    /**
     * initiates a delete operation of a node and its descendants (primitive)
     *
     * @return IntegerStatus
     **/
    public final IntegerStatus treeDeleteNodeInitialize(final Pointer ctx, final int nid) throws MdsException {
        return this.treeDeleteNodeInitialize(ctx, nid, 0, true);
    }

    /**
     * initiates a delete operation of a node and its descendants
     *
     * @return IntegerStatus
     **/
    public final IntegerStatus treeDeleteNodeInitialize(final Pointer ctx, final int nid, final int count, final boolean init) throws MdsException {
        return new IntegerStatus(this.mds.getDescriptor(ctx, TreeShr.treeDeleteNodeInitialize(nid, count, init)));
    }

    public final DescriptorStatus treeDoMethod(final Pointer ctx, final int nid, final String method, final Descriptor<?>... args) throws MdsException {
        return new DescriptorStatus(this.mds.getDescriptor(ctx, TreeShr.treeDoMethod(nid, method, args)));
    }

    /**
     * finishes the added conglomerate
     *
     * @return IntegerStatus
     **/
    public final int treeEndConglomerate(final Pointer ctx) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeEndConglomerate()).getValue();
    }

    public final StringStatus treeFileName(final Pointer ctx, final String expt, final int shot) throws MdsException {
        return new StringStatus(this.mds.getDescriptor(ctx, TreeShr.treeFileName(expt, shot)));
    }

    /**
     * searches for the next tag assigned to a node
     *
     * @return TagRef: next tag found
     **/
    public final TagRef treeFindNodeTags(final Pointer ctx, final int nid, final TagRef ref) throws MdsException {
        return new TagRef(this.mds.getDescriptor(ctx, TreeShr.treeFindNodeTagsDsc(nid, ref)));
    }

    /**
     * searches for the next node meeting the criteria of a matching search string and usage mask
     *
     * @return NodeRefStatus: next nid-number found
     **/
    public final NodeRefStatus treeFindNodeWild(final Pointer ctx, final String searchstr, final int usage_mask, final NodeRefStatus ref) throws MdsException {
        return new NodeRefStatus(this.mds.getDescriptor(ctx, TreeShr.treeFindNodeWild(searchstr, usage_mask, ref)));
    }

    /**
     * searches for the next tag in the tag list matching a search string
     *
     * @return TagRefStatus: next tag found
     **/
    public final TagRefStatus treeFindTagWild(final Pointer ctx, final String searchstr, final TagRefStatus ref) throws MdsException {
        return new TagRefStatus(this.mds.getDescriptor(ctx, TreeShr.treeFindTagWild(searchstr, ref)));
    }

    /**
     * resolves the current shot number (0)
     *
     * @return int: shot number
     **/
    public final int treeGetCurrentShotId(final Null NULL, final String expt) throws MdsException {
        return this.mds.getDescriptor(null, TreeShr.treeGetCurrentShotId(expt)).getValue();
    }

    /**
     * checks the size of the data file
     *
     * @return long: file size in byte
     **/
    public final int treeGetDatafileSize(final Pointer ctx) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeGetDatafileSize()).getValue();
    }

    /**
     * checks the default node of active tree
     *
     * @return IntegerStatus: nid-number of default node
     **/
    public final IntegerStatus treeGetDefaultNid(final Pointer ctx) throws MdsException {
        return new IntegerStatus(this.mds.getDescriptor(ctx, TreeShr.treeGetDefaultNid()));
    }

    /**
     * returns the shortest path to node (from default node)
     *
     * @return String: minimal path to node
     **/
    public final String treeGetMinimumPath(final Pointer ctx, final int nid) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeGetMinimumPath(nid)).getValue();
    }

    /**
     * @return IntegerStatus: number of segments
     **/
    public final IntegerStatus treeGetNumSegments(final Pointer ctx, final int nid) throws MdsException {
        return new IntegerStatus(this.mds.getDescriptor(ctx, TreeShr.treeGetNumSegments(nid)));
    }

    /**
     * returns the natural path of a node (form its parent tree root)
     *
     * @return String: path to node
     **/
    public final String treeGetPath(final Pointer ctx, final int nid) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeGetPath(nid)).getValue();
    }

    /**
     * checks for the current private context state
     *
     * @return boolean: true if private
     **/
    public final boolean treeGetPrivateCtx() throws MdsException {
        return this.mds.getDescriptor(null, TreeShr.treeUsingPrivateCtx()).getValue() == 1;
    }

    /**
     * reads the full record of a node
     *
     * @return DescriptorStatus: record of node
     **/
    public final Descriptor<?> treeGetRecord(final Pointer ctx, final int nid) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeGetRecord(nid));// compressed data cannot be put in a list without decompression
    }

    /**
     * reads idx-th segment of a node
     *
     * @return SignalStatus: segment
     **/
    public final List treeGetSegment(final Pointer ctx, final int nid, final int idx) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeGetSegment(nid, idx));
    }

    /**
     * @return SegmentInfo: info about the idx-th segment of a node
     **/
    public final SegmentInfo treeGetSegmentInfo(final Pointer ctx, final int nid, final int idx) throws MdsException {
        return new SegmentInfo(this.mds.getDescriptor(ctx, TreeShr.treeGetSegmentInfo(nid, idx)));
    }

    /**
     * @return DescriptorStatus: time limits of the idx-th segment of a node
     **/
    public final DescriptorStatus treeGetSegmentLimits(final Pointer ctx, final int nid, final int idx) throws MdsException {
        return new DescriptorStatus(this.mds.getDescriptor(ctx, TreeShr.treeGetSegmentLimits(nid, idx)));
    }

    public final Descriptor<?> treeGetSegmentScale(final Pointer ctx, final int nid) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeGetSegmentScale(nid));
    }

    public final List treeGetSegmentTimesXd(final Pointer ctx, final int nid) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeGetSegmentTimesXd(nid));
    }

    /**
     * reads the list of attributes of a node
     *
     * @return int: status
     **/
    public final DescriptorStatus treeGetXNci(final Pointer ctx, final int nid) throws MdsException {
        return this.treeGetXNci(ctx, nid, "attributenames");
    }

    /**
     * reads an extended attribute of a node
     *
     * @return int: status
     **/
    public final DescriptorStatus treeGetXNci(final Pointer ctx, final int nid, final String name) throws MdsException {
        return new DescriptorStatus(this.mds.getDescriptor(ctx, TreeShr.treeGetXNci(nid, name)));
    }

    /**
     * checks the state of a node
     *
     * @return int: status
     **/
    public final int treeIsOn(final Pointer ctx, final int nid) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeIsOn(nid)).getValue();
    }

    /**
     * adds a new segment to segmented node
     *
     * @return int: status
     **/
    public final int treeMakeSegment(final Pointer ctx, final int nid, final Descriptor<?> start, final Descriptor<?> end, final Descriptor<?> dimension, final Descriptor_A<?> values, final int idx, final int rows_filled) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeMakeSegment(nid, start, end, dimension, values, idx, rows_filled)).getValue();
    }

    /**
     * adds a new time-stamped segment to segmented node
     *
     * @return int: status
     **/
    public final int treeMakeTimestampedSegment(final Pointer ctx, final int nid, final Int64Array timestamps, final Descriptor_A<?> values, final int idx, final int rows_filled) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeMakeTimestampedSegment(nid, timestamps, values, idx, rows_filled)).getValue();
    }

    /**
     * adds a new time-stamped segment to segmented node
     *
     * @return int: status
     **/
    public final int treeMakeTimestampedSegment(final Pointer ctx, final int nid, final long[] dim, final Descriptor_A<?> values, final int idx, final int rows_filled) throws MdsException {
        return this.treeMakeTimestampedSegment(ctx, nid, new Int64Array(dim), values, idx, rows_filled);
    }

    /**
     * open tree and in normal or read-only mode
     *
     * @return int: status
     **/
    public final int treeOpen(final Pointer ctx, final String expt, final int shot, final boolean readonly) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeOpen(expt, shot, readonly)).getValue();
    }

    /**
     * open tree in edit mode
     *
     * @return int: status
     **/
    public final int treeOpenEdit(final Pointer ctx, final String expt, final int shot) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeOpenEdit(expt, shot)).getValue();
    }

    /**
     * create new tree and open it in edit mode
     *
     * @return int: status
     **/
    public final int treeOpenNew(final Pointer ctx, final String expt, final int shot) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeOpenNew(expt, shot)).getValue();
    }

    /**
     * sets the record of a node (primitive)
     *
     * @return int: status
     **/
    public final int treePutRecord(final Pointer ctx, final int nid, final Descriptor<?> dsc) throws MdsException {
        return this.treePutRecord(ctx, nid, dsc, 0);
    }

    /**
     * sets the record of a node
     *
     * @param utility_update
     *            2:compress
     *            !=0: no_write-flags are ignored and flags are reset to 0x5400
     * @return int: status
     **/
    public final int treePutRecord(final Pointer ctx, final int nid, final Descriptor<?> dsc, final int utility_update) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treePutRecord(nid, dsc, utility_update)).getValue();
    }

    /**
     * adds row to segmented node
     *
     * @return int: status
     **/
    public final int treePutRow(final Pointer ctx, final int nid, final int bufsize, final long timestamp, final Descriptor_A<?> data) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treePutRow(nid, bufsize, timestamp, data)).getValue();
    }

    /**
     * adds a segment to a segmented node
     *
     * @return int: status
     **/
    public final int treePutSegment(final Pointer ctx, final int nid, final int rowidx, final Descriptor_A<?> data) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treePutSegment(nid, rowidx, data)).getValue();
    }

    /**
     * adds a time-stamped segment to segmented node
     *
     * @return int: status
     **/
    public final int treePutTimestampedSegment(final Pointer ctx, final int nid, final long timestamp, final Descriptor_A<?> data) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treePutTimestampedSegment(nid, timestamp, data)).getValue();
    }

    /**
     * exits a tree (EDIT)
     *
     * @return int: status
     **/
    public final int treeQuitTree(final Pointer ctx, final String expt, final int shot) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeQuitTree(expt, shot)).getValue();
    }

    /**
     * clears all tags assigned to a node (EDIT)
     *
     * @return int: status
     **/
    public final int treeRemoveNodesTags(final Pointer ctx, final int nid) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeRemoveNodesTags(nid)).getValue();
    }

    /**
     * renames/moves a node (EDIT)
     *
     * @return int: status
     **/
    public final int treeRenameNode(final Pointer ctx, final int nid, final String name) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeRenameNode(nid, name)).getValue();
    }

    /**
     * restores a saved context
     *
     * @return int: status; 1 on success, context if input is null
     **/
    public final int treeRestoreContext(final Pointer ctx, final Pointer treectx) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeRestoreContext(treectx)).getValue();
    }

    /**
     * save the current context
     *
     * @return Pointer: pointer to saved context
     **/
    public final Pointer treeSaveContext(final Pointer ctx) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeSaveContext());
    }

    /**
     * sets the current shot of an experiment
     *
     * @return int: status
     **/
    public final int treeSetCurrentShotId(final Pointer ctx, final String expt, final int shot) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeSetCurrentShotId(expt, shot)).getValue();
    }

    public final IntegerStatus treeSetDefault(final Pointer ctx, final String path) throws MdsException {
        return new IntegerStatus(this.mds.getDescriptor(ctx, TreeShr.treeSetDefault(path)));
    }

    /**
     * makes a node the default node
     *
     * @return int: status
     **/
    public final int treeSetDefaultNid(final Pointer ctx, final int nid) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeSetDefaultNid(nid)).getValue();
    }

    /**
     * sets or resets the flags selected by a mask
     *
     * @return int: status
     **/
    public final int treeSetNciItm(final Pointer ctx, final int nid, final boolean state, final int flags) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeSetNciItm(nid, state, flags)).getValue();
    }

    /**
     * removes subtree flag from a node (EDIT)
     *
     * @return int: status
     **/
    public final int treeSetNoSubtree(final Pointer ctx, final int nid) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeSetNoSubtree(nid)).getValue();
    }

    /**
     * sets the current private context state
     *
     * @return boolean: previous state
     **/
    public final boolean treeSetPrivateCtx(final boolean state) throws MdsException {
        return this.mds.getDescriptor(null, TreeShr.treeUsePrivateCtx(state)).getValue() == 1;
    }

    public final int treeSetSegmentScale(final Pointer ctx, final int nid, final Descriptor<?> scale) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeSetSegmentScale(nid, scale)).getValue();
    }

    /**
     * adds subtree flag to a node (EDIT)
     *
     * @return int: status
     **/
    public final int treeSetSubtree(final Pointer ctx, final int nid) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeSetSubtree(nid)).getValue();
    }

    /**
     * clears current time context that will affect signal reads from a tree
     *
     * @return int: status
     **/
    public final int treeSetTimeContext(final Pointer ctx) throws MdsException {
        return this.treeSetTimeContext(ctx, (NUMBER<?>)null, (NUMBER<?>)null, (NUMBER<?>)null);
    }

    /**
     * sets the current time context that will affect signal reads a a tree
     *
     * @return int: status
     **/
    public final int treeSetTimeContext(final Pointer ctx, final Number start, final Number end, final Number delta) throws MdsException {
        return this.treeSetTimeContext(ctx, NUMBER.NEW(start), NUMBER.NEW(end), NUMBER.NEW(delta));
    }

    /**
     * sets the current time context that will affect signal reads from a tree
     *
     * @return int: status
     **/
    public final int treeSetTimeContext(final Pointer ctx, final NUMBER<?> start, final NUMBER<?> end, final NUMBER<?> delta) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeSetTimeContext(start, end, delta)).getValue();
    }

    /**
     * sets/adds an extended attribute to a node
     *
     * @return int: status
     **/
    public final int treeSetXNci(final Pointer ctx, final int nid, final String name, final Descriptor<?> value) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeSetXNci(nid, name, value)).getValue();
    }

    /**
     * initiates a conglomerate of nodes similar to devices (EDIT)
     *
     * @return int: status
     **/
    public final int treeStartConglomerate(final Pointer ctx, final int size) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeStartConglomerate(size)).getValue();
    }

    public final Pointer treeSwitchDbid(final Null NULL, final Pointer ctx) throws MdsException {
        return this.mds.getDescriptor(null, TreeShr.treeSwitchDbid(ctx));
    }

    /**
     * turns a node off (265392050 : TreeLock-Failure but does change the state)
     *
     * @return int: status
     **/
    public final int treeTurnOff(final Pointer ctx, final int nid) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeTurnOff(nid)).getValue();
    }

    /**
     * turns a node on (265392050 : TreeLock-Failure but does change the state)
     *
     * @return int: status
     **/
    public final int treeTurnOn(final Pointer ctx, final int nid) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeTurnOn(nid)).getValue();
    }

    public final int treeUpdateSegment(final Pointer ctx, final int nid, final Descriptor<?> start, final Descriptor<?> end, final Descriptor<?> dim, final int idx) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeUpdateSegment(nid, start, end, dim, idx)).getValue();
    }

    public final int treeUpdateSegment(final Pointer ctx, final int nid, final long start, final long end, final Descriptor<?> dim, final int idx) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeUpdateSegment(nid, start, end, dim, idx)).getValue();
    }

    public final int treeVerify(final Pointer ctx) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeVerify()).getValue();
    }

    /**
     * writes changes to a tree (EDIT)
     *
     * @return int: status
     **/
    public final int treeWriteTree(final Pointer ctx, final String expt, final int shot) throws MdsException {
        return this.mds.getDescriptor(ctx, TreeShr.treeWriteTree(expt, shot)).getValue();
    }
}

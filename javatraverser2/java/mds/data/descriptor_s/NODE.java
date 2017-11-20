package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import debug.DEBUG;
import mds.MdsException;
import mds.TCL;
import mds.TreeShr.SegmentInfo;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.TREE;
import mds.data.TREE.NodeInfo;
import mds.data.TREE.RecordInfo;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;
import mds.data.descriptor.Descriptor_S;
import mds.data.descriptor_a.NidArray;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_r.Signal;

public abstract class NODE<T>extends Descriptor_S<T>{
    public static final class Flags{
        public static final int STATE             = 1 << 0;
        public static final int PARENT_STATE      = 1 << 1;
        public static final int ESSENTIAL         = 1 << 2;
        public static final int CACHED            = 1 << 3;
        public static final int VERSION           = 1 << 4;
        public static final int SEGMENTED         = 1 << 5;
        public static final int SETUP             = 1 << 6;
        public static final int WRITE_ONCE        = 1 << 7;
        public static final int COMPRESSIBLE      = 1 << 8;
        public static final int DO_NOT_COMPRESS   = 1 << 9;
        public static final int COMPRESS_ON_PUT   = 1 << 10;
        public static final int NO_WRITE_MODEL    = 1 << 11;
        public static final int NO_WRITE_SHOT     = 1 << 12;
        public static final int PATH_REFERENCE    = 1 << 13;
        public static final int NID_REFERENCE     = 1 << 14;
        public static final int INCLUDE_IN_PULSE  = 1 << 15;
        public static final int COMPRESS_SEGMENTS = 1 << 16;
        public static final int ERROR             = 1 << 31;
        public final int        flags;

        public Flags(){
            this(Integer.MIN_VALUE);
        }

        public Flags(final int flags){
            this.flags = flags;
        }

        public final StringBuilder info(final StringBuilder sb, final String sep) {
            sb.append(this.isOn() ? "on" : "off");
            sb.append(sep).append("parent is ").append(this.isParentOn() ? "on" : "off");
            if(this.isSetup()) sb.append(sep).append("setup");
            if(this.isEssential()) sb.append(sep).append("essential");
            if(this.isCached()) sb.append(sep).append("cached");
            if(this.isVersion()) sb.append(sep).append("version");
            if(this.isSegmented()) sb.append(sep).append("segmented");
            if(this.isWriteOnce()) sb.append(sep).append("write once");
            if(this.isCompressible()) sb.append(sep).append("compressible");
            if(this.isDoNotCompress()) sb.append(sep).append("do not compress");
            if(this.isCompressOnPut()) sb.append(sep).append("compress on put");
            if(this.isNoWriteModel()) sb.append(sep).append("no write model");
            if(this.isNoWriteShot()) sb.append(sep).append("no write shot");
            if(this.isPathReference()) sb.append(sep).append("path reference");
            if(this.isNidReference()) sb.append(sep).append("nid reference");
            if(this.isCompressSegments()) sb.append(sep).append("compress segments");
            if(this.isIncludeInPulse()) sb.append(sep).append("include in pulse");
            return sb;
        }

        public final boolean isCached() {
            return (this.flags & Flags.CACHED) != 0;
        }

        public final boolean isCompressible() {
            return (this.flags & Flags.COMPRESSIBLE) != 0;
        }

        public final boolean isCompressOnPut() {
            return (this.flags & Flags.COMPRESS_ON_PUT) != 0;
        }

        public final boolean isCompressSegments() {
            return (this.flags & Flags.COMPRESS_SEGMENTS) != 0;
        }

        public final boolean isDoNotCompress() {
            return (this.flags & Flags.DO_NOT_COMPRESS) != 0;
        }

        public final boolean isError() {
            return this.flags < 0;
        }

        public final boolean isEssential() {
            return (this.flags & Flags.ESSENTIAL) != 0;
        }

        public final boolean isIncludeInPulse() {
            return (this.flags & Flags.INCLUDE_IN_PULSE) != 0;
        }

        public final boolean isNidReference() {
            return (this.flags & Flags.NID_REFERENCE) != 0;
        }

        public final boolean isNoWriteModel() {
            return (this.flags & Flags.NO_WRITE_MODEL) != 0;
        }

        public final boolean isNoWriteShot() {
            return (this.flags & Flags.NO_WRITE_SHOT) != 0;
        }

        public final boolean isOn() {
            return !this.isState();
        }

        public final boolean isParentOn() {
            return !this.isParentState();
        }

        public final boolean isParentState() {
            return (this.flags & Flags.PARENT_STATE) != 0;
        }

        public final boolean isPathReference() {
            return (this.flags & Flags.PATH_REFERENCE) != 0;
        }

        public final boolean isSegmented() {
            return (this.flags & Flags.SEGMENTED) != 0;
        }

        public final boolean isSetup() {
            return (this.flags & Flags.SETUP) != 0;
        }

        public final boolean isState() {
            return (this.flags & Flags.STATE) != 0;
        }

        public final boolean isVersion() {
            return (this.flags & Flags.VERSION) != 0;
        }

        public final boolean isWriteOnce() {
            return (this.flags & Flags.WRITE_ONCE) != 0;
        }

        @Override
        public final String toString() {
            return this.info(new StringBuilder(256), ", ").toString();
        }
    }
    public static final byte     USAGE_MAXIMUM       = 12;
    public static final byte     USAGE_ANY           = 0;
    public static final byte     USAGE_STRUCTURE     = 1;
    public static final byte     USAGE_ACTION        = 2;
    public static final byte     USAGE_DEVICE        = 3;
    public static final byte     USAGE_DISPATCH      = 4;
    public static final byte     USAGE_NUMERIC       = 5;
    public static final byte     USAGE_SIGNAL        = 6;
    public static final byte     USAGE_TASK          = 7;
    public static final byte     USAGE_TEXT          = 8;
    public static final byte     USAGE_WINDOW        = 9;
    public static final byte     USAGE_AXIS          = 10;
    public static final byte     USAGE_SUBTREE       = 11;
    public static final byte     USAGE_COMPOUND_DATA = 12;
    public static final int      CHILD               = 1;
    public static final int      MEMBER              = 2;
    private static final boolean atomic              = false;

    public static final String getUsageStr(final byte usage) {
        switch(usage){
            default:
                return "ALL";
            case USAGE_ANY:
                return "ANY";
            case USAGE_STRUCTURE:
                return "STRUCTURE";
            case USAGE_ACTION:
                return "ACTION";
            case USAGE_DEVICE:
                return "DEVICE";
            case USAGE_DISPATCH:
                return "DISPATCH";
            case USAGE_NUMERIC:
                return "NUMERIC";
            case USAGE_SIGNAL:
                return "SIGNAL";
            case USAGE_TASK:
                return "TASK";
            case USAGE_TEXT:
                return "TEXT";
            case USAGE_WINDOW:
                return "WINDOW";
            case USAGE_AXIS:
                return "AXIS";
            case USAGE_SUBTREE:
                return "SUBTREE";
            case USAGE_COMPOUND_DATA:
                return "COMPOUND_DATA";
        }
    }

    public NODE(final byte dtype, final ByteBuffer data){
        super(dtype, data);
    }

    public NODE(final ByteBuffer b){
        super(b);
    }

    public final Nid addConglom(final String name, final String model) throws MdsException {
        return this.tree.addConglom(this, name, model);
    }

    public final Nid addNode(final String name, final byte usage) throws MdsException {
        synchronized(this.tree.mds){
            final Nid def = this.tree.getDefaultNid();
            this.setDefault();
            final Nid nid = this.tree.addNode(name, usage);
            def.setDefault();
            if(usage == NODE.USAGE_SUBTREE) nid.setSubtree();
            return nid;
        }
    }

    public final NODE<T> addTag(final String tag) throws MdsException {
        this.tree.addTag(this.getNidNumber(), tag);
        return this;
    }

    public final NODE<T> clearFlags(final int flags) throws MdsException {
        this.tree.clearFlags(this.getNidNumber(), flags);
        return this;
    }

    public final NODE<T> clearTags() throws MdsException {
        this.tree.clearTags(this.getNidNumber());
        return this;
    }

    public int deleteInitialize() throws MdsException {
        return this.tree.deleteNodeInitialize(this.getNidNumber());
    }

    public final NODE<T> doAction() throws MdsException {
        this.tree.doAction(this.getNidNumber());
        return this;
    }

    public final Descriptor<?> doDeviceMethod(final String method, final Descriptor<?>... args) throws MdsException {
        return this.doDeviceMethod(method, args);
    }

    public final NODE<T> doDeviceMethod(final String method, final String arg) throws MdsException {
        new TCL(this.tree.mds).doMethod(this.getNciPath(), method, arg, true);
        return this;
    }

    public final NODE<?> followReference() throws MdsException {
        final byte rec_dtype = this.getNciDType();
        if(rec_dtype == DTYPE.NID || rec_dtype == DTYPE.PATH) return ((NODE<?>)this.getNciRecord()).followReference();
        return this;
    }

    @Override
    public final DATA<?> getData() throws MdsException {
        return this.tree.mds.getDescriptor(this.tree, "DATA($)", this).getData();
    }

    public final Descriptor<?> getNci(final String name) throws MdsException {
        if(DEBUG.D) System.err.println(name);
        return this.tree.getNci(this, name);
    }

    public final Nid getNciBrother() throws MdsException {
        return (Nid)this.getNci(TREE.NCI_BROTHER);
    }

    public final Nid getNciChild() throws MdsException {
        return (Nid)this.getNci(TREE.NCI_CHILD);
    }

    public final NidArray getNciChildrenNids() throws MdsException {
        return this.tree.getNciChildrenNids(this);
    }

    public final byte getNciClass() throws MdsException {
        return this.getNci(TREE.NCI_CLASS).toByte();
    }

    public final String getNciClassStr() throws MdsException {
        return this.getNci(TREE.NCI_CLASS_STR).toString();
    }

    public final short getNciConglomerateElt() throws MdsException {
        return this.getNci(TREE.NCI_CONGLOMERATE_ELT).toShort();
    }

    public final NidArray getNciConglomerateNids() throws MdsException {
        return (NidArray)this.getNci(TREE.NCI_CONGLOMERATE_NIDS);
    }

    public final int getNciDataInNci() throws MdsException {
        return this.getNci(TREE.NCI_DATA_IN_NCI).toInt();
    }

    public final int getNciDepth() throws MdsException {
        return this.getNci(TREE.NCI_DEPTH).toInt();
    }

    public final byte getNciDType() throws MdsException {
        return this.getNci(TREE.NCI_DTYPE).toByte();
    }

    public final String getNciDTypeStr() throws MdsException {
        return this.getNci(TREE.NCI_DTYPE_STR).toString();
    }

    public final int getNciErrorOnPut() throws MdsException {
        return this.getNci(TREE.NCI_ERROR_ON_PUT).toInt();
    }

    public final int getNciFlags() throws MdsException {
        return this.getNci(TREE.NCI_GET_FLAGS).toInt();
    }

    public final String getNciFullPath() throws MdsException {
        return this.getNci(TREE.NCI_FULLPATH).toString();
    }

    public final boolean getNciIsChild() throws MdsException {
        return this.getNci(TREE.NCI_IS_CHILD).toByte() != 0;
    }

    public final boolean getNciIsMember() throws MdsException {
        return this.getNci(TREE.NCI_IS_MEMBER).toByte() != 0;
    }

    public final int getNciLength() throws MdsException {
        return this.getNci(TREE.NCI_LENGTH).toInt();
    }

    public final Nid getNciMember() throws MdsException {
        return (Nid)this.getNci(TREE.NCI_MEMBER);
    }

    public final NidArray getNciMemberNids() throws MdsException {
        return this.tree.getNciMemberNids(this);
    }

    public final String getNciMinPath() throws MdsException {
        return this.getNci(TREE.NCI_MINPATH).toString();
    }

    public final int getNciNidNumber() throws MdsException {
        return this.getNci(TREE.NCI_NID_NUMBER).toInt();
    }

    public final String getNciNodeName() throws MdsException {
        return this.getNci(TREE.NCI_NODE_NAME).toString().trim();
    }

    public final int getNciNumberOfChildren() throws MdsException {
        return this.getNci(TREE.NCI_NUMBER_OF_CHILDREN).toInt();
    }

    public final int getNciNumberOfElts() throws MdsException {
        return this.getNci(TREE.NCI_NUMBER_OF_ELTS).toInt();
    }

    public final int getNciNumberOfMembers() throws MdsException {
        return this.getNci(TREE.NCI_NUMBER_OF_MEMBERS).toInt();
    }

    public final String getNciOriginalPartName() throws MdsException {
        return this.getNci(TREE.NCI_ORIGINAL_PART_NAME).toString().trim();
    }

    public final int getNciOwnerId() throws MdsException {
        return this.getNci(TREE.NCI_OWNER_ID).toInt();
    }

    public final Nid getNciParent() throws MdsException {
        return (Nid)this.getNci(TREE.NCI_PARENT);
    }

    public final int getNciParentRelationship() throws MdsException {
        return this.getNci(TREE.NCI_PARENT_RELATIONSHIP).toInt();
    }

    public final String getNciParentTree() throws MdsException {
        return this.getNci(TREE.NCI_PARENT_TREE).toString();
    }

    public final String getNciPath() throws MdsException {
        return this.getNci(TREE.NCI_PATH).toString();
    }

    public final Descriptor<?> getNciRecord() throws MdsException {
        return this.getNci(TREE.NCI_RECORD);
    }

    public final long getNciRfa() throws MdsException {
        return this.getNci("RFA").toLong();
    }

    public final int getNciRLength() throws MdsException {
        return this.getNci(TREE.NCI_RLENGTH).toInt();
    }

    public final int getNciStatus() throws MdsException {
        return this.getNci(TREE.NCI_STATUS).toInt();
    }

    public final long getNciTimeInserted() throws MdsException {
        return this.getNci(TREE.NCI_TIME_INSERTED).toLong();
    }

    public final String getNciTimeInsertedStr() throws MdsException {
        return this.tree.getNciTimeInsertedStr(this);
    }

    public final byte getNciUsage() throws MdsException {
        return this.getNci(TREE.NCI_USAGE).toByte();
    }

    public final String getNciUsageStr() throws MdsException {
        return this.getNci(TREE.NCI_USAGE_STR).toString();
    }

    public final int getNciVersion() throws MdsException {
        return this.getNci(TREE.NCI_VERSION).toInt();
    }

    public abstract int getNidNumber() throws MdsException;

    public NodeInfo getNodeInfo() throws MdsException {
        return this.tree.getNodeInfo(this);
    }

    public final int getNumSegments() throws MdsException {
        return this.tree.getNumSegments(this.getNidNumber());
    }

    public final Descriptor<?> getRecord() throws MdsException {
        return this.tree.getRecord(this.getNidNumber());
    }

    public RecordInfo getRecordInfo() throws MdsException {
        return this.tree.getRecordInfo(this);
    }

    public final Signal getSegment(final int idx) throws MdsException {
        return this.tree.getSegment(this.getNidNumber(), idx);
    }

    public final SegmentInfo getSegmentInfo(final int idx) throws MdsException {
        return this.tree.getSegmentInfo(this.getNidNumber(), idx);
    }

    public final Descriptor<?> getSegmentLimits(final int idx) throws MdsException {
        return this.tree.getSegmentLimits(this.getNidNumber(), idx);
    }

    public final List getSegmentTimes() throws MdsException {
        return this.tree.getSegmentTimes(this.getNidNumber());
    }

    public final String[] getTags() throws MdsException {
        return this.tree.getTags(this.getNidNumber());
    }

    public final String[] getTagsLL() throws MdsException {
        return this.tree.getTagsLL(this.getNidNumber());
    }

    public final TREE getTree() {
        return this.tree;
    }

    public final Descriptor<?> getXNci(final String name) throws MdsException {
        return this.tree.getXNci(this.getNidNumber(), name);
    }

    @Override
    public final boolean isAtomic() {
        return NODE.atomic;
    }

    public final boolean isNidReference() throws MdsException {
        return new Flags(this.getNciFlags()).isNidReference();
    }

    public final boolean isOn() throws MdsException {
        final Flags flags = new Flags(this.getNciFlags());
        return flags.isOn() && flags.isParentOn();
    }

    public final boolean isPathReference() throws MdsException {
        return new Flags(this.getNciFlags()).isPathReference();
    }

    public final boolean isSegmented() throws MdsException {
        synchronized(this.tree.mds){
            if(new Flags(this.getNciFlags()).isSegmented()) return true; // cannot be sure due to issue in winter 2015/2016
            return this.getNumSegments() > 0;
        }
    }

    public final NODE<T> makeSegment(final Descriptor_A<?> dimension, final Descriptor_A<?> values) throws MdsException {
        this.tree.makeSegment(this.getNidNumber(), dimension.getScalar(0), dimension.getScalar(dimension.getLength() - 1), dimension, values, -1, dimension.getLength());
        return this;
    }

    public final NODE<T> makeSegment(final Descriptor<?> start, final Descriptor<?> end, final Descriptor<?> dimension, final Descriptor_A<?> values, final int idx, final int rows_filled) throws MdsException {
        this.tree.makeSegment(this.getNidNumber(), start, end, dimension, values, idx, rows_filled);
        return this;
    }

    public final NODE<T> putRecord(final Descriptor<?> data) throws MdsException {
        this.tree.putRecord(this.getNidNumber(), data);
        return this;
    }

    public final NODE<T> putRow(final long time, final Descriptor_A<?> data) throws MdsException {
        this.tree.putRow(this.getNidNumber(), time, data);
        return this;
    }

    public final NODE<T> putSegment(final int idx, final Descriptor_A<?> data) throws MdsException {
        this.tree.putSegment(this.getNidNumber(), idx, data);
        return this;
    }

    public final NODE<T> putTimestampedSegment(final int nid, final long timestamp, final Descriptor_A<?> data) throws MdsException {
        this.tree.putTimestampedSegment(this.getNidNumber(), timestamp, data);
        return this;
    }

    public final NODE<T> setDefault() throws MdsException {
        this.tree.setDefault(this.getNidNumber());
        return this;
    }

    public final NODE<T> setFlags(final int flags) throws MdsException {
        this.tree.setFlags(this.getNidNumber(), flags);
        return this;
    }

    public final NODE<T> setNoSubtree() throws MdsException {
        this.tree.setNoSubtree(this.getNidNumber());
        return this;
    }

    public final NODE<T> setOn(final boolean state) throws MdsException {
        if(state) this.tree.turnOn(this.getNidNumber());
        else this.tree.turnOff(this.getNidNumber());
        return this;
    }

    public final NODE<T> setPath(final String path) throws MdsException {
        this.tree.setPath(this.getNidNumber(), path);
        return this;
    }

    public final NODE<T> setSubtree() throws MdsException {
        this.tree.setSubtree(this.getNidNumber());
        return this;
    }

    public final NODE<T> setTags(final String... tags) throws MdsException {
        this.tree.setTags(this.getNidNumber(), tags);
        return this;
    }

    @Override
    public final byte[] toByteArray() {
        try{
            return this.tree.mds.getByteArray(this.tree, "BYTE($)", this);
        }catch(final MdsException e){
            return null;
        }
    }

    @Override
    public final double[] toDoubleArray() {
        try{
            return this.tree.mds.getDoubleArray(this.tree, "FT_FLOAT($)", this);
        }catch(final MdsException e){
            return null;
        }
    }

    @Override
    public final float[] toFloatArray() {
        try{
            return this.tree.mds.getFloatArray(this.tree, "FS_FLOAT($)", this);
        }catch(final MdsException e){
            return null;
        }
    }

    public abstract Path toFullPath() throws MdsException;

    @Override
    public final int[] toIntArray() {
        try{
            return this.tree.mds.getIntegerArray(this.tree, "LONG($)", this);
        }catch(final MdsException e){
            return null;
        }
    }

    @Override
    public final long[] toLongArray() {
        try{
            return this.tree.mds.getLongArray(this.tree, "QUADWORD($)", this);
        }catch(final MdsException e){
            return null;
        }
    }

    public abstract Path toMinPath() throws MdsException;

    public abstract Nid toNid() throws MdsException;

    public abstract Path toPath() throws MdsException;

    @Override
    public final short[] toShortArray() {
        try{
            return this.tree.mds.getShortArray(this.tree, "WORD($)", this);
        }catch(final MdsException e){
            return null;
        }
    }

    @Override
    public final String[] toStringArray() {
        try{
            return this.tree.mds.getStringArray(this.tree, "TEXT($)", this);
        }catch(final MdsException e){
            return null;
        }
    }

    public final NODE<T> updateSegment(final Descriptor<?> start, final Descriptor<?> end, final Descriptor<?> dim, final int idx) throws MdsException {
        this.tree.updateSegment(this.getNidNumber(), start, end, dim, idx);
        return this;
    }
}

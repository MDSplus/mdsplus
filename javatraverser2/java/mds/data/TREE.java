package mds.data;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import mds.Mds;
import mds.Mds.Request;
import mds.MdsEvent;
import mds.MdsException;
import mds.MdsListener;
import mds.TreeShr;
import mds.TreeShr.DescriptorStatus;
import mds.TreeShr.IntegerStatus;
import mds.TreeShr.NodeRefStatus;
import mds.TreeShr.SegmentInfo;
import mds.TreeShr.StringStatus;
import mds.TreeShr.TagRef;
import mds.TreeShr.TagRefStatus;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;
import mds.data.descriptor_a.Int32Array;
import mds.data.descriptor_a.NidArray;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_s.CString;
import mds.data.descriptor_s.NODE;
import mds.data.descriptor_s.NODE.Flags;
import mds.data.descriptor_s.Nid;
import mds.data.descriptor_s.Path;
import mds.data.descriptor_s.Pointer;
import mds.mdsip.MdsIp;

public final class TREE implements MdsListener, CTX{
    public final static class NodeInfo{
        public static final Request<List> getRequest(final NODE<?> node) {
            return new Request<List>(List.class, "_n=GETNCI($,'NID_NUMBER');" + NodeInfo.request(), node);
        }

        private static final String request() {
            return "List(*,GETNCI(_n,'USAGE'),_n,GETNCI(_n,'GET_FLAGS'),GETNCI(_n,'STATUS'),GETNCI(_n,'NUMBER_OF_CHILDREN')+GETNCI(_n,'NUMBER_OF_MEMBERS'),TRIM(GETNCI(_n,'NODE_NAME')),GETNCI(_n,'MINPATH'),GETNCI(_n,'PATH'),GETNCI(_n,'FULLPATH'))";
        }

        private static final String requests() {
            return "_m=$;_l=LIST();FOR(_i=0;_i<SIZE(_m);_i++)(_n=_m[_i];List(_l," + NodeInfo.request() + "););_l";
        }
        public final byte   usage;
        public final int    nid_number, get_flags, status, num_descendants;
        public final String node_name, minpath, path, fullpath;

        private NodeInfo(final Descriptor<?>[] dscs){
            this.usage = dscs[0].toByte();
            this.nid_number = dscs[1].toInt();
            this.get_flags = dscs[2].toInt();
            this.status = dscs[3].toInt();
            this.num_descendants = dscs[4].toInt();
            this.node_name = dscs[5].toString().trim();
            this.minpath = dscs[6].toString();
            this.path = dscs[7].toString();
            this.fullpath = dscs[8].toString();
        }

        private NodeInfo(final List list){
            this(list.toArray());
        }

        private NodeInfo(final NODE<?> node, final Mds mds, final Pointer ctx) throws MdsException{
            this(mds.getDescriptor(ctx, NodeInfo.getRequest(node)));
        }
    }
    public final static class RecordInfo{
        private static final String request = "_n=GETNCI($,'NID_NUMBER');List(*,GETNCI(_n,'DTYPE'),GETNCI(_n,'CLASS'),_n,GETNCI(_n,'STATUS'),GETNCI(_n,'GET_FLAGS'),GETNCI(_n,'LENGTH'),GETNCI(_n,'RLENGTH'),(_a=-1;TreeShr->TreeGetNumSegments(val(_n),ref(_a));_a;),DATE_TIME(GETNCI(_n,'TIME_INSERTED')))";

        public static final Request<List> getRequest(final NODE<?> node) {
            return new Request<List>(List.class, RecordInfo.request, node);
        }
        public final byte   dtype, dclass;
        public final int    nid_number, status, get_flags, length, rlength, num_segments;
        public final String time_inserted;

        private RecordInfo(final NODE<?> node, final Mds mds, final Pointer ctx) throws MdsException{
            final List list = mds.getDescriptor(ctx, RecordInfo.getRequest(node));
            this.dtype = list.get(0).toByte();
            this.dclass = list.get(1).toByte();
            this.nid_number = list.get(2).toInt();
            this.status = list.get(3).toInt();
            this.get_flags = list.get(4).toInt();
            this.length = list.get(5).toInt();
            this.rlength = list.get(6).toInt();
            this.num_segments = list.get(7).toInt();
            this.time_inserted = list.get(8).toString();
        }

        @Override
        public final String toString() {
            final StringBuilder sb = new StringBuilder(255).append('(');
            sb.append("dtype:").append(this.dtype).append(", ");
            sb.append("dclass:").append(this.dclass).append(", ");
            sb.append("nid_number:").append(this.nid_number).append(", ");
            sb.append("status:").append(this.status).append(", ");
            sb.append("get_flags:").append(this.get_flags).append(", ");
            sb.append("length:").append(this.length).append(", ");
            sb.append("rlength:").append(this.rlength).append(", ");
            sb.append("num_segments:").append(this.num_segments).append(", ");
            sb.append("time_inserted:").append(this.time_inserted).append(')');
            return sb.toString();
        }
    }
    @SuppressWarnings("serial")
    public static final class TagList extends HashMap<String, Nid>{
        private final String root;

        public TagList(final int capacity, final String expt){
            super(capacity);
            this.root = new StringBuilder(expt.length() + 3).append("\\").append(expt).append("::").toString();
        }

        @Override
        public final String toString() {
            final StringBuilder str = new StringBuilder(this.size() * 64);
            for(final Map.Entry<String, Nid> entry : this.entrySet())
                str.append(entry.getKey().replace(this.root, "\\")).append("  =>  ").append(entry.getValue()).append("\n");
            return str.toString();
        }
    }
    public static final int    EDITABLE                = 2;
    public static final int    NEW                     = 3;
    public static final int    NORMAL                  = 1;
    public static final int    READONLY                = 0;
    private static TREE        active                  = null;
    public static final String NCI_BROTHER             = "BROTHER";
    public static final String NCI_CHILD               = "CHILD";
    public static final String NCI_CHILDREN_NIDS       = "CHILDREN_NIDS";
    public static final String NCI_CLASS               = "CLASS";
    public static final String NCI_CLASS_STR           = "CLASS_STR";
    public static final String NCI_CONGLOMERATE_ELT    = "CONGLOMERATE_ELT";
    public static final String NCI_CONGLOMERATE_NIDS   = "CONGLOMERATE_NIDS";
    public static final String NCI_DATA_IN_NCI         = "DATA_IN_NCI";
    public static final String NCI_DEPTH               = "DEPTH";
    public static final String NCI_DTYPE               = "DTYPE";
    public static final String NCI_DTYPE_STR           = "DTYPE_STR";
    public static final String NCI_ERROR_ON_PUT        = "ERROR_ON_PUT";
    public static final String NCI_FULLPATH            = "FULLPATH";
    public static final String NCI_GET_FLAGS           = "GET_FLAGS";
    public static final String NCI_IS_CHILD            = "IS_CHILD";
    public static final String NCI_IS_MEMBER           = "IS_MEMBER";
    public static final String NCI_LENGTH              = "LENGTH";
    public static final String NCI_MEMBER              = "MEMBER";
    public static final String NCI_MEMBER_NIDS         = "MEMBER_NIDS";
    public static final String NCI_MINPATH             = "MINPATH";
    public static final String NCI_NID_NUMBER          = "NID_NUMBER";
    public static final String NCI_NODE_NAME           = "NODE_NAME";
    public static final String NCI_NUMBER_OF_CHILDREN  = "NUMBER_OF_CHILDREN";
    public static final String NCI_NUMBER_OF_ELTS      = "NUMBER_OF_ELTS";
    public static final String NCI_NUMBER_OF_MEMBERS   = "NUMBER_OF_MEMBERS";
    public static final String NCI_ORIGINAL_PART_NAME  = "ORIGINAL_PART_NAME";
    public static final String NCI_OWNER_ID            = "OWNER_ID";
    public static final String NCI_PARENT              = "PARENT";
    public static final String NCI_PARENT_RELATIONSHIP = "PARENT_RELATIONSHIP";
    public static final String NCI_PARENT_TREE         = "PARENT_TREE";
    public static final String NCI_PATH                = "PATH";
    public static final String NCI_RECORD              = "RECORD";
    public static final String NCI_RFA                 = "RFA";
    public static final String NCI_RLENGTH             = "RLENGTH";
    public static final String NCI_STATUS              = "STATUS";
    public static final String NCI_TIME_INSERTED       = "TIME_INSERTED";
    public static final String NCI_TIME_INSERTED_STR   = "TIME_INSERTED_STR";
    public static final String NCI_USAGE               = "USAGE";
    public static final String NCI_USAGE_STR           = "USAGE_STR";
    public static final String NCI_VERSION             = "VERSION";

    public static final TREE getActiveTree() {
        return TREE.active;
    }

    public static final <T extends Descriptor<?>> Request<T> getNciRequest(final Class<T> cls, final NODE<?> node, final String name) {
        if(TREE.NCI_TIME_INSERTED_STR.equals(name)) return new Request<T>(cls, "DATE_TIME(GETNCI($,'TIME_INSERTED'))", node);
        return new Request<T>(cls, "GETNCI($,$)", node, new CString(name));
    }
    private Pointer      oldctx = null;
    private Nid          def_nid;
    public final Pointer ctx    = Pointer.NULL();
    public final int     shot;
    public final Mds     mds;
    public final String  expt;
    private int          mode;
    public final TreeShr treeshr;
    public boolean       opened = false;
    private boolean      ready;

    public TREE(final Mds mds, final String expt, final int shot){
        this(mds, expt, shot, TREE.READONLY);
    }

    public TREE(final Mds mds, final String expt, int shot, final int mode){
        this.mds = mds;
        this.ready = mds.isReady() == null;
        this.treeshr = new TreeShr(mds);
        this.expt = expt.toUpperCase();
        try{
            if(shot == 0) shot = this.getCurrentShot();
        }catch(final MdsException e){/**/}
        this.shot = shot;
        this.mode = mode;
        this.def_nid = this.getTop();
    }

    private final Descriptor<?> _getNci(final int nid, final String name) throws MdsException {
        if(TREE.NCI_TIME_INSERTED_STR.equals(name)) return this.mds.getDescriptor(this.ctx, new StringBuilder(48).append("DATE_TIME(GETNCI(").append(nid).append(",'TIME_INSERTED'))").toString());
        return this.mds.getDescriptor(this.ctx, new StringBuilder(24).append("GETNCI(").append(nid).append(",$)").toString(), new CString(name));
    }

    private final TREE _open() throws MdsException {
        final int status;
        switch(this.mode){
            case TREE.NEW:
                this.mode = TREE.EDITABLE;
                status = this.treeshr.treeOpenNew(this.ctx, this.expt, this.shot);
                break;
            case TREE.EDITABLE:
                status = this.treeshr.treeOpenEdit(this.ctx, this.expt, this.shot);
                break;
            default:
                this.mode = TREE.READONLY;
                //$FALL-THROUGH$
            case TREE.READONLY:
            case TREE.NORMAL:
                status = this.treeshr.treeOpen(this.ctx, this.expt, this.shot, this.isReadonly());
        }
        MdsException.handleStatus(status);
        this.updateListener(true);
        return this;
    }

    public final Nid addConglom(final NODE<?> node, final String name, final String model) throws MdsException {
        synchronized(this.mds){
            final Nid def = this.getDefaultNid();
            node.setDefault();
            final Nid nid = this.addConglom(name, model);
            def.setDefault();
            return nid;
        }
    }

    public final Nid addConglom(final String path, final String model) throws MdsException {
        final IntegerStatus res = this.setActive().treeshr.treeAddConglom(this.ctx, path, model);
        MdsException.handleStatus(res.status);
        return new Nid(res.data, this);
    }

    public final Nid addNode(final String path, final byte usage) throws MdsException {
        synchronized(this.mds){
            final IntegerStatus res = this.setActive().treeshr.treeAddNode(this.ctx, path, usage);
            MdsException.handleStatus(res.status);
            final Nid nid = new Nid(res.data, this);
            if(usage == NODE.USAGE_SUBTREE) nid.setSubtree();
            return nid;
        }
    }

    public final TREE addTag(final int nid, final String tag) throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treeAddTag(this.ctx, nid, tag));
        return this;
    }

    public final TREE clearFlags(final int nid, final int flags) throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treeSetNciItm(this.ctx, nid, false, flags & 0x7FFFFFFC));
        return this;
    }

    public final TREE clearTags(final int nid) throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treeRemoveNodesTags(this.ctx, nid));
        return this;
    }

    public final TREE close() throws MdsException {
        MdsException.handleStatus(this.treeshr.treeClose(this.ctx, this.expt, this.shot));
        this.updateListener(false);
        return this;
    }

    public final TREE createTreeFiles(final int newshot) throws MdsException {
        MdsException.handleStatus(this.treeshr.treeCreateTreeFiles(null, this.expt, newshot, this.shot));
        return new TREE(this.mds, this.expt, newshot);
    }

    public final TREE deleteNodeExecute() throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treeDeleteNodeExecute(this.ctx));
        return this;
    }

    public final Nid[] deleteNodeGetNids() throws MdsException {
        synchronized(this.mds){
            final ArrayList<Nid> nids = new ArrayList<Nid>(256);
            int last = 0;
            for(;;){
                final IntegerStatus res = this.setActive().treeshr.treeDeleteNodeGetNid(this.ctx, last);
                if(res.status == MdsException.TreeNMN) break;
                MdsException.handleStatus(res.status);
                nids.add(new Nid(last = res.data, this));
            }
            return nids.toArray(new Nid[0]);
        }
    }

    public final int deleteNodeInitialize(final int nid) throws MdsException {
        final IntegerStatus res = this.setActive().treeshr.treeDeleteNodeInitialize(this.ctx, nid);
        MdsException.handleStatus(res.status);
        return res.data;
    }

    public final TREE doAction(final int nid) throws MdsException {
        MdsException.handleStatus(this.setActive().mds.getInteger(this.ctx, new StringBuilder(37).append("TCL('do '//GetNci(").append(nid).append(",'PATH'))").toString()));
        return this;
    }

    public final Descriptor<?> doDeviceMethod(final int nid, final String method, final Descriptor<?>... args) throws MdsException {
        final DescriptorStatus res = this.treeshr.treeDoMethod(this.ctx, nid, method, args);
        MdsException.handleStatus(res.status);
        return res.data;
    }

    @Override
    protected final void finalize() {
        if(this.opened) try{
            this.quitTree();
        }catch(final MdsException e){
            this.mds.removeMdsListener(this);
        }
    }

    public Nid[] findNodesWild(final byte usage) throws MdsException {
        return this.findNodesWild("***", usage);
    }

    public final Nid[] findNodesWild(final String searchstr, final byte usage) throws MdsException {
        try{
            return Nid.getArrayOfNids(this.mds.getIntegerArray(this.ctx, "GETNCI('***','NID_NUMBER',$)", new CString(NODE.getUsageStr(usage))), this);
        }catch(final MdsException e){
            if(e.getStatus() == MdsException.TreeNNF) return new Nid[0];
            throw e;
        }
    }

    public final Nid[] findNodesWildLL(final int usage) throws MdsException {
        return this.findNodesWildLL("***", 1 << usage);
    }

    public final Nid[] findNodesWildLL(final String searchstr, final int usage_mask) throws MdsException {
        final ArrayList<Nid> nids = new ArrayList<Nid>(1024);
        synchronized(this.mds){
            this.holdDbid();
            try{
                NodeRefStatus ref = NodeRefStatus.init;
                while((ref = this.treeshr.treeFindNodeWild(null, searchstr, usage_mask, ref)).ok())
                    nids.add(new Nid(ref.data, this));
            }finally{
                this.releaseDbid();
            }
        }
        return nids.toArray(new Nid[0]);
    }

    public final TagRef findNodeTags(final int nid, final TagRef ref) throws MdsException {
        return this.setActive().treeshr.treeFindNodeTags(this.ctx, nid, ref);
    }

    public final NodeRefStatus findNodeWild(final String searchstr, final int usage_mask, final NodeRefStatus ref) throws MdsException {
        return this.setActive().treeshr.treeFindNodeWild(this.ctx, searchstr, usage_mask, ref);
    }

    public final TagList findTagsWildLL() throws MdsException {
        return this.findTagsWildLL("***", 255);
    }

    public final TagList findTagsWildLL(final String searchstr, final int max) throws MdsException {
        final TagList taglist = new TagList(max, this.expt);
        synchronized(this.mds){
            this.holdDbid();
            try{
                TagRefStatus tag = TagRefStatus.init;
                while(taglist.size() < max && (tag = this.treeshr.treeFindTagWild(null, searchstr, tag)).ok())
                    taglist.put(tag.data, new Nid(tag.nid, this));
            }finally{
                this.releaseDbid();
            }
        }
        return taglist;
    }

    public final TagRefStatus findTagWild(final String searchstr, final TagRefStatus ref) throws MdsException {
        return this.setActive().treeshr.treeFindTagWild(this.ctx, searchstr, ref);
    }

    public final Pointer getCtx() throws MdsException {
        return this.setActive().treeshr.treeDbid(this.ctx);
    }

    public final int getCurrentShot() throws MdsException {
        return this.setActive().treeshr.treeGetCurrentShotId(null, this.expt);
    }

    @Override
    public final Pointer getDbid() {
        return this.ctx;
    }

    public final Nid getDefaultC() {
        return this.def_nid;
    }

    public final Nid getDefaultNid() throws MdsException {
        final IntegerStatus res = this.setActive().treeshr.treeGetDefaultNid(this.ctx);
        MdsException.handleStatus(res.status);
        return this.def_nid = new Nid(res.data, this);
    }

    public final String getFileName() throws MdsException {
        return this.getFileName(this.expt);
    }

    public final String getFileName(final String subtree) throws MdsException {
        final StringStatus ans = this.setActive().treeshr.treeFileName(this.ctx, subtree, this.shot);
        if(ans.status == MdsException.TreeFOPENR) throw new MdsException(ans.status);
        MdsException.handleStatus(ans.status);
        return ans.data;
    }

    public final int getMode() {
        return this.mode;
    }

    public final Descriptor<?> getNci(final int nid, final String name) throws MdsException {
        return this.setActive()._getNci(nid, name);
    }

    public final Descriptor<?> getNci(final NODE<?> node, final String name) throws MdsException {
        return this.setActive().mds.getDescriptor(this.ctx, TREE.getNciRequest(Descriptor.class, node, name));
    }

    public final Nid getNciBrother(final int nid) throws MdsException {
        return (Nid)this.getNci(nid, TREE.NCI_BROTHER);
    }

    public final Nid getNciChild(final int nid) throws MdsException {
        return (Nid)this.getNci(nid, TREE.NCI_CHILD);
    }

    public final NidArray getNciChildrenNids(final int nid) throws MdsException {
        final StringBuilder cmd = new StringBuilder(128).append("IF(GetNci(").append(nid);
        cmd.append(",'NUMBER_OF_CHILDREN')>0)GetNci(").append(nid).append(",'CHILDREN_NIDS')");
        final NidArray result = this.setActive().mds.getDescriptor(this.ctx, cmd.toString(), NidArray.class);
        if(result == null) return new NidArray();
        return result;
    }

    public final NidArray getNciChildrenNids(final NODE<?> node) throws MdsException {
        final NidArray result = this.setActive().mds.getDescriptor(this.ctx, "IF(GetNci(_a=as_is($),'NUMBER_OF_CHILDREN')>0)GetNci(_a,'CHILDREN_NIDS')", NidArray.class, node);
        if(result == null) return new NidArray();
        return result;
    }

    public final byte getNciClass(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_CLASS).toByte();
    }

    public final String getNciClassStr(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_CLASS_STR).toString();
    }

    public final short getNciConglomerateElt(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_CONGLOMERATE_ELT).toShort();
    }

    public final NidArray getNciConglomerateNids(final int nid) throws MdsException {
        return (NidArray)this.getNci(nid, TREE.NCI_CONGLOMERATE_NIDS);
    }

    public final int getNciDataInNci(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_DATA_IN_NCI).toInt();
    }

    public final int getNciDepth(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_DEPTH).toInt();
    }

    public final byte getNciDType(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_DTYPE).toByte();
    }

    public final String getNciDTypeStr(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_DTYPE_STR).toString();
    }

    public final int getNciErrorOnPut(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_ERROR_ON_PUT).toInt();
    }

    public final int getNciFlags(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_GET_FLAGS).toInt();
    }

    public final String getNciFullPath(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_FULLPATH).toString();
    }

    public final boolean getNciIsChild(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_IS_CHILD).toByte() != 0;
    }

    public final boolean getNciIsMember(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_IS_MEMBER).toByte() != 0;
    }

    public final int getNciLength(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_LENGTH).toInt();
    }

    public final Nid getNciMember(final int nid) throws MdsException {
        return (Nid)this.getNci(nid, TREE.NCI_MEMBER);
    }

    public final NidArray getNciMemberNids(final int nid) throws MdsException {
        final StringBuilder cmd = new StringBuilder(128).append("IF(GetNci(").append(nid);
        cmd.append(",'NUMBER_OF_MEMBERS')>0)GetNci(").append(nid).append(",'MEMBER_NIDS')");
        final NidArray result = this.setActive().mds.getDescriptor(this.ctx, cmd.toString(), NidArray.class);
        if(result == null) return new NidArray();
        return result;
    }

    public final NidArray getNciMemberNids(final NODE<?> node) throws MdsException {
        final NidArray result = this.setActive().mds.getDescriptor(this.ctx, "IF(GetNci(_a=as_is($),'NUMBER_OF_MEMBERS')>0)GetNci(_a,'MEMBER_NIDS')", NidArray.class, node);
        if(result == null) return new NidArray();
        return result;
    }

    public final String getNciMinPath(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_MINPATH).toString();
    }

    public final int getNciNidNumber(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_NID_NUMBER).toInt();
    }

    public final String getNciNodeName(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_NODE_NAME).toString().trim();
    }

    public final int getNciNumberOfChildren(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_NUMBER_OF_CHILDREN).toInt();
    }

    public final int getNciNumberOfElts(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_NUMBER_OF_ELTS).toInt();
    }

    public final int getNciNumberOfMembers(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_NUMBER_OF_MEMBERS).toInt();
    }

    public final String getNciOriginalPartName(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_ORIGINAL_PART_NAME).toString().trim();
    }

    public final int getNciOwnerId(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_OWNER_ID).toInt();
    }

    public final Nid getNciParent(final int nid) throws MdsException {
        return (Nid)this.getNci(nid, TREE.NCI_PARENT);
    }

    public final int getNciParentRelationship(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_PARENT_RELATIONSHIP).toInt();
    }

    public final String getNciParentTree(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_PARENT_TREE).toString();
    }

    public final String getNciPath(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_PATH).toString();
    }

    public final Descriptor<?> getNciRecord(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_RECORD);
    }

    public final long getNciRfa(final int nid) throws MdsException {
        return this.getNci(nid, "RFA").toLong();
    }

    public final int getNciRLength(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_RLENGTH).toInt();
    }

    public final int getNciStatus(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_STATUS).toInt();
    }

    public final long getNciTimeInserted(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_TIME_INSERTED).toLong();
    }

    public final long getNciTimeInserted(final NODE<?> node) throws MdsException {
        return this.getNci(node, TREE.NCI_TIME_INSERTED).toLong();
    }

    public final String getNciTimeInsertedStr(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_TIME_INSERTED_STR).toString();
    }

    public final String getNciTimeInsertedStr(final NODE<?> node) throws MdsException {
        return this.getNci(node, TREE.NCI_TIME_INSERTED_STR).toString();
    }

    public final byte getNciUsage(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_USAGE).toByte();
    }

    public final String getNciUsageStr(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_USAGE_STR).toString();
    }

    public final int getNciVersion(final int nid) throws MdsException {
        return this.getNci(nid, TREE.NCI_VERSION).toInt();
    }

    public final Nid getNode(final String path) throws MdsException {
        return new Path(path, this).toNid();
    }

    public final NodeInfo getNodeInfo(final NODE<?> node) throws MdsException {
        return new NodeInfo(node, this.mds, this.ctx);
    }

    public final NodeInfo[] getNodeInfos(final Nid[] nids) throws MdsException {
        final NodeInfo[] infos = new NodeInfo[nids.length];
        if(nids.length == 0) return infos;
        final List list = this.mds.getDescriptor(this.ctx, NodeInfo.requests(), List.class, new Int32Array(nids));
        for(int i = 0; i < list.getLength(); i++)
            infos[i] = new NodeInfo((List)list.get(i));
        return infos;
    }

    public final int getNumSegments(final int nid) throws MdsException {
        final IntegerStatus res = this.setActive().treeshr.treeGetNumSegments(this.ctx, nid);
        MdsException.handleStatus(res.status);
        return res.data;
    }

    public final Descriptor<?> getRecord(final int nid) throws MdsException {
        return this.setActive().treeshr.treeGetRecord(this.ctx, nid);
    }

    public final RecordInfo getRecordInfo(final NODE<?> node) throws MdsException {
        return new RecordInfo(node, this.mds, this.ctx);
    }

    public final List getSegment(final int nid, final int idx) throws MdsException {
        return this.setActive().treeshr.treeGetSegment(this.ctx, nid, idx);
    }

    public final SegmentInfo getSegmentInfo(final int nid, final int idx) throws MdsException {
        return this.setActive().treeshr.treeGetSegmentInfo(this.ctx, nid, idx);
    }

    public final List getSegmentLimits(final int nid, final int idx) throws MdsException {
        final DescriptorStatus dscs = this.setActive().treeshr.treeGetSegmentLimits(this.ctx, nid, idx);
        MdsException.handleStatus(dscs.status);
        return (List)dscs.data;
    }

    public final Descriptor<?> getSegmentScale(final int nid) throws MdsException {
        return this.setActive().treeshr.treeGetSegmentScale(this.ctx, nid);
    }

    public final List getSegmentScale(final int nid, final int idx) throws MdsException {
        return this.setActive().treeshr.treeGetSegment(this.ctx, nid, idx);
    }

    public List getSegmentTimes(final int nid) throws MdsException {
        final List list = this.setActive().treeshr.treeGetSegmentTimesXd(this.ctx, nid);
        MdsException.handleStatus(list.toInt(1));
        return (List)list.get(0);
    }

    public final String[] getTags(final int nid) throws MdsException {
        final StringBuilder cmd = new StringBuilder(170).append("_a=0Q;_i=0;_l=LIST();");
        cmd.append("WHILE((_i<1024)&&KIND(_t=TreeShr->TreeFindNodeTags:T(val(");
        cmd.append(nid).append("),ref(_a)))>0)(_i++;_l=List(_l,_t);MdsShr->StrFree1Dx(ref(_t)););_l");
        final List list = this.mds.getDescriptor(this.ctx, cmd.toString(), List.class);
        return list.toStringArray();
    }

    public final String[] getTagsLL(final int nid) throws MdsException {
        final ArrayList<String> tags = new ArrayList<String>(255);
        synchronized(this.mds){
            this.holdDbid();
            try{
                TagRef tag = TagRef.init;
                while(tags.size() < 255 && (tag = this.treeshr.treeFindNodeTags(this.ctx, nid, tag)).ok())
                    tags.add(tag.data);
            }finally{
                this.releaseDbid();
            }
        }
        return tags.toArray(new String[0]);
    }

    public final Nid getTop() {
        return new Nid(0, this);
    }

    public final Descriptor<?> getXNci(final int nid, final String name) throws MdsException {
        final DescriptorStatus res = this.setActive().treeshr.treeGetXNci(this.ctx, nid, name);
        MdsException.handleStatus(res.status);
        return res.data;
    }

    public final void holdDbid() throws MdsException {
        if(this.oldctx != null) return;
        this.oldctx = this.treeshr.treeSwitchDbid(null, this.ctx);
    }

    public final boolean isEditable() {
        return this.mode == TREE.EDITABLE;
    }

    public final boolean isNidReference(final int nid) throws MdsException {
        return new Flags(this.getNciFlags(nid)).isNidReference();
    }

    public final boolean isOpen() {
        return(this.ctx != null && !this.ctx.isNull() && this.opened);
    }

    public final boolean isPathReference(final int nid) throws MdsException {
        return new Flags(this.getNciFlags(nid)).isPathReference();
    }

    public final boolean isReadonly() {
        return this.mode == TREE.READONLY;
    }

    public final boolean isSegmented(final int nid) throws MdsException {
        synchronized(this.mds){
            if(new Flags(this.getNciFlags(nid)).isSegmented()) return true; // cannot be sure due to issue in winter 2015/2016
            return this.getNumSegments(nid) > 0;
        }
    }

    public final TREE makeSegment(final int nid, final Descriptor_A<?> dimension, final Descriptor_A<?> values) throws MdsException {
        return this.makeSegment(nid, dimension.getScalar(0), dimension.getScalar(dimension.getLength() - 1), dimension, values, -1, dimension.getLength());
    }

    public final TREE makeSegment(final int nid, final Descriptor<?> start, final Descriptor<?> end, final Descriptor<?> dimension, final Descriptor_A<?> values, final int idx, final int rows_filled) throws MdsException {
        this.treeshr.treeMakeSegment(this.ctx, nid, start, end, dimension, values, idx, rows_filled);
        return this;
    }

    public final TREE open() throws MdsException {
        return this.setActive()._open();
    }

    public final TREE open(final int new_mode) throws MdsException {
        this.mode = new_mode;
        return this.open();
    }

    @Override
    public void processMdsEvent(final MdsEvent e) {
        switch(e.getID()){
            case MdsEvent.IDLE:
            case MdsEvent.TRANSFER:
                break;
            case MdsEvent.HAVE_CONTEXT:
                this.ready = true;
                break;
            case MdsEvent.LOST_CONTEXT:
                this.ctx.setAddress(0);
                this.ready = false;
                break;
            default:
                System.out.println(e.getID() + e.getInfo());
        }
    }

    public final TREE putRecord(final int nid, final Descriptor<?> data) throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treePutRecord(this.ctx, nid, data));
        return this;
    }

    public final TREE putRow(final int nid, final long time, final Descriptor_A<?> data) throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treePutRow(this.ctx, nid, 1 << 20, time, data));
        return this;
    }

    public final TREE putSegment(final int nid, final int idx, final Descriptor_A<?> data) throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treePutSegment(this.ctx, nid, idx, data));
        return this;
    }

    public final TREE putTimestampedSegment(final int nid, final long timestamp, final Descriptor_A<?> data) throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treePutTimestampedSegment(this.ctx, nid, timestamp, data));
        return this;
    }

    public final TREE quitTree() throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treeQuitTree(this.ctx, this.expt, this.shot));
        this.updateListener(false);
        return this;
    }

    public final void releaseDbid() throws MdsException {
        if(this.oldctx == null) return;
        this.treeshr.treeSwitchDbid(null, this.oldctx);
        this.oldctx = null;
    }

    public final TREE setActive() throws MdsException {
        if(this.opened && !this.isOpen() && this.ready){
            this._open();
            this.def_nid.setDefault();
        }
        return TREE.active = this;
    }

    public final TREE setCurrentShot() throws MdsException {
        return this.setCurrentShot(this.shot);
    }

    public final TREE setCurrentShot(final int shot) throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treeSetCurrentShotId(null, this.expt, shot));
        return this;
    }

    public final TREE setDefault(final int nid) throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treeSetDefaultNid(this.ctx, nid));
        this.def_nid = new Nid(nid, this);
        return this;
    }

    public final TREE setFlags(final int nid, final int flags) throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treeSetNciItm(this.ctx, nid, true, flags & 0x7FFFFFFC));
        return this;
    }

    public final TREE setNoSubtree(final int nid) throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treeSetNoSubtree(this.ctx, nid));
        return this;
    }

    public final TREE setPath(final int nid, final String path) throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treeRenameNode(this.ctx, nid, path));
        return this;
    }

    public final int setSegmentScale(final int nid, final Descriptor<?> scale) throws MdsException {
        return this.setActive().treeshr.treeSetSegmentScale(this.ctx, nid, scale);
    }

    public final TREE setSubtree(final int nid) throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treeSetSubtree(this.ctx, nid));
        return this;
    }

    public final TREE setTags(final int nid, final String... tags) throws MdsException {
        synchronized(this.mds){
            this.clearTags(nid);
            if(tags == null) return this;
            for(int i = tags.length; i-- > 0;)
                this.addTag(nid, tags[i]);
        }
        return this;
    }

    @Override
    public final String toString() {
        final StringBuilder sb = new StringBuilder("Tree(\"").append(this.expt).append("\", ");
        if(this.shot == -1) sb.append("model");
        else sb.append(this.shot);
        if(this.mode == TREE.EDITABLE) sb.append(", edit");
        else if(this.mode == TREE.READONLY) sb.append(", readonly");
        return sb.append(')').toString();
    }

    public final TREE turnOff(final int nid) throws MdsException {
        final int status = this.setActive().treeshr.treeTurnOff(this.ctx, nid);
        if(status == MdsException.TreeLOCK_FAILURE) return this;// ignore: it changes the state
        MdsException.handleStatus(status);
        return this;
    }

    public final TREE turnOn(final int nid) throws MdsException {
        final int status = this.setActive().treeshr.treeTurnOn(this.ctx, nid);
        if(status == MdsException.TreeLOCK_FAILURE) return this;// ignore: it changes the state
        MdsException.handleStatus(status);
        return this;
    }

    private final void updateListener(final boolean opened_in) {
        this.opened = opened_in;
        if(this.opened) this.mds.addMdsListener(this);
        else this.mds.removeMdsListener(this);
    }

    public final TREE updateSegment(final int nid, final Descriptor<?> start, final Descriptor<?> end, final Descriptor<?> dim, final int idx) throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treeUpdateSegment(this.ctx, nid, start, end, dim, idx));
        return this;
    }

    public final TREE verify() throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treeVerify(this.ctx));
        return this;
    }

    public final TREE withPrivateConnection() {
        if(!(this.mds instanceof MdsIp)) return null;
        return new TREE(new MdsIp(((MdsIp)this.mds).getProvider()), this.expt, this.shot);
    }

    public final TREE writeTree() throws MdsException {
        MdsException.handleStatus(this.setActive().treeshr.treeWriteTree(this.ctx, this.expt, this.shot));
        return this;
    }
}

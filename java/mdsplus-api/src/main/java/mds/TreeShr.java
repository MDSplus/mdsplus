package mds;

import mds.Mds.Request;
import mds.data.CTX;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;
import mds.data.descriptor_a.Int32Array;
import mds.data.descriptor_a.Int64Array;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_r.Signal;
import mds.data.descriptor_s.*;

public class TreeShr extends MdsShr
{
	/** to set maximum number of trees in context */
	public static final int DBI_MAX_OPEN = 7;
	/** to 1: enable, 0: disable versioning in model files */
	public static final int DBI_VERSIONS_IN_MODEL = 10;
	/** to 1: enable, 0: disable versioning in pulse files */
	public static final int DBI_VERSIONS_IN_PULSE = 11;

	public static class DescriptorStatus
	{
		private final Descriptor<?> data;
		private final int status;

		public DescriptorStatus(final Descriptor<?> data, final int status)
		{
			this.data = data;
			this.status = status;
		}

		public DescriptorStatus(final List list)
		{
			this(list.get(0), list.toInt(1));
		}

		public final Descriptor<?> getData() throws MdsException
		{
			MdsException.handleStatus(this.status);
			return this.data;
		}

		@Override
		public final String toString()
		{
			return new StringBuilder().append(this.data).append(";").append(this.status).toString();
		}
	}

	public static class IntegerStatus
	{
		private final int data;
		public final int status;

		public IntegerStatus(final int data, final int status)
		{
			this.data = data;
			this.status = status;
		}

		public IntegerStatus(final int[] datastatus)
		{
			this(datastatus[0], datastatus[1]);
		}

		public IntegerStatus(final Int32Array descriptor)
		{
			this(descriptor.toArray());
		}

		public int getData() throws MdsException
		{
			MdsException.handleStatus(this.status);
			return this.data;
		}

		@Override
		public final String toString()
		{
			final StringBuilder str = new StringBuilder(32).append(this.data).append(';');
			return str.append(this.status).toString();
		}
	}

	public static class NodeRefStatus
	{
		public static final NodeRefStatus init = new NodeRefStatus(-1, 0, 0);
		public final int data;
		public final long ref;
		public final int status;

		public NodeRefStatus(final int data, final long ref, final int status)
		{
			this.data = data;
			this.ref = ref;
			this.status = status;
		}

		public NodeRefStatus(final List list)
		{
			this(list.toInt(0), list.toLong(1), list.toInt(2));
		}

		public final boolean ok()
		{
			return (this.status & 1) == 1;
		}

		@Override
		public final String toString()
		{
			final StringBuilder str = new StringBuilder(64).append(this.data).append(';');
			str.append(this.ref).append("l;");
			return str.append(this.status).toString();
		}
	}

	public static class SegmentInfo
	{
		public DTYPE dtype;
		public byte dimct;
		public int[] dims;
		public int next_row;
		public final int status;

		public SegmentInfo(final DTYPE dtype, final byte dimct, final int[] dims, final int next_row)
		{
			this(dtype, dimct, dims, next_row, -1);
		}

		public SegmentInfo(final DTYPE dtype, final byte dimct, final int[] dims, final int next_row, final int status)
		{
			this.dtype = dtype;
			this.dimct = dimct;
			this.dims = dims;
			this.next_row = next_row;
			this.status = status;
		}

		public SegmentInfo(final int[] array)
		{
			this.dtype = DTYPE.get((byte) array[0]);
			this.dimct = (byte) array[1];
			this.dims = new int[8];
			System.arraycopy(array, 2, this.dims, 0, 8);
			this.next_row = array[10];
			this.status = array.length > 11 ? array[11] : -1;
		}

		public SegmentInfo(final Int32Array descriptor)
		{
			this(descriptor.toArray());
		}

		public SegmentInfo(final List list)
		{
			this(DTYPE.get(list.toByte(0)), list.toByte(1), list.toIntArray(2), list.toInt(3), list.toInt(4));
		}

		@Override
		public final String toString()
		{
			final StringBuilder str = new StringBuilder(128).append(this.dtype).append(';');
			str.append(this.dimct).append(';');
			for (final int dim : this.dims)
				str.append(dim).append(',');
			str.append(this.next_row).append(';');
			return str.append(this.status).toString();
		}
	}

	public static class SignalStatus
	{
		public final Signal data;
		public final int status;

		public SignalStatus(final List list)
		{
			this((Signal) list.get(0), list.toInt(1));
		}

		public SignalStatus(final Signal data, final int status)
		{
			this.data = data;
			this.status = status;
		}

		@Override
		public final String toString()
		{
			return new StringBuilder().append(this.data).append(";").append(this.status).toString();
		}
	}

	public static class TagRef
	{
		public static final TagRef init = new TagRef(null, 0);
		public final String data;
		public final long ref;

		public TagRef(final List list)
		{
			this(list.get(0).toString(), list.get(1).toLong());
		}

		public TagRef(final String data, final long ref)
		{
			this.data = data;
			this.ref = ref;
		}

		public final boolean ok()
		{
			return this.ref != 0;
		}

		@Override
		public final String toString()
		{
			return new StringBuilder(32).append(this.data).append(";").append(this.ref).append('l').toString();
		}
	}

	public static class TagRefStatus
	{
		public static final TagRefStatus init = new TagRefStatus(null, -1, 0, 0);
		public final String data;
		public final int nid;
		public final long ref;
		public final int status;

		public TagRefStatus(final List list)
		{
			this(list.toString(0), list.toInt(1), list.toLong(2), list.toInt(3));
		}

		public TagRefStatus(final String data, final int nid, final long ref, final int status)
		{
			this.data = data;
			this.nid = nid;
			this.ref = ref;
			this.status = status;
		}

		public final boolean ok()
		{
			return (this.status & 1) == 1;
		}

		@Override
		public final String toString()
		{
			final StringBuilder str = new StringBuilder(128).append(this.data).append(';');
			str.append(this.nid).append(';');
			str.append(this.ref).append("l;");
			return str.append(this.status).toString();
		}
	}

	@SuppressWarnings("rawtypes")
	public static final class TreeCall<T extends Descriptor> extends Shr.LibCall<T>
	{
		public TreeCall(final Class<T> rtype, final String name)
		{
			super(0, rtype, name);
		}

		public TreeCall(final int prop, final Class<T> rtype, final String name)
		{
			super(prop, rtype, name);
		}

		@Override
		protected final String getImage()
		{ return "TreeShr"; }
	}

	public static final int NO_STATUS = -1;

	public TreeShr(final Mds mds)
	{
		super(mds);
	}

	public final int _treeIsOpen(final CTX ctx) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "_TreeIsOpen")//
				.ctx(ctx.getDbid()).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * returns the absolute (full) path of the node
	 *
	 * @return StringStatus
	 **/
	public final StringStatus treeAbsPath(final CTX ctx, final String relpath) throws MdsException
	{
		final Request<List> request = new TreeCall<>(List.class, "TreeAbsPathDsc")//
				.ref(Descriptor.valueOf(relpath)).descr("x", "REPEAT(' ',512)").finL("x", "s");
		return new StringStatus(this.mds.getDescriptor(ctx, request));
	}

	/**
	 * adds a device node of specific model to a tree (EDIT)
	 *
	 * @return IntegerStatus
	 **/
	public final IntegerStatus treeAddConglom(final CTX ctx, final String path, final String model) throws MdsException
	{
		final Request<Int32Array> request = new TreeCall<>(Int32Array.class, "TreeAddConglom")//
				.ref(Descriptor.valueOf(path)).ref(Descriptor.valueOf(model)).ref("a", -1).finA("a", "s");
		return new IntegerStatus(this.mds.getDescriptor(ctx, request));
	}

	/**
	 * adds a node of specific usage to a tree (EDIT)
	 *
	 * @return IntegerStatus
	 **/
	public final IntegerStatus treeAddNode(final CTX ctx, final String path, final byte usage) throws MdsException
	{
		final Request<Int32Array> request = new TreeCall<>(Int32Array.class, "TreeAddNode")//
				.ref(Descriptor.valueOf(path)).ref("a", -1).val(usage).finA("a", "s");
		return new IntegerStatus(this.mds.getDescriptor(ctx, request));
	}

	/**
	 * adds a tag to a node
	 *
	 * @return int: status
	 **/
	public final int treeAddTag(final CTX ctx, final int nid, final String tag) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeAddTag")//
				.val(nid).ref(Descriptor.valueOf(tag)).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * initiates a new time-stamped segment
	 *
	 * @return int: status
	 **/
	public final int treeBeginTimestampedSegment(final CTX ctx, final int nid, final Descriptor_A<?> initialValue,
			final int idx) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class,
				"TreeBeginTimestampedSegment")//
						.val(nid).xd(initialValue).val(idx).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * cleans the data file by removing unreferenced data
	 *
	 * @return int: status
	 **/
	public final int treeCleanDatafile(final Null NULL, final String expt, final int shot) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeCleanDatafile")//
				.ref(Descriptor.valueOf(expt)).val(shot).fin();
		return this.mds.getDescriptor(null, request).getValue();
	}

	/**
	 * closes a tree
	 *
	 * @return int: status
	 **/
	public final int treeClose(final CTX ctx, final String expt, final int shot) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeClose")//
				.ref(Descriptor.valueOf(expt)).val(shot).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * cleans and compresses the data file of a tree
	 *
	 * @return int: status
	 **/
	public final int treeCompressDatafile(final Null NULL, final String expt, final int shot) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeCompressDatafile")//
				.ref(Descriptor.valueOf(expt)).val(shot).fin();
		return this.mds.getDescriptor(null, request).getValue();
	}

	/**
	 * creates a copy of a tree with a new shot number
	 *
	 * @return int: status
	 **/
	public final int treeCreateTreeFiles(final Null NULL, final String expt, final int newshot, final int fromshot)
			throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeCreateTreeFiles")//
				.ref(Descriptor.valueOf(expt)).val(newshot).val(fromshot).fin();
		return this.mds.getDescriptor(null, request).getValue();
	}

	/**
	 * returns the current tree context
	 *
	 * @return Pointer: context
	 **/
	public final Pointer treeCtx(final Null NULL) throws MdsException
	{
		final Request<Pointer> request = new TreeCall<>(Pointer.class, "TreeCtx:P")//
				.fin();
		return this.mds.getDescriptor(null, request);
	}

	/**
	 * returns the dbid pointer of the current context
	 *
	 * @return Pointer: current dbid
	 **/
	public Pointer treeDbid(final Null NULL) throws MdsException
	{
		final Request<Pointer> request = new TreeCall<>(Pointer.class, "TreeDbid:P")//
				.fin();
		return this.mds.getDescriptor(null, request);
	}

	/**
	 * executes the initiated delete operation
	 *
	 * @return int: status
	 **/
	public final int treeDeleteNodeExecute(final Pointer ctx) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeDeleteNodeExecute")//
				.fin();
		this.mds.getDescriptor(ctx, request).getValue();
		return MdsException.TreeSUCCESS;// treeDeleteNodeExecute returns void
	}

	/**
	 * @return IntegerStatus: nid-number of the next node in the list of nodes to be
	 *         deleted
	 **/
	public final IntegerStatus treeDeleteNodeGetNid(final CTX ctx, final int idx) throws MdsException
	{
		final Request<Int32Array> request = new TreeCall<>(Int32Array.class, "TreeDeleteNodeGetNid")//
				.ref("a", idx).finA("a", "s");
		return new IntegerStatus(this.mds.getDescriptor(ctx, request));
	}

	/**
	 * initiates a delete operation of a node and its descendants (primitive)
	 *
	 * @return IntegerStatus
	 **/
	public final IntegerStatus treeDeleteNodeInitialize(final CTX ctx, final int nid) throws MdsException
	{
		return this.treeDeleteNodeInitialize(ctx, nid, 0, true);
	}

	/**
	 * initiates a delete operation of a node and its descendants
	 *
	 * @return IntegerStatus
	 **/
	public final IntegerStatus treeDeleteNodeInitialize(final CTX ctx, final int nid, final int count,
			final boolean init) throws MdsException
	{
		final Request<Int32Array> request = new TreeCall<>(Int32Array.class, "TreeDeleteNodeInitialize")//
				.val(nid).ref("a", count).val(init ? 1 : 0).finA("a", "s");
		return new IntegerStatus(this.mds.getDescriptor(ctx, request));
	}

	public final DescriptorStatus treeDoMethod(final CTX ctx, final int nid, final String method,
			final Descriptor<?>... args) throws MdsException
	{
		final Request<List> request = new TreeCall<>(List.class, "TreeDoMethodA")//
				.val(nid).ref(Descriptor.valueOf(method)).val(args.length).ref(new List(args)).xd("x").finL("x", "s");
		return new DescriptorStatus(this.mds.getDescriptor(ctx, request));
	}

	/**
	 * finishes the added conglomerate
	 *
	 * @return IntegerStatus
	 **/
	public final int treeEndConglomerate(final Pointer ctx) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeEndConglomerate")//
				.fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	public final StringStatus treeFileName(final CTX ctx, final String expt, final int shot) throws MdsException
	{
		final Request<List> request = new TreeCall<>(List.class, "TreeFileName").ref(Descriptor.valueOf(expt))//
				.val(shot).xd("a").finL("a", "s");
		return new StringStatus(this.mds.getDescriptor(ctx, request));
	}

	/**
	 * searches for the next tag assigned to a node
	 *
	 * @return TagRef: next tag found
	 **/
	public final TagRef treeFindNodeTags(final CTX ctx, final int nid, final TagRef ref) throws MdsException
	{
		final Request<List> request = new TreeCall<>(List.class, "TreeFindNodeTagsDsc")//
				.val(nid).ref("a", ref.ref).descr("t", "REPEAT(' ',64)").fin("List(*,TRIM(__t),__a,__s)");
		return new TagRef(this.mds.getDescriptor(ctx, request));
	}

	/**
	 * searches for the next node meeting the criteria of a matching search string
	 * and usage mask
	 *
	 * @return NodeRefStatus: next nid-number found
	 **/
	public final NodeRefStatus treeFindNodeWild(final CTX ctx, final String searchstr, final int usage_mask,
			final NodeRefStatus ref) throws MdsException
	{
		final Request<List> request = new TreeCall<>(List.class, "TreeFindNodeWild")//
				.ref(Descriptor.valueOf(searchstr)).ref("a", -1).ref("q", ref.ref).val(usage_mask).finL("a", "q", "s");
		return new NodeRefStatus(this.mds.getDescriptor(ctx, request));
	}

	public final void treeFindTagEnd(final Null NULL, final TagRefStatus tag) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeFindTagEnd")//
				.ref(tag.ref).fin();
		this.mds.getDescriptor(null, request);
	}

	/**
	 * searches for the next tag in the tag list matching a search string
	 *
	 * @return TagRefStatus: next tag found
	 **/
	public final TagRefStatus treeFindTagWild(final CTX ctx, final String searchstr, final TagRefStatus ref)
			throws MdsException
	{
		final Request<List> request = new TreeCall<>(List.class, "TreeFindTagWildDsc")//
				.ref(Descriptor.valueOf(searchstr)).ref("i", -1).ref("q", ref.ref).xd("a").finL("a", "i", "q", "s");
		return new TagRefStatus(this.mds.getDescriptor(ctx, request));
	}

	public final int treeFreeDbid(final Pointer dbid) throws MdsException
	{
		if (dbid == null || dbid.getAddress() == 0)
			return 0;
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeFreeDbid").val(dbid)
				.fin();
		final int errno = this.mds.getDescriptor(null, request).getValue();
		dbid.setAddress(0);
		return errno;
	}

	/**
	 * resolves the current shot number (0)
	 *
	 * @return int: shot number
	 **/
	public final int treeGetCurrentShotId(final Null NULL, final String expt) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeGetCurrentShotId")//
				.ref(Descriptor.valueOf(expt)).fin();
		return this.mds.getDescriptor(null, request).getValue();
	}

	/**
	 * checks the size of the data file
	 *
	 * @return long: file size in byte
	 **/
	public final int treeGetDatafileSize(final Pointer ctx) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeGetDatafileSize")//
				.fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * checks the default node of active tree
	 *
	 * @return IntegerStatus: nid-number of default node
	 **/
	public final IntegerStatus treeGetDefaultNid(final Pointer ctx) throws MdsException
	{
		final Request<Int32Array> request = new TreeCall<>(Int32Array.class, "TreeGetDefaultNid")//
				.ref("a", -1).finA("a", "s");
		return new IntegerStatus(this.mds.getDescriptor(ctx, request));
	}

	/**
	 * returns the shortest path to node (from default node)
	 *
	 * @return String: minimal path to node
	 **/
	public final StringStatus treeGetMinimumPath(final CTX ctx, final int nid) throws MdsException
	{
		final Request<List> request = new TreeCall<>(List.class, "TreeGetMinimumPathDsc")//
				.val(0).val(nid).xd("x").finL("x", "s");
		return new StringStatus(this.mds.getDescriptor(ctx, request));
	}

	/**
	 * @return IntegerStatus: number of segments
	 **/
	public final IntegerStatus treeGetNumSegments(final CTX ctx, final int nid) throws MdsException
	{
		final Request<Int32Array> request = new TreeCall<>(Int32Array.class, "TreeGetNumSegments")//
				.val(nid).ref("a", 0).finA("a", "s");
		return new IntegerStatus(this.mds.getDescriptor(ctx, request));
	}

	/**
	 * returns the natural path of a node (form its parent tree root)
	 *
	 * @return String: path to node
	 **/
	public final StringStatus treeGetPath(final CTX ctx, final int nid) throws MdsException
	{
		final Request<List> request = new TreeCall<>(List.class, "TreeGetPathDsc")//
				.val(nid).xd("x").finL("x", "s");
		return new StringStatus(this.mds.getDescriptor(ctx, request));
	}

	/**
	 * reads the full record of a node
	 *
	 * @return DescriptorStatus: record of node
	 **/
	public final DescriptorStatus treeGetRecord(final CTX ctx, final int nid) throws MdsException
	{
		final Request<List> request = new TreeCall<>(List.class, "TreeGetRecord")//
				.val(nid).xd("x").finL("x", "s");
		return new DescriptorStatus(this.mds.getDescriptor(ctx, request));// compressed data cannot be put in a list
																			// without decompression
	}

	/**
	 * reads idx-th segment of a node
	 *
	 * @return SignalStatus: segment
	 **/
	public final Signal treeGetSegment(final CTX ctx, final int nid, final int idx) throws MdsException
	{
		final Request<Signal> request = new TreeCall<>(Signal.class, "TreeGetSegment")//
				.val(nid).val(idx).xd("a").xd("d").fin("make_signal(__a,*,__d)");
		return this.mds.getDescriptor(ctx, request);
	}

	public final Descriptor_A<?> treeGetSegmentData(final CTX ctx, final int nid, final int idx) throws MdsException
	{
		@SuppressWarnings("rawtypes")
		final Request<Descriptor> request = new TreeCall<>(Descriptor.class, "TreeGetSegment")//
				.val(nid).val(idx).xd("a").xd("d").finV("a");
		return this.mds.getDescriptor(ctx, request).getDataA();
	}

	public final Descriptor<?> treeGetSegmentDim(final CTX ctx, final int nid, final int idx) throws MdsException
	{
		@SuppressWarnings("rawtypes")
		final Request<Descriptor> request = new TreeCall<>(Descriptor.class, "TreeGetSegment")//
				.val(nid).val(idx).val(0).xd("a").finV("a");
		return this.mds.getDescriptor(ctx, request);
	}

	/**
	 * @return SegmentInfo: info about the idx-th segment of a node
	 **/
	public final SegmentInfo treeGetSegmentInfo(final CTX ctx, final int nid, final int idx) throws MdsException
	{
		final Request<List> request = new TreeCall<>(List.class, "TreeGetSegmentInfo")//
				.val(nid).val(idx).ref("a", "0B").ref("b", "0B").ref("d", "ZERO(8,0)").ref("i", 0)
				.finL("a", "b", "d", "i", "s");
		return new SegmentInfo(this.mds.getDescriptor(ctx, request));
	}

	/**
	 * @return DescriptorStatus: time limits of the idx-th segment of a node
	 **/
	public final DescriptorStatus treeGetSegmentLimits(final CTX ctx, final int nid, final int idx) throws MdsException
	{
		final Request<List> request = new TreeCall<>(List.class, "TreeGetSegmentLimits")//
				.val(nid).val(idx).xd("a").xd("b").fin("List(*,List(*,__a,__b),__s)");
		return new DescriptorStatus(this.mds.getDescriptor(ctx, request));
	}

	public final Descriptor<?> treeGetSegmentScale(final CTX ctx, final int nid) throws MdsException
	{
		@SuppressWarnings("rawtypes")
		final Request<Descriptor> request = new TreeCall<>(Descriptor.class, "TreeGetSegmentScale")//
				.val(nid).xd("a").finV("a");
		return this.mds.getDescriptor(ctx, request);
	}

	public final DescriptorStatus treeGetSegmentTimesXd(final CTX ctx, final int nid) throws MdsException
	{
		final Request<List> request = new TreeCall<>(List.class, "TreeGetSegmentTimesXd")//
				.val(nid).ref("a", "0").xd("b").xd("c").fin("List(*,List(*,__a,__b,__c),__s)");
		return new DescriptorStatus(this.mds.getDescriptor(ctx, request));
	}

	public DescriptorStatus treeGetTimeContext(final CTX ctx) throws MdsException
	{
		final Request<List> request = new TreeCall<>(List.class, "TreeGetTimeContext")//
				.ref("a", "*").xd("b").xd("c").xd("c").fin("List(*,List(*,__a,__b,__c),__s)");
		return new DescriptorStatus(this.mds.getDescriptor(ctx, request));
	}

	/**
	 * reads the list of attributes of a node
	 *
	 * @return int: status
	 **/
	public final DescriptorStatus treeGetXNci(final CTX ctx, final int nid) throws MdsException
	{
		return this.treeGetXNci(ctx, nid, "attributenames");
	}

	/**
	 * reads an extended attribute of a node
	 *
	 * @return int: status
	 **/
	public final DescriptorStatus treeGetXNci(final CTX ctx, final int nid, final String name) throws MdsException
	{
		final Request<List> request = new TreeCall<>(List.class, "TreeGetXNci")//
				.val(nid).ref(Descriptor.valueOf(name)).xd("a").finL("a", "s");
		return new DescriptorStatus(this.mds.getDescriptor(ctx, request));
	}

	/**
	 * checks the state of a node
	 *
	 * @return int: status
	 **/
	public final int treeIsOn(final CTX ctx, final int nid) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeIsOn")//
				.val(nid).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * adds a new segment to segmented node
	 *
	 * @return int: status
	 **/
	public final int treeMakeSegment(final CTX ctx, final int nid, final Descriptor<?> start, final Descriptor<?> end,
			final Descriptor<?> dimension, final Descriptor_A<?> values, final int idx, final int rows_filled)
			throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeMakeSegment")//
				.val(nid).xd(start).xd(end).xd(dimension).xd(values).val(idx).val(rows_filled).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * adds a new time-stamped segment to segmented node
	 *
	 * @return int: status
	 **/
	public final int treeMakeTimestampedSegment(final CTX ctx, final int nid, final Int64Array timestamps,
			final Descriptor_A<?> values, final int idx, final int rows_filled) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class,
				"TreeMakeTimestampedSegment")//
						.val(nid).ref(timestamps).xd(values).val(idx).val(rows_filled).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * adds a new time-stamped segment to segmented node
	 *
	 * @return int: status
	 **/
	public final int treeMakeTimestampedSegment(final CTX ctx, final int nid, final long[] dim,
			final Descriptor_A<?> values, final int idx, final int rows_filled) throws MdsException
	{
		return this.treeMakeTimestampedSegment(ctx, nid, new Int64Array(dim), values, idx, rows_filled);
	}

	/**
	 * open tree and in normal or read-only mode
	 *
	 * @return int: status
	 **/
	public final int treeOpen(final CTX ctx, final String expt, final int shot, final boolean readonly)
			throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeOpen")//
				.ref(Descriptor.valueOf(expt)).val(shot).val(readonly ? 1 : 0).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * open tree in edit mode
	 *
	 * @return int: status
	 **/
	public final int treeOpenEdit(final CTX ctx, final String expt, final int shot) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeOpenEdit")//
				.ref(Descriptor.valueOf(expt)).val(shot).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * create new tree and open it in edit mode
	 *
	 * @return int: status
	 **/
	public final int treeOpenNew(final CTX ctx, final String expt, final int shot) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeOpenNew")//
				.ref(Descriptor.valueOf(expt)).val(shot).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * sets the record of a node (primitive)
	 *
	 * @return int: status
	 **/
	public final int treePutRecord(final CTX ctx, final int nid, final Descriptor<?> dsc) throws MdsException
	{
		return this.treePutRecord(ctx, nid, dsc, 0);
	}

	/**
	 * sets the record of a node
	 *
	 * @param utility_update 2:compress !=0: no_write-flags are ignored and flags
	 *                       are reset to 0x5400
	 * @return int: status
	 **/
	public final int treePutRecord(final CTX ctx, final int nid, final Descriptor<?> dsc, final int utility_update)
			throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreePutRecord")//
				.val(nid).xd(dsc).val(utility_update).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * adds row to segmented node
	 *
	 * @return int: status
	 **/
	public final int treePutRow(final CTX ctx, final int nid, final int bufsize, final long timestamp,
			final Descriptor_A<?> data) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreePutRow")//
				.val(nid).val(bufsize).ref(timestamp).xd(data).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * adds a segment to a segmented node
	 *
	 * @return int: status
	 **/
	public final int treePutSegment(final CTX ctx, final int nid, final int rowidx, final Descriptor_A<?> data)
			throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreePutSegment")//
				.val(nid).val(rowidx).descr(data).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * adds a time-stamped segment to segmented node
	 *
	 * @return int: status
	 **/
	public final int treePutTimestampedSegment(final CTX ctx, final int nid, final long timestamp,
			final Descriptor_A<?> data) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class,
				"TreePutTimestampedSegment")//
						.val(nid).ref(timestamp).xd(data).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * exits a tree (EDIT)
	 *
	 * @return int: status
	 **/
	public final int treeQuitTree(final CTX ctx, final String expt, final int shot) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeQuitTree")//
				.ref(Descriptor.valueOf(expt)).val(shot).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * clears all tags assigned to a node (EDIT)
	 *
	 * @return int: status
	 **/
	public final int treeRemoveNodesTags(final CTX ctx, final int nid) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeRemoveNodesTags")//
				.val(nid).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * renames/moves a node (EDIT)
	 *
	 * @return int: status
	 **/
	public final int treeRenameNode(final CTX ctx, final int nid, final String name) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeRenameNode")//
				.val(nid).ref(Descriptor.valueOf(name)).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * restores a saved context
	 *
	 * @return int: status; 1 on success, context if input is null
	 **/
	public final int treeRestoreContext(final CTX ctx, final Pointer treectx) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeRestoreContext")//
				.val(treectx).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * save the current context
	 *
	 * @return Pointer: pointer to saved context
	 **/
	public final Pointer treeSaveContext(final Pointer ctx) throws MdsException
	{
		final Request<Pointer> request = new TreeCall<>(Pointer.class, "TreeSaveContext:P")//
				.fin();
		return this.mds.getDescriptor(ctx, request);
	}

	/**
	 * sets the current shot of an experiment
	 *
	 * @return int: status
	 **/
	public final int treeSetCurrentShotId(final CTX ctx, final String expt, final int shot) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeSetCurrentShotId")//
				.ref(Descriptor.valueOf(expt)).val(shot).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * sets the DbiItem of ctx to value
	 *
	 * @param ctx
	 * @param item  one of {@link TreeShr#DBI_MAX_OPEN},
	 *              {@link TreeShr#DBI_VERSIONS_IN_MODEL}, or
	 *              {@link TreeShr#DBI_VERSIONS_IN_PULSE}
	 * @param value
	 * @return
	 * @throws MdsException
	 */
	public final int treeSetDbiItm(final CTX ctx, final int item, final int value) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Int32.class, "TreeSetDbiItm")//
				.val(item).val(value).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	public final IntegerStatus treeSetDefault(final CTX ctx, final String path) throws MdsException
	{
		final Request<Int32Array> request = new TreeCall<>(Int32Array.class, "TreeSetDefault")//
				.ref(Descriptor.valueOf(path)).ref("a", -1).finA("a", "s");
		return new IntegerStatus(this.mds.getDescriptor(ctx, request));
	}

	/**
	 * makes a node the default node
	 *
	 * @return int: status
	 **/
	public final int treeSetDefaultNid(final CTX ctx, final int nid) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeSetDefaultNid")//
				.val(nid).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * sets or resets the flags selected by a mask
	 *
	 * @return int: status
	 **/
	public final int treeSetNciItm(final CTX ctx, final int nid, final boolean state, final int flags)
			throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeSetNciItm")//
				.val(nid).val(state ? 1 : 2).val(flags).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * removes subtree flag from a node (EDIT)
	 *
	 * @return int: status
	 **/
	public final int treeSetNoSubtree(final CTX ctx, final int nid) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeSetNoSubtree")//
				.val(nid).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	public final int treeSetSegmentScale(final CTX ctx, final int nid, final Descriptor<?> scale) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeSetSegmentScale")//
				.val(nid).xd(scale).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * adds subtree flag to a node (EDIT)
	 *
	 * @return int: status
	 **/
	public final int treeSetSubtree(final CTX ctx, final int nid) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeSetSubtree")//
				.val(nid).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * sets the current time context that will affect signal reads a a tree
	 *
	 * @return int: status
	 **/
	public final int treeSetTimeContext(final CTX ctx, final Number start, final Number end, final Number delta)
			throws MdsException
	{
		return this.treeSetTimeContext(ctx, Descriptor.valueOf(start), Descriptor.valueOf(end),
				Descriptor.valueOf(delta));
	}

	/**
	 * sets the current time context that will affect signal reads from a tree
	 *
	 * @return int: status
	 **/
	public final int treeSetTimeContext(final CTX ctx, final NUMBER<?> start, final NUMBER<?> end,
			final NUMBER<?> delta) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeSetTimeContext")//
				.descr(start).descr(end).descr(delta).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * clears current time context that will affect signal reads from a tree
	 *
	 * @return int: status
	 **/
	public final int treeSetTimeContext(final Pointer ctx) throws MdsException
	{
		return this.treeSetTimeContext(ctx, (NUMBER<?>) null, (NUMBER<?>) null, (NUMBER<?>) null);
	}

	/**
	 * sets/adds an extended attribute to a node
	 *
	 * @return int: status
	 **/
	public final int treeSetXNci(final CTX ctx, final int nid, final String name, final Descriptor<?> value)
			throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeSetXNci")//
				.val(nid).ref(Descriptor.valueOf(name)).xd(value).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * initiates a conglomerate of nodes similar to devices (EDIT)
	 *
	 * @return int: status
	 **/
	public final int treeStartConglomerate(final CTX ctx, final int size) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeStartConglomerate")//
				.val(size).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	public final Pointer treeSwitchDbid(final Null NULL, final Pointer ctx) throws MdsException
	{
		final Request<Pointer> request = new TreeCall<>(Pointer.class, "TreeSwitchDbid:P")//
				.val(ctx).fin();
		return this.mds.getDescriptor(null, request);
	}

	/**
	 * turns a node off (265392050 : TreeLock-Failure but does change the state)
	 *
	 * @return int: status
	 **/
	public final int treeTurnOff(final CTX ctx, final int nid) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeTurnOff")//
				.val(nid).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * turns a node on (265392050 : TreeLock-Failure but does change the state)
	 *
	 * @return int: status
	 **/
	public final int treeTurnOn(final CTX ctx, final int nid) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeTurnOn")//
				.val(nid).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	public final int treeUpdateSegment(final CTX ctx, final int nid, final Descriptor<?> start, final Descriptor<?> end,
			final Descriptor<?> dim, final int idx) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeUpdateSegment")//
				.val(nid).descr(start).descr(end).descr(dim).val(idx).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	public final int treeUpdateSegment(final CTX ctx, final int nid, final long start, final long end,
			final Descriptor<?> dim, final int idx) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeUpdateSegment")//
				.val(nid).ref(start).ref(end).ref(dim).val(idx).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * sets the current private context state
	 *
	 * @return boolean: previous state
	 **/
	public final boolean treeUsePrivateCtx(final boolean state) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeUsePrivateCtx")//
				.val(state ? 1 : 0).fin();
		return this.mds.getDescriptor(null, request).getValue() == 1;
	}

	/**
	 * checks for the current private context state
	 *
	 * @return boolean: true if private
	 **/
	public final boolean treeUsingPrivateCtx() throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeUsingPrivateCtx")//
				.fin();
		return this.mds.getDescriptor(null, request).getValue() == 1;
	}

	public final int treeVerify(final Pointer ctx) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeVerify")//
				.fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}

	/**
	 * writes changes to a tree (EDIT)
	 *
	 * @return int: status
	 **/
	public final int treeWriteTree(final CTX ctx, final String expt, final int shot) throws MdsException
	{
		final Request<Int32> request = new TreeCall<>(Request.PROP_ATOMIC_RESULT, Int32.class, "TreeWriteTree")//
				.ref(Descriptor.valueOf(expt)).val(shot).fin();
		return this.mds.getDescriptor(ctx, request).getValue();
	}
}

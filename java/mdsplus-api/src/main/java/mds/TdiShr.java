package mds;

import java.util.ArrayList;

import mds.Mds.Request;
import mds.data.CTX;
import mds.data.OPC;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_S;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_s.*;

public class TdiShr extends TreeShr
{
	@SuppressWarnings("rawtypes")
	public static final class TdiCall<T extends Descriptor> extends Shr.LibCall<T>
	{
		public TdiCall(final Class<T> rtype, final String name)
		{
			super(0, rtype, name);
		}

		@Override
		protected final String getImage()
		{ return "TdiShr"; }
	}

	public TdiShr(final Mds mds)
	{
		super(mds);
	}

	public final DescriptorStatus _tdiCompile(final CTX ctx, final String expr, final Descriptor<?>... args)
			throws MdsException
	{
		if (expr == null || expr.isEmpty())
			return new DescriptorStatus(Missing.NEW, MdsException.MDSplusSUCCESS);
		return this._tdiIntrinsic(ctx, OPC.OpcCompile, Descriptor.valueOf(expr), args);
	}

	public final DescriptorStatus _tdiDecompile(final CTX ctx, final Descriptor<?> dsc) throws MdsException
	{
		return this._tdiIntrinsic(ctx, OPC.OpcDecompile, dsc);
	}

	public final DescriptorStatus _tdiEvaluate(final CTX ctx, final Descriptor<?> dsc) throws MdsException
	{
		if (Descriptor.isMissing(dsc))
			return new DescriptorStatus(Missing.NEW, MdsException.MDSplusSUCCESS);
		return this._tdiIntrinsic(ctx, OPC.OpcEvaluate, dsc);
	}

	public final DescriptorStatus _tdiExecute(final CTX ctx, final String expr, final Descriptor<?>... args)
			throws MdsException
	{
		if (expr == null || expr.isEmpty())
			return new DescriptorStatus(Missing.NEW, MdsException.MDSplusSUCCESS);
		return this._tdiIntrinsic(ctx, OPC.OpcExecute, Descriptor.valueOf(expr), args);
	}

	public final DescriptorStatus _tdiIntrinsic(final CTX ctx, final OPC opcode, final Descriptor<?>... args)
			throws MdsException
	{
		final Request<List> request = new TdiCall<>(List.class, "_TdiIntrinsic").ctxp(ctx.getDbid())
				.val(opcode.ordinal())//
				.val(args.length).ref(new List(args)).xd("a").finL("a", "s", "c");
		return new DescriptorStatus(this.mds.getDescriptor(null, request));
	}

	public final DescriptorStatus _tdiIntrinsic(final CTX ctx, final OPC opcode, final Descriptor<?> arg0,
			final Descriptor<?>[] args1) throws MdsException
	{
		final Descriptor<?>[] args = new Descriptor<?>[args1.length + 1];
		args[0] = arg0;
		System.arraycopy(args1, 0, args, 1, args1.length);
		return this._tdiIntrinsic(ctx, opcode, args);
	}

	public final int[] getShotDB(final StringDsc expt, final Descriptor_S<?> path, final Int32 lower, final Int32 upper)
			throws MdsException
	{
		final ArrayList<Descriptor<?>> args = new ArrayList<>(4);
		final StringBuilder expr = new StringBuilder(63).append("getShotDB(($;)");
		args.add(expt);
		if (path != null)
		{
			args.add(path);
			expr.append(",($;)");
		}
		else
			expr.append(",*");
		if (lower != null)
		{
			args.add(lower);
			expr.append(",($;)");
		}
		else
			expr.append(",*");
		if (upper != null)
		{
			args.add(upper);
			expr.append(",($;)");
		}
		else
			expr.append(",*");
		return this.mds.getIntegerArray(expr.append(')').toString(), args.toArray(new Descriptor[args.size()]));
	}

	public final DescriptorStatus tdiCompile(final CTX ctx, final String expr, final Descriptor<?>... args)
			throws MdsException
	{
		if (expr == null || expr.isEmpty())
			return new DescriptorStatus(Missing.NEW, MdsException.MDSplusSUCCESS);
		return this.tdiIntrinsic(ctx, OPC.OpcCompile, Descriptor.valueOf(expr), args);
	}

	public final String tdiDecompile(final CTX ctx, final Descriptor<?> dsc) throws MdsException
	{
		return this.tdiIntrinsic(ctx, OPC.OpcDecompile, dsc).getData().toString();
	}

	public final String tdiDecompile(final CTX ctx, final String exec, final Descriptor<?>... dsc) throws MdsException
	{
		return this.tdiExecute(ctx, "Decompile(`(" + exec + ";))", dsc).getData().toString();
	}

	public final Descriptor<?> tdiDoTask(final CTX ctx, final Descriptor<?> task) throws MdsException
	{
		return this.tdiIntrinsic(ctx, OPC.OpcDoTask, task).getData();
	}

	public final DescriptorStatus tdiEvaluate(final CTX ctx, final Descriptor<?> dsc) throws MdsException
	{
		if (Descriptor.isMissing(dsc))
			return new DescriptorStatus(Missing.NEW, MdsException.MDSplusSUCCESS);
		return this.tdiIntrinsic(ctx, OPC.OpcEvaluate, dsc);
	}

	public final DescriptorStatus tdiExecute(final CTX ctx, final String expr, final Descriptor<?>... args)
			throws MdsException
	{
		if (expr == null || expr.isEmpty())
			return new DescriptorStatus(Missing.NEW, MdsException.MDSplusSUCCESS);
		return this.tdiIntrinsic(ctx, OPC.OpcExecute, Descriptor.valueOf(expr), args);
	}

	public final DescriptorStatus tdiIntrinsic(final CTX ctx, final OPC opcode, final Descriptor<?>... args)
			throws MdsException
	{
		if (ctx != null && this.mds.MdsEND_ARG() > 0)
			return this._tdiIntrinsic(ctx, opcode, args);
		final Request<List> request = new TdiCall<>(List.class, "TdiIntrinsic").val(opcode.ordinal())//
				.val(args.length).ref(new List(args)).xd("a").finL("a", "s");
		return new DescriptorStatus(this.mds.getDescriptor(ctx, request));
	}

	private final DescriptorStatus tdiIntrinsic(final CTX ctx, final OPC opcode, final Descriptor<?> arg0,
			final Descriptor<?>[] args1) throws MdsException
	{
		final Descriptor<?>[] args = new Descriptor<?>[args1.length + 1];
		args[0] = arg0;
		System.arraycopy(args1, 0, args, 1, args1.length);
		return this.tdiIntrinsic(ctx, opcode, args);
	}
}

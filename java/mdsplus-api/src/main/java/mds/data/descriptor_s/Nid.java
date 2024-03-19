package mds.data.descriptor_s;

import java.nio.ByteBuffer;

import mds.MdsException;
import mds.data.DTYPE;
import mds.data.TREE;
import mds.data.descriptor.Descriptor;

/** Nid 192 (-64) **/
public final class Nid extends NODE<Integer>
{
	public static final Nid[] getArrayOfNids(final int[] nid_nums)
	{
		return Nid.getArrayOfNids(nid_nums, TREE.getActiveTree());
	}

	public static final Nid[] getArrayOfNids(final int[] nid_nums, final TREE tree)
	{
		final Nid[] nids = new Nid[nid_nums.length];
		for (int i = 0; i < nids.length; i++)
			nids[i] = new Nid(nid_nums[i], tree);
		return nids;
	}

	/** prevents recursive cycles thru decompile **/
	private boolean recursive_deco = false;

	public Nid(final ByteBuffer b)
	{
		super(b);
	}

	public Nid(final int nid_number)
	{
		super(DTYPE.NID, INTEGER.toByteBuffer(nid_number));
	}

	public Nid(final int nid_number, final TREE tree)
	{
		this(nid_number);
		this.tree = tree;
	}

	public Nid(final Nid nid, final int relative)
	{
		this(nid.getAtomic().intValue() + relative);
		this.tree = nid.tree;
	}

	@Override
	public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		if (this.recursive_deco)
			return pout.append("<nid ").append(this.getAtomic()).append('>');
		this.recursive_deco = true;
		try
		{
			pout.append(this.getNciFullPath());
		}
		catch (final MdsException e)
		{
			this.decompile();
		}
		catch (final Exception e)
		{
			e.printStackTrace();
			pout.append(e.toString());
		}
		this.recursive_deco = false;
		return pout;
	}

	@Override
	public final Integer getAtomic()
	{
		return new Integer(this.p.getInt(0));
	}

	@Deprecated
	public String getFullPath() throws MdsException
	{
		return this.getNciFullPath();
	}

	@Override
	public Descriptor<?> getLocal_(final FLAG local)
	{
		FLAG.set(local, false);
		return this.getDataD().setLocal();
	}

	@Override
	public final int getNidNumber()
	{
		return this.getAtomic().intValue();
	}

	@Deprecated
	public int getValue()
	{
		return this.getNidNumber();
	}

	@Override
	public final Path toFullPath() throws MdsException
	{
		return new Path(this.getNciFullPath(), this.tree);
	}

	@Override
	public final Path toMinPath() throws MdsException
	{
		return new Path(this.getNciMinPath(), this.tree);
	}

	@Override
	public final Nid toNid()
	{
		return this;
	}

	@Override
	public final Path toPath() throws MdsException
	{
		return new Path(this.getNciPath(), this.tree);
	}
}

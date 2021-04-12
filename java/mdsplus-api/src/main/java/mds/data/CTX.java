package mds.data;

import mds.Mds;
import mds.data.descriptor_s.Pointer;

public interface CTX
{
	public Pointer getDbid();

	public Mds getMds();
}

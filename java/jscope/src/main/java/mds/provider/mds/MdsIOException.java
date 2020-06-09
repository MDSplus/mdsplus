package mds.provider.mds;

import java.io.IOException;

public class MdsIOException extends IOException
{
	private static final long serialVersionUID = 1L;

	MdsIOException(String message)
	{
		super(message);
	}
}

package debug;

public final class DEBUG
{
	public static final boolean A = (DEBUG.LV & 8) > 0;
	public static final boolean D = (DEBUG.LV & 1) > 0;
	public static final boolean E = (DEBUG.LV & 2) > 0;
	public static final boolean G = (DEBUG.LV & 16) > 0;
	// set to false to allow compiler to identify and eliminate unreachable code
	public static final int LV = 0;
	public static final boolean M = (DEBUG.LV & 4) > 0;
	public static final boolean ON = DEBUG.LV > 0;

	public static void printByteArray(final byte[] buf, final int byteblock, final int width, final int height,
			final int pages)
	{
		int i = 0;
		String s;
		final int max = buf.length < 0x10000 ? buf.length : 0x10000;
		System.out.println("---page break---");
		for (int f = 0; f < pages && i < max; f++)
		{
			for (int h = 0; h < height && i < max; h++)
			{
				s = ", ";
				for (int w = 0; w < width && i < max; w++)
				{
					for (int b = 0; b < byteblock && i < max; b++)
					{
						final StringBuilder sb = new StringBuilder();
						sb.append(Integer.toHexString(0xFF & buf[i]));
						if (sb.length() < 2)
							sb.insert(0, '0');
						s += sb.toString() + " ";
						i++;
					}
					s += ", ";
				}
				System.out.println(s);
			}
			if (i < buf.length)
				System.out.println(" ... " + i + "/" + buf.length);
			System.out.println("---page break---");
		}
	}

	public static void printDoubleArray(final double[] buf, final int width, final int height, final int pages)
	{
		int i = 0;
		String s;
		final int max = buf.length < 0x10000 ? buf.length : 0x10000;
		System.out.println("---page break---");
		for (int f = 0; f < pages && i < max; f++)
		{
			for (int h = 0; h < height && i < max; h++)
			{
				s = ", ";
				for (int w = 0; w < width && i < max; w++)
				{
					s += buf[i] + ", ";
					i++;
				}
				System.out.println(s);
			}
			if (i < buf.length)
				System.out.println(" ... " + i + "/" + buf.length);
			System.out.println("---page break---");
		}
	}

	public static void printFloatArray(final float[] buf, final int width, final int height, final int pages)
	{
		int i = 0;
		String s;
		final int max = buf.length < 0x10000 ? buf.length : 0x10000;
		System.out.println("---page break---");
		for (int f = 0; f < pages && i < max; f++)
		{
			for (int h = 0; h < height && i < max; h++)
			{
				s = ", ";
				for (int w = 0; w < width && i < max; w++)
				{
					s += buf[i] + ", ";
					i++;
				}
				System.out.println(s);
			}
			if (i < buf.length)
				System.out.println(" ... " + i + "/" + buf.length);
			System.out.println("---page break---");
		}
	}

	public static void printIntArray(final int[] buf, final int byteblock, final int width, final int height,
			final int pages)
	{
		int i = 0;
		String s;
		final int max = buf.length < 0x10000 ? buf.length : 0x10000;
		System.out.println("---page break---");
		for (int f = 0; f < pages && i < max; f++)
		{
			for (int h = 0; h < height && i < max; h++)
			{
				s = ", ";
				for (int w = 0; w < width && i < max; w++)
				{
					for (int b = 0; b < byteblock && i < max; b++)
					{
						final StringBuilder sb = new StringBuilder();
						sb.append(Integer.toHexString(0xFFFFFFFF & buf[i]));
						if (sb.length() < 8)
							sb.insert(0, '0');
						s += sb.toString() + " ";
						i++;
					}
					s += ", ";
				}
				System.out.println(s);
			}
			if (i < buf.length)
				System.out.println(" ... " + i + "/" + buf.length);
			System.out.println("---page break---");
		}
	}
}

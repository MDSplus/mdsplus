package debug;

public final class DEBUG
{
	// set to false to allow compiler to identify and eliminate unreachable code
	public static final boolean A = (DEBUG.LV & 8) > 0;
	public static final boolean D = (DEBUG.LV & 1) > 0;
	public static final boolean E = (DEBUG.LV & 2) > 0;
	public static final boolean G = (DEBUG.LV & 6) > 0;
	public static final boolean N = DEBUG.LV < 0;
	public static final int LV = 0; // Integer.MIN_VALUE;
	public static final boolean M = (DEBUG.LV & 4) > 0;
	public static final boolean ON = (DEBUG.LV & 127) > 0;

	public static void printByteArray(final byte[] buf, final int byteblock, final int width, final int height,
			final int pages)
	{
		int i = 0;
		String s;
		System.out.println("---page break---");
		for (int f = 0; f < pages && i < 0x10000; f++)
		{
			for (int h = 0; h < height && i < 0x10000; h++)
			{
				s = ", ";
				for (int w = 0; w < width && i < 0x10000; w++)
				{
					for (int b = 0; b < byteblock && i < 0x10000; b++)
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
			if (i == 0x10000)
				System.out.println(" ... " + i + "/" + buf.length);
			System.out.println("---page break---");
		}
	}

	public static void printDoubleArray(final double[] buf, final int width, final int height, final int pages)
	{
		int i = 0;
		String s;
		System.out.println("---page break---");
		for (int f = 0; f < pages && i < 0x10000; f++)
		{
			for (int h = 0; h < height && i < 0x10000; h++)
			{
				s = ", ";
				for (int w = 0; w < width && i < 0x10000; w++)
				{
					s += buf[i] + ", ";
					i++;
				}
				System.out.println(s);
			}
			if (i == 0x10000)
				System.out.println(" ... " + i + "/" + buf.length);
			System.out.println("---page break---");
		}
	}

	public static void printFloatArray(final float[] buf, final int width, final int height, final int pages)
	{
		int i = 0;
		String s;
		System.out.println("---page break---");
		for (int f = 0; f < pages && i < 0x10000; f++)
		{
			for (int h = 0; h < height && i < 0x10000; h++)
			{
				s = ", ";
				for (int w = 0; w < width && i < 0x10000; w++)
				{
					s += buf[i] + ", ";
					i++;
				}
				System.out.println(s);
			}
			if (i == 0x10000)
				System.out.println(" ... " + i + "/" + buf.length);
			System.out.println("---page break---");
		}
	}

	public static void printIntArray(final int[] buf, final int byteblock, final int width, final int height,
			final int pages)
	{
		int i = 0;
		String s;
		System.out.println("---page break---");
		for (int f = 0; f < pages && i < 0x10000; f++)
		{
			for (int h = 0; h < height && i < 0x10000; h++)
			{
				s = ", ";
				for (int w = 0; w < width && i < 0x10000; w++)
				{
					for (int b = 0; b < byteblock && i < 0x10000; b++)
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
			if (i == 0x10000)
				System.out.println(" ... " + i + "/" + buf.length);
			System.out.println("---page break---");
		}
	}
}

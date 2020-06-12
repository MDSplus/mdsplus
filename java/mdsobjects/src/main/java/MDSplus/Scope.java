package MDSplus;

public class Scope
{
	private static boolean loaded = false;
	private static java.lang.reflect.Method createWindow, removeAllSignals, showWindow, addSignal;
	Object window;

	public Scope(java.lang.String name, int x, int y, int width, int height)
	{
		try
		{
			if (!loaded)
			{
				final Class<?> cls = Class.forName("mds.jscope.CompositeWaveDisplay");
				createWindow = cls.getMethod("createWindow", String.class);
				showWindow = cls.getMethod("showWindow", int.class, int.class, int.class, int.class);
				removeAllSignals = cls.getMethod("removeAllSignals", int.class, String.class);
				addSignal = cls.getMethod("addSignal", float[].class, float[].class, int.class, int.class, String.class,
						String.class, boolean.class, int.class);
			}
			window = createWindow.invoke(null, name);
			showWindow.invoke(window, x, y, width, height);
		}
		catch (final Exception e)
		{
			window = e;
		}
		loaded = true;
	}

	public void plot(Data x, Data y, int row, int col, java.lang.String color) throws Exception
	{
		if (window instanceof Exception)
			throw (Exception) window;
		removeAllSignals.invoke(window, row, col);
		this.oplot(x, y, row, col, color);
	}

	public void oplot(Data x, Data y, int row, int col, java.lang.String color) throws Exception
	{
		addSignal.invoke(window, x.getFloatArray(), y.getFloatArray(), row, col, color, "", true, 0);
	}

	// Test
	public static void main(java.lang.String args[])
	{
		final Scope scope = new Scope("CACCA", 100, 100, 100, 100);
		final float x[] = new float[1000];
		final float y1[] = new float[1000];
		final float y2[] = new float[1000];
		for (int i = 0; i < 1000; i++)
		{
			x[i] = i;
			y1[i] = x[i] * x[i];
			y2[i] = 2 * y1[i];
		}
		try
		{
			scope.plot(new Float32Array(x), new Float32Array(y1), 1, 1, "black");
			scope.oplot(new Float32Array(x), new Float32Array(y2), 1, 1, "red");
		}
		catch (final Exception exc)
		{
			System.err.println(exc);
		}
	}
}

package mds;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

import mds.data.CONST_Test;
import mds.data.TREE_Test;
import mds.data.descriptor.*;
import mds.data.descriptor_r.Function;
import mds.data.descriptor_r.Function_Test;
import mds.mdsip.MdsIp;
import mds.mdsip.MdsIp.Provider;

@RunWith(Suite.class)
@SuiteClasses(
{ TREE_Test.class, CONST_Test.class, Mds_Test.class, TreeShr_Test.class, MdsShr_Test.class, Function_Test.class,
		Descriptor_S_Test.class, Descriptor_A_Test.class, Descriptor_CA_Test.class }) // ,
// Editors_Test.class})
public class AllTests
{
	private static boolean local = true;
	private static boolean mdsip = true;
	private static boolean use_local = false;
	private static final int port = 8000;
	private static final boolean remote_win = false;
	private static final boolean ssh = false;
	public static final String tree = "test";

	@SuppressWarnings("resource")
	public static Mds setUpBeforeClass() throws Exception
	{
		final boolean local_win = System.getProperty("os.name").startsWith("Win");
		final String host = System.getenv("MDSIP_SERVER");
		AllTests.local = AllTests.local || (!AllTests.mdsip) || host == null || host.length() == 0;
		final String treepath = (AllTests.local ? local_win : AllTests.remote_win) ? "C:\\Temp" : "/tmp";
		final String prefix = AllTests.local ? "local://" : (AllTests.ssh ? "ssh://" : null);
		final String command = AllTests.local ? "local" : (AllTests.ssh ? host : host + ":" + AllTests.port);
		final Mds mdslocal = Mds.getLocal();
		if (!AllTests.use_local && mdslocal != null)
			mdslocal.close();
		Mds mds = null;
		if (!AllTests.mdsip)
		{
			mds = new MdsIp();
		}
		else
		{
			final MdsIp tmds = MdsIp.sharedConnection(new Provider(prefix + command));
			if (tmds.isConnected())
				mds = tmds;
		}
		if (mds == null)
			throw new Exception("Could not connect to mdsip.");
		Function.setWindowsLineEnding(AllTests.local ? local_win : AllTests.remote_win);
		mds.getAPI().setenv(AllTests.tree + "_path", treepath);
		return mds;
	}

	public static void tearDownAfterClass(final Mds mds)
	{
		mds.close();
	}

	@SuppressWarnings("boxing")
	public static void testStatus(final int expected, final int actual)
	{
		if (actual != expected)
			throw new AssertionError(String.format("Return status mismatch, expected: %d '%s' actual: %d '%s'",
					expected, MdsException.getMdsMessage(expected), actual, MdsException.getMdsMessage(actual)));
	}
}

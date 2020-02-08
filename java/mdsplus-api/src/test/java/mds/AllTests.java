package mds;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;
import mds.data.CONST_Test;
import mds.data.TREE_Test;
import mds.data.descriptor.Descriptor_A_Test;
import mds.data.descriptor.Descriptor_CA_Test;
import mds.data.descriptor.Descriptor_S_Test;
import mds.data.descriptor_r.Function;
import mds.data.descriptor_r.Function_Test;
import mds.mdsip.MdsIp;
import mds.mdsip.MdsIp.Provider;

@RunWith(Suite.class)
@SuiteClasses({TREE_Test.class, CONST_Test.class, Mds_Test.class, TreeShr_Test.class, MdsShr_Test.class, Function_Test.class, Descriptor_S_Test.class, Descriptor_A_Test.class, Descriptor_CA_Test.class}) // , Editors_Test.class})
public class AllTests{
	private static boolean			local		= true;
	private static boolean			mdsip		= true;
	private static boolean			use_local	= false;
	private static final int		port		= 8000;
	private static final boolean	remote_win	= false;
	private static final boolean	ssh			= false;
	public static final String		tree		= "test";

	public static Mds setUpBeforeClass() throws Exception {
		final boolean local_win = System.getProperty("os.name").startsWith("Win");
		final String host = System.getenv("MDSIP_SERVER");
		final String user = System.getProperty("user.name");
		AllTests.local = AllTests.local || (!mdsip) || host == null || host.length()==0;
		final String treepath = (AllTests.local ? local_win : remote_win) ? "C:\\Temp" : "/tmp";
		final String hostinfo = AllTests.local ? "local://" : host;
		final boolean use_ssh = AllTests.local ? false : AllTests.ssh;
		final int use_port = use_ssh ? 22 : AllTests.port;
		Mds mdslocal = Mds.getLocal();
		if (!use_local && mdslocal!=null) mdslocal.close();
		Mds mds = null;
		if(!AllTests.mdsip){
			mds = new MdsIp();
		}else{
			MdsIp tmds = MdsIp.sharedConnection(new Provider(hostinfo, use_port, user, use_ssh));
			if(tmds.isConnected()) mds = tmds;
		}
		if(mds == null) throw new Exception("Could not connect to mdsip.");
		Function.setWindowsLineEnding(AllTests.local ? local_win : AllTests.remote_win);
		mds.getAPI().setenv(tree + "_path", treepath);
		return mds;
	}

	public static void tearDownAfterClass(final Mds mds) {
		mds.close();
	}

	public static void testStatus(final int expected, final int actual) {
		if(actual != expected) throw new AssertionError(String.format("Return status mismatch, expected: %d '%s' actual: %d '%s'", expected, MdsException.getMdsMessage(expected), actual, MdsException.getMdsMessage(actual)));
	}
}

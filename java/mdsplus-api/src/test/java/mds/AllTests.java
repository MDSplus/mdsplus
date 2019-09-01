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
import mds.mdslib.MdsLib;

@RunWith(Suite.class)
@SuiteClasses({TREE_Test.class, CONST_Test.class, Mds_Test.class, TreeShr_Test.class, MdsShr_Test.class, Function_Test.class, Descriptor_S_Test.class, Descriptor_A_Test.class, Descriptor_CA_Test.class}) // , Editors_Test.class})
public class AllTests{
	private static boolean			mdslib		= true;
	private static boolean			mdsip		= System.getenv("test_mdsip")==null ? false : Integer.parseInt(System.getenv("test_mdsip"))!=0;
	private static boolean			local		= true;
	private static final boolean		ssh		= false;
	private static final boolean		remote_win	= false;
	private static final boolean		local_win	= System.getProperty("os.name").startsWith("Win");
	private static final int		port		= 8000;
	private static final String		treepath	= (AllTests.local ? AllTests.local_win : AllTests.remote_win) ? "C:\\Temp" : "/tmp";
	private static final String		user		= System.getProperty("user.name");
	private static final String		host		= System.getenv("MDSIP_SERVER");
	private static final String		killcmd		= AllTests.local_win ? "taskkill /im mdsip.exe /F" : "kill -s 9 $pidof mdsip";
	public static final String		tree		= "test";
	static{// clean up test files
		if(!AllTests.mdslib) MdsLib.lib_loaded = "use disabled";
		if(AllTests.mdsip && AllTests.local) try{
			Runtime.getRuntime().exec(AllTests.killcmd).waitFor();
		}catch(final Exception e){
			System.err.println(e + ": " + e.getMessage());
		}
	}

	public static Mds setUpBeforeClass() throws Exception {
		final String hostinfo = AllTests.local ? "localhost" : AllTests.host;
		final boolean use_ssh = AllTests.local ? false :  AllTests.ssh;
		final int use_port = use_ssh ? 22 :  AllTests.port;
		final Provider provider = new Provider(hostinfo, use_port, AllTests.user, use_ssh);
		Mds mds = null;
		if(!AllTests.mdsip) mds = new MdsLib();
		else{
			MdsIp tmds = MdsIp.sharedConnection(provider);
			if(tmds.isConnected()) mds = tmds;
			else if(AllTests.local){
				System.out.println("Started new local mdsip server");
				final ProcessBuilder pb = new ProcessBuilder("mdsip", "-h", System.getenv("hostfile"), "-m", "-p", String.valueOf(AllTests.port)).inheritIO();// , "-P", "ssh"
				// final Map<String, String> env = pb.environment();
				// env.put(AllTests.tree + "_path", AllTests.treepath);
				pb.start();
				Thread.sleep(300);
				tmds = MdsIp.sharedConnection(provider);
				if(tmds.isConnected()) mds = tmds;
			}
		}
		if(mds == null) throw new Exception("Could not connect to mdsip.");
		Function.setWindowsLineEnding(AllTests.local ? AllTests.local_win : AllTests.remote_win);
		mds.getAPI().setenv(AllTests.tree + "_path", AllTests.treepath);
		mds.getAPI().setenv("MDSPLUS_DEFAULT_RESAMPLE_MODE=Closest");
		return mds;
	}

	public static void tearDownAfterClass(final Mds mds) {
		mds.close();
	}

	@SuppressWarnings("boxing")
	public static void testStatus(final int expected, final int actual) {
		if(actual != expected) throw new AssertionError(String.format("Return status mismatch, expected: %d '%s' actual: %d '%s'", expected, MdsException.getMdsMessage(expected), actual, MdsException.getMdsMessage(actual)));
	}
}

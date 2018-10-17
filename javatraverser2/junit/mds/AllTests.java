package mds;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;
import mds.data.CONST_Test;
import mds.data.TREE_Test;
import mds.data.descriptor.Descriptor_A_Test;
import mds.data.descriptor.Descriptor_S_Test;
import mds.data.descriptor_r.Function;
import mds.data.descriptor_r.Function_Test;
import mds.mdsip.MdsIp;
import mds.mdsip.MdsIp.Provider;
import mds.mdslib.MdsLib;

@RunWith(Suite.class)
@SuiteClasses({TREE_Test.class, CONST_Test.class, Mds_Test.class, TreeShr_Test.class, MdsShr_Test.class, Function_Test.class, Descriptor_S_Test.class, Descriptor_A_Test.class}) // , Editors_Test.class})
public class AllTests{
    private static boolean       local      = false;
    private static boolean       mdsip      = true;
    private static final boolean remote_win = false;
    private static final boolean local_win  = System.getProperty("os.name").startsWith("Win");
    // System.getenv("test_mdsip") == null ? true :
    // Integer.valueOf(System.getenv("test_mdsip")).intValue() != 0;
    public static final int      port       = AllTests.use_ssh ? 22 : 8000;
    public static final String   tree       = "test";
    public static final String   treepath   = (AllTests.local ? AllTests.local_win : AllTests.remote_win) ? "C:\\Temp" : "/tmp";
    private static final String  user       = AllTests.local ? "user" : "cloud";
    private static final boolean use_ssh    = false;
    private static final String  host       = AllTests.local ? "localhost" : "mds-data-1";                                      // "gate.rzg.mpg.de"; //
    private static final String  killcmd    = AllTests.local_win ? "taskkill /im mdsip.exe /F" : "kill -s 9 $pidof mdsip";
    static{// clean up test files
        if(AllTests.mdsip && AllTests.local){
            try{
                Runtime.getRuntime().exec(AllTests.killcmd).waitFor();
            }catch(final Exception e){
                System.err.println(e + ": " + e.getMessage());
            }
        }
    }

    public static Mds setUpBeforeClass() throws Exception {
        final Provider provider = new Provider(AllTests.host, AllTests.port, AllTests.user, AllTests.use_ssh);
        Mds mds = null;
        if(!AllTests.mdsip){
            mds = new MdsLib();
        }else{
            MdsIp tmds = MdsIp.sharedConnection(provider);
            if(tmds.isConnected()) mds = tmds;
            if(mds == null && AllTests.local){
                System.out.println("Started new local mdsip server");
                final ProcessBuilder pb = new ProcessBuilder("mdsip", "-h", System.getenv("userprofile") + "/mdsip.hosts", "-m", "-p", String.valueOf(AllTests.port)).inheritIO();// , "-P", "ssh"
                // final Map<String, String> env = pb.environment();
                // env.put(AllTests.tree + "_path", AllTests.treepath);
                pb.start();
                tmds = MdsIp.sharedConnection(provider);
                if(tmds.isConnected()) mds = tmds;
            }
        }
        if(mds == null) throw new Exception("Could not connect to mdsip.");
        Function.setWindowsLineEnding(AllTests.local ? AllTests.local_win : AllTests.remote_win);
        mds.getAPI().setenv(AllTests.tree + "_path", AllTests.treepath);
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

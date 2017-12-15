package mds;

import java.io.File;
import java.io.FilenameFilter;
import java.util.Map;
import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;
//import jtraverser.editor.Editors_Test;
import mds.data.CONST_Test;
import mds.data.TREE_Test;
import mds.data.descriptor.Descriptor_A_Test;
import mds.data.descriptor.Descriptor_S_Test;
import mds.data.descriptor_r.Function_Test;
import mds.mdsip.MdsIp;
import mds.mdsip.MdsIp.Provider;
import mds.mdslib.MdsLib;

@RunWith(Suite.class)
@SuiteClasses({TREE_Test.class, CONST_Test.class, Mds_Test.class, TreeShr_Test.class, MdsShr_Test.class, Function_Test.class, Descriptor_S_Test.class, Descriptor_A_Test.class})
public class AllTests{
    private static boolean      mdsip   = System.getenv("test_mdsip") == null ? true : Integer.valueOf(System.getenv("test_mdsip")).intValue() != 0;
    public static final int     port  = 4400;
    public static final String  tree  = "test";
    private static final String user  = System.getProperty("user.name"), password = null, host = "localhost";
    private static final String killcmd = System.getProperty("os.name").toLowerCase().indexOf("win") >= 0 ? "taskkill /im mdsip.exe /F" : "kill -s 9 $pidof mdsip";

    static{// clean up test files
        if(AllTests.mdsip){
            try{
                Runtime.getRuntime().exec(killcmd).waitFor();
            }catch(final Exception e){
                System.err.println(e + ": " + e.getMessage());
            }
        }
    }

    public static Mds setUpBeforeClass() throws Exception {
        final Provider provider = new Provider(AllTests.host, AllTests.port, AllTests.user, AllTests.password);
        if(!AllTests.mdsip) return new MdsLib();
        MdsIp mds = MdsIp.sharedConnection(provider);
        if(mds.isConnected()) return mds;
        System.out.println("Started new local mdsip server");
        final ProcessBuilder pb = new ProcessBuilder("mdsip", "-h", System.getenv("hostfile"), "-m", "-p", String.valueOf(AllTests.port)).inheritIO();// , "-P", "ssh"
        final Map<String, String> env = pb.environment();
        env.put(AllTests.tree + "_path", System.getenv(AllTests.tree + "_path"));
        pb.start();
        mds = MdsIp.sharedConnection(provider);
        if(mds.isConnected()) return mds;
        throw new Exception("Could not connect to mdsip.");
    }

    public static void tearDownAfterClass(final Mds mds) {
        mds.close();
    }
}

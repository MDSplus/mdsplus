package mds;

import java.io.File;
import java.io.FilenameFilter;
import java.util.Map;
import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;
import jtraverser.editor.Editors_Test;
import mds.data.CONST_Test;
import mds.data.TREE_Test;
import mds.data.descriptor.Descriptor_A_Test;
import mds.data.descriptor.Descriptor_S_Test;
import mds.data.descriptor_r.Function_Test;
import mds.mdsip.MdsIp;
import mds.mdsip.MdsIp.Provider;
import mds.mdslib.MdsLib;

@RunWith(Suite.class)
@SuiteClasses({TREE_Test.class, CONST_Test.class, Mds_Test.class, TreeShr_Test.class, MdsShr_Test.class, Function_Test.class, Descriptor_S_Test.class, Descriptor_A_Test.class, Editors_Test.class})
public class AllTests{
    private static boolean      local = true;
    private static boolean      mdsip = false;
    public static final int     port  = 8000;
    public static final String  tree  = "test";
    private static final String user  = AllTests.local ? "cloud" : "user", password = AllTests.local ? null : "user", host = AllTests.local ? "localhost" : "debian";
    static{// clean up test files
        if(AllTests.mdsip && AllTests.local){
            try{
                Runtime.getRuntime().exec("taskkill /im mdsip.exe /F").waitFor();
            }catch(final Exception e){
                System.err.println(e + ": " + e.getMessage());
            }
            final String paths = System.getenv(AllTests.tree + "_path").replace("~t", AllTests.tree);
            for(final String path : paths.split(";")){
                final File folder = new File(path);
                final String filesearchstring = String.format("%s_([0-9]+|model)\\..*", AllTests.tree);
                final File[] files = folder.listFiles(new FilenameFilter(){
                    @Override
                    public boolean accept(final File dir, final String name) {
                        return name.matches(filesearchstring);
                    }
                });
                for(final File file : files){
                    if(!file.delete()){
                        System.err.println("Can't remove " + file.getAbsolutePath());
                    }
                }
            }
        }
    }

    public static Mds setUpBeforeClass() throws Exception {
        final Provider provider = new Provider(AllTests.host, AllTests.port, AllTests.user, AllTests.password);
        if(!AllTests.mdsip) return new MdsLib();
        MdsIp mds = MdsIp.sharedConnection(provider);
        if(mds.isConnected()) return mds;
        if(AllTests.local){
            System.out.println("Started new local mdsip server");
            final ProcessBuilder pb = new ProcessBuilder("mdsip", "-h", System.getenv("userprofile") + "/mdsip.hosts", "-m", "-p", String.valueOf(AllTests.port)).inheritIO();// , "-P", "ssh"
            final Map<String, String> env = pb.environment();
            env.put(AllTests.tree + "_path", System.getenv("TMP"));
            pb.start();
            mds = MdsIp.sharedConnection(provider);
            if(mds.isConnected()) return mds;
        }
        throw new Exception("Could not connect to mdsip.");
    }

    public static void tearDownAfterClass(final Mds mds) {
        mds.close();
    }
}

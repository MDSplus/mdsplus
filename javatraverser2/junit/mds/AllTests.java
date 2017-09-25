/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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

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
package mds.mdslib;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.util.Arrays;
import debug.DEBUG;
import mds.Mds;
import mds.MdsException;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Pointer;

public final class MdsLib extends Mds{
    public static final String lib_loaded = MdsLib.loadLibraryFromJar("JavaMdsLib");

    public static native byte[] evaluate(String expr, byte[]... args) throws MdsException;

    private static final String loadLibraryFromJar(final String libname) {
        if(libname == null || libname.length() < 3) return "The libname has to be at least 3 characters long: " + libname;
        final boolean is64bit = System.getenv("ProgramFiles(x86)") != null;
        final boolean isWin = System.getProperty("os.name", "").startsWith("Win");
        final String suffix = isWin ? ".dll" : ".so";
        final String libfileroot = isWin ? libname : "lib" + libname;
        final String libfilename = libfileroot + suffix;
        final String libinjar = "/lib/" + (is64bit ? "amd64" : "x86") + '/' + libfilename;
        File libpath = new File(System.getProperty("user.dir"), libfilename);
        if(libpath.exists()) try{
            System.load(libpath.getAbsolutePath());
            return MdsLib.testLib(libname);
        }catch(final UnsatisfiedLinkError exc){/**/}
        try{
            System.loadLibrary(libname);
            return MdsLib.testLib(libname);
        }catch(final UnsatisfiedLinkError exc){/**/}
        try{
            final InputStream is = MdsLib.class.getResourceAsStream(libinjar);
            if(is == null) throw new FileNotFoundException("File '" + libinjar + "' was not found inside JAR: " + libinjar);
            try{
                libpath = new File(System.getProperty("java.io.tmpdir"), libfilename);
                if(libpath.exists()) try{
                    System.load(libpath.getAbsolutePath());
                    return MdsLib.testLib(libname);
                }catch(final UnsatisfiedLinkError exc){
                    System.err.println(exc.toString());
                    libpath = File.createTempFile(libfileroot, suffix);
                }
                else libpath.createNewFile();
                if(!libpath.exists()) throw new FileNotFoundException("Could not create file '" + libpath.getAbsolutePath() + "'.");
                libpath.deleteOnExit();
                final OutputStream os = new FileOutputStream(libpath);
                try{
                    int readBytes;
                    final byte[] buffer = new byte[1024];
                    while((readBytes = is.read(buffer)) != -1)
                        os.write(buffer, 0, readBytes);
                }finally{
                    os.close();
                }
            }finally{
                is.close();
            }
            try{
                System.loadLibrary(libpath.getPath());
                return MdsLib.testLib(libname);
            }catch(final UnsatisfiedLinkError e){
                return "Library is broken " + libname + ": " + e.toString();
            }
        }catch(final Exception e){
            return "Could not find library " + libname + ": " + e.toString();
        }
    }

    private static final String testLib(final String libname) {
        try{
            final MdsLib mdslib = new MdsLib();
            if(1 != mdslib.getInteger("1BU")) return "Library does not work properly.";
        }catch(final Exception e){
            return "Library is broken " + libname + ": " + e.toString();
        }
        return null;
    }

    @Override
    @SuppressWarnings({"rawtypes", "unchecked"})
    protected final <T extends Descriptor> T _getDescriptor(final Pointer ctx, final Request<T> request) throws MdsException {
        final byte[] buffer;
        synchronized(this){
            byte[] tmpctx = null;
            if(ctx != null) //
                tmpctx = MdsLib.evaluate("TreeShr->TreeSwitchDbid:P(val($))", ctx.serializeArray());
            final byte[][] args = new byte[request.args.length][];
            for(int i = 0; i < args.length; i++)
                args[i] = request.args[i].serializeArray();
            try{
                if(DEBUG.N) System.out.println(request.expr + ", " + Arrays.toString(request.args));
                buffer = MdsLib.evaluate(request.expr, args);
            }finally{
                if(ctx != null){
                    tmpctx = MdsLib.evaluate("TreeShr->TreeSwitchDbid:P(val($))", tmpctx);
                    ctx.setAddress(((ByteBuffer)ByteBuffer.wrap(tmpctx).position(8)).slice().order(Descriptor.BYTEORDER));
                }
            }
        }
        return (T)(buffer == null ? null : Mds.bufferToClass(ByteBuffer.wrap(buffer).order(Descriptor.BYTEORDER), request.cls).setLocal());
    }

    @Override
    public final boolean isLowLatency() {
        return true;
    }

    @Override
    public final String isReady() {
        return MdsLib.lib_loaded;
    }

    @Override
    protected final void mdsSetEvent(final String event, final int eventid) {
        System.err.print("mdsSetEvent not implemented in mds.mdslib.MdsLib.");
    }

    @Override
    public final String toString() {
        return "local";
    }
}

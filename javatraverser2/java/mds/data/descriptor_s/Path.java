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
package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DTYPE;
import mds.data.TREE;
import mds.data.descriptor.Descriptor;

public final class Path extends NODE<String>{
    public static final Descriptor<?> make(final String in) {
        return in == null ? Missing.NEW : new Path(in);
    }

    public Path(final ByteBuffer b){
        super(b);
    }

    public Path(final Descriptor<?>... arguments){
        super(DTYPE.PATH, arguments[0].getBuffer());
    }

    public Path(final String path){
        super(DTYPE.PATH, ByteBuffer.wrap(path.getBytes()).order(Descriptor.BYTEORDER));
    }

    public Path(final String path, final TREE tree){
        super(DTYPE.PATH, ByteBuffer.wrap(path.getBytes()).order(Descriptor.BYTEORDER));
        this.tree = tree;
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return pout.append(this.getAtomic());
    }

    @Override
    public final String getAtomic() {
        final byte[] buf = new byte[this.length()];
        this.getBuffer().get(buf);
        return new String(buf);
    }

    @Override
    public Descriptor<?> getLocal_() {
        return this.evaluate().setLocal();
    }

    @Override
    public final int getNidNumber() throws MdsException {
        return this.tree.getNci(this, "NID_NUMBER").toInt();
    }

    @Override
    public final Path toFullPath() throws MdsException {
        return new Path(this.getNciFullPath(), this.tree);
    }

    @Override
    public final Path toMinPath() throws MdsException {
        return new Path(this.getNciMinPath(), this.tree);
    }

    @Override
    public final Nid toNid() throws MdsException {
        return new Nid(this.getNciNidNumber(), this.tree);
    }

    @Override
    public final Path toPath() throws MdsException {
        return new Path(this.getNciPath(), this.tree);
    }

    @Override
    public final String toString() {
        return this.getAtomic();
    }
}

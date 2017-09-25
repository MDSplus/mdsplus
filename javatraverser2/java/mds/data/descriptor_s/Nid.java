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

/** Nid 192 (-64) **/
public final class Nid extends NODE<Integer>{
    public static final Nid[] getArrayOfNids(final int[] nid_nums) {
        return Nid.getArrayOfNids(nid_nums, TREE.getActiveTree());
    }

    public static final Nid[] getArrayOfNids(final int[] nid_nums, final TREE tree) {
        final Nid[] nids = new Nid[nid_nums.length];
        for(int i = 0; i < nids.length; i++)
            nids[i] = new Nid(nid_nums[i], tree);
        return nids;
    }
    /** prevents recursive cycles thru decompile **/
    private boolean recursive_deco = false;

    public Nid(final ByteBuffer b){
        super(b);
    }

    public Nid(final int nid_number){
        super(DTYPE.NID, INTEGER.toByteBuffer(nid_number));
    }

    public Nid(final int nid_number, final TREE tree){
        this(nid_number);
        this.tree = tree;
    }

    public Nid(final Nid nid, final int relative){
        this(nid.getAtomic().intValue() + relative);
        this.tree = nid.tree;
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        if(this.recursive_deco) return pout.append("<nid ").append(this.getAtomic()).append('>');
        this.recursive_deco = true;
        try{
            pout.append(this.getNciFullPath());
        }catch(final MdsException e){
            this.decompile();
        }catch(final Exception e){
            e.printStackTrace();
            pout.append(e.toString());
        }
        this.recursive_deco = false;
        return pout;
    }

    @Override
    public final Integer getAtomic() {
        return new Integer(this.p.getInt(0));
    }

    @Override
    public final int getNidNumber() {
        return this.getAtomic().intValue();
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
    public final Nid toNid() {
        return this;
    }

    @Override
    public final Path toPath() throws MdsException {
        return new Path(this.getNciPath(), this.tree);
    }
}
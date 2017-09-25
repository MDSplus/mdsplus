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
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_S;

public final class CString extends Descriptor_S<String> implements DATA<String>{
    private static final String addQuotes(final String str) {
        if(str.contains("\"")){
            if(str.contains("'")) return "\"" + str.replaceAll("\"", "\\\\\"") + "\"";
            return "'" + str + "'";
        }
        return "\"" + str + "\"";
    }

    public static final String decompile(final String str) {
        return CString.addQuotes(str).replaceAll("\\\\", "\\\\\\\\");
    }

    public static final Descriptor<?> make(final String in) {
        return in == null ? Missing.NEW : new CString(in);
    }

    public static byte[] pad(final int len, final byte[] value, final byte c) {
        final ByteBuffer bb = ByteBuffer.allocate(len).order(Descriptor.BYTEORDER);
        for(int i = len; i-- > value.length;)
            bb.put(c);
        return bb.put(value).array();
    }

    public static void putString(final ByteBuffer b, final String value) {
        b.put(//
                (b.remaining() < value.length() ? value.substring(0, b.remaining()) : value)//
                        .getBytes());
    }

    public CString(final byte[] array){
        super(DTYPE.T, ByteBuffer.wrap(array).order(Descriptor.BYTEORDER));
    }

    public CString(final ByteBuffer b){
        super(b);
    }

    public CString(final int len, final String value){
        this(CString.pad(len, value.getBytes(), (byte)' '));
    }

    public CString(final String value){
        this(value.getBytes());
    }

    @Override
    public final Missing abs() throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final Missing add(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        return pout.append(((mode & Descriptor.DECO_STR) == 0) ? CString.decompile(this.getAtomic()) : this.getAtomic());
    }

    @Override
    public final Missing divide(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final String getAtomic() {
        final byte[] buf = new byte[this.length()];
        this.getBuffer().get(buf);
        return new String(buf);
    }

    @Override
    public final DATA<String> getData_() {
        return this;
    }

    @Override
    public final CString getLocal_() {
        return this;
    }

    @Override
    public final byte getRank() {
        return -1;
    }

    public final String getValue() {
        return this.getAtomic();
    }

    @Override
    public final Missing inot() throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final boolean isLocal() {
        return true;
    }

    @Override
    public final Missing multiply(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final Missing neg() throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final Missing power(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        throw DATA.tdierror;
    }

    public final void setString(final String value) {
        final ByteBuffer tbuf = (ByteBuffer)((ByteBuffer)this.b.duplicate().position(this.pointer())).slice().limit(this.length());
        if(value.length() > this.length()) tbuf.put(value.substring(0, this.length()).getBytes());
        else tbuf.put(value.getBytes());
        while(tbuf.hasRemaining())
            tbuf.put((byte)32);// fill with spaces
    }

    @Override
    public final Missing shiftleft(final Descriptor<?> X) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final Missing shiftright(final Descriptor<?> X) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final Missing subtract(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final CString text() {
        return this;
    }
}

package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_S;

public final class StringDsc extends Descriptor_S<String> implements DATA<String>{
    public static final String addQuotes(final String str) {
	if(str.contains("\"")){
	    if(str.contains("'")) return "\"" + str.replaceAll("\"", "\\\\\"") + "\"";
	    return "'" + str + "'";
	}
	return "\"" + str + "\"";
    }

    public static final String decompile(final String str) {
	return StringDsc.addQuotes(str).replaceAll("\\\\", "\\\\\\\\");
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

    public StringDsc(final byte[] array){
	super(DTYPE.T, ByteBuffer.wrap(array).order(Descriptor.BYTEORDER));
    }

    public StringDsc(final ByteBuffer b){
	super(b);
    }

    public StringDsc(final int len, final String value){
	this(StringDsc.pad(len, value.getBytes(), (byte)' '));
    }

    public StringDsc(final String value){
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
	return pout.append(((mode & Descriptor.DECO_STR) == 0) ? StringDsc.decompile(this.getAtomic()) : this.getAtomic());
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
    public final StringDsc getLocal_(final FLAG local) {
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
    public final StringDsc text() {
	return this;
    }
}

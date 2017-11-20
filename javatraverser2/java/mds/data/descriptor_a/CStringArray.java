package mds.data.descriptor_a;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;
import mds.data.descriptor_s.CString;
import mds.data.descriptor_s.Missing;

public final class CStringArray extends Descriptor_A<String> implements DATA<String[]>{
    private static final ByteBuffer toBytes(final int length, final Object[] lines, final boolean prepend) {
        final ByteBuffer b = ByteBuffer.allocate(length * lines.length).order(Descriptor.BYTEORDER);
        for(final Object line : lines){
            final String str = line.toString();
            final int maxline = Math.max(str.length(), length);
            if(!prepend) b.put(str.getBytes(), 0, maxline);
            for(int i = maxline; i < length; i++)
                b.put((byte)32);
            if(prepend) b.put(str.getBytes(), 0, maxline);
        }
        return b;
    }

    private static final ByteBuffer toBytes(final String[] lines) {
        int max = 0;
        for(final String line : lines)
            if(max < line.length()) max = line.length();
        return CStringArray.toBytes(max, lines, false);
    }

    public CStringArray(final ByteBuffer b){
        super(b);
    }

    public CStringArray(final int length, final Object... lines){
        super(DTYPE.T, CStringArray.toBytes(length, lines, true), lines.length);
    }

    public CStringArray(final int shape[], final String... lines){
        super(DTYPE.T, CStringArray.toBytes(lines), shape);
    }

    public CStringArray(final String... lines){
        super(DTYPE.T, CStringArray.toBytes(lines), lines.length);
    }

    @Override
    public final Missing abs() throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public Descriptor<?> add(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    protected StringBuilder decompile(final StringBuilder pout, final String t) {
        return pout.append('\"').append(t).append('\"');
    }

    @Override
    public Descriptor<?> divide(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final DATA<String[]> getData_() throws MdsException {
        return this;
    }

    @Override
    public final String getElement(final ByteBuffer b_in) {
        final byte[] buf = new byte[this.length()];
        b_in.get(buf);
        return new String(buf).replaceAll(" +$", "");
    }

    @Override
    public final String getElement(final int i) {
        return this.getElement((ByteBuffer)this.p.duplicate().position(i * this.length()));
    }

    @Override
    public final CStringArray getLocal_(final FLAG local) {
        return this;
    }

    @Override
    public final byte getRank() {
        return -1;
    }

    @Override
    public CString getScalar(final int idx) {
        return new CString(this.getElement(idx));
    }

    @Override
    protected final String getSuffix() {
        return "";
    }

    @Override
    protected final String[] initArray(final int size) {
        return new String[size];
    }

    @Override
    public final Missing inot() throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final boolean isLocal() {
        return true;
    }

    public final String join_by(final char separator) {
        return this.join_by(String.valueOf(separator));
    }

    public final String join_by(final String separator) {
        final int length = this.getLength();
        if(length < 1) return "";
        final int len = (length - 1) * separator.length() + this.arsize();
        final ByteBuffer buf = this.getBuffer();
        final StringBuilder sb = new StringBuilder(len).append(this.getElement(buf));
        for(int i = 1; i < length; i++)
            sb.append(separator).append(this.getElement(buf));
        return sb.toString();
    }

    @Override
    public Descriptor<?> multiply(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final Missing neg() throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public Descriptor<?> power(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    protected final void setElement(final ByteBuffer b, final String value) {
        final int maxlength = this.length() < b.remaining() ? this.length() : b.remaining();
        b.put((maxlength < value.length() ? value.substring(0, maxlength) : value).getBytes());
    }

    @Override
    public final void setElement(final int i, final String value) {
        this.setElement((ByteBuffer)this.p.duplicate().position(i * this.length()), value);
    }

    @Override
    public Descriptor<?> shiftleft(final Descriptor<?> X) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public Descriptor<?> shiftright(final Descriptor<?> X) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public Descriptor<?> subtract(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException {
        throw DATA.tdierror;
    }

    @Override
    public final CStringArray text() {
        return this;
    }

    public final String[] toArray() {
        return this.getAtomic();
    }

    @Override
    public BigInteger toBigInteger(final String t) {
        return BigInteger.valueOf(Long.parseLong(t));
    }

    @Override
    public final byte toByte(final String t) {
        return Byte.parseByte(t);
    }

    @Override
    public final double toDouble(final String t) {
        return Double.parseDouble(t);
    }

    @Override
    public final float toFloat(final String t) {
        return Float.parseFloat(t);
    }

    @Override
    public final int toInt(final String t) {
        return Integer.parseInt(t);
    }

    @Override
    public final long toLong(final String t) {
        return Long.parseLong(t);
    }

    @Override
    public final short toShort(final String t) {
        return Short.parseShort(t);
    }

    @Override
    protected final String toString(final String t) {
        return t;
    }
}

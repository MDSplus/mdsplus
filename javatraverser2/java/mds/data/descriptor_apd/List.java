package mds.data.descriptor_apd;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_APD;

/** List (214 : -42) **/
public class List extends Descriptor_APD{
    public List(final ByteBuffer b){
        super(b);
    }

    public List(final Descriptor<?>... descs){
        super(DTYPE.LIST, descs);
    }

    public final Descriptor<?> get(final int idx) {
        return this.getScalar(idx);
    }

    public final Descriptor<?>[] toArray() {
        return this.getAtomic();
    }
}

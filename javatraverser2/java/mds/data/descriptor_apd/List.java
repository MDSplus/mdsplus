package mds.data.descriptor_apd;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_APD;

/** List (214 : -42) **/
public class List extends Descriptor_APD{
    public static String       list       = List.list_new;
    public static String       apdadd     = List.apdadd_new;
    public static final String list_new   = "LIST(*,";
    public static final String list_old   = "LIST(";
    public static final String apdadd_new = "_l=LIST(";
    public static final String apdadd_old = "apdadd(";

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

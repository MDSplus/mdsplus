package mds.data.descriptor_apd;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_APD;

/** Dictionary (216 : -40) **/
public class Dictionary extends Descriptor_APD{
    public Dictionary(final ByteBuffer b){
        super(b);
    }

    public Dictionary(final Descriptor<?>... descs){
        super(DTYPE.DICTIONARY, descs);
    }
}

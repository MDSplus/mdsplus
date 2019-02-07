package devices.acq4xx;

import java.awt.Frame;
import mds.data.descriptor_s.NODE;

public class ACQ425_2000_4CH extends ACQ425{
    public ACQ425_2000_4CH(final Frame frame, final NODE<?> node, final boolean editable, final int index){
        super(frame, node, editable, index, 4, 2000);
    }
}

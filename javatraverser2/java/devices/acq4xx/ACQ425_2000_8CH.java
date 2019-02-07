package devices.acq4xx;

import java.awt.Frame;
import mds.data.descriptor_s.NODE;

public class ACQ425_2000_8CH extends ACQ425{
    public ACQ425_2000_8CH(final Frame frame, final NODE<?> node, final boolean editable, final int index){
        super(frame, node, editable, index, 8, 2000);
    }
}

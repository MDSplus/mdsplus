package devices;

import java.awt.Frame;
import devices.acq4xx.ACQ1001;
import devices.acq4xx.ACQ480;
import mds.data.descriptor_s.NODE;

public class ACQ1001_ACQ480 extends ACQ1001{
    public ACQ1001_ACQ480(final Frame frame, final NODE<?> head, final boolean editable){
	super(frame, head, editable, ACQ480.class);
    }
}

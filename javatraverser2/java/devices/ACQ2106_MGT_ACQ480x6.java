package devices;

import java.awt.Frame;
import devices.acq4xx.ACQ2106;
import devices.acq4xx.ACQ480;
import mds.data.descriptor_s.NODE;

public class ACQ2106_MGT_ACQ480x6 extends ACQ2106{
    public ACQ2106_MGT_ACQ480x6(final Frame frame, final NODE<?> head, final boolean editable){
	super(frame, head, editable, 6, ACQ480.class);
    }
}

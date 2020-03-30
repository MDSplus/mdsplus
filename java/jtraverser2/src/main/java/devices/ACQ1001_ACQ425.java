package devices;

import java.awt.Frame;
import devices.acq4xx.ACQ1001;
import devices.acq4xx.ACQ425;
import mds.data.descriptor_s.NODE;

public class ACQ1001_ACQ425 extends ACQ1001{
	public ACQ1001_ACQ425(final Frame frame, final NODE<?> head, final boolean editable){
		super(frame, head, editable, ACQ425.ACQ425_16CH.class);
	}
}
 
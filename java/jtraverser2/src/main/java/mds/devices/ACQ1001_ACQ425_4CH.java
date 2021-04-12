package mds.devices;

import java.awt.Frame;

import mds.data.descriptor_s.NODE;
import mds.devices.acq4xx.ACQ1001;
import mds.devices.acq4xx.ACQ425;

public class ACQ1001_ACQ425_4CH extends ACQ1001
{
	public ACQ1001_ACQ425_4CH(final Frame frame, final NODE<?> head, final boolean editable)
	{
		super(frame, head, editable, ACQ425.ACQ425_4CH.class);
	}
}

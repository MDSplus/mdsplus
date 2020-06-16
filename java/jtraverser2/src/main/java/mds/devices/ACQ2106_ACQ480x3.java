package mds.devices;

import java.awt.Frame;

import mds.data.descriptor_s.NODE;
import mds.devices.acq4xx.ACQ2106;
import mds.devices.acq4xx.ACQ480;

public class ACQ2106_ACQ480x3 extends ACQ2106
{
	public ACQ2106_ACQ480x3(final Frame frame, final NODE<?> head, final boolean editable)
	{
		super(frame, head, editable, 3, ACQ480.class);
	}
}

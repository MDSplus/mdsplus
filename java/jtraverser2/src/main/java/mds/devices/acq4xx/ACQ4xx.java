package mds.devices.acq4xx;

import java.awt.Frame;
import java.awt.GridLayout;

import mds.data.descriptor_s.NODE;
import mds.devices.Device;

class ACQ4xx extends Device
{
	class Channel extends Device
	{
		protected Channel(final NODE<?> node, final int nargs)
		{
			super(ACQ4xx.this.frame, node, ACQ4xx.this.editable, nargs);
			this.pane.setLayout(new GridLayout(1, 0));
		}
	}

	class Master extends Device
	{
		protected Master(final NODE<?> node, final int nargs)
		{
			super(ACQ4xx.this.frame, node, ACQ4xx.this.editable, nargs);
			this.pane.setLayout(new GridLayout(1, 0));
		}
	}

	protected ACQ4xx(final Frame frame, final NODE<?> head, final boolean editable, final int nargs)
	{
		super(frame, head, editable, nargs);
	}

	@SuppressWarnings("static-method")
	public Master getMaster()
	{
		return null;
	}
}

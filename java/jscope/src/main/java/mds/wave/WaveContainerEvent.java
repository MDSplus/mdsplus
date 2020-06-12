package mds.wave;

/* $Id$ */
import java.awt.AWTEvent;

public class WaveContainerEvent extends AWTEvent
{
	private static final long serialVersionUID = 1L;
	public static final int START_UPDATE = AWTEvent.RESERVED_ID_MAX + 4;
	public static final int END_UPDATE = AWTEvent.RESERVED_ID_MAX + 5;
	public static final int KILL_UPDATE = AWTEvent.RESERVED_ID_MAX + 6;
	public static final int WAVEFORM_EVENT = AWTEvent.RESERVED_ID_MAX + 7;
	public String info;
	public AWTEvent we;

	public WaveContainerEvent(Object source, AWTEvent we)
	{
		super(source, WAVEFORM_EVENT);
		this.we = we;
	}

	public WaveContainerEvent(Object source, int event_id, String info)
	{
		super(source, event_id);
		this.info = info;
	}
}
